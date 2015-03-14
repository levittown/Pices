#ifndef  _SIPPERIMAGEEXTRACTIONPARMS_
#define  _SIPPERIMAGEEXTRACTIONPARMS_

//**************************************************************************
//*                          SipperImageExtraction                         *
//*                                                                        *
//*                                                                        *
//*                                                                        *
//* <p>Copyright: Copyright (c) 2004</p>                                   *
//* <p>author     Kurt Kramer                                              * 
//*                                                                        *
//*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
//*                                                                        *
//*  February 2004  Modifying the Image Extraction such that it can be     *   
//*  Kurt Kramer    used by several different programs.                    *
//*                                                                        *
//*                                                                        *
//**************************************************************************


#include  "KKBaseTypes.h"
#include  "CmdLineExpander.h"
#include  "KKStr.h"

#include  "SipperBuff.h"

#include  "ClassStatistic.h"
#include  "DataBaseServer.h"
#include  "MLClass.h"
#include  "SipperExtractionImageManager.h"
using namespace MLL;

#ifndef  _RUNLOG_
class RunLog;
#endif



#define   DefaultConnectedPixelDist  3



class  SipperImageExtractionParms
{
public:
  typedef  SipperHardware::SipperFileFormat  SipperFileFormat;

  SipperImageExtractionParms (int      argc,
                              char**   argv,
                              bool     _runningAsAWindowsApp,
                              KKStr&   errorMessage,
                              bool&    formatGood,
                              RunLog&  _log
                             );

  SipperImageExtractionParms (KKStr    progName,
                              KKStr    cmdLine,
                              bool     _runningAsAWindowsApp,
                              KKStr&   errorMessage,
                              bool&    formatGood,
                              RunLog&  _log
                             );

  ~SipperImageExtractionParms ();


  void     Cancel (bool _cancel)  {cancel = _cancel;}

  KKStr    CmdLine ()  const;

  void     DisplayExampleCmdLine ();

  void     DoneExecuting   (bool  _doneExecuting)  
  {
    doneExecuting = _doneExecuting;
  }

  bool     RunningAsAWindowsApp ()  {return  runningAsAWindowsApp;}

  void     ParseCmdLine (const CmdLineExpander&  cmdLineExpander,
                         KKStr&                  errorMessage,
                         bool&                   formatGood
                        );


  uchar              CameraNum             () const  {return  cameraNum;}
                 
  bool               Cancel                () const  {return  cancel;}
                 
                 
  bool&              CancelField           ()        {return  cancel;}
                 
  bool               Colorize              () const  {return  colorize;}
                 
  const          
  KKStr&             ConfigFileName        () const  {return  configFileName;}
                 
  const          
  KKStr&             ConfigFileNameSpecified () const {return configFileNameSpecified;}
                 
  kkuint32           ConnectedPixelDist    () const  {return  connectedPixelDist;}
                 
  bool               CountOnly             () const  {return  countOnly;}

  DataBaseServerPtr  DataBaseServer        () const  {return  dataBaseServer;}

  KKStr              DataBaseServerDesc    () const;

  bool               DoneExecuting         () const  {return  doneExecuting;}

  bool               ExtractFeatureData    () const  {return  extractFeatureData;}

  SipperFileFormat   FileFormat            () const  {return  fileFormat;}

  const           
  KKStr&             FlowRateFileName      () const  {return  flowRateFileName;}

  bool               FramesOnly            () const  {return  framesOnly;}

  kkuint32           ImagesPerDirectory    () const  {return  imagesPerDirectory;}

  kkuint32           MinImageSize          () const  {return  minImageSize;}

  kkuint32           MaxImageSize          () const  {return  maxImageSize;}

  const KKStr&       MorphOperations       () const  {return  morphOperations;}

  bool               MultiThreaded         () const  {return  multiThreaded;}

  const           
  KKStr&             OutputRootDir         () const  {return  outputRootDir;}

  bool               PreProcess            () const  {return  preProcess;}

  const
  KKStr&             ReportFileName        () const  {return  reportFileName;}

  bool               RefreshDataBaseImages () const  {return  refreshDataBaseImages;}

  const          
  KKStr&             SipperFileName        () const  {return  sipperFileName;}

  kkuint32           ScanLineEnd           () const  {return  scanLineEnd;}

  kkuint32           ScanLineStart         () const  {return  scanLineStart;}

  bool               SaveFrames            () const  {return  saveFrames;}

  bool               SaveFramesAfter       () const  {return  saveFramesAfter;}

  bool               UpdateDataBase        () const  {return  updateDataBase;}


  // Runtime Stats
  ClassStatisticListPtr  ClassStats        () const  {return  classStats;     }

  kkuint64           BytesExtracted        () const  {return  bytesExtracted; }

  kkuint32           LinesExtracted        () const  {return  linesExtracted; }

  kkuint32           ImagesExtracted       () const  {return  imagesExtracted;}

  kkuint32           ImagesWritten         () const  {return  imagesWritten;  }

  kkuint32           PixelsRead            () const  {return  pixelsRead;     }

  kkuint32           PixelsWritten         () const  {return  pixelsWritten;  }

  RunLog&            Log                   () const  {return   log;}

  kkuint64           SipperFileSize        () const  {return  sipperFileSize;   }

  const          
  KKStr&             StatusMessage         () const  {return  statusMessage;}

  KKStr&             StatusMessageField    ()        {return statusMessage;}


  //  Update RunTime Stats
  void             ClassStats      (ClassStatisticListPtr  _classStats);  // Will take ownership of '_classStats'

  void             ImagesPerDirectory (uint  _imagesPerDirectory)   {imagesPerDirectory = _imagesPerDirectory;}

  void             IncrementBytesExtracted  (uint _bytesExtracted)  {bytesExtracted  += _bytesExtracted;}

  kkuint32         IncrementImagesExtracted () {return  (++imagesExtracted);}

  kkuint32         IncrementImagesWritten   () {return  (++imagesWritten);}

  kkuint32         IncrementLinesExtracted  () {return  (++linesExtracted);}

  void             IncrementPixelsRead      (uint _pixelsRead)      {pixelsRead      += _pixelsRead;}

  void             IncrementPixelsWritten   (uint _pixelsWritten)   {pixelsWritten   += _pixelsWritten;}

  void             BytesExtracted  (kkuint64 _bytesExtracted)  {bytesExtracted  = _bytesExtracted;}

  void             LinesExtracted  (uint _linesExtracted)  {linesExtracted  = _linesExtracted;}

  void             ImagesExtracted (uint _imagesExtracted) {imagesExtracted = _imagesExtracted;}

  void             ImagesWritten   (uint _imagesWritten)   {imagesWritten   = _imagesWritten;}

  void             PixelsRead      (uint _pixelsRead)      {pixelsRead      = _pixelsRead;}

  void             PixelsWritten   (uint _pixelsWritten)   {pixelsWritten   = _pixelsWritten;}

  void             StatusMessage   (const KKStr& _statusMessage)  {statusMessage = _statusMessage;}




private:
  void  SetupOtherFileNames ();

  uchar              cameraNum;
  bool               cancel;
  bool               colorize;
  KKStr              configFileName;
  KKStr              configFileNameSpecified;
  kkuint32           connectedPixelDist;
  bool               countOnly;
  DataBaseServerPtr  dataBaseServer;
  bool               doneExecuting;
  bool               extractFeatureData;
  SipperFileFormat   fileFormat;
  KKStr              flowRateFileName;
  bool               framesOnly;
  kkuint32           imagesPerDirectory;
  RunLog&            log;
  kkuint32           maxImageSize;
  kkuint32           minImageSize;
  KKStr              morphOperations;
  bool               multiThreaded;
  KKStr              outputRootDir;
  bool               preProcess;
  bool               refreshDataBaseImages;
  KKStr              reportFileName;
  bool               runningAsAWindowsApp;
  bool               saveFrames;
  bool               saveFramesAfter;
  KKStr              sipperFileName;
  kkuint32           scanLineStart;
  kkuint32           scanLineEnd;
  KKStr              statusMessage;
  bool               updateDataBase;


  // Runtime Stats
  kkuint64               bytesExtracted;
  ClassStatisticListPtr  classStats;
  ClassStatisticListPtr  classStatsPrev;   // Used to help prevent issues with thread conflicts.
  kkuint32               imagesExtracted;
  kkuint32               imagesWritten;
  kkuint32               linesExtracted;
  kkuint32               pixelsRead;
  kkuint32               pixelsWritten;
  kkuint64               sipperFileSize;
};



typedef  SipperImageExtractionParms*  SipperImageExtractionParmsPtr;

#endif
