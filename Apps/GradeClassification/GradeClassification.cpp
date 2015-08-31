#include "FirstIncludes.h"

#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

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
#include "DuplicateImages.h"
#include "FeatureFileIO.h"
#include "FeatureFileIOPices.h"
#include "FeatureNumList.h"
#include "MLClass.h"
#include "TrainingProcess2.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;


#include "DataBase.h"
#include "ImageFeatures.h"
using namespace  MLL;


#include  "GradeClassification.h"




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

// GradeClassification  -c ETP08_BFS  -gt <Specify Directory where ground truth oimages are>  -R <Name of report file>

// -c GulfOilBroad2_Discreate_Dual.cfg  -gt E:\Pices\TrainingLibraries\GulfOilBroad2_Validation -r C:\Temp\GulfOilBroad2_Validation_Graded.txt

GradeClassification::GradeClassification () :
  PicesApplication    (),
  cancelFlag          (false),
  groundTruthDirName  (),
  groundTruth         (NULL),
  html                (NULL),
  htmlFileName        (),
  sourceRootDirPath   (),
  mlClass             (NULL),
  mlClasses           (new MLClassList ()),
  report              (NULL),
  reportFile          (NULL),
  reportFileName      ()
{
}




/******************************************************************************
 * Destructor
 ******************************************************************************/
GradeClassification::~GradeClassification ()
{
  if  (reportFile)
  {
    reportFile->close ();
    delete  reportFile;
  }

  delete  html;

  SummaryList::iterator  idx;
  for  (idx = resultsSummary.begin ();  idx != resultsSummary.end ();  idx++)
  {
    delete  idx->confussionMatrix;
    idx->confussionMatrix = NULL;
  }

  delete  groundTruth;
  groundTruth = NULL;
  //delete  mlClass;
}




void  GradeClassification::InitalizeApplication (kkint32 argc,
                                                 char**  argv
                                                )
{
  PicesApplication::InitalizeApplication (argc, argv);
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  mlClass = MLClass::CreateNewMLClass ("UnKnown");

  if  (groundTruthDirName.Empty ())
  {
    log.Level (-1) << endl << endl << endl
                   << "-GroundTruth directory was not specified. This is a mandatory parameter." << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  if  (!osValidDirectory (groundTruthDirName))
  {
    log.Level (-1) << endl << endl << endl
                   << "-GroundTruth directory[" <<  groundTruthDirName << "] is not a valid directory."  << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  if  (configFileName.Empty ()  &&  sourceRootDirPath.Empty ())
  {
    log.Level (-1) << endl << endl << endl
                   << "You mst specify either a configuration (-Config)  or  Source-Directory(-s)"  << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  if  ((!configFileName.Empty ())  &&  (!sourceRootDirPath.Empty ()))
  {
    log.Level (-1) << endl << endl << endl
                   << "You can not specify both '-Config' and '-s' parameters."  << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  if  (reportFileName.Empty ())
    reportFileName = osGetRootNameOfDirectory (groundTruthDirName) + "_GradingReport.txt";
  reportFileName  = osCreateUniqueFileName (reportFileName);
  reportFile = new ofstream (reportFileName.Str ());
  report = reportFile;
  
  if  (htmlFileName.Empty ())
    htmlFileName = osRemoveExtension (reportFileName) + ".html";
  html = new HTMLReport (htmlFileName, "Grading Report", HTMLReport::AlignmentType::Center);

  *report << endl;

  PrintStandardHeaderInfo (*report);
  *report << endl;
  *report << "------------------------------------------------------------------------" << endl;
  *report << "Report File Name        [" << reportFileName        << "]."  << endl;
  *report << "HTML File Name          [" << htmlFileName          << "]."  << endl;
  *report << "Source Root Directory   [" << sourceRootDirPath     << "]."  << endl;
  *report << "Ground Truch Directory  [" << groundTruthDirName    << "]."  << endl;
  *report << "------------------------------------------------------------------------" << endl;
  *report << endl;


  *html << endl
        << "<h1>Run Time Parameters</h1>" << endl
        << "<table align=\"center\" border=\"2\" cellpadding=\"4\">"                             << endl
        << "<caption>Run Time Parameters</caption>" << endl
        << "<tr><td>Run Time</td>"                << "<td>" << osGetLocalDateTime ()    << "</td></tr>" << endl
        << "<tr><td>Ground Truch Directory</td>"  << "<td>" << groundTruthDirName       << "</td></tr>" << endl
        << "<tr><td>Config File Name</td>"        << "<td>" << configFileName           << "</td></tr>" << endl;
 
  if  (config)
  {
    *html << "Model Type              [" << config->ModelTypeStr ()           << "]." << endl;
    *html << "SVM Parameters          [" << config->ModelParameterCmdLine ()  << "]." << endl;
    *html << "Num Hierarchial Levels  [" << config->NumHierarchialLevels  ()  << "]." << endl;
    if  (config->OtherClass ())
      *html << "Other Class             [" << config->OtherClass ()->Name ()    << "]." << endl;
  }

  *html << "<tr><td>Source Root Directory</td>"   << "<td>" << sourceRootDirPath        << "</td></tr>" << endl
        << "<tr><td>Report File Name</td>"        << "<td>" << reportFileName           << "</td></tr>" << endl
        << "<tr><td>HTML File Name</td>"          << "<td>" << htmlFileName             << "</td></tr>" << endl
        << "</table>" << endl
        << endl 
        << endl;

}  /* InitalizeApplication */







/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  GradeClassification::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                    const KKStr&  parmValue
                                                   )
{
  bool  validParm = true;

  if  ((parmSwitch.EqualIgnoreCase ("-R"))  ||  (parmSwitch.EqualIgnoreCase ("-REPORT")))
  {
    reportFileName = parmValue;
  }

  else if  ((parmSwitch.EqualIgnoreCase ("-SOURCEDIR"))        ||  
            (parmSwitch.EqualIgnoreCase ("-SOURCEDIRECTORY"))  ||
            (parmSwitch.EqualIgnoreCase ("-SD"))               ||
            (parmSwitch.EqualIgnoreCase ("-S"))
           )
  {  
    sourceRootDirPath = parmValue;
  }

  else if  ((parmSwitch.EqualIgnoreCase ("-HTML")) || (parmSwitch.EqualIgnoreCase ("-H")))
  {
    htmlFileName = parmValue;
  }

  else if  ((parmSwitch.EqualIgnoreCase ("-GROUNDTRUTH")) || (parmSwitch.EqualIgnoreCase ("-GT")))
  {
    groundTruthDirName = parmValue;
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
void   GradeClassification::DisplayCommandLineParameters ()
{ 
  DisplayCommandLineParameters ();
  cout << endl
       << "    -config     <FileName.>      Works in conjunction with '-GroundTruth'. It will"      << endl
       << "                                 build a classifier using the training data specified"   << endl
       << "                                 in the configuration file. It will then classify all"   << endl
       << "                                 the images in the ground truth and grade. It will"      << endl
       << "                                 build and grade for each level of hierarchy."           << endl
       << endl
       << "    -r          <FileName>       Report Filename; defaults to StdOut (cout)."            << endl
       << endl
       << "    -s          <Dir-Path>       Sub-Directory of images to grade; Specify this instead" << endl
       << "                                 of a config file. Images in this directory will be"     << endl
       << "                                 graded against the GroundTruth directory."              << endl
       << endl
       << "    -H          <File-Name>      File where a 'html' formated report will be written"    << endl
       << "                                 to. Will default to Report-Filename with '.html"        << endl
       << "                                 extension."                                             << endl
       << endl
       << "    -GroundTruth <Dir-Path>      Sub-Directory where ground-truth images will be"        << endl
       << "                                 stored. Directory name will indicate class."            << endl
       << endl;
}  /* DisplayCommandLineParameters */



// GradeClassification -gt D:\Pices\TrainingLibraries\oil  -s C:\temp\oil -r C:\temp\oil\OilGradingReport.txt





/******************************************************************************
 * Grade()
 * DESC: Builds a new data file in the root directory that reflects the currect 
 * location of all the images. This new data file is then compared to old one
 * and a grading report that indicates the number of classes correctly predicted
 * by the classifier is generated
 ******************************************************************************/
void  GradeClassification::Grade ()
{
  log.Level (10) << "GradeClassification::Grade   Loading Ground Truth"  << endl;

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
    {
      *html << "<br /><br /><br />"
            << "<h1>Duplicates Detected in Ground Truth</h1>" << endl
            << "<br />"
            << endl;
      dupDetector.ReportDuplicates (html->OStream ());
      *html << "<br /><br /><br />" << endl;
    }
  }

  if  (!configFileName.Empty ())
  {
    GradeUsingTrainingConfiguration ();
  }
  else
  {
    GradeSourceImagesAgainstGroundTruth ();
  }
}  /* Grade*/





void  GradeClassification::GradeSourceImagesAgainstGroundTruth ()
{
  log.Level (10) << "GradeClassification::GradeSourceImagesAgainstGroundTruth" << endl;

  // We were provided a GroundTruth directory and a SourceDirectory that is to be graded. We 
  // only need to load in both directories and f=grade the SourceDirectory against the GroundTruth
  // Directory.

  ImageFeaturesListPtr  sourceImages = FeatureFileIOPices::Driver ()->LoadInSubDirectoryTree 
                          (fvFactoryProducer,
                           sourceRootDirPath, 
                           *mlClasses,
                           true,              // true = useDirectoryNameForClassName
                           DB (),
                           cancelFlag, 
                           false,             // rewiteRootFeatureFile (Don't update root feature file)
                           log
                          );

  if  (!sourceImages)
  {
    log.Level (-1) << endl << endl << endl
                   << "GradedlClassification::GradeSourceImagesAgainstGroundTruth      ***ERROR***" << endl
                   << endl
                   << "               Could not load SourceImages[" << sourceRootDirPath << "]" << endl
                   << endl;
    Abort (true);
    return;
  }

  {
    DuplicateImages  dupDetector (sourceImages, log);
    if  (dupDetector.DuplicatesFound ())
    {
      *html << "<br /><br /><br />"
            << "<h1>Duplicates Detected in Source Images</h1>" << endl
            << "<br />"
            << endl;
      dupDetector.ReportDuplicates (html->OStream ());
      *html << "<br /><br /><br />" << endl;
    }
  }

  bool theyAreTheSame = true;
  ValidateThatBothListsHaveSameEntries (*groundTruth, *sourceImages, theyAreTheSame);

  GradeExamplesAgainstGroundTruth (sourceImages, groundTruth);
  
  ReportResults ();
  
  delete  sourceImages;
}  /* GradeSourceImagesAgainstGroundTruth */





void  GradeClassification::GradeUsingTrainingConfiguration ()
{
  log.Level (10) << "GradeClassification::GradeUsingTrainingConfiguration" << endl;

  delete  mlClasses;
  mlClasses = config->ExtractClassList ();

  bool  changesMadeToTrainingLibraries = false;

  KKB::DateTime  latestImageTimeStamp;

  log.Level (10) << "GradeUsingTrainingConfiguration  Loading Training Data." << endl;

  FeatureVectorListPtr  trainingData 
    = config->LoadFeatureDataFromTrainingLibraries (latestImageTimeStamp, changesMadeToTrainingLibraries, cancelFlag, log);
  if  (!trainingData)
  {
    log.Level (-1) << endl << endl << endl
                   << "GradedlClassification::GradeUsingTrainingConfiguration      ***ERROR***" << endl
                   << endl
                   << "               Could not load training data file Configuration File[" << configFileName << "]" << endl
                   << endl
                   << endl;
    Abort (true);
    return;
  }

  uint  maxLevelsOfHiearchy = config->NumHierarchialLevels ();
  uint  hierarchyLevel = 0;

  while  (hierarchyLevel < maxLevelsOfHiearchy)
  {
    log.Level (10) << "GradeUsingTrainingConfiguration    Hierarchy Level[" << hierarchyLevel << "]" << endl;

    TrainingConfiguration2Ptr  configThisLevel = config->GenerateAConfiguraionForAHierarchialLevel (hierarchyLevel, log);

    FeatureVectorListPtr  trainingDataThisLevel = trainingData->ExtractExamplesForHierarchyLevel (hierarchyLevel);
    FeatureVectorListPtr  groundTruthThisLevel  = groundTruth->ExtractExamplesForHierarchyLevel (hierarchyLevel);
    FeatureVectorListPtr  groundTruthThisLevelClassified  = new FeatureVectorList (*groundTruthThisLevel, true);

    KKStr  statusMessage;

    TrainingProcess2Ptr  trainer = TrainingProcess2::CreateTrainingProcessFromTrainingExamples
                             (configThisLevel,
                              trainingDataThisLevel,
                              false,      // false = Don't takeOwnershipOfTrainingExamples
                              false,      // false = features are NOT already normalized.
                              cancelFlag,
                              log
                             );

    {
      Classifier2 classifier (trainer, log);
      FeatureVectorList::iterator  idx;

      for  (idx = groundTruthThisLevelClassified->begin ();  idx != groundTruthThisLevelClassified->end ();  idx++)
      {
        FeatureVectorPtr  fv = *idx;
        MLClassPtr  ic = classifier.ClassifyAExample (*fv);
        fv->MLClass (ic);
      }
    }

    GradeExamplesAgainstGroundTruth (groundTruthThisLevelClassified, groundTruthThisLevel);

    delete  trainer;                         trainer                        = NULL;
    delete  groundTruthThisLevelClassified;  groundTruthThisLevelClassified = NULL;
    delete  groundTruthThisLevel;            groundTruthThisLevel           = NULL;
    delete  trainingDataThisLevel;           trainingDataThisLevel          = NULL;

    hierarchyLevel++;
  }

  ReportResults ();

  delete  trainingData;
}  /* GradeUsingTrainingConfiguration */



void  GradeClassification::ReportResults ()
{
  *report << endl << endl << endl
          << "Summary by Training and Prediction Hierarchy Levels" << endl
          << endl
          << "Train"  << "\t" << "Prediction" << "\t" << ""  << endl
          << "Level"  << "\t" << "Level"      << "\t" << "Acuracy" << endl;


  *html << "<h1>Summary of Results.</h1>" << endl
        << "<table  align=\"center\" border=\"2\" cellpadding=\"3\" cellspacing=\"0\" frame=\"box\"  summary=\"Summary of Results\">"     << endl
        << "  <thead>"                    << endl
        << "    <tr><th>Training<br />Level</th><th>Prediction<br />Level</th><th>Accuracy</th></tr>" << endl
        << "  </thead>" << endl
        << "  <tbody style=\"font-weight:normal; text-align:center; font-family:Courier\">"  << endl;


  SummaryList::iterator idx;
  for  (idx = resultsSummary.begin ();  idx != resultsSummary.end ();  idx++)
  {
    SummaryRec&  sr = *idx;
    KKStr accStr = StrFormatDouble (sr.confussionMatrix->Accuracy (), "ZZ0.00") + "%";

    *report << sr.tranLevel  << "\t"
            << sr.predLevel  << "\t"
            << accStr        << endl;

    *html << "    <tr>" << "<td>" << sr.tranLevel << "</td>" << "<td>" << sr.predLevel << "</td>" << "<td>" << "<a href=\"#" << sr.name << "\">" << accStr << "</a>" << "</td>" << "</tr>" << endl;
  }

  *html << "  </tbody>" << endl
        << "</table>"   << endl
        << "<br />"     << endl
        << "<br />"     << endl;

  
  
  *html << "<h1>Confusion Matrixes</h1>" << endl;

  for  (idx = resultsSummary.begin ();  idx != resultsSummary.end ();  idx++)
  {
    SummaryRec&  sr = *idx;

    *html << "<br />"     << endl
          << "<br />"     << endl
          << "<br />"     << endl
          << "<br />"     << endl;

    *html << "<h2><a name=\"" << sr.name << "\"/>Training Level[" << sr.tranLevel << "]  Prediction Level[" << sr.predLevel << "]" << "</h2>" << endl
          << "<br />" << endl;

    sr.confussionMatrix->PrintConfusionMatrixHTML (html->OStream ());
    *html << "<br />" << endl;
  }
}  /* ReportResults */



void  GradeClassification::ValidateThatBothListsHaveSameEntries (FeatureVectorList&  groundTruth, 
                                                                 FeatureVectorList&  examplesToGrade,
                                                                 bool&               theyAreTheSame
                                                                )
{
  theyAreTheSame = true;  // We will assume that they are the same to start with.

  int  missingExamplesToGrade = 0;
  int  missingGroundTruthExamples = 0;

  groundTruth.SortByRootName ();
  examplesToGrade.SortByRootName ();

  *report << endl << endl << endl
          << "Missing Examples To Grade" << endl
          << "=========================" << endl;

  ImageFeaturesList::iterator  idx;
  for  (idx = groundTruth.begin ();  idx !=  groundTruth.end ();  idx++)
  {
    FeatureVectorPtr  groundTruthExample = *idx;

    KKStr  rootName = osGetRootName (groundTruthExample->ExampleFileName ());

    FeatureVectorPtr  exampleToGrade = examplesToGrade.LookUpByRootName (rootName);

    if  (!exampleToGrade)
    {
      theyAreTheSame = false;
      *report << rootName << "\t" << "*** MISSING ***" << "\t" << groundTruthExample->ExampleFileName () << endl;
      missingExamplesToGrade++;
    }
  }

  if  (missingExamplesToGrade == 0)
  {
    *report  << "    *** None ***" << endl;
  }


  *report << endl << endl << endl
          << "Missing Ground Truth Examples" << endl
          << "=============================" << endl;

  for  (idx = examplesToGrade.begin ();  idx !=  examplesToGrade.end ();  idx++)
  {
    FeatureVectorPtr  exampleToGrade = *idx;

    KKStr  rootName = osGetRootName (exampleToGrade->ExampleFileName ());

    FeatureVectorPtr  groundTruthExample = groundTruth.LookUpByRootName (rootName);

    if  (!groundTruthExample)
    {
      theyAreTheSame = false;
      *report << rootName << "\t" << "*** MISSING ***" << "\t" << exampleToGrade->ExampleFileName () << "\t" << endl;
      missingGroundTruthExamples++;
    }
  }

  if  (missingGroundTruthExamples == 0)
  {
    *report << "   *** None ***" << endl;
  }
} /* ValidateThatBothListsHaveSameEntries */






void  GradeClassification::GradeExamplesAgainstGroundTruth (FeatureVectorListPtr  examplesToGrade,
                                                            FeatureVectorListPtr  groundTruth
                                                           )
{
  log.Level (10) << "GradeClassification::GradeExamplesAgainstGroundTruth" << endl;

  groundTruth->SortByRootName ();

  MLClassPtr  unknownClass = mlClasses->GetUnKnownClass ();

  MLClassListPtr classes = NULL;
  {
    MLClassListPtr examplesToGradeClasses = examplesToGrade->ExtractListOfClasses ();
    MLClassListPtr groundTruthClasses     = groundTruth->ExtractListOfClasses ();
    classes = MLClassList::MergeClassList (*examplesToGradeClasses, *groundTruthClasses);
    delete  examplesToGradeClasses;
    delete  groundTruthClasses;
  }

  kkuint16  maxHierarchialLevel = 0;
  {
    MLClassList::iterator  idx;
    for  (idx = classes->begin ();  idx != classes->end ();  idx++)
    {
      MLClassPtr  c = *idx;
      maxHierarchialLevel = Max (maxHierarchialLevel, c->NumHierarchialLevels ());
    }
  }

  // Create ConfusionMatrix objects for each possible level of Hierarchy. The 'resultsSummary' vector will 
  // end up owning the instances of 'ConfusionMatrix2' and the destructor will be responsible for deleting them.
  uint  curLevel = 0;
  vector<ConfusionMatrix2Ptr>  cmByLevel;
  for  (curLevel = 0;  curLevel < maxHierarchialLevel;  curLevel++)
  {
    MLClassListPtr  classesThisLevel = classes->ExtractListOfClassesForAGivenHierarchialLevel (curLevel);
    ConfusionMatrix2Ptr  cm = new ConfusionMatrix2 (*classesThisLevel);
    cmByLevel.push_back (cm);
  }

  ConfusionMatrix2  cm (*classes);


  ImageFeaturesList::iterator  idx;

  for  (idx = examplesToGrade->begin ();  idx !=  examplesToGrade->end ();  idx++)
  {
    ImageFeaturesPtr  exampleToGrade = *idx;
    MLClassPtr  predictedClass = exampleToGrade->MLClass ();
    float          origSize       = exampleToGrade->OrigSize ();
    float          probability    = exampleToGrade->Probability ();

    KKStr  rootName = osGetRootName (exampleToGrade->ExampleFileName ());
    FeatureVectorPtr  groundTruthExample = groundTruth->LookUpByRootName (rootName);
    MLClassPtr  groundTruthClass = unknownClass;
    if  (groundTruthExample)
      groundTruthClass = groundTruthExample->MLClass ();

    cm.Increment (groundTruthClass, predictedClass, (int)origSize, probability, log);

    for  (curLevel = 0;  curLevel < maxHierarchialLevel;  curLevel++)
    {
      MLClassPtr  groundTruthClasssThisLevel = groundTruthClass->MLClassForGivenHierarchialLevel (curLevel);
      MLClassPtr  predictedClassThisLevel    = predictedClass->MLClassForGivenHierarchialLevel   (curLevel);

      cmByLevel[curLevel]->Increment (groundTruthClasssThisLevel, predictedClassThisLevel, (int)origSize, probability, log);
    }
  }    


  //cm.PrintTrueFalsePositivesTabDelimited (*report);

  {
    // report Hierarchal results
    for  (curLevel = 0;  curLevel < maxHierarchialLevel;  curLevel++)
    {
      log.Level (10) << "GradeClassification::GradeExamplesAgainstGroundTruth   Printing Level[" << curLevel << "]" << endl;
      *report << endl << endl << endl
              << "Confusion Matrix   Training Level[" << maxHierarchialLevel << "]       Prediction Level[" << (curLevel + 1) << "]" << endl
              << endl;
      cmByLevel[curLevel]->PrintConfusionMatrixTabDelimited (*report);
      resultsSummary.push_back (SummaryRec (maxHierarchialLevel, curLevel + 1, cmByLevel[curLevel]));
    }

    *report << endl << endl << endl;
  }

  log.Level (10) << "GradeClassification::GradeExamplesAgainstGroundTruth     Exiting"  << endl;
}  /* GradeExamplesAgainstGroundTruth */
