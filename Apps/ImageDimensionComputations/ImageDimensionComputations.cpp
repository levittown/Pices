#include  "FirstIncludes.h"
#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace  KKB;


#include "Classifier2.h"
#include "DuplicateImages.h"
#include "FileDesc.h"
#include "FeatureFileIO.h"
#include "MLClass.h"
#include "TrainingConfiguration2.h"
#include "TrainingProcess2.h"
using namespace  KKMLL;


#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
#include "InstrumentDataFileManager.h"
#include "PicesTrainingConfiguration.h"
#include "PicesVariables.h"
#include "RasterSipper.h"
using namespace  MLL;


#include  "ImageDimensionComputations.h"



ImageDimensionComputations::ImageDimensionComputations ():
  PicesApplication (),

  cancelFlag  (false),
  report      (NULL),
  successful  (true)

{
}  /* ImageDimensionComputations */






ImageDimensionComputations::~ImageDimensionComputations ()
{
  if  (reportFile)
  {
    reportFile->close ();
    delete  reportFile;
  }
}



void  ImageDimensionComputations::InitalizeApplication (kkint32 argc,
                                                        char**  argv
                                                       )
{
  PicesApplication::InitalizeApplication (argc, argv);
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (!DB ())
  {
    log.Level (-1) << endl
      << "ImageDimensionComputations::InitalizeApplication   ***ERROR***  Requires a valid DataBase connection." << endl
      << endl;
    Abort (true);
    return;
  }


  if  (reportFileName.Empty ())
  {
    reportFile = NULL;
    report = &cout;
  }
  else
  {
    reportFile = new ofstream (reportFileName.Str ());
    report = reportFile;
  }

  PicesApplication::PrintStandardHeaderInfo (*report);
  *report << "Report File Name"  << "\t" << reportFileName      << endl;

}  /* InitalizeApplication */




// -c "C:\TrainingApp\DataFiles\TrainingModels\working model.cfg"   -S K:\v1\Temp\ExtractedImages  -D K:\v1\Temp\ClassifiedImages  -n c:\Temp\ExtractedData.data

bool  ImageDimensionComputations::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                           const KKStr&  parmValue
                                                          )
{
  bool  validParm = true;

  if (parmSwitch.EqualIgnoreCase ("-R")  || parmSwitch.EqualIgnoreCase ("-Report"))
  {
    reportFileName = parmValue;
  }
  else
  {
    validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }

  return  validParm;
}  /* ProcessCmdLineParameter */



// -s D:\Pices\TrainingLibraries\NRDA_test_200  -u


void   ImageDimensionComputations::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
       << "    -r  Report File,  Defaults to Command Line."                         << endl
       << endl;
}  /* DisplayCommandLineParameters */



void  ImageDimensionComputations::ProcessImage(SipperFilePtr sf,
                                               const KKStr&   imageFileName
                                              )
{
  *report << sf->CruiseName    () << "\t" 
          << sf->StationName   () << "\t"
          << sf->DeploymentNum () << "\t"
          << endl;

  RasterSipperPtr i = DB ()->ImageFullSizeLoad(imageFileName);
  if  (!i)
    return;

  InstrumentDataPtr id = InstrumentDataFileManager::GetClosestInstrumentData(imageFileName, cancelFlag, log);
  if  (!id)
    return;

  float    flowRate          = id->FlowRate1();
  kkuint32 pixelsPerScanLine = 3800;
  float    scanRate          = sf->ScanRate();
  if  (scanRate < 100.0f)
    scanRate = 25950.0f;

  kkint32 idPixelsPerScanLine = id->CropRight () - id->CropLeft ();
  if  (idPixelsPerScanLine > 100) 
    pixelsPerScanLine = idPixelsPerScanLine;
  

  float pixelHeight = 1000.0f * flowRate / scanRate; //   (m/s)/(sl/s)(1000) = (m/s)(s/sl)(1000) = (m/sl)(1000(mm/m)) = length of pixel in mm

  float pixelWidth = 96.0f /  (float)pixelsPerScanLine;

  float ratioHeightToWidth = pixelHeight / pixelWidth;

  float colFactor = 1.0f;
  float rowFactor = 1.0f / ratioHeightToWidth;

  RasterSipperPtr  streatchedImage = i->StreatchImage(rowFactor, colFactor);


  float pixelArea = pixelHeight * pixelWidth;
  float  momentf[9];
  streatchedImage->CentralMoments (momentf);

  kkint32 pixelCount = (kkint32)(momentf[0] + 0.5f);  // Moment-0 is the same as the number of foreground pixels in image.
  float adjArea = 0.0f;
  float adjHeight = 0.0f;
  float adjWidth  = 0.0f;

  float lenMiliMeters = 0.0f;
  float widthMiliMeters = 0.0f;

  adjArea = pixelArea * pixelCount;

  {
    float  eigenRatio;
    float  orientationAngle;

    streatchedImage->CalcOrientationAndEigerRatio (eigenRatio,
                                                   orientationAngle
                                                 );
    if  ((orientationAngle > TwoPie)  ||  (orientationAngle < -TwoPie))
    {
      orientationAngle = 0.0;
      eigenRatio = 1.0;
    }
    
    RasterSipperPtr rotatedImage = streatchedImage->Rotate (orientationAngle);

    kkint32  tlRow;
    kkint32  tlCol;
    kkint32  brRow;
    kkint32  brCol;

    rotatedImage->FindBoundingBox (tlRow, tlCol, brRow, brCol);
    if  (tlRow < 0)
    {
    }
    else
    {
      kkint32 firstQtrWeight = 0;
      kkint32 forthQtrWeight = 0;

      kkint32 boundingWidth = brCol - tlCol + 1;
      kkint32 boundingHeight  = brRow - tlRow + 1;

      adjHeight = boundingHeight * pixelWidth;
      adjWidth  = boundingWidth  * pixelWidth;

      if  (adjHeight  > adjWidth)
      {
        lenMiliMeters   = adjWidth;
        widthMiliMeters = adjHeight;
      } else {
        widthMiliMeters = adjWidth;
        lenMiliMeters   = adjHeight;
      }

  float widthMilimeters = 0.0f;
    }

    delete  rotatedImage;
    rotatedImage = NULL;
  }

  *report << sf->CruiseName    () << "\t" 
          << sf->StationName   () << "\t"
          << sf->DeploymentNum () << "\t"
          << imageFileName        << "\t"
          << id->Depth         () << "\t"
          << flowRate             << "\t"
          << area                 << "\t"
          << pixelArea            << "\t" 
          << adjArea              << "\t"
          << lenMiliMeters        << "\t"
          << widthMiliMeters;

  delete  streatchedImage;
  streatchedImage= NULL;

  delete i;
  i = NULL;
}  /* ProcessImage */



void ImageDimensionComputations::ProcessSipperFile (SipperFilePtr sf)
{
  const char*  colNames[] = {"ImageFileName", NULL};

  KKStr  selStr(256);
  selStr << "select i.ImageFileName from Images i where i.SipperFileName = " << sf->SipperFileId ();
  KKStrMatrixPtr fileNames = DB()->QueryStatementKKStr(selStr, colNames);
  if  (fileNames != NULL)
  {
    for (kkuint32 idx = 0; idx < fileNames->NumRows();  ++idx)
    {
      ProcessImage(sf, (*fileNames)[idx][0], sf);
    }
  }
}



void  ImageDimensionComputations::ProcessDeployment (SipperDeploymentPtr  deployment)
{
  log.Level (10) << "ImageDimensionComputations::ProcessDeployment  " <<   deployment->CruiseName () << " " <<  deployment->StationName () << " " << deployment->DeploymentNum () << endl;
  log.Level(10) << deployment->CruiseName() << "\t" << deployment->StationName() << "\t" << deployment->DeploymentNum() << endl;
  SipperFileListPtr  sipperFiles = DB ()->SipperFileLoad (deployment->CruiseName (), deployment->StationName (), deployment->DeploymentNum ());
  if  (sipperFiles)
  {
    for (auto idx: *sipperFiles) 
    {
      ProcessSipperFile (idx);
    }
  }
}


//milk 2
//berries 
//chgeasse
//cereal
//pencil sharpner






void  ImageDimensionComputations::Extract ()
{
  log.Level (10) << "ImageDimensionComputations::Extract" << endl;

  InstrumentDataFileManager::Initialize ();

  SipperCruiseListPtr allCruises = DB()->SipperCruiseLoadAllCruises();

  for (auto cruise: *allCruises) 
  {
    log.Level (10) << "Cruise: " + cruise->CruiseName() << endl;

    SipperDeploymentListPtr deployments = DB()->SipperDeploymentLoad(cruise->CruiseName(), "");

    for (auto deployment: *deployments)
    {
      ProcessDeployment (deployment);
    }
  }

  delete  allCruises;
  allCruises = NULL;



  log.Level (10) << "ImageDimensionComputations::Extract    Exiting"  << endl;
}  /* Extract */
