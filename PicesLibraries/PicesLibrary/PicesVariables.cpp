/* PicesVariables.cpp -- Generic PicesVariables class.  
 * Copyright (C) 1994-2011 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */
#include "FirstIncludes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "MemoryDebug.h"
using  namespace  std;

#include "GlobalGoalKeeper.h"
#include "Option.h"
#include "OSservices.h"
using  namespace  KKB;

#include "KKMLVariables.h"
using namespace  KKMLL;

#include "PicesVariables.h"
using namespace MLL;



bool   PicesVariables::allowUpdates = false;

KKStr  PicesVariables::homeDir = "";

void  PicesVariables::InitializeEnvironment ()
{
  homeDir = "";
  HomeDir ();
}




bool  PicesVariables::ValidHomeDir (const KKStr&  dir)
{
  // The Pices home directory will have a couple of sub directories below it which must exist.
  return   ((osValidDirectory (osAddSlash (dir) + "exe"))        &&
            (osValidDirectory (osAddSlash (dir) + "DataFiles"))  &&
            (osValidDirectory (osAddSlash (dir) + "MySQL"))      &&
            (osValidDirectory (osAddSlash (dir) + "Classifier"))
           );
}


const KKStr&  PicesVariables::HomeDir ()
{
  if  (!homeDir.Empty ())
    return homeDir;

  GlobalGoalKeeper::StartBlock ();
  {
    KKStrPtr  homeDirVar = osGetEnvVariable ("PicesHomeDir");
    if  (homeDirVar)
      homeDir = *homeDirVar;
    delete  homeDirVar;
    homeDirVar = NULL;
  }

  if  (homeDir.Empty ())
  {
    KKStr  fullAppName = osGetFullPathOfApplication ();
    KKStr  pathToApp = osGetPathPartOfFile (fullAppName);
    KKStr  parentPathToApp = osGetParentDirPath (pathToApp);
    if  (ValidHomeDir (parentPathToApp))
    {
      homeDir = parentPathToApp;
    }
    else
    {
      KKStr  curDir = osGetCurrentDirectory ();
      if  (ValidHomeDir (curDir))
      {
        homeDir = curDir;
      }
      else
      {
        KKStr parentCurDir = osGetParentDirPath (curDir);
        if  (ValidHomeDir (parentCurDir))
        {
          homeDir = parentCurDir;
        }
        else
        {
          osDisplayWarning ("Can Not locate a valid PICES Home Directory; make sure 'PicesHomeDir' is setup correctly.");
        }
      }
    }
  }

  KKMLVariables::SetMachineLearninigHomeDir (osAddSlash (homeDir) + "Classifier");

  ReadPermissions ();

  GlobalGoalKeeper::EndBlock ();
  return homeDir;
}  /* PicesHomeDir */



void  PicesVariables::SetHomeDir (const KKStr&  _homeDir)
{
  homeDir = _homeDir;
}



KKStr  PicesVariables::CmdLineProgDir ()
{
  return osAddSlash (HomeDir ()) + "exe";
}



KKStr  PicesVariables::InstrumentDataFilesDir ()
{
  KKStr  s = osAddSlash (DataFilesDirectory ()) + "InstrumentDataFiles";
  return  s;
}
 


KKStr  PicesVariables::InstrumentDataReportDir ()
{
  return  osAddSlash (ReportDir ()) + "InstrumentData";
}


KKStr  PicesVariables::SipperInterfaceDir ()
{
  KKStrPtr  sidPtr = osGetEnvVariable ("SipperInterfaceDir");
  if  (sidPtr)
    return  KKStr (*sidPtr);

  KKStr  sipperInterfaceDir = osAddSlash (HomeDir ()) +  "SipperInterfaceDir";
  return  sipperInterfaceDir;
}



KKStr  PicesVariables::SipperInterfaceLogDir ()
{
  KKStr  sipperInterfaceLogDir = osAddSlash (SipperInterfaceDir ()) + "LogFiles";
  return  sipperInterfaceLogDir;
}  /* SipperInterfaceLogDir */




KKStr  PicesVariables::SipperDirectoryListFileName ()
{
  KKStr  sipperDirectoryListFileName = osAddSlash (ConfigurationDirectory ()) + "SipperDirectoryList.cfg";
  return  sipperDirectoryListFileName;
}



KKStr  PicesVariables::SipperFilesDefaultDir ()
{
  KKStr  sipperFilesDefaultDir = osAddSlash (HomeDir ()) + "SipperFiles";
  return  sipperFilesDefaultDir;
}



KKStr  PicesVariables::ConfigurationDirectory ()
{
  KKStr  configDir = osAddSlash (osAddSlash (HomeDir ()) + "Configurations");
  return  configDir;
}



KKStr  PicesVariables::ConfigrationFileName ()
{
  KKStr  configFileName = osAddSlash (ConfigurationDirectory ()) + "SipperConfiguration.txt";
  return  configFileName;
}





KKStr  PicesVariables::DataFilesDirectory ()
{
  return  osAddSlash (HomeDir ()) + "DataFiles";
}



KKStr  PicesVariables::TrainingModelsConfigurationDir ()
{
  return  KKMLVariables::TrainingModelsDir ();
}


KKStr  PicesVariables::TrainingModelsSaveFilesDir ()
{
  return  KKMLVariables::TrainingModelsDir ();
}



KKStr  PicesVariables::TrainingLibrariesDir ()
{
  return  KKMLVariables::TrainingLibrariesDir ();
}


KKStr  PicesVariables::ValidationInfoDir ()
{
  return osAddSlash (PicesVariables::DataFilesDirectory ()) + "ValidationHistory";
}



KKStr  PicesVariables::ReportDir ()
{
  return  osAddSlash (HomeDir ()) + "Reports";
}



KKStr  PicesVariables::DataFileDirectory ()
{
  return  osAddSlash (HomeDir ()) + "DataFiles";
}



KKStr  PicesVariables::TempDirectory ()
{
  KKStr  tempDirName = osAddSlash (DataFileDirectory ()) + "Temp";
  if  (!osValidDirectory (tempDirName))
    osCreateDirectoryPath (tempDirName);
  return  tempDirName;
}






KKStr  PicesVariables::LatitudeToStr  (double latitude)
{
  bool  north = (latitude >= 0.0);
  latitude = fabs (latitude);

  while  (latitude > 90)
  {
    latitude = 180 - latitude;
    if  (latitude < 0.0)
    {
      latitude = fabs (latitude);
      north = !north;
    }
  }

  double  latitudeDegrees = 0.0;
  double  latitudeMinutes = modf (latitude, &latitudeDegrees) * 60.0;

  KKStr  latStr (12);
  latStr << StrFormatInt ((int)latitudeDegrees, "00") 
         << ":"
         << StrFormatDouble (latitudeMinutes, "#00.0000")
         << " "
         << (north ? "N" : "S");
  return  latStr;
}  /* LatitudeToStr */



KKStr  PicesVariables::LongitudeToStr  (double longitude)
{
  bool  east = longitude >= 0.0;
  longitude = fabs (longitude);
  while  (longitude > 180.0)
  {
    longitude = longitude - 360.0;
    if  (longitude < 0.0)
    {
      longitude = fabs (longitude);
      east = !east;
    }
  }

  double  longitudeDegrees = 0.0;
  double  longitudeMinutes = modf (longitude, &longitudeDegrees) * 60.0;

  KKStr  longStr (12);
  longStr << StrFormatInt ((int)longitudeDegrees, "##0")
    << ":" 
    << StrFormatDouble (longitudeMinutes, "#00.0000")
    << " "
    << (east  ? "E" : "W");
  return  longStr;
}  /* LongitudeToStr */



bool  PicesVariables::AllowUpdates ()
{
  if  (homeDir.Empty ())
    HomeDir ();
  return  allowUpdates;
}



void  PicesVariables::ParseImageFileName (const KKStr&  fullFileName, 
                                          KKStr&        sipperFileName,
                                          kkuint32&     scanLineNum,
                                          kkuint32&     scanCol
                                         )
{
  sipperFileName = "";
  scanLineNum    = 0;
  scanCol        = 0;
  
  KKStr  rootName = osGetRootName (fullFileName);
  if  (rootName.Empty ())
    return;
  
  auto  x = rootName.LocateLastOccurrence ('_');
  if  (x  &&  (x.value () > 0))
  {
    KKStr  colStr = rootName.SubStrPart (x + 1);
    KKStr  temp = rootName.SubStrSeg (0, x);
    x = temp.LocateLastOccurrence ('_');
    if  (x  &&  (x.value () > 0))
    {
      sipperFileName = temp.SubStrSeg (0, x);
      KKStr  rowStr = temp.SubStrPart (x + 1);
      scanCol     = atoi (colStr.Str ());
      scanLineNum = atoi (rowStr.Str ());
    }
  }
}  /* ParseImageFileName */



void  PicesVariables::ReadPermissions ()
{
  allowUpdates = false;
  KKStr  configFileName = osAddSlash (ConfigurationDirectory ()) + "Permissions.cfg";

  ifstream  in (configFileName.Str ());
  if  (!in.is_open ())
  {
    cerr << "PicesVariables::ReadPermissions   File failed to open: " + configFileName << endl;
    return;
  }

  bool  eof = false;
  KKStrPtr ln = osReadRestOfLine(in, eof);
  while  (ln != NULL)
  {
    if  ((ln->Len () > 3)  &&  (!ln->StartsWith("//")))
    {
      KKStr  lineName = ln->ExtractToken2 ("\t=");
      if  (lineName.EqualIgnoreCase ("AllowUpdates"))
        allowUpdates = ln->ExtractTokenBool ("\n\r\t");
    }
    delete  ln;
    ln = osReadRestOfLine(in, eof);
  }

  in.close ();
}
