//  because <my_global.h> needs to define some Macro's before windows.h  and stdlib.h  we need t include
// before  "FirstIncludes.h"
#include <errmsg.h>
#include <my_global.h>
#undef min
#undef max


#include "FirstIncludes.h"
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#if  defined(WIN32)
#include <windows.h>
#include <wingdi.h>
#endif
#include "MemoryDebug.h"
using namespace std;
#include <my_sys.h>
#include <mysql.h>


// Base Library
#include "KKBaseTypes.h"
#include "KKException.h"
#include "OSservices.h"
#include "RasterSipper.h"
using namespace KKB;


#include "FileDesc.h"
#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
using  namespace  KKMLL;


#include "DataBase.h"
#include "DataBaseServer.h"
#include "GPSDataPoint.h"
#include "InstrumentDataFileManager.h"
#include "SipperFile.h"
#include "SipperStation.h"
#include "PicesVariables.h"
#include "SipperCruise.h"
#include "VolumeSampledStat.h"
using  namespace  MLL;




KKB::kkint32 CharStarToInt32 (const char* s)
{
  if  (s == NULL)
    return 0;
  return (KKB::kkint32)atoi (s);
}  /* CharStarToInt32 */


KKB::kkuint32 CharStarToUint16 (const char* s)
{
  if  (s == NULL)  return 0;
  return (KKB::kkuint16)atol (s);
}  /* CharStarToUint32 */



KKB::kkuint32 CharStarToUint32 (const char* s)
{
  if  (s == NULL)  return 0;
  return (KKB::kkuint32)atol (s);
}  /* CharStarToUint32 */



KKB::ulong  CharStarToUlong (const char*  s)
{
  if  (s == NULL)  return 0;
  KKB::ulong  l = (KKB::ulong)atol (s);
  return l;
}  /* CharStarToUlong */



KKB::kkint64  CharStarToInt64 (const char*  s)
{
  if  (s == NULL)  return 0;
  #if  defined(__GNUC__)
    kkint64  l = (KKB::kkint64)atoll (s);
  #else
    kkint64  l = (KKB::kkint64)_atoi64 (s);
  #endif
  return l;
}  /* CharStarToInt64 */



KKB::kkuint64  CharStarToUint64 (const char*  s)
{
  if  (s == NULL)  return 0;

  #if  defined(__GNUC__)
    KKB::kkuint64  l = (KKB::kkuint64)atoll (s);
  #else
    KKB::kkuint64  l = (KKB::kkuint64)_atoi64 (s);
  #endif
  return l;
}  /* CharStarToUint64 */



float  CharStarToFloat (const char*  s)
{
  if  (s == NULL)  return 0.0f;
  return  (float)atof (s);
}  /* CharStarToFloat */



double  CharStarToDouble (const char*  s)
{
  if  (s == NULL)  return 0.0;
  return  atof (s);
}  /* CharStarToDouble */




void  DataBase::WriteBuff (char*        dest,
                           KKB::kkint32 destSize,
                           const char*  src
                          )
{
  memset (dest, 0, destSize);
  if  (strlen (src) < (size_t)destSize)
  {
    STRCOPY (dest, destSize, src);
  }
  else
  {
    KKB::STRCOPY (dest, destSize - 1, src);
    dest[destSize - 1] = 0;
  }
  return;
}


#if defined(WIN32)
KKStr  DataBase::FloatToStr (float f)
{
  KKStr  s (12);
  switch  (_fpclass (f))
  {
  case  _FPCLASS_PINF:
  case  _FPCLASS_NINF:
  case  _FPCLASS_SNAN:
  case  _FPCLASS_QNAN:
    s = "0.0";
    break;

  default:
    s << f; 
  }
  return  s;  
}  /* FloatToStr */
#else

KKStr  DataBase::FloatToStr (float f)
{
  KKStr s(20);
  if  (isnan (f))
  {
    s =  "0.0";
  }
  else
  { 
    s << f;
  }
  return  s;
}  /* FloatToStr */

#endif



GoalKeeperPtr            DataBase::blocker                  = NULL;
volatile KKB::kkint32    DataBase::numDataBaseInstances     = 0;
volatile bool            DataBase::mySqlEmbeddedServerInitailzied = false;
KKB::KKStr               DataBase::mySqlHomeDir             = "";
KKB::KKStr               DataBase::mySqlDataDir             = "";
KKB::KKStr               DataBase::mySqlErroLogFileName     = "";
volatile bool            DataBase::staticVariablesInialized = false;
DataBase::DataBaseIndex  DataBase::existingInstances;



void  DataBase::Initialization ()
{
  CreateBlocker ();
  blocker->StartBlock ();
  InitializeDataBaseStaticVariables ();
  blocker->EndBlock ();
}


bool  DataBase::EmbeddedServerRunning ()
{
  return  mySqlEmbeddedServerInitailzied;
}



// Will initiate an instance of "GoalKeeper" if "blocker" does not already
// point one.
void  DataBase::CreateBlocker ()
{
  if  (!blocker)
    GoalKeeper::Create ("DataBaseBlocker", blocker);  // Will handle Race condition.
}


void   DataBase::ErrorLogMsg (const KKStr&  msg)
{
  if  (blocker)
    blocker->StartBlock ();

  ofstream  o (mySqlErroLogFileName.Str (), ios::app);
  o << endl << osGetThreadId () << ": " << osGetLocalDateTime () << ": " << msg << endl;
  o.close ();

  if  (blocker)
    blocker->EndBlock ();
}  /* WriteErrorLogMsg */



void   DataBase::UpdatesNotAllowed (const KKStr&  methodName)
{
  ErrorLogMsg ("Attempt to perform Update:" + methodName);
}




void  DataBase::InitializeDataBaseStaticVariables ()
{
  if  (!staticVariablesInialized)
  {
    CreateBlocker ();
    blocker->StartBlock ();

    if  (!staticVariablesInialized)
    {
      KKStr  picesHomeDir = PicesVariables::HomeDir ();
      if  (picesHomeDir.Empty ())
      {
        KKStr  errMsg = "DataBase::InitializeDataBaseStaticVariables    ***ERROR***       Can not locate the PICES home directory.    Please make sure that the Environment variable \"PicesHomeDir\" is set to the correct location.";
        cerr << endl << errMsg << endl << endl;
        osDisplayWarning (errMsg);
        exit (-1);
      }

      mySqlHomeDir = osAddSlash (osAddSlash (picesHomeDir) + "MySql");
      mySqlDataDir = osAddSlash (mySqlHomeDir) + "data";
      mySqlErroLogFileName = osAddSlash (mySqlHomeDir) + "DataBase_ErrorLog.txt";
      osCreateDirectoryPath (mySqlHomeDir);
      staticVariablesInialized = true;
      InitializeMySqlLibrary ();
      atexit (DataBase::FinalCleanUp);
    }

    blocker->EndBlock ();
  }
}



void  DataBase::InitializeMySqlLibrary ()
{
#if  defined(EMBEDDED_MYSQL)
  if  (osValidDirectory (mySqlDataDir))
    InitializeMySqlLibraryEmbedded ();
  else
    InitializeMySqlLibraryServer ();
#else
  InitializeMySqlLibraryServer ();
#endif
}



void  DataBase::InitializeMySqlLibraryServer ()
{
  cout << "DataBase::InitializeMySqlLibraryServer" << endl;
  kkint32  retVal = mysql_library_init (-1, NULL, NULL);
  if  (retVal == 0)
  {
    bool  threadSafe = mysql_thread_safe () != 0;
    if  (!threadSafe)
    {
      ErrorLogMsg ("DataBase::InitializeMySqlLibraryServer     ***NOT THREAD SAFE ***");
    }
  }
}  /* InitializeMySqlLibraryServer */




KKB::kkint32  initializeMySqlLibraryEmbeddedThreadId = 0;

void  DataBase::InitializeMySqlLibraryEmbedded ()
{
  KKStr  startDefaultDir = osGetCurrentDirectory ();

  cout << "DataBase::InitializeMySqlLibraryEmbedded" << endl;
  KKStr dataDir   = "--datadir="  + mySqlDataDir;
  KKStr baseDir   = "--basedir="  + mySqlHomeDir;
  KKStr language  = "--language=" + osAddSlash (osAddSlash (mySqlHomeDir) + "share") + "english";
  KKStr defaultsFile = osAddSlash (mySqlDataDir) + "my.ini";

  char**  server_options = new char*[10];
  server_options[0] = STRDUP ("mysql_test");
  server_options[1] = STRDUP (baseDir.Str ());
  server_options[2] = STRDUP (dataDir.Str ());
  server_options[3] = STRDUP (language.Str ());
  server_options[4] = STRDUP ("--tmp-table-size=256M");
  server_options[5] = STRDUP ("--innodb-flush-log-at-trx-commit=1");
  server_options[6] = STRDUP ("--innodb-buffer-pool-size=512M");
  server_options[7] = STRDUP ("--innodb_flush_method=normal");    /**<  This setting allowed me to run on 3TB external USB drive
                                                                   * see http://www.apachefriends.org/f/search.php?st=0&sk=t&sd=d&sr=posts&sid=24b3f455e3ba3c7ebe038d06957fe9d3&keywords=InnoDB&fid%5B%5D=16&ch=-1&start=75
                                                                   */
  server_options[8] = NULL;
  server_options[9] = NULL;
  kkint32  num_elements = 8;

  char const *  server_groups[] = {"libmysqld_server", "client", "mysqld",  NULL};

  kkint32  retval = 0;

  // We want to make sure that when MySQl is shutdown (call to mysql_library_end()) it is done by the same thread.
  initializeMySqlLibraryEmbeddedThreadId = osGetThreadId ();

  retval = mysql_library_init (num_elements, 
                               server_options, 
                               (char **)server_groups
                              );
  if  (retval != 0)
  {
    KKStr  errMsg = mysql_error (NULL);
    KKStr  errMsgStr = "Call to \"mysql_library_init\"  Failed with RetVal[" + StrFormatInt (retval, "##0") + "]";
    ErrorLogMsg (errMsgStr);
    ErrorLogMsg ("Error Description = " + errMsg);
  }

  cout  << " DataBase::InitializeMySqlLibraryEmbedded   num_elements" << "\t" << num_elements << endl
        << "                                            RetVal"       << "\t" << retval       << endl;


  if  (retval == 0)
  {
    mySqlEmbeddedServerInitailzied = true;
    bool  threadSafe = mysql_thread_safe () != 0;
    if  (!threadSafe)
      ErrorLogMsg ("DataBase::InitializeMySqlLibraryEmbedded     ***NOT THREAD SAFE ***");
  }

  delete  server_options[0];
  delete  server_options[1];
  delete  server_options[2];
  delete  server_options;
  server_options = NULL;


  // The following code is attempting to duplicate a bug with "mysql_library_end" where it hangs under normal operation.
  /*
  RunLogPtr  runLog = new RunLog ();

  DataBasePtr dbConnFirstOne = new DataBase (*runLog);

  DataBasePtr dbConn = new DataBase (*runLog);

  SipperCruiseListPtr cruises = dbConn->SipperCruiseLoadAllCruises ();

  MLClassListPtr  classes = dbConn->MLClassLoadList ();

  DataBaseImageGroupListPtr  imageGroups = dbConn->ImageGroupLoad ();   // returns a list of all Group Assignments

  SipperStationListPtr  stations = dbConn->SipperStationLoad ("SD01");

  SipperDeploymentListPtr  deployments = dbConn->SipperDeploymentLoad ("SD01",
                                                                       "06day"
                                                                      );

  delete  dbConnFirstOne;  dbConnFirstOne = NULL;

  delete  dbConn;  dbConn = NULL;


  ThreadEnd ();


  try  {mysql_library_end ();}
  catch  (std::exception  e1)
  {
    cerr << endl << "DataBase::FinalCleanUp    ***EXCEPTION***    " << e1.what () << endl << endl;
  }
  catch  (...)
  {
    cerr << endl << "DataBase::FinalCleanUp    ***EXCEPTION***    " << endl << endl;
  }

  delete  runLog;  runLog = NULL;
*/


}  /* InitializeMySqlLibraryEmbedded */




/**
 *@details  When using Embedded server inside a ".net" application you need to call the "FinalCleanUp" method at the same level
 *  that the Initialization method gets called.  If you wait until the "atexit function in "DateBase" gets called all the threads 
 *  that the MySQL server create will be terminated and "mysql_library_end"  will hang.
 */
void  DataBase::FinalCleanUp ()
{
  if  (!staticVariablesInialized)
  {
    // No need to run 'FinalCleanUp';  Either static variables were never initialized or 'FinalCleanUp'
    // was not ran yet.
    return;
  }

  if  (!blocker)
  {
    // If the GoalKeeper instance 'blocker' is not defined then either the DataBase initialization code never 
    // completed or some other thread had already deleted 'blocker'.  In either case we can not perform
    // the clean up code safely.
    return;
  }

  blocker->StartBlock ();
  if  (!staticVariablesInialized)
  {
    blocker->EndBlock ();
    return;
  }

  // If you made it this far; then you are the thread that has to clean up the 'DataBase' static variables.
  staticVariablesInialized = false;
  DataBaseIndex::iterator  idx;
  for  (idx = existingInstances.begin ();  idx != existingInstances.end ();  ++idx)
  {
    DataBasePtr  db = idx->first;
    db->Close ();
  }

  existingInstances.clear ();

  if  (mySqlEmbeddedServerInitailzied)
  {
    // We only want to call 'mysql_library_end' if we initialized embedded server.

    kkint32  curThreadId = osGetThreadId ();
    if  (curThreadId != initializeMySqlLibraryEmbeddedThreadId)
    {
      KKStr errMsg (128);
      errMsg << "DataBase::FinalCleanUp    Wrong thread is calling 'mysql_library_end'" << endl
             << "   Initialization ThreadId[" << initializeMySqlLibraryEmbeddedThreadId << "]  CurrentThreadId[" << curThreadId << "]" << endl;

      ErrorLogMsg (errMsg);
    }


    try  {mysql_library_end ();}
    catch  (std::exception  e1)
    {
      cerr << endl << "DataBase::FinalCleanUp    ***EXCEPTION***    " << e1.what () << endl << endl;
    }
    catch  (...)
    {
      cerr << endl << "DataBase::FinalCleanUp    ***EXCEPTION***    " << endl << endl;
    }
  }

  if  (featureDataFieldNames)
  {
    int x = 0;
    while  (featureDataFieldNames[x] != NULL)
    {
      delete  featureDataFieldNames[x];  featureDataFieldNames[x] = NULL;
      ++x;
    }
    delete  featureDataFieldNames;
    featureDataFieldNames = NULL;
  }

  staticVariablesInialized = false;

  blocker->EndBlock ();
  GoalKeeper::Destroy (blocker);
  blocker = NULL;
}  /* FinalCleanUp */



void   DataBase::DataBaseAddInstance (DataBasePtr  _instance)
{
  existingInstances.insert (DataBaseIndexPair (_instance, _instance));
}



void   DataBase::DataBaseDeleteInstance (DataBasePtr  _instance)
{
  DataBaseIndex::iterator  idx;
  idx = existingInstances.find (_instance);
  if  (idx != existingInstances.end ())
    existingInstances.erase (idx);
}




DataBase::DataBase (RunLog&  _log):
  allowUpdates      (false),
  conn              (NULL),
  useEmbeddedServer (false),
  featureFileDesc   (NULL),
  resultSet         (NULL),
  server            (NULL),
  log               (_log)

{
  CreateBlocker ();
  blocker->StartBlock ();
  allowUpdates = PicesVariables::AllowUpdates ();
  InitializeDataBaseStaticVariables ();
  server = GetDefaultMySqlParameters ();
  int  returnCd = Connect ();
  valid  = (returnCd == 0);
  numDataBaseInstances++;
  DataBaseAddInstance (this);
  blocker->EndBlock ();
}



DataBase::DataBase (DataBaseServerConstPtr  _server,
                    RunLog&                 _log
                   ):

  allowUpdates      (false),
  conn              (NULL),
  featureFileDesc   (NULL),
  resultSet         (NULL),
  server            (NULL),
  useEmbeddedServer (false),
  log               (_log)
  
{
  CreateBlocker ();
  blocker->StartBlock ();

  allowUpdates = PicesVariables::AllowUpdates ();

  InitializeDataBaseStaticVariables ();
  
  if  (_server)
  {
    server = new DataBaseServer (*_server);
  }
  else
  {
    log.Level (-1) << endl
                   << "DataBase::DataBase  ***ERROR***       No Server Parameters provided;  will use default ones." << endl
                   << endl;
    server = new DataBaseServer ();
  }

  int  returnCd = Connect ();
  valid  = (returnCd == 0);
  numDataBaseInstances++;
  DataBaseAddInstance (this);
  blocker->EndBlock ();
}




DataBase::~DataBase ()
{
  CreateBlocker ();
  blocker->StartBlock ();

  Close ();

  delete  server;
  server = NULL;
  numDataBaseInstances--;
  DataBaseDeleteInstance (this);
  blocker->EndBlock ();
}




void  DataBase::Close ()
{
  CreateBlocker ();
  blocker->StartBlock ();

  if  (conn)
  {
    mysql_close (conn);
    conn = NULL;
  }

  blocker->EndBlock ();
}  /* Close */




bool  DataBase::ThreadInit ()
{
  // http://bugs.mysql.com/bug.php?id=59082

  MYSQL*  tempPtr = NULL;
  tempPtr = mysql_init (tempPtr);
  //my_init ();
  if (!mysql_thread_safe ())
  {
    ErrorLogMsg ("DataBase::ThreadInit   \"mysql_thread_safe\"  returned 'false';        Major problem: libmysql compiled ThreadSafe?");
    return  false;
  }

  bool  successful = (bool)(mysql_thread_init () == 0);

  mysql_close(tempPtr);

  return  successful;
}  /* ThreadInit */



void  DataBase::ThreadEnd ()
{
  mysql_thread_end ();
}  /* ThreadEnd */



DataBaseServerListPtr  DataBase::GetListOfServers (RunLog&  _log)
{
  CreateBlocker ();
  blocker->StartBlock ();

  DataBaseServerListPtr  list = new DataBaseServerList (_log);

  blocker->EndBlock ();
  return  list;
}



DataBaseServerPtr  DataBase::GetDefaultServer (RunLog&  _log)
{
  DataBaseServerPtr  defaultSerever = NULL;
  CreateBlocker ();
  blocker->StartBlock ();

  DataBaseServerListPtr  list = new DataBaseServerList (_log);
  if  (list)
  {
    defaultSerever = list->GetDefaultServer ();
    if  (defaultSerever)
      defaultSerever = new DataBaseServer (*defaultSerever);  //  Caller will own this instance.
  }

  delete  list;  list = NULL;
  blocker->EndBlock ();
  return  defaultSerever;
}  /* GetDefaultServer */




DataBaseServerPtr  DataBase::GetServer (const KKStr&  description,
                                        RunLog&       _log
                                       )
{
  DataBaseServerPtr  selServer = NULL;
  CreateBlocker ();
  blocker->StartBlock ();

  DataBaseServerListPtr  list = new DataBaseServerList (_log);
  if  (list)
  {
    selServer = list->LookUpByDescription (description);
    if  (selServer)
      selServer = new DataBaseServer (*selServer);
  }
  delete  list;
  list = NULL;

  blocker->EndBlock ();
  return  selServer;
}




KKStr  DataBase::ServerDescription ()  const
{
  if  (server == NULL)
    return  "*** no server parameters selected ***";

  KKStr  description = server->ServerDescription ();
  if  (!Valid ())
    description << "  ***ERROR***";

  return  description;
}  /* ServerDescription */




KKB::kkint32  DataBase::Connect ()
{
  kkint32  returnCd = 0;
  if  (server->Description ().EqualIgnoreCase ("Embedded"))
  {
    useEmbeddedServer = true;
    returnCd = ConnectEmbedded ();
  }
  else
  {
    useEmbeddedServer = false;
    returnCd = ConnectServer ();
  }

  return  returnCd;
}  /* Connect */




KKB::kkint32  DataBase::ConnectEmbedded ()
{
  if  (conn)
  {
    mysql_close (conn);
    conn = NULL;
  }

  conn = mysql_init (NULL);

  if  (conn == NULL)
  {
    log.Level (-1) << endl << endl
                   << "DataBase     ***ERROR***" << endl
                   << endl
                   << "       mysql_init ()    Failed    No database activity can work without it" << endl
                   << endl;
    valid = false;
    return - 1;
  }

  // mysql_options (conn, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);

  //kkuint32  timeOut = 120;
  //kkint32 returnCd = mysql_options (conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)(&timeOut)); 
  kkint32 returnCd = mysql_options (conn, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL); 

  MYSQL*  returnMYSQLstruct = 
        mysql_real_connect (conn,                /* pointer to connection handler */
                            server->HostName     ().Str (),
                            server->UserName     ().Str (),
                            server->PassWord     ().Str (),
                            server->DataBaseName ().Str (), /* Database Name                 */
                            0,                        /* port (use default)            */
                            NULL,                     /* socket (use default)          */
                            CLIENT_MULTI_STATEMENTS   /* flags (none)                  */
                           );

  if  (returnMYSQLstruct == NULL)
  {
    // The connection failed.
    valid = false;
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);
    log.Level (-1) << endl 
                   << endl
                   << "DataBase     ***ERROR***" << endl
                   << endl
                   << "       ErrorCd      [" << lastMySqlErrorNo        << "]" << endl
                   << "       Error        [" << lastMySqlErrorDesc      << "]" << endl
                   << "       HostName     [" << server->HostName     () << "]" << endl
                   << "       UserName     [" << server->UserName     () << "]" << endl
                   << "       DataBaseName [" << server->DataBaseName () << "]" << endl
                   << endl;
    return  lastMySqlErrorNo;
  }
  else
  {
    valid = true;
    lastMySqlErrorNo = 0;
    lastMySqlErrorDesc = "";
    return 0;
  }

}  /* ConnectEmbedded */



KKB::kkint32  DataBase::ConnectServer ()
{
  if  (conn)
  {
    mysql_close (conn);
    conn = NULL;
  }

  conn = mysql_init (NULL);

  if  (conn == NULL)
  {
    KKStr  msg (255);
    msg << "DataBase::ConnectServer     ***ERROR***   'mysql_init' Failed  No database activity can work without it.";
    log.Level (-1) << endl << endl << msg << endl << endl;
    valid = false;
    return - 1;
  }


  //kkuint32  timeOut = 120;
  //kkint32 returnCd = mysql_options (conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)(&timeOut)); 
  //http://www.linuxtopia.org/online_books/database_guides/mysql_5.1_database_reference_guide/mysql-options.html
  kkint32 returnCd = mysql_options (conn, MYSQL_OPT_USE_REMOTE_CONNECTION, NULL); 

  MYSQL*  returnMYSQLstruct = NULL;

  {
    char*  hostName     = STRDUP (server->HostName     ().Str ());
    char*  userName     = STRDUP (server->UserName     ().Str ());
    char*  passWord     = STRDUP (server->PassWord     ().Str ());
    char*  dataBaseName = STRDUP (server->DataBaseName ().Str ());
   
    returnMYSQLstruct = 
        mysql_real_connect (conn,                           /* pointer to connection handler */
                            hostName,
                            userName,
                            passWord,
                            dataBaseName,                   /* Database Name        */
                            0,                              /* port (use default)   */
                            NULL,                           /* socket (use default) */
                            CLIENT_MULTI_STATEMENTS         /* flags (none)         */
                           );

    delete hostName;     hostName = NULL;
    delete userName;     userName = NULL;
    delete passWord;     passWord = NULL;
    delete dataBaseName; dataBaseName = NULL;

  }

  if  (returnMYSQLstruct == NULL)
  {
    // The connection failed.
    valid = false;
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    KKStr  logMsg (256);
    logMsg << "DataBase::ConnectServer  ***ERROR***  returnMYSQLstruct == NULL"  << endl
           << "mysql_errno"     << "\t" << lastMySqlErrorNo        << "\t" 
           << "mysql_error"     << "\t" << lastMySqlErrorDesc      << endl
           << "HostName"        << "\t" << server->HostName ()     << "\t" 
           << "UserName"        << "\t" << server->UserName ()     << "\t" 
           << "DataBaseName"    << "\t" << server->DataBaseName () << endl;

    ErrorLogMsg (logMsg);
    
    log.Level (-1) << endl << endl << logMsg << endl << endl;
    return  lastMySqlErrorNo;
  }
  else
  {
    valid = true;
    lastMySqlErrorNo = 0;
    lastMySqlErrorDesc = "";
    return 0;
  }
}  /* ConnectServer */




DataBaseServerPtr  DataBase::GetDefaultMySqlParameters ()
{
  DataBaseServerPtr  defaultParameters = NULL;
  {
    DataBaseServerListPtr servers = this->GetListOfServers (log);
    if  (servers)
    {
      DataBaseServerPtr entryInList = servers->GetDefaultServer ();
      if  (entryInList)
        defaultParameters = new DataBaseServer (*entryInList);
      delete  servers;
    }
  }

  if  (!defaultParameters)
  {
    defaultParameters = new DataBaseServer ();
  }

  return  defaultParameters;
}  /* GetDefaultMySqlParameters */




bool  DataBase::ResultSetLoad (ConstCharStarArray    fieldNames)
{
  resultSetFieldDefs  = NULL;
  resultSetNumRows    = 0;
  resultSetNumFields  = 0;
  resultSetMore       = false;
  resultSet           = NULL;
  resultSetNextRow    = NULL;
  resultSetLengths    = NULL;
  resultSetFieldIndex.clear ();
 
  resultSet = mysql_use_result (conn); /* generate result set */
  if  (resultSet == NULL)
  {
    resultSetMore = false;
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ResultSetLoad    ***ERROR***     occurred when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  false;
  }

  resultSetMore = true;
  resultSetNumRows    = (kkuint32)mysql_num_rows   (resultSet);
  resultSetNumFields  = (kkuint32)mysql_num_fields (resultSet);
  resultSetFieldDefs  = mysql_fetch_fields (resultSet);

  if  (fieldNames)
  {
    ResultSetBuildFieldIndexTable (fieldNames);
  }
  else
  {
    for (kkuint32 x = 0; x < resultSetNumFields;  x++)
      resultSetFieldIndex.push_back (x);
  }

  return  resultSetMore;
}  /* ResultSetLoad */





/** @brief   Use instead of 'ResultSetLoad'; returns all fields plus their names. */
bool  DataBase::ResultSetLoadFieldNames (VectorKKStr&  fieldNames)
{
  resultSetFieldDefs  = NULL;
  resultSetNumRows    = 0;
  resultSetNumFields  = 0;
  resultSetMore       = false;
  resultSet           = NULL;
  resultSetNextRow    = NULL;
  resultSetLengths    = NULL;
  resultSetFieldIndex.clear ();
 
  resultSet = mysql_use_result (conn); /* generate result set */
  if  (resultSet == NULL)
  {
    resultSetMore = false;
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ResultSetLoad    ***ERROR***     occurred when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  false;
  }

  resultSetMore = true;
  resultSetNumRows    = (kkuint32)mysql_num_rows   (resultSet);
  resultSetNumFields  = (kkuint32)mysql_num_fields (resultSet);
  resultSetFieldDefs  = mysql_fetch_fields (resultSet);

  fieldNames.clear ();

  if  (resultSet == NULL)
    return  false;

  resultSetFieldIndex.clear ();
  for  (kkuint32 fieldNum = 0;   fieldNum < resultSetNumFields;  ++fieldNum)
  {
    fieldNames.push_back (resultSetFieldDefs[fieldNum].name);
    resultSetFieldIndex.push_back (fieldNum);
  }

  return  resultSetMore;
}  /* ResultSetLoadFieldNames */



bool   DataBase::ResultSetGetNext (const char**  fieldNames)
{
  kkint32  returnCd = mysql_next_result (conn);
  if  (returnCd == 0)
  {
    resultSetMore = true;
    ResultSetLoad (fieldNames);
  }
  else
  {
    resultSetMore= false;
  }

  return  resultSetMore;
}  /* ResultSetGetNext */




void DataBase::ResulSetFree ()
{
  if  (resultSet)
    mysql_free_result (resultSet);
  resultSet = NULL;
  resultSetFieldDefs = NULL;
  resultSetNumFields = 0;
  resultSetNumRows   = 0;
  resultSetNextRow   = NULL;
  resultSetLengths   = NULL;
  resultSetFieldIndex.clear ();
}


/**
 *@brief  Call this method after you are done processing the results of the last query.  If you do not 
 *      do this the next call may fail.
 */
void  DataBase::ResultSetsClear ()
{
  if  (resultSet)
    ResulSetFree ();

  kkint32 returnCd = mysql_next_result (conn);
  while  (returnCd == 0)
  {
    resultSet = mysql_use_result (conn); /* generate result set */
    if  (resultSet)
    {
      do  
      {
        resultSetNextRow = mysql_fetch_row (resultSet);
      } while  (resultSetNextRow);
      ResulSetFree ();
    }
    returnCd = mysql_next_result (conn);
  }
}  /* ResultSetsClear*/




/**
 *@brief  Returns an array with the indirection index for each field listed in fieldNames.
 *@details
 * Will return an array that has the indirection index for each field in fieldNames.
 * ex   results[2]  specifies which entry in the results row contains the value for 
 * the field name in 'fieldNames[2]'.
 *@param[in]  fieldNames  List of field names to populate indirection matrix with; the last entry needs to contain a NULL pointer to mark end of list.
 */
void  DataBase::ResultSetBuildFieldIndexTable (ConstCharStarArray   fieldNames)
{
  if  ((resultSet == NULL)  ||  (fieldNames == NULL))
    return;

  resultSetFieldIndex.clear ();

  if  (resultSetNumFields < 1)
    return;

  kkint32  fieldNum = 0;

  while  (fieldNames[fieldNum])
  {
    const char* fieldName = fieldNames[fieldNum];
    kkint32 idx = -1;
    for  (kkuint32 x = 0;  x < resultSetNumFields;  x++)
    {
      if  (KKB::STRICMP (resultSetFieldDefs[x].name, fieldName) == 0)
      {
        idx = x;
        break;
      }
    }
    resultSetFieldIndex.push_back (idx);
    fieldNum++;
  }

  return;
}  /* ResultSetBuildFieldIndexTable */



  
bool  DataBase::ResultSetFetchNextRow ()
{
  if  (resultSet == NULL)
  {
    KKStr  errMsg (256);
    errMsg << "DataBase::ResultSetFetchNextRow   'resultSet' == NULL   ***ERROR***" << endl
           << "             Previous SQL Command :" << prevQueryStatement << endl
           << "           LastMySQLError :" << lastMySqlErrorNo   << "\t" << lastMySqlErrorDesc << endl;

    log.Level (-1) << endl << endl << errMsg << endl << endl;
    ErrorLogMsg (errMsg);
    return false;
  }

  resultSetLengths = NULL;
  resultSetNextRow = mysql_fetch_row (resultSet);
  return  resultSetNextRow != NULL;
}



const char*  DataBase::ResultSetGetField (kkuint32 fieldIdx)
{
  if  ((fieldIdx < 0)  ||  (fieldIdx >= (kkuint32)resultSetFieldIndex.size ()))
    return  "";

  kkuint32  rowFieldIndex = resultSetFieldIndex[fieldIdx];
  if  ((rowFieldIndex < 0)  ||  (rowFieldIndex >= resultSetNumFields))
    return  "";

  if  (!resultSetNextRow)
  {
    log.Level (-1) << endl << endl
                   << "DataBase::ResultSetGetField     ***ERROR***" << endl
                   << "                             There is no active ResultSetRow." << endl
                   << endl;
    return  NULL;
  }

  //if  (resultSetNextRow[rowFieldIndex] == NULL)
  //  return  "";
  
  return  resultSetNextRow[rowFieldIndex];
}



KKB::ulong  DataBase::ResultSetGetFieldLen (kkuint32  fieldIdx)
{
  if  (!resultSetLengths)
    resultSetLengths = mysql_fetch_lengths (resultSet);

  if  (!resultSetLengths)
    return  0;

  if  ((fieldIdx < 0)  ||  (fieldIdx >= (kkint32)resultSetFieldIndex.size ()))
    return  0;

  kkuint32  rowFieldIndex = resultSetFieldIndex[fieldIdx];
  if  ((rowFieldIndex < 0)  ||  (rowFieldIndex >= resultSetNumFields))
    return  0;

  return  resultSetLengths[rowFieldIndex];
}



RasterSipperPtr  DataBase::ResultSetGetRasterField (kkuint32 fieldIdx)
{
  const char*  buffer = ResultSetGetField (fieldIdx);
  if  (buffer == NULL)
    return NULL;
  ulong  bufferLen = ResultSetGetFieldLen (fieldIdx);
  
  RasterSipperPtr r = NULL;
  try
  {
    r = RasterSipper::FromCompressor ((uchar*)buffer, bufferLen);
  }
  catch  (const KKException&  e1)
  {
    ErrorLogMsg ("DataBase::ResultSetGetRasterField      Exception Thrown calling   'RasterSipper::FromCompressor' \n" + e1.ToString ());
    r = NULL;
  }
  catch  (...)
  {
    ErrorLogMsg ("DataBase::ResultSetGetRasterField      Exception Thrown calling   'RasterSipper::FromCompressor'" );
    r = NULL;
  }

  return  r;
}



KKStr  DataBase::ResultSetGetKKStrField (kkuint32 fieldIdx)
{
  return  ResultSetGetField (fieldIdx);
}


char  DataBase::ResultSetGetCharField (kkuint32 fieldIdx)
{
  KKStr  s = ResultSetGetField (fieldIdx);
  return  s[0];
}




bool  DataBase::ResultSetGetBool (kkuint32 fieldIdx)
{
  KKStr s = ResultSetGetField (fieldIdx);
  s.Upper ();
  return  ((s == "1")  ||  (s == "TRUE")  ||  (s == "YES")  ||  (s == "T")  ||  (s == "Y"));
}




float  DataBase::ResultSetGetFloatField (kkuint32 fieldIdx)
{
  return  CharStarToFloat (ResultSetGetField (fieldIdx));
}


DateTime  DataBase::ResultSetGetDateTimeField (kkuint32 fieldIdx)
{
  return  DateTimeFromMySqlDateTimeField (ResultSetGetField (fieldIdx));
}


TimeType  DataBase::ResultSetGetTimeField (kkuint32 fieldIdx)
{
  return  TimeFromMySqlDateTimeField (ResultSetGetField (fieldIdx));
}


double  DataBase::ResultSetGetDoubleField (kkuint32 fieldIdx)
{
  return  CharStarToDouble (ResultSetGetField (fieldIdx));
}


KKB::kkint32 DataBase::ResultSetGetIntField (kkuint32 fieldIdx)
{
  return  CharStarToInt32 (ResultSetGetField (fieldIdx));
}


KKB::kkint64  DataBase::ResultSetGetInt64Field    (kkuint32 fieldIdx)
{
  return  CharStarToInt64 (ResultSetGetField (fieldIdx));
}


KKB::kkuint32 DataBase::ResultSetGetUintField (kkuint32 fieldIdx)
{
  return  CharStarToUint32 (ResultSetGetField (fieldIdx));
}


KKB::kkuint16 DataBase::ResultSetGetUint16Field (kkuint32 fieldIdx)
{
  return  CharStarToUint16 (ResultSetGetField (fieldIdx));
}


KKB::ulong   DataBase::ResultSetGetUlongField (kkuint32 fieldIdx)
{
  return  CharStarToUlong (ResultSetGetField (fieldIdx));
}


KKB::kkuint64 DataBase::ResultSetGetUint64Field (kkuint32 fieldIdx)
{
  return  CharStarToUint64 (ResultSetGetField (fieldIdx));
}



KKStr  DataBase::LastErrorDesc ()  const
{
  return  lastMySqlErrorDesc;
}



KKB::DateType  DataBase::DateFromMySqlDateTimeField (KKStr  field)
{
  kkint32  year  = field.ExtractTokenInt ("-");
  kkint32  month = field.ExtractTokenInt ("-");
  kkint32  day   = field.ToInt ();

  if  ((year < 1900)  || (year > 2199)  ||  (month < 1)  ||  (month > 12)  || (day < 1)  || (day > 31))
    return DateType (1900, 1, 1);

  return  DateType (year, month, day);
}  /* DateFromMySqlDateTimeField */





DateTime  DataBase::DateTimeFromMySqlDateTimeField (KKStr  field)
{
  KKStr  yearStr   = field.ExtractToken ("-");
  KKStr  monthStr  = field.ExtractToken ("-");
  KKStr  dayStr    = field.ExtractToken (" ");
  KKStr  hourStr   = field.ExtractToken (":");
  KKStr  minStr    = field.ExtractToken (":");
  
  DateTime result ((kkuint32) yearStr.ToInt (),  (uchar)monthStr.ToInt (), (uchar)dayStr.ToInt (), 
                   (uchar)hourStr.ToInt (),  (uchar)minStr.ToInt (),   (uchar)field.ToInt ());
  return  result;
}  /* DateTimeFromMySqlField */





KKB::TimeType  DataBase::TimeFromMySqlDateTimeField (KKStr field)
{
  KKStr  hourStr = field.ExtractToken (":");
  KKStr  minStr  = field.ExtractToken (":");
  TimeType  result ((uchar)hourStr.ToInt16 (), (uchar)minStr.ToInt16 (), (uchar)field.ToInt16 ());
  return  result;
}



KKStr  DataBase::DateTimeToQuotedStr (const DateTime&  dt)
{
  KKStr  result = "\"" + dt.Date ().YYYY_MM_DD () + " " + dt.Time ().HH_MM_SS () + "\"";
  return  result;
}




KKStr  DateToQuotedStr (const DateType&  d)
{
  KKStr  result = "\"" + d.YYYY_MM_DD () + "\"";
  return  result;
}



KKB::kkint32  DataBase::ReConnect (const KKStr&  funcCall,
                                 const KKStr&  queryStrMsg,
                                 kkint32       numReTries
                                )
{
  // We will attempt to reconnect to the server 
  KKStr  l (256);
  l << "DataBase::ReConnect   Server Connection Lost    FuncCall[" << funcCall << "]" << endl
    << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
    << "LastQueryStr"   << "\t" <<queryStrMsg << endl;
  log.Level (-1) << endl << endl << l << endl << endl;
  ErrorLogMsg (l);

  kkint32  attempt = 1;

  kkint32  returnCd = Connect ();

  while  (returnCd != 0)
  {
    if  ((attempt > numReTries)  &&  (numReTries > 0))
    {
      KKStr  errMsg(256);
      errMsg << "DataBase::ReConnect  " << numReTries << " Attempts at reconnection have failed.";
      log.Level (-1) << endl << errMsg << endl << endl;
      ErrorLogMsg (errMsg);
      return  CR_SERVER_LOST;
    }

    if  (attempt > 10)
      osSleep (10);
    else if  (attempt > 3)
      osSleep (5);
    else
      osSleep (2.0f);

    attempt++;
    {
      KKStr  msg (256);
      msg << "DataBase::ReConnect    Attempt[" << attempt << "]";
      log.Level (-1) << endl << msg << endl << endl;
      ErrorLogMsg (msg);
    }

    returnCd = Connect ();
  }

  {
    KKStr  msg (256);
    msg << "DataBase::ReConnect    Connection reestablished after [" << attempt << "] attempts.";
    log.Level (-1) << endl << msg << endl << endl;
    ErrorLogMsg (msg);
  }

  return  0;
}  /* Reconnect */



void  DataBase::ValidateConnection (const char*  funcCall)
{
  if  (resultSet != NULL)
  {
    // This should not happen; we will add warning to SQL ERROR File.

    KKStr  msg (256);
    msg << "DataBase::ValidateConnection   ***ERROR*** Function[" << funcCall << "]  'resultSet != NULL'   This should never happen." << endl
        << "     Previous Statement[" << prevQueryStatement << "]";

    log.Level (-1) << endl << msg << endl << endl;

    ErrorLogMsg (msg);

    // Since we do not know the status of the Previous ResultSet we will set it to NULL.
    resultSet = NULL;
  }

  if  (!conn)
  {
    KKStr  msg (256);
    msg << "DataBase::ValidateConnection   Function[" << funcCall << "]  Connection was closed.";
    log.Level (-1) << endl << msg << endl << endl;
    ErrorLogMsg (msg);
    Connect ();
  }
}  /* ValidateConnectiuon */



KKB::kkint32  DataBase::QueryStatement (const char* queryStr,
                                        kkint32     queryStrLen
                                       )
{
  ValidateConnection ("QueryStatement");

  kkint32  outOfSeqLoopCount = 0;

  kkint32  attempt = 1;

  while  (true)
  {
    kkint32 returnCd = mysql_real_query (conn, queryStr, queryStrLen);
    if  (returnCd == 0)
      break;

    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    if  ((lastMySqlErrorNo == CR_SERVER_GONE_ERROR)  ||  (lastMySqlErrorNo == CR_SERVER_LOST))
    {
      // We will attempt to reconnect to the server 
      KKStr  queryStrMsg;
      if  (queryStrLen > 256)  queryStrMsg.Append (queryStr, 255);
      else  queryStrMsg = queryStr;

      returnCd = ReConnect ("QueryStatement", queryStrMsg, 30);
      if  (returnCd != 0)
        return returnCd;

      continue;
    }

    else if  (lastMySqlErrorNo == 2014)
    {
      // Calls are out of sequence;  

      KKStr  curStatement;
      if  (queryStrLen > 512)  curStatement.Append (queryStr, 512);
      else  curStatement.Append (queryStr, queryStrLen);

      KKStr  msg (512);
      msg << "DataBase::QueryStatement    MySQL calls out of sequence." << endl
          << "          lastMySqlErrorDesc[" << lastMySqlErrorDesc << "]" << endl
          << "          lastMySqlErrorNo  [" << lastMySqlErrorNo   << "]" << endl
          << endl;

      log.Level (-1) << endl << endl << endl  << msg << endl << endl;
      ErrorLogMsg (msg);

      if  (outOfSeqLoopCount == 0)
      {
        ResultSetsClear ();
      }

      else if  (outOfSeqLoopCount == 1)
      {
        // Clearing result sets did not work so lets try closing and opening connection
        returnCd = Connect ();
      }

      else if  (outOfSeqLoopCount > 1)
      {
        return  lastMySqlErrorNo;
      }

      outOfSeqLoopCount++;
    }

    duplicateKey = ((lastMySqlErrorNo == 1022)  ||  (lastMySqlErrorNo == 1062));

    if  (returnCd != 0)
    {
      valid = false;

      KKStr  msg (255);
      msg << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl << queryStr;
      ErrorLogMsg (msg);
      return  lastMySqlErrorNo;
    }
  }  /* End of while(true)  loop */

  valid = true;

  prevQueryStatement = "";
  if  (queryStrLen > 512)
    prevQueryStatement.Append (queryStr, 512);
  else
    prevQueryStatement.Append (queryStr, queryStrLen);

  return 0;
}  /* QueryStatement */




KKB::kkint32  DataBase::QueryStatement2 (const char*  queryStr,
                                       kkint32      queryStrLen
                                      )
{
  ValidateConnection ("QueryStatement2");

  kkint32  outOfSeqLoopCount = 0;

  kkint32 returnCd = mysql_real_query (conn, queryStr, queryStrLen);
  if  (returnCd != 0)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    if  ((lastMySqlErrorNo == CR_SERVER_GONE_ERROR)  ||  (lastMySqlErrorNo == CR_SERVER_LOST))
    {
      KKStr  msg (255);
      msg << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl << queryStr << endl;
      ErrorLogMsg (msg);
      return  returnCd;
    }
    else
    {
      duplicateKey = ((lastMySqlErrorNo == 1022)  ||  (lastMySqlErrorNo == 1062));
      if  (returnCd != 0)
      {
        valid = false;

        KKStr  msg (255);
        msg << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl << queryStr << endl;
        ErrorLogMsg (msg);

        return  lastMySqlErrorNo;
      }
    }
  }  /* End of while(true)  loop */

  valid = true;

  prevQueryStatement = "";
  if  (queryStrLen > 512)
    prevQueryStatement.Append (queryStr, 512);
  else
    prevQueryStatement.Append (queryStr, queryStrLen);

  return 0;
}  /* QueryStatement2 */



KKB::kkint32  DataBase::QueryStatement (const KKStr& statement)
{
  ValidateConnection ("QueryStatement");

  kkint32  outOfSeqLoopCount = 0;
  while  (true)
  {
    kkint32 returnCd = mysql_real_query (conn, statement.Str (), statement.Len ());
    if  (returnCd == 0)
      break;

    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    if  ((lastMySqlErrorNo == CR_SERVER_GONE_ERROR)  ||  (lastMySqlErrorNo == CR_SERVER_LOST))
    {
      returnCd = ReConnect ("QueryStatement", "", 30);
      if  (returnCd != 0)
        return returnCd;
      continue;
    }

    if  (lastMySqlErrorNo == 2014)
    {
      // Calls are out of sequence;  
      KKStr l(512);
      l << "MySQL Calls out of sequence" << "\t" << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << "Previous Statement" << "\t" << prevQueryStatement << endl 
        << "Current  Statement" << "\t" << statement          << endl 
        << "Loop Count"         << "\t" << outOfSeqLoopCount  << endl;
      log.Level (-1) << endl << l << endl;
      ErrorLogMsg (l);

      if  (outOfSeqLoopCount == 0)
      {
        ResultSetsClear ();
      }

      else if  (outOfSeqLoopCount == 1)
      {
        // Clearing result sets did not work so lets try closing and opening connection
        returnCd = Connect ();
      }

      else if  (outOfSeqLoopCount > 1)
      {
        return  lastMySqlErrorNo;
      }

      outOfSeqLoopCount++;
    }

    duplicateKey = ((lastMySqlErrorNo == 1022)  ||  (lastMySqlErrorNo == 1062));

    if  (returnCd != 0)
    {
      valid = false;
      KKStr  l (512);
      l << "DataBase::QueryStatement" << "\t"
        << "lastMySqlErrorNo"    << "\t" << lastMySqlErrorNo << "\t" 
        << "mysql_error"         << "\t" << lastMySqlErrorDesc << endl
        << "Current Statement"   << "\t" << statement          << endl
        << "Previous Statement"  << "\t" << prevQueryStatement << endl;
      ErrorLogMsg (l);

      return  lastMySqlErrorNo;
    }
  }

  valid = true;

  prevQueryStatement = statement;

  return 0;
}  /* QueryStatement */





KKStrMatrixPtr  DataBase::QueryStatementKKStr (const KKStr&        statement,
                                               ConstCharStarArray  colsToReturn
                                              )
{
  kkint32  returnCd = QueryStatement (statement.Str (), statement.Len ());
  if  (returnCd != 0)
    return NULL;

  if  (!ResultSetLoad (colsToReturn))
    return NULL;

  kkuint32 numCols = resultSetNumFields;
  kkuint32 col = 0;

  KKStrMatrixPtr  results = new KKStrMatrix (numCols);
  while  (ResultSetFetchNextRow ())
  {
    KKStrListPtr  newResultRow = new KKStrList (true);
    for  (col = 0;  col < numCols;  col++)
    {
      newResultRow->PushOnBack (new KKStr (ResultSetGetField (col)));
    }
    results->AddRow (newResultRow);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* QueryStatement */




KKStrMatrixPtr  DataBase::QueryStatementReturnAllColumns (const char*   statement,
                                                          kkint32       statementLen,
                                                          VectorKKStr&  columnsReturned
                                                         )
{
  kkint32  returnCd = QueryStatement (statement, statementLen);
  if  (returnCd != 0)
    return NULL;

  columnsReturned.clear ();
  if  (!ResultSetLoadFieldNames (columnsReturned))
    return NULL;

  kkuint32 numCols = resultSetNumFields;
  kkuint32 col = 0;

  KKStrMatrixPtr  results = new KKStrMatrix (numCols);
  while  (ResultSetFetchNextRow ())
  {
    KKStrListPtr  newResultRow = new KKStrList (true);
    for  (col = 0;  col < numCols;  col++)
    {
      newResultRow->PushOnBack (new KKStr (ResultSetGetField (col)));
    }
    results->AddRow (newResultRow);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* QueryStatementReturnAllColumns */





//********************************************************************************************
//*                                 FeatureData   Routines                                   *
//********************************************************************************************

void  DataBase::FeatureDataCreateTable ()
{
  KKStr  createStatement;

  createStatement << "create table  FeatureData"         << endl
                  << "("                                 << endl
                  << "  ImageFileName    varchar(32)   not null,"   << endl
                  << "  SipperFileName   char (14)     not null,"   << endl;

  kkint32  fieldNum = 0;
  for  (fieldNum = 0;  fieldNum < FeatureFileIOPices::PlanktonMaxNumOfFields ();  fieldNum++)
  {
    KKStr  fieldName = FeatureFileIOPices::PlanktonFieldName (fieldNum);
    if  (!fieldName.Empty ())
      createStatement << "  " << FeatureFileIOPices::PlanktonFieldName (fieldNum) << "  real  null," << endl;
  }

  createStatement << endl
                  << "  Primary Key       (ImageFileName   ASC),"  << endl
                  << "  Key SipperFileKey (SipperFileName  ASC)"  << endl
                  << ")";

  kkint32  returnCd = QueryStatement (createStatement);

  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl
                   << "DataBase::CreateFeatureDataTable    ***ERROR***" << endl
                   << endl
                   << "         Error[" << lastMySqlErrorDesc << "] occurred trying to create FeatureData table." << endl
                   << endl;
    valid = false;
    return;
  }

  ResultSetsClear ();
}  /* CreateFeatureDataTable */




void  DataBase::FeatureDataInsertRow (const KKStr&          _sipperFileName,
                                      const ImageFeatures&  example
                                     )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("FeatureDataInsertRow");
    return;
  }

  KKStr  sipperFileName = osGetRootName (_sipperFileName);
  KKStr  imageFileName  = osGetRootName (example.ExampleFileName ());

  KKStr  insertStatement (3000);
  insertStatement << "call  FeatureDataInsert(" << imageFileName.QuotedStr () << ", " << sipperFileName.QuotedStr ();
  for  (kkint32 fieldNum = 0;  fieldNum < example.NumOfFeatures ();  fieldNum++)
  {
    double f = example.FeatureData (fieldNum);
    if  ((IsNaN (f))  ||  (!IsFinite (f)))
      insertStatement << ", " << "NULL";
    else
      insertStatement << ", " << example.FeatureData (fieldNum);
  }
  insertStatement << ")";

  kkint32  returnCd = QueryStatement (insertStatement);
  ResultSetsClear ();
}  /* FeatureDataInsertRow */




KKB::kkint32  LookUpFeatureDataField (const KKStr& fieldName)
{
  KKB::kkint32  fieldNum = 0;
  for  (fieldNum = 0;  fieldNum < FeatureFileIOPices::PlanktonMaxNumOfFields ();  fieldNum++)
  {
    if  (fieldName == FeatureFileIOPices::PlanktonFieldName (fieldNum))
      return  fieldNum;
  }
  return -1;
}  /* LookUpFeatureDataField */




char**   DataBase::featureDataFieldNames = NULL;



ConstCharStarArray  DataBase::GetFeatureDataFieldNames ()
{
  if  (featureDataFieldNames != NULL)
    return  featureDataFieldNames;

  CreateBlocker ();
  blocker->StartBlock ();

  if  (featureDataFieldNames != NULL)
  {
    // Looks like another thread must have been in the middle of creating this list.
    blocker->EndBlock ();
    return  featureDataFieldNames;
  }

  char**  fieldNames = NULL;
  kkint32  fieldNamesSize = 17 + FeatureFileIOPices::PlanktonMaxNumOfFields ();
  fieldNames = new char*[fieldNamesSize];
  fieldNames[ 0] = STRDUP ("ImageId");
  fieldNames[ 1] = STRDUP ("ImageFileName");
  fieldNames[ 2] = STRDUP ("TopLeftRow");
  fieldNames[ 3] = STRDUP ("TopLeftCol");
  fieldNames[ 4] = STRDUP ("PixelCount");
  fieldNames[ 5] = STRDUP ("CentroidRow");
  fieldNames[ 6] = STRDUP ("CentroidCol");
  fieldNames[ 7] = STRDUP ("Class1Id");
  fieldNames[ 8] = STRDUP ("Class1Name");
  fieldNames[ 9] = STRDUP ("Class1Prob");
  fieldNames[10] = STRDUP ("Class2Prob");
  fieldNames[11] = STRDUP ("ClassValidatedId");
  fieldNames[12] = STRDUP ("ClassNameValidated");
  fieldNames[13] = STRDUP ("ImagesDepth");
  fieldNames[14] = STRDUP ("CtdDateTime");
  fieldNames[15] = STRDUP ("AreaMM");
   
  kkint32  fieldNum = 0;
  for  (fieldNum = 0;  fieldNum < FeatureFileIOPices::PlanktonMaxNumOfFields ();  fieldNum++)
    fieldNames[16 + fieldNum] = STRDUP (FeatureFileIOPices::PlanktonFieldName (fieldNum).Str ());
  fieldNames[16 + fieldNum] = NULL;

  featureDataFieldNames = fieldNames;
  blocker->EndBlock ();
  return  featureDataFieldNames;
}  /* GetFeatureDataFieldNames */





ImageFeaturesListPtr  DataBase::FeatureDataProcessResults ()
{
  ConstCharStarArray   fieldNames = GetFeatureDataFieldNames ();

  if  (!featureFileDesc)
    featureFileDesc = FeatureFileIOPices::NewPlanktonFile ();

  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  ImageFeaturesListPtr  results = new ImageFeaturesList (featureFileDesc, true, resultSetNumRows);

  kkuint32  topLeftRow  = 0;
  kkuint32  topLeftCol  = 0;

  KKStr    class1Name (64);
  kkint32  class1Id = -1;
  KKStr    classNameValidated (64);
  kkint32  classValidatedId = -1;
  float    class1Prob = 0.0f;
  float    class2Prob = 0.0f;

  float  imagesDepth = 0.0f;

  KKB::DateTime  ctdDateTime;

  while  (ResultSetFetchNextRow ())
  {
    ImageFeaturesPtr  i = new ImageFeatures (featureFileDesc->NumOfFields ());

    i->ExampleFileName (ResultSetGetField     (1));
    topLeftRow = ResultSetGetUintField        (2);
    topLeftCol = ResultSetGetUintField        (3);
    i->OrigSize (ResultSetGetFloatField       (4));
    i->CentroidRow (ResultSetGetFloatField    (5));
    i->CentroidCol (ResultSetGetFloatField    (6));
    class1Id   = ResultSetGetIntField         (7);
    class1Name = ResultSetGetField            (8);
    class1Prob = ResultSetGetFloatField       (9);
    class2Prob = ResultSetGetFloatField       (10);
    classValidatedId = ResultSetGetIntField   (11);
    classNameValidated = ResultSetGetField    (12);
    imagesDepth = ResultSetGetFloatField      (13);
    i->CtdDateTime (ResultSetGetDateTimeField (14));
    i->AreaMMSquare (ResultSetGetFloatField   (15));

    for  (kkint32 fieldNum = 0;  fieldNum < FeatureFileIOPices::PlanktonMaxNumOfFields ();  fieldNum++)
      i->FeatureData (fieldNum, ResultSetGetFloatField (16 + fieldNum));

    if  (class1Id == 0)
      class1Id = -1;

    if  (classValidatedId == 0)
      classValidatedId = -1;

    if  (!class1Name.Empty ())
      i->PredictedClass (MLClass::CreateNewMLClass (class1Name, class1Id));

    if  (!classNameValidated.Empty ())
    {
      i->MLClass (MLClass::CreateNewMLClass (classNameValidated, classValidatedId));
      i->Validated (true);
    }
    else
    {
      if  (!class1Name.Empty ())
        i->MLClass (MLClass::CreateNewMLClass (class1Name, class1Id));
      else
        i->MLClass (MLClass::GetUnKnownClassStatic ());
    }

    i->Probability (class1Prob);
    i->BreakTie (fabs (class1Prob - class2Prob));
    i->TrainWeight (1.0f);

    i->SfCentroidCol (topLeftCol + i->CentroidCol ());
    i->SfCentroidRow (topLeftRow + i->CentroidRow ());

    results->PushOnBack (i);
  }

  ResulSetFree ();

  return  results;
}  /* FeatureDataProcessResults */




ImageFeaturesPtr    DataBase::FeatureDataRecLoad (const KKStr&  imageFileName)
{
  KKStr  rootName = osGetRootName (imageFileName);

  KKStr  selectStr (256);

  selectStr << "call  FeatureDataLoadByImageFileName2(" << rootName.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl 
                   << "DataBase::FeatureDataRecLoad   ***ERROR***" << endl
                   << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl
                   << endl;
    return NULL;
  }

  ImageFeaturesListPtr  results = FeatureDataProcessResults ();  // true = Refresh Instrument Data from Sipper Files.
  ResultSetsClear ();
  if  (results == NULL)  
    return NULL;

  if  (results->QueueSize () < 1)
  {
    delete  results;  results = NULL;
    return NULL;
  }

  ImageFeaturesPtr  result = results->PopFromBack ();
  delete results;  results = NULL;

  if  (result)
  {
    // To make sure that we have valid CTD data the search must start no sooner than 30K scan 
    // lines from the beginning of the sipper file.
    KKStr  SipperFileName = InstrumentDataFileManager::SipperFileRootNameFromSipperImageFileName (result->ExampleFileName ());
    InstrumentDataPtr  id = this->InstrumentDataGetByScanLine (SipperFileName, Max ((ulong)30000, (ulong)(result->SfCentroidRow ())));
    if  (id  &&  id->Depth () > 0.0)
    {
      if  (result->Depth () == 0.0)
      {
        result->Depth         (id->Depth        ());
        result->Fluorescence  (id->Fluorescence ());       
        result->Oxygen        (id->Oxygen       ());
        result->Salinity      (id->Salinity     ());
      }

      if  (result->FlowRate1 () == 0.0)
      {
        result->FlowRate1     (id->FlowRate1    ());
        result->FlowRate2     (id->FlowRate2    ());
      }

      if  (result->Latitude () == 0.0)
      {
        result->Latitude      (id->Latitude     ());
        result->Longitude     (id->Longitude    ());
      }

      delete  id;  id = NULL;
    }
  }

  ResultSetsClear ();
  return  result;
}  /* FeatureDataRecLoad */




/// Retrieves FeatureData for a given 'DataBaseImage' object.  It will also refresh the Instrument data fields
/// of the 'ImageFeatures' objects from the 'InstruentData' table
ImageFeaturesPtr   DataBase::FeatureDataRecLoad (DataBaseImagePtr  image)
{
  return FeatureDataRecLoad (image->ImageFileName ());
}  /* FeatureDataRecLoad */





/**
 *@brief  Loads all Images with related Feature data for the specified 'SipperFile'
 *@param[in] sipperFileName            Sipper file to retrieve feature data for.
 *@param[in] mlClass                   You are only interested in feature data for a specific class;  whether predicted or validated class depends on 'classKeyToUse'.
 *@param[in] classKeyToUse             Indicates which ClassId field to use for selecting feature data by class;  'V' = 'Images.ClassValidatedId' otherwise 'Images.Class1Id'.
 *@param[in] reExtractInstrumentData:  True indicates to re-extract feature data from the original SipperFile and update InmstrumentData tables.
 *@param[in] cancelFlag                Flag will be monitored;  if turns true the method will return at 1st chance.
 */
ImageFeaturesListPtr  DataBase::FeatureDataGetOneSipperFile (const KKStr&     sipperFileName,
                                                             MLClassPtr       mlClass,
                                                             char             classKeyToUse,
                                                             bool             reExtractInstrumentData,
                                                             bool&            cancelFlag
                                                            )
{
  SipperFilePtr  sipperFile = NULL;
  InstrumentDataListPtr  instrumentData = NULL;

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);

  if  (reExtractInstrumentData)
  {
    // We are making sure that the InstrumentData for this sipper file is loaded into memory
    SipperFilePtr  sipperFile = SipperFileRecLoad (osGetRootName (sipperFileName));
    instrumentData = InstrumentDataFileManager::ReExtractInstrumentDataForSipperFile    // We will not own 'instrumentData' it 
                         (sipperFileName, sipperFile, cancelFlag, log);                 // is maintained by 'InsrumentDataFileManager'
    if  (instrumentData)
      InstrumentDataSaveListForOneSipperFile (sipperFileName, *instrumentData, cancelFlag);
  }

  KKStr  selectStr (256);
  if  (mlClass)
    selectStr << "call FeatureDataGetOneSipperFileClassName(" << osGetRootName (sipperFileName).QuotedStr () << "," 
                                                              << mlClass->Name ().QuotedStr ()            << ","
                                                              << classKeyToUseStr.QuotedStr ()
                                                              << ")";
  else
    selectStr << "call FeatureDataGetOneSipperFile(" << osGetRootName (sipperFileName).QuotedStr () << ", " 
                                                     << classKeyToUseStr.QuotedStr () 
                                                     << ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  ImageFeaturesListPtr results = FeatureDataProcessResults ();
  if  (results  &&  instrumentData)
  {
    ImageFeaturesList::iterator  idx;
    for  (idx = results->begin ();  (idx != results->end ())  &&  (!cancelFlag);  idx++)
    {
      ImageFeaturesPtr  i = *idx;
      if  ((i->Depth () == 0.0)  ||  (i->FlowRate1 () == 0.0)  ||  (i->Latitude () == 0.0))
      {
        InstrumentDataPtr  id = instrumentData->SearchForNearestScanLine (Max ((ulong)i->SfCentroidRow (), (ulong)30000));
        if  (id)
        {
          if  (i->Depth () == 0.0)
          {
            i->Depth         (id->Depth        ());
            i->Fluorescence  (id->Fluorescence ());       
            i->Oxygen        (id->Oxygen       ());
            i->Salinity      (id->Salinity     ());
          }

          if  (i->FlowRate1 () == 0.0)
          {
            i->FlowRate1     (id->FlowRate1    ());
            i->FlowRate2     (id->FlowRate2    ());
          }

          if  (i->Latitude () == 0.0)
          {
            i->Latitude      (id->Latitude     ());
            i->Longitude     (id->Longitude    ());
          }
        }
      }
    }
  }
  
  ResultSetsClear ();

  if  (cancelFlag)
  {
    delete  results;
    results = NULL;
  }

  return  results;
}  /* FeatureDataGetOneSipperFile */






ImageFeaturesListPtr  DataBase::FeatureDataForImageGroup (const DataBaseImageGroupPtr  imageGroup,
                                                          MLClassPtr                   mlClass,
                                                          char                         classKeyToUse,
                                                          const bool&                  cancelFlag
                                                         )
{
  if  (imageGroup == NULL)
  {
    log.Level (10) << endl << endl
                   << "DataBase::FeatureDataForImageGroup   ***WARNING***    (imageGroup == NULL)"  << endl
                   << endl;
    return NULL;
  }

  log.Level (20) << "DataBase::FeatureDataForImageGroup   ImageGroup[" << imageGroup->Name () << "]" << endl;

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);

  InstrumentDataListPtr  instrumentData = NULL;

  KKStr  selectStr (256);

  if  (mlClass)
    selectStr << "call FeatureDataGetOneImageGroupClassName(" << imageGroup->ImageGroupId ()      << "," 
                                                              << mlClass->Name ().QuotedStr () << "," 
                                                              << classKeyToUseStr.QuotedStr ()
                                                              << ")";
  else
    selectStr << "call FeatureDataGetOneImageGroup(" << imageGroup->ImageGroupId ()   << ","
                                                     << classKeyToUseStr.QuotedStr ()
                                                     << ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  ImageFeaturesListPtr results = FeatureDataProcessResults ();
  ResultSetsClear ();

  return  results;
}  /* FeatureDataForImageGroup */




void   DataBase::FeatureDataUpdate (DataBaseImagePtr  dataBaseImage,
                                    ImageFeaturesPtr  example
                                   )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("FeatureDataUpdate");
    return;
  }

  KKStr  updateStr (3072);

  updateStr << "update FeatureData set ";
  for  (kkint32 featureNum = 0;  featureNum < example->NumOfFeatures ();  featureNum++)
  {
    if  (featureNum > 0)
      updateStr << ", ";
    updateStr << FeatureFileIOPices::PlanktonFieldName (featureNum) << " = " << example->FeatureData (featureNum);
  }

  if  (dataBaseImage != NULL)
  {
    updateStr << "  where ImageId = " << dataBaseImage->ImageId ();
  }
  else
  {
    KKStr  rootImageName =  KKB::osGetRootName (example->ExampleFileName ());
    updateStr << "  where ImageId = (select  ImageId from Images where ImageFileName = " << rootImageName.QuotedStr () << ")";
  }

  kkint32  returnCd = QueryStatement (updateStr);
}  /* FeatureDataUpdate */




void   DataBase::FeatureDataUpdateInstrumentDataFields (const KKStr&       imageFileName,
                                                        InstrumentDataPtr  instumentData
                                                       )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("FeatureDataUpdateInstrumentDataFields");
    return;
  }

  KKStr  updateStr (1024);

  updateStr << "call FeatureDataUpdateInstrumentDataFields("
            << imageFileName.QuotedStr  () << ", "
            << instumentData->FlowRate1 () << ", "
            << instumentData->FlowRate2 () << ", "
            << instumentData->Depth     () << ", "  
            << instumentData->Salinity  () << ", "
            << instumentData->Oxygen    () << ", "
            << instumentData->Fluorescence ()
            << ")";
  kkint32  returnCd = QueryStatement (updateStr);

  ResultSetsClear ();
}  /* FeatureDataUpdateInstrumentDataFields */



vector<KKB::ulong>*  DataBase::FeatureDataGetScanLinesPerMeterProfile (const KKStr&  sipperFileName)
{
  KKStr  selectStr = "Call FeatureDataGetScanLinesPerMeterProfile (" + osGetRootName (sipperFileName).QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (selectStr);

  char const *  fieldNames[] = {"StartDepth", "ScanLineCount", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return NULL;

  vector<ulong>* results = new vector<ulong> (0);

  while  (ResultSetFetchNextRow ())
  {
    kkuint32 startDepth    = ResultSetGetUintField (0);
    ulong  scanLineCount = ResultSetGetUlongField (1);

    while  (results->size () < startDepth)
      results->push_back (0);


    (*results)[startDepth] += scanLineCount;
  }
    
  return  results;
}  /* FeatureDataGetScanLinesPerMeterProfile*/







//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*                                   GpsData Routines                                      *
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

GPSDataPointListPtr   DataBase::GpsDataProcessResults ()
{
  char const *const   fieldList[] = {"CruiseName", "UtcDateTime", "Latitude", "Longitude", "CourseOverGround", "SpeedOverGround", NULL};
  ResultSetLoad (fieldList);
  if  (!resultSetMore)
    return  NULL;

  GPSDataPointListPtr  results = new GPSDataPointList (true);

  KKStr     cruiseName;
  DateTime  utcDateTime;
  double    latitude;
  double    longitude;
  float     courseOverGround;
  float     speedOverGround;

  while  (ResultSetFetchNextRow ())
  {
    cruiseName       = ResultSetGetField         (0);
    utcDateTime      = ResultSetGetDateTimeField (1);
    latitude         = ResultSetGetDoubleField   (2);
    longitude        = ResultSetGetDoubleField   (3);
    courseOverGround = ResultSetGetFloatField    (4);
    speedOverGround  = ResultSetGetFloatField    (5);

    GPSDataPointPtr  gpsData = new GPSDataPoint (utcDateTime, latitude, longitude, courseOverGround, speedOverGround);
    results->PushOnBack (gpsData);
  }

  ResulSetFree ();

  return  results;
}  /* GpsDataProcessResults */



void    DataBase::GpsDataInsert (const KKStr&         cruiseName,
                                 const GPSDataPoint&  gpsData
                                )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("GpsDataInsert");
    return;
  }

  KKStr  insertStr = "call GpsDataInsert(" + cruiseName.QuotedStr ( )                                   + ", " +
                                             gpsData.GpsUtcTime ().YYYY_MM_DD_HH_MM_SS ().QuotedStr () + ", " +
                                             StrFormatDouble (gpsData.Latitude  (), "###0.0000000")     + ", " + 
                                             StrFormatDouble (gpsData.Longitude (), "###0.0000000")     + ", " +
                                             StrFormatDouble (gpsData.CourseOverGround (), "##0.0000")  + ", " +
                                             StrFormatDouble (gpsData.SpeedOverGround (), "##0.0000")   +
                                        ")";
  
  kkint32  returnCd = QueryStatement (insertStr);
  bool  successful = (returnCd == 0);

  ResultSetsClear ();
}  /* GpsDataInsert */




GPSDataPointListPtr   DataBase::GpsDataQuery (const KKStr&     cruiseName,
                                              const DateTime&  utcDateTimeStart,
                                              const DateTime&  utcDateTimeEnd
                                             )
{
  GPSDataPointListPtr   results = NULL;

  KKStr  queryStr = "call GpsDataQuery(" + cruiseName.QuotedStr ( )                             + ", " +
                                           utcDateTimeStart.YYYY_MM_DD_HH_MM_SS ().QuotedStr () + ", " +
                                           utcDateTimeEnd.YYYY_MM_DD_HH_MM_SS ().QuotedStr ()   +
                                        ")";

  kkint32  returnCd = QueryStatement (queryStr);
  if  (returnCd == 0)
    results = GpsDataProcessResults ();

  return results;
}  /* GpsDataQuery*/




GPSDataPointListPtr   DataBase::GpsDataQueryByIntervals (const KKStr&     cruiseName,
                                                         const DateTime&  utcDateTimeStart,
                                                         const DateTime&  utcDateTimeEnd,
                                                         kkint32          timeInterval
                                                        )
{
  GPSDataPointListPtr   results = NULL;

  KKStr  queryStr = "call GpsDataQueryByIntervals(" + cruiseName.QuotedStr ( )                             + ", " +
                                                      utcDateTimeStart.YYYY_MM_DD_HH_MM_SS ().QuotedStr () + ", " +
                                                      utcDateTimeEnd.YYYY_MM_DD_HH_MM_SS ().QuotedStr ()   + ", " + 
                                                      StrFormatDouble (timeInterval, "###0")              +
                                                ")";

  kkint32  returnCd = QueryStatement (queryStr);
  if  (returnCd == 0)
    results = GpsDataProcessResults ();

  return results;
}  /* GpsDataQuery*/







void  DataBase::GpsDataDelete (const KKStr&     cruiseName,
                               const DateTime&  utcDateTimeStart,
                               const DateTime&  utcDateTimeEnd
                              )
{
  KKStr  queryStr = "call GpsDataDelete(" + cruiseName.QuotedStr ( )                             + ", " +
                                            utcDateTimeStart.YYYY_MM_DD_HH_MM_SS ().QuotedStr () + ", " +
                                            utcDateTimeEnd.YYYY_MM_DD_HH_MM_SS ().QuotedStr   () + ", " +
                                         ")";
  
  kkint32  returnCd = QueryStatement (queryStr);
  ResultSetsClear ();
  return;
}  /* GpsDataDelete */





//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*                                   MLClass Routines                                      *
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************

MLClassListPtr   DataBase::MLClassProcessResults ()
{
  char const * const  fieldNames[] = {"ClassId", "ClassName", "ParentId", "Description", "ParentName", "Mandatory", "Summarize", NULL};

  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  MLClassListPtr  results = new MLClassList ();

  kkint32  classId = -1;
  KKStr  className (64);
  kkint32  parentId = -1;
  KKStr  description = "";
  KKStr  parentName = "";
  bool   mandatory = false;
  bool   summarize = false;

  while  (ResultSetFetchNextRow ())
  {
    classId     = ResultSetGetIntField (0);
    className   = ResultSetGetField    (1);
    parentId    = ResultSetGetIntField (2);
    description = ResultSetGetField    (3);
    parentName  = ResultSetGetField    (4);
    mandatory   = ResultSetGetBool     (5);
    summarize   = ResultSetGetBool     (6);

    parentName.TrimLeft ();
    parentName.TrimRight ();

    MLClassPtr  ic = MLClass::CreateNewMLClass (className, classId);
    ic->Description (description);
    ic->Mandatory (mandatory);
    ic->Summarize (summarize);
    if  (!parentName.Empty ())
      ic->Parent (MLClass::CreateNewMLClass (parentName, parentId));
    else if  (parentId >= 0)
      ic->Parent (MLClass::GetByClassId (parentId));
      
    results->PushOnBack (ic);
  }

  ResulSetFree ();

  return  results;
}  /* MLClassProcessResults */




MLClassPtr       DataBase::MLClassLoad (const KKStr&  className)
{
  KKStr  selectStr (128);

  selectStr << "call ClassesRetrieveByName(" << className.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl << "DataBase::MLClassLoad     ***ERROR***" << endl << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl << endl;
    return NULL;
  }

  MLClassPtr       result = NULL;

  MLClassListPtr classes = MLClassProcessResults ();
  if  ((classes != NULL)  &&  (classes->size () > 0))
  {
    result = classes->BackOfQueue ();
  }

  ResultSetsClear ();

  delete  classes;
  classes = NULL;
  return result;
}  /* MLClassLoad */




MLClassListPtr  DataBase::MLClassLoadList ()
{
  KKStr  selectStr = "call  ClassesRetrieveAll()";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl << "DataBase::MLClassLoadList     ***ERROR***" << endl << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl << endl;
    return NULL;
  }

  char const *  fieldNames[] = {"ClassId", "ClassName", "ParentId", "ParentName", "Description", "Mandatory", "Summarize", NULL};

  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  // Since we are reloading ALL classes we will initialize the parents of all classes to 'NULL'.  The
  // relationship will be re-established as they are reloaded from the database. This will help prevent
  // corruptions when we switch between MySQL servers.

  
  MLClass::ResetAllParentsToAllClasses ();

  MLClassListPtr  classes = new MLClassList ();

  vector<MLClassPtr>   classesWithParents;
  vector<KKStr>        classesWithParentsNames;

  while  (ResultSetFetchNextRow ())
  {
    kkint32  classId     = ResultSetGetIntField (0);
    KKStr    className   = ResultSetGetField    (1);
    kkint32  parentId    = ResultSetGetIntField (2);
    KKStr    parentName  = ResultSetGetField    (3);
    KKStr    description = ResultSetGetField    (4);
    bool     mandatory   = ResultSetGetBool     (5);
    bool     summarize   = ResultSetGetBool     (6);

    if  (parentName.EqualIgnoreCase (className))
      parentName = "";

    if  (!className.Empty ())
    {
      MLClassPtr  c = MLClass::CreateNewMLClass (className, classId);
      c->Description (description);
      classes->PushOnBack (c);
      c->StoredOnDataBase (true);
      c->Mandatory (mandatory);
      c->Summarize (summarize);

      if  (!parentName.Empty ())
      {
        classesWithParents.push_back (c);
        classesWithParentsNames.push_back (parentName);
      }
    }
  }

  ResulSetFree ();

  for  (kkuint32 x = 0;  x < classesWithParents.size ();  x++)
  {
    MLClassPtr      parent = classes->LookUpByName (classesWithParentsNames[x]);
    if  (parent)
    {
      if  (parent->IsAnAncestor (classesWithParents[x]))
      {
        log.Level (-1) << endl << endl << "DataBase::LoadSipperFileRec    ***ERROR***     occurred when retrieving result set." << endl << endl
                       << "    There is a Circular Parent Assignment" << endl 
                       << "    Class[" << classesWithParents[x]->Name () << "]   Parent[" << parent->Name () << "]" << endl
                       << endl;
        // We can not make this parent assignment;  otherwise we will end up crating a circular data structure.
      }
      else
      {
        classesWithParents[x]->Parent (parent);
      }
    }
  }

  ResultSetsClear ();

  return  classes;
}  /* MLClassLoadList */



MLClassListPtr  DataBase::MLClassLoadChildren (const KKStr&  className)
{
  KKStr  selectStr (128);

  selectStr << "call MLClassLoadChildren(" << className.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl << "DataBase::MLClassLoadChildren     ***ERROR***" << endl << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl << endl;
    return NULL;
  }

  MLClassListPtr temp = MLClassProcessResults ();
  ResultSetsClear ();

  MLClassListPtr results = new MLClassList (*temp);
  delete temp;
  temp = NULL;
  return results;
}  /* MLClassLoadChildren */





void  DataBase::MLClassInsert (MLClass&  mlClass,
                               bool&     successful
                              )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("MLClassInsert");
    return;
  }

  KKStr  mandatoryStr = "N";
  if  (mlClass.Mandatory ())
    mandatoryStr = "Y";

  KKStr  summarizeStr = "N";
  if  (mlClass.Summarize ())
    summarizeStr = "Y";

  KKStr  insertStr = "call MLClassInsert(" + 
                     mlClass.Name ().QuotedStr ()         + ", " + 
                     mlClass.ParentName  ().QuotedStr ()  + ", " +
                     mlClass.Description ().QuotedStr ()  + ", " +
                     mandatoryStr.QuotedStr ()            + ", " +
                     summarizeStr.QuotedStr ()            +
                     ")";
  
  kkint32  returnCd = QueryStatement (insertStr);
  successful = (returnCd == 0);
  if  (successful)
  {
    // We perform the 'MLClassLoad' so that the classId field is populated with correct value.  Also note
    // that there is only one instance in memory of any given class(See MLClass.h).
    KKStr  className = mlClass.Name ();
    MLClassLoad (className);
  }

  ResultSetsClear ();
}  /* MLClassInsert */



void  DataBase::MLClassInsertReturn (MLClass&  mlClass,
                                     bool&     successful
                                    )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("MLClassInsertReturn");
    return;
  }

  KKStr  mandatoryStr = "N";
  if  (mlClass.Mandatory ())
    mandatoryStr = "Y";

  KKStr  summarizeStr = "N";
  if  (mlClass.Summarize ())
    summarizeStr = "Y";


  KKStr  insertStr = "call MLClassInsertReturn(" + 
                     mlClass.Name       ().QuotedStr () + ", " + 
                     mlClass.ParentName ().QuotedStr () + ", " +
                     mandatoryStr.QuotedStr () +        + ", " +
                     summarizeStr.QuotedStr () +
                     ")";
  
  kkint32  returnCd = QueryStatement (insertStr);
  successful = (returnCd == 0);
  if  (successful)
  {
    MLClassListPtr  classes = MLClassProcessResults ();
    if  (!classes)
      successful = false;
    else if  (classes->QueueSize () < 1)
      successful = false;

    delete  classes;
    classes = NULL;
  }
}  /* MLClassInsertReturn */





void  DataBase::MLClassUpdate (const KKStr&    oldClassName,
                               const MLClass&  mlClass,
                               bool&           successful
                              )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("MLClassUpdate");
    return;
  }

  KKStr  mandatoryStr = "N";
  if  (mlClass.Mandatory ())
    mandatoryStr = "Y";

  KKStr  summarizeStr = "N";
  if  (mlClass.Summarize ())
    summarizeStr = "Y";

  KKStr updateStr = "Call MLClassUpdate(" +
                          oldClassName.QuotedStr ()           + ", " +
                          mlClass.Name        ().QuotedStr () + ", " +
                          mlClass.ParentName  ().QuotedStr () + ", " +
                          mlClass.Description ().QuotedStr () + ", " +
                          mandatoryStr.QuotedStr ()           + ", " +
                          summarizeStr.QuotedStr ()           +
                          ")";

  kkint32  returnCd = QueryStatement (updateStr);
  successful = (returnCd == 0);
  ResultSetsClear ();
}  /* MLClassUpdate */




void  DataBase::MLClassDelete (const KKStr&  mlClassName)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("MLClassDelete");
    return;
  }

  KKStr  deleteStr = "Call MLClassDelete(" + mlClassName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (deleteStr);
  ResultSetsClear ();
}






//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*                                  Image  Routines                                      *
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************


KKB::uchar*  DataBase::EncodeARasterImageIntoAThumbNail (const RasterSipper&  image,
                                                         kkuint32             maxDimSizeAllowed,
                                                         kkuint32&              buffLen
                                                        )
{
  RasterSipperPtr  thumbNail = NULL;
  kkuint32  maxDim = Max (image.Height (), image.Width ());

  if  (maxDim > maxDimSizeAllowed)
  {
    float  reductionFactor = (float)maxDimSizeAllowed / (float)maxDim;
    //thumbNail = image.ReduceByEvenMultiple (multiple);
    thumbNail = image.ReduceByFactor (reductionFactor);
  }

  uchar*  compressedBuff = NULL;
  if  (thumbNail)
  {
    compressedBuff = thumbNail->SimpleCompression (buffLen);
    delete  thumbNail; 
    thumbNail = NULL;
  }
  else
  {
    compressedBuff = image.SimpleCompression (buffLen);
  }
  
  return  compressedBuff;
}  /* EncodeARasterImageIntoAThumbNail */





void  DataBase::ImageInsert (const RasterSipper&  image,
                             const KKStr&         imageFileName,
                             const KKStr&         sipperFileName,
                                   kkuint64       byteOffset,     // byteOffset of SipperRow containing TopLeftRow
                                   kkuint32       topLeftRow,
                                   kkuint32       topLeftCol,
                                   kkuint32       height,
                                   kkuint32       width,
                                   kkuint32       pixelCount,
                                   uchar          connectedPixelDist,
                                   kkuint32       extractionLogEntryId,
                                   kkuint32       classLogEntryId,
                                   kkuint32       centroidRow,
                                   kkuint32       centroidCol,
                                   MLClassPtr     class1,
                                   float          class1Prob,
                                   MLClassPtr     class2,
                                   float          class2Prob,
                                   MLClassPtr     validatedClass,
                                   float          depth,
                                   float          imageSize,
                                   PointListPtr   sizeCoordinates,
                                   kkint32&       imageId,
                                   bool&          successful
                            )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageInsert");
    return;
  }

  KKStr  insertStatement (3000);
  KKStr  class1Name = "";
  KKStr  class2Name = "";
  KKStr  validatedClassName = "";

  if  (class1)
    class1Name = class1->Name ();

  if  (class2)
    class2Name = class2->Name ();

  if  (validatedClass)
    validatedClassName = validatedClass->Name ();

  kkuint32       imageCompressedBuffLen      = 0;
  uchar*         mySqlImageCompressedBuff    = NULL;
  unsigned long  mySqlImageCompressedBuffLen = 0;

  uchar* imageCompressedBuff = EncodeARasterImageIntoAThumbNail (image, 100, imageCompressedBuffLen);
  if  (imageCompressedBuff == NULL)
  {
    mySqlImageCompressedBuff = NULL;
    mySqlImageCompressedBuffLen = 0;
  }
  else
  {
    mySqlImageCompressedBuff = new uchar[imageCompressedBuffLen * 2 + 5];
    mySqlImageCompressedBuffLen = mysql_real_escape_string (conn, (char*)mySqlImageCompressedBuff, (char*)imageCompressedBuff, imageCompressedBuffLen);
  }

  //_fpclass

  KKStr  sizeCoordinatesStr (33);
  if  (sizeCoordinates)
  {
    PointList::const_iterator idx;
    int  x = 0;
    for  (idx = sizeCoordinates->begin ();  idx != sizeCoordinates->end ();  ++idx, ++x)
    {
      if  (x > 0)  sizeCoordinatesStr << ",";
      const PointPtr p = *idx;
      sizeCoordinatesStr << p->Row () << "," << p->Col ();
    }
  }

  insertStatement << "call ImagesInsert("
                  << imageFileName.QuotedStr ()       << ", "
                  << sipperFileName.QuotedStr ()      << ", "
                  << byteOffset                       << ", "
                  << topLeftRow                       << ", "
                  << topLeftCol                       << ", "
                  << height                           << ", "
                  << width                            << ", "
                  << pixelCount                       << ", "
                  << connectedPixelDist               << ", "
                  << extractionLogEntryId             << ", "
                  << classLogEntryId                  << ", "
                  << centroidRow                      << ", "
                  << centroidCol                      << ", "
                  << class1Name.QuotedStr ()          << ", "
                  << FloatToStr (class1Prob)          << ", "
                  << class2Name.QuotedStr ()          << ", "
                  << FloatToStr (class2Prob)          << ", "
                  << validatedClassName.QuotedStr ()  << ", "
                  << depth                            << ", "
                  << imageSize                        << ", "
                  << sizeCoordinatesStr.QuotedStr ()  << ", ";

  if  (mySqlImageCompressedBuff == NULL)
  {
    insertStatement << "NULL";
  }
  else
  {
    insertStatement.Append ('"');
    insertStatement.Append ((char*)mySqlImageCompressedBuff, mySqlImageCompressedBuffLen);
    insertStatement.Append ('"');
  }

  insertStatement << ")";

  kkint32  returnCd = QueryStatement (insertStatement);
  successful = (returnCd == 0);
  delete  mySqlImageCompressedBuff;
  mySqlImageCompressedBuff = NULL;
  
  delete  imageCompressedBuff;
  imageCompressedBuff = NULL;

  if  (successful)
  {
    imageId = (kkint32)mysql_insert_id (conn);

    ResultSetsClear ();

    kkint32  maxDim = Max (image.Height (), image.Width ());
    if  (maxDim > 100)
    {
      // The image had to be reduced to a thumbnail size so we want to store a copy of the original in the ImagesFullSize table.
      ImageFullSizeSave (imageFileName, image);
    }
  }
}  /* ImageInsert */





void  DataBase::ImagesEraseSipperFile (const KKStr&  _sipperFileName)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesEraseSipperFile");
    return;
  }

  KKStr  sipperFileName = osGetRootName (_sipperFileName);
  kkint32  returnCd = 0;

  KKStr  imageDeleteStatement = "call ImagesEraseSipperFile(" + sipperFileName.QuotedStr () + ")";
  returnCd = QueryStatement (imageDeleteStatement);
  ResultSetsClear ();
}  /* ImagesEraseSipperFile */




/**
 *@brief  Will remove all images and associated data for a specified SipperFile that are within the specified size range from the database.
 *@details Will also delete all Instrument data for the specified SIPPER file on the assumption that the calling procedure 
 * is going to re-extract instrument data.
 */
void  DataBase::ImagesEraseSipperFileSizeRange (const   KKStr&  _sipperFileName,
                                                kkuint32        _minSize,
                                                kkuint32        _maxSize
                                               )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesEraseSipperFileSizeRange");
    return;
  }

  KKStr  sipperFileName = osGetRootName (_sipperFileName);
  kkint32  returnCd = 0;

  if  (_maxSize == 0)
    _maxSize = INT_MAX;

  KKStr  queryStr = "call ImagesEraseSipperFileSizeRange(" + 
                                                         sipperFileName.QuotedStr ()  + ", "  +
                                                         StrFromUint32 (_minSize)     + ", "  +
                                                         StrFromUint32 (_maxSize)     +
                                                         ")";
  returnCd = QueryStatement (queryStr);
  ResultSetsClear ();
}  /* ImagesEraseSipperFileSizeRange */




ClassStatisticListPtr  DataBase::ImageProcessClassStaticsResults ()
{
  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  ClassStatisticListPtr  stats = new ClassStatisticList (true);

  kkint32  classId;
  KKStr  className;
  kkuint32 count;

  while  (ResultSetFetchNextRow ())
  {
    classId   = CharStarToInt32 (resultSetNextRow[0]);
    className = resultSetNextRow[1];
    count     = CharStarToInt32 (resultSetNextRow[2]);
    if  (count > 0)
    {
      ClassStatisticPtr  stat = new ClassStatistic (MLClass::CreateNewMLClass (className, classId), count);
      stats->PushOnBack (stat);
    }
  }

  ResulSetFree ();

  ResultSetsClear ();

  return  stats;
}  /* ImageProcessClassStaticsResults */




ClassStatisticListPtr  DataBase::ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                          const KKStr&           sipperFileName,
                                                          MLClassPtr             mlClass,
                                                          char                   classKeyToUse,
                                                          float                  minProb,
                                                          float                  maxProb,
                                                          kkint32                minSize,
                                                          kkint32                maxSize,
                                                          float                  minDepth,
                                                          float                  maxDepth
                                                         )
{
  KKStr  selectStr (1024);

  KKStr  groupNameStr = ((imageGroup == NULL) ? "" : imageGroup->Name ());

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);


  KKStr  className = "";
  if  (mlClass)
    className = mlClass->Name ();

  selectStr << "call ImagesGetClassStatistics("
            << groupNameStr.QuotedStr      () << ", "
            << sipperFileName.QuotedStr    () << ", "
            << className.QuotedStr         () << ", "
            << classKeyToUseStr.QuotedStr  () << ", "
            << minProb                        << ", "
            << maxProb                        << ", "
            << minSize                        << ", "
            << maxSize                        << ", "
            << minDepth                       << ", "
            << maxDepth
            << ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  ClassStatisticListPtr  stats = ImageProcessClassStaticsResults ();
  ResultSetsClear ();
 
  return  stats;
}  /* ImageGetClassStatistics */




ClassStatisticListPtr  DataBase::ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                          const KKStr&           cruiseName,
                                                          const KKStr&           stationName,
                                                          const KKStr&           deploymentNum,
                                                          MLClassPtr             mlClass,
                                                          char                   classKeyToUse,
                                                          float                  minProb,
                                                          float                  maxProb,
                                                          kkint32                minSize,
                                                          kkint32                maxSize,
                                                          float                  minDepth,
                                                          float                  maxDepth
                                                         )
{
  if  (cruiseName.Empty ()  &&  stationName.Empty ()  &&  deploymentNum.Empty ())
  {
    // We do not care about individual SipperFiles
    return  ImageGetClassStatistics (imageGroup, "", mlClass, classKeyToUse, minProb, maxProb, minSize, maxSize, minDepth, maxDepth);
  }

  VectorKKStr*  sipperFiles = SipperFileGetList (cruiseName, stationName, deploymentNum);

  if  (!sipperFiles)
    return NULL;

  ClassStatisticListPtr  stats = new ClassStatisticList (true);

  VectorKKStr::iterator  idx;
  for  (idx = sipperFiles->begin ();  idx != sipperFiles->end ();  idx++)
  {
    ClassStatisticListPtr  statsOneFile = ImageGetClassStatistics (imageGroup, *idx, mlClass, classKeyToUse, minProb, maxProb, minSize, maxSize, minDepth, maxDepth);
    if  (statsOneFile)
    {
      (*stats) += (*statsOneFile);
      delete  statsOneFile;
      statsOneFile = NULL;
    }
  }

  delete  sipperFiles;

  return  stats;
}  /* ImageGetClassStatistics */




VectorUint*  DataBase::ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                                const KKStr&           sipperFileName,
                                                float                  depthIncrements,
                                                MLClassPtr             mlClass,
                                                char                   classKeyToUse,
                                                float                  minProb,
                                                float                  maxProb,
                                                kkint32                minSize,
                                                kkint32                maxSize
                                               )
{
  KKStr  selectStr (1024);

  KKStr  groupNameStr = ((imageGroup == NULL) ? "" : imageGroup->Name ());
  KKStr  classNameStr = ((mlClass == NULL) ? "" : mlClass->Name ());

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);

  if  ((depthIncrements <= 0.0f)  ||  (depthIncrements > 1000.0f))
  {
    log.Level (-1) << endl << endl << "DataBase::ImageGetDepthStatistics    ***ERROR***    depthIncrements[" << depthIncrements << "] is unrealistic,  defaulting to 10.0 meters." << endl << endl;
    depthIncrements = 10.0f;
  }
  
  selectStr << "call ImagesGetDepthStatistics("
            << groupNameStr.QuotedStr      () << ", "
            << sipperFileName.QuotedStr   ()  << ", "
            << depthIncrements                << ", "
            << classNameStr.QuotedStr     ()  << ", "
            << classKeyToUseStr.QuotedStr ()  << ", "
            << minProb                        << ", "
            << maxProb                        << ", "
            << minSize                        << ", "
            << maxSize
            << ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  VectorUint*  counts = new VectorUint;
  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  counts;

  kkint32 incIndex;
  kkuint32  count;

  while  (ResultSetFetchNextRow ())
  {
    incIndex = atoi (resultSetNextRow[0]);
    if  (incIndex < 0)
      incIndex = 0;

    count = CharStarToUint32 (resultSetNextRow[1]);

    while  (counts->size () <= (kkuint32)incIndex)
      counts->push_back (0);

    (*counts)[incIndex] = count;
  }

  ResulSetFree ();

  ResultSetsClear ();

  return  counts;
}  /* ImageGetDepthStatistics */




VectorUint*  DataBase::ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                                const KKStr&           cruiseName,
                                                const KKStr&           stationName,
                                                const KKStr&           deploymentNum,
                                                float                  depthIncrements,
                                                MLClassPtr             mlClass,
                                                char                   classKeyToUse,
                                                float                  minProb,
                                                float                  maxProb,
                                                kkint32                minSize,
                                                kkint32                maxSize,
                                                bool&                  cancelFlag
                                               )
{
  if  ((depthIncrements <= 0.0f)  ||  (depthIncrements > 1000.0f))
  {
    log.Level (-1) << endl
                   << endl
                   << "DataBase::ImageGetDepthStatistics    ***ERROR***    depthIncrements[" << depthIncrements << "] is unrealistic,  defaulting to 10.0 meters." << endl
                   << endl;
    depthIncrements = 10.0f;
  }


  bool  allSipperFiles = cruiseName.Empty ()  &&  stationName.Empty ()  &&  deploymentNum.Empty ();
  if  (allSipperFiles)
  {
    return ImageGetDepthStatistics (imageGroup, "", depthIncrements, mlClass, classKeyToUse, minProb, maxProb, minSize, maxSize);
  }

  VectorKKStr*  sipperFiles = SipperFileGetList (cruiseName, stationName, deploymentNum);
  if  (!sipperFiles)
    return  NULL;

  VectorUint* counts = new VectorUint ();

  VectorKKStr::iterator  idx;
  for  (idx = sipperFiles->begin ();  ((idx != sipperFiles->end ())  &&  (!cancelFlag));  idx++)
  {
    VectorUint*  countsThisSipperFile = ImageGetDepthStatistics (imageGroup, *idx, depthIncrements, mlClass, classKeyToUse, minProb, maxProb, minSize, maxSize);
    if  (countsThisSipperFile)
    {
      while  (counts->size () <= countsThisSipperFile->size ())
        counts->push_back (0);
      for  (kkuint32 x = 0;  x < countsThisSipperFile->size ();  x++)
        (*counts)[x] += (*countsThisSipperFile)[x];

      delete  countsThisSipperFile;
      countsThisSipperFile = NULL;
    }
  }

  delete  sipperFiles;

  return  counts;
}  /* ImageGetDepthStatistics */



DataBaseImageListPtr  DataBase::ImageQueryProcessResults ()
{
  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  DataBaseImageListPtr  results = new DataBaseImageList (true);
 
  kkint32  imageIdIdx              = -1;
  kkint32  imageFileNameIdx        = -1;
  kkint32  sipperFileNameIdx       = -1;
  kkint32  class1IdIdx             = -1;
  kkint32  class1NameIdx           = -1; 
  kkint32  class1ProbIdx           = -1;
  kkint32  class2IdIdx             = -1;
  kkint32  class2NameIdx           = -1;
  kkint32  class2ProbIdx           = -1;
  kkint32  ClassValidatedIdIdx     = -1;
  kkint32  classNameValidatedIdx   = -1;
  kkint32  topLeftRowIdx           = -1;
  kkint32  topLeftColIdx           = -1;
  kkint32  pixelCountIdx           = -1;
  kkint32  connectedPixelDistIdx   = -1;
  kkint32  extractionLogEntryIdIdx = -1;
  kkint32  classLogEntryIdIdx      = -1;
  kkint32  centroidRowIdx          = -1;
  kkint32  centroidColIdx          = -1;
  kkint32  heightIdx               = -1;
  kkint32  widthIdx                = -1;
  kkint32  byteOffsetIdx           = -1;
  kkint32  depthIdx                = -1;
  kkint32  imageSizeIdx            = -1;
  kkint32  sizeCoordinatesIdx      = -1;
  kkint32  thumbnailIdx            = -1;

  {
    // Build Field Index
    MYSQL_FIELD *field;
    kkint32  idx = 0;
    while  ((field = mysql_fetch_field (resultSet)))
    {
      KKStr  fieldName = field->name;
      if  (fieldName.CompareIgnoreCase ("ImageId") == 0)
        imageIdIdx = idx;

      else if  (fieldName.EqualIgnoreCase ("ImageFileName"))
        imageFileNameIdx = idx;
      
      else if  (fieldName.CompareIgnoreCase ("SipperFileName") == 0)
        sipperFileNameIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class1Id") == 0)
        class1IdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class1Name") == 0)
        class1NameIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class1Prob") == 0)
        class1ProbIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class2Id") == 0)
        class2IdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class2Name") == 0)
        class2NameIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class2Prob") == 0)
        class2ProbIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ClassValidatedId") == 0)
        ClassValidatedIdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ClassNameValidated") == 0)
        classNameValidatedIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("topLeftRow") == 0)
        topLeftRowIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("topLeftCol") == 0)
        topLeftColIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("pixelCount") == 0)
        pixelCountIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ConnectedPixelDist") == 0)
        connectedPixelDistIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ExtractionLogEntryId") == 0)
        extractionLogEntryIdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ClassLogEntryId") == 0)
        classLogEntryIdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("CentroidRow") == 0)
        centroidRowIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("CentroidCol") == 0)
        centroidColIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("height") == 0)
        heightIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("width") == 0)
        widthIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("byteOffset") == 0)
        byteOffsetIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("depth") == 0)
        depthIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ImageSize") == 0)
        imageSizeIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("SizeCoordinates") == 0)
        sizeCoordinatesIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("thumbnail") == 0)
        thumbnailIdx = idx;

      idx++;
    }
  }

  kkuint32 numFields = resultSetNumFields;

  kkuint32 imageId            = 0;
  kkint32  class1Id           = -1;
  KKStr  class1Name         = "";
  kkint32  class2Id           = -1;
  KKStr  class2Name         = "";
  kkint32  classValidatedId   = -1;
  KKStr  classNameValidated = "";

  while  (ResultSetFetchNextRow ())
  {
    ulong*  lengths    = mysql_fetch_lengths (resultSet);
    DataBaseImagePtr  i = new DataBaseImage ();

    class1Id           = -1;
    class1Name         = "";
    class2Id           = -1;
    class2Name         = "";
    classValidatedId   = -1;
    classNameValidated = "";
    imageId            = 0;

    if  (imageIdIdx >= 0)
      i->ImageId (CharStarToUint32 (resultSetNextRow[imageIdIdx]));

    if  (imageFileNameIdx >= 0)
      i->ImageFileName (resultSetNextRow[imageFileNameIdx]);

    if  (sipperFileNameIdx >= 0)
      i->SipperFileName (resultSetNextRow[sipperFileNameIdx]);

    if  (class1IdIdx >= 0)
    {
      if  (resultSetNextRow[class1IdIdx])
        class1Id = CharStarToInt32 (resultSetNextRow[class1IdIdx]);
    }

    if  (class1NameIdx >= 0)
    {
      KKStr  class1Name = resultSetNextRow[class1NameIdx];
      if  (class1Name.Empty ())
        class1Name = "ClassId_" + StrFormatInt (class1Id, "0000");
      i->Class1 (MLClass::CreateNewMLClass (class1Name, class1Id));
    }

    if  (class1ProbIdx >= 0)
      i->Class1Prob (CharStarToFloat (resultSetNextRow[class1ProbIdx]));

    if  (class2IdIdx >= 0)
    {
      if  (resultSetNextRow[class2IdIdx])
        class2Id = CharStarToInt32 (resultSetNextRow[class2IdIdx]);
    }

    if  (class2NameIdx >= 0)
    {
      KKStr  class2Name = resultSetNextRow[class2NameIdx];
      if  (!class2Name.Empty ())
        i->Class2 (MLClass::CreateNewMLClass (resultSetNextRow[class2NameIdx], class2Id));
    }

    if  (class2ProbIdx >= 0)
      i->Class2Prob (CharStarToFloat (resultSetNextRow[class2ProbIdx]));

    if  (ClassValidatedIdIdx >= 0)
    {
      if  (resultSetNextRow[ClassValidatedIdIdx])
        classValidatedId = CharStarToInt32 (resultSetNextRow[ClassValidatedIdIdx]);
    }

    if  (classNameValidatedIdx >= 0)
    {
      KKStr classNameValidated = resultSetNextRow[classNameValidatedIdx];
      if  (!classNameValidated.Empty ())
        i->ValidatedClass (MLClass::CreateNewMLClass (classNameValidated, classValidatedId));
    }

    if  (topLeftRowIdx >= 0)
      i->TopLeftRow (CharStarToUint32  (resultSetNextRow[topLeftRowIdx]));

    if  (topLeftColIdx >= 0)
      i->TopLeftCol (CharStarToUint32  (resultSetNextRow[topLeftColIdx]));

    if  (pixelCountIdx >= 0)
      i->PixelCount (CharStarToUint32  (resultSetNextRow[pixelCountIdx]));

    if  (connectedPixelDistIdx >= 0)
      i->ConnectedPixelDist ((uchar)CharStarToUint32 (resultSetNextRow[connectedPixelDistIdx]));

    if  (extractionLogEntryIdIdx >= 0)
      i->ExtractionLogEntryId (CharStarToUint32 (resultSetNextRow[extractionLogEntryIdIdx]));

    if  (classLogEntryIdIdx >= 0)
      i->ClassLogEntryId (CharStarToUint32 (resultSetNextRow[classLogEntryIdIdx]));

    if  (centroidRowIdx >= 0)
      i->CentroidRow (CharStarToUint32 (resultSetNextRow[centroidRowIdx]));

    if  (centroidColIdx >= 0)
      i->CentroidCol (CharStarToUint32 (resultSetNextRow[centroidColIdx]));

    if  (heightIdx >= 0)
      i->Height (CharStarToUint32  (resultSetNextRow[heightIdx]));

    if  (widthIdx >= 0)
      i->Width (CharStarToUint32  (resultSetNextRow[widthIdx]));

    if  (byteOffsetIdx >= 0)
      i->ByteOffset (CharStarToInt64 (resultSetNextRow[byteOffsetIdx]));

    if  (depthIdx >= 0)
      i->Depth (CharStarToFloat (resultSetNextRow[depthIdx]));

    if  (imageSizeIdx >= 0)
      i->ImageSize (CharStarToFloat (resultSetNextRow[imageSizeIdx]));

    if  (sizeCoordinatesIdx >= 0)
    {
      i->SizeCoordinates (resultSetNextRow[sizeCoordinatesIdx]);
    }

    if  (thumbnailIdx >= 0)
    {
      kkint32 blockLen = lengths[thumbnailIdx];

      uchar*  encodedThumbNail = new uchar[blockLen];
      memcpy (encodedThumbNail, resultSetNextRow[thumbnailIdx], blockLen);

      kkint32 encBlockLen = encodedThumbNail[0] * 256 + encodedThumbNail[1];  // This should be the same as 'blockLen'

      if  (encBlockLen != blockLen)
      {
        log.Level (-1)  << "DataBase::ImageQueryProcessResults     ***ERROR***"  << endl
                        << endl
                        << "DataBase::ImageQueryProcessResults      There is an error in Compress Block Length data." << endl
                        << endl;
      }

      i->EncodedThumbnail    (encodedThumbNail);
      i->EncodedThumbnailLen (blockLen);
    }

    results->PushOnBack (i);
  }

  ResulSetFree ();

  return  results;
}  /* ImageQueryProcessResults */





DataBaseImagePtr  DataBase::ImagesLocateClosestImage (const KKStr&  imageFileName)
{
  KKStr   sipperFileName (32);
  kkuint32  scanLineNum = 0;
  kkuint32  scanCol     = 0;
  PicesVariables::ParseImageFileName (imageFileName, sipperFileName, scanLineNum, scanCol);
  return  ImagesLocateClosestImage (sipperFileName, scanLineNum, scanCol);
}  /* ImagesLocateClosestImage */





DataBaseImagePtr  DataBase::ImagesLocateClosestImage (const KKStr&  sipperFileName,
                                                      kkuint32      scanLine,
                                                      kkuint32      scanCol
                                                     )
{
  KKStr  sqlStr = "call  ImagesLocateClosestImage(" + sipperFileName.QuotedStr ()         + ", " + 
                                                      StrFormatInt (scanLine, "ZZZZZZZ0") + ", " +
                                                      StrFormatInt (scanCol,  "ZZZZZZZ0") + 
                                                 ")";
  

  KKStrMatrixPtr  results = QueryStatementKKStr (sqlStr, NULL);
  if  (results == NULL)
    return NULL;

  if  ((results->NumRows () < 1)  ||  (results->NumCols () < 2))
  {
    delete  results;
    return NULL;
  }

  KKStr  imageFileNameInDataBase = (*results)[0][1];
  delete  results;  results = NULL;

  return  ImageLoad (imageFileNameInDataBase);
}  /* ImagesLocateClosestImage */




DataBaseImagePtr  DataBase::ImageLoad (kkuint32  imageId)
{
  KKStr selectStr (100);
  selectStr << "call ImagesLoadByImageId(" << imageId << ")";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr  images =  ImageQueryProcessResults ();

  ResultSetsClear ();
 
  if  ((images == NULL)  ||  (images->QueueSize () < 1))
  {
    delete  images;  images = NULL;
    return NULL;
  }
  DataBaseImagePtr  dbImage = images->PopFromBack ();
  delete  images; images = NULL;

  return  dbImage;
}  /* ImageLoad */




DataBaseImagePtr  DataBase::ImageLoad (const KKStr&   imageFileName)
{
  if  (this == NULL)
  {
    cerr  << endl << "DataBase::ImageLoad     this == NULL" << endl << endl;
  }

  KKStr selectStr = "call ImagesLoadByImageFileName(" + imageFileName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr  images =  ImageQueryProcessResults ();

  ResultSetsClear ();
 
  if  ((images == NULL)  ||  (images->QueueSize () < 1))
  {
    delete  images;  images = NULL;
    return NULL;
  }
  DataBaseImagePtr  dbImage = images->PopFromBack ();
  delete  images; images = NULL;

  return  dbImage;
}  /* ImageLoad */




DataBaseImageListPtr  DataBase::ImagesQuery (DataBaseImageGroupPtr  group,
                                             bool                   includeThumbnail,
                                             const bool&            cancelFlag
                                            )
{
  if  (!group)
    return NULL;

  KKStr  selectStr = "call  ImagesQueryByImageGroup(" + 
                     group->Name ().QuotedStr () + ", " +
                     (includeThumbnail ? "true":"false") +
                     ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  return  results;
}  /* ImagesQuery */
    



DataBaseImageListPtr  DataBase::ImagesQuery (DataBaseImageGroupPtr  imageGroup,
                                             const KKStr&           sipperFileName,
                                             MLClassPtr             mlClass,
                                             char                   classKeyToUse,
                                             float                  probMin,
                                             float                  probMax,
                                             kkint32                sizeMin,
                                             kkint32                sizeMax,
                                             float                  depthMin,
                                             float                  depthMax,
                                             kkuint32               restartImageId,
                                             kkint32                limit,            // Max # of rows 2 return.  -1 indicates no limit.
                                             bool                   includeThumbnail,
                                             const bool&            cancelFlag
                                            )
{
  KKStr  selectStr(512);

  KKStr  emptyStr = "\"\"";

  KKStr  groupNameStr = ((imageGroup == NULL) ? "" : imageGroup->Name ());
  KKStr  classNameStr = ((mlClass == NULL) ? "" : mlClass->Name ());
  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);


  selectStr << "call  ImagesQuery("  
            << groupNameStr.QuotedStr     ()  << ", "
            << sipperFileName.QuotedStr   ()  << ", " 
            << classNameStr.QuotedStr     ()  << ", "
            << classKeyToUseStr.QuotedStr ()  << ", "
            << probMin  << ", " << probMax    << ", "
            << sizeMin  << ", " << sizeMax    << ", "
            << depthMin << ", " << depthMax   << ", "
            << restartImageId                 << ", "
            << limit                          << ", "  
            << (includeThumbnail ? "true" : "false")
            << ")";
   
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  return  results;
}  /* ImagesQuery */




DataBaseImageListPtr  DataBase::ImagesQuery (DataBaseImageGroupPtr  imageGroup,
                                             const KKStr&           cruiseName,
                                             const KKStr&           stationName,
                                             const KKStr&           deploymentNum,
                                             MLClassPtr             mlClass,
                                             char                   classKeyToUse,
                                             float                  probMin,
                                             float                  probMax,
                                             kkint32                sizeMin,
                                             kkint32                sizeMax,
                                             float                  depthMin,
                                             float                  depthMax,
                                             kkuint32               restartImageId,
                                             kkint32                limit,            // Max # of rows to return.  -1 indicates no limit.,
                                             bool                   includeThumbnail,
                                             const bool&            cancelFlag
                                            )
{
  KKStr  selectStr (4096);
  VectorKKStr*  sipperFiles = SipperFileGetList (cruiseName, stationName, deploymentNum);

  if  (!sipperFiles)
    return NULL;

  DataBaseImageListPtr  images = new DataBaseImageList (true);

  VectorKKStr::iterator  idx;
  for  (idx = sipperFiles->begin ();  (idx != sipperFiles->end ())  &&  (!cancelFlag);  idx++)
  {
    DataBaseImageListPtr  imagesForOneSipperFile = ImagesQuery (imageGroup,
                                                                *idx,           mlClass,  classKeyToUse,
                                                                probMin,        probMax, 
                                                                sizeMin,        sizeMax, 
                                                                depthMin,       depthMax, 
                                                                restartImageId, limit,
                                                                includeThumbnail,        //  true = include thumbNail
                                                                cancelFlag
                                                               );
    if  (imagesForOneSipperFile)
    {
      imagesForOneSipperFile->Owner (false);
      images->AddQueue (*imagesForOneSipperFile);
      delete  imagesForOneSipperFile;
      imagesForOneSipperFile = NULL;
    }
  }

  if  (cancelFlag)
  {
    delete  images;
    images = NULL;
  }

  delete  sipperFiles;

  return  images;
}  /* ImagesQuery */



DataBaseImageListPtr  DataBase::ImagesQueryByGrouop 
                                  (DataBaseImageGroupPtr  imageGroup,
                                   const KKStr&           cruiseName,
                                   const KKStr&           stationName,
                                   const KKStr&           deploymentNum,
                                   const KKStr&           sipperFileName,
                                   MLClassPtr             mlClass,
                                   char                   classKeyToUse,
                                   float                  probMin,
                                   float                  probMax,
                                   kkint32                sizeMin,
                                   kkint32                sizeMax,
                                   float                  depthMin,
                                   float                  depthMax,
                                   kkuint32               restartImageId,
                                   kkint32                limit,            // Max # of rows to return.  -1 indicates no limit.,
                                   bool                   includeThumbnail
                                  )
{
  KKStr  sqlStr(512);

  KKStr  imageGroupName (32);
  if  (imageGroup)  imageGroupName = imageGroup->Name ();

  KKStr  className (32);
  if  (mlClass)  className = mlClass->Name ();

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);

  sqlStr    << "call  ImagesQueryByGrouop("  
            << imageGroupName.QuotedStr   ()         << ", "
            << cruiseName.QuotedStr       ()         << ", "
            << stationName.QuotedStr      ()         << ", "
            << deploymentNum.QuotedStr    ()         << ", "
            << sipperFileName.QuotedStr   ()         << ", "
            << className.QuotedStr        ()         << ", "
            << classKeyToUseStr.QuotedStr ()         << ", "
            << probMin  << ", " << probMax           << ", "
            << sizeMin  << ", " << sizeMax           << ", "
            << depthMin << ", " << depthMax          << ", "
            << restartImageId                        << ", "
            << limit                                 << ", "  
            << (includeThumbnail ? "true" : "false")
            << ")";
   
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  return  results;
}  /* ImagesQuery */






DataBaseImageListPtr  DataBase::ImagesQueryForScanLineRange (const KKStr&   sipperFileName,
                                                             kkuint32       scanLineStart,
                                                             kkuint32       scanLineEnd
                                                            )
{
  KKStr selectStr = "call ImagesQueryForScanLineRange(" + sipperFileName.QuotedStr ()            + ", "
                                                        + StrFormatInt (scanLineStart, "ZZZZZ0") + ", "
                                                        + StrFormatInt (scanLineStart, "ZZZZZ0") +
                                                      ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr  results = ImageQueryProcessResults ();
  ResultSetsClear ();
  return  results;
}  /* ImagesQueryForScanLineRange */




DataBaseImageListPtr  DataBase::ImagesQueryDeploymentSizeRange (const KKStr&     cruiseName,
                                                                const KKStr&     stationName,
                                                                const KKStr&     deploymentNum,
                                                                MLClassPtr       mlClass,
                                                                char             cast,         /**< 'U' = UpCast, 'D' = DownCast,  'B' = Both' */
                                                                char             statistic,    /**< '0' = Area mm^2,  '1' = Diameter,  '2' = Spheroid Volume and '3' = EBv ((4/3)(Pie)(Major/2)(Minor/2)^2) */
                                                                float            sizeStart,
                                                                float            sizeEnd,
                                                                float            depthMin,
                                                                float            depthMax,
                                                                kkint32          sampleQty,
                                                                bool             includeChildren,
                                                                VolConstBool&    cancelFlag
                                                               )
{
  if  (cancelFlag)
    return NULL;

  KKStr  sqlStr(512);

  KKStr  className (32);
  if  (mlClass)  className = mlClass->Name ();

  KKStr  castStr = "";
  castStr.Append (cast);
  KKStr  statisticStr;
  statisticStr.Append (statistic);

  sqlStr  << "call  ImagesQueryDeploymentSizeRange("  
            << cruiseName.QuotedStr       ()         << ", "
            << stationName.QuotedStr      ()         << ", "
            << deploymentNum.QuotedStr    ()         << ", "
            << className.QuotedStr        ()         << ", "
            << castStr.QuotedStr          ()         << ", "
            << statisticStr.QuotedStr     ()         << ", "
            << sizeStart  << ", " << sizeEnd         << ", "
            << depthMin   << ", " << depthMax        << ", "
            << sampleQty
            << ")";
   
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  if  ((mlClass != NULL)   &&  includeChildren)
  {
    MLClassListPtr  children = MLClassLoadChildren (mlClass->Name ());
    if  (children)
    {
      MLClassList::const_iterator  idx;
      for  (idx = children->begin ();  idx != children->end ();  ++idx)
      {
        MLClassPtr       child = *idx;
        DataBaseImageListPtr  childsImages 
          = ImagesQueryDeploymentSizeRange (cruiseName, stationName, deploymentNum,
                                            child,
                                            cast,       statistic,
                                            sizeStart,  sizeEnd,
                                            depthMin,   depthMax,
                                            sampleQty,
                                            includeChildren,
                                            cancelFlag
                                           );
        if  (results)
        {
          results->AddQueue (*childsImages);
          childsImages->Owner (false);
          delete  childsImages;
          childsImages = NULL;
        }
        else
        {
          results = childsImages;
        }
      }
      delete  children;
      children = NULL;
    }
  }

  return  results;
}  /* ImagesExtractDeploymentSizeRange */






VectorKKStr*   DataBase::ImageListOfImageFileNamesByScanLineRange (const KKStr&   sipperFileName,
                                                                   kkuint32       scanLineStart,
                                                                   kkuint32       scanLineEnd
                                                                  )
{
  KKStr selectStr = "call ImagesImageFileNamesByScanLineRange(" + sipperFileName.QuotedStr ()            + ", "
                                                                + StrFormatInt (scanLineStart, "ZZZZZ0") + ", "
                                                                + StrFormatInt (scanLineStart, "ZZZZZ0") +
                                                             ")";

  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  char const *  fieldNames[] = {"ImageId", "ImageFileName", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  VectorKKStr*  results = new VectorKKStr ();

  MYSQL_ROW  row = NULL;
  while  (ResultSetFetchNextRow ())
    results->push_back (ResultSetGetField (1));

  ResulSetFree ();

  ResultSetsClear ();

  return  results;
}  /* ImageListOfImageFileNamesByScanLineRange */





void  DataBase::ImagesUpdatePredictions (const KKStr&     imageFileName,
                                         MLClassPtr       class1Pred,
                                         float            class1Prob,
                                         MLClassPtr       class2Pred,
                                         float            class2Prob,
                                         kkuint32         logEntryId
                                       )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesUpdatePredictions");
    return;
  }

  KKStr  updateStr (512);
  if  ((class1Pred == NULL)  &&  (class2Pred == NULL))
    return;

  KKStr  emptyStr = "";
  KKStr  c1Name = ((class1Pred) ?  class1Pred->Name () : emptyStr);
  KKStr  c2Name = ((class2Pred) ?  class2Pred->Name () : emptyStr);

  updateStr << "call ImagesUpdatePredictions("  << imageFileName.QuotedStr () << ", "
                                                << c1Name.QuotedStr ()        << ", "
                                                << class1Prob                 << ", "
                                                << c2Name.QuotedStr ()        << ", "
                                                << class2Prob                 << ", "
                                                << logEntryId
                                                << ")";
  kkint32  returnCd = QueryStatement (updateStr);

  ResultSetsClear ();

}  /* ImagesUpdatePredictions */




void  DataBase::ImagesUpdatePredictionsList (kkuint32      _logEntryId,
                                             const KKStr&  _predictionList
                                            )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesUpdatePredictionsList");
    return;
  }

  KKStr  sqlStr (64 + _predictionList.Len ());
  sqlStr << "call  ImagesUpdatePredictionsList(" << _logEntryId << "," << _predictionList.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  ResultSetsClear ();
}  /* ImagesUpdatePredictionsList */
 





void  DataBase::ImagesUpdateValidatedClass (const KKStr&     imageFileName, 
                                            MLClassPtr       mlClass
                                           )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesUpdateValidatedClass");
    return;
  }

  if  (!mlClass)
    return;

  KKStr  updateStr (512);

  updateStr << "call  ImagesUpdateValidatedClass(" << imageFileName.QuotedStr ()    << ", " 
            <<                                        mlClass->Name ().QuotedStr () << ", "
            <<                                        "1.0"                                                   
            <<                                 ")";
  kkint32  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}  /* ImagesUpdateValidatedClass */




void  DataBase::ImagesUpdateImageSize (const KKStr&        imageFileName,
                                       const PointListPtr  sizeCoordinates,
                                       float               imageSize
                                      )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesUpdateImageSize");
    return;
  }

  KKStr  sizeCoordinatesStr = "";
  if  (sizeCoordinates)
    sizeCoordinatesStr = sizeCoordinates->ToDelStr (',');

  KKStr  updateStr (256);

  updateStr << "call  ImagesUpdateImageSize(" << imageFileName.QuotedStr ()      << ", " 
            <<                                   sizeCoordinatesStr.QuotedStr () << ", "
            <<                                   imageSize
            <<                             ")";
  kkint32  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}





void  DataBase::ImageRemoveValidation (const KKStr&   imageFileName)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageRemoveValidation");
    return;
  }

  KKStr  updateStr (512);

  updateStr << "call  ImagesRemoveValidation(" << imageFileName.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}  /* ImageRemoveValidation */




DataBaseImageValidatedEntryListPtr
      DataBase::ImagesRetrieveValidatedClass (const KKStr&  sipperFileName)
{
  KKStr  sqlStr (128);
  sqlStr << "call  ImagesRetrieveValidatedClass ("
         << osGetRootName (sipperFileName).QuotedStr ()
         << ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)  
    return NULL;

  char  const *  fieldNames[] = {"ImageId",  "ImageFileName", "ClassValidatedId", "ClassNameValidated", "SizeCoordinates", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  DataBaseImageValidatedEntryListPtr  results = new DataBaseImageValidatedEntryList (true);
  MYSQL_ROW  row = NULL;
  while  (ResultSetFetchNextRow ())
  {
    KKStr  imageFileName = ResultSetGetField (1);
    kkint32  validatedClassId = ResultSetGetIntField (2);
    KKStr  validateClassName = ResultSetGetField (3);
    KKStr  sizeCoordinatesStr = ResultSetGetField (4);

    MLClassPtr       validatedClass = NULL;
    if  (!validateClassName.Empty ())
      validatedClass = MLClass::CreateNewMLClass (validateClassName, validatedClassId);
    results->push_back (new DataBaseImageValidatedEntry (imageFileName, sizeCoordinatesStr, validatedClass));
  }

  ResulSetFree ();

  ResultSetsClear ();

  return  results;
}  /* ImagesRetrieveValidatedClass */





void  DataBase::ImagesDeleteOneImage (const KKStr&   imageFileName)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesDeleteOneImage");
    return;
  }

  KKStr  updateStr (512);

  updateStr << "call  ImagesDeleteOneImage(" << imageFileName.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}  /* ImagesDeleteOneImage */





void  DataBase::ImageUpdate (DataBaseImage&  dbImage,
                             RasterSipper&   image
                            )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageUpdate");
    return;
  }

  KKStr  sqlStr (3000);
  KKStr  class1Name = "";
  KKStr  class2Name = "";
  KKStr  validatedClassName = "";

  class1Name = dbImage.Class1Name ();
  class2Name = dbImage.Class2Name ();
  validatedClassName = dbImage.ValidatedClassName ();

  kkuint32       imageCompressedBuffLen      = 0;
  uchar*         mySqlImageCompressedBuff    = NULL;
  unsigned long  mySqlImageCompressedBuffLen = 0;

  uchar* imageCompressedBuff = EncodeARasterImageIntoAThumbNail (image, 100, imageCompressedBuffLen);
  if  (imageCompressedBuff == NULL)
  {
    mySqlImageCompressedBuff = NULL;
    mySqlImageCompressedBuffLen = 0;
  }
  else
  {
    // In a worst case situation the escape string could be twice as long as the source string plus 5 bytes overhead.
    mySqlImageCompressedBuff = new uchar[imageCompressedBuffLen * 2 + 5];
    mySqlImageCompressedBuffLen = mysql_real_escape_string (conn, (char*)mySqlImageCompressedBuff, (char*)imageCompressedBuff, imageCompressedBuffLen);
  }

  sqlStr << "call ImagesUpdate("
         << dbImage.ImageId        ()               << ", "
         << dbImage.ImageFileName  ().QuotedStr ()  << ", "
         << dbImage.SipperFileName ().QuotedStr ()  << ", "
         << (kkint64)dbImage.ByteOffset ()            << ", "
         << dbImage.TopLeftRow           ()         << ", "
         << dbImage.TopLeftCol           ()         << ", "
         << dbImage.Height               ()         << ", "
         << dbImage.Width                ()         << ", "
         << dbImage.PixelCount           ()         << ", "
         << dbImage.ConnectedPixelDist   ()         << ", "
         << dbImage.ClassLogEntryId      ()         << ", "
         << dbImage.ExtractionLogEntryId ()         << ", "
         << dbImage.CentroidRow          ()         << ", "
         << dbImage.CentroidCol          ()         << ", "
         << class1Name.QuotedStr         ()         << ", "
         << dbImage.Class1Prob           ()         << ", "
         << class2Name.QuotedStr         ()         << ", "
         << dbImage.Class2Prob           ()         << ", "
         << validatedClassName.QuotedStr ()         << ", "
         << dbImage.Depth                ()         << ", ";

  if  (mySqlImageCompressedBuff == NULL)
  {
    sqlStr << "NULL";
  }
  else
  {
    sqlStr.Append ('"');
    sqlStr.Append ((char*)mySqlImageCompressedBuff, mySqlImageCompressedBuffLen);
    sqlStr.Append ('"');
  }

  sqlStr << ")";

  kkint32 returnCd = QueryStatement (sqlStr);
  bool  successful = (returnCd == 0);
  delete  mySqlImageCompressedBuff;
  mySqlImageCompressedBuff = NULL;
  
  delete  imageCompressedBuff;
  imageCompressedBuff = NULL;

  if  (successful)
  {
    ResultSetsClear ();
    sqlStr = "delete  from  ImagesFullSize  where  ImageId = " + StrFormatInt (dbImage.ImageId (), "ZZZZZZZZ0");
    kkint32  returnCd = QueryStatement (sqlStr);
    ResultSetsClear ();

    kkint32  maxDim = Max (dbImage.Height (), dbImage.Width ());
    if  (maxDim > 100)
    {
      // The image had to be reduced to a thumbnail size so we want to store a copy of the original in the ImagesFullSize table.
      ImageFullSizeSave (dbImage.ImageFileName (), image);
    }
  }
}  /* ImageUpdate */


void  DataBase::ImagesGetGpsData (const KKStr& imageRootName,
                                  double&      latitude,
                                  double&      longitude
                                 )
{
  latitude = 0.0;
  longitude = 0.0;

  KKStr  sqlCmd = "call ImagesGetGpsData(" + imageRootName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (sqlCmd);
  if  (returnCd != 0)
    return;

  char const *  fieldNames[] = {"Latitude", "Longitude", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return;

  MYSQL_ROW  row = NULL;
  while  (ResultSetFetchNextRow ())
  {
    latitude  = ResultSetGetDoubleField (0);
    longitude = ResultSetGetDoubleField (1);
  }

  ResulSetFree ();

  ResultSetsClear ();
}  /* ImagesGetGpsData */


void  DataBase::ImagesUpdateValidatedAndPredictClass (const KKStr&     imageFileName, 
                                                      MLClassPtr       mlClass, 
                                                      float            class1Prob
                                                     )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImagesUpdateValidatedAndPredictClass");
    return;
  }

  if  (!mlClass)
    return;

  KKStr  updateStr (512);
  updateStr << "call  ImagesUpdateValidatedClass(" << imageFileName.QuotedStr ()       << ", " 
            <<                                        mlClass->Name ().QuotedStr () << ", "
            <<                                        class1Prob                                                   
            <<                                 ")";
  kkint32  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}  /* ImagesUpdateValidatedAndPredictClass*/




void  DataBase::ImageUpdateInstrumentDataFields (InstrumentDataPtr  instumentData,
                                                 const KKStr&       sipperFileName,
                                                 kkuint32           scanLineStart,
                                                 kkuint32           scanLineEnd
                                                )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageUpdateInstrumentDataFields");
    return;
  }

  KKStr  updateStr (512);

  updateStr << "call ImagesUpdateInstrumentDataFields("
    <<      sipperFileName.QuotedStr ()    << ", "
    <<      scanLineStart                  << ", "
    <<      scanLineEnd                    << ", "
    <<      instumentData->FlowRate1    () << ", "
    <<      instumentData->FlowRate2    () << ", "
    <<      instumentData->Depth        () << ", "
    <<      instumentData->Salinity     () << ", "
    <<      instumentData->Oxygen       () << ", "
    <<      instumentData->Fluorescence () 
    <<    ")";
    
  kkint32  returnCd = QueryStatement (updateStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl 
                   << "DataBase::ImageUpdateInstrumentDataFields    ***ERROR***     occurred performing update." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
  }

  ResultSetsClear ();
}  /* ImageUpdateInstrumentDataFields */








void  DataBase::ImagesSizeDistributionByDepth (const KKStr&               cruiseName,
                                               const KKStr&               stationName,
                                               const KKStr&               deploymentNum,
                                               const KKStr&               className,
                                               float                      maxDepth,
                                               float                      depthBinSize,
                                               char                       statistic,
                                               double                     initialValue,
                                               double                     growthRate,
                                               double                     endValue,
                                               ImageSizeDistributionPtr&  downCast,
                                               ImageSizeDistributionPtr&  upCast
                                              )
{
  delete  downCast;   downCast = NULL;
  delete  upCast;     upCast   = NULL;


  VolumeSampledStatListPtr  volSampledUpCast = NULL;
  VolumeSampledStatListPtr  volSampledDownCast = NULL;

  InstrumentDataDeploymentVolumeSampled (cruiseName, stationName, deploymentNum, depthBinSize, volSampledDownCast, volSampledUpCast);

  KKStr  sqlStr (1024);

  KKStr  statisticStr;
  statisticStr.Append (statistic);

  sqlStr << "call ImagesSizeDistributionByDepth(" 
         <<               cruiseName.QuotedStr ()
         <<       ","  << stationName.QuotedStr ()
         <<       ","  << deploymentNum.QuotedStr ()
         <<       ","  << className.QuotedStr ()
         <<       ","  << maxDepth
         <<       ","  << depthBinSize
         <<       ","  << statisticStr.QuotedStr ()
         <<       ","  << initialValue
         <<       ","  << growthRate
         <<       ","  << endValue
         <<       ")";
  
  VectorKKStr  columnsReturned;

  KKStrMatrixPtr  results = QueryStatementReturnAllColumns (sqlStr.Str (), 
                                                            sqlStr.Len (),
                                                            columnsReturned
                                                           );
  if  (!results)
  {
    delete volSampledDownCast;  volSampledDownCast = NULL;
    delete volSampledUpCast;    volSampledUpCast   = NULL;
    return;
  }

  kkint32  downCastIdx        = -1;
  kkint32  bucketIdx          = -1;
  kkint32  bucketDepthIdx     = -1;
  kkint32  imageCountIdx      = -1;
  kkint32  totalPixelCountIdx = -1;
  kkint32  totalFilledAreaIdx = -1;
  kkint32  firstSizeBucketIdx = -1;

  VectorFloat  startValues;
  VectorFloat  endValues;

  float   lastEndValue = 0.0f;

  kkuint32  columnIdx = 0;
  while  (columnIdx < columnsReturned.size ())
  {
    const KKStr&  colName = columnsReturned[columnIdx];
    if  (colName.EqualIgnoreCase ("DownCast"))
    {
      downCastIdx = columnIdx;
    }

    else if  (colName.EqualIgnoreCase ("BucketIdx"))
    {
      bucketIdx = columnIdx;
    }

    else if  (colName.EqualIgnoreCase ("BucketDepth"))
    {
      bucketDepthIdx = columnIdx;
    }

    else if  (colName.EqualIgnoreCase ("ImageCount"))
    {
      imageCountIdx = columnIdx;
    }

    else if  (colName.EqualIgnoreCase ("TotalPixelCount"))
    {
      totalPixelCountIdx = columnIdx;
    }

    else if  (colName.EqualIgnoreCase ("TotalFilledArea"))
    {
      totalFilledAreaIdx = columnIdx;
    }

    else if  (colName[0] == '<')
    {
      firstSizeBucketIdx = columnIdx;
      lastEndValue = colName.SubStrPart (1).ToFloat ();
      startValues.push_back (0.0f);
    }

    else if  (colName.SubStrPart (0, 4) == "Size_")
    {
      lastEndValue = colName.SubStrPart (5).ToFloat ();
      endValues.push_back (lastEndValue);
      startValues.push_back (lastEndValue);
    }

    else if  (colName.SubStrPart (0, 1) == ">=")
    {
      lastEndValue = colName.SubStrPart (2).ToFloat ();
      endValues.push_back (lastEndValue);
      startValues.push_back (lastEndValue);
    }
    else
    {
      log.Level (-1) << endl
        << "DataBase::ImagesSizeDistributionByDepth   ***ERROR***   Unrecognized Column Idx:" << columnIdx << "  Name:" <<  colName.QuotedStr () << endl
        << endl;
    }
    ++columnIdx;
  }
  endValues.push_back (9999999.99f);

  kkuint32  sizeBucketCount = startValues.size ();

  kkint32  maxColIdx = Max(downCastIdx, bucketIdx, bucketDepthIdx, imageCountIdx, totalPixelCountIdx, totalFilledAreaIdx, firstSizeBucketIdx);

  bool  colDefAllThere = ((downCastIdx        >= 0) &&
                          (bucketIdx          >= 0) &&
                          (bucketDepthIdx     >= 0) &&
                          (imageCountIdx      >= 0) &&
                          (totalPixelCountIdx >= 0) &&
                          (totalFilledAreaIdx >= 0) &&
                          (firstSizeBucketIdx >= 0)
                         );
  if  (!colDefAllThere)
  {
    log.Level (-1) << endl 
        << "DataBase::ImagesSizeDistributionByDepth   ***ERROR***   Column definitions are missing." << endl
        << "downCastIdx        = " << downCastIdx         << endl
        << "bucketIdx          = " << bucketIdx           << endl
        << "bucketDepthIdx     = " << bucketDepthIdx      << endl
        << "imageCountIdx      = " << imageCountIdx       << endl
        << "totalPixelCountIdx = " << totalPixelCountIdx  << endl
        << "totalFilledAreaIdx = " << totalFilledAreaIdx  << endl
        << "firstSizeBucketIdx = " << firstSizeBucketIdx  << endl
        << endl;
  }
  else
  {
    for  (kkuint32  resultsIDX = 0;  resultsIDX < results->NumRows ();  ++resultsIDX)
    {
      KKStrList& row = (*results)[resultsIDX];
      bool  downCastRow = row[downCastIdx].ToBool ();

      ImageSizeDistributionPtr  sizeDistribution = NULL;

      float  volSampled = 0.0f;

      kkint32  totalPixelCount = 0;
      kkint32  totalFilledArea = 0;

      float  depth      = row[bucketDepthIdx].ToFloat ();
      kkint32  imageCount = row[imageCountIdx].ToInt32 ();

      kkint32  depthBinIdx = (kkint32)(depth / depthBinSize);

      if  (totalPixelCountIdx >= 0)
        totalPixelCount = row[totalPixelCountIdx].ToInt32 ();

      if  (totalFilledAreaIdx >= 0)
        totalFilledArea = row[totalFilledAreaIdx].ToInt32 ();

      VolumeSampledStatPtr  volStat = NULL;

      if  (downCastRow)
      {
        if (!downCast)
        {
          downCast = new ImageSizeDistribution ((float)depthBinSize, (float)initialValue, (float)growthRate, (float)endValue, startValues, endValues, log);
        }
        sizeDistribution = downCast;
        if  (volSampledDownCast)
          volStat = volSampledDownCast->Locate (depthBinIdx);
        else
          volStat = NULL;
      }
      else
      {
        if  (!upCast)
        {
          upCast = new ImageSizeDistribution (depthBinSize, (float)initialValue, (float)growthRate, (float)endValue, startValues, endValues, log);
        }
        sizeDistribution = upCast;
        if  (volSampledUpCast)
          volStat = volSampledUpCast->Locate (depthBinIdx);
        else
          volStat = NULL;
      }

      if  (volStat)
        volSampled = volStat->VolumeSampled ();

      sizeDistribution->DefineRow (depth, imageCount, totalPixelCount, totalFilledArea, volSampled);
      for  (kkuint32  zed = firstSizeBucketIdx, sizeIdx = 0;  zed < row.size ();  ++zed, ++sizeIdx)
      {
        sizeDistribution->AddData (depth, sizeIdx, row[zed].ToInt32 ());
      }
    }
  }

  delete  results;             results            = NULL;
  delete  volSampledUpCast;    volSampledUpCast   = NULL;
  delete  volSampledDownCast;  volSampledDownCast = NULL;

  return;
}  /* ImagesSizeDistributionByDepth */






//*************************************************************************************
//*                             Image Full SizeRoutines                               *
//*************************************************************************************
RasterSipperPtr  DataBase::ImageFullSizeLoad (const KKStr&  imageFileName)
{
  RasterSipperPtr  fullSizeImage = NULL;

  char const *  fieldNames[] = {"ImageId", "ImageFileName", "FullSizeImage", NULL};

  KKStr  selectStr = "call ImagesFullSizeLoad(" + osGetRootName (imageFileName).QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (selectStr);
  if (returnCd == 0)
  {
    ResultSetLoad (fieldNames);
    if  (ResultSetFetchNextRow ())
      fullSizeImage = ResultSetGetRasterField (2);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  fullSizeImage;
}  /* ImageFullSizeLoad */



void  DataBase::ImageFullSizeDelete (const KKStr&  imageFileName)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageFullSizeDelete");
    return;
  }

  KKStr  deleteStr = "call  ImagesFullSizeDelete(" + imageFileName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (deleteStr);
  if  (returnCd == 0)
  {
    ResultSetLoad (NULL);
  }

  ResulSetFree ();
  ResultSetsClear ();
}  /* ImageFullSizeDelete */




void   DataBase::ImageFullSizeSave (const KKStr&         imageFileName,
                                    const RasterSipper&  raster
                                   )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageFullSizeSave");
    return;
  }

  kkuint32  compressedBuffLen = 0;
  uchar*  compressedBuff = raster.ToCompressor (compressedBuffLen);
  if  (!compressedBuff)
  {
    log.Level (-1) << endl << endl << "DataBase::ImageFullSizeSave   ***ERROR***    Compressing RasterSipper  ImageFileName[" << imageFileName << "]" << endl << endl;
    return;
  }

  char*          queryStr  = NULL;
  kkint32        queryStrLen = 0;
  unsigned long  mySqlImageCompressedBuffLen = 0;

  kkint32  queryStrSize = compressedBuffLen * 2 + 300;
  kkint32  queryStrLeft = queryStrSize;
  queryStr = new char[queryStrSize];
  if  (!queryStr)
  {
    log.Level (-1) << endl << endl << "DataBase::ImageFullSizeSave   ***ERROR***    ImageFileName[" << imageFileName << "]  Could not allocate 'QueryStr'.  Length[" << queryStrSize << "]" << endl << endl;
    delete  compressedBuff;
    compressedBuff = NULL;
    return;
  }

  char*  queryStrPtr = queryStr;
  STRCOPY (queryStrPtr, queryStrLeft, "call ImagesFullSizeSave(");
  kkuint32  x = strlen (queryStrPtr);  queryStrPtr += x;   queryStrLeft -= x;

  STRCOPY (queryStrPtr, queryStrLeft, osGetRootName (imageFileName).QuotedStr ().Str ());
  x = strlen (queryStrPtr);  queryStrPtr += x;   queryStrLeft -= x;

  STRCOPY (queryStrPtr, queryStrLeft, ", \"");
  x = strlen (queryStrPtr);  queryStrPtr += x;   queryStrLeft -= x;
  queryStrLen = (kkint32)strlen (queryStr); 

  mySqlImageCompressedBuffLen = mysql_real_escape_string (conn, (char*)queryStrPtr, (char*)compressedBuff, compressedBuffLen);
  queryStrLen  += mySqlImageCompressedBuffLen;
  queryStrPtr  += mySqlImageCompressedBuffLen;
  queryStrLeft -= mySqlImageCompressedBuffLen;

  // We no longer need 'compressedBuff' so lets delete it.
  delete  compressedBuff;  compressedBuff = NULL;
  
  KKB::STRCAT (queryStrPtr, queryStrLeft, "\")");
  x = strlen (queryStrPtr);
  queryStrLen  += x;
  queryStrPtr  += x;
  queryStrLeft -= x;

  kkint32  returnCd = QueryStatement (queryStr, queryStrLen);
  if  (returnCd == 0)
  {
    kkint32  imageId = -1;
    ResultSetLoad (NULL);
    if  (resultSetMore)
    {
      if  (ResultSetFetchNextRow ())
        imageId = ResultSetGetIntField (0);
      ResulSetFree ();
    }
    ResultSetsClear ();
  }

  delete  queryStr; 
  queryStr = NULL;

  delete  compressedBuff;
  compressedBuff = NULL;
}  /* ImageFullSizeSave*/






RasterSipperPtr  DataBase::ImageFullSizeFind (const KKStr&  imageFileName)
{
  DataBaseImagePtr i = this->ImageLoad (imageFileName);

  if  (!i)
  {
    // Without this entry there is no way to locate the image.
    return NULL;
  }

  RasterSipperPtr  r = NULL;
  kkuint32  maxDim = Max (i->Height (), i->Width ());
  if  (maxDim <= 100)
  {
    try
    {r = i->ThumbNail (log);}
    catch (KKException& e1)     {throw KKException ("DataBase::ImageFullSizeFind  Exception while calling 'ThumbNail'  " + e1.ToString ());}
    catch (std::exception& e2)  {throw KKException ("DataBase::ImageFullSizeFind  std::exception while calling 'ThumbNail'.", e2);}
    catch (...)                 {throw KKException ("DataBase::ImageFullSizeFind  Exception while calling 'ThumbNail'.");}
  }

  if  (!r)
  {
    // Will now try the ImagesFullSize table
    try  {r = ImageFullSizeLoad (imageFileName);}
    catch (KKException& e1)  {throw KKException ("DataBase::ImageFullSizeFind  Exception while calling 'ImageFullSizeLoad'  " + e1.ToString ());}
    catch (std::exception& e2)  {throw KKException ("DataBase::ImageFullSizeFind  std::exception while calling 'ImageFullSizeLoad'.", e2);}
    catch (...)                 {throw KKException ("DataBase::ImageFullSizeFind  Exception while calling 'ImageFullSizeLoad'.");}
  }

  if  (!r)
  {
    // Will now try to go to original sipper file.
    try  {r = i->GetOrigImageFromSipperFile (log);}
    catch (KKException& e1)  {throw KKException ("DataBase::ImageFullSizeFind  Exception while calling 'GetOrigImageFromSipperFile'  " + e1.ToString ());}
    catch (std::exception& e2)  {throw KKException ("DataBase::ImageFullSizeFind  std::exception while calling 'GetOrigImageFromSipperFile'.", e2);}
    catch (...)                 {throw KKException ("DataBase::ImageFullSizeFind  Exception while calling 'GetOrigImageFromSipperFile'.");}

    if  (r != NULL)
    {
      // Sine we just went to the trouble to load construct the Full Size version of the image from the 
      // SIPPER file we should update the database so that next time we can retrieve without resorting to 
      // the SIPPER file.
      ImageFullSizeSave (imageFileName, *r);
    }
  }

  if  (r)
    r->FileName (imageFileName);

  delete  i;
  i = NULL;

  return  r;
}  /* ImageFullSizeFind */




//*************************************************************************************
//*                             Image Group Routines                                  *
//*************************************************************************************
void  DataBase::ImageGroupInsert (DataBaseImageGroup&  imageGroup)
{
  KKStr  insertStr (256);

  insertStr << "CALL ImageGroupInsert(" 
            <<                         imageGroup.Name       ().QuotedStr () << ", "
            <<                         imageGroup.Description().QuotedStr ()
            << ")";
    
  kkint32  returnCd = QueryStatement (insertStr.Str ());
  if  (returnCd != 0)
    return;

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return;

  if  (resultSetNumFields < 1)
  {
    lastMySqlErrorDesc = "ImageGroupId was not returned.";
    lastMySqlErrorNo   = -1;
  }
  else
  {
    while  (ResultSetFetchNextRow ())
      imageGroup.ImageGroupId (ResultSetGetIntField (0));
  }

  ResulSetFree ();
  ResultSetsClear ();

  return;
}  /* ImageGroupInsert */




DataBaseImageGroupPtr  DataBase::ImageGroupLoad (const KKStr&  imageGroupName)   // returns a list of all Group Assignments
{
  DataBaseImageGroupPtr  group = NULL;

  KKStr  selectCmd = "call ImageGroupLoad(" + imageGroupName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  char const *  fieldNames[] = {"ImageGroupId", "ImageGroupName", "Description", "GroupCount", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  kkint32  groupId = 0;
  KKStr  name    = "";
  KKStr  desc    = "";
  kkint32  count   = 0;
  while  (ResultSetFetchNextRow ())
  {
    if  (group)
    {
      delete  group;  group = NULL;
    }
    groupId = ResultSetGetIntField (0);
    name    = ResultSetGetField    (1);
    desc    = ResultSetGetField    (2);
    count   = ResultSetGetIntField (3);
    group = new DataBaseImageGroup (groupId, name, desc, count);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  group;
}  /* ImageGroupLoad */






DataBaseImageGroupListPtr  DataBase::ImageGroupLoad ()   // returns a list of all Group Assignments
{
  DataBaseImageGroupListPtr  groups = NULL;

  KKStr  selectCmd = "call ImageGroupLoadAll()";
  kkint32  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  groups;

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  if  (resultSetNumFields < 4)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ImageGroupLoad    ***ERROR***     Not enough fields returned." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }

  groups = new DataBaseImageGroupList (true);

  while  (ResultSetFetchNextRow ())
  {
    kkint32  groupId = ResultSetGetIntField (0);
    KKStr  name    = ResultSetGetField (1);
    KKStr  desc    = ResultSetGetField (2);
    kkuint32 count   = (kkuint32)ResultSetGetUlongField (3);
    groups->PushOnBack (new DataBaseImageGroup (groupId, name, desc, count));
  }

  ResulSetFree ();
  ResultSetsClear  ();
  return  groups;
}  /* ImageGroupLoad */




void  DataBase::ImageGroupDelete (kkint32 imageGroupId)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("ImageGroupDelete");
    return;
  }

  KKStr  deleteStr (1000);
  deleteStr << "CALL ImageGroupDelete(" << imageGroupId << ")";
    
  kkint32  returnCd = QueryStatement (deleteStr.Str ());
  if  (returnCd != 0)
    return;

  ResultSetsClear  ();

  return;
}  /* ImageGroupDelete */




//*************************************************************************************
//*                               ImageGroupEntries                                   *
//*************************************************************************************

void  DataBase::ImageGroupEntriesInsert (const DataBaseImageGroupEntry&  imageGroupEntry)
{
  KKStr  sqlStr (256);
  sqlStr << "call ImageGroupEntryInsert(" <<  imageGroupEntry.ImageGroupId () << ", " << imageGroupEntry.ImageFileName ().QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (sqlStr.Str ());

  return;
}  /* ImageGroupEntriesInsert */





void  DataBase::ImageGroupEntriesInsert (const DataBaseImageGroupEntryList&  imageGroupList)
{
  DataBaseImageGroupEntryList::const_iterator idx;
  for  (idx = imageGroupList.begin ();   idx != imageGroupList.end ();  idx++)
  {
    DataBaseImageGroupEntryPtr ie = *idx;
    ImageGroupEntriesInsert (*ie);
  }
}  /* ImageGroupEntriesInsert */





KKStrMatrixPtr  DataBase::ImageGroupEntriesInsert (kkint32             groupId,
                                                   const VectorKKStr&  imageFileNames
                                                  )
{
  kkint32  x = 0;
  kkint32  maxParmLen = 60000;  // The stored procedure parameter is limited to 60,000 characters.
  kkint32  sqlStrLen = Min (maxParmLen, (kkint32)(49 * imageFileNames.size ()));  
  sqlStrLen = Max (sqlStrLen, 100);
  
  KKStr  sqlStr (sqlStrLen);

  sqlStr = "";
  sqlStr << "call  ImageGroupEntryInsertList(" << groupId << ", \"";

  kkuint32  zed = 0;

  while  (zed < imageFileNames.size ())
  {
    if  (zed > 0)
      sqlStr << "\t";
    sqlStr << imageFileNames[zed];
    zed++;
  }
  sqlStr << "\")";

  if  (zed < imageFileNames.size ())
  {
    log.Level (-1) << endl
                   << "DataBase::ImageGroupEntriesInsert    ***ERROR***" << endl
                   << endl
                   << "      Parameter for ImageFileNames was to large." << endl
                   << endl;
    return  NULL;
  }

  KKStrMatrixPtr results = new KKStrMatrix (2);  // 2 Columns

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
  {
    // Since we failed to process anything we will return 'NULL' indicating total failure.
    delete  results;
    return  NULL;
  }
  
  // Process results of procedure call.
  char const *  fieldNames[] = {"ImageId", "ImageFileName", "Successful", "ErrorDesc", NULL};
  ResultSetLoad (fieldNames);
  while  (ResultSetFetchNextRow ())
  {
    bool  successful    = ResultSetGetBool (2);
    if  (!successful)
    {
      KKStrPtr  imageFileName = new KKStr (ResultSetGetKKStrField (1));
      KKStrPtr  errorDesc     = new KKStr (ResultSetGetKKStrField (3));
      KKStrListPtr  row = new KKStrList (true);
      row->push_back (imageFileName);
      row->push_back (errorDesc);
      results->AddRow (row);
    }
  }

  ResulSetFree ();
  ResultSetsClear  ();

  return  results;
}  /* ImageGroupEntriesInsert */





DataBaseImageGroupEntryListPtr  DataBase::ImageGroupEntriesLoad (kkint32 groupId)
{
  KKStr  selectCmd (200);

  selectCmd << "select  ImageFileName  from ImageGroupEntries  where  ImageGroupId = " << groupId << endl;

  kkint32  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  MYSQL_RES*  resSet;
  resSet = mysql_use_result (conn); /* generate result set */
  if  (resSet == NULL)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ImageGroupEntriesLoad    ***ERROR***     occurred when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }

  DataBaseImageGroupEntryListPtr  entries = new DataBaseImageGroupEntryList (true);
  kkint32  numFields  = mysql_num_fields (resSet);
  if  (numFields < 1)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ImageGroupEntriesLoad    ***ERROR***     Not enough columns ." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }

  MYSQL_ROW  row;
  while  (row = mysql_fetch_row (resSet))
  {
    KKStr  imageFileName = row[0];
    DataBaseImageGroupEntryPtr  entry = new DataBaseImageGroupEntry (groupId, imageFileName);
    entries->PushOnBack (entry);
  }

  mysql_free_result (resSet);

  return  entries;
}  /* ImageGroupEntriesLoad */






//*************************************************************************************
//*                           Sipper Deployment Routines                              *
//*************************************************************************************





SipperDeploymentListPtr  DataBase::SipperDeploymentProcessResults ()
{
  char const *  fieldNames[] 
                      = {"CruiseName",        "StationName",       "DeploymentNum",
                         "Description",       "DateTimeStart",     "DateTimeEnd",
                         "Latitude",          "Longitude",         "SyncTimeStampActual",
                         "SyncTimeStampCTD",  "SyncTimeStampGPS",  "CropLeft",
                         "CropRight",         "ChamberWidth",
                         NULL
                        };
  
  if  (!ResultSetLoad (fieldNames))
    return NULL;

  SipperDeploymentListPtr  results = new SipperDeploymentList (true);

  while  (ResultSetFetchNextRow ())
  {
    SipperDeploymentPtr  d = new SipperDeployment ();
    d->CruiseName          (ResultSetGetField (0));
    d->StationName         (ResultSetGetField (1));
    d->DeploymentNum       (ResultSetGetField (2));
    d->Description         (ResultSetGetField (3));
    d->DateTimeStart       (ResultSetGetDateTimeField  (4));
    d->DateTimeEnd         (ResultSetGetDateTimeField  (5));
    d->Latitude            (ResultSetGetDoubleField    (6));
    d->Longitude           (ResultSetGetDoubleField    (7));
    d->SyncTimeStampActual (ResultSetGetDateTimeField  (8));
    d->SyncTimeStampCTD    (ResultSetGetDateTimeField  (9));
    d->SyncTimeStampGPS    (ResultSetGetDateTimeField (10));
    d->CropLeft            (ResultSetGetUintField     (11));
    d->CropRight           (ResultSetGetUintField     (12));
    d->ChamberWidth        (ResultSetGetFloatField    (13));

    if  ((d->SyncTimeStampActual ().Year () < 1)  &&
         (d->SyncTimeStampCTD    ().Year () < 1)  &&
         (d->SyncTimeStampGPS    ().Year () < 1)
        )
    {
      d->SyncTimeStampActual (d->DateTimeStart ());
      d->SyncTimeStampCTD    (d->DateTimeStart ());
      d->SyncTimeStampGPS    (d->DateTimeStart ());
    }

    results->PushOnBack (d);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* SipperDeploymentProcessResults */





SipperDeploymentListPtr  DataBase::SipperDeploymentLoad (const KKStr& cruiseName,
                                                         const KKStr& stationName
                                                        )
{
  KKStr  sqlStr = "call DeploymentLoadList(" + cruiseName.QuotedStr () + ", " + stationName.QuotedStr () + ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  return  SipperDeploymentProcessResults ();
}  /* SipperDeploymentLoad */




SipperDeploymentPtr    DataBase::SipperDeploymentLoad (const KKStr&  cruiseName,
                                                       const KKStr&  stationName,
                                                       const KKStr&  deploymentNum
                                                      )
{
  KKStr  sqlStr = "call DeploymentLoad(" + cruiseName.QuotedStr () + ", " + stationName.QuotedStr () + ", " + deploymentNum.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  SipperDeploymentListPtr  results = SipperDeploymentProcessResults ();
  if  (results == NULL)
    return NULL;
  SipperDeploymentPtr  result = results->PopFromBack ();
  delete  results;
  results = NULL;
  return result;
}  /* SipperDeploymentLoad*/






SipperDeploymentListPtr  DataBase::DeploymentLoadByStation (const KKStr& stationName)
{
  KKStr  sqlStr = "call DeploymentLoadByStation(" + stationName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;
  SipperDeploymentListPtr  results = SipperDeploymentProcessResults ();
  return results;
}  /* DeploymentLoadByStation */





void   DataBase::SipperDeploymentInsert (const SipperDeployment&  deployment)
{
  KKStr  sqlStr = "call DeploymentInsert(" + 
                        deployment.CruiseName    ().QuotedStr ()                + ", " +
                        deployment.StationName   ().QuotedStr ()                + ", " +
                        deployment.DeploymentNum ().QuotedStr ()                + ", " +
                        deployment.Description   ().QuotedStr ()                + ", " +
                        DateTimeToQuotedStr (deployment.DateTimeStart ())       + ", " + 
                        DateTimeToQuotedStr (deployment.DateTimeEnd   ())       + ", " + 
                        StrFormatDouble (deployment.Latitude  (), "##0.000000") + ", " +
                        StrFormatDouble (deployment.Longitude (), "##0.000000") + ", " +
                        DateTimeToQuotedStr (deployment.SyncTimeStampActual ()) + ", " +
                        DateTimeToQuotedStr (deployment.SyncTimeStampCTD    ()) + ", " +
                        DateTimeToQuotedStr (deployment.SyncTimeStampGPS    ()) + ", " +
                        StrFormatInt (deployment.CropLeft  (), "####0")         + ", " +
                        StrFormatInt (deployment.CropRight (), "####0")         + ", " +
                        StrFormatDouble (deployment.ChamberWidth (), "##0.0000") + 
                       ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  ResultSetsClear ();
}  /* SipperDeploymentInsert */



void   DataBase::SipperDeploymentUpdate (const SipperDeployment&  deployment)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperDeploymentUpdate");
    return;
  }

  KKStr  sqlStr = "call DeploymentUpdate(" + 
                        deployment.CruiseName    ().QuotedStr ()                + ", " +
                        deployment.StationName   ().QuotedStr ()                + ", " +
                        deployment.DeploymentNum ().QuotedStr ()                + ", " +
                        deployment.Description   ().QuotedStr ()                + ", " +
                        DateTimeToQuotedStr (deployment.DateTimeStart ())       + ", " + 
                        DateTimeToQuotedStr (deployment.DateTimeEnd   ())       + ", " + 
                        StrFormatDouble (deployment.Latitude  (), "##0.000000") + ", " +
                        StrFormatDouble (deployment.Longitude (), "##0.000000") + ", " +
                        DateTimeToQuotedStr (deployment.SyncTimeStampActual ()) + ", " +
                        DateTimeToQuotedStr (deployment.SyncTimeStampCTD    ()) + ", " +
                        DateTimeToQuotedStr (deployment.SyncTimeStampGPS    ()) + ", " +
                        StrFormatInt (deployment.CropLeft  (), "####0")         + ", " +
                        StrFormatInt (deployment.CropRight (), "####0")         + ", " + 
                        StrFormatDouble (deployment.ChamberWidth (), "##0.0000") + 
                       ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  ResultSetsClear ();
}  /* SipperDeploymentUpdate */




void   DataBase::SipperDeploymentDelete (const KKStr&  cruiseName,
                                         const KKStr&  stationName,
                                         const KKStr&  deploymentNum
                                        )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperDeploymentDelete");
    return;
  }

  KKStr  sqlStr = "call DeploymentDelete(" + 
                        cruiseName.QuotedStr    ()  + ", " +
                        stationName.QuotedStr   ()  + ", " +
                        deploymentNum.QuotedStr ()  +
                       ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  ResultSetsClear ();
}  /* SipperDeploymentDelete */







//*******************************************************************************************
//*                                SipperFile Routines                                      *
//*******************************************************************************************


char const *  DataBase::sipperFileFieldNames[] = 
                       {"SipperFileId",
                        "SipperFileName", "CruiseName",     "StationName",    "DeploymentNum",
                        "Description",    "Latitude",       "Longitude",      "DateTimeStart",
                        "SP0Instrument",  "SP1Instrument",  "SP2Instrument",  "SP3Instrument",
                        "CtdExt0",        "CtdExt1",        "CtdExt2",        "CtdExt3",
                        "SizeInBytes",    "NumScanLines",   "ScanRate",       "Depth",
                        "ExtractionStatus",        // 21
                        "ExtractionScanLineStart", // 22
                        "ExtractionScanLineEnd",   // 23
                        NULL
                       };


SipperFileListPtr  DataBase::SipperFileProcessResults ()
{
  ResultSetLoad (sipperFileFieldNames);
  if  (!resultSetMore)
    return NULL;

  SipperFileListPtr  results = new SipperFileList (true);
  while (ResultSetFetchNextRow ())
  {
    SipperFilePtr  sf = new SipperFile (ResultSetGetField (1));
    sf->SipperFileId            (ResultSetGetIntField (0));
    sf->CruiseName              (ResultSetGetField (2));
    sf->StationName             (ResultSetGetField (3));
    sf->DeploymentNum           (ResultSetGetField (4));
    sf->Description             (ResultSetGetField (5));
    sf->Latitude                (ResultSetGetDoubleField (6));
    sf->Longitude               (ResultSetGetDoubleField (7));
    sf->DateTimeStart           (DateTimeFromMySqlDateTimeField (ResultSetGetField (8))); 
    sf->Sp0                     (Instrument::LookUpByShortName  (ResultSetGetField (9))); 
    sf->Sp1                     (Instrument::LookUpByShortName  (ResultSetGetField (10))); 
    sf->Sp2                     (Instrument::LookUpByShortName  (ResultSetGetField (11)));
    sf->Sp3                     (Instrument::LookUpByShortName  (ResultSetGetField (12)));
    sf->CtdExt0                 (ResultSetGetField       (13));
    sf->CtdExt1                 (ResultSetGetField       (14));
    sf->CtdExt2                 (ResultSetGetField       (15));
    sf->CtdExt3                 (ResultSetGetField       (16));
    sf->SizeInBytes             (ResultSetGetInt64Field  (17));
    sf->NumScanLines            (ResultSetGetIntField    (18));
    sf->ScanRate                (ResultSetGetFloatField  (19));
    sf->Depth                   (ResultSetGetFloatField  (20));
    sf->ExtractionStatus        (ResultSetGetCharField   (21));
    sf->ExtractionScanLineStart (ResultSetGetUintField   (22));
    sf->ExtractionScanLineEnd   (ResultSetGetUintField   (23));
    
    results->PushOnBack (sf);
  }

  ResulSetFree ();
  return  results;
}  /* SipperFileProcessResults */



void  DataBase::SipperFileLoadOracle ()
{
  SipperFileListPtr  sipperFiles = SipperFileLoad ("", "", "");
  SipperFileOracle::AddSipperFileEntries (sipperFiles);
  sipperFiles->Owner (false);
  delete  sipperFiles;
}



SipperFilePtr  DataBase::SipperFileRecLoad (const KKStr&  _sipperFileName)
{
  KKStr sipperFileName = osGetRootName (_sipperFileName);

  KKStr  queryStr = "select *  from  SipperFiles  " 
                    "where SipperFileName = " + sipperFileName.QuotedStr ();

  kkint32  returnCd = QueryStatement (queryStr);
  if  (returnCd != 0)
    return  NULL;

  SipperFileListPtr  results = SipperFileProcessResults ();
  if  (results == NULL)
    return NULL;

  SipperFilePtr  result = results->PopFromBack ();
  delete  results;
  results = NULL;
  ResultSetsClear ();
  return  result;
}  /* SiperFileRecLoad */



void  DataBase::SipperFilesUpdateExtractionStatus (const KKStr&  sipperFileName,
                                                   char          extractionStatus
                                                  )
{
  KKStr sqlStr (128);

  KKStr rootName = osGetRootName (sipperFileName);

  sqlStr << "call SipperFilesUpdateExtractionStatus(" << rootName.QuotedStr () << ", \"" << extractionStatus << "\")";
  kkint32  returnCd = QueryStatement (sqlStr);
}  /* SipperFilesUpdateExtractionStatus */




void  DataBase::SipperFilesUpdateExtractionStatusIfExpected (const KKStr&  sipperFileName,
                                                             char          extractionStatusExpected,
                                                             char          extractionStatusNew,
                                                             char&         ExtractionStatusBeforeUpdate,
                                                             char&         extractionStatusResult,
                                                             bool&         extrtactionSatusUpdated
                                                            )
{
  KKStr  sqlStr (128);

  extractionStatusResult = 0;
  
  sqlStr << "Call SipperFilesUpdateExtractionStatusIfExpected"
         << "(" 
               << osGetRootName (sipperFileName).QuotedStr ()  << ", "
               << "\"" << extractionStatusExpected << "\""     << ", "
               << "\"" << extractionStatusNew << "\""
         << ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
  {
    const char*   returnFields[] = {"SipperFileName", "ExtractionStatusBeforeUpdate", "ExtractionStatusResult", "ExtrtactionSatusUpdated", NULL};
    ResultSetLoad (returnFields);
    while  (ResultSetFetchNextRow ())
    {
      ExtractionStatusBeforeUpdate = ResultSetGetCharField (1);
      extractionStatusResult       = ResultSetGetCharField (2);
      extrtactionSatusUpdated      = ResultSetGetBool      (3);
    }
    ResulSetFree ();
    ResultSetsClear ();
  }

  return;
}  /* SipperFilesUpdateExtractionStatusIfExpected*/










VectorKKStr*  DataBase::SipperFileGetList (const KKStr& cruiseName,
                                           const KKStr& stationName,
                                           const KKStr& deploymentNum
                                          )
{
  KKStr  selectCmd (4096);

  selectCmd << "select  sf.SipperFileName  from  SipperFiles  sf ";

  VectorKKStr  conditions;

  if  (!cruiseName.Empty ())
    conditions.push_back ("CruiseName = " + cruiseName.QuotedStr ());

  if  (!stationName.Empty ())
    conditions.push_back ("StationName = " + stationName.QuotedStr ());

  if  (!deploymentNum.Empty ())
    conditions.push_back ("DeploymentNum = " + deploymentNum.QuotedStr ());

  if  (conditions.size () > 0)
  {
    selectCmd << "  where  ";
    for  (kkuint32 x = 0;  x < conditions.size ();  x++)
    {
      if  (x > 0)
        selectCmd << "  and  ";
      selectCmd << conditions[x];
    }
  }

  selectCmd << "    order by CruiseName, StationName, DeploymentNum, DateTimeStart";

  kkint32  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;


  MYSQL_RES*  resSet;
  resSet = mysql_use_result (conn);
  if  (resSet == NULL)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::SipperFileGetList    ***ERROR***     occurred when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }


  VectorKKStr*  sipperFiles = new VectorKKStr ();

  MYSQL_ROW  row;
  while  (row = mysql_fetch_row (resSet))
  {
    KKStr  sipperFileName (row[0]);
    sipperFiles->push_back (sipperFileName);
  }

  mysql_free_result (resSet);

  return  sipperFiles;
}  /* SipperFileGetList */





SipperFileListPtr  DataBase::SipperFileLoad (const KKStr& cruiseName,
                                             const KKStr& stationName,
                                             const KKStr& deploymentNum
                                            )
{
  KKStr  selectCmd (200);

  selectCmd << "select * from  SipperFiles  "  << endl;

  if  (!cruiseName.Empty ())
  {
    selectCmd << "  where  CruiseName = " << cruiseName.QuotedStr ();

    if  (!stationName.Empty ())
    {
      selectCmd  << "  and  StationName = " << stationName.QuotedStr ();

      if  (!deploymentNum.Empty ())
      {
        selectCmd  << "  and  DeploymentNum = " << deploymentNum.QuotedStr ();
      }
    }
  }

  selectCmd << "  order by CruiseName, StationName, DeploymentNum, DateTimeStart";

  kkint32  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  return  SipperFileProcessResults ();
}  /* SipperFileLoad */






void  DataBase::SipperFileInsert (SipperFile&  sipperFile)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperFileInsert");
    return;
  }

  KKStr  insertStr (1000);
  KKStr  extractionStatusStr;
  extractionStatusStr.Append (sipperFile.ExtractionStatus ());

  insertStr <<  "insert into  SipperFiles (SipperFileName, CruiseName, StationName, DeploymentNum, Description, Latitude, "      
                                           "Longitude, DateTimeStart, SP0Instrument, SP1Instrument, SP2Instrument, SP3Instrument, "
                                           "CtdExt0, CtdExt1, CtdExt2, CtdExt3, SizeInBytes, NumScanLines, Scanrate, Depth, "
                                           "ExtractionStatus, ExtractionScanLineStart, ExtractionScanLineEnd"
                                         ")" << endl
            <<      "values(" 
            <<         sipperFile.SipperFileName ().QuotedStr ()          << ", "
            <<         sipperFile.CruiseName     ().QuotedStr ()          << ", "
            <<         sipperFile.StationName    ().QuotedStr ()          << ", "
            <<         sipperFile.DeploymentNum  ().QuotedStr ()          << ", "
            <<         sipperFile.Description    ().QuotedStr ()          << ", "
            <<         sipperFile.Latitude       ()                       << ", "
            <<         sipperFile.Longitude      ()                       << ", "
            <<         DateTimeToQuotedStr (sipperFile.DateTimeStart  ()) << ", "
  
            <<         sipperFile.Sp0ShortName   ().QuotedStr ()          << ", "
            <<         sipperFile.Sp1ShortName   ().QuotedStr ()          << ", "
            <<         sipperFile.Sp2ShortName   ().QuotedStr ()          << ", "
            <<         sipperFile.Sp3ShortName   ().QuotedStr ()          << ", "

            <<         sipperFile.CtdExt0        ().QuotedStr ()          << ", "
            <<         sipperFile.CtdExt1        ().QuotedStr ()          << ", "
            <<         sipperFile.CtdExt2        ().QuotedStr ()          << ", "
            <<         sipperFile.CtdExt3        ().QuotedStr ()          << ", "
   
            <<         sipperFile.SizeInBytes    ()                       << ", "
            <<         sipperFile.NumScanLines   ()                       << ", "
            <<         sipperFile.ScanRate       ()                       << ", "
            <<         sipperFile.Depth          ()                       << ", "

            <<         extractionStatusStr.QuotedStr ()                   << ", "
            <<         sipperFile.ExtractionScanLineStart ()              << ", "
            <<         sipperFile.ExtractionScanLineEnd   ()
            <<       ")";

  kkint32  returnCd = QueryStatement (insertStr.Str ());
  if  (returnCd == 0)
     sipperFile.SipperFileId ((kkint32)mysql_insert_id (conn));

  return;
}  /* SiperFileInsert */





void   DataBase::SipperFileUpdate (SipperFile&   sipperFile)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperFileUpdate");
    return;
  }

  KKStr  updateStr(1024);

  KKStr  extractionStatusStr;
  extractionStatusStr.Append (sipperFile.ExtractionStatus ());

  updateStr << "update  SipperFiles sf"  << endl
            << "  set  CruiseName = "     << sipperFile.CruiseName    ().QuotedStr ()          << ", " << endl
            << "       StationName = "    << sipperFile.StationName   ().QuotedStr ()          << ", " << endl
            << "       DeploymentNum = "  << sipperFile.DeploymentNum ().QuotedStr ()          << ", " << endl
            << "       Description = "    << sipperFile.Description   ().QuotedStr ()          << ", " << endl
            << "       Latitude = "       << sipperFile.Latitude      ()                       << ", " << endl
            << "       Longitude = "      << sipperFile.Longitude     ()                       << ", " << endl
            << "       DateTimeStart = "  << DateTimeToQuotedStr (sipperFile.DateTimeStart ()) << ", " << endl
            << "       SP0Instrument = "  << sipperFile.Sp0ShortName  ().QuotedStr ()          << ", " << endl
            << "       SP1Instrument = "  << sipperFile.Sp1ShortName  ().QuotedStr ()          << ", " << endl
            << "       SP2Instrument = "  << sipperFile.Sp2ShortName  ().QuotedStr ()          << ", " << endl
            << "       SP3Instrument = "  << sipperFile.Sp3ShortName  ().QuotedStr ()          << ", " << endl
            << "       CtdExt0 = "        << sipperFile.CtdExt0       ().QuotedStr ()          << ", " << endl
            << "       CtdExt1 = "        << sipperFile.CtdExt1       ().QuotedStr ()          << ", " << endl
            << "       CtdExt2 = "        << sipperFile.CtdExt2       ().QuotedStr ()          << ", " << endl
            << "       CtdExt3 = "        << sipperFile.CtdExt3       ().QuotedStr ()          << ", " << endl
            << "       SizeInBytes = "    << sipperFile.SizeInBytes   ()                       << ", " << endl
            << "       NumScanLines = "   << sipperFile.NumScanLines  ()                       << ", " << endl
            << "       ScanRate = "       << sipperFile.ScanRate      ()                       << ", " << endl
            << "       Depth = "          << sipperFile.Depth         ()                       << ", " << endl
            << "       ExtractionStatus = "        << extractionStatusStr.QuotedStr ()         << ", " << endl
            << "       ExtractionScanLineStart = " << sipperFile.ExtractionScanLineStart ()    << ", " << endl
            << "       ExtractionScanLineEnd   = " << sipperFile.ExtractionScanLineEnd   ()
            << "  where  sf.SipperFileName = " << sipperFile.SipperFileName ().QuotedStr () << endl;

  kkint32  returnCd = QueryStatement (updateStr.Str ());
}  /* SipperFileUpdate */





void   DataBase::SipperFileUpdateFileSizeStats (const KKStr&  _sipperFileName, 
                                                kkint64       _sizeInBytes,
                                                kkuint32      _numScanLines
                                               )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperFileUpdateFileSizeStats");
    return;
  }

  KKStr  updateStr (512);
  
  updateStr << "update  SipperFiles  set SizeInBytes = " << _sizeInBytes << ", NumScanLines = " << _numScanLines << endl 
            << "        where SipperFileName = " << _sipperFileName.QuotedStr ();

  kkint32  returnCd = QueryStatement (updateStr.Str ());
}  /* SipperFileUpdateFileSizeStats */





void  DataBase::SipperFilesGetCTDDateTime (const KKStr&  _sipperFileName,
                                           kkint32&      _sipperFileId, 
                                           kkuint32&     _numScanLines, 
                                           DateTime&     _CTDDateTimeStart, 
                                           DateTime&     _CTDDateTimeEnd
                                          )
{
  _sipperFileId = -1;

  char const *  fieldNames[] =
  {"sipperFileId", "sipperFileName", "numScanLines", "CTDDateTimeStart", "CTDDateTimeEnd", NULL};

  KKStr  sqlStr (256);
  sqlStr << "call  SipperFilesGetCTDDateTime("  << _sipperFileName.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (sqlStr.Str ());

  if  (returnCd == 0)
  {
    ResultSetLoad (fieldNames);
    if  (ResultSetFetchNextRow ())   
    {
      _sipperFileId     = ResultSetGetIntField (0);
      _numScanLines     = ResultSetGetIntField (2);
      _CTDDateTimeStart = DateTimeFromMySqlDateTimeField (ResultSetGetField (3));
      _CTDDateTimeEnd   = DateTimeFromMySqlDateTimeField (ResultSetGetField (4));
    }
    ResulSetFree ();
    ResultSetsClear ();
  }

  return;
}  /* SipperFilesGetCTDDateTime */




/**
 *@brief  Delete the contents of a single SIPPER file from the database from all tables such as Images, FeatureData, InstrumentData, etc.
 *@param[in]  _sipperFileName  Name of SIPPER file to delete.
 */
void   DataBase::SipperFilesDelete (const KKStr&  _sipperFileName)
{
  KKStr  updateStr (512);
  updateStr << "call  SipperFilesDelete (" << _sipperFileName.QuotedStr () << ")";
  kkint32  returnCd = QueryStatement (updateStr.Str ());
}



//*******************************************************************************************
//*                            SipperStation  Routines                                      *
//*******************************************************************************************
SipperStationListPtr  DataBase::SipperStationProcessResults ()
{
  char const *  fieldNames[] = {"CruiseName", "StationName", "Description", "Latitude", "Longitude", "DateTimeStart", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return NULL;

  SipperStationListPtr  results = new SipperStationList (true);
  while  (ResultSetFetchNextRow ())
  {
    SipperStationPtr s = new SipperStation ();
    s->CruiseName    (ResultSetGetField         (0));
    s->StationName   (ResultSetGetField         (1));
    s->Description   (ResultSetGetField         (2));
    s->Latitude      (ResultSetGetDoubleField   (3));
    s->Longitude     (ResultSetGetDoubleField   (4));
    s->DateTimeStart (ResultSetGetDateTimeField (5));

    results->PushOnBack (s);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* SipperStationProcessResults */


 


SipperStationListPtr  DataBase::SipperStationLoad (const KKStr&  cruiseName)
{
  KKStr  selectCmd (200);

  selectCmd << "select   * from Stations" << endl;
  if  (!cruiseName.Empty ())
    selectCmd << "     where CruiseName = " << cruiseName.QuotedStr () << endl;

  kkint32  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  return  SipperStationProcessResults ();
}  /* SipperStationLoad */





SipperStationPtr  DataBase::SipperStationLoad (const KKStr&  cruiseName,
                                               const KKStr&  stationName
                                              )
{
  KKStr  sqlStr (256);

  sqlStr << "select * from Stations s" << endl
         << "      where s.CruiseName  = " << cruiseName.QuotedStr  () << "  and  " << endl
         << "            s.StationName = " << stationName.QuotedStr () << endl;

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  SipperStationListPtr  results = SipperStationProcessResults ();
  if  (results == NULL)
    return NULL;

  SipperStationPtr  sipperStation = NULL;

  if  (results->QueueSize () > 0)
    sipperStation = results->PopFromBack ();

  delete  results;
 
  return  sipperStation;
}  /* SipperStationLoad */




SipperStationListPtr  DataBase::SipperStationsLoadByGpsRange (double  latitudeMin,
                                                              double  latitudeMax,
                                                              double  longitudeMin,
                                                              double  longitudeMax
                                                             )
{
  KKStr  sqlCmd (200);

  sqlCmd << "call  StationsLoadByGpsRange(" << latitudeMin << ", " << latitudeMax << ", " << longitudeMin << ", " << longitudeMax << ")";
  kkint32  returnCd = QueryStatement (sqlCmd);
  if  (returnCd != 0)
    return  NULL;

  return  SipperStationProcessResults ();
}  /* SipperStationsLoadByGpsRange */





void   DataBase::SipperStationInsert (const SipperStation&  station)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperStationInsert");
    return;
  }

  KKStr  sqlStr (256);
  sqlStr << "call StationsInsert(" + 
                  station.CruiseName    ().QuotedStr ()  << ", " <<
                  station.StationName   ().QuotedStr ()  << ", " <<
                  station.Description   ().QuotedStr ()  << ", " <<
                  station.Latitude      ()               << ", " <<
                  station.Longitude     ()               << ", " <<
                  DateTimeToQuotedStr (station.DateTimeStart ()) << 
                ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
    ResultSetsClear ();
}  /* SipperStationInsert */




void  DataBase::SipperStationUpdate (const SipperStation&  station)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperStationUpdate");
    return;
  }

  KKStr sqlStr(255);
  sqlStr << "call StationsUpdate(" + 
                  station.CruiseName    ().QuotedStr ()  << ", " <<
                  station.StationName   ().QuotedStr ()  << ", " <<
                  station.Description   ().QuotedStr ()  << ", " <<
                  station.Latitude      ()               << ", " <<
                  station.Longitude     ()               << ", " <<
                  DateTimeToQuotedStr (station.DateTimeStart ()) << 
                ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
    ResultSetsClear ();
}  /* SipperStationUpdate */



void  DataBase::SipperStationDelete (const KKStr&  cruiseName,
                                     const KKStr&  stationName
                                   )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperStationDelete");
    return;
  }

  KKStr sqlStr = "call StationsDelete(" + cruiseName.QuotedStr  () + ", " + stationName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
    ResultSetsClear ();
}




InstrumentDataListPtr  DataBase::InstrumentDataProcessResults (const bool&  cancelFlag)

{
  char const *  instrumentDataFieldNames[] 
                                    = {"SipperFileId",        // 0
                                       "SipperFileName",      // 1
                                       "ScanLine",            // 2
                                       "ByteOffset",          // 3
                                       "FlowRate1",           // 4
                                       "FlowRate2",           // 5
                                       "Latitude",            // 6
                                       "Longitude",           // 7
                                       "CTDBattery",          // 8
                                       "Conductivity",        // 9
                                       "Density",             // 10
                                       "Depth",               // 11
                                       "CTDDateTime",         // 12
                                       "Fluorescence",        // 13
                                       "FluorescenceSensor",  // 14
                                       "Oxygen",              // 15
                                       "OxygenSensor",        // 16
                                       "Pressure",            // 17
                                       "RecordRate",          // 18
                                       "Salinity",            // 19
                                       "SoundVelocity",       // 20
                                       "Temperature",         // 21
                                       "Transmisivity",       // 22
                                       "TransmisivitySensor", // 23
                                       "Turbidity",           // 24
                                       "TurbiditySensor",     // 25
                                       "Pitch",               // 26
                                       "Roll",                // 27
                                       "CropLeft",            // 28
                                       "CropRight",           // 29
                                       "ActiveColumns",       // 30
                                       NULL,
                                      };

  ResultSetLoad (instrumentDataFieldNames);
  if  (!resultSetMore)
    return NULL;

  InstrumentDataListPtr  results = new InstrumentDataList (true);

  while  (ResultSetFetchNextRow ()  &&  (!cancelFlag))
  {
    InstrumentDataPtr  id = new InstrumentData ();
    id->ScanLine            (ResultSetGetUintField     ( 2));
    id->ByteOffset          (ResultSetGetUint64Field   ( 3));
    id->FlowRate1           (ResultSetGetFloatField    ( 4));
    id->FlowRate2           (ResultSetGetFloatField    ( 5));
    id->Latitude            (ResultSetGetDoubleField   ( 6));
    id->Longitude           (ResultSetGetDoubleField   ( 7));
    id->CTDBattery          (ResultSetGetFloatField    ( 8));
    id->Conductivity        (ResultSetGetFloatField    ( 9));
    id->Density             (ResultSetGetFloatField    (10));
    id->Depth               (ResultSetGetFloatField    (11));
    id->CtdDate             (ResultSetGetDateTimeField (12));
    id->Fluorescence        (ResultSetGetFloatField    (13));
    id->FluorescenceSensor  (ResultSetGetFloatField    (14));
    id->Oxygen              (ResultSetGetFloatField    (15));
    id->OxygenSensor        (ResultSetGetFloatField    (16));
    id->Pressure            (ResultSetGetFloatField    (17));
    id->RecordRate          (ResultSetGetFloatField    (18));
    id->Salinity            (ResultSetGetFloatField    (19));
    id->SoundVelocity       (ResultSetGetFloatField    (20));
    id->Temperature         (ResultSetGetFloatField    (21));
    id->Transmisivity       (ResultSetGetFloatField    (22));
    id->TransmisivitySensor (ResultSetGetFloatField    (23));
    id->Turbidity           (ResultSetGetFloatField    (24));
    id->TurbiditySensor     (ResultSetGetFloatField    (25));
    id->Pitch               (ResultSetGetFloatField    (26));
    id->Roll                (ResultSetGetFloatField    (27));
    id->CropLeft            (ResultSetGetUint16Field   (28));
    id->CropRight           (ResultSetGetUint16Field   (29));
    id->ActiveColumns       (ResultSetGetUint16Field   (30));

    results->PushOnBack (id);
  }

  ResulSetFree ();

  return  results;
}  /* InstrumentDataProcessResults */





InstrumentDataPtr  DataBase::InstrumentDataGetByScanLine (const KKStr&  sipperFileName,
                                                          kkuint32      scanLine
                                                         )
{
  InstrumentDataPtr result = NULL;

  KKStr   slqStr = "call  InstrumentDataGetByScanLine(" + sipperFileName.QuotedStr () + ", " + StrFormatInt (scanLine, "########0") + ")";

  kkint32  returnCd = QueryStatement (slqStr);
  if  (returnCd != 0)
    return  NULL;

  bool  cancelFlag = false;
  InstrumentDataListPtr  results = InstrumentDataProcessResults (cancelFlag);
  ResultSetsClear ();
  if  ((results != NULL) &&  (results->QueueSize () > 0))
    result = results->PopFromBack ();

  delete  results;
  results = NULL;

  return  result;
}  /* InstrumentDataGetByScanLine */





void  DataBase::InstrumentDataInsert (const KKStr&            _sipperFileName,
                                      const InstrumentData&   id
                                     )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("InstrumentDataInsert");
    return;
  }

  KKStr  sipperFileName = osGetRootName (_sipperFileName);

  KKStr  insertStr (2500);
  insertStr << "call  InstrumentDataInsert2("
            << sipperFileName.QuotedStr  ()         << ", "
            << id.ScanLine               ()         << ", "
            << id.ByteOffset             ()         << ", "
            << DateTimeToQuotedStr (id.CtdDate ())  << ", "
            << id.Latitude               ()         << ", "
            << id.Longitude              ()         << ", "
            << id.CTDBattery             ()         << ", "
            << id.Conductivity           ()         << ", "
            << id.Density                ()         << ", "
            << id.Depth                  ()         << ", "
            << id.FlowRate1              ()         << ", "
            << id.FlowRate2              ()         << ", "
            << id.Fluorescence           ()         << ", "
            << id.FluorescenceSensor     ()         << ", "
            << id.CdomFluorescence       ()         << ", "
            << id.CdomFluorescenceSensor ()         << ", "
            << id.Oxygen                 ()         << ", "
            << id.OxygenSensor           ()         << ", "
            << id.Pressure               ()         << ", "
            << id.RecordRate             ()         << ", "
            << id.Salinity               ()         << ", "
            << id.SoundVelocity          ()         << ", "
            << id.Temperature            ()         << ", "
            << id.Transmisivity          ()         << ", "
            << id.TransmisivitySensor    ()         << ", "
            << id.Turbidity              ()         << ", "
            << id.TurbiditySensor        ()         << ", "
            << id.Pitch                  ()         << ", "
            << id.Roll                   ()         << ", "
            << id.CropLeft               ()         << ", "
            << id.CropRight              ()         << ", "
            << id.ActiveColumns          ()
            << ")";

  kkint32  returnCd = QueryStatement (insertStr);

  return;
}  /* InstrumentDataInsert */





void  DataBase::InstrumentDataUpdateCropSettings (const KKStr&  sipperFileName,
                                                  kkuint32      scanLineStart,
                                                  kkuint32      scanLineEnd,
                                                  kkuint16      cropLeft,
                                                  kkuint16      cropRight,
                                                  kkuint16      activeColumns
                                                 )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("InstrumentDataUpdateCropSettings");
    return;
  }
  KKStr  sqlStr = "call  InstrumentDataUpdateCropSettings(" + sipperFileName.QuotedStr ()               + ", "
                                                            + StrFormatInt (scanLineStart, "ZZZZZZZ0")  + ", "
                                                            + StrFormatInt (scanLineEnd,   "ZZZZZZZ0")  + ", "
                                                            + StrFormatInt (cropLeft,      "ZZZZ0")     + ", "
                                                            + StrFormatInt (cropRight,     "ZZZZ0")     + ", "
                                                            + StrFormatInt (activeColumns, "ZZZZ0")
                                                      + ")";
                                                            
  kkint32  returnCd = QueryStatement (sqlStr);
  ResultSetsClear ();
}  /* InstrumentDataUpdateCropSettings */







void  DataBase::InstrumentDataSaveListForOneSipperFile (const KKStr&               _sipperFileName,
                                                        const InstrumentDataList&  instrumentData,
                                                        const bool&                _cancelFlag
                                                       )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("InstrumentDataSaveListForOneSipperFile");
    return;
  }

  KKStr  sipperFileName = osGetRootName (_sipperFileName);

  // Lets first make sure that a record exists for this SipperFile

  SipperFilePtr  sf = SipperFileRecLoad (sipperFileName);
  if  (sf == NULL)
  {
    sf = new SipperFile (sipperFileName);
    sf->AssignCtdExternalInstruments (osGetRootName (sipperFileName));
    SipperFileInsert (*sf);
  }

  KKStr  deleteStr = "call  InstrumentDataDeleteOneSipperFile(" + sipperFileName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (deleteStr);
  ResultSetsClear ();

  InstrumentDataList::const_iterator  idx;
  for  (idx = instrumentData.begin ();  (idx != instrumentData.end ())  &&  (!_cancelFlag);  idx++)
    InstrumentDataInsert (sipperFileName, **idx);

  delete  sf;
  sf = NULL;
}  /* InstrumentDataSaveListForOneSipperFile */





void  DataBase::InstrumentDataLoadAllSipperFiles (const bool&  cancelFlag)
{
  VectorKKStr  allSipperFiles = InstrumentDataFileManager::GetListOfSipperFiles (log);

  VectorKKStr::iterator  idx;
  for  (idx = allSipperFiles.begin ();  (idx != allSipperFiles.end ())  &&  (!cancelFlag);   idx++)
  {
    KKStr  sipperFileRootName = osGetRootName (*idx);

    cout << sipperFileRootName << std::endl;

    SipperFilePtr  sipperRec = SipperFileRecLoad (sipperFileRootName);

    if  (sipperRec)
    {
      if  (sipperFileRootName.Len () <= 14)
      {
        InstrumentDataListPtr   oneSipperFile = InstrumentDataFileManager::ReExtractInstrumentDataForSipperFile (*idx, sipperRec, cancelFlag, log);
        if  ((oneSipperFile != NULL)  &&  (!cancelFlag))
          InstrumentDataSaveListForOneSipperFile (*idx, *oneSipperFile, cancelFlag);
        delete  oneSipperFile;
        oneSipperFile = NULL;
      }
    }
  }
}  /* InstrumentDataLoadAllSipperFiles */



VectorKKStr  DataBase::InstrumentDataGetFieldNames ()
{
  VectorKKStr  results;
  KKStr  selectStr = "select Column_Name, Data_Type  from information_schema.Columns  c where c.Table_Schema= \"pices\"  and c.Table_Name = \"InstrumentData\"";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return  results;

  MYSQL_RES*  resSet;
  resSet = mysql_store_result (conn); /* generate result set */
  if  (resSet == NULL)
    return  results;


  MYSQL_ROW  row = NULL;
  while  (row = mysql_fetch_row (resSet))
  {
    KKStr fieldName   = row[0];
    results.push_back (fieldName);
  }


  mysql_free_result (resSet);
  resSet = NULL;

  return  results;
}   /* InstrumentDataGetFieldNames */







InstrumentDataListPtr  DataBase::InstrumentDataLoad (const KKStr&  sipperFileName,
                                                     const bool&   cancelFlag
                                                    )
{
  KKStr   sqlStr = "call  InstrumentDataLoadOneSipperFile(" + sipperFileName.QuotedStr () + ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  InstrumentDataListPtr  results = InstrumentDataProcessResults (cancelFlag);
  if  (results == NULL)
    return NULL;

  ResultSetsClear ();
  if  (!cancelFlag)
  {
    SipperFilePtr sf = SipperFileRecLoad (sipperFileName);
    if  (sf)
      results->ScanRate (sf->ScanRate ());
    delete  sf;
    sf = NULL;
  }

  return  results;
}  /* InstrumentDataLoad */






InstrumentDataListPtr  DataBase::InstrumentDataLoad (const KKStr&  cruiseName,
                                                     const KKStr&  stationName,
                                                     const KKStr&  deploymentNum,
                                                     const bool&   cancelFlag
                                                    )
{
  // Will accomplish in two parts.
  // 1) Get list of sipper files that meet the criteria passed in,
  // 2) For each sipper file we will call "InstrumentDataLoad" by SipperFileName as defined in previous method.

  VectorKKStr*  sipperFileNames = SipperFileGetList (cruiseName, stationName, deploymentNum);
  if  (sipperFileNames == NULL)
    return NULL;

  InstrumentDataListPtr  data = new InstrumentDataList (true);

  VectorKKStr::iterator  idx;
  for  (idx = sipperFileNames->begin ();  (idx != sipperFileNames->end ())  &&  (!cancelFlag);  idx++)
  {
    InstrumentDataListPtr  dataThisSipperFile = InstrumentDataLoad (*idx, cancelFlag);
    data->AddQueue (*dataThisSipperFile);
    dataThisSipperFile->Owner (false);
    delete  dataThisSipperFile;
    dataThisSipperFile = NULL;
  }

  delete  sipperFileNames;
  sipperFileNames = NULL;

  return  data;
}  /* InstrumentDataLoad */




/**
 @brief  Will update all InstrumentData entries that fall within the dateTime range with the Latitude and longitude.
 */
void  DataBase::InstrumentDataUpdateLatitudeAndLongitude (const DateTime&  dateTimeStart,
                                                          const DateTime&  dateTimeEnd,
                                                          double           latitude,
                                                          double           longitude
                                                         )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("InstrumentDataUpdateLatitudeAndLongitude");
    return;
  }

  KKStr   sqlStr (1024);
  
  sqlStr << "call  InstrumentDataUpdateLatitudeAndLongitude("
         << DateTimeToQuotedStr (dateTimeStart)         << ", "
         << DateTimeToQuotedStr (dateTimeEnd)           << ", " 
         << StrFormatDouble (latitude,  "---0.0000000")  << ", " 
         << StrFormatDouble (longitude, "---0.0000000")
         << ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
    ResultSetsClear ();
    
}  /* InstrumentDataUpdateLatitudeAndLongitude */




DataBase::DeploymentTimeResult*  DataBase::InstrumentDataGetDeploymentTimes (SipperDeploymentPtr deployment)
{
  char const *  fieldNames[] = {"ctdDateTimeStart", "ctdDateTimeEnd", "utcDateTimeStart", "utcDateTimeEnd", NULL);

  DeploymentTimeResult* result = NULL;

  KKStr  sqlStr = "call  InstrumentDataDeleteOneSipperFile(" + 
                              deployment->CruiseName    ().QuotedStr () + "," +
                              deployment->StationName   ().QuotedStr () + "," +
                              deployment->DeploymentNum ().QuotedStr () +
                            ")";

  kkint32  returnCd = QueryStatement (sqlStr.Str ());

  if  (returnCd == 0)
  {
    result = new DeploymentTimeResult ();

    ResultSetLoad (fieldNames);
    if  (ResultSetFetchNextRow ())   
    {
      delete result;
      result = new DeploymentTimeResult ();
      result->ctdDateTimeStart = ResultSetGetDateTimeField(0);
      result->ctdDateTimeEnd   = ResultSetGetDateTimeField(1);
      result->utcDateTimeStart = ResultSetGetDateTimeField(2);
      result->utcDateTimeEnd   = ResultSetGetDateTimeField(3);
    }
    ResulSetFree ();
    ResultSetsClear ();
  }

  return result;
}



vector<vector<float> >*  DataBase::InstrumentDataGetSpecificFields (const KKStr&        sipperFileName,
                                                                    const KKStrListPtr  fieldNames,
                                                                    const DateTime&     dateTimeStart,
                                                                    const DateTime&     dateTimeEnd
                                                                   )
{
  if  (fieldNames == NULL)
    return NULL;

  int numFields = fieldNames->QueueSize ();

  KKStr  fieldNameList (256);

  KKStrList::iterator idx;
  for  (idx = fieldNames->begin ();  idx != fieldNames->end ();  ++idx)
  {
    if  (!fieldNameList.Empty ())
      fieldNameList << ", ";
    fieldNameList << "id." << **idx;
  }

  KKStr  dateTimeStartStr = dateTimeStart.YYYY_MM_DD_HH_MM_SS ().QuotedStr ();
  KKStr  dateTimeEndStr   = dateTimeEnd.YYYY_MM_DD_HH_MM_SS ().QuotedStr ();

  KKStr  sqlStr (256);
  sqlStr << "select " << fieldNameList << "\n"
         << "     from InstrumentData id"                                                   << "\n"
         << "     join SipperFiles sf on(sf.SipperFileId = id.SipperFileId)"                << "\n"
         << "     where (sf.SipperFileName = " << sipperFileName.QuotedStr () << ")   and"  << "\n"
         << "           (id.CTDDateTime >= " << dateTimeStartStr << ") and"                 << "\n"
         << "           (id.CTDDateTime <  " << dateTimeEndStr   << ")"                     << "\n";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return NULL;

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return NULL;

  vector<vector<float> >* results = new vector<vector<float> >();

  while  (ResultSetFetchNextRow ())
  {
    vector<float>  row (numFields, 0.0f);
    for  (int xxx = 0;  xxx < numFields;  ++xxx)
      row[xxx] = (float)this->ResultSetGetDoubleField (xxx);
    results->push_back (row);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* InstrumentDataGetSpecificFields */




vector<vector<float> >*  DataBase::InstrumentDataGetSpecificFields (const KKStr&        cruiseName,
                                                                    const KKStr&        stationName,
                                                                    const KKStr&        deploymentNum,
                                                                    const KKStrListPtr  fieldNames,
                                                                    const DateTime&     dateTimeStart,
                                                                    const DateTime&     dateTimeEnd
                                                                   )
{
  if  (fieldNames == NULL)
    return NULL;

  int numFields = fieldNames->QueueSize ();

  KKStr  fieldNameList (256);

  KKStrList::iterator idx;
  for  (idx = fieldNames->begin ();  idx != fieldNames->end ();  ++idx)
  {
    if  (!fieldNameList.Empty ())
      fieldNameList << ", ";
    fieldNameList << "id." << **idx;
  }

  KKStr  dateTimeStartStr = dateTimeStart.YYYY_MM_DD_HH_MM_SS ().QuotedStr ();
  KKStr  dateTimeEndStr   = dateTimeEnd.YYYY_MM_DD_HH_MM_SS ().QuotedStr ();

  KKStr  sqlStr (256);
  sqlStr << "select " << fieldNameList << "\n"
         << "     from InstrumentData id"                                     << "\n"
         << "     join SipperFiles sf on(sf.SipperFileId = id.SipperFileId)"  << "\n"
         << "     where ";

  if  (!cruiseName.Empty ())
    sqlStr << "(sf.CruiseName    = " << cruiseName.QuotedStr    () << ")   and"    << "\n";

  if  (!stationName.Empty ())
    sqlStr << "           (sf.stationName   = " << stationName.QuotedStr   () << ")   and"    << "\n";

  if  (!deploymentNum.Empty ())
    sqlStr <<                               "           (sf.deploymentNum = " << deploymentNum.QuotedStr () << ")   and"    << "\n";

  sqlStr << "           (id.CTDDateTime  >= " << dateTimeStartStr           << ")   and"    << "\n"
         << "           (id.CTDDateTime  <  " << dateTimeEndStr             << ")"          << "\n";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return NULL;
    
  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return NULL;

  vector<vector<float> >* results = new vector<vector<float> >();

  while  (ResultSetFetchNextRow ())
  {
    vector<float>  row (numFields, 0.0f);
    for  (int xxx = 0;  xxx < numFields;  ++xxx)
      row[xxx] = (float)this->ResultSetGetDoubleField (xxx);
    results->push_back (row);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* InstrumentDataGetSpecificFields */






/**
 *@brief  Re-Extracts Instrument data from SIPPER file and updates DataBase tables to make sure they are correct.
 *@details This method re-extracts instrument data for the given SipperFile and updates the
 *        appropriate tables.  This way all tables are in agreement with each other.  For the Specified 
 *        SIPPER File "siperFileName".
 * 1) Extract Instrument Data from original SipperFile.
 * 2) Replaces all entries in IntrumentData table for 'sipperFile' with what was just extracted.
 * 3) Updates the InstrumentData fields in Images with data just extracted.
 */
void  DataBase::InstrumentDataReFreshSipperFile (const KKStr&   sipperFileName,
                                                 const bool&    cancelFlag,
                                                 char*          msgBuff,
                                                 kkint32        msgBuffLen
                                                )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("InstrumentDataReFreshSipperFile");
    return;
  }

  SipperFilePtr  sf = SipperFileRecLoad (sipperFileName);
  if  (!sf) 
  {
    WriteBuff (msgBuff, msgBuffLen, "No such SIPPER file.");
    return;
  }

  WriteBuff (msgBuff, msgBuffLen, "Retrieving Instrument Data.");
  InstrumentDataListPtr  data = InstrumentDataFileManager::ReExtractInstrumentDataForSipperFile (sipperFileName, sf, cancelFlag, log);
  if  (!data)
  {
    WriteBuff (msgBuff, msgBuffLen, "Instrument data not found.");
    return;
  }

  if   (cancelFlag)
    return;

  WriteBuff (msgBuff, msgBuffLen, "Updating Instrument Data File.");
  InstrumentDataSaveListForOneSipperFile (sipperFileName, *data, cancelFlag);

  InstrumentDataList::iterator  idx;
  idx = data->begin ();
  InstrumentDataPtr id = *idx;    idx++;

  while  ((idx != data->end ())  &&  (id->ScanLine () < 30000))
  {
    id = *idx;
    idx++;
  }

  if  ((idx == data->end ())  ||  (id == NULL))
  {
    // There is just not enough instrument data
    return;
  }

  InstrumentDataPtr lastId = id;
  kkuint32  scanLineStart = 0;

  while  ((idx != data->end ())  &&  (!cancelFlag))
  {
    id = *idx;
    kkuint32  scanLineEnd = id->ScanLine ();

    KKStr  msg (128);
    msg << "Updating[" << sipperFileName << "]  ScanLines[" << scanLineStart << " - " << scanLineEnd << "]";
    WriteBuff (msgBuff, msgBuffLen, msg.Str ());

    cout << msg << endl;

    ImageUpdateInstrumentDataFields (lastId, sipperFileName, scanLineStart, scanLineEnd);

    lastId = id;
    scanLineStart = scanLineEnd;
    idx++;
  }
}  /* InstrumentDataReFreshSipperFile */




VolumeSampledStatListPtr  DataBase::InstrumentDataGetVolumePerMeterDepth (const KKStr&  siperFileName,
                                                                          float         depthBinSize
                                                                         )
{
  KKStr  sipperFileRootName = osGetRootName (siperFileName);
  KKStr  sqlStr (128);
  sqlStr << "call  InstrumentDataGetVolumePerMeterDepth (" << sipperFileRootName.QuotedStr () << ", " << depthBinSize << ")";
  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  char const *  fieldNames[] = {"DownCast", "binId", "binDepth", "volumeSampled", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
  {
    log.Level (-1) << endl << "DataBase::InstrumentDataGetVolumePerMeterDepth   ResultSets not returned." << endl;
    return NULL;
  }

  VolumeSampledStatListPtr  results = new VolumeSampledStatList ();
  while  (ResultSetFetchNextRow ())
  {
    VolumeSampledStatPtr  stat = new VolumeSampledStat ();
    stat->BinId         (ResultSetGetIntField   (1));
    stat->BinDepth      (ResultSetGetFloatField (2));
    stat->VolumeSampled (ResultSetGetFloatField (3));
    results->Give (stat);
  }

  ResulSetFree ();
  ResultSetsClear ();
  return  results;
}  /* InstrumentDataGetVolumePerMeterDepth */





VolumeSampledStatListPtr  DataBase::InstrumentDataGetVolumePerMeterDepth (const KKStr&  cruiseName,
                                                                          const KKStr&  stationName,
                                                                          const KKStr&  deploymentNum,
                                                                          float         depthBinSize,
                                                                          const bool&   cancelFlag
                                                                         )
{
  VectorKKStr*  sipperFileNames = SipperFileGetList (cruiseName, stationName, deploymentNum);
  if  (sipperFileNames == NULL)
    return NULL;

  VolumeSampledStatListPtr  results = new VolumeSampledStatList ();

  VectorKKStr::iterator  idx;
  for  (idx = sipperFileNames->begin ();  (idx != sipperFileNames->end ())  &&  (!cancelFlag);  idx++)
  {
    VolumeSampledStatListPtr  profileThisSiperFile = InstrumentDataGetVolumePerMeterDepth (*idx, depthBinSize);
    if  (profileThisSiperFile)
      (*results) += (*profileThisSiperFile);

    delete  profileThisSiperFile;
    profileThisSiperFile = NULL;
  }

  delete  sipperFileNames;
  sipperFileNames = NULL;

  return  results;
}  /* InstrumentDataGetVolumePerMeterDepth */






void  DataBase::InstrumentDataDeploymentVolumeSampled (const KKStr&               cruiseName,
                                                       const KKStr&               stationName,
                                                       const KKStr&               deploymentNum,
                                                       float                      depthBinSize,
                                                       VolumeSampledStatListPtr&  downCastStats,
                                                       VolumeSampledStatListPtr&  upCastStats
                                                      )
{
  KKStr  depthBinSizeStr = StrFormatDouble (depthBinSize, "##0.0");
  KKStr  sqlStr = "Call  InstrumentDataDeploymentVolumeSampled(" + cruiseName.QuotedStr () + "," + stationName.QuotedStr () + "," + deploymentNum.QuotedStr () + "," + depthBinSizeStr + ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return;

  char const *  fieldNames[] 
                      = {"DownCast", "BinId", "BinDepth", "ScanLines", "VolumeSampled", NULL};

  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
  {
    log.Level (-1) << endl << "DataBase::InstrumentDataDeploymentVolumeSampled   ResultSets not returned." << endl;
    return;
  }

  delete  downCastStats;  downCastStats = NULL;
  delete  upCastStats;    upCastStats   = NULL;

  downCastStats = new VolumeSampledStatList ();
  upCastStats   = new VolumeSampledStatList ();
  while  (ResultSetFetchNextRow ())
  {
    bool  downCast       = ResultSetGetBool        (0);
    kkint32 binId          = ResultSetGetIntField    (1);
    float binDepth       = ResultSetGetFloatField  (2);
    kkint32 scanLines      = ResultSetGetIntField    (3);
    float volumeSampled  = ResultSetGetFloatField  (4);

    VolumeSampledStatPtr  stat = new VolumeSampledStat (binId, binDepth, volumeSampled);
    if  (downCast)
      downCastStats->Add (*stat);
    else
      upCastStats->Add (*stat);
    delete  stat;
    stat = NULL;
  }

  ResulSetFree ();
  ResultSetsClear ();
  return;
}  /* InstrumentDataDeploymentVolumeSampled */





InstrumentDataMeansListPtr  DataBase::InstrumentDataBinByMeterDepth (const KKStr&  cruiseName,
                                                                     const KKStr&  stationName,
                                                                     const KKStr&  deploymentNum,
                                                                     float         depthBinSize,
                                                                     const bool&   cancelFlag
                                                                    )
{
  KKStr  depthBinSizeStr = StrFormatDouble (depthBinSize, "##0.0");
  KKStr  sqlStr = "Call  InstrumentDataBinByMeterDepth(" + cruiseName.QuotedStr () + "," + stationName.QuotedStr () + "," + deploymentNum.QuotedStr () + "," + depthBinSizeStr + ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  char const *  fieldNames[] 
                      = {"DownCast", "BinId", "BinDepth", "ScanLines", "VolumeSampled",  
                         "TemperatureMean", "SalinityMean", "DenisityMean",
                         "FluorescenceMean", "FluorescenceSensorMean", "OxygenMean", 
                         "DepthMean",  "TransmisivityMean", "TurbidityMean", "CdomFluorescenceMean",
                         NULL
                        };

  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
  {
    log.Level (-1) << endl << "DataBase::InstrumentDataGetVolumePerMeterDepth   ResultSets not returned." << endl;
    return NULL;
  }

  InstrumentDataMeansListPtr  results = new InstrumentDataMeansList (true);
  while  (ResultSetFetchNextRow ())
  {
    bool  downCast               = ResultSetGetBool        (0);
    kkint32 binId                  = ResultSetGetIntField    (1);
    float binDepth               = ResultSetGetFloatField  (2);
    kkint32 scanLines              = ResultSetGetIntField    (3);
    float volumeSampled          = ResultSetGetFloatField  (4);
    float temperatureMean        = ResultSetGetFloatField  (5);
    float salinityMean           = ResultSetGetFloatField  (6);
    float denisityMean           = ResultSetGetFloatField  (7);
    float fluorescenceMean       = ResultSetGetFloatField  (8);
    float fluorescenceSensorMean = ResultSetGetFloatField  (9);
    float oxygenMean             = ResultSetGetFloatField  (10);
    float depthMean              = ResultSetGetFloatField  (11);
    float transmisivityMean      = ResultSetGetFloatField  (12);
    float turbidityMean          = ResultSetGetFloatField  (13);
    float cdomFluorescenceMean   = ResultSetGetFloatField  (14);

    results->PushOnBack (new InstrumentDataMeans (downCast,
                                                  binId,
                                                  binDepth,
                                                  scanLines,
                                                  volumeSampled,
                                                  temperatureMean,
                                                  salinityMean,
                                                  denisityMean,
                                                  fluorescenceMean,
                                                  fluorescenceSensorMean,
                                                  oxygenMean,
                                                  depthMean,
                                                  transmisivityMean,
                                                  turbidityMean,
                                                  cdomFluorescenceMean
                                                 )
                        );
  }

  ResulSetFree ();
  ResultSetsClear ();
  return  results;
}  /* InstrumentDataBinByMeterDepth */





GPSDataPointListPtr DataBase::InstrumentDataRetrieveGPSInfo (const KKStr&  cruiseName,
                                                             const KKStr&  stationName,
                                                             const KKStr&  deploymentNum,
                                                             kkint32       timeInterval   /**< Interval in seconds */
                                                            )
{
  KKStr  sqlStr (128);
  sqlStr << "call  InstrumentDataRetrieveGpsInfo (" 
         << cruiseName.QuotedStr () << ", " 
         << stationName.QuotedStr () << ", " 
         << deploymentNum.QuotedStr () << ", " 
         << timeInterval
         << ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return  NULL;

  char const *  fieldNames[] = {"CTDDateTime", "UtcDateTime", "Latitude", "Longitude", "courseOverGround", "SpeedOverGround", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
  {
    log.Level (-1) << endl << "DataBase::InstrumentDataRetrieveGPSInfo   ResultSets not returned." << endl;
    return NULL;
  }

  GPSDataPointListPtr  results = new GPSDataPointList (true);
  while  (ResultSetFetchNextRow ())
  {
    GPSDataPointPtr  stat 
      = new GPSDataPoint (ResultSetGetDateTimeField (1),   /**< GpsUtcTime        */
                          ResultSetGetDoubleField   (2),   /**< Latitude          */
                          ResultSetGetDoubleField   (3),   /**< Longitude         */
                          ResultSetGetFloatField    (4),   /**< CourseOverGround  */
                          ResultSetGetFloatField    (5)    /**< SpeedOverGround   */
                         );
    results->PushOnBack (stat);
  }

  ResulSetFree ();
  ResultSetsClear ();
  return  results;
}   /* InstrumentDataRetrieveGpsInfo */







//*********************************************************************************************
//*                               LogEntries  Routines                                        *
//*********************************************************************************************
DataBaseLogEntryPtr  DataBase::LogEntriesProcessStart 
                                       (const KKStr&     progCode,
                                        const KKStr&     progName,
                                        const DateTime&  dateTimeCompiled,
                                        const KKStr&     cmdLine,
                                        const KKStr&     sipperFileName
                                        )
{
  KKStr  sqlStr (256);

  KKStr  compName = osGetHostName ();
  KKStr  userName = osGetUserName ();
  KKStr  sipperFileNameRoot = osGetRootName (sipperFileName);

  sqlStr << "call LogEntriesProcessStart("
         << progCode.MaxLen (4).QuotedStr ()  << ","
         << progName.MaxLen (64).QuotedStr () << ","
         << dateTimeCompiled.YYYY_MM_DD_HH_MM_SS ().QuotedStr () << ","
         << cmdLine.MaxLen (512).QuotedStr () << ","
         << compName.QuotedStr ()             << ","
         << userName.QuotedStr ()             << ","
         << sipperFileNameRoot.QuotedStr ()   
         << ")";

  DataBaseLogEntryPtr  result = new DataBaseLogEntry (progCode, progName, dateTimeCompiled, cmdLine, compName, userName, sipperFileName);

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
  {
    char const *  fieldNames[] = {"LogEntryId", "DataBaseUserName", "DateTimeStart", "DateTimeStartUtc", NULL};
    ResultSetLoad (fieldNames);
    if  (!resultSetMore)
      return  NULL;

    MYSQL_ROW  row = NULL;
    while  (ResultSetFetchNextRow ())
    {
      result->LogEntryId       (ResultSetGetUintField     (0));
      result->DataBaseUserName (ResultSetGetField         (1));
      result->DateTimeStart    (ResultSetGetDateTimeField (2));
      result->DateTimeStartUtc (ResultSetGetDateTimeField (3));
    }
  }

  ResulSetFree ();
  ResultSetsClear ();
  return  result;
}  /* LogEntriesProcessStart */



  

void  DataBase::LogEntriesProcessEnd (DataBaseLogEntryPtr   logEntry,
                                      float                 cpuTimeUsed,
                                      const KKStr&          completionResult
                                     )
{
  if  (!logEntry)
  {
    log.Level (-1) << endl << "DataBase::LogEntriesProcessEnd   ***ERROR***      logEntry == NULL" << endl << endl;
    return;
  }

  KKStr  sqlStr (128);

  sqlStr << "call LogEntriesProcessEnd("
         << logEntry->LogEntryId ()        << ","
         << cpuTimeUsed                    << ","
         << completionResult.QuotedStr ()
         << ")";

  kkint32  returnCd = QueryStatement (sqlStr);
  if  (returnCd == 0)
  {
    char const *  fieldNames[] = {"DateTimeEnd", NULL};
    ResultSetLoad (fieldNames);
    if  (!resultSetMore)
      return;

    MYSQL_ROW  row = NULL;
    while  (ResultSetFetchNextRow ())
    {
      logEntry->DateTimeEnd (ResultSetGetDateTimeField (0));
    }
  }

  ResulSetFree ();
  ResultSetsClear ();
  return;
}  /* LogEntriesProcessEnd */




DataBaseLogEntryPtr  DataBase::LogEntriesSelect (kkuint32 _logEntryId)
{
  DataBaseLogEntryPtr  result = NULL;
  KKStr  sqlCmd (100);
  sqlCmd << "call LogEntriesSelect(" << _logEntryId << ")";
  kkint32  returnCd = QueryStatement (sqlCmd);
  if  (returnCd == 0)
  {
    char const *  fieldNames[] 
                        = {"LogEntryId",         //  0
                           "ProgCode",           //  1
                           "ProgName",           //  2
                           "DateTimeCompiled",   //  3
                           "CmdLine",            //  4
                           "CompName",           //  5
                           "DataBaseUserName",   //  6
                           "CompUserName",       //  7
                           "DateTimeStart",      //  8
                           "DateTimeStartUtc",   //  9
                           "DateTimeEnd",        // 10
                           "CpuTimeUsed",        // 11
                           "CompletionStatus",   // 12
                           "SipperFileName",     // 13
                           NULL
                          };

    ResultSetLoad (fieldNames);
    if  (!resultSetMore)
      return  NULL;

    MYSQL_ROW  row = NULL;
    while  (ResultSetFetchNextRow ())
    {
      delete result;
      result = NULL;
      result = new DataBaseLogEntry ();
      result->LogEntryId       (ResultSetGetUintField      (0));
      result->ProgCode         (ResultSetGetField          (1));
      result->ProgName         (ResultSetGetField          (2));
      result->DateTimeCompiled (ResultSetGetDateTimeField  (3));
      result->CmdLine          (ResultSetGetField          (4));
      result->CompName         (ResultSetGetField          (5));
      result->DataBaseUserName (ResultSetGetField          (6));
      result->CompUserName     (ResultSetGetField          (7));
      result->DateTimeStart    (ResultSetGetDateTimeField  (8));
      result->DateTimeStartUtc (ResultSetGetDateTimeField  (9));
      result->DateTimeEnd      (ResultSetGetDateTimeField (10));
      result->CpuTimeUsed      (ResultSetGetFloatField    (11));
      result->CompletionStatus (ResultSetGetField         (12));
      result->SipperFileName   (ResultSetGetField         (13));
    }
  }

  ResulSetFree ();
  ResultSetsClear ();
  return  result;
}





//*********************************************************************************************
//*                               SipperCruise  Routines                                      *
//*********************************************************************************************

void  DataBase::InsertCruise (const KKStr&     shipName, 
                              const KKStr&     description,
                              const DateType&  dateStart,
                              const DateType&  dateEnd,
                              const KKStr&     location,
                              const KKStr&     objective,
                              const KKStr&     principal,
                              const KKStr&     researchOrg
                             )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("InsertCruise");
    return;
  }

  KKStr  shipNameAdj = shipName;
  if  (shipNameAdj.Len () > 10)
    shipNameAdj = shipNameAdj.SubStrPart (0, 9);

  KKStr  insertStatement;
  insertStatement << "Insert into Cruises (CruiseName, ShipName, Description, DateStart, DateEnd, Location, Objective, Principal, ResearchOrg)" << endl
                  << "             values ("
                  << shipNameAdj.QuotedStr ()    << ", "
                  << description.QuotedStr ()    << ", "
                  << DateToQuotedStr (dateStart) << ", "
                  << DateToQuotedStr (dateEnd)   << ", "
                  << location.QuotedStr ()       << ", "
                  << objective.QuotedStr ()      << ", "
                  << principal.QuotedStr ()      << ", "
                  << researchOrg.QuotedStr ()
                  << ")" << endl;

  kkint32  returnCd = QueryStatement (insertStatement.Str ());
}  /* InsertCruise  */



                            
void  DataBase::SipperCruiseDelete (const KKStr&  cruiseName)
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperCruiseDelete");
    return;
  }

  KKStr  sqlStr = "call  CruisesDelete(" + cruiseName.QuotedStr () + ")";
  kkint32  returnCd = QueryStatement (sqlStr.Str ());
  ResultSetsClear ();
}  /* SipperCruiseDelete */




void  DataBase::SipperCruiseInsert (SipperCruisePtr  cruise,
                                    bool&            successful
                                   )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperCruiseInsert");
    return;
  }

  KKStr  insertStr (1000);

  insertStr <<  "insert into  Cruises (CruiseName, ShipName, Description, DateStart, DateEnd, Location, Objective, Principal, ResearchOrg)" << endl
            <<  "values(" 
            <<  cruise->CruiseName  ().QuotedStr () << ", "
            <<  cruise->ShipName    ().QuotedStr () << ", "
            <<  cruise->Description ().QuotedStr () << ", "
            <<  cruise->DateStart   ().YYYY_MM_DD ().QuotedStr () << ", "
            <<  cruise->DateEnd     ().YYYY_MM_DD ().QuotedStr () << ", "
            <<  cruise->Location    ().QuotedStr () << ", "
            <<  cruise->Objective   ().QuotedStr () << ", "
            <<  cruise->Principal   ().QuotedStr () << ", "
            <<  cruise->ResearchOrg ().QuotedStr ()
            <<  ")"
            << endl;

  kkint32  returnCd = QueryStatement (insertStr.Str ());

  successful   = (returnCd == 0);
  return;
}  /* SipperCruiseInsert */




void  DataBase::SipperCruiseUpdate (SipperCruisePtr  cruise,
                                    bool&            successful
                                   )
{
  if  (!allowUpdates)
  {
    UpdatesNotAllowed ("SipperCruiseUpdate");
    return;
  }

  KKStr  updateStr (2000);

  updateStr << "update Cruises " << endl
            << "  set  ShipName     = " << cruise->ShipName     ().QuotedStr () << ", "
            << "       Description  = " << cruise->Description  ().QuotedStr () << ", "
            << "       DateStart    = " << cruise->DateStart    ().YYYY_MM_DD ().QuotedStr () << ", "
            << "       DateEnd      = " << cruise->DateEnd      ().YYYY_MM_DD ().QuotedStr () << ", "
            << "       Location     = " << cruise->Location     ().QuotedStr () << ", "
            << "       Objective    = " << cruise->Objective    ().QuotedStr () << ", "
            << "       Principal    = " << cruise->Principal    ().QuotedStr () << ", "
            << "       ResearchOrg  = " << cruise->ResearchOrg  ().QuotedStr ()
            << endl
            << "    where CruiseName = " << cruise-> CruiseName ().QuotedStr () << endl;

  kkint32  returnCd = QueryStatement (updateStr.Str ());

  successful   = (returnCd == 0);
  return;
}  /* SipperCruiseUpdate */



SipperCruiseListPtr  DataBase::SipperCruiseProcessResults ()
{
  char const *  fieldNames[] = {"CruiseName", "ShipName", "Description", "DateStart", "DateEnd", "Location", "Objective", "Principal", "ResearchOrg", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
  {
    log.Level (-1) << endl << "DataBase::SipperCruiseProcessResults   ResultSets not returned." << endl;
    return NULL;
  }

  SipperCruiseListPtr  results = new SipperCruiseList (true);
  while  (ResultSetFetchNextRow ())
  {
    SipperCruisePtr sipperCruise = new SipperCruise ();
    sipperCruise->CruiseName   (ResultSetGetField (0));
    sipperCruise->ShipName     (ResultSetGetField (1));
    sipperCruise->Description  (ResultSetGetField (2));
    sipperCruise->DateStart    (ResultSetGetDateTimeField (3).Date ());
    sipperCruise->DateEnd      (ResultSetGetDateTimeField (4).Date ());
    sipperCruise->Location     (ResultSetGetField (5)); 
    sipperCruise->Objective    (ResultSetGetField (6));
    sipperCruise->Principal    (ResultSetGetField (7));
    sipperCruise->ResearchOrg  (ResultSetGetField (8));
    results->PushOnBack (sipperCruise);
  }

  ResulSetFree ();
  return  results;
}



SipperCruisePtr  DataBase::SipperCruiseLoad (const KKStr&  cruiseName)
{
  KKStr queryStr = "select * from Cruises  where cruiseName = " + cruiseName.QuotedStr ();
  kkint32  returnCd = QueryStatement (queryStr);
  if  (returnCd != 0)
    return  NULL;

  SipperCruiseListPtr  results = SipperCruiseProcessResults ();
  ResultSetsClear ();

  if  (!results)
    return NULL;

  SipperCruisePtr  cruise = results->PopFromBack ();

  delete  results;
  return  cruise;
}  /* SipperCruiseLoad */




SipperCruiseListPtr  DataBase::SipperCruiseLoadAllCruises ()
{
  KKStr  selectStr (1000);
  selectStr << "select * from Cruises c  order by c.DateStart";
  kkint32  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return  NULL;

  SipperCruiseListPtr  results = SipperCruiseProcessResults ();
  ResultSetsClear ();

  if  (results)
    results->SortByCruiseName ();

  return  results;
}  /* SipperCruiseLoadAllCruises */




