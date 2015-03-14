#include  "FirstIncludes.h"

#include <stdio.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
using namespace std;

#include "MemoryDebug.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;

#include "MLLTypes.h"
#include "BitReduction.h"
#include "FileDesc.h"
#include "FeatureVector.h"
using namespace  MLL;

BitReduction::BitReduction (const SVMparam&        _param,
                            const FileDescPtr      _fileDesc,
                            const FeatureNumList&  _selectedFeatures,
                            RunLog&                _log
                           ):

  bitsToReduceBy        (_param.BitsToReduceBy ()),
  bitsToReduceByFeature (),
  cardinality_table     (),
  encodingMethod        (NoEncoding),
  fileDesc              (_fileDesc),
  hashSize              (0),
  histogram             (NULL),
  histogramBucketStart  (NULL),
  histogramNumOfBuckets (0),
  histogramZeroIdx      (0),
  exampleGroups         (NULL),
  nodeLength            (0),
  nominal_counters      (NULL),
  param                 (_param),
  log                   (_log),
  selectedFeatures      (_selectedFeatures),
  type_table            ()
{
  nominal_counters = NULL;
  encodingMethod   = _param.EncodingMethod ();
}



BitReduction::~BitReduction()
{
  DestroyNominalCounters ();
  DeleteHistogram ();

  delete  histogramBucketStart;
  histogramBucketStart = NULL;

  delete  exampleGroups;
}



void  BitReduction::DeleteHistogram ()
{
  if  (histogram)
  {
    for  (kkuint32 x = 0;  x < fileDesc->NumOfFields ();  x++)
    {
      delete histogram[x];
      histogram[x] = NULL;
    }

    histogram = NULL;
  }
}  /* DeleteHistogram */



void  BitReduction::HistogramFeatures (const FeatureVectorList&  examples)
{
  DeleteHistogram ();

  histogram             = new kkint32*[fileDesc->NumOfFields ()];
  histogramNumOfBuckets = 128;
  histogramZeroIdx      = kkint32 (histogramNumOfBuckets / 2);

  kkint32  histogramRange = 6000;
  kkint32  bucketSize          = (histogramRange / histogramNumOfBuckets);
  kkint32  histogramRangeStart = 0 - kkint32 (histogramRange / 2) + kkint32 (bucketSize / 2);

  kkint32  x, y;
  for  (x = 0;  x < (kkint32)fileDesc->NumOfFields ();  x++)
  {
    histogram[x] = new kkint32[histogramNumOfBuckets];
    for  (y = 0;  y < histogramNumOfBuckets; y++)
      histogram[x][y] = 0;
  }

  histogramBucketStart = new kkint32 [histogramNumOfBuckets];
  for  (x = 0;  x < histogramNumOfBuckets;  x++)
    histogramBucketStart[x] = (x * bucketSize) + histogramRangeStart;

  FeatureVectorList::const_iterator idx;

  kkuint32  temp;

  for  (idx = examples.begin ();  idx !=  examples.end ();  idx++)
  {
    const FeatureVectorPtr i = *idx;
    const FVFloat*  featureData = i->FeatureData();
    for (kkint32 fn = 0;  fn < examples.NumOfFeatures ();  fn++)
    {
      temp = kkuint32 (FVFloat (1000.0) * featureData[fn]);

      // We are assuming 97% will fall with in range of   (-3000 <-> +3000)  that is 3 std deviations
      kkint32  bucketIDX = kkint32 ((temp - histogramRangeStart) / bucketSize);

      if  (bucketIDX < 0)
        bucketIDX = 0;

      if  (bucketIDX >= histogramNumOfBuckets)
        bucketIDX = histogramNumOfBuckets - 1;

      histogram[fn][bucketIDX]++;
    }
  }
}  /* HistogramFeatures */




void  BitReduction::HistogramSave (const KKStr&  fileName)
{
  if  (!histogram)
    return;

  ofstream  o (fileName.Str ());

  kkint32  fn = 0;
  kkint32  x  = 0;


  o << "FN";
  for  (x = 0;  x < histogramNumOfBuckets;  x++)
    o << "\t" << histogramBucketStart[x];
  
  o << endl;


  for  (fn = 0;  fn < (kkint32)fileDesc->NumOfFields ();  fn++)
  {
    o << fn;

    for  (x = 0;  x < histogramNumOfBuckets;  x++)
      o << "\t" << histogram[fn][x];
    o << endl;
  }

  o.close ();
}  /* HistogramSave */




CompressionStats  BitReduction::compress (const FeatureVectorList&  examples,
                                          FeatureVectorListPtr      compressed_example_list,
                                          const ClassAssignments&   classAssignments 
                                         )
{
  {
    KKStr  histogramFileName = osRemoveExtension (examples.FileName ()) + "_Histogram.txt";
    HistogramFeatures (examples);
    HistogramSave (histogramFileName);
  }

  double time_before, time_after;

  FileDescPtr file_desc = examples.FileDesc ();

  kkint32  numSelFeatures   = selectedFeatures.NumOfFeatures ();
  kkint32  featursPerExample = file_desc->NumOfFields ();

  time_before = osGetSystemTimeUsed();

  CompressionStats stats;
  //memset(&stats, 0, sizeof(stats));  // Could potentially destroy a vat table, depending on implementation of c++

  // store information about the examples we're going to be processing
  type_table        = examples.FileDesc()->CreateAttributeTypeTable ();
  cardinality_table = examples.FileDesc()->CreateCardinalityTable ();

  //kkint32 unit_length = numSelFeatures + 1; // +1 for -1 index value
  nodeLength      = numSelFeatures;
  hashSize        = kkint32(examples.QueueSize() * 1.5);

  ChainHashNode::setLength           (nodeLength);
  ChainHashNode::setHashSize         (hashSize);
  //ChainHashNode::init                ();
  ChainHashNode::SetSelectedFeatures (selectedFeatures);
  ChainHashNode::SetFeatureTypes     (type_table);

  KKStr  unBalFeatureStr;


 if  (param.CompressionMethod () == BRunBalancedVariance)
    bitsToReduceByFeature = DeriveBitReductionForEachFeatureByVariance  (examples, classAssignments, unBalFeatureStr);

  else if  (param.CompressionMethod () == BRunBalancedSpecified)
    bitsToReduceByFeature = DeriveBitReductionForEachFeatureFromSpecifoedParameter ();

  else
    bitsToReduceByFeature = DeriveBitReductionPlain (examples, classAssignments);

  ChainHashNode::SetBitsToReduceByFeature (bitsToReduceByFeature);

  ChainHashNode notFound (0, 0, 0);

  // build the hash table
  ChainHash<ChainHashNode> ch (notFound, hashSize);
  FeatureVectorList::const_iterator  i;
  FeatureVectorPtr example;
  kkint32 example_index = 0;
  // kkint32 class_label = 0;
  bool success = false;  // indicates if example was successfully inserted into hash table

  example_index = 0;

  MLClassConstPtr  lastMLClass = NULL;
  kkint32             lastClassNum   = -1;

  for  (i = examples.begin ();  i != examples.end ();  ++i)
  {
    example = *i;
    if  (example->MLClass () != lastMLClass)
    {
      lastMLClass = example->MLClass ();
      lastClassNum   = classAssignments.GetNumForClass (lastMLClass);
    }

    ChainHashNodePtr  node 
      = new ChainHashNode(example_index++, 
                          lastClassNum, 
                          example,
                          success
                         );
    ch.insert (node);
  }

  ch.DoSomeStats ();

  // exampleGroups = ch.GroupImages ();


  // setup the nominal counters
  SetupNominalCounters ();

  // walk the hash table and build the newly compressed example list
  vector< list<kkint32> >&                  bucketList  = ch.getBucketList();
  vector< list<kkint32> >::const_iterator   it;
  list<kkint32>::const_iterator             it2;

  //kkint32  num_buckets = bucketList.size();
  kkint32  size        = 1;

  // each entry in the bucketList contains a list of examples that need to be merged
  for  (it = bucketList.begin(); it != bucketList.end(); it++)
  {
    // create a new example using the feature values from the first example in the current bucket
    size = 1;
    it2 = it->begin();
    //FVFloat *crap = (*features_list)[*it2].FeatureData ( );
    FeatureVectorPtr example = new FeatureVector (featursPerExample);
    
    
    //FeatureVectorPtr srcExample = features_list->IdxToPtr (*it2);
    FeatureVectorPtr srcExample = examples.IdxToPtr (*it2);


    example->MLClass (srcExample->MLClass ());
    add (example, srcExample);
    it2++;

    // now add in the rest the features (cumulatively)
    for(; it2 != it->end(); it2++)
    {
      //srcExample = features_list->IdxToPtr (*it2);
      srcExample = examples.IdxToPtr (*it2);
      add (example, srcExample);
      size++;
    }

    divide (example, size);
    example->TrainWeight ((float)size);
    ZeroNominalCounters ();

    compressed_example_list->PushOnBack (example);
  }

  //stats.num_images_before = features_list->size();
  stats.num_images_before = (long)examples.size();

  stats.num_images_after  = (long)compressed_example_list->size();
  stats.compression_ratio = (float)stats.num_images_after / (float)stats.num_images_before;

  // free the memory used for the nominal counters
  DestroyNominalCounters ();

  time_after = osGetSystemTimeUsed();
  stats.elapsed_cpu_time = time_after - time_before;

  stats.bitReductionByFeature = BitReductionByFeature ();
  stats.unBalFeatures         = unBalFeatureStr;

  return stats;
}  /* compress */





//effects: divided each feature of example by t
void BitReduction::divide (FeatureVectorPtr example, double t)
{
  FVFloat*  feature_data = example->FeatureDataAlter ();
  for (kkint32 i=0;  i < example->NumOfFeatures();  i++)
  {
    if (((type_table[i] == NominalAttribute) || (type_table[i] == SymbolicAttribute))  &&  (encodingMethod != NoEncoding))
    {
      feature_data[i] = (FVFloat)NominalFeatureAverage (i);
    }
    else 
    {
      feature_data[i] /= (FVFloat)t;
    }
  }
}  /* divide */




// effects: adds the feature values of b to the feature values of a
void BitReduction::add (      FeatureVectorPtr a, 
                        const FeatureVectorPtr b
                       )
{
  FVFloat*        feature_data_a = a->FeatureDataAlter ();
  const FVFloat*  feature_data_b = b->FeatureData      ();
  
  for (kkint32 i=0; i < a->NumOfFeatures(); i++)
  {
    if ((
          (type_table[i] == NominalAttribute)  ||  (type_table[i] == SymbolicAttribute)
        )  &&  
        (encodingMethod != NoEncoding)
       )
    {
      IncrementNominalCounters (i, (kkint32)feature_data_b[i]);
    }
    else
    {
      feature_data_a[i] += feature_data_b[i];
    }
  }
}  /* Add */





void BitReduction::ZeroNominalCounters ()
{
  if (!nominal_counters)
    return;

  for (kkuint32 i = 0;  i < fileDesc->NumOfFields ();  i++)
  {
    if  ((type_table[i] != NominalAttribute)  &&  (type_table[i] != SymbolicAttribute))
      continue;

    for (kkint32 j = 0;  j < cardinality_table[i];  j++)
    {
      nominal_counters[i][j] = 0;
    }
  }
}  /* ZeroNominalCounters */




void BitReduction::DestroyNominalCounters ()
{
  if (!nominal_counters)
    return;

  for (kkuint32 i = 0;  i < fileDesc->NumOfFields ();  i++)
  {
    if ((type_table[i] == NominalAttribute)  ||  (type_table[i] == SymbolicAttribute))
    {
      delete [] nominal_counters[i];
    }
    nominal_counters[i] = NULL;
  }

  delete [] nominal_counters;
  nominal_counters = NULL;
}




void BitReduction::SetupNominalCounters ()
{
  // destroy the existing counters, if they exist
  DestroyNominalCounters ();

  // create the new counters
  nominal_counters = new kkint32 *[fileDesc->NumOfFields ()];
  for (kkuint32 i = 0; i < fileDesc->NumOfFields (); i++)
  {
    if  ((type_table[i] == NominalAttribute)  ||  (type_table[i] == SymbolicAttribute))
      nominal_counters[i] = new kkint32[cardinality_table[i]];
    else
      nominal_counters[i] = NULL;
  }

  // set all of the counters to zero
  ZeroNominalCounters ();
}




void BitReduction::IncrementNominalCounters (kkint32 feature_num,
                                             kkint32 value
                                            )
{
  if (feature_num >= (kkint32)fileDesc->NumOfFields ())
  {
    return;
  }

  if  ((value < 0)  ||  (value >= fileDesc->Cardinality (feature_num, log)))
  {
    cout << "Unable to increment the nominal counter: Bad value" << endl;
    osWaitForEnter ();
    exit(-1);
  }

  nominal_counters[feature_num][value]++;
}  /* IncrementNominalCounters */




kkint32 BitReduction::NominalFeatureAverage (kkint32 feature_num)
{
  kkint32 max = -1;
  kkint32 max_index = -1;

  if (feature_num >= (kkint32)fileDesc->NumOfFields () || (feature_num < 0))
  {
    return max;
  }

  for (kkint32 j = 0;  j < fileDesc->Cardinality (feature_num, log);  j++)
  {
    if (nominal_counters[feature_num][j] > max)
    {
       max = nominal_counters[feature_num][j];
       max_index = j;
    }
  }

  return max_index;
}





void BitReduction::output_nominal_counts ()
{
  for (kkuint32 i = 0;  i < fileDesc->NumOfFields ();  i++)
  {
    cout << "Feature " << i << ": ";

    if  ((fileDesc->Type (i) != NominalAttribute)  &&  (fileDesc->Type (i) != SymbolicAttribute))
    {
      cout << "not nominal" << endl;
      continue;
    }

    for (kkint32 j=0;  j < fileDesc->Cardinality (i, log);  j++)
    {
      cout << nominal_counters[i][j] << " ";
    }
    cout << endl;
  }
}



ostream&  operator<< (ostream&                 o,
                      const CompressionStats&  stats
                     )
{
  o << "Before[" << stats.num_images_before << "] After[" << stats.num_images_after << "] Ratio[" << stats.compression_ratio << "]";
  return  o;
}  /* operator<< */




CompressionStats::CompressionStats ()
{
  num_images_before  = num_images_after = 0;
  elapsed_cpu_time   = 0.0;
  compression_ratio  = 0.0f;
  unBalFeatures      = "";
}


CompressionStats::CompressionStats (const CompressionStats&  cs):
  bitReductionByFeature (cs.bitReductionByFeature),
  compression_ratio     (cs.compression_ratio),
  elapsed_cpu_time      (cs.elapsed_cpu_time),
  num_images_after      (cs.num_images_after),
  num_images_before     (cs.num_images_before),
  unBalFeatures         (cs.unBalFeatures)
{}



CompressionStats::CompressionStats (KKStr  s)
{
  UpdateFromTabDelStr (s);
  compression_ratio = (float)num_images_after / (float)num_images_before;
}  




CompressionStats&  CompressionStats::operator=  (const CompressionStats& right)
{
  num_images_before = right.num_images_before;
  num_images_after  = right.num_images_after;
  elapsed_cpu_time  = right.elapsed_cpu_time;
  compression_ratio = (float)num_images_after / (float)num_images_before;

  if  (unBalFeatures.Empty ())
    unBalFeatures = right.unBalFeatures;
  else if  (unBalFeatures.FirstChar () == '(')
    unBalFeatures << ",(" << right.unBalFeatures << ")";
  else
    unBalFeatures << "(" << right.unBalFeatures << ")";

  return  *this;
}  /* operator= */



CompressionStats&  CompressionStats::operator+= (const CompressionStats& right)
{
  num_images_before += right.num_images_before;
  num_images_after  += right.num_images_after;
  elapsed_cpu_time  += right.elapsed_cpu_time;
  compression_ratio = (float)num_images_after / (float)num_images_before;

  if  (unBalFeatures.Empty ())
    unBalFeatures = right.unBalFeatures;
  else if  (unBalFeatures.FirstChar () == '(')
    unBalFeatures << ",(" << right.unBalFeatures << ")";
  else
    unBalFeatures << "(" << right.unBalFeatures << ")";

  return  *this;
}  /* operator+= */



CompressionStats&  CompressionStats::operator/= (kkint32  divisor)
{
  if  (divisor == 0)
  {
    num_images_before = 0;
    num_images_after  = 0;
    elapsed_cpu_time  = 0;
    compression_ratio = 0;
  }
  else
  {
    num_images_before /= divisor;
    num_images_after  /= divisor;
    elapsed_cpu_time  /= divisor;
    compression_ratio = (float)num_images_after / (float)num_images_before;
  }

  return  *this;
}  /* operator/= */




KKStr  CompressionStats::ToTabDelString ()  const
{
  KKStr  s (100);
   
  s << "ImagesBefore"          << "\t" << num_images_before     << "\t"
    << "ImagesAfter"           << "\t" << num_images_after      << "\t"
    << "ElapsedCpuTime"        << "\t" << elapsed_cpu_time      << "\t"
    << "BitReductionByFeature" << "\t" << bitReductionByFeature << "\t"
    << "UnBalFeatures"         << "\t" << unBalFeatures;

  return  s;
}


void  CompressionStats::UpdateFromTabDelStr (const KKStr&  _str)
{
  KKStr  str (_str);

  while  (!str.Empty ())
  {
    KKStr  field = str.ExtractQuotedStr ("\n\r\t", true);
    field.Upper ();
    KKStr  value = str.ExtractQuotedStr ("\n\r\t", true);

    if  (field == "IMAGESBEFORE")
      num_images_before = atoi (value.Str ());

    else if  (field == "IMAGESAFTER")
      num_images_after = atoi (value.Str ());

    else if  (field == "ELAPSEDCPUTIME")
      elapsed_cpu_time = atof (value.Str ());

    else if  (field == "BITREDUCTIONBYFEATURE")
      bitReductionByFeature = value;

    else if  (field == "UNBALFEATURES")
      unBalFeatures = value;
  }
}  /* UpdateFromTabDelStr */



struct  BitReduction::FeatureVar
{
  FeatureVar (kkint32        _featureNum,
              AttributeType  _attributeType,
              kkint32        _idx,
              double         _var
             ):
          attributeType (_attributeType),
          featureNum    (_featureNum), 
          idx           (_idx),
          var           (_var)
        {}

    MLL::AttributeType  attributeType;
    kkint32             featureNum;
    kkint32             idx;
    double              var;
};



// typedef  BitReduction::FeatureVar*  FeatureVarPtr;

class  BitReduction::FeatureVarList: public KKQueue<BitReduction::FeatureVar>
{
public:
  FeatureVarList (bool _owner):
     KKQueue<FeatureVar> (_owner)
     {}

  ~FeatureVarList () 
  {}
};





class  BitReduction::FeatureVarComparrison
{
public:
  FeatureVarComparrison ()  {};

  kkint32  AttributeValue (AttributeType  attributeType)
  {
    if  (attributeType == NumericAttribute)
      return 0;
    
    else if  (attributeType == OrdinalAttribute)
      return 1;

    else if  (attributeType == NominalAttribute)
      return 2;

    else if  (attributeType == SymbolicAttribute)
      return 3;

    else
      return 9;
  }
    
    
  bool  operator () (FeatureVarPtr  p1,
                     FeatureVarPtr  p2
                    )
  {
    kkint32 p1AttrVal = AttributeValue (p1->attributeType);
    kkint32 p2AttrVal = AttributeValue (p2->attributeType);

    if  (p1AttrVal < p2AttrVal)
      return true;

    if  (p1AttrVal > p2AttrVal)
      return false;

    else
      return  (p1->var < p2->var);
  }
};  /* FeatureVarComparrison */



/**
 * @brief Uses one 'BitsToReduceBy' value for all features.
 * @param[in] Not Used
 * @param[in] Not Used
 */
VectorInt32  BitReduction::DeriveBitReductionPlain (const FeatureVectorList&  examples,
                                                    const ClassAssignments&   classAssignments
                                                   )
{
  kkint32 numOfFeatures = selectedFeatures.NumOfFeatures ();
  VectorInt32  bitsToReduceBy (numOfFeatures, param.BitsToReduceBy ());
  return  bitsToReduceBy;
}
                                                      


VectorInt32  BitReduction::DeriveBitReductionForEachFeatureFromSpecifoedParameter ()
{
  kkint32 numSelFeatures = selectedFeatures.NumOfFeatures ();
  VectorInt32  bitsToReduceByFeature (numSelFeatures, param.BitsToReduceBy ());

  KKStr  s = param.UnBalancedBitsStr ();

  while  (!s.Empty ())
  {
    kkint32  featureNum = s.ExtractTokenInt (",");
    if  ((featureNum < 0)  ||  (featureNum >= numSelFeatures))
    {
      cerr << endl << endl
           << "BitReduction::DeriveBitReductionForEachFeatureFromSpecifoedParameter     ***WARNING***" << endl
           << endl
           << "*** Invalid Unbalanced Feature Specified[" << featureNum << "]" << endl
           << endl;
    }
    else
    {
      bitsToReduceByFeature[featureNum]++;
    }
  }

  return  bitsToReduceByFeature;
}  /* DeriveBitReductionForEachFeatureFromSpecifoedParameter */



/**
 @brief  Determines how many bits to reduce each feature by.
 @details
 @code
 **************************************************************************
 *  Determines how many bits to reduce each feature by.  It is meant to   *
 *  be called when BRunBalancedVariance is specified in SvmParam.  The    *
 *  features with the lowest variance are to be reduced by one extra      *
 *  bit.  The vector returned specifies the number of bits by feature to  *
 *  reduce by.                                                            *
 *                                                                        *
 *  The index to the returned vector will be with respect to the selected *
 *  features.  Ex: selectdFeaturs = {0,2,5,7,8},                          *
 *                 Returned Vector  {7,7,8,8,7}                           *
 **************************************************************************
 @endcode
 */
VectorInt32  BitReduction::DeriveBitReductionForEachFeatureByVariance (const FeatureVectorList&  examples,
                                                                       const ClassAssignments&   classAssignments,
                                                                             KKStr&              unBalFeatures
                                                                    )
{
  kkint32 idx;
  kkint32 numSelFeatures = selectedFeatures.NumOfFeatures ();

  FeatureVarList  featureVariances (true);

  vector<kkint32>     totals    (numSelFeatures, 0);
  vector<double>  means     (numSelFeatures, 0.0);
  vector<double>  sqrTotals (numSelFeatures, 0.0);

  kkint32  numBitsToReduceBy = param.BitsToReduceBy ();
  //kkint32  numBitsToReduceBy = 8;

  FeatureVectorList::const_iterator imageIDX;
  for  (imageIDX = examples.begin ();  imageIDX != examples.end ();  imageIDX++)
  {
    FeatureVectorPtr  example = *imageIDX;

    const FVFloat*  featureData = example->FeatureData ();

    for  (idx = 0;  idx < numSelFeatures;  idx++)
    {
      kkint32  featureNum = selectedFeatures[idx];
    
      kkint32 featureDataInt = kkint32 (featureData[featureNum] * 100000.0);
      if  (featureDataInt > 0)
        featureDataInt = featureDataInt >> numBitsToReduceBy;
      else
        featureDataInt = -1 * ((-1 * featureDataInt) >> numBitsToReduceBy);

      totals[idx] += featureDataInt;
    }
  }

  for  (idx = 0;  idx < numSelFeatures;  idx++)
  {
    means[idx] = double (totals[idx]) / double (examples.QueueSize ());
  }


  for  (imageIDX = examples.begin ();  imageIDX != examples.end ();  imageIDX++)
  {
    FeatureVectorPtr  example = *imageIDX;

    const FVFloat*  featureData = example->FeatureData ();

    for  (idx = 0;  idx < numSelFeatures;  idx++)
    {
      kkint32  featureNum = selectedFeatures[idx];

      kkint32 featureDataInt = kkint32 (featureData[featureNum] * 100000.0);
      if  (featureDataInt > 0)
        featureDataInt = featureDataInt >> numBitsToReduceBy;
      else
        featureDataInt = -1 * ((-1 * featureDataInt) >> numBitsToReduceBy);

      double  delta = means[idx] - double (featureDataInt);
      sqrTotals[idx] += delta * delta;
    }
  }


  cout << endl << "Variances:";
  for  (idx = 0;  idx < numSelFeatures;  idx++)
  {
    kkint32  featureNum = selectedFeatures[idx];
    double  var = sqrTotals[idx] / double (examples.QueueSize ());
    featureVariances.PushOnBack (new FeatureVar (featureNum, type_table[featureNum], idx, var));
    cout << ", " << var;
  }
  cout << endl;


  FeatureVarComparrison  comp;
  sort (featureVariances.begin (),  featureVariances.end (), comp);

  VectorInt32  bitsToReduceByFeature (numSelFeatures, param.BitsToReduceBy ());
  kkint32  numToReduceByOneMore = kkint32 (param.UnBalancedBits ());

  unBalFeatures = "";

  cout << endl << endl << "UnBAl Bit Order:";
  for (kkint32 x = 0;  x < numToReduceByOneMore; x++)
  {
    //kkint32 z = numSelFeatures - (x + 1);
    kkint32 z = x;  // reduce features that have smallest var by 1 extra bit.
    idx = featureVariances[z].idx;
    bitsToReduceByFeature[idx]++;
    cout << ", " << idx;

    if  (x > 0)
      unBalFeatures << ",";
    unBalFeatures << featureVariances[z].featureNum;
  }
  cout << endl;

  return  bitsToReduceByFeature;
} /* DeriveBitReductionForEachFeatureByVariance */





KKStr  BitReduction::BitReductionByFeature ()
{
  KKStr  o (100);

  kkint32  x;

  for  (x = 0;  x < selectedFeatures.NumOfFeatures ();  x++)
  {
    if  (x > 0)
      o << ", ";
    o << selectedFeatures[x] << "(" << bitsToReduceByFeature[x] << ")";
  }

  return  o;
}  /* BitReductionByFeature */





float  BitReduction::CompressOneFeature (VectorInt32&  featureData,
                                         kkint32       bits
                                        )
{
  // kak,  2006-06-03
  // This is a function that is not being used yet,  it is meant to help with some
  // bit reduction experiments.

  kkint32  numBits = 13 - bits;
  kkuint32 numOfBuckets = kkint32 (pow (double (2), double (bits)));
  kkint32  lastBucketId = numOfBuckets - 1;
  kkint32  halfOfTheBuckets = kkint32 (numOfBuckets / 2);
  //kkint32 divisor = kkint32 (pow (2, numBits));
  kkuint32  x;

  VectorInt32  buckets (numOfBuckets, 0);

  for  (x = 0;  x < featureData.size ();  x++)
  {
    kkint32  bucketIdx = (featureData[x] >> numBits) + halfOfTheBuckets;
    if  (bucketIdx < 0)
      bucketIdx = 0;

    if  (bucketIdx > lastBucketId)
      bucketIdx = lastBucketId;

    buckets[bucketIdx]++;
  }

  kkint32  numBucketsWithData = 0;
  for  (x = 0;  x < numOfBuckets;  x++)
  {
    if  (buckets[x] > 0)
      numBucketsWithData++;
  }

  return  float (numBucketsWithData) / float (numOfBuckets);
  }  /* CompressOneFeature */
