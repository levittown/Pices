/* SipperThread.cpp -- Manages the threads that perform the image extraction process.
 * Copyright (C) 2012 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in ImageExtractionManager.h
 */
#include "FirstIncludes.h"
#include <string>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "MsgQueue.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;


#include "Classifier2.h"
#include "DataBase.h"
#include "FactoryFVProducer.h"
#include "FeatureVectorProducer.h"
#include "FileDesc.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "TrainingProcess2.h"
using namespace  KKMLL;


#include "FeatureFileIOPices.h"
#include "InstrumentData.h"
#include "InstrumentDataManager.h"
#include "InstrumentDataFileManager.h"
#include "PicesTrainingConfiguration.h"
#include "SipperBuff.h"
#include "SipperBuffBinary.h"
#include "SipperBuff3Bit.h"
#include "Sipper3Buff.h"
#include "Sipper3RevBuff.h"
#include "SipperExtractionImageManager.h"
using namespace  MLL;


#include "FrameProcessorThread.h"
#include "ExtractedImage.h"
#include "ExtractionManager.h"
#include "LogicalFrame.h"
#include "LogicalFrameQueue.h"
using namespace  ImageExtractionManager;


#include "FrameProcessorThread.h"


FrameProcessorThread::FrameProcessorThread (ExtractionParms&        _parms,
                                            ExtractionManagerPtr    _extractionManager,
                                            const KKStr&            _threadName,
                                            MsgQueuePtr             _msgQueue,
                                            LogicalFrameQueuePtr    _framePool,
                                            ExtractedImageQueuePtr  _imagesAwaitingUpdate,
                                            bool&                   _successful
                                           ):

  ImageExtractionThread (_parms, _extractionManager, _threadName, _msgQueue),
  classifier            (NULL),
  fileDesc              (NULL),
  framePool             (_framePool),
  framesProcessed       (0),
  fvProducer            (NULL),
  fvProducerFactory     (NULL),
  imagesAwaitingUpdate  (_imagesAwaitingUpdate),
  imagesClassified      (0),
  imagesFound           (0),
  log                   (),
  sipperRootName        (),
  trainer               (NULL),
  unKnownMLClass        (NULL),
  scanLinesProcessed    (0)

{
  AddMsg ("FrameProcessorThread");

  _successful = true;

  sipperRootName = osGetRootName (parms.SipperFileName ());

  fvProducerFactory = _extractionManager->FvProducerFactory ();
  fvProducer        = fvProducerFactory->ManufactureInstance (log);
  fileDesc          = fvProducerFactory->FileDesc ();

  InstrumentDataFileManager::InitializePush ();
 
  KKStr  siperFileRootName = osGetRootName (parms.SipperFileName ());

  unKnownMLClass = MLClass::CreateNewMLClass ("UnKnown");

  LoadClassifier (_successful);

  AddMsg ("FrameProcessorThread  Exiting");
}



FrameProcessorThread::~FrameProcessorThread ()
{
  AddMsg ("~FrameProcessorThread");

  InstrumentDataFileManager::InitializePop ();

  delete  trainer;     trainer     = NULL;
  delete  classifier;  classifier  = NULL;
  delete  fvProducer;  fvProducer  = NULL;
}



void  FrameProcessorThread::GetRunTimeStats (kkuint32&  _framesProcessed,
                                             kkuint32&  _scanLinesProcessed,
                                             kkuint32&  _imagesFound,
                                             kkuint32&  _imagesClassified
                                            )
{
  _framesProcessed     = framesProcessed;
  _scanLinesProcessed  = scanLinesProcessed;
  _imagesFound         = imagesFound;
  _imagesClassified    = imagesClassified;
}




KKStr  FrameProcessorThread::SipperFileFormatStr ()
{
  return  SipperFileFormatToStr (parms.FileFormat ());
}  /* SipperFileFormatStr */



void  FrameProcessorThread::ProcessFrame (LogicalFramePtr  frame)
{
  if  (TerminateFlag ())
    return;

  ExtractedImageListPtr  extractedImages = frame->ProcessFrame ();
  if  (!extractedImages)
    return;

  imagesFound += frame->ImagesInFrame ();

  while  ((extractedImages->QueueSize () > 0)  &&  (!TerminateFlag ()))
  {
    ExtractedImagePtr  extractedImage = extractedImages->PopFromBack ();

    kkuint32  sipperTopRow = extractedImage->SipperFileScanRow ();
    kkuint32  sipperTopCol = extractedImage->SipperFileScanCol ();

    KKStr  imageFileName (64);
    imageFileName << sipperRootName
                  << "_"
                  << StrFormatInt (sipperTopRow, "ZZ00000000")
                  << "_"
                  << StrFormatInt (sipperTopCol, "0000")
                  << ".bmp";
  
    FeatureVectorPtr  featureVector      = NULL;
    ImageFeaturesPtr  picesFeatureVector = NULL;

    RasterSipperPtr raster = extractedImage->Image ();
    raster->FileName (imageFileName);
    
    float        depth            = 0.0f;
    MLClassPtr   predClass1       = unKnownMLClass;
    MLClassPtr   predClass2       = NULL;
    kkint32      predClass1Votes  = 0;
    kkint32      predClass2Votes  = 0;
    double       knownClassProb   = 0.0;
    double       predClass1Prob   = 0.0;
    double       predClass2Prob   = 0.0;
    
    bool  cf = false;


    /**@todo  When a proper implementation for SIPPER 4 is made I will need to get instrument data for it.  */

    InstrumentDataConstPtr  id = NULL;
    if  (this->parms.FileFormat () != sfSipper4Bit)
      id = InstrumentDataFileManager::GetClosestInstrumentData (imageFileName, cf, log);

    if  (id)
      depth = id->Depth ();

    if  (parms.ExtractFeatureData ())
    {
      raster->FileName (imageFileName);
      featureVector = fvProducer->ComputeFeatureVector (*raster, unKnownMLClass, NULL, 1.0f, log);
      featureVector->ExampleFileName (imageFileName);

      picesFeatureVector = NULL;
      if  (typeid (*featureVector) == typeid (ImageFeatures))
      {
        picesFeatureVector = dynamic_cast<ImageFeaturesPtr> (featureVector);
        featureVector = NULL;
      }
      else
      {
        picesFeatureVector = new ImageFeatures (*featureVector);
        delete  featureVector;
        featureVector = NULL;
      }

      if  (id)
      {
        picesFeatureVector->FlowRate1    (id->FlowRate1    ());
        picesFeatureVector->FlowRate2    (id->FlowRate2    ());
        picesFeatureVector->Latitude     (id->Latitude     ());
        picesFeatureVector->Longitude    (id->Longitude    ());
        picesFeatureVector->Depth        (id->Depth        ());
        picesFeatureVector->Salinity     (id->Salinity     ());
        picesFeatureVector->Oxygen       (id->Oxygen       ());
        picesFeatureVector->Fluorescence (id->Fluorescence ());
      }

      picesFeatureVector->SfCentroidCol (picesFeatureVector->CentroidCol () + float  (sipperTopCol));
      picesFeatureVector->SfCentroidRow (picesFeatureVector->CentroidRow () + double (sipperTopRow));

      // Duplicate Image Detection was moved over to the 'DataBaseUpdateThread".

      if  (classifier)
      {
        int  numOfWinners;
        bool knownClassOneOfTheWinners  = false;
        double  breakTie    = 0.0f;

        ImageFeaturesPtr  dupFV = new ImageFeatures (*picesFeatureVector);
        classifier->ClassifyAExample (*dupFV,
                                     predClass1,       predClass2,
                                     predClass1Votes,  predClass2Votes,
                                     knownClassProb,   
                                     predClass1Prob,   predClass2Prob,
                                     numOfWinners,
                                     breakTie
                                    );

        picesFeatureVector->MLClass        (predClass1);
        picesFeatureVector->Probability    ((float)predClass1Prob);
        picesFeatureVector->BreakTie       ((float)breakTie);
        picesFeatureVector->PredictedClass (predClass1);
        delete  dupFV;
        dupFV = NULL;
      }
    }

    extractedImage->Depth         (depth);
    extractedImage->FeatureVector (picesFeatureVector);
    
    extractedImage->Prediction (predClass1, (float)predClass1Prob, predClass2, (float)predClass2Prob);

    ++imagesClassified;

    imagesAwaitingUpdate->AddExtractedImage (extractedImage);
  }

  delete  extractedImages;
  extractedImages = NULL;

  ++framesProcessed;
  scanLinesProcessed += frame->FrameHeight ();
} /* ProcessFrame */



void  FrameProcessorThread::LoadClassifier (bool&  _successful)
{
  _successful = true;
  delete  trainer;     trainer    = NULL;
  delete  classifier;  classifier = NULL;

  if  ((parms.ExtractFeatureData ())  &&  (!parms.ConfigFileName ().Empty ()))
  {
    AddMsg ("LoadClassifier    Building Training Model");

    trainer = TrainingProcess2::LoadExistingTrainingProcess (parms.ConfigFileName (), CancelFlag (), log);

    if  (ShutdownOrTerminateRequested ())
    {
      AddMsg ("LoadClassifier    Image Extraction Canceled.");
    }

    else if  (!trainer)
    {
      _successful = false;
      KKStr  msg (100);
      msg << "LoadClassifier    ***ERROR***  TrainingProcess  failed to load;  ConfigFileName[" << parms.ConfigFileName () << "].";
      AddMsg (msg);
    }

    else if  (trainer->Abort ())
    {
      _successful = false;
      KKStr  msg (100);
      msg << "LoadClassifier    ***ERROR***  Building Classifier for ConfigFileName[" << parms.ConfigFileName () << "].";
      AddMsg (msg);
    }

    else
    {
      AddMsg ("LoadClassifier   Classifier2 Created Successfully.");
      classifier = new Classifier2 (trainer, log);
    }
  }
}  /* LoadClassifier */



LogicalFramePtr  FrameProcessorThread::GetNextFrameToProcess ()
{
  LogicalFramePtr  logicalFrame = framePool->GetNextFrameToProcess ();
  while  ((logicalFrame == NULL)  &&  (!CancelFlag ()))
  {
    osSleepMiliSecs (20);
    logicalFrame = framePool->GetNextFrameToProcess ();
    if  ((logicalFrame == NULL)  &&  ShutdownFlag ())
    {
      // There are no more frames needing processing and the Extraction Manager wants to stop when convenient.
      break;
    }
  }

  if  (CancelFlag ()  &&  (logicalFrame != NULL))
  {
    framePool->AddBackToFramesAwaitingProcessing (logicalFrame);
    logicalFrame = NULL;
  }

  return  logicalFrame;
}  /* GetNextFrameToProcess */




void  FrameProcessorThread::Run ()
{
  Status (ThreadStatus::Starting);

  Status (ThreadStatus::Running);

  LogicalFramePtr  logicalFrame = GetNextFrameToProcess ();
  while  (!CancelFlag ())
  {
    if  (logicalFrame)
    {
      ProcessFrame (logicalFrame);
      framePool->FrameProcessed (logicalFrame);
    }
    
    else if  (ShutdownFlag ())
      break;

    logicalFrame = GetNextFrameToProcess ();
  }

  Status (ThreadStatus::Stopping);
  AddMsg ("Run " + ThreadName () + " done.");

  Status (ThreadStatus::Stopped);
}  /* Run */
