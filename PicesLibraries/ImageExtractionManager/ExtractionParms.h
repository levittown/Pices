#if  !defined(_IEM_EXTRACTIONPARMS_)
#define  _IEM_EXTRACTIONPARMS_

#include "KKBaseTypes.h"
#include "CmdLineExpander.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;

#include "ClassStatistic.h"
#include "DataBaseServer.h"
#include "MLClass.h"
#include "SipperBuff.h"
#include "SipperExtractionImageManager.h"
using namespace MLL;


#include "PreDefinedClasses.h"
using namespace ImageExtractionManager;


#define   DefaultConnectedPixelDist  3


namespace  ImageExtractionManager
{ 
  class  ExtractionParms
  {
  public:
    typedef  MLL::SipperFileFormat  SipperFileFormat;

    ExtractionParms (kkint32  argc,
                     char**   argv,
                     bool     _runningAsAWindowsApp,
                     KKStr&   errorMessage,
                     bool&    formatGood,
                     RunLog&  log
                    );

    ExtractionParms (KKStr    progName,
                     KKStr    cmdLine,
                     bool     _runningAsAWindowsApp,
                     KKStr&   errorMessage,
                     bool&    formatGood,
                     RunLog&  log
                    );

    ~ExtractionParms ();


    KKStr    CmdLine ()  const;

    void     DisplayExampleCmdLine ();

    void     DoneExecuting   (bool  _doneExecuting)   {doneExecuting = _doneExecuting;}

    bool     RunningAsAWindowsApp ()  {return  runningAsAWindowsApp;}

    void     ParseCmdLine (const CmdLineExpander&  cmdLineExpander,
                           KKStr&                  errorMessage,
                           bool&                   formatGood,
                           RunLog&                 log
                          );

    // Parameter Values
    uchar              CameraNum                () const  {return  cameraNum;}
    bool               Colorize                 () const  {return  colorize;}
    const KKStr&       ConfigFileName           () const  {return  configFileName;}
    const KKStr&       ConfigFileNameSpecified  () const  {return  configFileNameSpecified;}
    kkuint32           ConnectedPixelDist       () const  {return  connectedPixelDist;}
    bool               CountOnly                () const  {return  countOnly;}
    DataBaseServerPtr  DataBaseServer           () const  {return  dataBaseServer;}
    KKStr              DataBaseServerDesc       () const;
    bool               DoneExecuting            () const  {return  doneExecuting;}
    bool               ExtractFeatureData       () const  {return  extractFeatureData;}
    SipperFileFormat   FileFormat               () const  {return  fileFormat;}
    const KKStr&       FlowRateFileName         () const  {return  flowRateFileName;}
    bool               FramesOnly               () const  {return  framesOnly;}
    kkuint32           ImagesPerDirectory       () const  {return  imagesPerDirectory;}
    float              MinAreaSize              () const  {return  minAreaSize;}
    float              MaxAreaSize              () const  {return  maxAreaSize;}
    const KKStr&       MorphOperations          () const  {return  morphOperations;}
    bool               MultiThreaded            () const  {return  multiThreaded;}
    const KKStr&       OutputRootDir            () const  {return  outputRootDir;}
    bool               PreProcess               () const  {return  preProcess;}
    const KKStr&       ReportFileName           () const  {return  reportFileName;}
    bool               RefreshDataBaseImages    () const  {return  refreshDataBaseImages;}
    bool               SaveFrames               () const  {return  saveFrames;}
    bool               SaveFramesAfter          () const  {return  saveFramesAfter;}
    kkuint32           ScanLineEnd              () const  {return  scanLineEnd;}
    kkuint32           ScanLineStart            () const  {return  scanLineStart;}
    const KKStr&       SipperFileName           () const  {return  sipperFileName;}
    bool               UpdateDataBase           () const  {return  updateDataBase;}


    // Runtime Stats
    ClassStatisticListPtr  ClassStats      () const  {return  classStats;     }

    kkuint64          BytesExtracted       () const  {return  bytesExtracted; }
    kkuint32          LinesExtracted       () const  {return  linesExtracted; }
    kkuint32          ImagesExtracted      () const  {return  imagesExtracted;}
    kkuint32          ImagesWritten        () const  {return  imagesWritten;  }
    kkuint32          PixelsRead           () const  {return  pixelsRead;     }
    kkuint32          PixelsWritten        () const  {return  pixelsWritten;  }
    kkuint64          SipperFileSize       () const  {return  sipperFileSize; }
    const KKStr&      StatusMessage        () const  {return  statusMessage;}
    KKStr&            StatusMessageField   ()        {return  statusMessage;}



    //  Update RunTime Stats
    void             ClassStats      (ClassStatisticListPtr  _classStats);  // Will take ownership of '_classStats'
    kkuint32         IncrementImagesExtracted () {return  (++imagesExtracted);}
    kkuint32         IncrementImagesWritten   () {return  (++imagesWritten);}
    kkuint32         IncrementLinesExtracted  () {return  (++linesExtracted);}

    void             ImagesPerDirectory       (kkuint32 _imagesPerDirectory)   {imagesPerDirectory = _imagesPerDirectory;}
    void             IncrementBytesExtracted  (kkuint32 _bytesExtracted)       {bytesExtracted  += _bytesExtracted;}
    void             IncrementPixelsRead      (kkuint32 _pixelsRead)           {pixelsRead      += _pixelsRead;}
    void             IncrementPixelsWritten   (kkuint32 _pixelsWritten)        {pixelsWritten   += _pixelsWritten;}
    void             BytesExtracted           (kkuint64 _bytesExtracted)       {bytesExtracted  = _bytesExtracted;}
    void             LinesExtracted           (kkuint32 _linesExtracted)       {linesExtracted  = _linesExtracted;}
    void             ImagesExtracted          (kkuint32 _imagesExtracted)      {imagesExtracted = _imagesExtracted;}
    void             ImagesWritten            (kkuint32 _imagesWritten)        {imagesWritten   = _imagesWritten;}
    void             PixelsRead               (kkuint32 _pixelsRead)           {pixelsRead      = _pixelsRead;}
    void             PixelsWritten            (kkuint32 _pixelsWritten)        {pixelsWritten   = _pixelsWritten;}
    void             StatusMessage            (const KKStr& _statusMessage)    {statusMessage   = _statusMessage;}


  private:
    void  SetupOtherFileNames ();
    float ComputeAreaFromPixelsUsingDefaults (kkint32 pixelCount)  const;

    uchar              cameraNum;
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
    float              maxAreaSize;       /**< Area in mm^2 */
    float              minAreaSize;       /**< Area in mm^2 */
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
    kkuint32           scanLineEnd;
    kkuint32           scanLineStart;
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
  };  /* ExtractionParms*/
 
  typedef  ExtractionParms*  ExtractionParmsPtr;

}  /* ImageExtractionManager */

#endif
