/* PicesApplication.cpp -- Generic PicesApplication class.  
 * Copyright (C) 1994-2011 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKU.h
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

#include "OSservices.h"
using  namespace  KKU;




#include "PicesApplication.h"
#include "DataBase.h"
#include "DataBaseServer.h"
using namespace MLL;




PicesApplication::PicesApplication (RunLog&  _log):
  Application (_log),
  db          (NULL),
  dbServer    (NULL)
{
}



PicesApplication::PicesApplication ():
  Application (),
  db          (NULL),
  dbServer    (NULL)
{
}




PicesApplication::PicesApplication (const PicesApplication&  _application):
  Application (_application),
  db          (NULL),
  dbServer    (NULL)
{
  if  (_application.db)
    db = new DataBase (_application.db->Server (), log);

  if  (_application.dbServer)
    dbServer = new DataBaseServer (*_application.dbServer);
}



PicesApplication::~PicesApplication ()
{
  delete  dbServer;  dbServer = NULL;
  delete  db;        db       = NULL;
}




void  PicesApplication::InitalizeApplication (int32   argc,
                                              char**  argv
                                             )
{
  Application::InitalizeApplication (argc, argv);
}



const char*  PicesApplication::ApplicationName ()
{
  return  "PicesApplication";
}  /* ApplicationName */






bool  PicesApplication::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                 const KKStr&  parmValue
                                                )
{
  if  (parmSwitch.EqualIgnoreCase ("-DataBase")  ||  parmSwitch.EqualIgnoreCase ("-DB"))
    return ProcessDataBaseParameter (parmSwitch, parmValue);
  else
    return Application::ProcessCmdLineParameter (parmSwitch, parmValue);
}




bool  PicesApplication::ProcessDataBaseParameter (const KKStr&  parmSwitch, 
                                                  const KKStr&  parmValue
                                                 )
{
  delete  dbServer;  dbServer = NULL;
  delete  db;        db       = NULL;

  bool  validDataBase = false;

  if  (parmValue.Empty ())
  {
    // User wants to us default database server
    db = new DataBase (log);
  }

  else
  {
    dbServer = DataBase::GetServer (parmValue, log);
    if  (!dbServer)
    {
      log.Level (-1) << endl
        << "PicesApplication::ProcessDataBaseParameter  ***ERROR***    Invalid DataBase Sepcified Name [" << parmValue << "]." << endl
        << endl;
      return  false;
    }
    else
    {
      db = new DataBase (dbServer, log);
      if  (db->Valid ())
      {
        validDataBase = true;
      }
      else
      {
        log.Level (-1) << endl
          << "PicesApplication::ProcessDataBaseParameter  ***ERROR***    Could not connect to DataBase [" << parmValue << "]." << endl
          << "     : " << db->LastErrorDesc () << endl
          << endl;
      }
    }
  }

  return  validDataBase;
}  /* ProcessDataBaseParameter */



DataBasePtr   PicesApplication::DB ()
{
  if  (db == NULL)
  {
    db = new DataBase (log);
  }
  return  db;
}




void  PicesApplication::PrintStandardHeaderInfo (ostream&  o)
{
  o << "Application     :" << ApplicationName    () << endl;
  o << "Build Date/Time :" << BuildDate          () << endl;
  o << "Run Date/Time   :" << osGetLocalDateTime () << endl; 
  o << "Host Name       :" << osGetHostName      () << endl;
  o << "User Name       :" << osGetUserName      () << endl;
  if  (db)
    o << "DataBase        :" << db->ServerDescription () << endl;
}  /* PrintStandardHeaderInfo */
