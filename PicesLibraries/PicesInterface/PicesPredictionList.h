#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;


#include  "PicesPrediction.h"



namespace PicesInterface
{
  public ref  class PicesPredictionList:  List<PicesPrediction^>
  {
  public:
    PicesPredictionList ();

    PicesPredictionList (int  size);

    PicesPredictionList (KKMLL::ClassProbList&  predictions);

    PicesPrediction^  LookUpByClass (PicesClass^ c);

    void  SortByProbabilityHighToLow ();
    void  SortByVotingHighToLow ();

  private:
    ref class SortPredictionsByProbability;
    ref class SortPredictionsByVotes;
  };
}