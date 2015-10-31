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
#include <strstream>
#include <vector>

#include "MemoryDebug.h"
#include "KKBaseTypes.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

#include "GoalKeeper.h"
#include "Matrix.h"
using namespace  KKB;


#include "ClassStatistic.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "AbundanceCorrectionMatrix.h"
using namespace  MLL;


#include "PicesOSservices.h"
#include "PicesKKStr.h"
#include "PicesClass.h"
#include "PicesClassList.h"
#include "PicesMethods.h"
#include "PicesAbundanceCorrectionMatrix.h"
using namespace  PicesInterface;



PicesAbundanceCorrectionMatrix::PicesAbundanceCorrectionMatrix (PicesClassList^ _classes,
                                                                PicesClass^     _otherClass,
                                                                PicesRunLog^    _runLog
                                                               ):
   abundanceMatrix (NULL)
{
  MLClassListPtr  unManagedClasses = _classes->ToMLClassConstList ();
  abundanceMatrix = new AbundanceCorrectionMatrix (*unManagedClasses, _otherClass->UnmanagedMLClass (), _runLog->Log ());
  delete  unManagedClasses;
  unManagedClasses= NULL;
}



PicesAbundanceCorrectionMatrix::PicesAbundanceCorrectionMatrix (String^       _configFileName,
                                                                PicesRunLog^  _runLog
                                                               ):
   abundanceMatrix (NULL)
{
  abundanceMatrix = new AbundanceCorrectionMatrix (PicesKKStr::SystemStringToKKStr (_configFileName), _runLog->Log ());
}



PicesAbundanceCorrectionMatrix::!PicesAbundanceCorrectionMatrix ()
{
  delete  abundanceMatrix;
  abundanceMatrix = NULL;
}


PicesAbundanceCorrectionMatrix::~PicesAbundanceCorrectionMatrix ()
{
  this->!PicesAbundanceCorrectionMatrix ();
}


System::DateTime  PicesAbundanceCorrectionMatrix::DateTimeComputed ()
{
  return  PicesMethods::DateTimeKKUtoSystem (abundanceMatrix->DateTimeComputed ());
}


cli::array<float>^  PicesAbundanceCorrectionMatrix::ProbOfDetection ()
{
  const std::vector<float>&  v = abundanceMatrix->ProbOfDetection ();
  cli::array<float>^  result = gcnew cli::array<float> (v.size ());
  for  (kkuint32 x = 0;  x < v.size ();  ++x)
    result[x] = v[x];

  return  result;
}



cli::array<float>^     PicesAbundanceCorrectionMatrix::ProbOfFalseAlarm ()
{
  const std::vector<float>&  v = abundanceMatrix->ProbOfFalseAlarm ();
  cli::array<float>^  result = gcnew cli::array<float> (v.size ());
  for  (kkuint32 x = 0;  x < v.size ();  ++x)
    result[x] = v[x];

  return  result;
}

bool  PicesAbundanceCorrectionMatrix::Valid ()
{
  if  (!abundanceMatrix)
    return false;
  else
    return abundanceMatrix->Valid ();
}



void  PicesAbundanceCorrectionMatrix::AddIn (PicesAbundanceCorrectionMatrix^  matrix,
                                             PicesRunLog^                     runLog
                                            )
{
  abundanceMatrix->AddIn (*(matrix->abundanceMatrix), runLog->Log ());
}


void  PicesAbundanceCorrectionMatrix::ComputeStatistics ()
{
  abundanceMatrix->ComputeStatistics ();
}


void  PicesAbundanceCorrectionMatrix::Prediction (PicesClass^   knownClass,
                                                  PicesClass^   predClass,
                                                  PicesRunLog^  log
                                                 )
{
  abundanceMatrix->Prediction (knownClass->UnmanagedMLClass (), predClass->UnmanagedMLClass (), log->Log ());
}


void  PicesAbundanceCorrectionMatrix::PrintConfusionMatrixTabDelimited (StreamWriter^  outFile)
{
  std::ostrstream o;
  abundanceMatrix->PrintConfusionMatrixTabDelimited (o);
  char*  orig = o.str ();
  String^ systemString = gcnew String (orig);
  outFile->Write (systemString);
  delete systemString;
  systemString = nullptr;
}  /* PrintConfusionMatrixTabDelimited */



void  PicesAbundanceCorrectionMatrix::SaveForConfigFileName (String^  configFileName)
{
  abundanceMatrix->SaveForConfigFileName (PicesKKStr::SystemStringToKKStr (configFileName));
}



PicesClassStatisticList^  PicesAbundanceCorrectionMatrix::LumpCounts (PicesClassStatisticList^  inputCounts)
{
  ClassStatisticListPtr  unmanagedInputCounts = inputCounts->ToUnmanagedClassStatisticList ();
  ClassStatisticListPtr  lumpedCounts = abundanceMatrix->LumpCounts (*unmanagedInputCounts);

  PicesClassStatisticList^  results = gcnew PicesClassStatisticList (*lumpedCounts);

  delete  lumpedCounts;          lumpedCounts         = NULL;
  delete  unmanagedInputCounts;  unmanagedInputCounts = NULL;
  return  results;
}


PicesClassStatisticList^  PicesAbundanceCorrectionMatrix::AdjustClassificationCounts 
                                          (PicesClassStatisticList^  inputCounts,
                                           PicesRunLog^              log
                                          )
{
  ClassStatisticListPtr  unmanagedInputCounts = inputCounts->ToUnmanagedClassStatisticList ();
  ClassStatisticListPtr  adjustedCounts = abundanceMatrix->AdjustClassificationCounts (*unmanagedInputCounts, log->Log ());

  PicesClassStatisticList^  results = gcnew PicesClassStatisticList (*adjustedCounts);

  delete  adjustedCounts;       adjustedCounts       = NULL;
  delete  unmanagedInputCounts; unmanagedInputCounts = NULL;

  return  results;
}  /* AdjustClassificationCounts */



void  PicesAbundanceCorrectionMatrix::PrintAdjustedResults (StreamWriter^             sw,
                                                            PicesClassStatisticList^  classifiedCounts,
                                                            PicesRunLog^              runLog
                                                           )
{
  PicesClassStatisticList^  lumpedCounts = nullptr;
  PicesClassStatisticList^  adjCounts    = nullptr;

  System::DateTime  dateTimeParametersComputed = PicesMethods::DateTimeKKUtoSystem (abundanceMatrix->DateTimeComputed ());

  sw->WriteLine ();
  sw->WriteLine ();
  sw->WriteLine ("Abundance Adjustment:   Parameters Computed on " + dateTimeParametersComputed.ToLongTimeString ());
  sw->WriteLine ();


  try
  {
    lumpedCounts = LumpCounts (classifiedCounts);
    adjCounts = AdjustClassificationCounts (lumpedCounts, runLog);
  }
  catch  (Exception^  eeee)
  {
    sw->WriteLine ();
    sw->WriteLine ("     ***ERROR***          ***ERROR***          ***ERROR***");
    sw->WriteLine ();
    sw->WriteLine ("Exception occurred attempting to compute abundance adjusted results.");
    sw->WriteLine ();
    sw->WriteLine (eeee->ToString ());
    sw->WriteLine ();
    return;
  }

  if  (adjCounts == nullptr)
  {
    sw->WriteLine ();
    sw->WriteLine ("        ***ERROR***          ***ERROR***          ***ERROR***");
    sw->WriteLine ();
    sw->WriteLine ("'AdjustClassificationCounts'  failed to compute abundance adjusted results.");
    sw->WriteLine ();
    return;
  }

  PicesClassList^  classOrder = adjCounts->ToPicesClassList ();
  int  size = adjCounts->Count;

  cli::array<String^>^  classTitles = classOrder->ExtractThreeTitleLines ();
  if  (classTitles != nullptr)
  {
    for  (int z = 0;  z < classTitles->Length;  ++z)
    {
      if  (z >= (classTitles->Length - 1))
        sw->Write ("Description");

      sw->WriteLine ("\t" + "\t" + classTitles[z]);
    }
  }

  int  col = 0;

  cli::array<float>^  probOfFalseAlarm = ProbOfFalseAlarm ();
  cli::array<float>^  probOfDetection  = ProbOfDetection ();

  if  (probOfFalseAlarm->Length == size)
  {
    sw->Write ("ProbOfFalseAlarm" + "\t");
    for  (col = 0;  col < size;  col++)
      sw->Write ("\t" + (100.0f * probOfFalseAlarm[col]).ToString ("##0.000") + "%");
    sw->WriteLine ();
  }

  if  (probOfDetection->Length == size)
  {
    sw->Write ("ProbOfDetection" + "\t");
    for  (col = 0;  col < size;  col++)
      sw->Write ("\t" + (100.0f * probOfDetection[col]).ToString ("##0.000") + "%");
    sw->WriteLine ();
  }

  sw->Write ("Classified Results" + "\t");
  for  (col = 0;  col < size;  col++)
    sw->Write ("\t" + lumpedCounts[col]->Count.ToString ("###,##0"));
  sw->WriteLine ();

  sw->Write ("Adjusted Results"   + "\t");
  for  (col = 0;  col < size;  col++)
    sw->Write ("\t" + adjCounts[col]->Count.ToString ("###,##0"));
  sw->WriteLine ();

  sw->Flush ();
}  /* PrintAdjustedResults */

