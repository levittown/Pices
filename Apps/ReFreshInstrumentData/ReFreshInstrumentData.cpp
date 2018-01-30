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
#include "SipperFile.h"
#include "PicesVariables.h"
using namespace MLL;


#include "ReFreshInstrumentData.h"




ReFreshInstrumentData::ReFreshInstrumentData ():

   PicesApplication  (),
   cruiseName     (),
   deploymentNum  (),
   stationName    ()
{
}





ReFreshInstrumentData::~ReFreshInstrumentData ()
{
}



void  ReFreshInstrumentData::InitalizeApplication (kkint32 argc,
                                                   char**  argv
                                                  )
{
  DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  InstrumentDataFileManager::InitializePush ();
}  /* InitalizeApplication */





// -cn WB1008 -sn DSH09


bool  ReFreshInstrumentData::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                      const KKStr&  parmValue
                                                     )
{
  bool  parmValid = true;
  if  (parmSwitch.EqualIgnoreCase ("-Cruise")      ||  
       parmSwitch.EqualIgnoreCase ("-CruiseName")  ||  
       parmSwitch.EqualIgnoreCase ("-CN")
      )
  {
    cruiseName = parmValue;
  }

  else if  (parmSwitch.EqualIgnoreCase ("-Station")      ||  
            parmSwitch.EqualIgnoreCase ("-StationName")  ||  
            parmSwitch.EqualIgnoreCase ("-SN")
           )
  {
    stationName = parmValue;
  }

  else if  (parmSwitch.EqualIgnoreCase ("-DeploymentNum")      ||  
            parmSwitch.EqualIgnoreCase ("-Deployment")  ||  
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
void   ReFreshInstrumentData::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
       << "    -CruiseName  <Cruise Name>"       << endl
       << "    -StationName <Statuion NAme>"     << endl
       << "    -Deployment  <CDeploymentNum>"    << endl
       << endl;
}  /* DisplayCommandLineParameters */





void   ReFreshInstrumentData::Main ()
{
  if  (Abort ())
    return;

  DateTime  runDate = osGetLocalDateTime ();

  KKStr reportFileDir = osAddSlash (PicesVariables::ReportDir ()) + "ReFreshInstrumentData";
  osCreateDirectoryPath (reportFileDir);

  reportFileName = osAddSlash (reportFileDir) + "ReFreshInstrumentData";
  if  (!cruiseName.Empty ())
    reportFileName << "_" << cruiseName;
  if  (!stationName.Empty ())
    reportFileName << "_" << stationName;
  if  (!deploymentNum.Empty ())
    deploymentNum << "_" << stationName;
  reportFileName << "_" << runDate.YYYYMMDDHHMMSS () + ".txt";

  ofstream r (reportFileName.Str ());

  r << "ReFreshInstrumentData"  << endl << endl;

  PicesApplication::PrintStandardHeaderInfo (r);

  r << endl
    << "Cruise"      << "\t" << cruiseName                   << endl
    << "Station"     << "\t" << stationName                  << endl
    << "Deployment"  << "\t" << deploymentNum                << endl
    << endl;
    
  InstrumentDataFileManager::InitializePush ();
  SipperFileListPtr  sipperFiles = DB ()->SipperFileLoad (cruiseName, stationName, deploymentNum);
  if  ((!sipperFiles)  ||  (sipperFiles->QueueSize () < 1))
  {
    log.Level (-1) << endl << endl << "No SIPPER Files Found." << endl << endl;
    r << "*** No SIPPER File Located in Database ***" << endl << endl;
    delete  sipperFiles;
    sipperFiles = NULL;
    return;
  }

  auto  numSipperFiles = sipperFiles->size ();
  KKB::uint  sipperFileNum = 0;

  log.Level (10) << "Number Of Sipper Files[" << numSipperFiles << "]." << endl;
  r << "Number Of Sipper Files" << "\t" << numSipperFiles << endl;

  SipperFileList::iterator  idx;

  bool   cancelFlag = false;
  char   msgBuff[1024];

  for  (idx = sipperFiles->begin ();  idx != sipperFiles->end ();  idx++)
  {
    SipperFilePtr sf = *idx;
    ++sipperFileNum;

    KKStr  fullSipperFileName = InstrumentDataFileManager::GetFullSipperFileName (sf->SipperFileName ());
    if  (fullSipperFileName.Empty ())
    {
      log.Level (-1) << endl << "Sipper File: " << sf->SipperFileName () <<  "*** Not Found ***" << endl << endl;
      r << endl << "Sipper File: " << sf->SipperFileName () <<  "*** Not Found ***" << endl << endl;
      continue;
    }

    log.Level (10) << " " << sipperFileNum << " of " << numSipperFiles << "  File [" << sf->SipperFileName () << "]" << endl;
    r << osGetLocalDateTime () << "\t" << sf->SipperFileName () << "\t" << fullSipperFileName << endl;
    DB ()->InstrumentDataReFreshSipperFile (sf->SipperFileName (), cancelFlag, msgBuff, sizeof (msgBuff));
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
  ReFreshInstrumentData  reFreshInstrumentData;
  reFreshInstrumentData.InitalizeApplication (argc, argv);
  if  (reFreshInstrumentData.Abort ())
    return -1;

  reFreshInstrumentData.Main ();
  if  (reFreshInstrumentData.Abort ())
    return -1;
  else
    return 0;
}
