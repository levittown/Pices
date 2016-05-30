#include "FirstIncludes.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "MemoryDebug.h"
#if  defined(_WINDOWS)
typedef wchar_t WCHAR;
#endif
using namespace std;


#include "KKBaseTypes.h"
#include "CmdLineExpander.h"
#include "KKStr.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace KKB;


#include "DataBase.h"
#include "PicesTrainingConfiguration.h"
#include "SipperBuff.h"
#include "SipperFile.h"
using namespace MLL;


#include "ExtractionParms.h"
using namespace  ImageExtractionManager;


ExtractionParms::ExtractionParms (KKStr    progName,
                                  KKStr    cmdLine,
                                  bool     _runningAsAWindowsApp,
                                  KKStr&   errorMessage,
                                  bool&    formatGood,
                                  RunLog&  log
                                 ):

        colorize              (false),
        configFileName        (),
        configFileNameSpecified (),
        connectedPixelDist    (DefaultConnectedPixelDist),
        countOnly             (false),
        dataBaseServer        (NULL),
        doneExecuting         (false),
        extractFeatureData    (false),
        fileFormat            (sfUnKnown),
        flowRateFileName      (),
        framesOnly            (false),
        imagesPerDirectory    (2000),
        maxAreaSize           (0.0f),
        minAreaSize           (0.1f),
        morphOperations       (),
        multiThreaded         (true),
        outputRootDir         (),
        preProcess            (true),
        refreshDataBaseImages (false),
        reportFileName        (),
        runningAsAWindowsApp  (_runningAsAWindowsApp),
        saveFrames            (false),
        saveFramesAfter       (false),
        sipperFileName        (),
        scanLineStart         (0),
        scanLineEnd           (0),
        statusMessage         (),
        updateDataBase        (true),

        bytesExtracted   (0),
        classStats       (NULL),
        classStatsPrev   (NULL),
        imagesExtracted  (0),
        imagesWritten    (0),
        linesExtracted   (0),
        pixelsRead       (0),
        pixelsWritten    (0),
        sipperFileSize   (0)

{
  errorMessage = "";

  CmdLineExpander  cmdLineExpander (progName, log, cmdLine);
  ParseCmdLine (cmdLineExpander,  errorMessage, formatGood, log);

  if  (!cmdLineExpander.ParmsGood ())
    formatGood = false;

  if  (formatGood)
    SetupOtherFileNames ();
 }




ExtractionParms::ExtractionParms (kkint32  argc,
                                  char**   argv,
                                  bool     _runningAsAWindowsApp,
                                  KKStr&   errorMessage,
                                  bool&    formatGood,
                                  RunLog&  log
                                 ):

        colorize              (false),
        configFileName        (),
        configFileNameSpecified (),
        connectedPixelDist    (DefaultConnectedPixelDist),
        countOnly             (false),
        dataBaseServer        (NULL),
        doneExecuting         (false),
        extractFeatureData    (false),
        fileFormat            (sfUnKnown),
        flowRateFileName      (),
        framesOnly            (false),
        imagesPerDirectory    (1000),
        maxAreaSize           (0.0f),
        minAreaSize           (0.1f),
        morphOperations       (),
        multiThreaded         (true),
        outputRootDir         (),
        preProcess            (true),
        refreshDataBaseImages (false),
        reportFileName        (),
        runningAsAWindowsApp  (_runningAsAWindowsApp),
        saveFrames            (false),
        saveFramesAfter       (false),
        sipperFileName        (),
        scanLineStart         (0),
        scanLineEnd           (0),
        statusMessage         (),
        updateDataBase        (true),
        
        bytesExtracted  (0),
        classStats      (NULL),
        classStatsPrev  (NULL),
        imagesExtracted (0),
        imagesWritten   (0),
        linesExtracted  (0),
        pixelsRead      (0),
        pixelsWritten   (0),
        sipperFileSize  (0)

{
  errorMessage = "";

  KKStr  progName = argv[0];

  if  (argc < 2)
  {
    DisplayExampleCmdLine ();
    formatGood = false;
    return;
  }

  CmdLineExpander  cmdLineExpander (progName, log, argc, argv);
  ParseCmdLine (cmdLineExpander,  errorMessage, formatGood, log);

  if  (!cmdLineExpander.ParmsGood ())
    formatGood = false;

  if  (formatGood)
    SetupOtherFileNames ();
}




ExtractionParms::~ExtractionParms ()
{
  delete  classStats;       classStats     = NULL;
  delete  classStatsPrev;   classStatsPrev = NULL;
  delete  dataBaseServer;   dataBaseServer = NULL;  
}


KKStr  ExtractionParms::DataBaseServerDesc () const
{
  if  (dataBaseServer)
    return  dataBaseServer->Description ();
  else
    return  "No Database Specified.";
}


void  ExtractionParms::SetupOtherFileNames ()
{
  osAddLastSlash (outputRootDir);

  KKStr  dirPath, sipperRootName, ext;

  osParseFileName (sipperFileName, dirPath, sipperRootName, ext);

  if  (reportFileName.Empty ())
  {
    reportFileName = outputRootDir + sipperRootName + ".txt";
  }

  if  (flowRateFileName.Empty ())
  {
    flowRateFileName = outputRootDir + sipperRootName + "_flowRates.txt";
  }
}  /* SetupOtherFileNames */




KKStr  ExtractionParms::CmdLine ()  const
{
  KKStr  cmdLine;

  cmdLine << "-A "    << cameraNum            << "  "
          << "-CPD "  << connectedPixelDist   << "  ";

  if  (colorize)
    cmdLine << "-COLORIZE"  << "  ";

  cmdLine << "-D "    << outputRootDir        << "  ";

  if  (saveFrames)
    cmdLine << "-F  ";

  if  (saveFramesAfter)
    cmdLine << "-SaveAfter  ";

  cmdLine << "-ipd "     << imagesPerDirectory                 << "  "
          << "-minArea " << minAreaSize                        << "  "
          << "-maxArea " << maxAreaSize                        << "  "
          << "-Pre "     << (preProcess ? "On" : "Off")        << "  "
          << "-R "       << reportFileName                     << "  "
          << "-S "       << sipperFileName                     << "  "
          << "-SF "      << SipperFileFormatToStr (fileFormat) << "  ";

  if  (!configFileName.Empty ())
    cmdLine << "-C " << configFileName;

  if  (scanLineStart > 0)
    cmdLine << "  " << "-ScanLineStart " << scanLineStart;

  if  (scanLineEnd > 0)
    cmdLine << "  " << "-ScanLineEnd " << scanLineEnd;

  if  (extractFeatureData)
    cmdLine << "  -X";

  if  (countOnly)
    cmdLine << "  -CountOnly";

  if  (framesOnly)
    cmdLine << "  -FO";

  if  (!morphOperations.Empty ())
    cmdLine << "  -Morph " << morphOperations;

  cmdLine << "  -MultiThreaded "  <<  (multiThreaded ? "Yes" : "No");

  if  (refreshDataBaseImages)
    cmdLine << "  -RefreshDataBaseImages";

  if  (!updateDataBase)
    cmdLine << "  -NoDataBase";

  if  (dataBaseServer)
    cmdLine << " -DataBase " << dataBaseServer->Description ();

  return  cmdLine;
}  /* CmdLine */





void  ExtractionParms::DisplayExampleCmdLine ()
{
  cout << endl;

  cout << "Usage:"  << endl;

  cout << "  GrayImageExtraction  -s   <sipper file name>"                    << endl;
  cout << "                       -d   <destination directory.>"              << endl;
  cout << "                       -r   <report file name>"                    << endl;
  cout << "                       -min <minimum area/ defaults 0.1(mm^2)>"    << endl;
  cout << "                       -max <maximum area/ ignored by default>"    << endl;
  cout << "                       -morph <o,c,d,e>"                           << endl;
  cout << "                       -f   <save raw frames>"                     << endl;
  cout << "                       -sf  <1Bit | 3Bit(Default)>"                << endl;
  cout << "                       -c   <Configuration Model>"                 << endl;
  cout << "                       -t   <start scan line>"                     << endl;
  cout << "                       -Erosions  <# of Erosions>"                 << endl;
  cout << "                       -ipd <Images Per Directory>"                << endl;
  cout << "                       -NoDataBase"                                << endl;
  cout << "                       -DataBase  <Database Description from MySql.cfg>"  << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -A    <Camera Number>"                                           << endl;
  cout <<                                                                        endl;
  cout << "  -c    <Configuration Model>"                                     << endl;
  cout <<                                                                        endl;
  cout << "  -Colorize    Use primary colors for 8 levels of gray scale."     << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -cpd  <Connnected Pixel Distance, defaults to 3>"                << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -CountOnly"                                                      << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -d    <Destination Directory>, Mandatory parameter, Extracted"   << endl;
  cout << "        images will be written into sub-directories below"         << endl;
  cout << "        this directory."                                           << endl;
  cout <<                                                                        endl;
  cout << "  -DataBase  <database Description from MySql.cfg>"                << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -f, -SaveFrames, or -SaveBefore"                                 << endl;
  cout << "  -f    If specified write out frames that images are"             << endl;
  cout << "        extracted from, before any processing is done."            << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -fo   Frames Only,  If you don't want to extract individual"     << endl;
  cout << "        images,"                                                   << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -ipd  Images Per Directory,   Defaults to 1000"                  << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -minArea  or  -ma"                                               << endl;
  cout << "       <Minimum Area> Minimum area(mm^2); function of number of"   << endl;
  cout << "       pixels  a particle, flow-rate, scan-rate, pixels/scanline," << endl;
  cout << "       and width of imaging chamber."                              << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -maxArea or -ma<Maximum Area> in mm^2"                           << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -Morph <operations>   operations are 'o'-Opening, 'c'-Closing"   << endl;
  cout << "        'e'-Erosion, 'd'-Dilation."                                << endl;
  cout << "        example string  'O,O,D' = Two Openings followed by a"      << endl;
  cout << "        followed by a Dilation operation."                         << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -MultiThreaded  <Yes | NO>"                                      << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -NoDataBase  Specify if you do not want to connect to a"         << endl;
  cout << "               Database Server.  You would do this if you are"     << endl;
  cout << "               only interested in writing images to disk."         << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -r    <Report File Name>, If not specified will default to"      << endl;
  cout << "        Sipper-File-Name, with '.txt' as the extension,  and"      << endl;
  cout << "        placed in the destination-Directory."                      << endl;
  cout <<                                                                        endl;
  cout << "  -RefreshDataBaseImages  Will update the data base image fields"  << endl;
  cout << "   or  -RDBI              with Image."                             << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -s    <Sipper File Name>, Mandatory parameter."                  << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -SaveAfter  If specified will same a frame after pre-processing" << endl;
  cout << "              see  '-f' option."                                   << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -sf   <Sipper Format>  1Bit       = Binary"                      << endl;
  cout << "                         3Bit       = GrayScale"                   << endl;
  cout << "                         Sipper3    = Sipper3"                     << endl;
  cout << "                         Sipper3Rev = Sipper3Rev"                  << endl;
  cout <<                                                                        endl;
  cout << "        If not specified will attempt to determine format"         << endl;
  cout << "        automatically."                                            << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -ScanLineStart    Starting Scan Line to use"                     << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -ScanLineEnd      Ending Scan Line to use"                       << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -Pre  <On | Off>  Specifies weather to pre-process frames"       << endl;
  cout << "        specifically the elimination of vertical lines."           << endl;
  cout << "        defaults to 'On'."                                         << endl;
  cout <<                                                                        endl;
  cout <<                                                                        endl;
  cout << "  -x   Extract Feature Data"                                       << endl;
  cout << endl;

}  /* DisplayExampleCmdLine */





void  ExtractionParms::ParseCmdLine (const CmdLineExpander&  cmdLineExpander,
                                     KKStr&                  errorMessage,
                                     bool&                   formatGood,
                                     RunLog&                 log
                                    )
{
  formatGood = true;

  scanLineStart = 0;
  scanLineEnd   = 0;

  cameraNum          = 0;
  configFileName     = "";
  configFileNameSpecified = "";
  extractFeatureData = false;
  fileFormat         = sfUnKnown;
  imagesPerDirectory = 1000;
  minAreaSize        = 0.1f;
  maxAreaSize        = 0.0f;
  saveFrames         = false;
  saveFramesAfter    = false;
  extractFeatureData = false;


  const VectorKKStr&  argv = cmdLineExpander.ExpandedParameters ();
  size_t              argc = argv.size ();

  size_t  i;

  // parse options
  for  (i = 0; i < argc;  i++)
  {
    if  (argv[i][0] != '-')  
        break;

    KKStr  switchStr (argv[i]);
    switchStr.Upper ();

    KKStr  parmStr;
    KKStr  parmStrUpper;

    if  (i < (argc - 1))
    {
      if  (argv[i + 1][0] != '-')
      {
        i++;
        parmStr = argv[i];
        parmStrUpper = parmStr;
        parmStrUpper.Upper ();
      }
    }
 
 
    if  (switchStr == "-A")
    {
      cameraNum = (uchar)parmStr.ToInt32 ();
    }

    else if  ((switchStr == "-C")  ||  (switchStr == "-CONFIG"))
    {
      configFileNameSpecified = parmStr;
      configFileName = PicesTrainingConfiguration::GetEffectiveConfigFileName (parmStr);
    }

    else if  ((switchStr == "-COLORIZE")  ||  (switchStr == "-COLOR"))
    {
      colorize = true;
    }

    else if  ((switchStr == "-COUNTONLY")  ||  (switchStr == "-CO"))
    {
      countOnly = true;
    }

    else if  (switchStr == "-CPD")
    {
      int  z = atoi (parmStr.Str ());
      if  ((z < 1)  ||  (z > 99))
      {
        errorMessage = "Connected Pixel Distance '-CPD' must be between 1 and 99.";
        cerr  << endl
              << "**** ERROR ****,  "  << errorMessage
              << endl;
        formatGood = false;
      }
      else
      {
        connectedPixelDist = uint (z);
      }
    }

    else if  (switchStr == "-D")
    {
      outputRootDir = parmStr;
    }

    else if  (switchStr.EqualIgnoreCase ("-DataBase")  ||  switchStr.EqualIgnoreCase ("-DB"))
    {
      delete  dataBaseServer;
      dataBaseServer = DataBase::GetServer (parmStr, log);
      if  (dataBaseServer == NULL)
      {
        errorMessage = "Invalid database description[" + parmStr + "].";
        cerr  << endl << "**** ERROR ****,  "  << errorMessage << endl;
        formatGood = false;
      }
    }

    else if  ((switchStr == "-IPD")  ||  (switchStr == "-IMAGESPERDIRECTORY"))
    {
      imagesPerDirectory = atoi (parmStr.Str ());
    }

    else if  (switchStr == "-MORPH")
    {
      morphOperations = parmStr;
    }

    else if  (switchStr.EqualIgnoreCase ("-MultiThreaded")  ||  switchStr.EqualIgnoreCase ("-MT"))
    {
      if  (parmStr.Empty ())
        multiThreaded = true;
      else
        multiThreaded = parmStr.ToBool ();
    }

    else if  ((switchStr == "-NODATABASE")  ||  ((switchStr == "-NDB")))
    {
      updateDataBase = false;
    }

    else if  ((switchStr == "-F")  ||  (switchStr == "-SAVEFRAMES")  ||  (switchStr == "-SAVEBEFORE"))
    {
      saveFrames = true;
    }

    else if  ((switchStr == "-SAVEAFTER")  ||  (switchStr == "-SA"))
    {
      saveFramesAfter = true;
    }

    else if  (switchStr == "-FO")
    {
      framesOnly = true;
      saveFrames = true;
    }

    else if  (switchStr == "-MINAREA")
    {
      minAreaSize = parmStr.ToFloat ();
    }

    else if  (switchStr == "-MAXAREA")
    {
      maxAreaSize = parmStr.ToFloat ();
    }

    else if  (switchStr == "-M" || switchStr == "-MIN")
    {
      minAreaSize = ComputeAreaFromPixelsUsingDefaults (parmStr.ToInt32 ());
    }

    else if  (switchStr == "-MAX")
    {
      maxAreaSize = ComputeAreaFromPixelsUsingDefaults (parmStr.ToInt32 ());
    }

    else if  ((switchStr == "-P")  ||  (switchStr == "-PRE"))
    {
      if  ((parmStrUpper == "ON")  ||  (parmStrUpper == "YES"))
        preProcess = true;
  
      else if  ((parmStrUpper == "OFF")  ||  (parmStrUpper == "NO"))
        preProcess = false;

      else
      {
        errorMessage = "Pre-Process Switch, must be 'On' or 'Off'";
        cerr  << endl
              << "**** ERROR ****,  "  << errorMessage
              << endl;
        formatGood = false;
      }
    }

    else if  (switchStr == "-R")
    {
      reportFileName = parmStr;
    }


    else if  ((switchStr == "-REFRESHDATABASEIMAGES")  ||  (switchStr == "-RDBI"))
    {
      refreshDataBaseImages = true;
    }

    else if  (switchStr == "-S")
    {
      sipperFileName = parmStr;
    }

    else if  (switchStr == "-SF")
    {
      fileFormat = SipperFileFormatFromStr (parmStrUpper);
      if  (fileFormat  ==  sfUnKnown)
      {
        errorMessage = "Invalid File Format Specified[" + parmStrUpper + "]";

        cerr  << endl
              << "**** ERROR ****,  "  << errorMessage
              << endl;
        formatGood = false;
      }
    }

    else if  ((switchStr == "-T")              ||  
              (switchStr == "-START")          ||
              (switchStr == "-STARTSCANLINE")  ||
              (switchStr == "-SCANLINESTART")  ||
              (switchStr == "-SSL")            ||
              (switchStr == "-SLS")
             )
    {
      scanLineStart = atoi (parmStr.Str ());
    }

    else if  ((switchStr == "-END")          ||
              (switchStr == "-ENDSCANLINE")  ||
              (switchStr == "-SCANLINEEND")  ||
              (switchStr == "-ESL")          ||
              (switchStr == "-SLE")
             )
    {
      scanLineEnd = atoi (parmStr.Str ());
    }

    else if  ((switchStr == "-X")  ||  (switchStr == "-EXTRACT"))
    {
      extractFeatureData = true;
    }

    else
    {
      errorMessage = "Invalid Switch Specified[" + switchStr + "]";
      cerr << endl
           << "**** ERROR ****,  " << errorMessage
           << endl;

      formatGood = false;
    }
  } /* End of For */


  if  (outputRootDir.Empty ())
  {
    errorMessage = "**** ERROR ****,  Destination Directory was not specified.";
    cerr << endl
         << errorMessage
         << endl;

    formatGood = false;
  }

  if  (sipperFileName.Empty ())
  {
    errorMessage = "Sipper File Name Was Not Specified.";
    cerr << endl
         << "**** ERROR ****,  " << errorMessage
         << endl;
    formatGood = false;
    return;
  }

  else
  {
    if  (SipperFile::JustRootName (sipperFileName))
      sipperFileName = InstrumentDataFileManager::GetFullSipperFileName(sipperFileName);

    sipperFileSize = osGetFileSize (sipperFileName);

    if  (sipperFileSize <= 0)
    {
      errorMessage = "Sipper File[" + sipperFileName + "] is Invalid";
      cerr << endl
           << "**** ERROR ****,  " << errorMessage
           << endl;

      formatGood = false;
      return;
    }

    else
    {
      KKStr  driveLetter, path, root, extension;
  
      osParseFileSpec (sipperFileName, driveLetter, path, root, extension);

      if  (reportFileName.Empty ())
      {
        KKStr  reptNameWithExt (root);
        reptNameWithExt << "_c" << (int)cameraNum << ".txt";
        reportFileName = osMakeFullFileName (outputRootDir, reptNameWithExt);
      }
  
      KKStr  flowRateFileNameWithExt (root);
      flowRateFileNameWithExt << "_flowRates" <<".txt";

      flowRateFileName = osMakeFullFileName (outputRootDir, flowRateFileNameWithExt);
    }
  }

  
  if  (fileFormat == sfUnKnown)
  {
    if  (!osFileExists (sipperFileName))
    {
      errorMessage = "Sipper File[" + sipperFileName + "] does not exist.";
      cerr  << endl
            << "*** ERROR ****,  " << errorMessage
            << endl;

      formatGood = false;
      return;
    }

    fileFormat = SipperBuff::GuessFormatOfFile (sipperFileName, cameraNum, log);
    
    if  (fileFormat == sfUnKnown)
    {
      errorMessage = "Can not Determine Format of Sipper File[" + sipperFileName + "].";
      cerr  << endl
            << "*** ERROR ****,  " << errorMessage
            << endl;

      formatGood = false;
      return;
    }
  }

  if  (framesOnly  &&  (countOnly  ||  extractFeatureData))
  {
    errorMessage = "**** ERROR ****,  Option Conflict,  '-fo' and ('-x' or '-CountOnly'.";
    cerr << endl
         << errorMessage
         << endl;

    formatGood = false;
  }

  if  (updateDataBase)
  {
    if  (dataBaseServer == NULL)
    {
      dataBaseServer = DataBase::GetDefaultServer (log);
    }

    if  (dataBaseServer == NULL)
    {
      errorMessage = "No database server is selected..";
      cerr << endl << errorMessage << endl << endl;
      formatGood = false;
    }
    else
    {
      DataBasePtr  db = new DataBase (dataBaseServer, log);
      if  (!db->Valid ())
      {
        errorMessage = "Unable to connect to database[" + dataBaseServer->Description () + "].";
        cerr << endl << errorMessage << endl << endl;
        formatGood = false;
      }
      delete  db;
      db = NULL;
    }
  }
  else
  {
    delete  dataBaseServer;
    dataBaseServer = NULL;
  }



  // If there a DataBase Server  and either the "start" or "end" scan line were not defined 
  // then go to data base to see if specified in SipperFiles table.
  if  ((dataBaseServer != NULL)  &&  (scanLineStart < 1)  &&  (scanLineEnd < 1))
  {
    DataBasePtr  dbConn = new DataBase (dataBaseServer, log);
    if  (dbConn->Valid ())
    {
      SipperFilePtr  sf = dbConn->SipperFileRecLoad (sipperFileName);
      if  (sf)
      {
        if  ((scanLineStart == 0)  &&  (sf->ExtractionScanLineStart () > 0))
          scanLineStart = sf->ExtractionScanLineStart ();

        if  ((scanLineEnd == 0)  &&  (sf->ExtractionScanLineEnd () > 0))
          scanLineEnd =  sf->ExtractionScanLineEnd ();
      }
      delete  sf;
      sf = NULL;
    }

    delete  dbConn;
    dbConn = NULL;
  }

  if  (refreshDataBaseImages  &&  (!extractFeatureData))
  {
    errorMessage = "**** ERROR ****,  When refreshing Image database(-RDBI) you must also extract features(-x).";
    cerr << endl << errorMessage << endl;
    formatGood = false;
  }

  if  ((scanLineEnd > 0)  &&  (scanLineEnd < scanLineStart))
  {
    errorMessage = "***ERROR***,  'StartScanLine["  + StrFormatInt (scanLineStart, "###,###,##0") + 
                   "] is greater than EndScanLine[" + StrFormatInt (scanLineEnd,   "###,###,##0") + "].";
    cerr << endl << errorMessage << endl;
    formatGood = false;
  }

  return;
}  /* ParseCmdLine */




void   ExtractionParms::ClassStats  (ClassStatisticListPtr  _classStats)
{
  delete  classStatsPrev;

  classStatsPrev = classStats;
  classStats     = _classStats;
}



float ExtractionParms::ComputeAreaFromPixelsUsingDefaults (kkint32 pixelCount)  const
{
  float scanRate = 24950.0;
  float flowRate = 0.5f;
  float scanLineWidthPixels = 3800.0;
  float scanLineWidthmm = 96.0f;
  float pixelWidth = scanLineWidthmm / scanLineWidthPixels;
  float pixelLen = 1000.0f * flowRate / scanRate;
  float pixelArea = pixelWidth* pixelLen;
  float area = pixelArea * pixelCount;
  return area;
}