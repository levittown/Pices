#include "FirstIncludes.h"
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "MemoryDebug.h"
#include "KKBaseTypes.h"

using namespace  std;


#include "OSservices.h"
#include "StatisticalFunctions.h"
#include "KKStr.h"
using namespace  KKB;


#include "FeatureFileIO.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
#include "InstrumentDataFileManager.h"
#include "PicesVariables.h"
#include "PicesFVProducer.h"
using namespace  MLL;


#include "SpatialAnalysis.h"


// -src D:\TrainingApp\ExtractedImages\hrs020501_man     -r  c:\Temp\hrs020501_man_SpatialAnalysis.txt
// -src D:\ManuallyClassifiedSipperFiles\HRS0207_05_man  -r c:\Temp\HRS0207_05_man.txt  -QS 1.0

// -src C:\Pices\ExtractedImages\hrs020501_man  -WS 100 -BC 100 -Iterations 100 -DSR 21000 -DFR 0.8  -WS 100 -BC 100 -Iterations 100 -DSR 21000 -DFR 0.8


// -s C:\Pices\ExtractedImages\hrs020501_man   -i 500 �defaultscanrate 21000 -defaultflowrate 0.67 -ws 1 -bc 100 -r c:\paper_classified\hrs0207_01_man -ci 95


// -s C:\temp\HRS0207_01_man -i 10 -defaultscanrate 21000 -defaultflowrate 0.67 -ws 10 -bc 100 -ci 95

SpatialAnalysis::SpatialAnalysis ():

  PicesApplication (),
  bucketCount          (10),
  quadratSize          (1.0f),
  cancelFlag           (false),
  confidenceInterval   (95.0f),
  data                 (NULL),
  defaultFlowRate      (1.0f),
  defaultScanRate      (24950.0f),
  instrumentData       (NULL),
  lowerConfIntervalIdx (0),
  numOfIterations      (20),
  r                    (NULL),
  sipperFile           (NULL),
  sipperFileName       (""),
  sourceRootDirPath    (""),
  statType             (StatType::Davis92),
  transactLength       (0.0),
  upperConfIntervalIdx (0),
  windowSize           (1.0f)

{
}



SpatialAnalysis::~SpatialAnalysis ()
{
  delete  r;
  r = NULL;

  delete  data;
  data = NULL;

  InstrumentDataFileManager::InitializePop ();
}



void  SpatialAnalysis::InitalizeApplication (kkint32 argc,
                                             char**  argv
                                            )
{
  log.Level (10) << "SpatialAnalysis::InitalizeApplication" << endl;

  PicesApplication::InitalizeApplication (argc, argv);
	if  (Abort ())
	{
		DisplayCommandLineParameters ();
		return;
  }

  InstrumentDataFileManager::InitializePush ();

  if  (sourceRootDirPath.Empty ())
  {
    log.Level (-1) << endl 
      << endl 
      << "No Source Directory Specified."
      << endl;
    Abort (true);
    return;
  }

  if  (reportFileName.Empty ())
  {

    reportFileName = osAddSlash (sourceRootDirPath ) + 
                     osGetRootNameOfDirectory (sourceRootDirPath) +
                     "_SpatialAnalysis_"                          + 
                     StatTypeToStr (statType)                     + "_";

    if  (statType == StatType::Davis92)
    {
      int  windowSizeInt = (int)(floor (windowSize));
      int  windowSizeFtp = (int)(floor (windowSize * 100.0f + 0.5f)) % 100;

      reportFileName << StrFormatInt (windowSizeInt, "Z000")   << "-" 
                     << StrFormatInt (windowSizeFtp, "ZZZ0")   << "_"
                     << StrFormatInt (bucketCount,   "Z000");
    }
    else
    {
      int  quadratSizeInt = (int)(floor (quadratSize));
      int  quadratSizeFtp = (int)(floor (quadratSize * 100.0f + 0.5f)) % 100;
      reportFileName << StrFormatInt (quadratSizeInt, "Z000")   << "-" 
                     << StrFormatInt (quadratSizeFtp, "ZZZ0");    
    }

    reportFileName << ".txt";
  }

  r = new ofstream (reportFileName.Str ());
  if  (!r->is_open ())
  {
    log.Level (-1) << endl << endl
                   << "SpatialAnalysis    ***ERROR***     Could not open ReportFile[" << reportFileName << "]" << endl
                   << endl;
    Abort (true);
    return;
  }

  MLClassListPtr  classes = new MLClassList ();

  log.Level (10) << "SpatialAnalysis   Loading Plankton Data." << endl;
  data =  FeatureFileIOPices::Driver ()->LoadInSubDirectoryTree
                                 (PicesFVProducerFactory::Factory (&log),
                                  sourceRootDirPath,
                                  *classes,
                                  true,            // useDirectoryNameForClassName,
                                  DB (),
                                  cancelFlag,
                                  false,           // rewiteRootFeatureFile
                                  log
                                 );

  if  ((!data)  ||  (data->QueueSize () < 1))
  {
    log.Level (-1) << endl << endl
                   << "SpatialAnalysis    ***ERROR***     No Image Data Loaded"  << endl
                   << endl;
    Abort (true);
    return;
  }

  GetSipperFileNameAndInstrumentData ();

  if  (bucketCount < 1)
    bucketSize = 0;
  else
    bucketSize = windowSize / bucketCount;


  uint  numInsideConfInterval = (uint)(ceil ((float)(numOfIterations) * confidenceInterval / 100.0f));
  uint  numOutsideInterval    = (uint)numOfIterations - numInsideConfInterval;
  lowerConfIntervalIdx = (numOutsideInterval / 2);
  upperConfIntervalIdx = lowerConfIntervalIdx + numInsideConfInterval - 1;

  PicesApplication::PrintStandardHeaderInfo (*r);
  *r << "---------------------------------------------------------------------------------------" << endl;
  *r << "Build Date"              << "\t" << __DATE__ << "  " << __TIME__                         << endl;
  *r << "Report File Name"        << "\t" << reportFileName                                       << endl;
  *r << "Source Root Directory"   << "\t" << sourceRootDirPath                                    << endl;
  *r << "Statistic"               << "\t" << StatTypeToStr (statType)                             << endl;
  *r << "Iterations"              << "\t" << numOfIterations                                      << endl;
  *r << "ConfidenceInterval"      << "\t" << confidenceInterval                                   << endl;
  *r << "ConfidenceIntervalRange" << "\t" << lowerConfIntervalIdx << "-" << upperConfIntervalIdx  << endl;
  *r << "QuadratSize"             << "\t" << quadratSize                                          << endl;
  *r << "WindowSize"              << "\t" << windowSize                                           << endl;
  *r << "BucketCount"             << "\t" << bucketCount                                          << endl;
  *r << "BucketSize"              << "\t" << bucketSize                                           << endl;
  *r << "DefaultFlowRate"         << "\t" << defaultFlowRate                                      << endl;
  *r << "DefaultScanRate"         << "\t" << defaultScanRate                                      << endl;
  *r << "---------------------------------------------------------------------------------------" << endl;
  *r << endl;

  delete  classes;
  classes = NULL;
}  /* InitalizeApplication */



void  SpatialAnalysis::GetSipperFileNameAndInstrumentData ()
{
  sipperFileName = "";

  ImageFeaturesList::const_iterator  idx;
  for  (idx = data->begin ();  idx != data->end ();  idx++)
  {
    const ImageFeaturesPtr  i = *idx;

    uint  scanLineNum     = 0;
    uint  scanCol         = 0;
    KKStr  tempSipperFileName = "";
    
    PicesVariables::ParseImageFileName (i->ExampleFileName (), tempSipperFileName, scanLineNum, scanCol);
    tempSipperFileName = osGetRootName (tempSipperFileName);
    if  (sipperFileName.Empty ())
    {
      sipperFileName = tempSipperFileName;
    }
    else
    {
      if  (tempSipperFileName != sipperFileName)
      {
        log.Level (-1) << endl << endl << "SpatialAnalysis::GetSipperFileNameAndInstrumentData    ***ERROR***" << endl
                       << endl
                       << "More than one SIPPER file in source data  [" << sipperFileName << "]  and  [" << tempSipperFileName << "]" << endl
                       << endl;
        if  (r)
        {
          *r << endl << endl
             << "***********************************************************************************************************" << endl
             << " More than one SIPPER file in source data  [" << sipperFileName << "]  and  [" << tempSipperFileName << "]"   << endl
             << "***********************************************************************************************************" << endl
             << endl
             << endl;
        }

        break;
      }
    }
  }

  sipperFile = new SipperFile (sipperFileName);
  instrumentData = InstrumentDataFileManager::GetInstrumentDataForSipperFile (sipperFileName, 
                                                                              sipperFile, 
                                                                              false,        // false = Do not fource reextraction of instrument data.
                                                                              cancelFlag, 
                                                                              log
                                                                             );

  VectorDouble  positions = data->ExtractPositionsByMeter (instrumentData, defaultScanRate, defaultFlowRate);
  transactLength = positions[positions.size () - 1];
}  /* GetSipperFileNameAndInstrumentData */



/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  SpatialAnalysis::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                const KKStr&  parmValue
                                               )
{
  bool validParm = true;

  if  (parmSwitch.EqualIgnoreCase ("-CONFIDENCEINTERVAL")  ||
       parmSwitch.EqualIgnoreCase ("-CI")
      )
  {
    confidenceInterval = parmValue.ToFloat ();
    if  ((confidenceInterval < 1.0f)  ||  (confidenceInterval > 100.0))
    {
      log.Level (-1) << "ProcessCmdLineParameter   *** Invalid ConfidenceInterval[" << confidenceInterval << "]    Valid range (1 <-> 100)" << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-QUADRATSIZE")  ||
       parmSwitch.EqualIgnoreCase ("-QS")
      )
  {
    quadratSize = parmValue.ToFloat ();
    if  (quadratSize <= 0.0f)
    {
      log.Level (-1) << "ProcessCmdLineParameter   *** Invalid QuadratSize[" << quadratSize << "] parameter" << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if  
      (parmSwitch.EqualIgnoreCase ("-ITERATIONS")  ||
       parmSwitch.EqualIgnoreCase ("-I")
      )
  {
    numOfIterations = atoi (parmValue.Str ());
    if  (numOfIterations < 1)
    {
      log.Level (-1) << "ProcessCmdLineParameter   *** Invalid Iterations[" << numOfIterations << "] parameter" << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-R")  ||
       parmSwitch.EqualIgnoreCase ("-REPORT")
      )
    reportFileName = parmValue;

  else if
      (parmSwitch.EqualIgnoreCase ("-S")   ||  
       parmSwitch.EqualIgnoreCase ("-SRC") ||
       parmSwitch.EqualIgnoreCase ("-SOURCE")
      )
    sourceRootDirPath = parmValue;

  else if
      (parmSwitch.EqualIgnoreCase ("-STAT")  ||
       parmSwitch.EqualIgnoreCase ("-STATISTIC")
      )
  {
    statType = StatTypeFromStr (parmValue);
    if  (statType == StatType::Null)
    {
      log.Level (-1) << endl << endl
                     << "SpatialAnalysis::ProcessCmdLineParameter    ***ERROR***" << endl
                     << endl
                     << "             Invalid Statistic[" << parmSwitch << "]" << endl
                     << endl
                     << "             Valid Statistivc Types ar <BQV | PQV | TTLQC>" << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-WS")     ||
       parmSwitch.EqualIgnoreCase ("-WINDOW") ||
       parmSwitch.EqualIgnoreCase ("-WINDOWSIZE")
      )
  {
    windowSize = parmValue.ToFloat ();
    if  (windowSize <= 0.0f)
    {
      log.Level (-1) << endl << endl
                     << "SpatialAnalysis::ProcessCmdLineParameter    ***ERROR***" << endl
                     << endl
                     << "             Window Size[" << windowSize << "]   must be greater than 0.0,  typical values are 1, 10, 100, etc" << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-BC") ||
       parmSwitch.EqualIgnoreCase ("-BUCKETCOUNT")
      )
  {
    bucketCount = parmValue.ToInt ();
    if  (bucketCount <= 1)
    {
      log.Level (-1) << endl << endl
                     << "SpatialAnalysis::ProcessCmdLineParameter    ***ERROR***" << endl
                     << endl
                     << "             Bucket Count[" << bucketCount << "]   must be greater than 1" << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-DEFAULTFLOWRATE")  ||
       parmSwitch.EqualIgnoreCase ("-FLOWRATE")         ||
       parmSwitch.EqualIgnoreCase ("-DFR")
      )
  {
    defaultFlowRate = parmValue.ToFloat ();
    if  (defaultFlowRate <= 0.01f)
    {
      log.Level (-1) << endl << endl
                     << "SpatialAnalysis::ProcessCmdLineParameter    ***ERROR***" << endl
                     << endl
                     << "             DefaultFlowRate[" << defaultFlowRate << "]   must be greater than 0.0" << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-DEFAULTSCANRATE")  ||
       parmSwitch.EqualIgnoreCase ("-SCANRATE")         ||
       parmSwitch.EqualIgnoreCase ("-DSR")
      )
  {
    defaultScanRate = parmValue.ToFloat ();
    if  (defaultScanRate <= 0.01f)
    {
      log.Level (-1) << endl << endl
                     << "SpatialAnalysis::ProcessCmdLineParameter    ***ERROR***" << endl
                     << endl
                     << "             DefaultScanRate[" << defaultScanRate << "]   must be greater than 0.0" << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }

  else
  {
    validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }

	return  validParm;
}  /* ProcessCmdLineParameter */



/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   SpatialAnalysis::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
       << "SpactialAnalysis  -SrcDir <xxx>  -report <xxxx>"                    << endl
       << endl
       << "    -QuadratSize   Lengthj of each quadrat;  defalts to 1.0."       << endl
       << endl
       << "    -report   Report File,  Defaults to Command Line."              << endl
       << endl
       << "    -src      Source Directory Tree to Search for images."          << endl
       << endl
       << "    -Stat     <BQV | PQV | TTLQC | DAVIS92>   Type of statistic"    << endl
       << endl
       << "    -WindowSize  <Size in Meters>  used by Davis92."                << endl
       << endl
       << "    -BucketCount <Number subdivisions of WindowSize>"               << endl
       << endl
       << "    -DefaultFlowRate  <Flow Rate,  defaults to 1.0>"                << endl
       << endl
       << "    -DefaultScanRate  <Scan Lines per Sec, defaults to 24950.0>"    << endl
       << endl
       << "    -Iterations  <Number of random resorts to do>"                  << endl
       << endl
       << "    -ConfidenceInterval  <Defaults to 95.0>"                        << endl
       << endl;
}  /* DisplayCommandLineParameters */



void  SpatialAnalysis::RandomizeLocations (ImageFeaturesList&  examples,
                                           long                maxNumScanLines
                                          )
{
  ImageFeaturesList::iterator  idx;

  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    ImageFeaturesPtr  i = *idx;
    long  randRow = (randNums.Next () % maxNumScanLines);
    i->SfCentroidRow (randRow);
  }
}  /* RandomizeLocations */



double   SpatialAnalysis::Davis92DesityForBinSizeH (const VectorDouble&  x,
                                                    int                  startParticle,   // Particle Index in 'x'
                                                    float                binSize
                                                   )
{
  uint i = startParticle + 1;  // We do not include the starting particle in th density calc.
  double  e = x[startParticle] + binSize;

  int  count = 0;
  while  ((i < x.size ())  &&  (x[i] <= e))
  {
    count++;
    i++;
  }
  
  double  density = (double)count / ((double)binSize * 0.098 * 0.098);

  return  density;
}  /* Davis92DesityForBinSizeH */



float   SpatialAnalysis::Davis92PatchinessIndexForBinSize (const VectorDouble&  x,
                                                           float                binSize,
                                                           double               transactLength
                                                          )
{
  double  totalDensites = 0.0;
  int     count = 0;
  for  (uint i = 0;  i < x.size ();  i++)
  {
    double  end = x[i] + binSize;
    if  (end > transactLength)
      break;

    totalDensites += Davis92DesityForBinSizeH (x, i, binSize);
    count++;
  }

  double  patchinessIndex = 0.0;
  if  (count > 0)
    patchinessIndex = totalDensites / (double)count;

  return  (float)patchinessIndex;
}  /* Davis92PatchinessIndexForBinSize */



VectorFloat  SpatialAnalysis::CalculatePatchinessIndexUsingDavis92
                       (const VectorDouble&   particles,          // Particle Locations
                        float                 windowSize,
                        uint                  binCount,
                        double                transactLength
                       )
{
  float  binSlotSize = windowSize / (float)bucketCount;

  VectorFloat  patchinessIndexes;

  uint  pi = 0;
  for  (pi = 0;  pi < binCount;  pi++)
  {
    float  binSize = binSlotSize * (pi + 1);
    float  patchinessIndex = Davis92PatchinessIndexForBinSize (particles, binSize, transactLength);
    patchinessIndexes.push_back (patchinessIndex);
  }

  return  patchinessIndexes;
}  /* CalculatePatchinessIndexUsingDavis92 */



VectorFloat  SpatialAnalysis::CalculatePatchinessIndexUsingDavis92Integrated 
                       (const VectorDouble&   particles,          // Particle Locations
                        float                 windowSize,
                        uint                  binCount,
                        double                transactLength
                       )
{
  double  binSlotSize = windowSize / (float)bucketCount;

  VectorFloat  patchinessIndexes (binCount);

  VectorInt  binCounts (binCount);
  VectorInt  binVotes  (binCount);

  VectorDouble::const_iterator  idx;
  VectorDouble::const_iterator  idx2;

  for  (idx = particles.begin ();  idx != particles.end ();  idx++)
  {
    double  startPos = *idx;
    
    idx2 = idx;
    idx2++;

    uint  numBinsToCover = (int)((transactLength - startPos) / binSlotSize);
    if (numBinsToCover > binCount)
      numBinsToCover = binCount;

    for  (uint x = 0;  x < numBinsToCover;  x++)
      binVotes[x]++;

    while  (idx2 != particles.end ())
    {
      double  nextPos = *idx2;
      double  dist = nextPos - startPos;

      if  (dist > windowSize)
        break;

      uint binIdx = (int)(dist / binSlotSize);

      if  (binIdx >= binCount)
        break;

      double  endOfBin = startPos + ((double)(binIdx + 1) * binSlotSize);
      if  (endOfBin > transactLength)
        break;

      binCounts[binIdx]++;
      idx2++;
    }
  }


  {
    int  accumulater = 0;
    // calculat Mean Densities
    for  (uint x = 0;  x < binCount;  x++)
    {
      accumulater += binCounts[x];
      double  length = binSlotSize * (double)(x + 1);
      double  avgCount = 0.0;
      if  (binVotes [x] > 0)
        avgCount = (double)accumulater / (double)binVotes[x];
      patchinessIndexes[x] = (float)(avgCount / (length * 0.098 * 0.098));
    }
  }

  return  patchinessIndexes;
}  /* CalculatePatchinessIndexUsingDavis92Integrated */






VectorFloat  SpatialAnalysis::CalculatePatchinessIndexUsingDavis92Integrated2 
                       (const VectorDouble&   particles,          // Particle Locations
                        float                 windowSize,
                        uint                  binCount,
                        double                transactLength
                       )
{
  double  binSlotSize = windowSize / (float)bucketCount;

  VectorFloat  patchinessIndexes (binCount);

  VectorInt  binCounts (binCount);
  VectorInt  binVotes  (binCount);

  VectorDouble::const_iterator  idx;
  VectorDouble::const_iterator  idx2;

  for  (idx = particles.begin ();  idx != particles.end ();  idx++)
  {
    double  startPos = *idx;
    
    idx2 = idx;
    idx2++;

    uint  acumulator = 0;

    uint  binIdx = 0;
    double  endOfBin = startPos + binSlotSize;
    binVotes[0]++;

    while  (idx2 != particles.end ())
    {
      double  nextPos = *idx2;
      double  dist = nextPos - startPos;
      if  (dist > windowSize)
        break;

      while  (nextPos > endOfBin)
      {
        binIdx++;
        endOfBin = startPos + (double)(binIdx + 1) * binSlotSize;
        if  (endOfBin > transactLength)
          break;
        binCounts[binIdx] += acumulator;
        binVotes[binIdx]++;
      }

      if  (endOfBin > transactLength)
        break;

      binCounts[binIdx]++;
      acumulator++;
      idx2++;
    }

    
    while  (binIdx < (binCount - 1))
    {
      binIdx++;
      endOfBin =  startPos + (double)(binIdx + 1) * binSlotSize;
      if  (endOfBin > transactLength)
        break;
      binCounts[binIdx] += acumulator;
      binVotes[binIdx]++;
    }
  }


  {
    for  (uint x = 0;  x < binCount;  x++)
    {
      double  length = binSlotSize * (double)(x + 1);
      double  avgCount = 0.0;
      if  (binVotes [x] > 0)
        avgCount = (double)(binCounts[x]) / (double)binVotes[x];
      patchinessIndexes[x] = (float)(avgCount / (length * 0.098 * 0.098));
    }
  }

  return  patchinessIndexes;
}  /* CalculatePatchinessIndexUsingDavis92Integrated2 */






void  SpatialAnalysis::OneDavis92SpatialAnalysisRun (ImageFeaturesList&     src,
                                                     long                   maxNumScanLines,
                                                     VectorFloat&           patchinessStats,
                                                     VectorFloat&           patchinessStatMeans,
                                                     VectorFloat&           patchinessStatStdDevs,
                                                     VectorFloat&           patchinessStatMins,
                                                     VectorFloat&           patchinessStatMaxs,
                                                     VectorFloat&           patchinessStatLowerConfidences,
                                                     VectorFloat&           patchinessStatUpperConfidences
                                                    )
{
  int  iteration = 0;

  patchinessStats.clear ();
  patchinessStatMeans.clear ();
  patchinessStatStdDevs.clear ();
  patchinessStatMins.clear ();
  patchinessStatMaxs.clear ();

  float        mean;
  float        max;
  float        min;
  float        patchiness;
  float        stdDev;
  VectorFloat  temp;

  uint  longestRun = 0;

  VectorDouble  positions = src.ExtractPositionsByMeter (instrumentData, defaultScanRate, defaultFlowRate);

  patchinessStats = CalculatePatchinessIndexUsingDavis92Integrated2 (positions, windowSize, bucketCount, transactLength);
  //patchinessStats = CalculatePatchinessIndexUsingDavis92           (positions, windowSize, bucketCount, transactLength);

  ImageFeaturesListPtr  workExamples = new ImageFeaturesList (src, true);  // true=Create duplicate examples and we will own them.

  vector<VectorFloat>  patchinessByIteration;
  for  (iteration = 0;  iteration < numOfIterations;  iteration++)
  {
    RandomizeLocations (*workExamples, maxNumScanLines);

    positions = workExamples->ExtractPositionsByMeter (instrumentData, defaultScanRate, defaultFlowRate);
    VectorFloat  iterationPatchines = CalculatePatchinessIndexUsingDavis92Integrated2 (positions, windowSize, bucketCount, transactLength);
    //VectorFloat  iterationPatchines = CalculatePatchinessIndexUsingDavis92           (positions, windowSize, bucketCount, transactLength);
    patchinessByIteration.push_back (iterationPatchines);
    longestRun = Max (longestRun, (uint)iterationPatchines.size ());
  }


  for  (uint bucketIdx = 0;  bucketIdx < (uint)bucketCount;  bucketIdx++)
  {
    min = FLT_MAX;
    max = FLT_MIN;

    temp.clear ();
    for  (iteration = 0;  iteration < numOfIterations;  iteration++)
    {
      if  (bucketIdx < patchinessByIteration[iteration].size ())
      {
        patchiness = patchinessByIteration[iteration][bucketIdx];
        temp.push_back (patchiness);
        min = Min (min, patchiness);
        max = Max (max, patchiness);
      }
    }
    
    CalcMeanAndStdDev (temp, mean, stdDev);

    patchinessStatMeans.push_back   (mean);
    patchinessStatStdDevs.push_back (stdDev);
    patchinessStatMins.push_back    (min);
    patchinessStatMaxs.push_back    (max);

    sort (temp.begin (), temp.end ());
    patchinessStatLowerConfidences.push_back (temp [lowerConfIntervalIdx]);
    patchinessStatUpperConfidences.push_back (temp [upperConfIntervalIdx]);
  }

  delete  workExamples;  
  workExamples = NULL;
}  /* OneDavis92SpatialAnalysisRun */
                                              


void  SpatialAnalysis::CalcStatistic (ImageFeaturesList&   src,
                                      VectorFloat&         densityByQuadrat,
                                      VectorFloat&         stats,
                                      long                 maxNumScanLines
                                     )
{
  densityByQuadrat = src.CalculateDensitesByQuadrat (24950, quadratSize, 0.75, cancelFlag, log);
  uint  maxDist = (uint)densityByQuadrat.size () / 4;

  stats.clear ();
  stats.push_back (0.0);   // There is no distance '0' so we populate it with zero.

  for  (uint dist = 1;  dist <= maxDist;  dist++)
  {
    float  stat = 0.0f;

    switch  (statType)
    {
    case  StatType::BQV:    stat = CalcBQV    (densityByQuadrat, dist);  break;
    case  StatType::PQV:    stat = CalcPQV    (densityByQuadrat, dist);  break;
    case  StatType::TTLQC:  stat = CalcTTLQC  (densityByQuadrat, dist);  break;
    }  /* switch  (statType) */

    stats.push_back (stat);
  }
}  /* CalcStatistic */



void  SpatialAnalysis::OneSpatialAnalysisRun (ImageFeaturesList&   src,
                                              long                 maxNumScanLines,
                                              VectorFloat&         densityByQuadrat,
                                              VectorFloat&         statistic,
                                              VectorFloat&         means,
                                              VectorFloat&         stdDevs
                                             )
{
  int  iteration = 0;

  statistic.clear ();
  means.clear     ();
  stdDevs.clear   ();

  VectorFloat  temp;
  float        mean;
  float        stdDev;

  CalcStatistic (src, densityByQuadrat, statistic, maxNumScanLines);

  ImageFeaturesListPtr  workExamples = new ImageFeaturesList (src, true);  // true=Create duplicate examples and we will own them.

  uint  shortestRun = UINT_MAX;
  uint  longestRun  = 0;

  vector<VectorFloat>  statsByIteration;
  for  (iteration = 0;  iteration < numOfIterations;  iteration++)
  {
    VectorFloat  iterationStat;
    VectorFloat  densitiesForThisIteration;
    RandomizeLocations (*workExamples, maxNumScanLines);
    CalcStatistic (*workExamples, densitiesForThisIteration, iterationStat, maxNumScanLines);
    statsByIteration.push_back (iterationStat);

    shortestRun = Min (shortestRun, (uint)iterationStat.size ());
    longestRun  = Max (longestRun,  (uint)iterationStat.size ());
  }


  for  (uint x = 0;  x < longestRun;  x++)
  {
    temp.clear ();
    for  (iteration = 0;  iteration < numOfIterations;  iteration++)
    {
      if  (x < statsByIteration[iteration].size ())
        temp.push_back (statsByIteration[iteration][x]);
    }
    
    CalcMeanAndStdDev (temp, mean, stdDev);
    means.push_back (mean);
    stdDevs.push_back (stdDev);
  }

  delete  workExamples;  
  workExamples = NULL;
}  /* OneSpatialAnalysisRun */
                                              


KKStr  SpatialAnalysis::StatTypeToStr (StatType  statType)
{
  switch  (statType)
  {
  case  StatType::Null:    return  "NULL";
  case  StatType::BQV:     return  "BQV";
  case  StatType::Davis92: return  "DAVIS92";
  case  StatType::PQV:     return  "PQV";
  case  StatType::TTLQC:   return  "TTLQC";
  default:
      return "UnKnown";
  }
}  /* StatTypeToStr */



SpatialAnalysis::StatType  SpatialAnalysis::StatTypeFromStr (KKStr  s)
{
  s = s.ToUpper ();

  if  (s == "BQV")
    return  StatType::BQV;

  else if  (s == "DAVIS92")
    return  StatType::Davis92;

  else if  (s == "PQV")
    return  StatType::PQV;

  else if  (s == "TTLQC")
    return  StatType::TTLQC;

  return  StatType::Null;
}  /* StatTypeToStr */



void  SpatialAnalysis::PrintSpatialHistogramReport ()
{
  *r << endl << endl
     << "Spatial Histogram  10 Meter Increments" << endl
     << endl;
  data->PrintSpatialHistogramReport (*r, instrumentData, defaultScanRate, defaultFlowRate, 10.0f);

  *r << endl << endl
     << "Spatial Histogram  5 Meter Increments" << endl
     << endl;
  data->PrintSpatialHistogramReport (*r, instrumentData, defaultScanRate, defaultFlowRate, 5.0f);

  *r << endl << endl
     << "Spatial Histogram  1 Meter Increments" << endl
     << endl;
  data->PrintSpatialHistogramReport (*r, instrumentData, defaultScanRate, defaultFlowRate, 1.0f);
}  /* PrintSpatialHistogramReport */



void  SpatialAnalysis::RunDavis92SpatialAnalysis ()
{
  bool  cancelFlag = false;

  double  startCPUclock = osGetSystemTimeUsed ();

  long  maxNumScanLines = (long)data->BackOfQueue ()->SfCentroidRow ();

  MLClassListPtr  classes = data->ExtractListOfClasses ();
  classes->SortByName ();

  vector<VectorFloat>   stats;
  vector<VectorFloat>   means;
  vector<VectorFloat>   stdDevs;
  vector<VectorFloat>   statMins;
  vector<VectorFloat>   statMaxs;
  vector<VectorFloat>   statLowerConfidences;
  vector<VectorFloat>   statUpperConfidences;

  VectorFloat    fVector;

  int  iteration;

  for  (iteration = 0;  iteration < (int)classes->size ();  iteration++)
  {
    stats.push_back                (fVector);
    means.push_back                (fVector);
    stdDevs.push_back              (fVector);
    statMins.push_back             (fVector);
    statMaxs.push_back             (fVector);
    statLowerConfidences.push_back (fVector);
    statUpperConfidences.push_back (fVector);
  }

  for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
  {
    MLClassPtr  ic = classes->IdxToPtr (classIdx);
    ImageFeaturesListPtr  imagesThisClass = data->ExtractExamplesForAGivenClass (ic);
    if  (!imagesThisClass)
      continue;

    cout << ic->Name () << std::endl;

    OneDavis92SpatialAnalysisRun (*imagesThisClass, 
                                  maxNumScanLines, 
                                  stats                [classIdx], 
                                  means                [classIdx], 
                                  stdDevs              [classIdx], 
                                  statMins             [classIdx], 
                                  statMaxs             [classIdx],
                                  statLowerConfidences [classIdx],
                                  statUpperConfidences [classIdx]
                                 );
  }

  double  endCPUclock = osGetSystemTimeUsed ();
  double  totalProcessingCpuSecs = endCPUclock - startCPUclock;
  {
    *r << endl << endl
      << "Processing Time:" << "\t" << totalProcessingCpuSecs << endl
      << endl;
  }

  *r  << "Class Statistics" << endl << endl;
  data->PrintClassStatistics (*r);
  *r << endl;

  {
    *r << endl << endl << endl
       << "Statistics by Class by Bucket Size" << endl
       << endl;

    KKStr  hl1 = "";
    KKStr  hl2 = "Bucket";
    for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
    {
      MLClassPtr  ic = classes->IdxToPtr (classIdx);

      hl1 << "\t" << "\t" << ic->Name ()              << "\t" << ""     << "\t" << ""        << "\t"  << ""     << "\t" << ""     << "\t"  << "Lower"  << "\t"  << "Upper";
      hl2 << "\t" << "\t" << StatTypeToStr (statType) << "\t" << "Mean" << "\t" << "StdDev"  << "\t"  << "Min"  << "\t" << "Max"  << "\t"  << "Conf"   << "\t"  << "Conf";
    }

    *r << hl1 << endl;
    *r << hl2 << endl;
  }

  for  (int bucketIdx = 0;  bucketIdx < bucketCount;  bucketIdx++)
  {
    *r << ((bucketIdx + 1) * bucketSize);
    for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
    {
      MLClassPtr  ic = classes->IdxToPtr (classIdx);
      if  (((uint)bucketIdx < stats   [classIdx].size ())  &&
           ((uint)bucketIdx < means   [classIdx].size ())  &&
           ((uint)bucketIdx < stdDevs [classIdx].size ())  &&
           ((uint)bucketIdx < statMins[classIdx].size ())  &&
           ((uint)bucketIdx < statMaxs[classIdx].size ())
          )
      {
        *r << "\t"
           << "\t" << stats                [classIdx][bucketIdx]
           << "\t" << means                [classIdx][bucketIdx]
           << "\t" << stdDevs              [classIdx][bucketIdx]
           << "\t" << statMins             [classIdx][bucketIdx]
           << "\t" << statMaxs             [classIdx][bucketIdx]
           << "\t" << statLowerConfidences [classIdx][bucketIdx]
           << "\t" << statUpperConfidences [classIdx][bucketIdx]
           ;
      }
      else
      {
        *r << "\t" << "\t" << "\t" << "\t" << "\t" << "\t" << "\t" << "\t";
      }
    }

    *r << std::endl;
  }

  PrintSpatialHistogramReport ();

  for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
  {
    MLClassPtr  ic = classes->IdxToPtr (classIdx);
    ImageFeaturesListPtr  imagesThisClass = data->ExtractExamplesForAGivenClass (ic);
    if  (!imagesThisClass)
      continue;

    cout << "Position report for class " << ic->Name () << std::endl;

    *r << endl << endl
       << "Position report for class " << ic->Name () << endl
       << endl;

    VectorDouble  positions = imagesThisClass->ExtractPositionsByMeter (instrumentData, defaultScanRate, defaultFlowRate);
    for  (uint  idx = 0;  idx < positions.size ();  idx++)
    {
      ImageFeaturesPtr fv = imagesThisClass->IdxToPtr (idx);
      *r << fv->ExampleFileName () << "\t" << fv->SfCentroidRow () << "\t" << positions[idx] << endl;
    }

    delete  imagesThisClass;
  }
}  /* RunDavis92SpatialAnalysis */



void  SpatialAnalysis::RunSpatialAnalysis ()
{
  bool  cancelFlag = false;

  int  classNum        = 0;
  int  dist            = 0;
  int  maxDist         = 0;
  uint maxNumQuadrats  = 0;
  long maxNumScanLines = 0;
  
  *r  << "Class Statistics" << endl << endl;
  data->PrintClassStatistics (*r);
  *r << endl;

  PrintSpatialHistogramReport ();
 
  maxNumScanLines = (long)ceil(data->BackOfQueue ()->SfCentroidRow ());

  MLClassListPtr  classes = data->ExtractListOfClasses ();
  classes->SortByName ();

  vector<VectorFloat>   stats;
  vector<VectorFloat>   means;
  vector<VectorFloat>   stdDevs;
  vector<VectorFloat>   densities;

  VectorFloat    fVector;

  for  (kkuint32 x = 0;  x < classes->size ();  x++)
  {
    stats.push_back     (fVector);
    means.push_back     (fVector);
    stdDevs.push_back   (fVector);
    densities.push_back (fVector);
  }

  for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
  {
    MLClassPtr  ic = classes->IdxToPtr (classIdx);
    ImageFeaturesListPtr  imagesThisClass = data->ExtractExamplesForAGivenClass (ic);
    if  (!imagesThisClass)
      continue;

    cout << ic->Name () << std::endl;

    OneSpatialAnalysisRun (*imagesThisClass, maxNumScanLines, densities[classIdx], stats[classIdx], means[classIdx], stdDevs[classIdx]);

    maxDist = Max (maxDist, (int)stats[classIdx].size ());
    maxNumQuadrats = Max (maxNumQuadrats, (uint)densities[classIdx].size ());
  }

  {
    int  quadratIdx = 0;
    KKStr  hl1 = "";
    KKStr  hl2 = "";
    KKStr  hl3 = "";
    classes->ExtractThreeTitleLines (hl1, hl2, hl3);

    *r << endl << endl << endl
       << "Denities by Class by Quadrat" << endl
       << endl;

    *r << ""        << "\t" << ""     << "\t" << hl1 << endl;
    *r << ""        << "\t" << "From" << "\t" << hl2 << endl;
    *r << "Quadrat" << "\t" << "To"   << "\t" << hl3 << endl;

    for  (quadratIdx = 0;  quadratIdx < (int)maxNumQuadrats;  quadratIdx++)
    {
      float  startOfQuadrat = (quadratIdx * quadratSize);
      float  endOfQuadrat   = ((quadratIdx + 1) * quadratSize);

      *r <<         quadratIdx 
         << "\t" << StrFormatDouble (startOfQuadrat, "Z,ZZ0.00") << "-" << StrFormatDouble (endOfQuadrat, "Z,ZZ0.00");

      for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
      {
        if  (quadratIdx < (int)densities[classIdx].size ())
          *r << "\t" << densities[classIdx][quadratIdx];
        else
          *r << "\t";
      }
      *r << endl;
    }  /* quadratIdx */
  }

  {
    *r << endl << endl << endl
       << "Statistics by Class by Block Size" << endl
       << endl;

    KKStr  hl1 = "";
    KKStr  hl2 = "Dist";
    for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
    {
      MLClassPtr  ic = classes->IdxToPtr (classIdx);

      hl1 << "\t" << "\t" << ic->Name ()              << "\t" << ""     << "\t" << "";
      hl2 << "\t" << "\t" << StatTypeToStr (statType) << "\t" << "Mean" << "\t" << "StdDev";
    }

    *r << hl1 << endl;
    *r << hl2 << endl;
  }

  for  (dist = 0;  dist < maxDist;  dist++)
  {
    *r << dist;
    for  (kkuint32 classIdx = 0;  classIdx < classes->QueueSize ();  classIdx++)
    {
      MLClassPtr  ic = classes->IdxToPtr (classIdx);
      if  (((uint)dist < stats[classIdx].size ())  &&
           ((uint)dist < means[classIdx].size ())  &&
           ((uint)dist < stdDevs[classIdx].size ())
          )
      {
        *r << "\t"
           << "\t" << stats[classIdx][dist]
           << "\t" << means[classIdx][dist]
           << "\t" << stdDevs[classIdx][dist];
      }
      else
      {
        *r << "\t" << "\t" << "\t" << "\t";
      }
    }

    *r << std::endl;
  }
}  /* RunSpatialAnalysis */



int  main (int     argc,
           char**  argv
          )
{
  SpatialAnalysis  spatialReport;
  spatialReport.InitalizeApplication (argc, argv);
  if  (!spatialReport.Abort ())
  {
    if  (spatialReport.TypeOfStat () == SpatialAnalysis::StatType::Davis92)
      spatialReport.RunDavis92SpatialAnalysis ();
    else
      spatialReport.RunSpatialAnalysis ();
  }
}
