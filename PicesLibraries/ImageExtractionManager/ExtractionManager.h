#if  !defined(_EXTRACTIONMANAGER_)
#define  _EXTRACTIONMANAGER_

#include <fstream>
#include <iostream>

#include "MsgQueue.h"
#include "RunLog.h"
using namespace  KKB;

#include "DuplicateImages.h"
#include "FactoryFVProducer.h"
#include "FileDesc.h"
using namespace  KKMLL;

#include "DataBase.h"
#include "DataBaseLogEntry.h"
#include "DataBaseImageValidatedEntry.h"
#include "SipperFile.h"
#include "SipperExtractionImageManager.h"
using namespace MLL;

#include "PreDefinedClasses.h"
#include "ExtractionParms.h"


namespace  ImageExtractionManager
{ 

  /**
   *@brief  Manages the extraction of images from a single Sipper File utilizing multiple threads.
   */
  class ExtractionManager
  {
  public:
    ExtractionManager (const KKStr&      _applicationName,
                       ExtractionParms&  _parms,
                       kkuint32          _maxNumThreads,
                       RunLog&           _log
                      );

    ~ExtractionManager ();

    const bool&           CancelFlag        ()  const  {return cancelFlag;}
    const bool&           Crashed           ()  const  {return crashed;}
    const bool&           DoneExecuting     ()  const  {return doneExecuting;}
    FactoryFVProducerPtr  FvProducerFactory ()  const  {return fvProducerFactory;}
    RunLog&               Log               ()  const  {return log;}
    kkuint32              LogEntryId        ()  const  {return logEntryId;}


    bool  AllProcessorsTerminated (ImageExtractionThreadListPtr  threads);

    bool  AnyProcessorsCrashed ();

    double  ChamberWidth ()  const;    /**< Physical width of chamber in meters. */

    void  TerminateProcessing (kkint32 miliSecsToWait);

    void  GetRunTimeStats (ExtractionManagerStats&  stats);

    void  GetRunTimeStats (kkuint64&  _bytesRead,
                           kkuint32&  _framesRead,
                           kkuint32&  _framesProcessed,
                           kkuint32&  _scanLinesProcessed,
                           kkuint32&  _imagesFound,
                           kkuint32&  _imagesClassified,
                           kkuint32&  _imagesUpdated,
                           kkuint32&  _duplicatesDetected,
                           kkuint32&  _updateFailures,
                           kkuint32&  _framesOnQueue,
                           kkuint32&  _imagesOnQueue
                          );

    /** 
     *@brief Returns a snapshot of the current class count statistics collected.
     *@details  Caller will get ownership of the instance of 'ClassStatisticList' so will be responsible
     *         for deleting when done.
     */
    ClassStatisticListPtr  GetClassStatistics ()  const;

    void  ManageTheExtraction (bool&  successful);

    void  ShutdownProcessing (ImageExtractionThreadListPtr  threads);

  private:
    void  DisplayRunTimeStats (kkuint32  loopCount);

    void  GenerateReport ();

    void  Initialize (bool&  _successful);

    void  MakeSureSipperCruiseExists ();

    void  MakeSureSipperStationExists ();

    void  MakeSureSipperDeploymentExists ();

    SipperFilePtr  MakeSureSipperSipperFileExists (const KKStr&  sipperFileName);

    void  MonitorUntilDone (ImageExtractionThreadListPtr  threadsToMonitor,
                            kkuint32&                       loopCount,
                            bool&                         successful
                           );

    void  ReportHeader (ostream&  r);

    void  StartThreads (bool&  threadsStartedSuccessfully);

    void  StartThread (ImageExtractionThreadPtr  threadInstance);
    void  StartFrameProcessor (bool&  threadStartedSuccessfully);
    void  StartFrameExtractor (bool&  threadStartedSuccessfully);

    void  ReUpdateValidationData ();


    ImageExtractionThreadListPtr        allThreads;        /**<  List of all threads created under this instance of 'ExtractionManager'.  It
                                                            * will own them.
                                                            */
    KKStr                               applicationName;
    bool                                cancelFlag;
    KKStr                               completionStatus;
    bool                                crashed;                /**< Indicates if any one of the threads crashed.         */
    DataBaseUpdateThreadPtr             dataBaseUpdaterThread;
    DataBasePtr                         dbConn;
    SipperDeploymentPtr                 deployment;
    bool                                doneExecuting;          /**< The last thing this instance will do in 'ManageTheExtraction'is set this flag to true. */
    double                              endCPUsecs;
    DateTime                            endTime;
    FactoryFVProducerPtr                fvProducerFactory;
    FileDescConstPtr                    fileDesc;
    FrameExtractorThreadPtr             frameExtractorThread;
    LogicalFrameQueuePtr                framePool;              /**< Frames that will be used by 'FrameExtractorThread' and 'FrameProcessorThread'. */
    ImageExtractionThreadListPtr        frameProcessors;   /**< List of threads that are specifically instances of 'FrameProcessorThread'.     */
    kkuint32                            frameWidth;
    SipperExtractionImageManagerPtr     imageManager;
    ExtractedImageQueuePtr              imagesAwaitingUpdate;   /**< These are images that are queued to be inserted into database.                 */
    RunLog&                             log;
    DataBaseLogEntryPtr                 logEntry;
    kkuint32                            logEntryId;
    kkuint32                            maxNumThreads;
    MsgQueuePtr                         msgQueue;               /**< Messages from all the different threads will be sent through this instance.    */
    ExtractionParms&                    parms;
    std::ofstream                       reportFile;
    SipperFilePtr                       sipperFileRec;
    KKStr                               sipperRootName;
    double                              startCPUsecs;
    DateTime                            startTime;
    bool                                terminateFlag;
    DataBaseImageValidatedEntryListPtr  validationInfo;         /**< Holds Images Validation info to be re-applied to images after extraction.       */
    KKStr                               validationInfoFileName;
    KKStr                               validationInfoFileNameHistory;
    kkuint32                            veryLargeImageSize;     /**< Threshold for putting Images into Very Large Directory.                       */
  };  /* ExtractionManager */

  typedef  ExtractionManager*  ExtractionManagerPtr;


}  /* ImageExtractionManager */


#endif

