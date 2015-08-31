#include "FirstIncludes.h"

#include <stdlib.h>
#include <memory>
#include <math.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "Compressor.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "ConfusionMatrix2.h"
#include "DuplicateImages.h"
#include "FeatureFileIO.h"
#include "FeatureFileIOPices.h"
#include "FeatureVector.h"
#include "FileDesc.h"
#include "ImageFeatures.h"
#include "InstrumentDataFileManager.h"
#include "MLClass.h"
using namespace KKMLL;


#include "AbundanceCorrectionMatrix.h"
#include "PicesVariables.h"
#include "PicesApplication.h"
using namespace  MLL;


#include "AbundanceCorrectionStatsBuilder.h"
#include "TrainTestThread.h"
using namespace  AbundanceCorrectionApplication;


AbundanceCorrectionStatsBuilder::AbundanceCorrectionStatsBuilder ():

   PicesApplication (),
   allClasses              (NULL),
   configClasses           (NULL),
   maxNumActiveThreads     (1),
   msgQueue                (NULL),
   normalizationParms      (NULL),
   numOfFolds              (10),
   oneOrMoreThreadsCrashed (false),
   otherClass              (NULL),
   otherClassData          (NULL),
   queueDone               (NULL),
   queueReady              (NULL),
   queueRunning            (NULL),
   report                  (NULL),
   reportFileName          (),
   trainLibData            (NULL),
   trainLibDataClasses     (NULL)
{
}



AbundanceCorrectionStatsBuilder::~AbundanceCorrectionStatsBuilder ()
{
  if  (!reportFileName.Empty ())
    delete  report;
  report = NULL;

  delete allClasses;           allClasses          = NULL;
  delete trainLibData;         trainLibData        = NULL;
  delete otherClassData;       otherClassData      = NULL;
  delete trainLibDataClasses;  trainLibDataClasses = NULL;
  delete configClasses;        configClasses       = NULL;
  delete normalizationParms;   normalizationParms  = NULL;
  delete msgQueue;             msgQueue            = NULL;
  delete queueDone;            queueDone           = NULL;
  delete queueReady;           queueReady          = NULL;
  delete queueRunning;         queueRunning        = NULL;

  InstrumentDataFileManager::InitializePop ();
}




void  AbundanceCorrectionStatsBuilder::InitalizeApplication (kkint32 argc,
                                                             char**  argv
                                                            )
{
  ConfigRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  if  (argc < 2)
  {
    log.Level (-1) << endl << endl 
                   << "No Command Line Parameters were provided." << endl
                   << endl;

    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  maxNumActiveThreads = Min (6, osGetNumberOfProcessors ());

  //maxNumActiveThreads = 1;  // KKKK  For debugging only.


  fileDesc = FeatureFileIOPices::NewPlanktonFile ();

  InstrumentDataFileManager::InitializePush ();
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  queueDone    = new TrainTestThreadList (true);
  queueReady   = new TrainTestThreadList (true);
  queueRunning = new TrainTestThreadList (true);
}  /* InitalizeApplication */



bool  AbundanceCorrectionStatsBuilder::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                                const KKStr&  parmValue
                                                               )
{
  bool  parmValid = true;

  if  (parmSwitch.EqualIgnoreCase ("-folds")  ||  parmSwitch.EqualIgnoreCase ("-numFolds")  ||  parmSwitch.EqualIgnoreCase ("-numOfFolds"))
  {
    numOfFolds = parmValue.ToInt ();
    if  ((numOfFolds < 2)  ||  (numOfFolds > 1000))
    {
      log.Level (-1) << "ProcessCmdLineParameter   ***ERROR***    Invalid '-NumFolds' [" << numOfFolds << "] Invalid;  valid values (2 - 1000)." << endl;
      Abort (true);
      parmValid = false;
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-r")  ||  parmSwitch.EqualIgnoreCase ("-report")  ||  parmSwitch.EqualIgnoreCase ("-ReportFileName"))
    reportFileName = parmValue;

  else
    parmValid = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);


	return  parmValid;
}  /* ProcessCmdLineParameter */




/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   AbundanceCorrectionStatsBuilder::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  log.Level (0) << "AbundanceCorrectionStatsBuilder"                           << endl;
  log.Level (0)                                                                << endl;
  log.Level (0) << "    -folds  <number of folds>  Will default to 10."        << endl;
  log.Level (0)                                                                << endl;
  log.Level (0) << "    -Report <Report File Name>  defaults to cout."         << endl;
  log.Level (0)                                                                << endl;
}  /* DisplayCommandLineParameters */

// -C GulfOilBroad2_Discreate_Dual  -folds 2  -Report C:\Temp\GulfOilBroad2_Discreate_Dual_AbundanceReport.txt

// -c NRDA_small_DWH_Dual -folds 10 -report  C:\Temp\NRDA_small_DWH_Dual_AbindanceAdg.txt



void   AbundanceCorrectionStatsBuilder::PrintComandLineParameters ()
{
  PrintStandardHeaderInfo (*report);

  *report << "NumOfFolds"      << "\t" << numOfFolds     << endl
          << "ReportFileName"  << "\t" << reportFileName << endl
          << endl;
}


void   AbundanceCorrectionStatsBuilder::PrintStartStatistics ()
{
  if  (trainLibData)
  {
    *report << endl << endl << "Training Library Stats" << endl;
    trainLibData->PrintClassStatistics (*report);
  }

  if  (otherClassData)
  {
    *report << endl << endl << "Other Class Stats" << endl;
    otherClassData->PrintClassStatistics (*report);
  }
}  /* PrintStartStatistics */



void  AbundanceCorrectionStatsBuilder::RemoveDuplicateImages ()
{
  *report << endl << endl;

  FeatureVectorListPtr  allExamples = new FeatureVectorList (fileDesc, 
                                                             false    // 'false' = will not own contents.
                                                            ); 
  allExamples->AddQueue (*trainLibData);
  allExamples->AddQueue (*otherClassData);
  
  DuplicateImages  dupChecker (allExamples, log);
  if  (dupChecker.DuplicatesFound ())
  {
    *report << "DUPLICATE IMAGES DETECTED." << endl;
    dupChecker.ReportDuplicates (*report);
    
    FeatureVectorListPtr  dups = dupChecker.ListOfExamplesToDelete ();
    if  (dups)
    {
      FeatureVectorList::iterator  idx;
      for  (idx = dups->begin ();  idx != dups->end ();  ++idx)
      {
        FeatureVectorPtr  fv = *idx;
        trainLibData->DeleteEntry (fv);
        otherClassData->DeleteEntry (fv);
      }
      delete  dups;
      dups = NULL;
    }
  }
  else
  {
    *report << "No duplicates detected." << endl;
  }

  delete  allExamples;  allExamples = NULL;

  *report << endl << endl;
}  /* RemoveDuplicateImages */



void   AbundanceCorrectionStatsBuilder::TerminateThreads ()
{
  TrainTestThreadList::iterator  idx;

  log.Level (10) << "TerminateThreads   Flagging all running threads to terminate." << endl;
  for  (idx = queueRunning->begin ();  idx != queueRunning->end ();  ++idx)
  {
    TrainTestThreadPtr  thread = *idx;
    thread->CancelThread ();
  }

  int  loopCount = 0;
  int  numThreadsRunning = 0;
  do
  {
    osSleep (0.1f);
    numThreadsRunning = 0;
    for  (idx = queueRunning->begin ();  idx != queueRunning->end ();  ++idx)
    {
      TrainTestThreadPtr  thread = *idx;
      if  ((thread->Status () != Stopped)  &&  (thread->Status () != NotStarted))
        ++numThreadsRunning;
    }

    if  ((loopCount  % 10) == 0)
      log.Level (10) << "TerminateThreads   [" << numThreadsRunning << "]  Threads are still running." << endl;

    ++loopCount;
  }  while  (numThreadsRunning > 0);
}  /* TerminateThreads */



void   AbundanceCorrectionStatsBuilder::ManageThreads ()
{
  kkuint32  loopCount = 0;
  bool  terminate = false;
  while  (!terminate)
  {
    {
      KKStrPtr  msg = msgQueue->GetNextMsg ();
      while  (msg)
      {
        log.Level (10) << *msg << endl;
        msg = msgQueue->GetNextMsg ();
      }
    }

    if  ((queueReady->QueueSize () < 1)  &&  (queueRunning->QueueSize () < 1))
    {
     terminate = true;
     break;
    }

    {
      // Move any completed threads from 'QueueRunning'  to  'QueueDone'
      TrainTestThreadList::iterator  idx = queueRunning->begin ();
      while  (idx != queueRunning->end ())
      {
        TrainTestThreadPtr thread = *idx;

        if  (thread->Crashed ())
        {
          oneOrMoreThreadsCrashed = true;
          log.Level (10) << endl << endl
            << "ManageThreads    ***ERROR***      Thread[" << thread->Desc () << "]   Crashed."  << endl
            << endl;
        }

        if  (thread->Status () == Stopped)
        {
          idx = queueRunning->erase (idx);
          queueDone->PushOnBack (thread);
        }
        else
        {
          ++idx;
        }
      }
    }

    if  (oneOrMoreThreadsCrashed)
    {
      terminate = true;
      log.Level (10) << "ManageThreads    ***ERROR***   One or more threads crashed;   terminating application." << endl
        << endl;
      TerminateThreads ();
    }

    while  ((queueRunning->QueueSize () < maxNumActiveThreads)  &&  (queueReady->QueueSize () > 0))
    {
      TrainTestThreadPtr  nextThreadToStart = queueReady->PopFromFront ();
      if  (nextThreadToStart != NULL)
      {
        log.Level (10) << "ManageThreads   Starting thread[" << nextThreadToStart->ThreadName () << "]" << endl;
        queueRunning->PushOnBack (nextThreadToStart);
        nextThreadToStart->Start ();
      }
    }

    if  ((loopCount  % 20) == 0)
    {
      cout << "Ready:"   << queueReady->QueueSize ()   << "  "
           << "Running:" << queueRunning->QueueSize () << "  "
           << "Done:"    << queueDone->QueueSize ()    << "  "
           << endl;
    }

    osSleep (0.5f);
    ++loopCount;
  }
}  /* ManageThreads */



void   AbundanceCorrectionStatsBuilder::CreateInitialThreadInstaces ()
{
  log.Level (10) << "AbundanceCorrectionStatsBuilder::CreateInitialThreadInstaces"  << endl;
  
  FeatureVectorListPtr  stratifiedTrainData = trainLibData->StratifyAmoungstClasses (numOfFolds, log);
  FeatureVectorListPtr  stratifiedOtherData = otherClassData->StratifyAmoungstClasses (numOfFolds, log);

  kkint32  numTrainExamples = stratifiedTrainData->QueueSize ();
  kkint32  numOtherExamples = stratifiedOtherData->QueueSize ();

  msgQueue = new MsgQueue ("AbundanceCorrectionStatsBuilder");

  kkint32  lastFvInFold = -1;
  kkint32  firstFvInFold = 0;

  kkint32  firstOtherFvInFold = 0;
  kkint32  lastOtherFvInFold = -1;

  for  (kkint32  foldNum = 0;  foldNum < numOfFolds;  ++foldNum)
  {
    firstFvInFold = lastFvInFold + 1;
    lastFvInFold  = (numTrainExamples * (foldNum + 1) / numOfFolds) - 1;

    firstOtherFvInFold = lastOtherFvInFold + 1;
    lastOtherFvInFold  = (numOtherExamples * (foldNum + 1) / numOfFolds) - 1;

    FeatureVectorListPtr  trainData = new FeatureVectorList (fileDesc, false);
    FeatureVectorListPtr  testData  = new FeatureVectorList (fileDesc, false);

    for  (kkint32 idx = 0;  idx < numTrainExamples;  ++idx)
    {
      FeatureVectorPtr fv = stratifiedTrainData->IdxToPtr (idx);
      if  ((idx >= firstFvInFold)  &&  (idx <= lastFvInFold))
        testData->PushOnBack (fv);
      else
        trainData->PushOnBack (fv);
    }

    // Add OtherClass examples to test data.
    for  (kkint32 idx = firstOtherFvInFold;  idx <= lastOtherFvInFold;  ++idx)
    {
      FeatureVectorPtr fv = stratifiedOtherData->IdxToPtr (idx);
      testData->PushOnBack (fv);
    }

    RunLogPtr  threadRunLog = new RunLog ();
    threadRunLog->AttachMsgQueue (msgQueue);

    KKStr  threadName = "AbundanceCorrFold" + StrFormatInt (foldNum, "00");
    TrainTestThreadPtr  thread = new TrainTestThread 
               ("Fold_" + StrFormatInt (foldNum, "00"),
                this,
                config,
                allClasses,
                trainData,            // Will take ownership and delete in its destructor.
                trainLibDataClasses,
                testData,             // Will take ownership and delete in its destructor.
                otherClass,
                threadName,
                msgQueue,             // Will take ownership and delete in its destructor.
                threadRunLog
               );

    queueReady->PushOnBack (thread);
  }

  delete  stratifiedOtherData;  stratifiedOtherData = NULL;
  delete  stratifiedTrainData;  stratifiedTrainData = NULL;
}  /* CreateInitialThreadInstaces */



void   AbundanceCorrectionStatsBuilder::GenerateCrashReport ()
{
  *report << endl << endl
    << "ONE OR MORE THREADS CRASHED"  << endl
    << "==========================="  << endl
    << endl
    << "Report and statistics were not generated." << endl
    << endl;
}  /* GenerateCrashReport */




void   AbundanceCorrectionStatsBuilder::GenerateReportAndStats ()
{
  AbundanceCorrectionMatrix  abundanceCorrMatrix (*trainLibDataClasses, otherClass, log);
  ConfusionMatrix2           confusionMatrix (*allClasses);

  TrainTestThreadList::iterator  idx;
  for  (idx = queueDone->begin ();  idx != queueDone->end ();  ++idx)
  {
    TrainTestThreadPtr  thread = *idx;

    const AbundanceCorrectionMatrixPtr  acm = thread->AbundanceCorMatrix ();
    const ConfusionMatrix2Ptr           cm  = thread->ConfusionMatrix ();

    if  (acm)  abundanceCorrMatrix.AddIn (*acm, log);
    if  (cm)   confusionMatrix.AddIn     (*cm, log);
  }

  *report << endl << endl << endl;
  abundanceCorrMatrix.PrintConfusionMatrixTabDelimited (*report);

  *report << endl << endl << endl << endl;
  confusionMatrix.PrintConfusionMatrixTabDelimited (*report);
  *report << endl << endl << endl;

  abundanceCorrMatrix.SaveForConfigFileName (configFileName);
}  /* GenerateReportAndStats */




void   AbundanceCorrectionStatsBuilder::Main ()
{
  if  (Abort ())
    return;

  if  (reportFileName.Empty ())
  {
    DateTime  d = osGetLocalDateTime ();
    KKStr  reportDir = osAddSlash (PicesVariables::ReportDir ()) + "AbundanceAdjustments";
    osCreateDirectoryPath (reportDir);
    if  (configFileName.Empty ())
      reportFileName = osAddSlash (reportDir) + "NoConfigFile" + "_" + d.YYYYMMDDHHMMSS () + ".txt";
    else
      reportFileName = osAddSlash (reportDir) + osGetRootName (configFileName) + "_" + d.YYYYMMDDHHMMSS () + ".txt";
  }
 
  report = new ofstream (reportFileName.Str ());
  PrintComandLineParameters ();

  bool      changesMadeToTrainingLibraries = false;
  bool      cancelFlag = false;
  DateTime  latestImageTimeStamp;

  delete  trainLibData;
  trainLibData = config->LoadFeatureDataFromTrainingLibraries (latestImageTimeStamp, changesMadeToTrainingLibraries, cancelFlag, log);
  if  (!trainLibData)
  {
    log.Level (-1) << endl
      << "AbundanceCorrectionStatsBuilder::Main   ***ERROR***    No training data was loaded." << endl
      << endl;

    *report << endl << "*** Failed to load training data ***" << endl;
    return;
  }

  otherClass = config->OtherClass ();
  if  (!otherClass)
    otherClass = MLClass::CreateNewMLClass ("Other", -1);

  configClasses = config->ExtractClassList ();
  configClasses->SortByName ();
  if  (configClasses->PtrToIdx (otherClass) >= 0)
  {
    log.Level (-1) << endl
      << "AbundanceCorrectionStatsBuilder::Main   ***ERROR***   OtherClass[" << otherClass->Name () << "] is specified as a Training Class; it must be specified separately." << endl
      << endl;
    *report << endl << "*** Failed to load other class data ***" << endl;
    return;
  }

  trainLibDataClasses = trainLibData->ExtractListOfClasses ();
  trainLibDataClasses->SortByName ();

  if  ((*configClasses) != (*trainLibDataClasses))
  {
    Abort (true);
    log.Level (-1) << endl 
      << "AbundanceCorrectionStatsBuilder::Main   ***ERROR***   Class make up of training data does not correspond to configuration file." << endl
      << endl;

    *report << endl << "*** Training data contains different classes that Config File  ***" << endl;
    return;
  }

  allClasses = new MLClassList (*trainLibDataClasses);
  allClasses->PushOnBack (otherClass);

  otherClassData = config->LoadOtherClasssExamples (log);
  if  ((!otherClassData)  ||  (otherClassData->QueueSize () < 1))
  {
    Abort (true);
    log.Level (-1) << endl 
      << "AbundanceCorrectionStatsBuilder::Main   ***ERROR***   No Other[" << otherClass->Name () << "] class examples found in training library." << endl
      << endl;

    *report << endl << "***  No other class data loaded. ***" << endl;
    return;
  }

  PrintStartStatistics ();

  RemoveDuplicateImages ();

  normalizationParms = new NormalizationParms (config, *trainLibData, log);
  normalizationParms->NormalizeExamples (trainLibData, log);
  normalizationParms->NormalizeExamples (otherClassData, log);

  CreateInitialThreadInstaces ();

  ManageThreads ();

  if  (oneOrMoreThreadsCrashed)
  {
    GenerateCrashReport ();
  }
  else
  {
    GenerateReportAndStats ();
  }
}  /* Main */




void  main (int argc,  char** argv)
{
  AbundanceCorrectionStatsBuilder  app;
  app.InitalizeApplication (argc, argv);
  if  (!app.Abort ())
  {
    app.Main ();
  }
}


