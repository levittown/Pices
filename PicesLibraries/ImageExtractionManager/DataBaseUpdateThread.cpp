#include "FirstIncludes.h"
#include <stdio.h>
#include <ctype.h>

#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

#include "MemoryDebug.h"
using namespace  std;


#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;


#include "DuplicateImages.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "DataBase.h"
#include "ImageFeatures.h"
using namespace  MLL;


#include "DataBaseUpdateThread.h"
#include "ExtractionManager.h"
#include "ExtractionParms.h"
#include "ExtractedImage.h"
using namespace  ImageExtractionManager;



DataBaseUpdateThread::DataBaseUpdateThread (ExtractionParms&                 _parms,
                                            ExtractionManagerPtr             _extractionManager,
                                            const KKStr&                     _threadName,
                                            MsgQueuePtr                      _msgQueue,
                                            ExtractedImageQueuePtr           _imagesAwaitingUpdate,
                                            SipperExtractionImageManagerPtr  _imageManager,
                                            FileDescConstPtr                 _fileDesc
                                           ):

    ImageExtractionThread (_parms, _extractionManager, _threadName, _msgQueue),

    dbConn                (NULL),
    dupImageDetector      (NULL),
    duplicateImageDir     (),
    duplicateImages       (NULL),
    duplicatesDetected    (0),
    fileDesc              (_fileDesc),
    imageManager          (_imageManager),
    imagesAwaitingUpdate  (_imagesAwaitingUpdate),
    imagesUpdated         (0),
    log                   (),
    sipperRootName        (),
    updateFailures        (0)

{
  sipperRootName = osGetRootName (parms.SipperFileName ());

  if  (parms.ExtractFeatureData ()  &&  (!sipperRootName.ToLower().StartsWith("port")))
  {
    //dupImageDetector = new DuplicateImages (fileDesc, log);
    duplicateImages  = new ImageFeaturesList (fileDesc, true);
    duplicateImageDir = osAddSlash (parms.OutputRootDir ()) + "DuplicateImages";
    osCreateDirectoryPath (duplicateImageDir);
  }
}



DataBaseUpdateThread::~DataBaseUpdateThread ()
{
  delete  dbConn;            dbConn           = NULL;
  delete  dupImageDetector;  dupImageDetector = NULL;
  delete  duplicateImages;   duplicateImages  = NULL;
}



void  DataBaseUpdateThread::GetRunTimeStats (kkuint32&  _imagesUpdated,
                                             kkuint32&  _duplicatesDetected,
                                             kkuint32&  _updateFailures
                                            )
{
  _imagesUpdated      =  imagesUpdated;
  _duplicatesDetected =  duplicatesDetected;
  _updateFailures     =  updateFailures;
}




ExtractedImagePtr  DataBaseUpdateThread::GetNextImageToUpdate ()
{
  ExtractedImagePtr  nextImage = imagesAwaitingUpdate->GetNextExtractedImage ();
  while  ((nextImage == NULL)  &&  (!ShutdownOrTerminateRequested ()))
  {
    KKB::osSleepMiliSecs(2);
    nextImage = imagesAwaitingUpdate->GetNextExtractedImage ();
  }
  return  nextImage;
}  /* GetNextImageToUpdate */




void  DataBaseUpdateThread::Run ()
{
  Status (ThreadStatus::Starting);

  AddMsg ("DataBaseUpdateThread::Run   Starting");
  if  (CancelFlag ())
  {
    Status (ThreadStatus::Stopped);
    return;
  }

  if  (parms.UpdateDataBase ())
  {
    dbConn = new DataBase (parms.DataBaseServer (), log);
    if  (!dbConn->Valid ())
    {
      Crashed (true);
      KKStr  msg (100);
      msg << "Run     Failed to connect to database;  Error[" << dbConn->LastErrorDesc () << "].";
      Status (ThreadStatus::Stopped);
      return;
    } 
  }

  Status (ThreadStatus::Running);

  ExtractedImagePtr  extractedImage = GetNextImageToUpdate ();
  while  (true)
  {
    if  (TerminateFlag ())
    {
      // If the TerminateFlag has been set then we want to exit out of this thread as
      // fast as we can even if there is still data to be updated.
      break;
    }

    if  (extractedImage == NULL)
    {
      if  (this->ShutdownFlag ())
      {
        // Since Shutdown has been requested and there is nothing left to update the database with we can exit this thread.
        break;
      }
      else
        osSleep (0.1f);
    }
    else
    {
      bool  imageIsDuplicate = false;
      bool  imageUpdated     = true;
      ImageFeaturesPtr  featureVector = extractedImage->FeatureVectorGiveOwnership ();
      RasterSipperPtr   raster        = extractedImage->ImageGiveOwnership ();

      KKStr  imageFileName = raster->FileName ();

      // Before we go and save this Plankton Image, lets make sure that
      // it is not a duplicate of a previous one extracted. This was a problem
      // with the earlier version of SIPPER. The disk controller would write out
      // the last 10mb buffer to the disk twice. This issue was fixed but to allow us to 
      // process the earlier SIPPER files we look for duplicates and exclude them
      // from the database.
      if  (dupImageDetector)
      {
        DuplicateImagePtr  dupImages = dupImageDetector->AddSingleExample (featureVector);
        if  (dupImages)
        {
          KKStr  msg (100);
          msg << "DataBaseUpdateThread::Run   Duplicate Image Detected[" << featureVector->ExampleFileName () << "].";
          AddMsg (msg);
          imageIsDuplicate = true;
          imageUpdated     = false;

          if  (!duplicateImageDir.Empty ())
          {
            // Since image is a duplicate we will need to move it to duplicates directory
            KKStr  newRootName = osGetRootName (dupImages->FirstExampleAdded ()->ExampleFileName ()) 
                                  + "-" +
                                  osGetRootName (featureVector->ExampleFileName ());
            KKStr  newFullFileName = osAddSlash (duplicateImageDir) + newRootName + ".bmp";
            osDeleteFile (newFullFileName);  // Make sure there is no file already there from prev run.
            
            SaveImageGrayscaleInverted4Bit (*raster, newFullFileName);
          }

          // We need to give ownership of the 'featureVector' instance to 'duplicateImages' because this instance
          // is not going to be given to the "imageManager"; as a result we need to keep it in memory somewhere
          // until we delete  the 'dupImageDetector' instance which is referring to it.
          duplicateImages->PushOnBack (featureVector);
          featureVector = NULL;

          // But at this point there is no reason to save the 'raster' instance.
          delete  raster;
          raster = NULL;
        }
      }

      if  (!imageIsDuplicate)
      {
        bool  successful = false;

        if  (parms.RefreshDataBaseImages ()  &&  (dbConn != NULL)  &&  (extractedImage != NULL))
        {
          ReFreshDataBaseImage (imageFileName, 
                                sipperRootName, 
                                raster, 
                                featureVector, 
                                extractedImage->ByteOffsetFirstScanRow (),
                                extractedImage->SipperFileScanRow (),
                                extractedImage->SipperFileScanCol ()
                               );
        }

        else if  ((parms.ExtractFeatureData ())  &&  (dbConn != NULL))
        {
          kkint32  imageId = 0;
          dbConn->ImageInsert (*raster,
                               osGetRootName (imageFileName),
                               sipperRootName,
                               extractedImage->ByteOffsetFirstScanRow (),
                               extractedImage->SipperFileScanRow (),
                               extractedImage->SipperFileScanCol (),
                               raster->Height (),
                               raster->Width  (),
                               extractedImage->PixelCount (),
                               parms.ConnectedPixelDist (),
                               extractionManager->LogEntryId (),
                               extractionManager->LogEntryId (),         // Classification Update LogEntryID
                               (kkint32)(featureVector->CentroidRow () + 0.5f),
                               (kkint32)(featureVector->CentroidCol () + 0.5f),
                               extractedImage->PredClass1 (),
                               extractedImage->PredClass1Prob (),
                               extractedImage->PredClass2 (),
                               extractedImage->PredClass2Prob (),
                               NULL,                                     // Validated Class
                               featureVector->Depth (),
                               extractedImage->ImageArea (),             // size in mm^2
                               NULL,                                     // sizeCoordinates
                               imageId,
                               successful
                              );

          if  (!successful)
          {
            imageUpdated = false;
            updateFailures++;
            KKStr  msg (100);
            msg << "DataBaseUpdateThread::Run    ***ERROR***   ImageInsert Failed   ErrorMessage[" 
                << dbConn->LastErrorDesc () << "]";
            AddMsg (msg);
          }
          else
          {
            dbConn->FeatureDataInsertRow (sipperRootName, *featureVector);
            if  (!dbConn->Valid ())
            {
              imageUpdated = false;
              updateFailures++;
              KKStr  msg (100);
              msg << "DataBaseUpdateThread::Run    ***ERROR***   FeatureDataInsertRow Failed   ErrorMessage[" 
                  << dbConn->LastErrorDesc () << "]";
              AddMsg (msg);
            }
          }
        }

        try
        {
          // We will be giving ownership of 'featureVector' to 'imageManager'.
          imageManager->AddImage (imageFileName, 
                                  extractedImage->PredClass1 (),
                                  extractedImage->PixelCount (),
                                  extractedImage->Depth (),
                                  featureVector,              // imageManager will take ownership of image.
                                  raster,
                                  parms.Colorize ()
                                 );
          featureVector = NULL;
        }
        catch (...)
        {
          AddMsg ("DataBaseUpdateThread::Run       Exception adding image to ImageManager.");
        }
      }

      if  (imageUpdated)
        ++imagesUpdated;

      if  (imageIsDuplicate)
        ++duplicatesDetected;

      delete  raster;
      raster = NULL;
    }

    delete  extractedImage; 
    extractedImage = NULL;
    extractedImage = GetNextImageToUpdate ();
  }

  Status (ThreadStatus::Stopping);

  delete  dbConn;
  dbConn = NULL;
  Status (ThreadStatus::Stopped);

  AddMsg ("DataBaseUpdateThread::Run    Exiting   Status: " + StoppingFlagsStr ());
} /* Run */




void  DataBaseUpdateThread::ReFreshDataBaseImage (const KKStr&      imageFileName, 
                                                  const KKStr&      sipperFileName, 
                                                  RasterSipperPtr   raster, 
                                                  ImageFeaturesPtr  featureVector,
                                                  kkuint64          byteOffset,
                                                  kkuint32          sipperTopRow,
                                                  kkuint32          sipperTopCol
                                                 )
{
  if  (!dbConn)
    return;

  float  centroidRow         = -1.0f;
  float  centroidCol         = -1.0f;
  float  centroidWeightedCol = -1.0f;
  float  centroidWeightedRow = -1.0f;
  int    imageId             = -1;
  int    size                = 0;
  bool   successful          = false;
  int    weight              = 0;

  raster->CalcCentroid (size, weight, centroidRow, centroidCol, centroidWeightedRow, centroidWeightedCol);

  DataBaseImagePtr  existingEntry = dbConn->ImagesLocateClosestImage (imageFileName);
  if  (existingEntry)
  {
    existingEntry->PixelCount ((uint)size);
    existingEntry->ConnectedPixelDist (parms.ConnectedPixelDist ());
    existingEntry->ExtractionLogEntryId (extractionManager->LogEntryId ());
    existingEntry->CentroidRow ((int)(centroidRow + 0.5f));
    existingEntry->CentroidCol ((int)(centroidCol + 0.5f));
    existingEntry->ByteOffset  (byteOffset);
    existingEntry->TopLeftRow  (sipperTopRow);
    existingEntry->TopLeftCol  (sipperTopCol);
    existingEntry->Height      (raster->Height ());
    existingEntry->Width       (raster->Width  ());


    dbConn->ImageUpdate (*existingEntry, *raster);
    if  (!dbConn->Valid ())
    {
      updateFailures++;
      AddMsg ("ReFreshDataBaseImage  " + dbConn->LastErrorDesc ());
    }
  }

  else if  (featureVector)
  {
    dbConn->ImageInsert (*raster,
                         osGetRootName (imageFileName),
                         sipperRootName,
                         byteOffset,       // byteOffset,     // byteOffset of SipperRow containing TopLeftRow
                         sipperTopRow,
                         sipperTopCol,
                         raster->Height (),
                         raster->Width  (),
                         (uint)(size),
                         parms.ConnectedPixelDist (),
                         extractionManager->LogEntryId (),    // Image Extraction Log ID
                         extractionManager->LogEntryId (),    // Log Entry for classification.
                         (uint)(centroidRow + 0.5f),
                         (uint)(centroidCol + 0.5f),
                         featureVector->PredictedClass (),
                         featureVector->Probability (),
                         NULL,
                         0.0f,
                         NULL,
                         featureVector->Depth (),
                         0.0f,                      // imageSize
                         NULL,                      // SizeCoordinates
                         imageId,
                         successful
                        );
    if  (!dbConn->Valid ())
    {
      updateFailures++;
      AddMsg ("ReFreshDataBaseImage  " + dbConn->LastErrorDesc ());
    }
  }

  delete  existingEntry;
  existingEntry = NULL;
}  /* ReFreshDataBaseImage */

