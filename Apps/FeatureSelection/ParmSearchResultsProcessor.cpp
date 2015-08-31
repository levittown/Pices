#include "FirstIncludes.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace  std;


#include "KKBaseTypes.h"
#include "KKQueue.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;


#include "MLClass.h"
using namespace  KKMLL;


#include "ParmSearchResultsProcessor.h"

#include "BinaryJob.h"
#include "BinaryJobList.h"
#include "FeatureSelection.h"
#include "Processor.h"
using namespace  FeatureSelectionApp;



ParmSearchResultsProcessor::ParmSearchResultsProcessor (ProcessorPtr _processor):
  featureSelection (NULL),
  processor        (_processor),
  testData         (NULL),
  trainingData     (NULL)
{
  featureSelection = processor->FeatureSelection ();
}



BinaryJobListPtr  ParmSearchResultsProcessor::GetBestCandidates ()
{
  BinaryJobList::ErrorCodes  result = BinaryJobList::ErrorCodes::NoError;

  BinaryJobListPtr  jobs = processor->BinaryJobs ();

 
  // We will first get jobs with the highest 1% accuracy.
  jobs->SortByAccuracy ();

  BinaryJobListPtr  accuracyCandidateJobs = new BinaryJobList (processor);
  accuracyCandidateJobs->Owner (false);

  BinaryJobPtr  highAccuracyJob = jobs->IdxToPtr (0);
  float  highAccuracy = highAccuracyJob->Accuracy ();

  BinaryJobList::iterator  idx;
  for  (idx = jobs->begin ();  idx != jobs->end ();  idx++)
  {
    BinaryJobPtr  j = *idx;

    if  ((highAccuracy - j->Accuracy ()) >= 1.0f)
      break;

    accuracyCandidateJobs->PushOnBack (j, result);
    if  (result != BinaryJobList::ErrorCodes::NoError)
    {
      cerr << endl
        << "ParmSearchResultsProcessor::GetBestCandidates   ***ERROR***   Duplicate Job adding to 'accuracyCandidateJobs'." << endl
        << "    " << j->ToStatusStr () << endl
        << endl;
    }
  }


  // From the High accuracy jobs we want the ones that ran fastest.
  BinaryJobListPtr  finalCandidateJobs = new BinaryJobList (processor);
  finalCandidateJobs->Owner (false);

  accuracyCandidateJobs->SortDeltaProbAccuarcy ();
  BinaryJobPtr  shortestTimeJob = accuracyCandidateJobs->IdxToPtr (0);
  double  shortestTime = shortestTimeJob->ProcessingTime ();
  double  longestTime  = accuracyCandidateJobs->BackOfQueue ()->ProcessingTime ();
  double  threshholdTime = shortestTime + (longestTime - shortestTime) / 3.0f;

  for  (idx = accuracyCandidateJobs->begin ();  idx != accuracyCandidateJobs->end ();  idx++)
  {
    BinaryJobPtr  j = *idx;

    if  ((j->ProcessingTime () - shortestTime) > threshholdTime)
      break;

    finalCandidateJobs->PushOnBack (j, result);
    if  (result != BinaryJobList::ErrorCodes::NoError)
    {
      cerr << endl
        << "ParmSearchResultsProcessor::GetBestCandidates   ***ERROR***   Duplicate Job adding to 'finalCandidateJobs'." << endl
        << "    " << j->ToStatusStr () << endl
        << endl;
    }
  }

  delete  accuracyCandidateJobs;  accuracyCandidateJobs = NULL;

  return  finalCandidateJobs;
}  /* GetBestCandidate */


