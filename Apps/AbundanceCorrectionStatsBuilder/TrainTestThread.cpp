/* TrainTestThread.cpp -- Manages the threads that perform the image extraction process.
 * Copyright (C) 2012 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in ImageExtractionManager.h
 */
#include "FirstIncludes.h"
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include <windows.h>

#include "MsgQueue.h"
using namespace KKB;

#include "Classifier2.h"
#include "ConfusionMatrix2.h"
#include "KKException.h"
#include "FeatureVector.h"
#include "MLClass.h"
#include "TrainingConfiguration2.h"
#include "TrainingProcess2.h"
using namespace  KKMLL;

#include "PicesTrainingConfiguration.h"
using namespace MLL;


#include "TrainTestThread.h"
#include "AbundanceCorrectionStatsBuilder.h"
using namespace AbundanceCorrectionApplication;



//
// Usage: SetThreadName (-1, "MainThread");
//
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void  SetThreadName (DWORD        dwThreadID, 
                     const char*  threadName
                    )
{
   Sleep(10);
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}  /* SetThreadName */





TrainTestThread::TrainTestThread (const KKStr&                        _desc,
                                  AbundanceCorrectionStatsBuilderPtr  _parent,
                                  PicesTrainingConfigurationPtr       _config,           /**< Will NOT  take ownership.       */
                                  MLClassListPtr                      _allClasses,
                                  FeatureVectorListPtr                _trainData,        /**< Will take ownship of this list. */
                                  MLClassListPtr                      _trainDataClasses,
                                  FeatureVectorListPtr                _testData,         /**< Will take ownship of this list. */
                                  MLClassPtr                          _otherClass,
                                  const KKStr&                        _threadName,
                                  MsgQueuePtr                         _msgQueue,
                                  RunLogPtr                           _runLog
                                 ):
   abundanceCorMatrix (NULL),
   allClasses         (_allClasses),
   cancelFlag         (false),
   config             (_config),
   confusionMatrix    (NULL),
   crashed            (false),
   desc               (_desc),
   msgQueue           (_msgQueue),
   otherClass         (_otherClass),
   parent             (_parent),
   runLog             (_runLog),
   status             (NotStarted),
   terminateFlag      (false),
   testData           (_testData),
   threadName         (_threadName),
   trainData          (_trainData),
   trainDataClasses   (_trainDataClasses),
   windowsThread      (NULL),
   windowsThreadId    (0)
{
}



TrainTestThread::~TrainTestThread ()
{
  delete  abundanceCorMatrix;   abundanceCorMatrix = NULL;
  delete  confusionMatrix;      confusionMatrix    = NULL;
  delete  testData;             testData           = NULL;
  delete  trainData;            trainData          = NULL;
}



void  TrainTestThread::AddMsg (KKStrPtr  msg)
{
  if  (msgQueue == NULL)
  {
    cerr << endl 
         << "TrainTestThread::AddMsg   ***ERROR***    msgQuue is not defined."  << endl
         << "                       Msg[" << *msg << "]." << endl
         << endl;
    delete  msg;
    msg = NULL;
  }
  else
  {
    msgQueue->AddMsg (msg);
  }
}  /* AddMsg */




void  TrainTestThread::AddMsg (const KKStr&  msg)
{
  if  (msgQueue == NULL)
  {
    cerr << endl 
         << "TrainTestThread::AddMsg   ***ERROR***    msgQuue is not defined."  << endl
         << "                       Msg[" << msg << "]." << endl
         << endl;
  }
  else
  {
    msgQueue->AddMsg (threadName + "::" + msg);
  }
}  /* AddMsg */



void  TrainTestThread::CancelThread ()
{
  cancelFlag = true;
}



void  TrainTestThread::TerminateThread ()
{
  terminateFlag = true;
}




extern "C" 
{
  unsigned int ThreadStartCallBack (void* param) 
  {
	  TrainTestThreadPtr  tp = (TrainTestThreadPtr)param;
    tp->Status (Starting);
	  tp->Run ();
    tp->Status (Stopped);
	  return 0;
  }
}


void  TrainTestThread::Start () 
{
	DWORD windowsThreadId = 0;

	windowsThread = (unsigned long*)CreateThread (NULL,
                                                0,
                                                (LPTHREAD_START_ROUTINE)ThreadStartCallBack,
                                                (TrainTestThreadPtr)this,
                                                0,
                                                &windowsThreadId
                                               );
	if  (windowsThread == NULL) 
  {
		throw KKException ("Failed to create thread");
	}
  else 
  {
	  SetPriority (THREAD_PRIORITY_NORMAL);
    SetThreadName (windowsThreadId, threadName.Str ());
	}
}  /* Start */



/** stop()
  * stops the running thread and frees the thread handle
**/ 
void  TrainTestThread::Stop ()
{
	if  (windowsThread == NULL)
    return;	

	WaitForSingleObject (windowsThread, INFINITE);
	CloseHandle (windowsThread);
	windowsThread = NULL;
}  /* Stop */




// http://www.codeproject.com/Articles/5218/Synchronized-multi-threading-in-C-No-MFC
void  TrainTestThread::SetPriority (int tp) 
{
	if  (windowsThread == NULL) 
  {
		throw KKException ("Thread object is null");
	}
  else 
  {
		if  (SetThreadPriority (windowsThread, tp) == 0) 
    {
			throw KKException ("Failed to set priority");
		}
  }
}  /* SetPriority */






void  TrainTestThread::Run () 
{
  Status (Starting);

  bool  successful = false;

  Status (Running);

  TrainingProcess2Ptr  tp = NULL;

  {
    tp = TrainingProcess2::CreateTrainingProcessFromTrainingExamples 
         (config,
          trainData,
          false,    //  false = Do NOT take ownership of training examples.
          false,    //  false = NOT  "featuresAlreadyNormalized"
          cancelFlag,
          *runLog
         );

    if  (tp->Abort ())
      msgQueue->AddMsg (threadName + " Construction of training model failed.");
  }

  if  (tp->Abort ())
  {
    AddMsg (threadName + "  TrainTestThread::Run     building of 'TrainingProcess2' failed.");
    Crashed (true);
    Status (Stopping);
    return;
  }

  Classifier2Ptr  classifier = new Classifier2 (tp, *runLog);

  abundanceCorMatrix = new AbundanceCorrectionMatrix (*trainDataClasses, otherClass, *runLog);
  confusionMatrix    = new ConfusionMatrix2 (*allClasses);
                     

  FeatureVectorList::const_iterator  idx;
  for  (idx = testData->begin ();  idx != testData->end ();  ++idx)
  {
    FeatureVectorPtr    fv = *idx;

    FeatureVectorPtr    dupImage = new FeatureVector (*fv); 

    MLClassPtr  knownClass = dupImage->MLClass ();

    MLClassPtr  predClass1      = NULL;
    MLClassPtr  predClass2      = NULL;
    kkint32     predClass1Votes = 0;
    kkint32     predClass2Votes = 0;
    double      knownClassProb  = 0.0;
    double      predClass1Prob  = 0.0; 
    double      predClass2Prob  = 0.0;
    kkint32     numOfWinners    = 0;
    double      breakTie        = 0.0;

    classifier->ClassifyAExample (*dupImage, 
                                predClass1,      predClass2, 
                                predClass1Votes, predClass2Votes, 
                                knownClassProb, 
                                predClass1Prob,  predClass2Prob, 
                                numOfWinners,    breakTie
                               );

    MLClassPtr class2UseForAbundanceMatrix = predClass1;
    if  (trainDataClasses->PtrToIdx (class2UseForAbundanceMatrix) < 0)
      class2UseForAbundanceMatrix = otherClass;

    abundanceCorMatrix->Prediction (knownClass, class2UseForAbundanceMatrix, *runLog);

    confusionMatrix->Increment (knownClass,
                                predClass1,
                                dupImage->OrigSize (),
                                predClass1Prob,
                                *runLog
                               );
    delete  dupImage;
    dupImage = NULL;
  }

  Status (Stopping);

  delete  classifier;   classifier = NULL;
  delete  tp;           tp         = NULL;

  AddMsg ("Run " + ThreadName () + " done.");

  Status (Stopped);
}  /* Run */




AbundanceCorrectionMatrixPtr  TrainTestThread::AbundanceCorMatrixTakeOwnership ()
{
  AbundanceCorrectionMatrixPtr result = abundanceCorMatrix;
  abundanceCorMatrix = NULL;
  return  result;
}  /* AbundanceCorMatrixTakeOwnership */




ConfusionMatrix2Ptr   TrainTestThread::ConfusionMatrixTakeOwnership ()
{
  ConfusionMatrix2Ptr result = confusionMatrix;
  confusionMatrix = NULL;
  return  result;
}
