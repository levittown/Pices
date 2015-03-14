#if  !defined(__BIT_REDUCTION__)
#define __BIT_REDUCTION__

//*********************************************************************
//* Written by: Kevin Shallow                                         *
//* Updated by  Kurt Kramer                                           *
//*        For: Research Work, Plankton recognition System            *
//*-------------------------------------------------------------------*
//*  History                                                          *
//*                                                                   *
//*    date     Programmer   Description                              *
//*                                                                   *
//*********************************************************************

//#include  "svm.h"
#include "KKBaseTypes.h"
#include "SVMparam.h"
#include "ChainHash.h"
#include "FeatureVector.h"
#include "ClassAssignments.h"


namespace MLL 
{
  struct CompressionStats
  {
    KKStr   bitReductionByFeature;
    float   compression_ratio;
    double  elapsed_cpu_time;
    kkint32 num_images_after;
    kkint32 num_images_before;
    KKStr   unBalFeatures;

    CompressionStats ();
    CompressionStats (const CompressionStats&  cs);
    CompressionStats (KKStr  s);
    CompressionStats&  operator=  (const CompressionStats& right);
    CompressionStats&  operator+= (const CompressionStats& right);
    CompressionStats&  operator/= (kkint32 divisor);
    KKStr  ToTabDelString ()  const;
    void    UpdateFromTabDelStr (const KKStr&  _str);


    const KKStr&   UnBalFeatures ()  const  {return  unBalFeatures;}
  };


  ostream&  operator<< (ostream&                 o,
                        const CompressionStats&  stats
                       );


  /**
   * @brief This class is responsible for performing tBit reduction compression of FeatureVector lists.
   * @details See
   */
  class  BitReduction
  {
  public:
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::ulong   ulong;

    BitReduction (const SVMparam&        _param,
                  FileDescPtr            _fileDesc,
                  const FeatureNumList&  _selectedFeatures,
                  RunLog&                _log 

                 );

    ~BitReduction ();

    float  CompressOneFeature (VectorInt32&  featureData,
                               kkint32       bits
                              );

    /**
     * @brief Performs compression of 'examples' using bit reduction.
     * @param[in] examples List of feature vectors that are to be compressed.
     * @param[out] compressed_example_list  The feature vector list container to hold compressed examples.
     * @param[in] classAssignments  A list of classes and how they are to be grouped.
     * @returns The compression statistics.
     */
    CompressionStats  compress (const FeatureVectorList&  examples,
                                FeatureVectorListPtr      compressed_example_list,
                                const ClassAssignments&   classAssignments 
                               ); 

    void  DeleteHistogram ();

    vector< vector <KKStr > >*  TakeExampleGroups ()  
      {
        vector< vector <KKStr > >*  x = exampleGroups;
        exampleGroups = NULL;
        return x;
      };

    void  output_nominal_counts ();

    void  ReportBitreductionByFeature (ostream&  o);


  private:
    struct  FeatureVar;
    typedef  FeatureVar*  FeatureVarPtr;

    class  FeatureVarList;
    typedef  FeatureVarList*  FeatureVarListPtr;

    class  FeatureVarComparrison;


    void  add (      FeatureVectorPtr a, 
               const FeatureVectorPtr b
              );


    KKStr  BitReductionByFeature ();

    VectorInt32  DeriveBitReductionForEachFeatureByVariance
                 (const FeatureVectorList&  examples,
                  const ClassAssignments&   classAssignments,
                        KKStr&             unBalFeatures
                 );

    VectorInt32  DeriveBitReductionPlain (const FeatureVectorList&  examples,
                                          const ClassAssignments&   classAssignments
                                         );

    VectorInt32  DeriveBitReductionForEachFeatureFromSpecifoedParameter ();


    void divide (FeatureVectorPtr example, 
                 double           t 
                );

    void  DestroyNominalCounters   ();
    void  HistogramFeatures        (const FeatureVectorList&  examples);
    void  HistogramSave            (const KKStr&  fileName);
    void  IncrementNominalCounters (kkint32 feature_num, kkint32 value);
    kkint32 NominalFeatureAverage    (kkint32 feature_num);
    void  SetupNominalCounters     ();
    void  ZeroNominalCounters      ();

    kkint32                     bitsToReduceBy;
    VectorInt32                 bitsToReduceByFeature;
    VectorInt32                 cardinality_table;
    SVM_EncodingMethod          encodingMethod;
    const FileDescPtr           fileDesc;
    kkint32                     hashSize;
    kkint32**                     histogram;
    kkint32*                      histogramBucketStart;
    kkint32                     histogramNumOfBuckets;
    kkint32                     histogramZeroIdx;
    vector< vector <KKStr > >*  exampleGroups;
    kkint32                     nodeLength;
    kkint32**                     nominal_counters;
    SVMparam                    param;
    RunLog&                     log;
    FeatureNumList              selectedFeatures;
    vector<MLL::AttributeType>  type_table;
  };


}  /* MLL */



#endif
