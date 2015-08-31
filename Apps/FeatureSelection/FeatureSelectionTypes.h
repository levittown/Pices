#ifndef  _FEATURESELECTIONTYPES_
#define  _FEATURESELECTIONTYPES_

#include  "KKStr.h"



namespace FeatureSelectionApp
{
  enum class  GradingMethodType: int
  {
    Null, 
    Accuracy,
    AccuracyNorm,
    FMeasure
  };


  enum class  JobSelectionCriteria: int
  {
    Null,
    MostAccurate,      /**< Select the job that has the highest Accuracy all other criteria is secondary. */
    FastestFromBest,   /**< Select the Fastest Job from the group of jobs that are most accurate,         */
    BruitForce         /**< Perform a plain bruit force Grid Search                                       */
  }; 


  enum class  ProcessingOrders: int
  {
    Null,
    Standard,  // AllClasses (Parameter,  FeatureSelection)  Binary (AllParam, FeatureSel, Parameter);
    Hall       // AllClasses (FeatureSel, ParamTuning)       Binary (ParamTuning, FeatureSel, ParamTuning);
  };




  /**
   @brief  MajorSteps that need to be processed.
   @details Depending on the Processing Order the FeatureSelection experiment will follow a specific order of major steps.
   @code
     Standard:   MajorSteps::MfsParmTuningPre,    MajorSteps::MfsFeatureSelection, MajorSteps::BfsFeatureSelection, MajorSteps::BfsParmTuningPost
     Hall:       MajorSteps::MfsFeatureSelection, MajorSteps::MfsParmTuningPost,   MajorSteps::BfsParmTuningPre,    MajorSteps::BfsFeatureSelection, MajorSteps::BfsParmTuningPost
   @endcode
   */
  enum class  MajorSteps: int
  {
    Null,

    MfsParmTuningPre,
    MfsFeatureSelection,
    MfsParmTuningPost,

    BfsParmTuningPre,
    BfsFeatureSelection,
    BfsParmTuningPost,

    GenerateFinalResults,
    FinalReport,
    GenerateBruitSvmSearch,
    Done
  };



  enum class  FinalResultType: int
  {
    Null,
    NoTuningAllFeatures,
    MfsFeaturesSel,
    MfsParmsTuned,
    MfsParmsTunedFeaturesSel,
    BfsFeaturesSel,
    BfsParmsTuned,
    BfsFeaturesSelParmsTuned
  };



  enum class  SearchTypes: int
  {
    Null,
    FeatureSelectionSearch,
    ParameterSearch,
    GeneratingFinalResults
  };



  enum class  ProcessorStatus: int
  {
    Null,
    NotStarted,
    Started,
    Done
  };


  enum class  SearchMethod: int
  {
    Null, 
    BestCaseNext,     //   
    Beam,             //  > Feature Search
    TestResults,      //    
    PickTheBest,      //
    Grid,             // Used by Parameter Search only
    OnePassOnly       // Means there are no expansions;  just process the initial Jobs.
  };



  enum class  FeatureCriteriaType: int
  {
    Null,
    Merge2Best,
    Merge3Best,
    Merge4Best,
    Merge5Best,
    Merge6Best,
    Merge7Best,
    BestTestSet,
    RemoveHurtful
  };






  class  ProcessorCycles: public  map<int, double>
  {
  public:
    ProcessorCycles ();
    
    ProcessorCycles (const ProcessorCycles&  _cycles);

    void  AddCycles (int     _procId,
                     double  _cycles
                    );

    void  AddCycles (const ProcessorCycles&  _cycles);

    double  LongestPath ()  const;  /**< returns the Number of CPU seconds that the  longets process used. */

    double  TotalCycles ()  const;

    VectorInt     ProcessorIds   ()  const;
    VectorDouble  CyclesByProcId ()  const;
    double        totalCycles;

    void  Report (ostream&  r);
  };





  ProcessingOrders  ProcessingOrderFromStr (const KKStr&  procOrderStr);
  KKStr              ProcessingOrderToStr   (ProcessingOrders  order);




  MajorSteps   MajorStepFromStr  (KKStr  majorStepStr);
  KKStr        MajorStepToStr    (MajorSteps  step);



  FinalResultType  FinalResultTypeFromStr (const KKStr&  s);
  KKStr            FinalResultTypeToStr   (FinalResultType  frt);


  SearchTypes  SearchTypeFromStr (KKStr  searchTypeStr);
  KKStr          SearchTypeToStr   (SearchTypes  _searchType);



  KKStr  GradingMethodToStr (GradingMethodType  gm);
  GradingMethodType  GradingMethodFromStr (const KKStr&  str);



  KKStr  JobSelectionCriteriaToStr (JobSelectionCriteria  gm);
  JobSelectionCriteria  JobSelectionCriteriaFromStr (const KKStr&  str);



  KKStr              SearchMethodToStr      (SearchMethod     searchMethod);
  SearchMethod     SearchMethodFromStr    (KKStr              searchMethodStr);



  KKStr              ProcessorStatusToStr   (ProcessorStatus  processorStatus);
  ProcessorStatus  ProcessorStatusFromStr (const KKStr&       processorStatusStr);



  KKStr              FeatureCriteriaToStr   (FeatureCriteriaType  featureCriteria);
  FeatureCriteriaType  FeatureCriteriaFromStr (KKStr              featureCriteriaStr);
}

#endif

