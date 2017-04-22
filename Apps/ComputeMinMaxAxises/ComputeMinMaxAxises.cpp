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
using namespace std;


#include "KKBaseTypes.h"
#include "Compressor.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "DataBase.h"
#include "InstrumentDataFileManager.h"
#include "SipperFile.h"
#include "PicesVariables.h"
using namespace MLL;


#include "ComputeMinMaxAxises.h"




ComputeMinMaxAxises::ComputeMinMaxAxises () :

  PicesApplication (),
  cruiseName (),
  deploymentNum (),
  stationName ()
{
}





ComputeMinMaxAxises::~ComputeMinMaxAxises ()
{
}



void  ComputeMinMaxAxises::InitalizeApplication (kkint32 argc,
  char**  argv
)
{
  DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  if (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  InstrumentDataFileManager::InitializePush ();
}  /* InitalizeApplication */





   // -cn WB1008 -sn DSH09


bool  ComputeMinMaxAxises::ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                                    const KKStr&  parmValue
                                                   )
{
  bool  parmValid = true;
  if (parmSwitch.EqualIgnoreCase ("-Cruise") ||
      parmSwitch.EqualIgnoreCase ("-CruiseName") ||
      parmSwitch.EqualIgnoreCase ("-CN")
     )
  {
    cruiseName = parmValue;
  }

  else if (parmSwitch.EqualIgnoreCase ("-Station") ||
           parmSwitch.EqualIgnoreCase ("-StationName") ||
           parmSwitch.EqualIgnoreCase ("-SN")
          )
  {
    stationName = parmValue;
  }

  else if (parmSwitch.EqualIgnoreCase ("-DeploymentNum") ||
           parmSwitch.EqualIgnoreCase ("-Deployment") ||
           parmSwitch.EqualIgnoreCase ("-DN")
          )
  {
    deploymentNum = parmValue;
  }

  else
  {
    parmValid = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }

  return  parmValid;
}  /* ProcessCmdLineParameter */




   /******************************************************************************
   * DisplayCommandLineParameters()
   * DESC: Displays a help message to the user
   ******************************************************************************/
void   ComputeMinMaxAxises::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
    << "    -CruiseName  <Cruise Name>" << endl
    << "    -StationName <Statuion NAme>" << endl
    << "    -Deployment  <CDeploymentNum>" << endl
    << endl;
}  /* DisplayCommandLineParameters */





void   ComputeMinMaxAxises::Main ()
{
  if (Abort ())
    return;

  const double  pixelsPerScanLineDefault = 3800.0;
  const double  chamberWidthDefault = 0.096f;
  const double  mmPerMeter = 1000.0;
  const double  scanRateDefault = 24950.0;
  const double  flowRate1Default = 0.5;



  DateTime  runDate = osGetLocalDateTime ();

  KKStr reportFileDir = osAddSlash (PicesVariables::ReportDir ()) + "ComputeMinMaxAxises";
  osCreateDirectoryPath (reportFileDir);

  reportFileName = osAddSlash (reportFileDir) + "ComputeMinMaxAxises";
  if (!cruiseName.Empty ())
    reportFileName << "_" << cruiseName;
  if (!stationName.Empty ())
    reportFileName << "_" << stationName;
  if (!deploymentNum.Empty ())
    deploymentNum << "_" << stationName;
  reportFileName << "_" << runDate.YYYYMMDDHHMMSS () + ".txt";

  ofstream r (reportFileName.Str ());

  r << "ComputeMinMaxAxises" << endl << endl;

  PicesApplication::PrintStandardHeaderInfo (r);

  r << endl
    << "Cruise" << "\t" << cruiseName << endl
    << "Station" << "\t" << stationName << endl
    << "Deployment" << "\t" << deploymentNum << endl
    << endl;

  InstrumentDataFileManager::InitializePush ();
  SipperFileListPtr  sipperFiles = DB ()->SipperFileLoad (cruiseName, stationName, deploymentNum);
  if ((!sipperFiles) || (sipperFiles->QueueSize () < 1))
  {
    log.Level (-1) << endl << endl << "No SIPPER Files Found." << endl << endl;
    r << "*** No SIPPER File Located in Database ***" << endl << endl;
    delete  sipperFiles;
    sipperFiles = NULL;
    return;
  }

  KKB::uint  numSipperFiles = sipperFiles->size ();
  KKB::uint  sipperFileNum = 0;

  log.Level (10) << "Number Of Sipper Files[" << numSipperFiles << "]." << endl;
  r << "Number Of Sipper Files" << "\t" << numSipperFiles << endl;

  SipperFileList::iterator  idx;

  bool   cancelFlag = false;
  char   msgBuff[1024];

  auto detrtitusClass = DB ()->MLClassLoad ("detritus_snow");

  for (idx = sipperFiles->begin (); idx != sipperFiles->end (); idx++)
  {
    SipperFilePtr sf = *idx;

    cout << sf->SipperFileName () << endl;
    sipperFileNum++;

    auto deployment = DB ()->SipperDeploymentLoad (sf->CruiseName (), sf->StationName (), sf->DeploymentNum ());

    auto images = DB ()->ImagesQuery(NULL,     // Group Name,
                                      sf->SipperFileName(),  //CruiseName (), sf->StationName (), sf->DeploymentNum (),
                                      detrtitusClass,       // Class Pointer
                                      'P',        //'P' = Use Prediced Class
                                      0.0f,       // minProb 
                                      1.0f,       // maxProb
                                      0,          // minSize 
                                      0,          // maxSize
                                      0.0f, 0.0f, 0, -1, false, cancelFlag);
    for (auto image : *images) {
      auto id = InstrumentDataFileManager::GetClosestInstrumentData (image->ImageFileName (), cancelFlag, log);

      double  pixelsPerScanLine = pixelsPerScanLineDefault;
      double  flowRate1 = flowRate1Default;
      double  scanRate = scanRateDefault;
      double  chamberWidth = chamberWidthDefault;

      if (deployment) {
        if (deployment->ChamberWidth () > 0.0)
          chamberWidth = deployment->ChamberWidth ();
      }

      if (id) {
        pixelsPerScanLine = (double)(1 + id->CropRight () - id->CropLeft ());
        flowRate1 = id->FlowRate1 ();
      }

      if (sf->ScanRate () > 0.0)
        scanRate = sf->ScanRate ();

      double mmPerPixelAccrossChamber = chamberWidth * mmPerMeter / pixelsPerScanLine;
      double mmPerPixelWithFlow = flowRate1  * mmPerMeter / scanRate;

      double flowStretchFactor = mmPerPixelAccrossChamber / mmPerPixelWithFlow;

      auto fullSizeImage = DB ()->ImageFullSizeLoad (image->ImageFileName ());

      auto normalizedImage = fullSizeImage->StreatchImage ((float)flowStretchFactor, 1.0f);


      float  eigenRatio;
      float  orientationAngle;

      normalizedImage->CalcOrientationAndEigerRatio (eigenRatio, orientationAngle);
      if ((orientationAngle > TwoPie) || (orientationAngle < -TwoPie))
      {
        orientationAngle = 0.0;
        eigenRatio = 1.0;
      }

      auto rotatedImage = normalizedImage->Rotate (orientationAngle);

      kkint32 minHeight = 99999999;
      kkint32 minWidth = 9999999;

      // Find minWidth
      for (auto row = 0; row < rotatedImage->Height (); ++row) {
        kkint32 firstCol = -1;
        kkint32 lastCol = -1;
        for (auto col = 0; col < rotatedImage->Width (); ++col) {
          if (rotatedImage->ForegroundPixel (row, col)) {
            lastCol = col;
            if (firstCol < 0)  firstCol = col;
          }
        }
        auto rowWidth = 1 + lastCol - firstCol;
        if ((firstCol > 0) && (rowWidth < minWidth))
          minWidth = rowWidth;
      }


      // Find minHeight
      for (auto col = 0; col < rotatedImage->Width (); ++col) {
        kkint32 firstRow = -1;
        kkint32 lastRow = -1;
        for (auto row = 0; row < rotatedImage->Height (); ++row) {
          if (rotatedImage->ForegroundPixel (row, col)) {
            lastRow = row;
            if (firstRow < 0)  firstRow = row;
          }
        }
        auto colHeight = 1 + lastRow - firstRow;
        if ((firstRow > 0) && (colHeight < minHeight))
          minHeight = colHeight;
      }

      auto minHeightMM = minHeight * mmPerPixelAccrossChamber;
      auto minWidthMM = minWidth * mmPerPixelAccrossChamber;

      cout << image->ImageFileName () << "\t" << minHeightMM << "\t" << minWidthMM << endl;

      delete  rotatedImage;
      rotatedImage = NULL;

      delete  normalizedImage;
      normalizedImage = NULL;

      delete fullSizeImage;
      fullSizeImage = NULL;
    }

    delete images;
    images = NULL;
  }

  r << endl << "Done Processing" << endl;

  InstrumentDataFileManager::InitializePop ();

  delete  sipperFiles;
  sipperFiles = NULL;
}  /* Main */




int  main (int     argc,
  char**  argv
)
{
  ComputeMinMaxAxises  reFreshInstrumentData;
  reFreshInstrumentData.InitalizeApplication (argc, argv);
  if (reFreshInstrumentData.Abort ())
    return -1;

  reFreshInstrumentData.Main ();
  if (reFreshInstrumentData.Abort ())
    return -1;
  else
    return 0;
}
