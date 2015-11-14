#pragma once

#include "DateTime.h"
#include "KKStr.h"
#include "Matrix.h"
using namespace  KKB;


#include "MLClass.h"
using namespace  KKMLL;


#include "PicesRunLog.h"
#include "PicesClassList.h"
#include "PicesClassStatistic.h"


namespace PicesInterface 
{
  ///<summary>Assists in adjusting a Classifiers output for bias of a classifier.</summary>
  ///<remarks>
  /// See the paper: <see href='http://darchive.mblwhoilibrary.org:8080/handle/1912/4501'>"Accurate automatic quantification of 
  /// taxa-specific plankton abundance using dual classification with correction"</see> by: Qiao Hu, Cabell Davis Date: 2006-01-11
  ///
  /// This class uses the results of a n-fold cross validation to compute parameters that are used to adjust the 
  /// results of a classification run.  The application "AbundanceCorrectionStatsBuilder" will create a parameter 
  /// file in the directory "${PicesHomeDir}\\DataFiles\\TrainingModels\\SaveFiles" that contains the parameters.
  ///</remarks>
  public  ref class PicesAbundanceCorrectionMatrix
  {
  public:
    ///<summary>Will construct a instance from the parameter file for '_configFileName'.</summary>
    ///<param name='_configFileName'> The name of the BiasMatrix file will be derived from the configuration 
    ///       file name.  Se the code for 'the term 'PicesAbundanceCorrectionMatrix' in PicesLibrary'.</param>
	  ///<param name='_runLog'> Logging file to use. </param>
    PicesAbundanceCorrectionMatrix (PicesClassList^  _classes,
                                    PicesClass^      _otherClass,
                                    PicesRunLog^     _runLog
                                   );
                             
    PicesAbundanceCorrectionMatrix (String^       _configFileName,
                                    PicesRunLog^  _runLog
                                   );

    !PicesAbundanceCorrectionMatrix ();

    ~PicesAbundanceCorrectionMatrix ();


    System::DateTime  DateTimeComputed ();
    array<float>^     ProbOfDetection  ();
    array<float>^     ProbOfFalseAlarm ();
    bool              Valid            ();


    void  AddIn (PicesAbundanceCorrectionMatrix^  matrix,
                 PicesRunLog^                     runLog
                );


    PicesClassStatisticList^   AdjustClassificationCounts (PicesClassStatisticList^  inputCounts,
                                                           PicesRunLog^              log
                                                          );

    ///<summary>computing  probOfDetection  and  probOfFalseAlarm(Specificity) </summary>
    void  ComputeStatistics ();

    PicesClassStatisticList^   LumpCounts (PicesClassStatisticList^  inputCounts);

    void  Prediction (PicesClass^   knownClass,
                      PicesClass^   predClass,
                      PicesRunLog^  log
                     );

    void  PrintConfusionMatrixTabDelimited (StreamWriter^  outFile);

    void  PrintAdjustedResults (StreamWriter^             sw,
                                PicesClassStatisticList^  classifiedCounts,
                                PicesRunLog^              runLog
                               );

    void  SaveForConfigFileName (String^  configFileName);


  private:
    MLL::AbundanceCorrectionMatrixPtr  abundanceMatrix;
  };
}  /* namespace PicesInterface */
