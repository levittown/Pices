#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


#include "Classifier2.h"
#include "TrainingProcess2.h"

#include "ImageFeatures.h"
#include "PicesTrainingConfiguration.h"

// Pices Interface Headers
#include "PicesClass.h"
#include "PicesClassList.h"
#include "PicesFeatureVector.h"
#include "PicesMsgQueue.h"
#include "PicesPrediction.h"
#include "PicesPredictionList.h"
#include "PicesRunLog.h"
#include "PicesTrainingConfigManaged.h"




namespace PicesInterface 
{
  public ref class  ProbNamePair
  {
  public:
    ProbNamePair (String^ _name,  double _probability):
        name (_name),  probability (_probability)
        {}

    ProbNamePair (const KKStr& _name,  double _probability):
        name (nullptr),  probability (0.0f)
    {
      name = PicesKKStr::KKStrToSystenStr (_name);
      probability = _probability;
    }

    String^  name;
    double   probability;
  };



	public ref class TrainingModel2 
	{
	public:
    enum class ModelTypes: int
    {
      Null      = KKMLL::Model::ModelTypes::Null,
      OldSVM    = KKMLL::Model::ModelTypes::OldSVM,
      SvmBase   = KKMLL::Model::ModelTypes::SvmBase,
      KNN       = KKMLL::Model::ModelTypes::KNN,
      UsfCasCor = KKMLL::Model::ModelTypes::UsfCasCor,
      Dual      = KKMLL::Model::ModelTypes::Dual
    };

    TrainingModel2 (PicesRunLog^ _picesRunLog,  System::String^ _modelName);

    /// <summary> Builds a configuration using the directory structure pointed to by <paramref name='_directoryInfo'/> .</summary>
    /// <remarks>Searches through the directory structure recursively using the directory names to define class names.</remarks>
    /// <param name="_picesRunLog"> The pices run log.</param>
    /// <param name="_directoryInfo"> Root to subdirectory that contains training images with each child directory containing examples for a specific class.</param>
    TrainingModel2 (PicesRunLog^ _picesRunLog,  System::IO::DirectoryInfo^ _directoryInfo);

    /// <summary> Constructor that utilizes <paramref name='_config'/>.</summary>
    /// <param name="_picesRunLog"> The pices run log.</param>
    /// <param name="_config">      The configuration.</param>
    TrainingModel2 (PicesRunLog^ _picesRunLog,  PicesTrainingConfigManaged^ _config);


    /// <summary> Creates an instance to be used with an already existing instance of
    ///   'TrainingProcess2'. I created this to get a break down of a prediction on a Dual class
    ///   classifier.</summary>
    /// <param name="_picesRunLog"> The pices run log.</param>
    /// <param name="_trainer">     The trainer.</param>
    TrainingModel2 (PicesRunLog^ _picesRunLog,  TrainingProcess2Ptr  _trainer);

    void  CleanUp ();

  protected:
    !TrainingModel2 ();

  private:
    ~TrainingModel2 ();

    void  CleanUpUnmanagedResources ();


  public:
    property  System::DateTime  BuildDateTime            {System::DateTime  get ();}
    property  bool              CancelFlag               {bool              get ()  {return *cancelFlag;}}
    property  String^           ConfigFileName           {String^           get ();}
    property  List<String^>^    ErrorMsgs                {List<String^>^    get ()  {return  errorMsgs;}}
    property  int               ImagesPerClass           {int               get ();}
    property  String^           ModelName                {String^           get ()  {return  modelName;}}
    property  ModelTypes        ModelType                {ModelTypes        get ();}
    property  uint              NumHierarchialLevels     {uint              get ();}     // returns back the number of hierarchical levels there
    property  PicesClass^       OtherClass               {PicesClass^       get ();}
    property  String^           ParameterStr             {String^           get ();}
    property  String^           RootDir                  {String^           get ();}
    //property  PicesRunLog^  PicesRunLog              {PicesRunLog^ get ()  {return picesRunLog;}}
    property  String^           RunLogFileName           {String^           get ();}
//    property  String^           SvmParamStr              {String^           get ();}
    property  bool              Valid                    {bool              get ()  {return  *valid;}}


    ///<summary> When using a Dual classifier will return an instance of the first/left classifier. </summary>
    TrainingModel2^   CreateTrainingModelLeft ();

    ///<summary> When using a Dual classifier will return an instance of the second/right classifier. </summary>
    TrainingModel2^   CreateTrainingModelRight ();



    void  AddClass (PicesClass^  newClass);

    void  AddImageToTrainingLibray (String^       imageFileName,
                                    PicesRaster^  raster, 
                                    PicesClass^   mlClass,
                                    bool          onLine  
                                   );


    void  AttachMsgQueueToRunLog (PicesMsgQueue^  msgQueue);


    void  BuildTrainingModel (PicesFeatureVectorList^  picesTrainingData);


    ///<summary>
    /// Sets the 'cancelFlag' to true; the load and training processes monitor this flag; if goes true  process will exit at earliest convenience.
    ///</summary>
    void  CancelLoad ();

    ///<summary> 
    /// Returns a duplicate list of classes that belong to this model; since a duplicated list caller can modify without impacting this TrainingModel.
    ///</summary>
    PicesClassList^  MLClasses ();


    /// <summary> Fully expanded directory path for specified class.</summary>
    /// <param name="mlClass">Class that we want to get directory path for training images.</param>
    /// <returns> Returns full path where images for mlClass are stored; directory path is fully expended; if class not defined in configuration will return nullptr. </returns>
    String^  DirectoryPathForClass (PicesClass^  mlClass);


    static
    array<String^>^  GetListOfTrainingModels ();

    bool  IncludesClass (PicesClass^  mlClass); 

    String^  LastRunLogTextLine ();

    void  BuildNewTrainedModel ();

    void  LoadExistingTrainedModel ();

    void  LoadExistingModelOtherwiseBuild (PicesMsgQueue^  msgQueue);  /**< Will attach message queue to runLog */

    void  LoadTrainigLibrary (bool  forceRebuild);


    void  LoadTrainingModelForGivenLevel (uint            level,
                                          PicesMsgQueue^  msgQueue  /**< Will attach message queue to runLog */
                                         );

    PicesPrediction^  PredictClass (PicesFeatureVector^  example);

    void  PredictClass (PicesFeatureVector^  featureVector,
                        PicesPrediction^     prediction1,
                        PicesPrediction^     prediction2
                       );

    void  PredictClass (System::String^   imageFileName,
                        System::Array^    raster,
                        System::Single^   depth,
                        System::Single^   salinity, 
                        System::Single^   oxygen, 
                        System::Single^   florences,
                        PicesPrediction^  prediction1,
                        PicesPrediction^  prediction2
                       );


    ///<summary> This call assumes that there was a successful call to PredictProbabilities before. </summary>
    PicesPredictionList^   BinaryProbailitiesForClass (PicesClass^  leftClass);

    array<PicesInterface::ProbNamePair^>^  
                           FindWorstSupportVectors (PicesFeatureVector^  picesFeatureVector,
                                                    int                  numToFind,
                                                    PicesClass^          c1,
                                                    PicesClass^          c2
                                                   );

    array<PicesInterface::ProbNamePair^>^  
                           FindWorstSupportVectors2 (PicesFeatureVector^  picesFeatureVector,
                                                     int                  numToFind,
                                                     PicesClass^          c1,
                                                     PicesClass^          c2
                                                    );

    PicesPredictionList^   PredictProbabilities (System::String^  imageFileName,
                                                 System::Array^   raster,
                                                 System::Single^  depth,
                                                 System::Single^  salinity, 
                                                 System::Single^  oxygen, 
                                                 System::Single^  florences
                                                );

    PicesPredictionList^   PredictProbabilities (System::String^   imageFileName,
                                                 PicesRasterList^  saveCalcImages   /**< If Not NULL intermediate instances created during feature calculation will be appended to this list. */
                                                );

    PicesPredictionList^   PredictProbabilities (PicesFeatureVector^  picesFeatureVector);


    void   ProbabilitiesByClassDual (PicesFeatureVector^    picesFeatureVector,
                                     String^%               classifier1Desc,
                                     String^%               classifier2Desc,
                                     PicesPredictionList^%  classifier1Results,
                                     PicesPredictionList^%  classifier2Results
                                    );

    void  SaveConfiguration ();

    /// <summary> Returns the names of the training examples that became support vectors in the binary classifier that was built for the class pair c1 and c2 .</summary>
    /// <param name="c1"> The first PicesClass.</param>
    /// <param name="c2"> The second PicesClass.</param>
    /// <returns> nullptr if there is not a SVM for the pair of classes specified otherwise a list of the examples that are S/Vs.</returns>
    array<String^>^  SupportVectorNames (PicesClass^ c1,  PicesClass^ c2);

	private:

    Bitmap^  BuildBitmapFromRasterData (uchar**  r,
                                        int      height,
                                        int      width
                                       );

    void CreateRunLog ();

    void ErrorMsgsClear ();
    void ErrorMsgsAdd (String^ errorMsg);
    void ErrorMsgsAdd (const VectorKKStr&  _errorMsgs);


    PicesTrainingConfigurationConstPtr  GetConfigToUse ();

    void  PopulateCSharpClassList ();

    void  UpdateMemoryPressure ();


    bool*                         cancelFlag;
    Classifier2Ptr                classifier;
    MLClassListPtr                classes;
    PicesTrainingConfigurationPtr config;
    double**                      crossProbTable;
    int                           crossProbTableNumClasses;
    int                           curMemoryPressure;
    List<String^>^                errorMsgs;     /**< Error messages for later recall are added to this list. */
    bool                          loadTrainingLibraryRunning;
    System::String^               modelName;
    PicesRunLog^                  picesRunLog;
    double*                       probabilities;
    RunLogPtr                     runLog;
    String^                       runLogFileName;
    int                           runLogLastLineNum;
    PicesMsgQueue^                runLogMsgQueue;
    TrainingProcess2Ptr           trainer;
    bool                          trainerWeOwn;
    bool*                         valid;     // True if Training Library Valid
    int*                          votes;

    PicesClassList^               classList;
    array<Pen^>^                  colorValues;
  };
}
