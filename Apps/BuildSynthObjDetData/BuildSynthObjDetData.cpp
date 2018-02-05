/// BuildSynthObjDetData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FirstIncludes.h"
#include <stdlib.h>
#include <fstream>
#include <map>
#include <string>
#include <iostream>
#include <math.h>
#include <memory>
#include <random>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "Blob.h"
#include "ImageIO.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "InstrumentDataManager.h"
#include "DataBase.h"
#include "ImageFeatures.h"
#include "SipperCruise.h"
#include "SipperDeployment.h"
#include "SipperFile.h"
#include "PicesVariables.h"
using namespace  MLL;


#include "BuildSynthObjDetData.h"


using namespace  PicesUtilityApps;

// -SrcDir  D:\Pices\Cruises\WB1101\GpsData


BuildSynthObjDetData::BuildSynthObjDetData () :

  PicesApplication (),
  cancelFlag (false),
  candidateMinSize (2000),
  candidateMaxSize (100000),
  imageHeight (2048),
  imageWidth (2048),
  instrumentDataManager (NULL),
  maxCandidates (100000),
  maxImages (10000),
  nextSipperFileIdx (0),
  rng(1000),
  sipperBuff (NULL),
  sipperFileRootDir("F:\\Pices\\SipperFiles\\USF\\ETP2008"),
  targetDir()
{
}



BuildSynthObjDetData::~BuildSynthObjDetData ()
{
  delete instrumentDataManager;
  instrumentDataManager = NULL;
}



void  BuildSynthObjDetData::InitalizeApplication (kkint32 argc,
                                                  char**  argv
                                                 )
{
  this->DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  if (targetDir.Empty ())
    targetDir = KKB::osGetCurrentDirectory ();
}  /* InitalizeApplication */



bool  BuildSynthObjDetData::ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                                     const KKStr&  parmValue
                                                    )
{
  bool  validParm = true;

  if (parmSwitch.EqualIgnoreCase ("-imageHeight"))
    imageHeight = parmValue.ToInt32 ();

  else if (parmSwitch.EqualIgnoreCase ("-imageWidth"))
    imageWidth = parmValue.ToInt32 ();

  else if (parmSwitch.EqualIgnoreCase ("-maxImages"))
    maxImages = parmValue.ToInt32 ();

  else if  (parmSwitch.EqualIgnoreCase ("-maxCandidates"))
    maxCandidates =  parmValue.ToInt32 ();

  else if (parmSwitch.EqualIgnoreCase ("-targetDir"))
    targetDir = parmValue;

  else
    validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);

  return  validParm;
}  /* ProcessCmdLineParameter */



void  BuildSynthObjDetData::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
    << "   Application that will scan through all images in the MySQL Images table" << endl
    << "   and verify that a FullSize image exists for those that are larger than" << endl
    << "   their thumbnail version.  The data will be processed by SipperFile entries." << endl
    << endl;
}  /* DisplayCommandLineParameters */



void  BuildSynthObjDetData::LoadAvailableSipperFileNames ()
{
  availableSipperFileNames.clear ();
  KKB::osGetListOfFilesInDirectoryTree (sipperFileRootDir, "*.spr", availableSipperFileNames);
}



bool  BuildSynthObjDetData::OpenSipperBuff ()
{
  delete sipperBuff;
  sipperBuff = NULL;

  while (sipperBuff == NULL) 
  {
    if (nextSipperFileIdx >= availableSipperFileNames.size ())
      nextSipperFileIdx = 0;
    KKStr sipperFileName = availableSipperFileNames[nextSipperFileIdx];
    ++nextSipperFileIdx;
    try
    {
      delete instrumentDataManager;
      instrumentDataManager = NULL;
      instrumentDataManager = new InstrumentDataManager (sipperFileName, NULL, log);
      sipperBuff = SipperBuff::CreateSipperBuff (sipperFileName, 0, instrumentDataManager, log);
    }
    catch (...)
    {
      log.Level (-1) << "Exception  Creating SipperBuff for file: " << sipperFileName << endl;
      sipperBuff = NULL;
    }
  } 

  return (sipperBuff != NULL);
}  /* OpenSipperBuff */



RasterPtr BuildSynthObjDetData::GetNextFrame ()
{
  RasterPtr r = new Raster (2048, 2048);
  if (sipperBuff == NULL)
    OpenSipperBuff ();

  kkint32 lineLen = sipperBuff->LineWidth ();
  auto line = new uchar[lineLen];
  auto colCount = new  kkuint32[lineLen];

  bool frameRead = false;
  while  (!frameRead)
  {
    kkint32  linesAddedToRaster = 0;
    kkuint32 lineSize = 0;
    kkuint32 pixelsInRow = 0;
    bool flow = false;

    sipperBuff->GetNextLine (line, lineLen, lineSize, colCount, pixelsInRow, flow);
    while ((linesAddedToRaster < r->Height ()) && (!sipperBuff->Eof ()))
    {
      uchar* rasterRow = r->Green ()[linesAddedToRaster];

      uchar* startCol = line + 1024;
      uchar* endCol = startCol + 2048;

      std::memcpy(rasterRow, startCol, 2048);
      ++linesAddedToRaster;
      sipperBuff->GetNextLine (line, lineLen, lineSize, colCount, pixelsInRow, flow);
    }
    if (linesAddedToRaster >= r->Height ())
      frameRead = true;

    else if (sipperBuff->Eof ())
    {
      OpenSipperBuff ();
    }
  }

  if (!frameRead)
  {
    delete r;
    r = NULL;
  }

  delete line;
  line = NULL;
  return r;
}



RasterPtr BuildSynthObjDetData::GetNextOpenFrame ()
{
  bool frameOpen = false;
  RasterPtr r = NULL;
  
  do
  {
    delete r;
    r = GetNextFrame ();
    BlobListPtr blobs = r->ExtractBlobs (3);
    if (blobs != NULL)
    {
      auto largestBlob = blobs->LocateLargestBlob ();
      if (largestBlob->PixelCount () <= 100)
        frameOpen = true;
      delete blobs;
      blobs = NULL;
    }
  } while (!frameOpen);
  
  if (!frameOpen)
  {
    delete r;
    r = NULL;
  }
  return r;
}



DataBaseImageListPtr  BuildSynthObjDetData::GetListOfValidatedImages (
    kkint32  minSize,
    kkint32  maxSize,
    kkuint32 restartImageId,
    kkint32  limit)
{
  MLClassPtr anyClass = NULL;
  DataBaseImageGroupPtr nullGroup = NULL;
  auto labeledExamples = DB ()->ImagesQuery (
      nullGroup, // Group
      "",        // cruiseName,
      "",        // stationName,
      "",        // deploymentNum,
      anyClass,  // mlClass,
      'V',       // classKeyToUse,
      0.0f,      // probMin,
      1.0f,      // probMax
      minSize,
      maxSize,
      0.0f,      // depthMin
      0.0f,      // depthMax
      restartImageId,
      limit,
      false,     // includeThumbnail
      cancelFlag
  );

  labeledExamples->RandomizeOrder (1000);

  return labeledExamples;
}



RasterPtr  BuildSynthObjDetData::ReduceToMinimumSize (RasterPtr&  src)
{
  Point topLeft, botRight;
  src->FindBoundingBox (topLeft, botRight);
  if (topLeft.Col () > botRight.Col ())
  {
    delete src;
    src = NULL;
    return NULL;
  }
  else
  {
    auto trimmedObj = new Raster (*src, topLeft, botRight);
    delete src;
    src = NULL;
    return trimmedObj;
  }
}



bool  BuildSynthObjDetData::SeeIfItFits 
       (Raster&       target, 
        const Raster& obj, 
        Raster&       marker, 
        kkint32       topLeftRow, 
        kkint32       topLeftCol
       )
{
  bool itFits = true;
  kkint32 srcRow = 0;
  kkint32 targetRow = topLeftRow;
  kkint32 lastTargetRow = Min (topLeftRow + obj.Height (), target.Height ());
  kkint32 lastTargetCol = Min (topLeftCol + obj.Width (),  target.Width ());
  while ((targetRow < lastTargetRow)  &&  itFits)
  {
    kkint32 targetCol = topLeftCol;
    kkint32 srcCol = 0;
    while ((targetCol < lastTargetCol)  &&  itFits)
    {
      if (obj.ForegroundPixel (srcRow, srcCol))
      {
        itFits = !marker.ForegroundPixel (targetRow, targetCol);
      }
      ++targetCol;
      ++srcCol;
    }
    ++targetRow;
    ++srcRow;
  }

  if (itFits)
  {
    targetRow = topLeftRow;
    srcRow = 0;
    while (targetRow < lastTargetRow)
    {
      kkint32 targetCol = topLeftCol;
      kkint32 srcCol = 0;
      while (targetCol < lastTargetCol)
      {
        auto srcPixel = obj.GetPixelValue (srcRow, srcCol);
        if (srcPixel > 0)
        {
          if (obj.ForegroundPixel(srcRow, srcCol))
          {
            target.SetPixelValue (targetRow, targetCol, srcPixel);
            marker.SetPixelValue (targetRow, targetCol, marker.ForegroundPixelValue ());
          }
          if  (target.GetPixelValue(targetRow, targetCol) < 1)
            target.SetPixelValue (targetRow, targetCol, srcPixel);
        }

        ++targetCol;
        ++srcCol;
      }
      ++targetRow;
      ++srcRow;
    }
  }

  return itFits;
}  /* SeeIfItFits */



BuildSynthObjDetData::PopulateRasterResult*  
      BuildSynthObjDetData::PopulateRaster (DataBaseImageList&  workingList, 
                                            int                 numToPlace)
{
  BoundBoxEntryList*  boundingBoxes = new BoundBoxEntryList ();
  RasterPtr raster = GetNextOpenFrame ();
  //RasterPtr raster = new Raster (imageHeight, imageWidth);
  Raster  marker (raster->Height(), raster->Width(), false);
  int numPlaced = 0;
  int numFailsToFindAPlace = 0;
  while ((numPlaced < numToPlace)  &&  (workingList.size() > 0)  &&  (numFailsToFindAPlace < 20))
  {
    auto nextImageToPlace = workingList.PopFromBack ();
    RasterPtr objToPlace = DB ()->ImageFullSizeFind (nextImageToPlace->ImageFileName ());
    if (objToPlace)
    {
      Point topLeft, botRight;

      auto trimmedObj = ReduceToMinimumSize(objToPlace);
      if (trimmedObj)
      {
        int numAttemptsLeft = 50;
        bool  imagePlaced = false;

        while (!imagePlaced && (numAttemptsLeft > 0))
        {
          kkint32 topLeftColCandidates = raster->Width () - trimmedObj->Width ();
          kkint32 topLeftRowCandidates = raster->Height () - trimmedObj->Height ();
          kkint32 topLeftCol = rng.Next () % topLeftColCandidates;
          kkint32 topLeftRow = rng.Next () % topLeftRowCandidates;
          imagePlaced = SeeIfItFits (*raster, *trimmedObj, marker, topLeftRow, topLeftCol);
          if (imagePlaced)
            boundingBoxes->PushOnBack (new BoundBoxEntry (topLeftRow, topLeftCol, trimmedObj->Height (), trimmedObj->Width (), nextImageToPlace->Class1 ()));

          --numAttemptsLeft;
        }

        if (imagePlaced)
        {
          ++numPlaced;
          delete nextImageToPlace;
        }
        else
        {
          workingList.PushOnFront (nextImageToPlace);
          ++numFailsToFindAPlace;
        }
        nextImageToPlace = NULL;
      }
    }
  }
  return new PopulateRasterResult (raster, boundingBoxes);
}  /* PopulateRaster */



/**@ brief  Empties out 'src' list returning new list that excludes noise images;  noise images are deleted. */
DataBaseImageListPtr  BuildSynthObjDetData::FilerOutNoise (DataBaseImageList& src)
{
  auto filteredList = new DataBaseImageList (true);

  while (src.QueueSize () > 0)
  {
    auto nextCandidate = src.PopFromBack ();
    if (nextCandidate->Class1Name ().StartsWith ("Noise", true))
    {
      delete nextCandidate;
      nextCandidate = NULL;
    }
    else
    {
      filteredList->PushOnBack (nextCandidate);
    }
  }

  return filteredList;
}


#include <random>

int  BuildSynthObjDetData::Main (int argc, char** argv)
{
  std::default_random_engine randEngine (1234567); //seed
  std::normal_distribution<float> normDist (10.0f, 4.0f); //mean followed by stdiv

  LoadAvailableSipperFileNames ();
  maxCandidates = 1000000;
  auto potentialCandidate = GetListOfValidatedImages (candidateMinSize, candidateMaxSize, 0, maxCandidates);
  auto candidates = FilerOutNoise (*potentialCandidate);

  KKStr labelingDataFileName = osAddSlash (targetDir) + "LabelingInfo.tsv";
  ofstream  labelingData (labelingDataFileName.Str ());

  kkint32 imagesCreated = 0;
  while ((imagesCreated < maxImages) && (candidates->size () > 0) && (!cancelFlag))
  {
    kkint32 numObjectsToAdd = Max(5, (kkint32)normDist (randEngine));
    auto createdRaster = PopulateRaster (*candidates, numObjectsToAdd);
    KKStr imageFileName = "PlanktonImage-" + KKB::StrFormatInt (imagesCreated, "00000") + ".bmp";
    KKStr fullImageFileName = osAddSlash (targetDir) + imageFileName;
    KKB::SaveImageGrayscaleInverted4Bit (*(createdRaster->raster), fullImageFileName);
     
    labelingData << imageFileName << "\t" + createdRaster->boundBoxes->ToJsonStr () << endl;
    labelingData.flush ();

    ++imagesCreated;
    delete createdRaster;
    createdRaster = NULL;
    cout << "candidates left: " << candidates->QueueSize () << endl;
  }

  labelingData.close ();

  delete candidates;
  candidates = NULL;

  return 0;
}



int  main (int argc,  char** argv)
{
  BuildSynthObjDetData  app;
  app.InitalizeApplication (argc, argv);
  if (app.Abort ())
    return 1;

  app.Main (argc, argv);
  if (app.Abort ())
    return 1;
  else
    return 0;
}



BuildSynthObjDetData::BoundBoxEntry::BoundBoxEntry (kkint32 _topLeftRow, kkint32 _topLeftCol, kkint32 _height, kkint32 _width, MLClassPtr _mlClass):
  topLeftRow(_topLeftRow), topLeftCol (_topLeftCol), height (_height), width (_width), mlClass (_mlClass)
{
}



BuildSynthObjDetData::BoundBoxEntry::BoundBoxEntry (const BoundBoxEntry& bbe) :
  topLeftRow (bbe.topLeftRow), topLeftCol (bbe.topLeftCol), height (bbe.height), width (bbe.width), mlClass (bbe.mlClass)
{
}



KKStr BuildSynthObjDetData::BoundBoxEntry::ToJsonStr () const
{
  KKStr jsonStr (256);

  jsonStr
    << "{"
    << "\"topLeftCol\":" << topLeftCol << ", "
    << "\"topLeftRow\":" << topLeftRow << ", "
    << "\"width\":" << width << ", "
    << "\"height\":" << height << ", "
    << "\"ClassName\":" << mlClass->Name ().QuotedStr ()
    << "}";

  return jsonStr;
}



BuildSynthObjDetData::BoundBoxEntryList::BoundBoxEntryList ():
  KKQueue<BoundBoxEntry> ()
{}



BuildSynthObjDetData::BoundBoxEntryList::BoundBoxEntryList (const BoundBoxEntryList& list):
  KKQueue<BoundBoxEntry> (list)
{}



KKStr BuildSynthObjDetData::BoundBoxEntryList::ToJsonStr () const
{
  KKStr jsonStr (QueueSize () * 256);

  jsonStr << "[";
  for (size_t x = 0; x < size (); ++x)
  {
    if (x > 0)
      jsonStr << ", ";
    jsonStr << IdxToPtr ((kkint32)x)->ToJsonStr ();
  }

  return jsonStr;
}