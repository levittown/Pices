#include "FirstIncludes.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include <sys/types.h>
#ifdef  WIN32
#include <io.h>
#include <windows.h>
#else
//#include  <sys/loadavg.h>
#include <unistd.h>
#endif


#include "KKBaseTypes.h"
#include "StatisticalFunctions.h"
#include "KKStr.h"
using namespace KKB;


#include "MLClass.h"
using namespace KKMLL;


#include "FeatureSelectionTypes.h"
using namespace  FeatureSelectionApp;



ProcessingOrders  FeatureSelectionApp::ProcessingOrderFromStr (const KKStr&  procOrderStr)
{
  if  (procOrderStr.EqualIgnoreCase ("StandardOrder")  ||  procOrderStr.EqualIgnoreCase ("Standard"))
    return  ProcessingOrders::Standard;

  if  (procOrderStr.EqualIgnoreCase ("HallOrder")      ||   procOrderStr.EqualIgnoreCase ("Hall"))
    return  ProcessingOrders::Hall;

  return  ProcessingOrders::Null;
}



KKStr  FeatureSelectionApp::ProcessingOrderToStr (ProcessingOrders  order)
{
  if  (order == ProcessingOrders::Standard)
    return "StandardOrder";

  if  (order == ProcessingOrders::Hall)
    return "HallOrder";

  return "";
}




MajorSteps  FeatureSelectionApp::MajorStepFromStr (KKStr  majorStepStr)
{
  majorStepStr.Upper ();

  if  (majorStepStr.EqualIgnoreCase ("ALLCLASSESPARMTUNING")  ||  majorStepStr.EqualIgnoreCase ("MfsParmTuningPre"))
    return  MajorSteps::MfsParmTuningPre;

  if  (majorStepStr.EqualIgnoreCase ("ALLCLASSESFEATURESELECTION")  ||  majorStepStr.EqualIgnoreCase ("MfsFeatureSelection"))
    return  MajorSteps::MfsFeatureSelection;

  if  (majorStepStr.EqualIgnoreCase ("MfsParmTuningPost"))
    return MajorSteps::MfsParmTuningPost;

  if  (majorStepStr.EqualIgnoreCase ("BINARYCLASSESFEATURESELECTION")  ||  majorStepStr.EqualIgnoreCase ("BfsFeatureSelection"))
    return  MajorSteps::BfsFeatureSelection;

  if  (majorStepStr.EqualIgnoreCase ("BfsParmTuningPre"))
    return MajorSteps::BfsParmTuningPre;

  if  ((majorStepStr == "BINARYCLASSESPARMTUNING")  ||  majorStepStr.EqualIgnoreCase ("BfsParmTuningPost"))
    return  MajorSteps::BfsParmTuningPost;

  if  ((majorStepStr == "GENERATEFINALERESULTS")  ||  (majorStepStr == "GENERATEFINALRESULTS"))
    return  MajorSteps::GenerateFinalResults;

  if  ((majorStepStr == "FINALEREPORT")  ||  (majorStepStr == "FINALREPORT"))
    return  MajorSteps::FinalReport;

  if  (majorStepStr.EqualIgnoreCase ("GenerateBruitSvmSearch"))
    return  MajorSteps::GenerateBruitSvmSearch;

  if  (majorStepStr == "ALLDONE")
    return  MajorSteps::Done;

  return  MajorSteps::Null;
}  /* MajorStepFromStr */




KKStr  FeatureSelectionApp::MajorStepToStr (MajorSteps  step)
{
  if  (step == MajorSteps::MfsParmTuningPre)
    return  "MfsParmTuningPre";

  if  (step == MajorSteps::MfsFeatureSelection)
    return  "MfsFeatureSelection";

  if  (step == MajorSteps::MfsParmTuningPost)
    return "MfsParmTuningPost";

  if  (step == MajorSteps::BfsFeatureSelection)
    return  "BfsFeatureSelection";

  if  (step == MajorSteps::BfsParmTuningPre)
    return  "BfsParmTuningPre";

  if  (step == MajorSteps::BfsParmTuningPost)
    return  "BfsParmTuningPost";

  if  (step == MajorSteps::GenerateFinalResults)
    return  "GenerateFinalResults";

  if  (step == MajorSteps::GenerateBruitSvmSearch)
    return  "GenerateBruitSvmSearch";

  if  (step == MajorSteps::FinalReport)
    return  "FinalReport";

  if  (step == MajorSteps::Done)
    return  "AllDone";

  return  "NULL";
}  /* MajorStepFromStr */



FinalResultType  FeatureSelectionApp::FinalResultTypeFromStr (const KKStr&  s)
{
  if  (s.EqualIgnoreCase ("NoTuningAllFeatures"))
    return FinalResultType::NoTuningAllFeatures;

  if  (s.EqualIgnoreCase ("MfsFeaturesSel"))
    return FinalResultType::MfsFeaturesSel;

  if  (s.EqualIgnoreCase ("FeaturesAllParmsTuned")  ||  s.EqualIgnoreCase ("MfsParmsTuned"))
    return FinalResultType::MfsParmsTuned;

  if  (s.EqualIgnoreCase ("FeaturesSelParmsTuned")  ||  s.EqualIgnoreCase ("MfsParmsTunedFeaturesSel"))
    return FinalResultType::MfsParmsTunedFeaturesSel;

  if  (s.EqualIgnoreCase ("BinaryFeaturesSel")  ||   s.EqualIgnoreCase ("BfsFeaturesSel"))
    return FinalResultType::BfsFeaturesSel;

  if  (s.EqualIgnoreCase ("BfsParmsTuned"))
    return FinalResultType::BfsParmsTuned;

  if  (s.EqualIgnoreCase ("BinaryFeaturesSelParmsTuned")  ||  s.EqualIgnoreCase ("BfsFeaturesSelParmsTuned"))
    return FinalResultType::BfsFeaturesSelParmsTuned;

  return  FinalResultType::Null;
}



KKStr  FeatureSelectionApp::FinalResultTypeToStr (FinalResultType  frt)
{
  switch  (frt)
  {
  case  FinalResultType::Null:                        return "";                             break;
  case  FinalResultType::NoTuningAllFeatures:         return "NoTuningAllFeatures";          break;
  case  FinalResultType::MfsFeaturesSel:              return "MfsFeaturesSel";               break;
  case  FinalResultType::MfsParmsTuned:               return "MfsParmsTuned";                break;
  case  FinalResultType::MfsParmsTunedFeaturesSel:    return "MfsParmsTunedFeaturesSel";     break;
  case  FinalResultType::BfsFeaturesSel:              return "BfsFeaturesSel";               break;
  case  FinalResultType::BfsParmsTuned:               return "BfsParmsTuned";                break;
  case  FinalResultType::BfsFeaturesSelParmsTuned:    return "BfsFeaturesSelParmsTuned";     break;
  }
  return "";
}



SearchTypes  FeatureSelectionApp::SearchTypeFromStr (KKStr  searchTypeStr)
{
  searchTypeStr.Upper ();

  if  ((searchTypeStr == "PARM")       ||
       (searchTypeStr == "PARAM")      ||
       (searchTypeStr == "PARAMETER")  ||
       (searchTypeStr == "P")
      )
    return  SearchTypes::ParameterSearch;

  if  ((searchTypeStr == "FEATURE")   ||
       (searchTypeStr == "F")
      )
    return  SearchTypes::FeatureSelectionSearch;

  if  ((searchTypeStr == "FINALEREPORTS")                         ||
       (searchTypeStr == "GENERATINGFINALEREPORTS")               ||
       (searchTypeStr == "FINALEREPORTS")                         ||
       (searchTypeStr.EqualIgnoreCase ("GenerateFinaleResults"))  ||
       (searchTypeStr.EqualIgnoreCase ("GenerateFinalResults"))
      )
    return  SearchTypes::GeneratingFinalResults;

  return SearchTypes::Null;
}  /* SerachTypeFromStr */




KKStr  FeatureSelectionApp::SearchTypeToStr (SearchTypes  _searchType)
{
  if  (_searchType == SearchTypes::FeatureSelectionSearch)
    return  "Feature";

  if  (_searchType == SearchTypes::ParameterSearch)
    return  "Parameter";

  if  (_searchType == SearchTypes::GeneratingFinalResults)
    return  "GenerateFinalResults";

  return  "NULL";
}  /* SearchTypeToStr */




KKStr  FeatureSelectionApp::GradingMethodToStr (GradingMethodType  gm)
{
  if  (gm == GradingMethodType::Accuracy)
    return  "Accuracy";
  
  else if  (gm == GradingMethodType::AccuracyNorm)
    return  "AccuracyNorm";
  
  else if  (gm == GradingMethodType::FMeasure)
    return  "FMeasure";

  else 
    return  "Null";
}  /* GradingMethodFromStr */




GradingMethodType  FeatureSelectionApp::GradingMethodFromStr (const KKStr&  str)
{
  if  ((str.CompareIgnoreCase ("Accuracy") == 0)  ||  (str.CompareIgnoreCase ("Accuracy") == 0)  ||  (str.CompareIgnoreCase ("AccuracyMethod") == 0))
    return  GradingMethodType::Accuracy;

  if  ((str.CompareIgnoreCase ("AccuracyNorm") == 0)  ||  (str.CompareIgnoreCase ("AccuracyNorm") == 0)  ||  (str.CompareIgnoreCase ("AccuracyNormMethod") == 0))
    return  GradingMethodType::AccuracyNorm;

  if  ((str.CompareIgnoreCase ("F-Measure") == 0)  ||  (str.CompareIgnoreCase ("FMeasure") == 0)  ||  (str.CompareIgnoreCase ("FMeasure") == 0))
    return  GradingMethodType::FMeasure;

  return GradingMethodType::Null;
}  /* GradingMethodFromStr */







KKStr  FeatureSelectionApp::JobSelectionCriteriaToStr (JobSelectionCriteria  jsc)
{
  switch  (jsc)
  {
    case  JobSelectionCriteria::MostAccurate:     return  "MostAccurate";
    case  JobSelectionCriteria::FastestFromBest:  return  "FastestFromBest";
    case  JobSelectionCriteria::BruitForce:       return  "BruitForce";
  }

  return  "NULL";
}




JobSelectionCriteria  FeatureSelectionApp::JobSelectionCriteriaFromStr (const KKStr&  str)
{
  KKStr  s = str.ToUpper ();
  if  ((s == "MOSTACCURATE")  ||  (s == "MA"))
    return  JobSelectionCriteria::MostAccurate;

  if  ((s == "FASTESTFROMBEST")  ||  (s == "FFB"))
    return  JobSelectionCriteria::FastestFromBest;

  if  ((s == "BRUITFORCE")       ||  (s == "BF"))
    return  JobSelectionCriteria::BruitForce;

  return  JobSelectionCriteria::Null;
}










//*************************************************************************
//*    Responsible for keep track of which set of Binary Classes a given  *
//*  Processor is responsible for.                                        *
//*                                                                       *
//*************************************************************************
KKStr  FeatureSelectionApp::SearchMethodToStr (SearchMethod  searchMethod)
{
  if  (searchMethod == SearchMethod::BestCaseNext)
    return "BestCaseNext";

  if  (searchMethod == SearchMethod::Beam)
    return "Beam";

  if  (searchMethod == SearchMethod::TestResults)
    return  "TestResults";

  if  (searchMethod == SearchMethod::PickTheBest)
    return  "PickTheBest";

  if  (searchMethod == SearchMethod::Grid)
    return "Grid";

  if  (SearchMethod::OnePassOnly == SearchMethod::OnePassOnly)
    return "OnePassOnly";

  return "";
}  /* SearchMethodToStr */





SearchMethod  FeatureSelectionApp::SearchMethodFromStr (KKStr  searchMethodStr)
{
  searchMethodStr.Upper ();

  if  (searchMethodStr      == "BESTCASENEXT")
    return SearchMethod::BestCaseNext;

  else if  (searchMethodStr == "BEAM")
    return SearchMethod::Beam;

  else if  (searchMethodStr == "TESTRESULTS")
    return SearchMethod::TestResults;

  else if  (searchMethodStr == "PICKTHEBEST")
    return SearchMethod::PickTheBest;

  else if  (searchMethodStr == "GRID")
    return SearchMethod::Grid;

  else if  (searchMethodStr == "ONEPASSONLY")
    return SearchMethod::OnePassOnly;

  else
    return SearchMethod::Null;
}  /* SearchMethodToStr */





KKStr  FeatureSelectionApp::ProcessorStatusToStr (ProcessorStatus  processorStatus)
{
  if  (processorStatus == ProcessorStatus::NotStarted)
    return "NotStarted";

  if  (processorStatus == ProcessorStatus::Started)
    return "Started";

  if  (processorStatus == ProcessorStatus::Done)
    return "Done";

  return "";
}  /* ProcessorStatusToStr */





ProcessorStatus  FeatureSelectionApp::ProcessorStatusFromStr (const KKStr&  processorStatusStr)
{
  if  (processorStatusStr.EqualIgnoreCase ("NOTSTARTED"))
    return  ProcessorStatus::NotStarted;

  if  (processorStatusStr.EqualIgnoreCase ("STARTED"))
    return ProcessorStatus::Started;
  
  if  (processorStatusStr.EqualIgnoreCase ("DONE"))
    return ProcessorStatus::Done;

  return  ProcessorStatus::Null;
}  /* ProcessorStatusFromStr */



KKStr  FeatureSelectionApp::FeatureCriteriaToStr (FeatureCriteriaType  featureCriteria)
{
  switch  (featureCriteria)
  {
  case  FeatureCriteriaType::Merge2Best:     return  "Merge2Best";
  case  FeatureCriteriaType::Merge3Best:     return  "Merge3Best";
  case  FeatureCriteriaType::Merge4Best:     return  "Merge4Best";
  case  FeatureCriteriaType::Merge5Best:     return  "Merge5Best";
  case  FeatureCriteriaType::Merge6Best:     return  "Merge6Best";
  case  FeatureCriteriaType::Merge7Best:     return  "Merge7Best";
  case  FeatureCriteriaType::BestTestSet:    return  "BestTestSet";
  case  FeatureCriteriaType::RemoveHurtful:  return  "RemoveHurtful";
  }

  return  "";
}  /* FeatureCriteriaToStr */



FeatureCriteriaType  FeatureSelectionApp::FeatureCriteriaFromStr (KKStr  featureCriteriaStr)
{
  featureCriteriaStr.Upper ();
  if  ((featureCriteriaStr == "MERGE2BEST")  ||  (featureCriteriaStr == "M2B"))
    return  FeatureCriteriaType::Merge2Best;

  if  ((featureCriteriaStr == "MERGE3BEST")  ||  (featureCriteriaStr == "M3B"))
    return  FeatureCriteriaType::Merge3Best;

  if  ((featureCriteriaStr == "MERGE4BEST")  ||  (featureCriteriaStr == "M4B"))
    return  FeatureCriteriaType::Merge4Best;

  if  ((featureCriteriaStr == "MERGE5BEST")  ||  (featureCriteriaStr == "M5B"))
    return  FeatureCriteriaType::Merge5Best;

  if  ((featureCriteriaStr == "MERGE6BEST")  ||  (featureCriteriaStr == "M6B"))
    return  FeatureCriteriaType::Merge6Best;

  if  ((featureCriteriaStr == "MERGE7BEST")  ||  (featureCriteriaStr == "M7B"))
    return  FeatureCriteriaType::Merge7Best;

  if  ((featureCriteriaStr == "BESTTESTSET")  ||  (featureCriteriaStr == "BTS"))
    return  FeatureCriteriaType::BestTestSet;

  if  ((featureCriteriaStr == "REMOVEHURTFUL")  ||  (featureCriteriaStr == "RH"))
    return  FeatureCriteriaType::RemoveHurtful;

  return  FeatureCriteriaType::Null;
}  /* FeatureCriteriaFromStr */




ProcessorCycles::ProcessorCycles ():
  totalCycles (0.0)
{
}


ProcessorCycles::ProcessorCycles (const ProcessorCycles&  _cycles):
   totalCycles (0.0)
{
  const_iterator  idx;
  for  (idx = _cycles.begin ();  idx != _cycles.end ();  idx++)
    AddCycles (idx->first, idx->second);
}



void  ProcessorCycles::AddCycles (int     _procId,
                                  double  _cycles
                                 )
{
  map<int,double>::iterator  idx = find (_procId);
  if  (idx == end ())
    insert (pair<int, double> (_procId, _cycles));
  else
    idx->second += _cycles;
  totalCycles += _cycles;
}




void  ProcessorCycles::AddCycles (const ProcessorCycles&  _cycles)
{
  const_iterator  idx;
  for  (idx = _cycles.begin ();  idx != _cycles.end ();  idx++)
    AddCycles (idx->first, idx->second);
}



double  ProcessorCycles::TotalCycles ()  const
{
  double  totalCycles = 0.0;
  const_iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
    totalCycles += idx->second;
  return  totalCycles;
}



double  ProcessorCycles::LongestPath ()  const
{
  double  longestPath = 0.0;
  const_iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
  {
    if  (idx->second > longestPath)
      longestPath = idx->second;
  }

  return  longestPath;
}



VectorInt  ProcessorCycles::ProcessorIds ()  const
{
  VectorInt  procIds;
  const_iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
    procIds.push_back (idx->first);
  return  procIds;
}



VectorDouble  ProcessorCycles::CyclesByProcId ()  const
{
  VectorDouble  cycles;
  const_iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
    cycles.push_back (idx->second);
  return  cycles;
}


void  ProcessorCycles::Report (ostream&  r)
{
  kkuint32  z;

  double        totalCycles = TotalCycles ();
  VectorInt     procIds     = ProcessorIds ();
  VectorDouble  cycles      = CyclesByProcId ();

  double  cyclesTotal  = 0.0;
  double  cyclesMean   = 0.0;
  double  cyclesStdDev = 0.0;
  double  cyclesMin    = 0.0;
  double  cyclesMax    = 0.0;

  CalcVectorStats (cycles, cyclesTotal, cyclesMean, cyclesStdDev, cyclesMin, cyclesMax);

  r << "Total";
  r << "Cycles" << "\t" << "Mean" << "\t" << "StdDev" << "\t" << "Min" << "\t" << "Max";
  for  (z = 0;  z < procIds.size ();  z++)
     r << "\t" << procIds[z];
  r << endl;

  r << cyclesTotal << "\t" << cyclesMean << "\t" << cyclesStdDev << "\t" << cyclesMin << "\t" << cyclesMax;
  for  (z = 0;  z < cycles.size ();  z++)
      r << "\t" << cycles[z];
  r << endl;
}
