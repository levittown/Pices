#include "FirstIncludes.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "MemoryDebug.h"

using namespace std;


#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "ImageIO.h"
#include "OSservices.h"
using namespace KKB;


#include "InstrumentDataFileManager.h"

#include "InstrumentData.h"
#include "InstrumentDataList.h"
#include "SipperBuff.h"
#include "PicesVariables.h"

using namespace  MLL;

InstrumentDataFileManager::StringLookUpTablePtr   
                       InstrumentDataFileManager::knownSipperFiles  = NULL;

VectorKKStr*           InstrumentDataFileManager::sipperDirectories = NULL;




InstrumentDataFileManager::InstrumentDataFileManager ():
   currentDataFile (NULL)

{
  InitializeBlocker ();

  blocker->StartBlock ();

  picesHomeDir = PicesVariables::HomeDir ();

  instrumentDataFilesDir = PicesVariables::InstrumentDataFilesDir ();
  osCreateDirectoryPath (instrumentDataFilesDir);

  if  (!sipperDirectories)
    LoadSipperDirectoryNames ();

  if  (!knownSipperFiles)
    LoadKnownSipperFileNames ();

  blocker->EndBlock ();
}




InstrumentDataFileManager::~InstrumentDataFileManager ()
{
  blocker->StartBlock ();

  DataFileList::iterator  idx;
  for  (idx = dataFiles.begin ();  idx != dataFiles.end ();  idx++)
  {
    InstrumentDataListPtr  idl = idx->second;
    delete  idl;  idl = NULL;
  }

  blocker->EndBlock ();
}



kkint32 InstrumentDataFileManager::initializedStackSize = 0;


GoalKeeperPtr   InstrumentDataFileManager::blocker = NULL;



InstrumentDataFileManager*    InstrumentDataFileManager::instrumentDataManager = NULL;



void  InstrumentDataFileManager::InitializeBlocker ()
{
  if  (blocker == NULL)
    GoalKeeper::Create ("InstrumentDataFileManager", blocker);
}



void  InstrumentDataFileManager::Initialize ()
{
  InitializePush ();
}



void  InstrumentDataFileManager::FinalCleanUp ()
{
  while  (initializedStackSize > 0)
    InitializePop ();
  delete  knownSipperFiles;      knownSipperFiles  = NULL;
  delete  sipperDirectories;     sipperDirectories = NULL;
  GoalKeeper::Destroy(blocker);  blocker           = NULL;
}  /* CleanUp */




bool  InstrumentDataFileManager::Initialized ()
{
  return  (instrumentDataManager != NULL);
}  /* Initialized */



void  InstrumentDataFileManager::InitializePush ()
{
  InitializeBlocker ();

  blocker->StartBlock ();

  if  (instrumentDataManager)
  {
    initializedStackSize++;
  }
  else
  {
    initializedStackSize = 1;
    instrumentDataManager = new InstrumentDataFileManager ();
  }

  blocker->EndBlock ();
}  /* InitializePush */



void  InstrumentDataFileManager::InitializePop  ()
{
  InitializeBlocker ();

  blocker->StartBlock ();

  initializedStackSize--;
  if  (initializedStackSize < 1)
  {
    delete  instrumentDataManager;  
    instrumentDataManager = NULL;
    initializedStackSize = 0;
  }

  blocker->EndBlock ();
}  /* InitializePop */



KKStr  InstrumentDataFileManager::GetFullSipperFileName (const KKStr&  rootName)
{
  KKStr  fullSipperFileName = "";

  InitializeBlocker ();

  blocker->StartBlock ();

  if  (knownSipperFiles == NULL)
    LoadKnownSipperFileNames ();

  StringLookUpTable::iterator  idx;

  idx = knownSipperFiles->find (rootName);
  if  (idx == knownSipperFiles->end ())
    fullSipperFileName = KKStr::EmptyStr ();
  else
    fullSipperFileName = idx->second;

  blocker->EndBlock ();

  return  fullSipperFileName;
}  /* GetFullSuperFileName*/




/**
 @brief Returns a list of all known SIPPER files on the local PC; each entry includes the file path.  
 */
VectorKKStr  InstrumentDataFileManager::GetListOfSipperFiles (RunLog&  log)
{
  InitializeBlocker ();

  blocker->StartBlock ();

  if  (instrumentDataManager == NULL)
    Initialize ();

  VectorKKStr sipperFileList;

  StringLookUpTable::iterator  idx;
  for  (idx = instrumentDataManager->knownSipperFiles->begin ();  idx != instrumentDataManager->knownSipperFiles->end ();  idx++)
    sipperFileList.push_back (idx->second);

  blocker->EndBlock ();

  return  sipperFileList;
}  /* GetListOfSipperFiles */




  
KKStr  InstrumentDataFileManager::SipperFileRootNameFromSipperImageFileName (const KKStr&  sipperFileName)
{
  KKStr rootFileName = osGetRootName (sipperFileName);
  auto  x = rootFileName.LocateLastOccurrence ('_');

  if  (x < 1)
    return  "";

  KKStr  lessCollumnNum = rootFileName.SubStrPart (0, x - 1);
  x = lessCollumnNum.LocateLastOccurrence ('_');
  if  (x < 1)
    return "";

  KKStr  sipperRootName = lessCollumnNum.SubStrPart (0, x - 1);
  return  sipperRootName;
}  /* SipperFileRootNameFromSipperImageFileName */




InstrumentDataPtr  InstrumentDataFileManager::GetClosestInstrumentData (const KKStr&   imageFileName,
                                                                        VolConstBool&  cancelFlag,
                                                                        RunLog&        log
                                                                       )
{
  InstrumentDataPtr  instrumentData = NULL;
  if  (instrumentDataManager == NULL)
    return  NULL;

  instrumentDataManager->GetClosestInstrumentData (imageFileName, instrumentData, cancelFlag, log);
  return  instrumentData;
}  /* GetClosestInstrumentData */



InstrumentDataPtr  InstrumentDataFileManager::GetClosestInstrumentData (const KKStr&   sipperFileRootName,
                                                                        kkuint32       scanLine,
                                                                        VolConstBool&  cancelFlag,
                                                                        RunLog&        log
                                                                       )
{
  InstrumentDataPtr  instrumentData = NULL;
  if  (instrumentDataManager == NULL)
    return  NULL;

  instrumentDataManager->GetClosestInstrumentData (sipperFileRootName, scanLine, instrumentData, cancelFlag, log);
  return  instrumentData;
}





void  InstrumentDataFileManager::GetClosestInstrumentData (const KKStr&                                                                                                                                 imageFileName,
                                                           InstrumentDataPtr&  instrumentData,
                                                           VolConstBool&            cancelFlag,
                                                           RunLog&                  log
                                                          )
{
  KKStr  sipperFileName;
  kkuint32  scanLine = 0;
  kkuint32  scanCol  = 0;

  PicesVariables::ParseImageFileName (imageFileName, sipperFileName, scanLine, scanCol);
  scanLine = Max ((kkuint32)30000, scanLine);

  {
    // Lets see if we can get what we want from the current data file.

    // Get a copy of the 'currentDataFile'  into a local stack allocated variable
    // this way if a different thread changes it will not impact this thread.
    InstrumentDataListPtr  tempDataFile = currentDataFile;  
                                                            
    if  (tempDataFile)
    {
      if  (tempDataFile->SipperFileName () == sipperFileName)
      {
        instrumentData = tempDataFile->SearchForNearestScanLine (scanLine);
        // We are done;  we have the instrument data that we are looking for.
        return;
      }
    }
  }


  {
    blocker->StartBlock ();

    if  (currentDataFile)
    {
      if  (currentDataFile->SipperFileName () != sipperFileName)
        currentDataFile = NULL;
    }

    if  (!currentDataFile)
      currentDataFile = GetInstrumentDataListForSipperFile (sipperFileName, NULL, false, cancelFlag, log);

    if  (!currentDataFile)
    {
      instrumentData = NULL;
    }
    else
    {
      instrumentData = currentDataFile->SearchForNearestScanLine (scanLine);
    }

    blocker->EndBlock ();
  }
  return;
}  /* GetClosestInstrumentData */



void  InstrumentDataFileManager::GetClosestInstrumentData (const KKStr&        sipperFileRootName,
                                                           kkuint32            scanLine,
                                                           InstrumentDataPtr&  instrumentData,
                                                           VolConstBool&       cancelFlag,
                                                           RunLog&             log
                                                          )
{
  scanLine = Max ((kkuint32)30000, scanLine);

  {
    // Lets see if we can get what we want from the current data file.

    // Get a copy of the 'currentDataFile'  into a local stack allocated variable
    // this way if a different thread changes it will not impact this thread.
    InstrumentDataListPtr  tempDataFile = currentDataFile;  
                                                            
    if  (tempDataFile)
    {
      if  (tempDataFile->SipperFileName () == sipperFileRootName)
      {
        instrumentData = tempDataFile->SearchForNearestScanLine (scanLine);
        // We are done;  we have the instrument data that we are looking for.
        return;
      }
    }
  }


  {
    blocker->StartBlock ();

    if  (currentDataFile)
    {
      if  (currentDataFile->SipperFileName () != sipperFileRootName)
        currentDataFile = NULL;
    }

    if  (!currentDataFile)
      currentDataFile = GetInstrumentDataListForSipperFile (sipperFileRootName, NULL, false, cancelFlag, log);

    if  (!currentDataFile)
    {
      instrumentData = NULL;
    }
    else
    {
      instrumentData = currentDataFile->SearchForNearestScanLine (scanLine);
    }

    blocker->EndBlock ();
  }
  return;
}  /* GetClosestInstrumentData */





void  InstrumentDataFileManager::LoadSipperDirectoryNames ()
{
  delete sipperDirectories;
  sipperDirectories = new VectorKKStr ();

  KKStr  sipperDirectoryListFileName = PicesVariables::SipperDirectoryListFileName ();

  sipperDirectories->push_back (osAddSlash (PicesVariables::SipperFilesDefaultDir ()));

  FILE*  in = osFOPEN (sipperDirectoryListFileName.Str (), "r");
  if  (in == NULL)
  {
    return;
  }

  VectorKKStr::iterator   idx;

  char  buff[10240];

  while  (fgets (buff, sizeof (buff), in))
  {
    KKStr  ln (buff);
    if  (ln.Empty ())
      continue;

    if  (ln.SubStrPart (0, 1) == "//")
      continue;

    ln.TrimLeft ();
    ln.TrimRight ();

    if  (ln.Empty ())
      continue;

    ln = osAddSlash (ln);

    // make sure we don't already have this directory in the list.
    for  (idx = sipperDirectories->begin ();  idx != sipperDirectories->end ();  idx++)
    {
      if  (ln.EqualIgnoreCase (*idx))
      {
        // We already have this directory entry in the list.
        break;
      }
    }

    if  (idx == sipperDirectories->end ())
      sipperDirectories->push_back (ln);
  }

  fclose  (in);
}  /* LoadSipperDirectoryNames */




void  InstrumentDataFileManager::LoadKnownSipperFileNames ()
{
  delete  knownSipperFiles;
  knownSipperFiles = NULL;

  if  (!sipperDirectories)
    LoadSipperDirectoryNames ();

  knownSipperFiles = new StringLookUpTable ();

  VectorKKStr::iterator  idx;
  VectorKKStr            sipperFilesNamesFound;
  for  (idx = sipperDirectories->begin ();   idx != sipperDirectories->end ();  idx++)
  {
    KKStr  sipperDirName = *idx;
    osGetListOfFilesInDirectoryTree (sipperDirName, "*.spr", sipperFilesNamesFound);
  }

  for  (idx = sipperFilesNamesFound.begin ();  idx != sipperFilesNamesFound.end ();  idx++)
  {
    KKStr  fullFileName = *idx;
    KKStr  rootName = osGetRootName (fullFileName);

    StringLookUpTable::iterator  existingNameIdx = knownSipperFiles->find (rootName);
    if  (existingNameIdx == knownSipperFiles->end ())
    {
      knownSipperFiles->insert (pair<KKStr,KKStr> (rootName, fullFileName));
    }
  }
  atexit (InstrumentDataFileManager::FinalCleanUp);
}  /* LoadKnownSipperFileNames */





InstrumentDataListPtr   InstrumentDataFileManager::GetInstrumentDataForSipperFile (const KKStr&    sipperFileName,
                                                                                   SipperFilePtr   sipperFile,
                                                                                   bool            forceReExtract,
                                                                                   VolConstBool&   cancelFlag,
                                                                                   RunLog&         log
                                                                                  )
{
  if  (instrumentDataManager == NULL)
    Initialize ();  

  KKStr  siperRootName = osGetRootName (sipperFileName);

  InstrumentDataListPtr  data = instrumentDataManager->GetInstrumentDataListForSipperFile (siperRootName, sipperFile, forceReExtract, cancelFlag, log);
  return data;
}  /* GetInstrumentDataForSipperFile */




InstrumentDataListPtr   InstrumentDataFileManager::ReExtractInstrumentDataForSipperFile (const KKStr&   sipperFileName,
                                                                                         SipperFilePtr  sipperFile,
                                                                                         VolConstBool&  cancelFlag,
                                                                                         RunLog&        log
                                                                                        )
{
  if  (instrumentDataManager == NULL)
    Initialize ();  

  KKStr  siperRootName = osGetRootName (sipperFileName);

  InstrumentDataListPtr  data = instrumentDataManager->GetInstrumentDataListForSipperFile (siperRootName, sipperFile, true, cancelFlag, log);
  return data;
}  /* ReExtractInstrumentDataForSipperFile */





InstrumentDataListPtr  InstrumentDataFileManager::GetInstrumentDataListForSipperFile (const KKStr&   sipperRootName,
                                                                                      SipperFilePtr  sipperFile,
                                                                                      bool           forceReExtract,
                                                                                      VolConstBool&  cancelFlag,
                                                                                      RunLog&        log
                                                                                     )
{
  if  (cancelFlag)
    return NULL;

  InstrumentDataListPtr  dataList = NULL;

  if  (!forceReExtract)
  {
    // Determine if we have already located this IsntrumentData file before.
    DataFileList::iterator  idx;
    idx = dataFiles.find (sipperRootName);
    if  (idx != dataFiles.end ())
    {
      // Wonderful we have already looked for this file before.
      return  idx->second;
    }
  }

  KKStr  instrumentDataListFileName = osAddSlash (instrumentDataFilesDir) + sipperRootName + "_InstrumentData.txt";
   
  if  ((!forceReExtract)  &&  (osFileExists (instrumentDataListFileName)))
  {
    // Looks like we have a data file that we can load the data from.
    bool  loadWasSucessfull = false;
    dataList = new InstrumentDataList (instrumentDataListFileName, loadWasSucessfull, log);
    if  (loadWasSucessfull)
    {
      dataFiles.insert (DataFileListPair (sipperRootName, dataList));
      return  dataList;
    }

    // We could not load the data file;
    delete  dataList;  dataList = NULL;
  }

  {
    // Since we still have not found it yet; lets hope that we can get if from the SIPPER file
    StringLookUpTable::iterator  idx;
    idx = knownSipperFiles->find (sipperRootName);
    if  (idx == knownSipperFiles->end ())
    {
      // Since we did all the work to find out that the SIPPER file does not even exist;  we will create an 
      // entry in 'dataFiles' with a 'NULL' pointer to act as a flag.
      dataFiles.insert (DataFileListPair (sipperRootName, NULL));
      return  NULL;
    }
    else
    {
      if  (sipperFile == NULL)
        sipperFile = SipperFileOracle::GetSipperFileEntry (sipperRootName);
      KKStr  fullSipperFileName = idx->second;
      dataList = InstrumentDataList::CreateFromSipperFile (fullSipperFileName, sipperFile, cancelFlag, log);
      if  (dataList == NULL)
      {
        // For what ever reason we could not load from the SIPPER file so we need to remove from file name list
        knownSipperFiles->erase (idx);
        // Since we did all the work to find out that the SIPPER file does not even exist;  we will create an 
        // entry in 'dataFiles' with a 'NULL' pointer to act as a flag.
        dataFiles.insert (DataFileListPair (sipperRootName, NULL));
        return  NULL;
      }
      else if  (!cancelFlag)
      {
        dataList->Save (instrumentDataListFileName, log);
        dataFiles.insert (DataFileListPair (sipperRootName, dataList));
        return  dataList;
      }
      else
      {
        delete  dataList;
        dataList = NULL;
        return dataList;
      }
    }
  }

  // We could not find this data anywhere     SORRY
  return  NULL;
}  /* GetInstrumentDataListForSipperFile */




VectorFloat  InstrumentDataFileManager::GetVolumePerMeterProfile2 (const KKStr&   sipperFileName,
                                                                   SipperFilePtr  sipperFile,
                                                                   VolConstBool&  cancelFlag,
                                                                   RunLog&        log
                                                                  )
{
  KKStr  sipperRootName = osGetRootName (sipperFileName);

  if  (currentDataFile)
  {
    if  (currentDataFile->SipperFileName () != sipperRootName)
      currentDataFile = NULL;
  }

  if  (!currentDataFile)
    currentDataFile = GetInstrumentDataListForSipperFile (sipperRootName, sipperFile, false, cancelFlag, log);

  if  ((!currentDataFile)  ||  cancelFlag)
  {
    return  VectorFloat ();
  }

  return  currentDataFile->GetVolumePerMeterProfile (sipperFile, cancelFlag, log);
}  /* GetVolumePerMeterProfile2 */




VectorUlong  InstrumentDataFileManager::GetScanLinesPerMeterProfile2 (const KKStr&   sipperFileName,
                                                                      SipperFilePtr  sipperFile,
                                                                      VolConstBool&  cancelFlag,
                                                                      RunLog&        log
                                                                     )
{
  KKStr  sipperRootName = osGetRootName (sipperFileName);

  if  (currentDataFile)
  {
    if  (currentDataFile->SipperFileName () != sipperRootName)
      currentDataFile = NULL;
  }

  if  (!currentDataFile)
    currentDataFile = GetInstrumentDataListForSipperFile (sipperRootName, sipperFile, false, cancelFlag, log);

  if  (!currentDataFile)
  {
    return  VectorUlong ();
  }

  return  currentDataFile->GetScanLinesPerMeterProfile ();
}  /* GetScanLinesPerMeterProfile2 */




VectorFloat  InstrumentDataFileManager::GetVolumePerMeterProfile (const KKStr&   sipperFileName,
                                                                  SipperFilePtr  sipperFile,
                                                                  VolConstBool&  cancelFlag,
                                                                  RunLog&        log
                                                                 )
{
  if  (instrumentDataManager == NULL)
    Initialize ();

  return  instrumentDataManager->GetVolumePerMeterProfile2 (sipperFileName, sipperFile, cancelFlag, log);
}  /* GetVolumePerMeterProfile */




VectorUlong  InstrumentDataFileManager::GetScanLinesPerMeterProfile (const KKStr&   sipperFileName,
                                                                     SipperFilePtr  sipperFile,
                                                                     VolConstBool&  cancelFlag,
                                                                     RunLog&        log
                                                                    )
{
  if  (instrumentDataManager == NULL)
    Initialize ();

  return  instrumentDataManager->GetScanLinesPerMeterProfile2 (sipperFileName, sipperFile, cancelFlag, log);
}  /* GetScanLinesPerMeterProfile */





RasterSipperPtr  InstrumentDataFileManager::GetOrigSipperImage (const KKStr&  sipperFileRootName,
                                                                kkuint64      byteOffset,
                                                                kkuint32      topLeftRow,
                                                                kkuint32      topLeftCol,
                                                                kkuint32      height,
                                                                kkuint32      width,
                                                                uchar         connectedComponentDist,
                                                                RunLog&       log
                                                               )
{
  if  ((height < 1)  ||  (width < 1))
    return NULL;

  if  (instrumentDataManager == NULL)
    Initialize ();

  KKStr  fullSipperFileName = instrumentDataManager->InstrumentDataFileManager::GetFullSipperFileName (sipperFileRootName);
  if  (fullSipperFileName.Empty())
    return NULL;

  SipperBuffPtr  sf = SipperBuff::CreateSipperBuff (fullSipperFileName, 0, NULL, log);
  if  (!sf)
    return NULL;

  sf->SkipToScanLine (topLeftRow, byteOffset);
  if  (sf->Eof ())
    return NULL;

  // because we are passing raster with 3 pixels on all sides we need separate variables that contain the 
  // adjusted raster Height and Width to determine when we have read enough columns and rows from the 
  // Sipper file.
  kkuint32  heightAdj = height + 3;
  kkuint32  widthAdj  = width  + 3;

  RasterSipperPtr  r = new RasterSipper (height + 6, width + 6);  //   Going 2 pad the edges with at least one blank row and column.

  kkuint32 lineBuffSize = 4096;

  kkuint32  endSipperRow =  topLeftRow + height - 1;
  kkuint32  endSipperCol =  topLeftCol + width  - 1;


  uchar*  lineBuff  = new uchar[lineBuffSize + 1];
  kkuint32* colCount  = new kkuint32 [lineBuffSize + 1];
  kkuint32  lineSize  = 0;
  kkuint32  pixelsInRow;
  bool    flow;

  kkuint32  rasterRow = 3;   // Starting row and column off with 1 so as to have one
  kkuint32  rasterCol = 3;   // padded rows and column n top and left side.
                             // will also pad bottom row and right column.
   
  kkuint32  sipperCol = 0;

  for  (sipperCol = 0;  sipperCol < lineBuffSize;  sipperCol++)
    colCount[sipperCol] = 0;

  uchar**  green = r->Green ();

  sf->GetNextLine (lineBuff, lineBuffSize, lineSize, colCount, pixelsInRow, flow);
  while  ((sf->CurRow () <= endSipperRow)  &&  (rasterRow  <= heightAdj)  &&  (!sf->Eof ()))
  {
    rasterCol = 3;  // Padding one blank left column
    sipperCol = topLeftCol;

    while  ((rasterCol <= widthAdj)  &&  (sipperCol <= endSipperCol))
    {
      green[rasterRow][rasterCol] = lineBuff[sipperCol];
      sipperCol++;
      rasterCol++;
    }

    rasterRow++;

    sf->GetNextLine (lineBuff, lineBuffSize, lineSize, colCount, pixelsInRow, flow);
  }

  delete  lineBuff;   lineBuff = NULL;
  delete  colCount;   colCount = NULL;

  delete  sf;
  sf = NULL;

  // SaveImageGrayscaleInverted4Bit (*r, "C:\\Temp\\" + sipperFileRootName + "_BeforeConnectedComponent.bmp");

  r->ReduceToMostCompleteBlob (connectedComponentDist);

  return  r;
}  /* GetOrigSipperImage */
