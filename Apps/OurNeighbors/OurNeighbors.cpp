#include "FirstIncludes.h"

#include <stdlib.h>
#include <memory>
#include <math.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "MemoryDebug.h"
#include "KKBaseTypes.h"

using namespace std;

#include "OSservices.h"
#include "KKStr.h"
#include "StatisticalFunctions.h"
using namespace KKB;


// From PICL
#include "FeatureFileIO.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
#include "PicesFVProducer.h"
#include "FeatureFileIOPices.h"
using namespace  MLL;


#include "ClassSummary.h"
#include "LLoydsEntry.h"
#include "OurNeighbors.h"
#include "Neighbor.h"
#include "RandomNND.h"



// -s C:\TrainingApp\ClassifiedImages\hrs020701 -r NeighborReport.txt


/******************************************************************************
 * command line parameters that have been used for testing purposes
 ******************************************************************************/

// -s "C:\TrainingApp\ClassifiedImages" -r "report.txt"


// 2005-06-03
// -src C:\TrainingApp\ClassifiedImages\HRS030208b  -Report TestTeport.txt  -random


// 2005-08-31
//  -src C:\TrainingApp\ClassifiedImages\hrs030204  -Report TestTeport.txt  -random -B 50 -bs 20 -I 20 -FP Cnidaria


// C:\TrainingApp\ClassifiedImages\hrs020404_23
//  -src C:\TrainingApp\ClassifiedImages\hrs020404_23  -Report TestTeport.txt  -random -B 50 -bs 20 -I 20


// C:\TrainingApp\ClassifiedImages\hrs020501_man
// -src C:\TrainingApp\ClassifiedImages\hrs020501_man  -report TestReport.txt 
// -src C:\TrainingApp\ClassifiedImages\hrs020403  -report TestReport.txt

// -src D:\TrainingApp\ExtractedImages\hrs020505_man  -report  TestReport.txt  -random  -b 60 -I 50  -LLoydsBinSize 20000  -nnt same  -ExcludeClass Artifact_lines
// -CmdFile  cmdLine

// C:\PICES\EXE>ourneighbors -s D:\TrainingApp\ExtractedImages\HRS0207_07_man -nnt same -r D:\TrainingApp\ExtractedImages\HRS0207_07_man\neighbor.txt -lloydsbinsize 31297 -bs 313 -bc 500 -random -i 500

OurNeighbors::OurNeighbors ():
  PicesApplication    (),
  baseLLoydsBinSize   (20000),
  bucketSize          (0),
  cancelFlag          (false),
  excludedClasses     (NULL),
  fvProducerFactory   (NULL),
  sourceRootDirPath   (),
  mlClass             (MLClass::CreateNewMLClass ("UnKnown")),
  mlClasses           (new MLClassList ()),
  report              (NULL),
  fromPlanktonName    (),
  fromPlankton        (NULL),
  lastScanLine        (0),
  lloydsBinsFileName  (),
  neighborType        (NeighborType::AnyPlankton),
  numOfBuckets        (0),
  numOfIterations     (20),
  randomizeLocations  (false)
{
  fvProducerFactory = PicesFVProducerFactory::Factory (&log);
}




/******************************************************************************
 * Destructor
 ******************************************************************************/
OurNeighbors::~OurNeighbors ()
{
	if  (reportFile)
	{
		reportFile->close ();
		delete  reportFile;
	}

	if  (htmlFile)
	{
		htmlFile->close ();
		delete  htmlFile;
	}

  delete  excludedClasses;  excludedClasses = NULL;

	//delete  mlClass;
}




void  OurNeighbors::InitalizeApplication (kkint32 argc,
                                          char**  argv
                                         )
{
  PicesApplication::InitalizeApplication (argc, argv);
	if  (Abort ())
	{
		DisplayCommandLineParameters ();
		return;
	}

  if  (!excludedClasses)
    excludedClasses = new MLClassList ();

	if  (sourceRootDirPath.Empty ())
		sourceRootDirPath = osGetCurrentDirectory ();

  if  (featureFileName.Empty ())
	{
		KKStr  path, root, extension;
		osParseFileName (sourceRootDirPath, path, root, extension);
		featureFileName = path;
		osAddLastSlash (featureFileName);
		featureFileName << root + DS + root + ".data";
	}

	if  (reportFileName.Empty ())
	{
		reportFile = NULL;
		report = &cout;
	}
	else
	{
		reportFile = new ofstream (reportFileName.Str ());
		report = reportFile;
	}

	htmlFile = NULL;
	if  (!htmlFileName.Empty ())
	{
		htmlFile = new ofstream (htmlFileName.Str());
		html = htmlFile;
	}

  lloydsBinsFileName = osRemoveExtension (reportFileName) + "_LLoydsBins.data";

  PicesApplication::PrintStandardHeaderInfo (*report);
	*report << endl;
	*report << "------------------------------------------------------------------------"  << endl;
  *report << "Build Date              [" << __DATE__ << "  " << __TIME__        << "]."  << endl;
  *report << "Report File Name        [" << reportFileName                      << "]."  << endl;
	*report << "HTML File Name          [" << htmlFileName                        << "]."  << endl;
	*report << "Source Root Directory   [" << sourceRootDirPath                   << "]."  << endl;
	*report << "Feature File            [" << featureFileName                     << "]."  << endl;
  *report << "Neighbor Type           [" << NeighborTypeToStr (neighborType)    << "]."  << endl;
  *report << "From Plankton           [" << fromPlanktonName                    << "]."  << endl;
  *report << "RandomizeLocations      [" << (randomizeLocations ? "Yes" : "No") << "]."  << endl;
  if  (randomizeLocations)
  {
    *report << "Iterations              [" << numOfIterations                     << "]."  << endl;
    *report << "BucketSize              [" << bucketSize                          << "]."  << endl;
    *report << "BucketCount             [" << numOfBuckets                        << "]."  << endl;
  }
  *report << "BaseLLoydsBinSize       [" << baseLLoydsBinSize                     << "]."  << endl;
  *report << "ExcludedClasses         [" << excludedClasses->ToCommaDelimitedStr ()  << "]."  << endl;
	*report << "------------------------------------------------------------------------"  << endl;
	*report << endl;
}  /* InitalizeApplication */







/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  OurNeighbors::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                             const KKStr&  parmValue
                                            )
{
  bool  validParm = true;
  if  (parmSwitch.EqualIgnoreCase ("-BUCKETCOUNT")  ||  parmSwitch.EqualIgnoreCase ("-BC"))
  {
    numOfBuckets = atoi (parmValue.Str ());
    if  (numOfBuckets < 2)
    {
      log.Level (-1) << "ProcessCmdLineParameter   *** Invalid Buckets[" << numOfBuckets << "] parameter" << endl;
      validParm = false;
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-BUCKETSIZE")  ||  parmSwitch.EqualIgnoreCase ("-BS"))
  {
    bucketSize = atoi (parmValue.Str ());
    if  (bucketSize < 1)
    {
      log.Level (-1) << "ProcessCmdLineParameter   *** Invalid BucketSize[" << bucketSize << "] parameter" << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-EXCLUDECLASS")  ||  parmSwitch.EqualIgnoreCase ("-EC"))
  {
    if  (parmValue.Empty ())
    {
      log.Level (-1) << endl << endl
                     << "ProcessCmdLineParameter   '-ExcludeClass'  missing class name to exclude." << endl
                     << endl;
      Abort (true);
    }

    else 
    {
      if  (!excludedClasses)
        excludedClasses = new MLClassList ();

      if  (excludedClasses->LookUpByName (parmValue) != NULL)
      {
        log.Level (-1) << "ProcessCmdLineParameter  '-ExcludeClass[" << parmValue << "]'  Specified more than once." << endl;
      }
      else
      {
        excludedClasses->PushOnBack (MLClass::CreateNewMLClass (parmValue));
      }
    }
  }


  else if  (parmSwitch.EqualIgnoreCase ("-FROMPLANKTON")  ||  parmSwitch.EqualIgnoreCase ("-FP"))
  {
    fromPlanktonName = parmValue;
  }

  else if  (parmSwitch.EqualIgnoreCase ("-H")  ||  parmSwitch.EqualIgnoreCase ("-HTML"))
		htmlFileName = parmValue;

  else if  (parmSwitch.EqualIgnoreCase ("-ITERATIONS")  ||  parmSwitch.EqualIgnoreCase ("-I"))
  {
    numOfIterations = atoi (parmValue.Str ());
    if  (numOfIterations < 1)
    {
      log.Level (-1) << "ProcessCmdLineParameter   *** Invalid Iterations[" << numOfIterations << "] parameter" << endl;
      Abort (true);
    }
  }


  else if  (parmSwitch.EqualIgnoreCase ("-LLOYDSBINSIZE")      ||  
            parmSwitch.EqualIgnoreCase ("-LBS")                ||  
            parmSwitch.EqualIgnoreCase ("-BASELLOYDSBINSIZE")  ||  
            parmSwitch.EqualIgnoreCase ("-BLBS")
           )
  {
    baseLLoydsBinSize = atoi (parmValue.Str ());
    if  (baseLLoydsBinSize < 10)
    {
      log.Level (-1) << endl << endl 
                     << "ProcessCmdLineParameter   *** Invalid BaseLLoydsBinSize[" << baseLLoydsBinSize << "] parameter  ***" << endl
                     << endl;
      Abort (true);
    }
  }


  else if  (parmSwitch.EqualIgnoreCase ("-NEARESTNEIGHBORTYPE")  ||  parmSwitch.EqualIgnoreCase ("-NNT"))
  {
    if  (parmValue.EqualIgnoreCase ("0")  ||  parmValue.EqualIgnoreCase ("ANY"))
      neighborType = NeighborType::AnyPlankton;

    else if  (parmValue.EqualIgnoreCase ("1")  ||  parmValue.EqualIgnoreCase ("SAME"))
      neighborType = NeighborType::SamePlankton;
  }

  else if  (parmSwitch.EqualIgnoreCase ("-R")  ||  parmSwitch.EqualIgnoreCase ("-REPORT"))
		reportFileName = parmValue;

  else if  (parmSwitch.EqualIgnoreCase ("-RAND")  ||  parmSwitch.EqualIgnoreCase ("-RANDOM"))
		randomizeLocations = true;

  else if  (parmSwitch.EqualIgnoreCase ("-S")  ||  parmSwitch.EqualIgnoreCase ("-SRC"))
		sourceRootDirPath = parmValue;

  else
    validParm = PicesApplication::ProcessCmdLineParameter(parmSwitch, parmValue);


	return  !Abort ();
}  /* ProcessCmdLineParameter */




/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   OurNeighbors::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
       << "    -report     <Filename>       Report File,  Defaults to Command Line."     << endl
	     << endl
			 << "    -src        <Dir-Path>       Source Directory Tree to Search for images." << endl
			 << endl
			 << "    -NearestNeighborType <0 | 1>."                                       << endl
			 << "                                 0 - Nearest Image (default)"            << endl
			 << "                                 1 - Nearest Image of Same Class"        << endl
			 << endl
			 << "    -FromPlankton  <Plankton Class>"                                     << endl
       << endl
			 << "    -Random                      Randomize the Row and Col Info."        << endl
       << endl
			 << "    -Buckets    <Number>         Number Buckets if Random Histogram."    << endl
       << endl
			 << "    -BucketSize <Number>         Histogram Bucket Size."                 << endl
       << endl
			 << "    -Iterations <Number>         Number of Random NND's to perform."     << endl
       << endl
			 << "    -ExcludeClass <ClassName>    Name of class to exclude.  You may "    << endl
       << "                                 specify this parameter more than once." << endl
       << endl
			 << "    -LLoydsBinSize <Number>      Number of scan lines per bin"           << endl
       << endl;
}  /* DisplayCommandLineParameters */





/****************************************************************************************
 *                           DetermineClassFromFileName                                 
 *                                                                                      
 * Given an Image File Name(fileName)  will return and mlClass.  Will use the      
 * path name in fileName to determine class.  If the class does not exist in            
 * mlClasses, will create a new instance and add it to mlClasses.                  
 *
 * After talking to the user it has come to light that the first sub-directory in
 * a path will dictate what class he/she feels a image belongs to.
 *   Ex:
 *    .../Protista/
 *    .../Protista/SubDir1
 *    .../Trichodesmium
 *    .../Trichodesmium/SubDir1
 *    .../Trichodesmium/SubDir2
 *    
 *  The idea is that when trying to determine what class a image really is we look
 *  at the first sub-dir name in the path.  We may also have to deal with seq-num's 
 *  as part of the name,  in that case we strip the _ and following numbers from the 
 *  name to get the correct class name.
 ****************************************************************************************/
MLClassPtr  OurNeighbors::DetermineClassFromFileName (const  KKStr&  fileName)
{
	KKStr filename_copy = fileName;

	// If there are no path separator characters('\'  or  '/')  characters in name
	// then we will not be able to determine the class.
	
  auto x = osLocateFirstSlashChar (filename_copy);
	if  (!x  ||  (x.value () < 1))
    return  mlClasses->GetUnKnownClass ();

  KKStr  className = filename_copy.SubStrPart (0, x.value () - 1);
  
  // now lets get rid of any possible trailing seq number.
  // We are assuming that a underscore{"_") character separates the calcs name from the seq number.
  // So if there is an underscore character,  and all the characters to the right of it are
  // underscore characters,  then we will remove the underscore and the following numbers.
	x = className.LocateLastOccurrence ('_');
  if  (x)
  {
    // Now lets eliminate any sequence number in name
    // We are assuming that a underscore{"_") character separates the class name from the seq number.
    // So if there is an underscore character, and all the characters to the right of it are
    // numeric characters, then we will remove the underscore and the following numbers.

    kkuint32  y = x.value () + 1;

    bool  allFollowingCharsAreNumeric = true;
    while  ((y < className.Len ()) &&  (allFollowingCharsAreNumeric))
    {
      char  ch = className[y];
      allFollowingCharsAreNumeric = ((ch >= '0')  &&  (ch <= '9'));
      y++;
    }

    if  (allFollowingCharsAreNumeric)
    {
      className = className.SubStrPart (0, x.value () - 1);
    }
  }

  // Now that we have a string with the class name,  lets get a pointer 
  // to a mlClass object from mlClasses ,  if none there then we get 
  // to create a new class.
	MLClassPtr  mlClass = mlClasses->GetMLClassPtr (className);

	return  mlClass;
}  /* DetermineClassFromFileName */





kkint32  OurNeighbors::LastScanLine (const ImageFeaturesList&  images)  const
{
  kkint32  lastScanLine = 0;
  ImageFeaturesList::const_iterator  idx;
  for  (idx = images.begin ();  idx != images.end ();  idx++)
  {
    const ImageFeaturesPtr image = *idx;
    if  (image->SfCentroidRow () > lastScanLine)
      lastScanLine = (kkint32)(image->SfCentroidRow () + 0.5f);
  }

  return  lastScanLine;
}  /* LastScanLine */



double  Z_Score (double  sampleMeanNND, 
                 double  randomMeanNND,
                 double  randomStdDevNND,
                 kkint32   N
                )

{
  double  z_Score = double ((double (sampleMeanNND) - double (randomMeanNND))   /   (double (randomStdDevNND) / sqrt (double (N))));
  return  z_Score;
}



void	OurNeighbors::RandomReport (ImageFeaturesList&  images)
{
   double   allClassesMeanNNDAnyClass    = 0.0f;
   double   allClassesMeanStdDevAnyClass = 0.0f;

  ClassSummaryList  classSummaries (log);

  MLClassList::iterator  classIdx;

  VectorKKStr  zScoreSummaryLines;

  for  (classIdx = mlClasses->begin ();  classIdx != mlClasses->end ();  classIdx++)
  {
    MLClassPtr  mlClass = *classIdx;

    if  (fromPlankton  &&  (fromPlankton != mlClass))
      continue;

    double  randomMeanNND   = 0.0f;
    double  randomStdDevNND = 0.0f;
    double  realDataU2Stat  = 0.0f;
    double  sampleMeanNND   = 0.0f;
    double  sampleStdDevNND = 0.0f;
    double  sampleMaxDist   = 0.0f;
    double  sampleMinDist   = 0.0f;

    ImageFeaturesListPtr  imagesInClass = images.ExtractExamplesForAGivenClass (mlClass);
    if  (imagesInClass->QueueSize () > 0)
    {
      // We are going to make a list of images that has duplicate instances of 'ImageFeatures' objects 
      // This way when we Randomize the locations of each 'sfCentroidRow' and 'sfCentroidCol' we do not 
      // imapct on the original data.
      ImageFeaturesListPtr  imagesToRandomize = new ImageFeaturesList (*imagesInClass,
                                                                       true  // 'true means we want to own the data so new instances will be created.
                                                                      );

      LLoydsEntryListPtr  lloydsEntries =  DeriveAllLLoydsBins (*imagesToRandomize);
      {
        // We will now get mean and stdDev of nnd for this class
        NeighborList  neighbors (*imagesToRandomize, log);
        neighbors.FindNearestNeighbors (neighborType, mlClass);
        neighbors.CalcStatistics (sampleMeanNND, sampleStdDevNND, sampleMaxDist, sampleMinDist);
      }


      KKStr  randomReportFileName;

      if  (reportFileName.Empty ())
        randomReportFileName = "RandomDistanceReport";
      else
        randomReportFileName = osRemoveExtension  (reportFileName) + "_Random";

      randomReportFileName << "_" << mlClass->Name () << ".txt";

      ofstream  randomReport (randomReportFileName.Str ());

      randomReport << "Random Distribution Report" << endl 
                   << endl;

      randomReport << "Source Directory  [" << sourceRootDirPath   << "]" << endl;
      randomReport << "Class             [" << mlClass->Name () << "]" << endl;

      RandomNND  randomizeLocations (lastScanLine, 
                                     *imagesToRandomize, 
                                     numOfIterations, 
                                     numOfBuckets, 
                                     bucketSize, 
                                     randomReport, 
                                     log
                                    );  

      randomizeLocations.GenerateReport ();

      randomMeanNND   = randomizeLocations.NND_Mean ();
      randomStdDevNND = randomizeLocations.NND_StdDev ();
      realDataU2Stat  = randomizeLocations.RealDataU2Stat ();

      //double  sampleMeanNND   = 0.0f;
      //double  sampleStdDevNND = 0.0f;

      double  z_Score = Z_Score (sampleMeanNND, randomMeanNND, randomStdDevNND, imagesToRandomize->QueueSize ());

      randomReport << endl << endl << endl
                   << "Z-Score" << endl
                   << "=======" << endl
                   << endl    
                   << "SampleMeanNND   " << "\t" << sampleMeanNND    << endl
                   << "SampleStdDevNND " << "\t" << sampleStdDevNND  << endl
                   << "RandomMeanNND   " << "\t" << randomMeanNND    << endl
                   << "RandomStdDevNND " << "\t" << randomStdDevNND  << endl
                   << "------- Z-Score " << "\t" << z_Score          << endl
                   << endl;

      KKStr  zScoreSummaryLine = mlClass->Name () + "\t" +
                                  StrFormatDouble (sampleMeanNND,   "###,##0.00")  + "\t" +
                                  StrFormatDouble (sampleStdDevNND, "###,##0.00")  + "\t" +
                                  StrFormatDouble (randomMeanNND,   "###,##0.00")  + "\t" +
                                  StrFormatDouble (randomStdDevNND, "###,##0.00")  + "\t" +
                                  StrFormatDouble (z_Score,         "###,##0.000");

      zScoreSummaryLines.push_back (zScoreSummaryLine);

      // The new instance on 'ClassSummary' that is aboiut to be created will take ownership
      // of lloydsBins.
      classSummaries.PushOnBack (new ClassSummary (mlClass, lloydsEntries, (float)realDataU2Stat, (float)z_Score));

      delete  imagesToRandomize;  imagesToRandomize = NULL;
    }

    delete  imagesInClass;  imagesInClass = NULL;
  }

  if  (!fromPlankton)
  {
    LLoydsEntryListPtr  allClassesLLoydsEntries = DeriveAllLLoydsBins (images);

    // Create a report for all classes
    KKStr  randomReportFileName;
    if  (reportFileName.Empty ())
      randomReportFileName = "RandomDistanceReport_All.txt";
    else
      randomReportFileName = osRemoveExtension  (reportFileName) + "_Random_All.txt";

    ofstream randomReport (randomReportFileName.Str ());

    randomReport << "Source Directory  [" << sourceRootDirPath  << "]" << endl;
    randomReport << "Class             [" << "All"              << "]" << endl;

    {
      // Find the mean and stddev of Nearest Neighbor regardless of class.
      NeighborList  allClassesNeighbors (images, log);
      allClassesNeighbors.FindNearestNeighbors (NeighborType::AnyPlankton, fromPlankton);

      double  allClassesMinDistAnyClass    = 0.0f;
      double  allClassesMaxDistAnyClass    = 0.0f;

      allClassesNeighbors.CalcStatistics (allClassesMeanNNDAnyClass,
                                          allClassesMeanStdDevAnyClass, 
                                          allClassesMinDistAnyClass,
                                          allClassesMaxDistAnyClass
                                         );
    }


    RandomNND  randomizeLocations (lastScanLine, 
                                   images, 
                                   numOfIterations, 
                                   numOfBuckets, 
                                   bucketSize, 
                                   randomReport, 
                                   log
                                  );

    randomizeLocations.GenerateReport ();

    // All classes Z-Score
    double  allClassesRandomMeanNND   = randomizeLocations.NND_Mean   ();
    double  allClassesRandomStdDevNND = randomizeLocations.NND_StdDev ();
    double  allClassesRealDataU2Stat  = randomizeLocations.RealDataU2Stat ();
    double  z_Score = Z_Score (allClassesMeanNNDAnyClass, allClassesRandomMeanNND, allClassesRandomStdDevNND, images.QueueSize ());

    KKStr  zScoreSummaryLine = KKStr ("All-Classes") + "\t" +
                                StrFormatDouble (allClassesMeanNNDAnyClass,     "###,##0.00")  + "\t" +
                                StrFormatDouble (allClassesMeanStdDevAnyClass,  "###,##0.00")  + "\t" +
                                StrFormatDouble (allClassesRandomMeanNND,       "###,##0.00")  + "\t" +
                                StrFormatDouble (allClassesRandomStdDevNND,     "###,##0.00")  + "\t" +
                                StrFormatDouble (z_Score,                       "###,##0.00");

    zScoreSummaryLines.push_back (zScoreSummaryLine);


    randomReport << endl << endl << endl
                 << "Z-Score" << endl
                 << "=======" << endl
                 << endl    
                 << "SampleMeanNND   " << "\t" << allClassesMeanNNDAnyClass     << endl
                 << "SampleStdDevNND " << "\t" << allClassesMeanStdDevAnyClass  << endl
                 << "RandomMeanNND   " << "\t" << allClassesRandomMeanNND       << endl
                 << "RandomStdDevNND " << "\t" << allClassesRandomStdDevNND     << endl
                 << "------- Z-Score " << "\t" << z_Score                       << endl
                 << endl;

    classSummaries.PushOnBack (new ClassSummary (MLClass::CreateNewMLClass (KKStr ("AllClasses")), 
                                                 allClassesLLoydsEntries, 
                                                 (float)allClassesRealDataU2Stat, 
                                                 (float)z_Score
                                                )
                              );
  }

  {
    // Z-Score Summary Report
    KKB::kkuint32  x;

    *report << std::endl << std::endl
      << "Z-Score Summary By Class" << std::endl
            << std::endl
            << "ClassName"  << "\t"  << "SampleMean"  << "\t"  << "SampleStdDev" << "\t" << "RandomMean" << "\t" << "RandomStdDev" << "\t" << "Z-Score" << std::endl
            << "========="  << "\t"  << "=========="  << "\t"  << "============" << "\t" << "==========" << "\t" << "============" << "\t" << "=======" << std::endl;

    for  (x = 0;  x < zScoreSummaryLines.size ();  x++)
      *report << zScoreSummaryLines[x] << std::endl;
  }

  *report << endl << endl << endl;
  classSummaries.SummaryReport (*report);

  *report << endl << endl << endl;
  classSummaries.SpatialOverlapReport (*report);

  classSummaries.SaveLLoydsBinsData (lloydsBinsFileName, sourceRootDirPath, lastScanLine, baseLLoydsBinSize);
                                           
}  /* RandomReport */



void	OurNeighbors::LookForNeighbors ()
{
	ImageFeaturesListPtr  currentImageFeatures = NULL;
	KKStr                relativeDir;

	log.Level (10) << "OurNeighbors::LookForNeighbors" << endl;


	/*
	 * create an image feature list from the source directory that corresponds to the
	 * current locations of the actual image files. Where possible, the feature data 
	 * file will be used. However, if an image has been moved it's features will have
	 * to be recalculated (which is handled by the function call) and we'll have to
	 * look in the origImageFeatures list for the original predicted class. We must do 
	 * this since the predicted class for an image file should NEVER change between
	 * classification runs.
	 */
  FeatureFileIOPices::Driver ()->LoadInSubDirectoryTree
                                 (PicesFVProducerFactory::Factory (&log),
                                  sourceRootDirPath,
                                  *mlClasses,
                                  false,           // useDirectoryNameForClassName,
                                  DB (),
                                  cancelFlag,
                                  false,           // rewiteRootFeatureFile
                                  log
                                 );

  currentImageFeatures->FixSipperFileScanLineAndColFields ();
  lastScanLine = LastScanLine (*currentImageFeatures);
  {
    // Make sure Class name matches subdirectory that Example was found in.
    ImageFeaturesList::iterator  idx;
    for  (idx = currentImageFeatures->begin ();  idx != currentImageFeatures->end ();  idx++)
    {
      ImageFeaturesPtr image = *idx;
      MLClassPtr  mlClass = DetermineClassFromFileName (image->ExampleFileName ());
      if  (mlClass)
        image->MLClass (mlClass);
    }
  }

  if  (excludedClasses)  
  {
    if  (excludedClasses->QueueSize () > 0)
      RemoveExcludedClasses (currentImageFeatures);
  }

  //if  (randomizeLocations)
  //  RandomizeLocations (*currentImageFeatures);

  if  (!fromPlanktonName.Empty ())
  {
    fromPlankton = mlClasses->LookUpByName (fromPlanktonName);
    if  (!fromPlankton)
    {
      log.Level (-1) << endl
                     << endl
                     << endl
                     << "LookForNeighbors     ****** ERROR *******" << endl
                     << endl
                     << "No images that are of PlanktonName[" << fromPlanktonName << "]" << endl
                     << endl;
      osWaitForEnter ();
      exit (-1);
    }
  }

  // We will now build Neighbor List
  NeighborList  neighbors (*currentImageFeatures, log);
  neighbors.FindNearestNeighbors (neighborType, fromPlankton);


  double  allClassesMeanNND    = 0.0f;
  double  allClassesMeanStdDev = 0.0f;
  double  allClassesMinDist    = 0.0f;
  double  allClassesMaxDist    = 0.0f;

  neighbors.CalcStatistics (allClassesMeanNND,
                            allClassesMeanStdDev, 
                            allClassesMinDist,
                            allClassesMaxDist
                           );



  if  (fromPlankton)
    neighbors.ReportClassRowRestricted (mlClasses, *report, fromPlankton);
  else
    neighbors.ReportClassRow (mlClasses, *report);

  neighbors.ReportClassNeighbor (mlClasses, *report);

  if  (randomizeLocations)
    RandomReport (*currentImageFeatures);

	log.Level (10) << "OurNeighbors::LookForNeighbors   Exiting"  << endl;
}  /* LookForNeighbors */



void	OurNeighbors::RemoveExcludedClasses (ImageFeaturesListPtr&  examples)
{
   bool  keepClass = true;
   MLClassPtr  oldClass = NULL;

   examples->SortByClass ();

   ImageFeaturesListPtr  examplesToKeep = new ImageFeaturesList (examples->FileDesc (), 
                                                                 true // true = We will own images,
                                                                );
   examples->Owner (false);

   ImageFeaturesList::iterator  idx;

   for  (idx = examples->begin ();  idx != examples->end ();  idx++)
   {
     ImageFeaturesPtr  i = *idx;
     if  (oldClass != i->MLClass ())
     {
       oldClass = i->MLClass ();
       keepClass = (excludedClasses->LookUpByName (oldClass->Name ()) == NULL);
     }

     if  (keepClass)
     {
       examplesToKeep->PushOnBack (i);
     }
     else
     {
       delete  i;  
     }
   }

   delete  examples;  
   examples = examplesToKeep;
}  /* RemoveExcludedClasses */



LLoydsEntryListPtr   OurNeighbors::DeriveAllLLoydsBins (const ImageFeaturesList&  examples)
{
  kkint32  lloydsBinSize = baseLLoydsBinSize;
  kkint32  maxLLoydsBinSize = lastScanLine / 4;

  LLoydsEntryListPtr  lloydsEntries = new LLoydsEntryList (log);

  while  (lloydsBinSize <= maxLLoydsBinSize)
  {
    LLoydsEntryPtr   lloydsBins = DeriveLLoydsBins (examples, lloydsBinSize);
    lloydsEntries->PushOnBack (lloydsBins);

    lloydsBinSize += baseLLoydsBinSize;
  }

  return  lloydsEntries;
}  /* DeriveAllLLoydsBins */



LLoydsEntryPtr  OurNeighbors::DeriveLLoydsBins (const ImageFeaturesList&  examples,
                                                kkint32                   lloydsBinSize
                                               )
{
  double  lloydsIndex = 0.0;

  kkint32  numLLoydsBins = kkint32 (lastScanLine + lloydsBinSize - 1)  / kkint32 (lloydsBinSize);

  VectorIntPtr  lloydsBins = new VectorInt (numLLoydsBins, 0);

  ImageFeaturesList::const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const ImageFeaturesPtr i = *idx;
    kkint32 lloydsBin =  kkint32 (i->SfCentroidRow () / double (lloydsBinSize));
    if  (lloydsBin >= numLLoydsBins)
    {
      // This can not happen; but if it does; then I must of screwed up the programming.
      log.Level (-1) << endl << endl << endl
                     << "OurNeighbors::DeriveLLoydsBins       **** ERROR ****" << endl
                     << endl
                     << "              An invalid lloydsBin[" << lloydsBin << "] was derived." << endl
                     << "              must be in range of  [0 - " << (numLLoydsBins - 1) << "]" << endl
                     << endl;

      lloydsBin = numLLoydsBins - 1;
    }

    (*lloydsBins)[lloydsBin]++;
  }

  lloydsIndex = LLoydsIndexOfPatchiness (*lloydsBins);

  return  new LLoydsEntry (lloydsBinSize, lloydsBins, lloydsIndex);
}  /* DeriveLLoydsBins */





kkint32  main (kkint32  argc,
               char**   argv
              )
{
  OurNeighbors  nearestNeighborReport;
  nearestNeighborReport.InitalizeApplication (argc, argv);
  if  (!nearestNeighborReport.Abort ())
    nearestNeighborReport.LookForNeighbors ();
}
