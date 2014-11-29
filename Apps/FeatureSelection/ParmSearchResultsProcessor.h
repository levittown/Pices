#ifndef  _PARMSEARCHRESULTSPROCESSOR_
#define  _PARMSEARCHRESULTSPROCESSOR_

#include  "BinaryJob.h"
#include  "FeatureVector.h"

namespace FeatureSelectionApp
{
  #ifndef  _FEATURESELECTION_
  class  FeatureSelection;
  typedef  FeatureSelection*  FeatureSelectionPtr;
  #endif


  #ifndef  _PROCESSOR_
  class  Processor;
  typedef  Processor*  ProcessorPtr;
  #endif


  class ParmSearchResultsProcessor
  {
  public:
    ParmSearchResultsProcessor (ProcessorPtr _processor);

    ~ParmSearchResultsProcessor ();

    BinaryJobListPtr  GetBestCandidates ();


  private:
    FeatureSelectionPtr  featureSelection;
    ProcessorPtr         processor;

    FeatureVectorListPtr  trainingData;
    FeatureVectorListPtr  testData;
  };  /* ParmSearchResultsProcessor */
}  /* FeatureSelection */

#endif
