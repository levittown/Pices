#include "FirstIncludes.h"

#include <stdlib.h>
#include <fstream>
#include <memory>
#include <iostream>
#include <ostream>
#include <queue>
#include <sstream>
#include <strstream>
#include <string>
#include <vector>
#include <windows.h>

#include "MemoryDebug.h"

using namespace  std;


#include "KKBaseTypes.h"
#include "BMPImage.h"
#include "HTMLReport.h"
#include "OSservices.h"
#include "Raster.h"
#include "KKStr.h"
using namespace  KKB;


#include "Classifier2.h"
#include "ConfusionMatrix2.h"
#include "CrossValidation.h"
#include "DuplicateImages.h"
#include "FeatureFileIO.h"
#include "FeatureFileIOPices.h"
#include "FeatureNumList.h"
#include "ImageFeatures.h"
#include "MLClass.h"
#include "PicesVariables.h"
#include "TrainingProcess2.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;


#include "PicesTrainingConfiguration.h"
#include "PicesFVProducer.h"
using namespace  MLL;


#include  "ClassCountSearch.h"



class  ClassCountSearch::ClassStats
{
public:
  ClassStats (MLClassPtr  _mlClass,
              double      _truePositives,
              double      _trueNegatives,
              double      _falsePositives,
              double      _falseNegatives
             )
  {
    mlClass     = _mlClass;
    truePositives  = _truePositives;
    trueNegatives  = _trueNegatives;
    falsePositives = _falsePositives;
    falseNegatives = _falseNegatives;
  }

  // http://www.cs.rpi.edu/~leen/misc-publications/SomeStatDefs.html


  double  FalsePositiveRatio ()  const
  {
    double  denominator = truePositives + falseNegatives;
    if  (denominator== 0.0)
      return 0.0;
    else
      return  falsePositives / denominator;
  }


  double  ClassCount ()  const
  {
    return  truePositives + falseNegatives;
  }



  double  Sensitivity ()  const
  {
    double  denominator = truePositives + falseNegatives;
    if  (denominator == 0.0)
      return 0.0;
    else
      return truePositives / denominator;
  }


  double  Type1Error ()  const  
  {
    double  denominator = falsePositives + trueNegatives;
    if  (denominator == 0.0)
      return 0.0;
    else
      return  falsePositives / denominator;
  }


  double  Type2Error ()  const  
  {
    double  denominator = truePositives + falseNegatives;
    if  (denominator == 0.0)
      return 0.0;
    else
      return  falseNegatives / denominator;
  }

  MLClassPtr  mlClass;

  double  truePositives;
  double  trueNegatives;
  double  falsePositives;
  double  falseNegatives;
};

typedef  ClassCountSearch::ClassStats*  ClassStatsPtr;



class  ClassCountSearch::ClassStatsList: KKQueue <ClassStats>
{
public:
  ClassStatsList (): KKQueue <ClassStats> ()
  {}


  void  Push (MLClassPtr  _mlClass,
              double      _truePositives,
              double      _trueNegatives,
              double      _falsePositives,
              double      _falseNegatives
             )
  {
    PushOnBack (new ClassStats (_mlClass, _truePositives, _trueNegatives, _falsePositives, _falseNegatives));
  }



  ClassStatsPtr  PopLargestType1Error ()
  {
    if  (size () < 1)
      return NULL;

    ClassStatsPtr  largestValue = NULL;

    iterator  idx;
    iterator  largestIdx;

    for  (idx = begin ();  idx != end ();  ++idx)
    {
      ClassStatsPtr  p = *idx;
      if  (largestValue == NULL)
      {
        largestValue = p;
        largestIdx = idx;
      }

      else if  (p->Type1Error () > largestValue->Type1Error ())
      {
        largestIdx = idx;
        largestValue = p;
      }
    }

    erase (largestIdx);
    return  largestValue;
  }  /* PopLargestType1Error */



  ClassStatsPtr  PopLargestFalsePositives ()
  {
    if  (size () < 1)
      return NULL;

    ClassStatsPtr  largestValue = NULL;

    iterator  idx;
    iterator  largestIdx;

    for  (idx = begin ();  idx != end ();  ++idx)
    {
      ClassStatsPtr  p = *idx;
      if  (largestValue == NULL)
      {
        largestValue = p;
        largestIdx = idx;
      }

      else if  (p->falsePositives > largestValue->falsePositives)
      {
        largestIdx = idx;
        largestValue = p;
      }
    }

    erase (largestIdx);
    return  largestValue;
  }  /* PopLargestFalsePositives */



  ClassStatsPtr  PopSmallestCount ()
  {
    if  (size () < 1)
      return NULL;

    ClassStatsPtr  s = NULL;

    iterator  idx;
    iterator  smallestIdx;

    ClassStatsPtr  smallestValue = NULL;

    for  (idx = begin ();  idx != end ();  ++idx)
    {
      ClassStatsPtr  p = *idx;
      if  (smallestValue == NULL)
      {
        smallestValue = p;
        smallestIdx = idx;
      }

      else if  (p->ClassCount () < smallestValue->ClassCount ())
      {
        smallestIdx = idx;
        smallestValue = p;
      }
    }

    erase (smallestIdx);
    return  smallestValue;
  }  /* PopLargestType1Error */



  ClassStatsPtr  PopLeastSensitive ()
  {
    if  (size () < 1)
      return NULL;

    ClassStatsPtr  s = NULL;

    iterator  idx;
    iterator  smallestIdx;

    ClassStatsPtr  smallestValue = NULL;

    for  (idx = begin ();  idx != end ();  ++idx)
    {
      ClassStatsPtr  p = *idx;
      if  (smallestValue == NULL)
      {
        smallestValue = p;
        smallestIdx = idx;
      }

      else if  (p->Sensitivity () < smallestValue->Sensitivity ())
      {
        smallestIdx = idx;
        smallestValue = p;
      }
    }

    erase (smallestIdx);
    return  smallestValue;
  }  /* PopLargestType1Error */

};  /* ClassStatsList */




typedef  ClassCountSearch::ClassStatsList*  ClassStatsListPtr;



// -s C:\TrainingApp\ClassifiedImages\hrs020409  -r Gradereport.txt

/******************************************************************************
 * command line parameters that have been used for testing purposes
 ******************************************************************************/

// -s "C:\TrainingApp\ClassifiedImages" -r "report.txt"

// -s C:\Temp\sennai_sorted   -gt C:\Temp\GroundTruth  -report  Sennai.txt

//  -s C:\Temp\hrs020503 -r c:\hrs020503_GradeReport.txt

// -s C:\temp\hrs020503_auto  -gt C:\temp\hrs020503_GroundTruth

// -s D:\TrainingApp\ExtractedImages\hrs020505_auto  -gt D:\TrainingApp\ExtractedImages\hrs020505_man

// -config WFS.cfg  -gt C:\PICES\ExtractedImages\hrs020505_man

// -config WFS_BinaryTuned -gt D:\TrainingApp\ExtractedImages\hrs020501_man

// -c Oil.cfg    -gt C:\Pices\TrainingLibraries\oil-test  -R c:\Temp\GradeReport.txt

// ClassCountSearch  -c ETP08_BFS  -gt <Specify Directory where ground truth oimages are>  -R <Name of report file>

// -c GulfOilBroad2_Discreate_Dual.cfg  -gt E:\Pices\TrainingLibraries\GulfOilBroad2_Validation -r C:\Temp\GulfOilBroad2_Validation_Graded.txt

// -c USF2_MFS



ClassCountSearch::ClassCountSearch ():
  PicesApplication     (),
  cancelFlag           (false),
  groundTruthDirName   (),
  groundTruth          (NULL),
  groundTruthClasses   (NULL),
  mlClasses            (new MLClassList ()),
  report               (NULL),
  report1Type1Errors   (NULL),
  reportFileName       (),
  trainExamples        (NULL),
  trainExamplesClasses (NULL)
{
}




/******************************************************************************
 * Destructor
 ******************************************************************************/
ClassCountSearch::~ClassCountSearch ()
{
  if  (report)
  {
    report->close ();
    delete  report;
    report = NULL;
  }

  if  (report1Type1Errors)
  {
    report1Type1Errors->close ();
    delete  report1Type1Errors;
    report1Type1Errors = NULL;
  }

  delete  groundTruth;           groundTruth          = NULL;
  delete  groundTruthClasses;    groundTruthClasses   = NULL;
  delete  trainExamples;         trainExamples        = NULL;
  delete  trainExamplesClasses;  trainExamplesClasses = NULL;
  //delete  mlClass;
}


void  ClassCountSearch::InitalizeApplication (kkint32 argc,
                                              char**  argv
                                             )
{
  ConfigRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  fileDesc = FeatureFileIOPices::NewPlanktonFile ();

  osRunAsABackGroundProcess ();

  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (argc < 2)
  {
    log.Level (-1) << endl << endl << endl
                   << "No parameters specified." << endl
                   << endl;
    Abort (true);
    DisplayCommandLineParameters ();
    return;
  }

  if  (configFileName.Empty ())
  {
    log.Level (-1) << endl << endl << endl
                   << "Configuration file was not specified."  << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  if  (!groundTruthDirName.Empty ()  &&  (!osValidDirectory (groundTruthDirName)))
  {
    log.Level (-1) << endl << endl << endl
                   << "-GroundTruth directory[" <<  groundTruthDirName << "] is not a valid directory."  << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  fileDesc = config->FileDesc ();

  if  (reportFileName.Empty ())
  {
    KKStr  reportDir = osAddSlash (PicesVariables::ReportDir ()) + "ClassCountSearch";
    KKB::osCreateDirectoryPath (reportDir);
    reportFileName = osAddSlash (reportDir) + osGetRootNameOfDirectory (configFileName) + "_ClassCountSearch.txt";
  }

  reportFileName  = osCreateUniqueFileName (reportFileName);
  report = new ofstream (reportFileName.Str ());

  KKStr  type1ErrorReportName = KKB::osRemoveExtension (reportFileName) + "_Sensitivity.txt";
  report1Type1Errors = new ofstream (type1ErrorReportName.Str ());
  
  *report << endl;
  PrintStandardHeaderInfo (*report);
  *report << "------------------------------------------------------------------------" << endl;
  *report << "Report File Name"       << "\t" << reportFileName       << endl;
  *report << "Config File Name"       << "\t" << configFileName       << endl;
  *report << "Ground Truth Directory" << "\t" << groundTruthDirName   << endl;
  if  (config)
  {
    *report << "Model Type"     << "\t" << config->ModelTypeStr          ()  << endl;
    *report << "SVM Parameters" << "\t" << config->ModelParameterCmdLine ()  << endl;
    if  (config->OtherClass ())
      *report << "Other Class"  << "\t" << config->OtherClass ()->Name   ()  << endl;
  }

  *report << "------------------------------------------------------------------------" << endl;
  *report << endl;


  *report1Type1Errors << endl;
  PrintStandardHeaderInfo (*report1Type1Errors);
  *report1Type1Errors << "------------------------------------------------------------------------" << endl;
  *report1Type1Errors << "Report File Name"       << "\t" << reportFileName      << endl;
  *report1Type1Errors << "Config File Name"       << "\t" << configFileName      << endl;
  *report1Type1Errors << "Ground Truth Directory" << "\t" << groundTruthDirName  << endl;
  if  (config)
  {
    *report1Type1Errors << "Model Type"      << "\t" << config->ModelTypeStr          ()  << endl;
    *report1Type1Errors << "SVM Parameters"  << "\t" << config->ModelParameterCmdLine ()  << endl;
    if  (config->OtherClass ())
      *report1Type1Errors << "Other Class"   << "\t" << config->OtherClass ()->Name   ()  << endl;
  }
  *report1Type1Errors << "------------------------------------------------------------------------" << endl;
  *report1Type1Errors << endl;
}





/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  ClassCountSearch::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                 const KKStr&  parmValue
                                                )
{
  bool  parmValid = true;
  if  (parmSwitch.EqualIgnoreCase ("-R")  ||  parmSwitch.EqualIgnoreCase ("-REPORT"))
  {
    reportFileName = parmValue;
  }

  else if  (parmSwitch.EqualIgnoreCase ("-GROUNDTRUTH") || parmSwitch.EqualIgnoreCase ("-GT"))
  {
    groundTruthDirName = parmValue;
  }

  else
  {
    parmValid = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }

  return  parmValid;
}  /* ProcessCmdLineParameter */




/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   ClassCountSearch::DisplayCommandLineParameters ()
{ 
  log.Level (0) << "ClassCountSearch  -r <xxx>  -gt <xxxx>  -c <config file>"                << endl;
  log.Level (0)                                                                              << endl;
  log.Level (0) << "    -config  <Configuration File Name.>"                                 << endl;
  log.Level (0) << "             Works in conjunction with '-GroundTruth'. It will build"    << endl;
  log.Level (0) << "             a classifier using the training data specified in the"      << endl;
  log.Level (0) << "             configuration file.   It will then classify all the images" << endl;
  log.Level (0) << "             in the ground truth and grade.  It will build and grade"    << endl;
  log.Level (0) << "             for each level of hierarchy."                               << endl;
  log.Level (0)                                                                              << endl;
  log.Level (0)                                                                              << endl;
  log.Level (0) << "    -r       <Report File Name>,  Defaults to Command Line."             << endl;
  log.Level (0)                                                                              << endl;
  log.Level (0) << "    -GroundTruth  <Sub-Directory where ground truth images are>"         << endl;
}

// ClassCountSearch -gt D:\Pices\TrainingLibraries\oil  -s C:\temp\oil -r C:\temp\oil\OilGradingReport.txt





/******************************************************************************
 ******************************************************************************/
void  ClassCountSearch::Main ()
{
  log.Level (10) << "ClassCountSearch::Grade   Loading Ground Truth"  << endl;

  if  (!groundTruthDirName.Empty ())
  {
    groundTruth = FeatureFileIOPices::Driver ()->LoadInSubDirectoryTree 
                        (fvFactoryProducer,
                         groundTruthDirName, 
                         *mlClasses, 
                         true,             // true = useDirectoryNameForClassName
                         DB (),
                         cancelFlag, 
                         false,            // false = don't rewiteRootFeatureFile 
                         log
                        );
    {
      DuplicateImages  dupDetector (groundTruth, log);
      if  (dupDetector.DuplicatesFound ())
        dupDetector.ReportDuplicates (*report);
    }

    groundTruthClasses = groundTruth->ExtractListOfClasses ();
  }

  DateTime  latestImageTimeStamp;
  bool      changesMadeToTrainingLibraries = false;

  {
    KKB::DateTime  latestImageTimeStamp;
    trainExamples = config->LoadFeatureDataFromTrainingLibraries (latestImageTimeStamp, changesMadeToTrainingLibraries, cancelFlag, log);
    if  (!trainExamples)
    {
      log.Level (-1) << endl
        << "ClassCountSearch::Main   ***ERROR***   Failed to load training examples for model: " << config->FileName () << endl << endl;
      exit (-1);
    }
  }

  trainExamplesClasses = trainExamples->ExtractListOfClasses ();
  trainExamplesClasses->SortByName ();


  KKStr  l1, l2, l3;
  trainExamplesClasses->ExtractThreeTitleLines (l1, l2, l3);

  *report1Type1Errors << ""       << "\t" << ""      << "\t" << ""         << "\t" << ""        << "\t" << ""         << "\t" << "Class"   << "\t" << "Class"      << "\t" << l1 << endl;
  *report1Type1Errors << "Error"  << "\t" << "Class" << "\t" << "Number"   << "\t" << "Number"  << "\t" << ""         << "\t" << "Wtd"     << "\t" << "With >"     << "\t" << l2 << endl;
  *report1Type1Errors << "Type"   << "\t" << "Count" << "\t" << "Examples" << "\t" << "Ignored" << "\t" << "Accuracy" << "\t" << "Equally" << "\t" << "Type1Error" << "\t" << l3 << endl;

  combinationsToTry.push (new MLClassList (*trainExamplesClasses));

  while  (combinationsToTry.size () > 0)
  {
    MLClassListPtr  nextClassCombo = combinationsToTry.front ();
    combinationsToTry.pop ();
    ProcessClassCombo (nextClassCombo);
    delete  nextClassCombo;
    nextClassCombo = NULL;
  }

}  /* Main */




ClassStatsListPtr  ClassCountSearch::ComputeClassStatsList (ConfusionMatrix2Ptr  cm)
{
  ClassStatsListPtr  stats = new ClassStatsList ();

  const MLClassList&  classes = cm->MLClasses ();

  MLClassList::const_iterator  idx;
  for  (idx = classes.begin ();  idx != classes.end ();  ++idx)
  {
    MLClassPtr  ic = *idx;

    double  truePositives  = 0.0;
    double  trueNegatives  = 0.0;
    double  falsePositives = 0.0;
    double  falseNegatives = 0.0;

    cm->ComputeFundamentalStats (ic, truePositives, trueNegatives, falsePositives, falseNegatives);

    // http://www.cs.rpi.edu/~leen/misc-publications/SomeStatDefs.html
    
    stats->Push (ic, truePositives, trueNegatives, falsePositives, falseNegatives);
  }

  return   stats;
}  /* ComputeClassStatsList */



void  ClassCountSearch::ProcessClassCombo (MLClassListPtr  classes)
{
  log.Level (10) << "ClassCountSearch::ProcessClassCombo" << endl;

  classes->SortByName ();
 
  *report << endl << endl << endl
          << "*****************************************************************************************************************************************************************" << endl
          << "*****************************************************************************************************************************************************************" << endl;

  *report << "ClassCount" << "\t" << classes->QueueSize ()         << endl;
  *report << "Classes"    << "\t" << classes->ToTabDelimitedStr () << endl;

  kkint32 numExamples = 0;
  kkint32 numIgnored  = 0;

  ConfusionMatrix2Ptr cm = GradeClassList (classes, numExamples, numIgnored);
  cm->PrintConfusionMatrixTabDelimited (*report);
  cm->PrintTrueFalsePositivesTabDelimited (*report);
  *report << endl;

  ClassStatsListPtr  stats =  ComputeClassStatsList (cm);

  //ClassStatsPtr  nextCandidate = stats->PopLargestFalsePositives ();
  //ClassStatsPtr  nextCandidate = stats->PopSmallestCount ();
  ClassStatsPtr  nextCandidate = stats->PopLeastSensitive ();

  ostringstream	 type2Errors;
  ostringstream	 falsePositivesStr;
  ostringstream	 sensitivityStr;

  *report1Type1Errors << "Type1Error"     << "\t" << classes->QueueSize () << "\t" << numExamples << "\t" << numIgnored << "\t" << cm->Accuracy () << "\t" << cm->AccuracyNorm () << "\t" << nextCandidate->mlClass->Name ();
  type2Errors         << "Type2Error"     << "\t" << classes->QueueSize () << "\t" << numExamples << "\t" << numIgnored << "\t" << cm->Accuracy () << "\t" << cm->AccuracyNorm () << "\t" << nextCandidate->mlClass->Name ();
  falsePositivesStr   << "FalsePositives" << "\t" << classes->QueueSize () << "\t" << numExamples << "\t" << numIgnored << "\t" << cm->Accuracy () << "\t" << cm->AccuracyNorm () << "\t" << nextCandidate->mlClass->Name ();
  sensitivityStr      << "FalsePositives" << "\t" << classes->QueueSize () << "\t" << numExamples << "\t" << numIgnored << "\t" << cm->Accuracy () << "\t" << cm->AccuracyNorm () << "\t" << nextCandidate->mlClass->Name ();

  MLClassList::iterator  idx;
  for  (idx = trainExamplesClasses->begin ();  idx != trainExamplesClasses->end ();  ++idx)
  {
    MLClassPtr  ic = *idx;
    if  (classes->PtrToIdx (ic) < 0)
    {
      *report1Type1Errors << "\t" << "********";
      type2Errors         << "\t" << "********";
      falsePositivesStr   << "\t" << "********";
      sensitivityStr      << "\t" << "********";
    }
    else
    {
      double  tp, tn, fp, fn;
      cm->ComputeFundamentalStats (ic, tp, tn, fp, fn);

      double  type1Error = 0.0;
      double  denominator = fp + tn;
      if  (denominator == 0.0)
        type1Error = 0.0;
      else
        type1Error = fp / denominator;

      double  type2Error = 0;
      denominator = (fn + tp);
      if  (denominator != 0.0)
        type2Error = fn / denominator;

      double  sensitivity = 0.0;
      denominator = tp + fn;
      if  (denominator != 0.0)
        sensitivity = tp / denominator;

      *report1Type1Errors << "\t" << type1Error;
      type2Errors         << "\t" << type2Error;
      falsePositivesStr   << "\t" << fp;
      sensitivityStr      << "\t" << sensitivity;
    }
  }

  *report1Type1Errors << endl;
  type2Errors         << endl;
  falsePositivesStr   << endl;
  *report1Type1Errors << type2Errors.str ();
  *report1Type1Errors << falsePositivesStr.str ();
  *report1Type1Errors << sensitivityStr.str ();

  if  (classes->QueueSize () > 3)
  {
    MLClassListPtr  nextClassList = new MLClassList (*classes);
    nextClassList->DeleteEntry (nextCandidate->mlClass);
    combinationsToTry.push (nextClassList);
  }

  delete  nextCandidate;  nextCandidate = NULL;
  delete  stats;          stats = NULL;
  delete  cm;             cm    = NULL;
}  /* ProcessClassCombo */



ConfusionMatrix2Ptr  ClassCountSearch::GradeClassList (MLClassListPtr  classes,
                                                       kkint32&        numExamples, 
                                                       kkint32&        numIgnored
                                                       )
{
  log.Level (10) << "ClassCountSearch::GradeClassList" << endl;

  numExamples = 0;
  numIgnored  = 0;

  classes->SortByName ();
  
  TrainingConfiguration2Ptr  specificConfig = new TrainingConfiguration2 (*config);

  specificConfig->EmptyTrainingClasses ();

  FeatureVectorListPtr  specificTrainExamples = fvFactoryProducer->ManufacturFeatureVectorList (false);
  FeatureVectorListPtr  specificGroundTruthExamples = NULL;

  if  (groundTruth)
    specificGroundTruthExamples = fvFactoryProducer->ManufacturFeatureVectorList (false);
  
  MLClassList::iterator  idx;
  for  (idx = classes->begin ();  idx != classes->end ();  ++idx)
  {
    MLClassPtr  ic = *idx;
    specificConfig->AddATrainingClass (ic);

    {
      FeatureVectorListPtr  trainExamplesThisClass = trainExamples->ExtractExamplesForAGivenClass (ic);
      specificTrainExamples->AddQueue (*trainExamplesThisClass);
      delete  trainExamplesThisClass;  trainExamplesThisClass = NULL;
    }

    if  (groundTruth)
    {
      FeatureVectorListPtr  groundTruthExamplesThisClass = groundTruth->ExtractExamplesForAGivenClass (ic);
      specificGroundTruthExamples->AddQueue (*groundTruthExamplesThisClass);
      delete  groundTruthExamplesThisClass;
      groundTruthExamplesThisClass = NULL;
    }
  }


  if  (!specificGroundTruthExamples)
  {
    FeatureVectorListPtr  stratified = specificTrainExamples->StratifyAmoungstClasses (10, log);
    delete  specificTrainExamples;
    specificTrainExamples = stratified;
    stratified = NULL;
  }

  CrossValidation  cv (specificConfig, 
                       specificTrainExamples,
                       classes,
                       10,                     // _numOfFolds,
                       false,                  // false = Not already normalized.
                       fileDesc,
                       log,
                       cancelFlag
                      );

  if  (specificGroundTruthExamples)
  {
    numExamples = specificGroundTruthExamples->QueueSize ();
    numIgnored  = groundTruth->QueueSize () - numExamples;
    cv.RunValidationOnly (specificGroundTruthExamples, NULL, log);
  }
  else
  {
    numExamples = specificTrainExamples->QueueSize ();
    numIgnored  = trainExamples->QueueSize () - numExamples;
    cv.RunCrossValidation (log);
  }

  ConfusionMatrix2Ptr  cm = cv.GiveMeOwnershipOfConfusionMatrix ();

  delete  specificTrainExamples;        specificTrainExamples       = NULL;
  delete  specificGroundTruthExamples;  specificGroundTruthExamples = NULL;
  delete  specificConfig;               specificConfig              = NULL;
  
  return cm;
}  /* GradeClassList */




void  main (int     argc,
            char**  argv
           )
{
  ClassCountSearch  app;
  app.InitalizeApplication (argc, argv);
  if  (!app.Abort ())
    app.Main ();
}
