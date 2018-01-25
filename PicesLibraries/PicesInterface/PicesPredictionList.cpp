#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;

#include "MemoryDebug.h"
#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "OSservices.h"
using namespace KKB;


#include "ClassProb.h"
#include "MLClass.h"
using namespace KKMLL;

//#include "PicesKKStr.h"


#include "PicesPredictionList.h"
#include "PicesClass.h"
#include "PicesClassList.h"
#include "PicesKKStr.h"
#include "PicesOSservices.h"

using namespace  PicesInterface;


PicesPredictionList::PicesPredictionList (): List<PicesPrediction^> ()  {}

PicesPredictionList::PicesPredictionList (int  size): List<PicesPrediction^> (size) {}



PicesPredictionList::PicesPredictionList (ClassProbList&  predictions):
       List<PicesPrediction^> ((int)predictions.size ())

{
  ClassProbList::const_iterator  idx;
  for  (idx = predictions.begin ();  idx != predictions.end ();  ++idx)
  {
    ClassProbPtr  cp = *idx;
    MLClassPtr  ic = cp->classLabel;
    if  (!ic)
      continue;

    PicesClass^ pc = PicesClassList::GetUniqueClass (PicesKKStr::KKStrToSystenStr (ic->Name ()), PicesKKStr::KKStrToSystenStr (ic->ParentName ()));

    Add (gcnew PicesPrediction (pc, (int)(cp->votes), cp->probability));
  }
}


PicesPrediction^  PicesPredictionList::LookUpByClass (PicesClass^ c)
{
  PicesPrediction^  result = nullptr;

  for  (int x = 0;  x < Count;  ++x)
  {
    PicesPrediction^ p = (this)[x];
    if  (p->ClassName->Equals (c->Name, StringComparison::InvariantCultureIgnoreCase))
    {
      result = p;
      break;
    }
  }

  return  result;
}



    
ref class PicesPredictionList::SortPredictionsByProbability: public System::Collections::Generic::IComparer<PicesPrediction^>
{
public:
  virtual  int  Compare (PicesPrediction^ p1, PicesPrediction^ p2)
  {
    double  prob1 = p1->Probability;
    double  prob2 = p2->Probability;
        
    if  (prob1 < prob2)
      return 1;
    else if  (prob1 > prob2)
      return -1;
    else
      return 0;
  }
};  /* SortPredictionsByProbability */



ref class PicesPredictionList::SortPredictionsByVotes : public System::Collections::Generic::IComparer<PicesPrediction^>
{
public:
  virtual  int  Compare (PicesPrediction^ p1, PicesPrediction^ p2)
  {
    int     votes1 = p1->Votes;
    int     votes2 = p2->Votes;

    double  prob1 = p1->Probability;
    double  prob2 = p2->Probability;
        
    if  (votes1 < votes2)
      return 1;

    if  (votes1 > votes2)
      return -1;

    else if  (prob1 < prob2)
      return 1;

    else if  (prob1 > prob2)
      return -1;

    else
      return 0;
  }
};  /* SortPredictionsByVotes */



void  PicesPredictionList::SortByProbabilityHighToLow ()
{
  SortPredictionsByProbability^  comp = gcnew SortPredictionsByProbability ();
  Sort (comp);
}



void  PicesPredictionList::SortByVotingHighToLow ()
{
  SortPredictionsByVotes^  comp = gcnew SortPredictionsByVotes ();
  Sort (comp);
}
