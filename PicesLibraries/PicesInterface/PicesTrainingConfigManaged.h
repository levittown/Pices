#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::IO;

#include  "MLClass.h"
#include  "ImageFeatures.h"
#include  "TrainingConfiguration2.h"

#include  "PicesFeatureVectorList.h"
#include  "PicesTrainingClass.h"
#include  "PicesRunLog.h"


namespace PicesInterface 
{
  public ref class PicesTrainingConfigManaged
  {
  public:
    PicesTrainingConfigManaged (PicesRunLog^  _log);

    PicesTrainingConfigManaged (String^       _configFileName,
                                PicesRunLog^  _log 
                               );

    PicesTrainingConfigManaged (DirectoryInfo^  di,
                                PicesRunLog^    log
                               );
  private:
    ~PicesTrainingConfigManaged  ();

  protected:
    !PicesTrainingConfigManaged  ();

  public:
    void                           CancelLoad ();  // Call this method to set the 'canclFlag' to true.

    PicesTrainingConfigurationPtr  Config ()  {return config;};

    PicesClassList^                ExtractClassList ();

    static
    String^                        GetEffectiveConfigFileName (String^  configFileName);

    String^                        LoadLogStream ()  {return loadLogStream;}

    PicesFeatureVectorList^        LoadFeatureDataFromTrainingLibraries (bool^ changesMadeToTrainingLibraries);

    String^                        ModelName ();

    uint                           NumHierarchialLevels ();  // returns back the number of hierarchical levels there

    PicesClass^                    OtherClass ();

    String^                        RootDir ();

    void                           Save (String^  configFileName);

    PicesTrainingClassList^        TrainingClasses ();

    Boolean                        Valid ();

  private:
    void  AdjustCurMemoryPressure ();

    bool*                          cancelFlag;
    PicesTrainingConfigurationPtr  config;
    MLClassListPtr                 classes;
    int                            curMemoryPressure;
    String^                        loadLogStream;
    PicesRunLog^                   log;
    Boolean^                       valid;
  };  /* PicesTrainingConfigManaged */
}
