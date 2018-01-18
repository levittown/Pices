// BuildSynthObjDetData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FirstIncludes.h"
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "MemoryDebug.h"
#include "KKBaseTypes.h"
using namespace std;


#include "ImageIO.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


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
  rng(1000)
{
}



BuildSynthObjDetData::~BuildSynthObjDetData ()
{
}



void  BuildSynthObjDetData::InitalizeApplication (kkint32 argc,
                                                  char**  argv
                                                 )
{
  this->DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  maxCandidates = 100000;
}  /* InitalizeApplication */



bool  BuildSynthObjDetData::ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                                     const KKStr&  parmValue
                                                    )
{
  bool  validParm = true;
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



DataBaseImageListPtr  BuildSynthObjDetData::GetListOfValidatedImages (
    float    minSize, 
    float    maxSize, 
    kkuint32 restartImageId,
    kkint32  limit)
{
  MLClassPtr anyClass = NULL;
  auto labeledExamples = DB ()->ImagesQuery (
      "",        // cruiseName,
      "",        // stationName,
      "",        // deploymentNum,
      anyClass,  // mlClass,
      'V',       // classKeyToUse,
      0.0f,      // minProb,
      minSize,
      "",        // dataField1Name,
      0.0f,      // dataField1Min,
      0.0f,      // dataField1Max,
      "",        // dataField2Name,
      0.0f,      // dataField2Min,
      0.0f,      // dataField2Max,
      "",        // dataField3Name,
      0.0f,      // dataField3Min,
      0.0f,      // dataField3Max,
      restartImageId,
      limit
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
    auto trimmedObj = new Raster (src, topLeft, botRight);
    delete src;
    src = NULL;
    return trimmedObj;
  }
}


void  BuildSynthObjDetData::PopulateRaster (Raster&             raster, 
                                            DataBaseImageList&  workingList, 
                                            int                 numToPlace)
{
  Raster  marker (raster.Height(), raster.Width(), false);
  int numPlaced = 0;
  while ((numPlaced < numToPlace)  &&  (workingList.size() > 0))
  {
    auto nextImageToPlace = workingList.PopFromBack ();
    RasterPtr objToPlace = DB ()->ImageFullSizeFind (nextImageToPlace->ImageFileName ());
    if (objToPlace)
    {
      Point topLeft, botRight;

      auto trimmedObj = ReduceToMinimumSize(objToPlace);
      if (trimmedObj)
      {

        int numAttemptsLeft = 30;
        bool  imagePlaced = false;

        while (!imagePlaced && (numAttemptsLeft > 0))
        {
          kkint32 topLeftColCandidates = raster.Width () - trimmedObj->Width ();
          kkint32 topLeftRowCandidates = raster.Height () - trimmedObj->Height ();
          kkint32 topLeftCol = rng.Next () % topLeftColCandidates;
          kkint32 topLeftRow = rng.Next () % topLeftRowCandidates;



          --numAttemptsLeft;
        }

        if (imagePlaced)
        {
          delete nextImageToPlace;
        }
        else
        {
          workingList.PushOnFront (nextImageToPlace);
        }
        nextImageToPlace = NULL;
      }
    }
  }
}


int  BuildSynthObjDetData::Main (int argc, char** argv)
{
  auto candidates = GetListyOfValidatedImages (500.0f, 10000.0f, 0, );
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
