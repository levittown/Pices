#include "FirstIncludes.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#ifdef  WIN32
#include <io.h>
#include <windows.h>
#else
//#include  <sys/loadavg.h>
#include <unistd.h>
#endif
#include  "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "KKQueue.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


#include "ConfusionMatrix2.h"
#include "CrossValidation.h"
#include "TrainingProcess2.h"
using namespace  KKMLL;


#include "JobValidation.h"
#include "BinaryJobList.h"
#include "FeatureSelectionTypes.h"
#include "Processor.h"
#include "ValidationResults.h"
using namespace  FeatureSelectionApp;



JobValidation::JobValidation (const JobValidation&  j):
  BinaryJob            (j),
  classedCorrectly     (NULL),
  classedCorrectlySize (j.classedCorrectlySize),
  configFileName       (j.configFileName)

{
  if  (classedCorrectlySize > 0)
  {
    classedCorrectly = new bool[classedCorrectlySize];
    for  (int x = 0;  x < classedCorrectlySize;  x++)
      classedCorrectly[x] = j.classedCorrectly[x];
  }
}




JobValidation::JobValidation (ProcessorPtr            _processor,
                              int                     _parentId,
                              const FeatureNumList&   _features,
                              int                     _chgFeatureNum,
                              kkint32                 _numOfRounds,
                              double                  _cParm,
                              double                  _gammaParm,
                              float                   _aParm,
                              const KKStr&            _configFileName
                             ):
     BinaryJob (_processor, _parentId, _features, _chgFeatureNum, _numOfRounds, _cParm, _gammaParm, _aParm),
     classedCorrectly     (NULL),
     classedCorrectlySize (0),
     configFileName       (_configFileName)
{
}




JobValidation::JobValidation (ProcessorPtr  _processor,
                              KKStrParser&  _statusLine
                             ):
     BinaryJob (_processor, _statusLine),
     classedCorrectly     (NULL),
     classedCorrectlySize (0),
     configFileName       ()
{
  // The call to the 'BinaryJob' constructor would have parsed the '_statusLine' string for fields that are particular to
  // 'BinaryJob'.  Now we will have to scan for fields that are particular to 'JobValidation'.
  ProcessStatusStr (_statusLine);
}





JobValidation::~JobValidation ()
{
  if  (classedCorrectly)
  {
    delete  classedCorrectly;
    classedCorrectly = NULL;
  }
}




JobTypes  JobValidation::JobType ()  const
{
  return  JobTypes::Validation;
}




KKStr   JobValidation::ToStatusStr ()
{
  KKStr  statusStr = BinaryJob::ToStatusStr ();
  statusStr << "\t" << "ConfigFileName" << "\t" << configFileName;
  return  statusStr;
}




void   JobValidation::EvaluateNode ()
{
  log.Level (9) << "  " << endl;
  log.Level (9) << "JobValidation::EvaluteNode JobId[" << jobId << "]" << endl;
  status = BinaryJobStatus::Started;

  bool  configFileFormatGood = true;
  
  TrainingConfiguration2Ptr  config = new TrainingConfiguration2 ();
  config->Load (configFileName, false, log);
  if  (!config->FormatGood ())
    configFileFormatGood;

  config->SetFeatureNums (features);
  config->C_Param (cParm);
  config->Gamma   (gammaParm);
  config->A_Param (aParm);
  config->SelectionMethod (processor->SelectionMethod ());


  switch  (processor->ResultType ())
  {
  case  FinalResultType::MfsFeaturesSel:
  case  FinalResultType::NoTuningAllFeatures:
  case  FinalResultType::MfsParmsTuned: 
  case  FinalResultType::MfsParmsTunedFeaturesSel: 
           config->MachineType (SVM_MachineType::OneVsOne);
           break;
    
  case  FinalResultType::BfsFeaturesSel:
  case  FinalResultType::BfsParmsTuned:
  case  FinalResultType::BfsFeaturesSelParmsTuned:
           config->MachineType (SVM_MachineType::BinaryCombos);
           break;
  }

  bool  cancelFlag = false;

  FeatureVectorListPtr  trainData       = processor->TrainingData ();
  FeatureVectorListPtr  validationData  = processor->ValidationData ();

  VectorDouble  trainDataMeans      = trainData->ExtractMeanFeatureValues ();
  VectorDouble  validationDataMeans = validationData->ExtractMeanFeatureValues ();


  CrossValidationPtr  crossValidation = new CrossValidation  
                                           (config,
                                            trainData,
                                            processor->MLClasses (),
                                            processor->NumOfFolds (),
                                            processor->AlreadyNormalized (),
                                            processor->FileDesc (),
                                            log,
                                            cancelFlag
                                           );

  delete  classedCorrectly;
  classedCorrectlySize = validationData->QueueSize ();
  classedCorrectly = new bool[classedCorrectlySize];

  crossValidation->RunValidationOnly (validationData, classedCorrectly, log);

  testAccuracy      = crossValidation->Accuracy ();
  testAccuracyNorm  = crossValidation->AccuracyNorm ();
  testAvgPredProb   = (float)crossValidation->AvgPredProb () * 100.0f;
  testFMeasure      = (float)crossValidation->ConfussionMatrix ()->FMeasure (processor->PositiveClass (), log);

  if  (processor->GradingMethod () == GradingMethodType::Accuracy)
    testGrade = testAccuracy;

  else if  (processor->GradingMethod () == GradingMethodType::AccuracyNorm)
    testGrade = testAccuracyNorm;

  else if  (processor->GradingMethod () == GradingMethodType::FMeasure)
    testGrade = testFMeasure;

  else
    testGrade = testAccuracy;

  testNumSVs  = crossValidation->NumOfSupportVectors ();

  {
    // Save results of this Split in Results file.
    processor->Block ();

    {
      uint  fn = 0;
      ofstream rl ("FinalResults.log", ios_base::app);
      rl << endl << endl
         << "ConfigFileName"          << "\t" << configFileName  << "\t" << "Format Good[" << (configFileFormatGood ? "Yes" : "No") << endl
         << "SummaryResultsFileName"  << "\t" << processor->SummaryResultsFileName () << endl
         << "Configuration CmdLine"   << "\t" << config->SVMparamREF (log).ToString ()   << endl
         << "ImagesPerClass"          << "\t" << config->ImagesPerClass ()            << endl
         << endl;

      rl << endl << endl
         << "Training Data Status" << endl
         << endl;
      trainData->PrintClassStatistics (rl);
      rl << endl << endl;


      rl << "TrainingDataMeans";
      for  (fn = 0;  fn < trainDataMeans.size ();  fn++)
        rl << "\t" << trainDataMeans[fn];
      rl << endl;

      rl << "ValidationDataMeans";
      for  (fn = 0;  fn < validationDataMeans.size ();  fn++)
        rl << "\t" << validationDataMeans[fn];
      rl << endl
         << endl;

      crossValidation->ConfussionMatrix ()->PrintConfusionMatrixTabDelimited (rl);
      rl << endl << endl << endl << endl;
      rl.close ();
    }

    {
      ofstream  f (processor->SummaryResultsFileName ().Str (), ios_base::app);
      ValidationResults r (processor->ResultType (), 
                           config, 
                           crossValidation,
                           trainData,
                           osGetHostName (),
                           classedCorrectlySize,
                           classedCorrectly,
                           this,
                           log
                          );
      r.Write (f);
      f.close ();
    }
    processor->EndBlock ();
  }

  delete  crossValidation;     crossValidation    = NULL;
  delete  config;              config = NULL;
  status = BinaryJobStatus::Done;
}  /* EvaluateNode */






// Will scan for fields that are specific to 'JobValidation'
void  JobValidation::ProcessStatusStr (KKStrParser&  statusStr)
{
  log.Level (30) << "JobValidation::ProcessStatusStr[" << statusStr.Str () << "]" << endl;
  KKStr  fieldName;
  KKStr  fieldValue;

  while  (statusStr.MoreTokens ())
  {
    fieldName = statusStr.GetNextToken ("\t\n\r");
    fieldName.Upper ();

    fieldValue = statusStr.GetNextToken ("\t");
    fieldValue.TrimLeft ("\n\r\t ");
    fieldValue.TrimRight ("\n\r\t ");

    ProcessStatusField (fieldName, fieldValue);
  }
}  /* ProcessStatusStr */




void  JobValidation::ProcessStatusField (const KKStr&  fieldName,
                                         const KKStr&  fieldValue
                                        )
{
  // 'JobValidation' does not have any fields.
  if  (fieldName.EqualIgnoreCase ("configFileName"))
    configFileName = fieldValue;
}  /* ProcessStatusField */





void  JobValidation::ReFresh (BinaryJob&  j)
{
  BinaryJob::ReFresh (j);

  delete  classedCorrectly;  
  classedCorrectly = NULL;
  classedCorrectlySize = 0;

  if  (j.JobType () == JobTypes::Validation)
  {
    JobValidation&  validationJob = dynamic_cast<JobValidation&> (j);

    configFileName  = validationJob.configFileName;

    classedCorrectlySize = validationJob.classedCorrectlySize;

    if  (classedCorrectlySize > 0)
    {
      classedCorrectly = new bool[classedCorrectlySize];
      for  (int x = 0;  x < classedCorrectlySize;  x++)
        classedCorrectly[x] = validationJob.classedCorrectly[x];
    }
  } 
}  /* ReFresh */




void  JobValidation::ReFresh (JobValidation&  j)
{
  BinaryJob::ReFresh (j);
 
  configFileName = j.configFileName;

  delete  classedCorrectly;  
  classedCorrectly = NULL;

  classedCorrectlySize = j.classedCorrectlySize;
  if  (classedCorrectlySize > 0)
  {
    classedCorrectly = new bool[classedCorrectlySize];
    for  (int x = 0;  x < classedCorrectlySize;  x++)
      classedCorrectly[x] = j.classedCorrectly[x];
  }
}  /* ReFresh */
