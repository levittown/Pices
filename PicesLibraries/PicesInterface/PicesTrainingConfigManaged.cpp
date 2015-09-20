#include "StdAfx.h"
#include  "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <strstream>
#include <vector>


#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "MemoryDebug.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "DuplicateImages.h"
#include "FileDesc.h"
#include "FeatureFileIOPices.h"
#include "PicesVariables.h"
#include "PicesTrainingConfiguration.h"
using namespace  MLL;


#include "PicesTrainingConfigManaged.h"
#include "PicesKKStr.h"
#include "PicesRunLog.h"
using namespace PicesInterface;


using namespace System;
using namespace System::Threading;

namespace  PicesInterface
{

PicesTrainingConfigManaged::PicesTrainingConfigManaged (PicesRunLog^  _log):
  cancelFlag (new bool (false)),
  classes           (NULL),
  config            (NULL),
  curMemoryPressure (0),
  loadLogStream     (nullptr),
  log               (_log),
  valid             (gcnew System::Boolean (false))

{
  AdjustCurMemoryPressure ();
}



PicesTrainingConfigManaged::PicesTrainingConfigManaged (String^       _configFileName,
                                                        PicesRunLog^  _log
                                                       ):
  cancelFlag (new bool (false)),
  classes           (NULL),
  config            (NULL),
  curMemoryPressure (0),
  loadLogStream     (nullptr),
  log               (_log),
  valid             (gcnew System::Boolean (false))
{
  std::strstream  logStr;
  KKStr  configFileName = PicesKKStr::SystemStringToKKStr (_configFileName);
  FileDescPtr  fileDesc = FeatureFileIOPices::NewPlanktonFile ();
  config = new PicesTrainingConfiguration ();
  config->Load (configFileName, true, log->Log ());
  valid = gcnew System::Boolean (config->FormatGood ());
  if  (!config->FormatGood ())
     loadLogStream = gcnew String (logStr.str ());
  AdjustCurMemoryPressure ();
}



PicesTrainingConfigManaged::PicesTrainingConfigManaged (DirectoryInfo^  di,
                                                        PicesRunLog^    _log
                                                       ):
  cancelFlag        (new bool (false)),
  classes           (NULL),
  config            (NULL),
  curMemoryPressure (0),
  loadLogStream     (nullptr),
  log               (_log),
  valid             (gcnew Boolean (false))

{
  osRunAsABackGroundProcess ();

  KKStr  errorMessage;

  KKStr  dirName    = PicesKKStr::SystemStringToKKStr (di->FullName);
  bool   successful = false;

  KKStr  modelNameKKStr = osGetRootNameOfDirectory (dirName) + ".cfg";
  KKStr  configFileName = osAddSlash (PicesVariables::TrainingModelsConfigurationDir ()) + modelNameKKStr;
    
  FileDescPtr fd = FeatureFileIOPices::NewPlanktonFile ();
  delete  classes;  classes = NULL;

  config = PicesTrainingConfiguration::CreateFromDirectoryStructure 
                                        (configFileName,
                                         dirName,
                                         log->Log (),
                                         successful,
                                         errorMessage
                                        );

  valid = gcnew System::Boolean (successful);

  classes = config->ExtractClassList ();

  // We are never ever supposed to delete a 'FileDesc' object.  they are managed wholly in the "FileDesc" class.  
  // Other parts of the application may and probably will be using this same instance.
  //delete  fd;  fd = NULL;
  AdjustCurMemoryPressure ();
}  /* CreateTrainingModelFromDirectory */
  


PicesTrainingConfigManaged::~PicesTrainingConfigManaged ()
{
  this->!PicesTrainingConfigManaged ();
}



PicesTrainingConfigManaged::!PicesTrainingConfigManaged ()
{
  delete  cancelFlag;  cancelFlag = NULL;
  delete  config;      config     = NULL;
  delete  classes;     classes    = NULL;
  if  (curMemoryPressure > 0)
    GC::RemoveMemoryPressure (curMemoryPressure);
  curMemoryPressure = 0;
}


void  PicesTrainingConfigManaged::AdjustCurMemoryPressure ()
{
  int  newMemoryPressure = 0;
  if  (config)   newMemoryPressure += config->MemoryConsumedEstimated ();
  if  (classes)  newMemoryPressure += classes->MemoryConsumedEstimated ();

  int  deltaMemororyPressure = newMemoryPressure - curMemoryPressure;
  if  (deltaMemororyPressure > 0)
    GC::AddMemoryPressure (deltaMemororyPressure);

  else if  (deltaMemororyPressure < 0)
    GC::RemoveMemoryPressure (-deltaMemororyPressure);

  curMemoryPressure = newMemoryPressure;
}




void  PicesTrainingConfigManaged::CancelLoad ()  // Call this method to set the 'canclFlag' to true.
{
  *cancelFlag = true;
}


String^   PicesTrainingConfigManaged::RootDir ()
{
  return  PicesKKStr::KKStrToSystenStr (config->RootDir ());
}



String^   PicesTrainingConfigManaged::ModelName ()
{
  if  (!config)
    return  System::String::Empty;

  return  PicesKKStr::KKStrToSystenStr (osGetRootNameWithExtension (config->ConfigFileNameSpecified ()));
}  /* ModelName */





System::Boolean  PicesTrainingConfigManaged::Valid ()
{
  return  *valid;
}  /* Valid */




uint  PicesTrainingConfigManaged::NumHierarchialLevels ()
{
  if  (config)
    return  config->NumHierarchialLevels ();
  else
    return 0;
}




PicesClassList^  PicesTrainingConfigManaged::ExtractClassList ()
{
  if  (config == NULL)
    return  nullptr;
  MLClassListPtr  classes = config->ExtractClassList ();

  PicesClassList^  results = gcnew PicesClassList (classes);
  delete classes;
  classes = NULL;

  return  results;
}




String^  PicesTrainingConfigManaged::GetEffectiveConfigFileName (String^  configFileName)
{
  KKStr  effectiveFileName = TrainingConfiguration2::GetEffectiveConfigFileName (PicesKKStr::SystemStringToKKStr (configFileName));
  return  PicesKKStr::KKStrToSystenStr (effectiveFileName);
}



PicesClass^  PicesTrainingConfigManaged::OtherClass ()
{
  PicesClass^ otherClass = nullptr;

  if  (config != NULL)
  {
    MLClassPtr  oc = config->OtherClass ();
    if  (oc != NULL)
      otherClass = PicesClassList::GetUniqueClass (oc);
  }
  return  otherClass;
}  /* OtherClass */




PicesFeatureVectorList^  PicesTrainingConfigManaged::LoadFeatureDataFromTrainingLibraries (bool^ _changesMadeToTrainingLibraries)
{
  if  (!config)
    return  nullptr;

  *cancelFlag = false;

  bool  changesMadeToTrainingLibraries = false;

  KKB::DateTime  latestImageTimeStamp;

  FeatureVectorListPtr  data = config->LoadFeatureDataFromTrainingLibraries 
         (latestImageTimeStamp,
          changesMadeToTrainingLibraries,
          *cancelFlag,
          log->Log ()
         );

  if  (data == NULL)
    return nullptr;

  if  (*cancelFlag)
  {
    delete  data;
    data=NULL;
   return nullptr;
  }

  {
    // Eliminate Duplicate data.
    DuplicateImages dup (data, this->log->Log ());
    dup.PurgeDuplicates (data, false, NULL);
  }

  ImageFeaturesListPtr  imageData = new ImageFeaturesList (*data, false);
  imageData->Owner (true);
  data->Owner (false);
  delete  data;
  data = NULL;

  PicesFeatureVectorList^ pfvl = gcnew PicesFeatureVectorList (*imageData);
  imageData->Owner (false);
  delete  imageData;  imageData = NULL;

  _changesMadeToTrainingLibraries = changesMadeToTrainingLibraries;

  //*_changesMadeToTrainingLibraries = changesMadeToTrainingLibraries;

  return  pfvl;
}  /* LoadFeatureDataFromTrainingLibraries */




PicesTrainingClassList^   PicesTrainingConfigManaged::TrainingClasses ()
{
  PicesTrainingClassList^  results = gcnew PicesTrainingClassList ();

  const TrainingClassList&  trainingClasses = config->TrainingClasses ();
  TrainingClassList::const_iterator  idx;
  for  (idx = trainingClasses.begin ();  idx != trainingClasses.end ();  idx++)
  {
    const TrainingClassPtr  tc = *idx;
    TrainingClassPtr  newTc = new TrainingClass (*tc);
    results->Add (gcnew PicesTrainingClass (newTc));
  }

  return  results;
}  /* TrainingClasses */




void   PicesTrainingConfigManaged::Save (String^  configFileName)
{
  KKStr  fileName = PicesKKStr::SystemStringToKKStr (configFileName);
  config->Save (fileName);
}  /* Save */

}  /* PicesInterface */