#include  "FirstIncludes.h"

#include  <stdlib.h>
#include <memory>
#include <math.h>

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <process.h>

#include "MemoryDebug.h"

using namespace std;

#include "KKBaseTypes.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;

#include "DataBase.h"
#include "PicesVariables.h"
using namespace  MLL;


#include "ExtractAllSipperFiles.h"

// -RootDir  D:\Pices\SipperFiles\ETP2008  -Min 250  -c ETP08_station1_harvest_Binary.cfg

// -RootDir  E:\Pices\SipperFiles\SMP751001\SMP751001_034 -min 100  -c Oil_MFS.cfg

//  -RootDir  E:\Pices\SipperFiles\SMP751001\SMP751001_034\West  -Min 100  -c ETP08_station1_harvest_Binary.cfg
//  -RootDir  I:\Pices\SipperFiles\SMP751001\SMP751001_035 -Min 100 -c Oil_MFS.cfg

// -RootDir F:\Pices\SipperFiles\SMP751001\SMP751001_037\FirstHalf  -Min 100 -c etp_08_MFS.cfg

// -RootDir C:\Pices\SipperFiles\ETP2008\ETP2008_1C -Min 250 -c etp_08_BFS.cfg
// -RootDir C:\Pices\SipperFiles\SMP7510010\SMP751001037 -c etp_08_BFS.cfg -Min 150  

// -RootDir D:\Pices\SipperFiles\ETP2008\Part2  -c GulfOilBroad2_Discreate_Dual.cfg  -Min 150

// -RootDir F:\Pices\SipperFiles\SMP751001  -c GulfOilBroad2_Discreate6_Dual.cfg  -Min 150

// -RootDir F:\Pices\SipperFiles\GG1002  -c GulfOilBroad2_Discreate6_Dual.cfg  -Min 150

// -RootDir F:\Pices\SipperFiles\GG1002\GG1002001S  -c GulfOilBroad_130314_MFS.cfg  -Min 250
// -RootDir F:\Pices\SipperFiles\GG1002\GG1002001S  -c NRDA_GG1002_37_Dual.cfg  -Min 250
// -RootDir C:\Pices\SipperFiles\WB0213  -c NRDA_small_Dual.cfg  -Min 250
// -RootDir C:\Pices\SipperFiles\WB0213  -c NRDA_small_Dual.cfg  -Min 250  -start WB0213PCB02_06.spr

// -RootDir   C:\Pices\SipperFiles\WB0813  -c NRDA_SMP751001_37_BFS  -Min 250 

// -RootDir   "F:\Pices\SipperFiles\WB0813\WB0813PCB05" -c Gulf_no_oil_MFS -min 250

// -RootDir  F:\Pices\SipperFiles  -c Gulf_no_oil_MFS -min 250

// -RootDir F:\Pices\SipperFiles\USF\WB0813S4\WB0813S4-DSH10  -c Gulf_no_oil_MFS -min 500

// -RootDir K:\Pices\SipperFiles\WB0512\DSH09  -min 250 -c USF_Dual.cfg

// -RootDir F:\Pices\SipperFiles\NOAA  -min 250 -c  NRDA_SD0102_Dual

// -RootDir F:\Pices\SipperFiles\WB1008\DSH09  -min 100  -c USF_Dual_MFS-BFS_current 



// -RootDir F:\Pices\SipperFiles\NOAA  -min 100   -c USF_Dual_MFS-BFS_current 

ExtractAllSipperFiles::ExtractAllSipperFiles ():
   PicesApplication    (),
   firstOneFound       (false),
   startSipperFileName (),
   maxSize             (0),
   minSize             (250),
   rootDir             ()

{
}



ExtractAllSipperFiles::~ExtractAllSipperFiles ()
{
}



void  ExtractAllSipperFiles::InitalizeApplication (kkint32 argc,
                                                   char**  argv
                                                  )
{
  ConfigRequired (true);
  DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (!Config ())
  {
    log.Level (-1) << endl
      << "ExtractAllSipperFiles::InitalizeApplication   ***ERROR***   A valid Training Model Configuration is required  '-Config'." << endl
      << endl;
    Abort (true);
  }
}  /* InitalizeApplication */






bool  ExtractAllSipperFiles::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                      const KKStr&  parmValue
                                                     )
{
  bool  validParm = true;

  if  (parmSwitch.EqualIgnoreCase ("-MinSize")  ||  parmSwitch.EqualIgnoreCase ("-min"))
  {
    minSize = parmValue.ToInt ();
  }

  else if  (parmSwitch.EqualIgnoreCase ("-MaxSize")  ||  parmSwitch.EqualIgnoreCase ("-max"))
  {
    maxSize = parmValue.ToInt ();
  }

  else if  (parmSwitch.EqualIgnoreCase ("-RootDir")  ||  parmSwitch.EqualIgnoreCase ("-Src")  ||  parmSwitch.EqualIgnoreCase ("-RD"))
  {
    rootDir = parmValue;
  }

  else if  (parmSwitch.EqualIgnoreCase ("-Start")  ||  parmSwitch.EqualIgnoreCase ("-StartSipperFileName")  ||  parmSwitch.EqualIgnoreCase ("-SSFN"))
  {
    startSipperFileName = parmValue;
  }

  else
  {
    validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue); 
  }

	return  validParm;
}  /* ProcessCmdLineParameter */




void  ExtractAllSipperFiles::DisplayCommandLineParameters ()
{
  cout << endl << endl << endl;

  cout << "ExtractAllSipperFiles"  << endl
       << "             -RootDir   <SubDir where sipper files are>"                                 << endl
       << "             -Config    <Configuration File To use.>"                                    << endl
       << "             -DataBase  <Database Description | Defaults to lasst specified database.>"  << endl
       << "             -MinSize   <Min size defaults to 250.>"                                     << endl
       << "             -MaxSize   <Max size defaults to Infinity.>"                                << endl
       << "             -Start     <Fisrt SIPPER file to start processing>"                         << endl
       << endl;
}



void  ExtractAllSipperFiles::ProcessADir (VectorKKStr&  list,
                                          KKStr         dirName
                                         )
{
  dirName = osAddSlash (dirName);
  KKStrListPtr  files = osGetListOfFiles (dirName + "*.spr");
  if  (files)
  {
    KKStrList::iterator  idx;
    for  (idx = files->begin ();  idx != files->end ();  idx++)
    {
      KKStr  fn = **idx;

      if  ((!firstOneFound)  &&  (fn.EqualIgnoreCase (startSipperFileName)))
        firstOneFound = true;

      if  (osGetFileExtension (fn).EqualIgnoreCase ("spr"))
      {
        if  (firstOneFound)
        {
          SipperFilePtr sf = DB()->SipperFileRecLoad (osGetRootName (fn));
          if  ((sf != NULL)  &&  (sf->ExtractionStatus () == '5'))
            list.push_back (dirName + fn);
          delete  sf;
          sf = NULL;

          //if  ((sf != NULL)  &&  
          //     ((sf->ExtractionStatus        () == '1')  ||
          //      (sf->ExtractionScanLineStart () > 0)     ||
          //      (sf->ExtractionScanLineEnd   () > 0)
          //     )
          //    )
          //  list.push_back (dirName + fn);
          //delete  sf;
          //  sf = NULL;
        }
      }
    }

    delete  files;
    files = NULL;
  }

  KKStrListPtr   dirs = osGetListOfDirectories (dirName);
  if  (dirs)
  {
    KKStrList::iterator  idx;
    for  (idx = dirs->begin ();  idx != dirs->end ();  idx++)
    {
      KKStr  subDirName = **idx;
      ProcessADir (list, dirName + subDirName);
    }

    delete  dirs;
  }
}  /* ProcessDir */



void  ExtractAllSipperFiles::RunOneImageExtraction (const KKStr&  sfn)
{
  // Make sure that Extraction has not been started or already ran.

  KKStr   rootName = osGetRootName (sfn);
  SipperFilePtr sf = DB()->SipperFileRecLoad (rootName);
  if  (sf == NULL)
  {
    log.Level (10) << "RunOneImageExtraction   SipperFile: " << rootName << "   Extraction Status: " << sf->ExtractionStatus () << "  No entry in SipperFiles table." << endl;
    delete  sf;
    sf = NULL;
    return;
  }
  else
  {
    delete  sf;
    sf = NULL;
  }


  char extractionStatusBeforeUpdate = 0;
  char extractionStatusResult = 0;
  bool extrtactionSatusUpdated = false;

  DB()->SipperFilesUpdateExtractionStatusIfExpected (rootName, 
                                                     '5',   // extractionStatusExpected,
                                                     '1',   // extractionStatusNew,
                                                     extractionStatusBeforeUpdate,
                                                     extractionStatusResult,
                                                     extrtactionSatusUpdated
                                                    );

  if  (!extrtactionSatusUpdated)
  {
    log.Level (10) << "ExtractAllSipperFiles::RunOneImageExtraction   '" + rootName + "'  status already updated.";
  }
  else
  {
    KKStr  destDir = osAddSlash (osAddSlash (PicesVariables::HomeDir ()) + "ExtractedImages") + osGetRootName (sfn);

    KKStr  cmd(1000);

    KKStr  progName = osAddSlash (osAddSlash (PicesVariables::HomeDir ()) + "exe") + "ImageExtractionWindows.exe";

    cmd << progName                            << "  "
        << "-s"      << " " << sfn             << "  "
        << "-min"    << " " << minSize         << "  ";

    if  (maxSize > 0)
      cmd << "-max"  << " " << maxSize         << "  ";

    cmd << "-co"                               << "  "
        << "-d"      << " " << destDir         << "  "
        << "-c"      << " " << configFileName  << "  "
        << "-x";

    system (cmd.Str ());
  }
}  /* RunOneImageExtraction */

 

void  ExtractAllSipperFiles::Run ()
{
  if  (startSipperFileName.Empty ())
    firstOneFound = true;
  else
    firstOneFound = false;

  if  (rootDir.Empty ())
    rootDir = KKB::osGetCurrentDirectory ();

  if  (configFileName.Empty ())
  {
    log.Level (-1) << endl 
      << "ExtractAllSipperFiles::Run  ***ERROR***  A training model must be specified  \"-Config\"  parameter" << endl
      << endl;
    return;
  }

  if  (!DB()->Valid ())
  {
    log.Level (-1) << endl << endl 
      << "ExtractAllSipperFiles::Run    ***ERROR***    Failed to connect to database."  << endl
      << endl;
    Abort (true);
    return;
  }

  VectorKKStr  listOfSipperFiles;

  ProcessADir (listOfSipperFiles, rootDir);

  int  fileNum = 1;
  VectorKKStr::iterator idx;
  for  (idx = listOfSipperFiles.begin ();  idx != listOfSipperFiles.end ();  idx++)
  {
    cout << endl << endl << endl
         << "File " << fileNum << "  of  " << listOfSipperFiles.size () << "  " << *idx << endl
         << endl;

    RunOneImageExtraction (*idx);
    fileNum++;
  }

  if  (DB ())
  {
    DB()->Close ();
  }
}  /* Run */





int  main (int     argc, 
           char**  argv
          )
{
  RunLog  log;
 
  ExtractAllSipperFiles  application;

  application.InitalizeApplication (argc, argv);
  if  (!application.Abort ())
  {
    application.Run ();
  }

  return  0;
}
