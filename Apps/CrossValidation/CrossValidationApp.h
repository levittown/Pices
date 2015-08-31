#ifndef  _CROSSVALIDATIONAPP_
#define  _CROSSVALIDATIONAPP_


#include "DateTime.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;

#include "ConfusionMatrix2.h"
#include "FactoryFVProducer.h"
#include "FeatureNumList.h"
#include "FileDesc.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "Orderings.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;

#include "picesTrainingConfiguration.h"
using namespace  MLL;



class  CrossValidationRun;
typedef  CrossValidationRun*  CrossValidationRunPtr;


class  CrossValidationApp
{
public:
  typedef  KKB::uint  uint;

  CrossValidationApp ();

  ~CrossValidationApp ();

  bool                       generateBiasMatrix;  /**< Create a simple Confusion matrix that is to be stored with the
                                                   * classifier. This matrix will then be used to adjust classification
                                                   * results for learner bias.
                                                   */

  bool                       cancelFlag;

  TrainingConfiguration2Ptr  config;
  KKStr                      configFileName;

  bool                       copyMissClassedImages;

  bool                       crossValidationByHierarcy;

  KKStr                      directory;

  bool                       doRandomSplits;

  DateTime                   endTime;

  FactoryFVProducerPtr       fvFactoryProducer;

  bool                       featuresAreNormalizedAlready;

  FileDescPtr                fileDesc;

  FeatureVectorListPtr       examples;

  MLClassListPtr             mlClasses;

  FeatureFileIOPtr           inputFormat;
  bool                       inputFormatSpecified;

  KKStr                      loadFileName;

  kkint32                    logLossCount;
  double                     logLossTotal;
  double                     logLossMax;
  double                     logLossMin;

  KKStr                      missedClassedDir; /**< Directory where miss classified examples should be copied to. */

  MLClassPtr                 noiseMLClass;

  int                        numOfFolds;

  int                        numRandomSplits;

  ofstream*                  predictionReport;

  KKStr                      predictionReportName;

  ofstream*                  probReport;

  KKStr                      probReportName;

  float                      punishmentFactor;

  //  If greater than 0, will indicate that we want to randomly select that many features 
  //  from the ones that are already specified in the configuration file. Zero (0) means no 
  //  random feature selection
  int                        randomFeatures;

  //  If set to true will randomize the order of the test data
  bool                       randomizeInputData;

  //  Will only be used if randomFeatures > 0,  will be set to 
  //  the features that are randomly selected from the ones specified in the
  //  configuration file.
  FeatureNumListPtr          randomlySelectedFeatureNums;

  int                        randomSetWidth;

  KKStr                      reportFileName;

  ostream*                   report;

  ofstream*                  reportFileStream;

  RunLog                     runLog;

  KKStr                      saveFileName;         /**< file name to save extracted examples into if user specifies -s option. */

  float                      splitPercentage;      /**< A number greater than 0 and less than 100.0%  */

  DateTime                   startTime;

  bool                       tryEachFeature;       /**< If set to true,  will rerun the Cross Validation once for each feature. */

  FeatureVectorListPtr       validationData;

  KKStr                      validationFileName;

  void                       WriteABiasMatrix (ConfusionMatrix2&  cm);



  void  CheckForDuplicates ();


  void  CmdLineHelp ();


  void  CrossValidate (FeatureVectorListPtr  testImages, 
                       FeatureVectorListPtr  trainingImages,
                       ConfusionMatrix2&     cm,
                       ConfusionMatrix2&     cm2cdGuess,
                       ConfusionMatrix2&     cmSmall,
                       ConfusionMatrix2&     cmMed,
                       ConfusionMatrix2&     cmLarge,
                       int&                  foldCount,
                       double&               foldAccuracy,
                       int                   foldNum,
                       double&               trainingTime,
                       double&               classificationTime,
                       int&                  supportPoints
                      );


  void  CrossValidationsByHierarchy ();


  void  DistributesImagesRandomlyWithInFolds (int    imagesPerClass,
                                              bool&  successful
                                             );


  KKStr  LocateImageFileUsingConfigurationFile (const  KKStr&   fileName,
                                                MLClassPtr      knownClass
                                               );

  void   ProcessCmdLineParameters (int     argc, 
                                   char**  argv
                                  );

  void   ProcessCmdLineParameter (KKStr  parmSwitch, 
                                  KKStr  parmValue 
                                 );


  void   PerformBinaryClassCrossValidations ();


  void   PerformTrueBinaryClassCrossValidations ();


  void   PrintSizeHistogram ();


  void   PostMisclassifiedImage (KKStr       fileName,
                                 MLClassPtr  knownClass,
                                 MLClassPtr  predictedClass
                                );


  void  RandomlySelectedProcessResults (vector<CrossValidationRunPtr>&  crossValidations,
                                        int                             numToCombine,
                                        KKStr                           order
                                       );

  void  RandomlySelectedResults (vector<CrossValidationRunPtr>&  crossValidations,
                                 KKStr                           order
                                );

  FeatureVectorListPtr  RandomlySampleData (const   FeatureVectorListPtr data, 
                                            float   percentToKeep, 
                                            double& samplingTime
                                           );


  void  RandomSplitsIntoTrainAndTest (FeatureVectorListPtr   splitData, 
                                      FeatureVectorListPtr&  splitDataTrain, 
                                      FeatureVectorListPtr&  splitDataTest
                                     );


  void  RemoveDuplicateImages (FeatureVectorList&  examples);


  void  ReportCmdLineParameters ();


  void  ReportConfigurationInfo ();


  void  TestBitreductionPerformance ();

  void  TestRandomSamplingTimings ();

  void  TryEachFeature (FeatureVectorListPtr  trainingImages,
                        ostream&              r
                       );


  void  ValidationProcess ();
  int   Main (int  argc,  char** argv);

};  /* CrossValidationApp */



typedef   CrossValidationApp*  CrossValidationAppPtr;
#endif
