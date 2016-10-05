#if  !defined(_IEM_DATABASEUPDATETHREAD_)
#define  _IEM_DATABASEUPDATETHREAD_

#include "KKBaseTypes.h"
#include "RunLog.h"
using namespace KKB;


#include "DuplicateImages.h"
using namespace  KKMLL;


#include "DataBase.h"
using namespace  MLL;


#include "PreDefinedClasses.h"
#include "ExtractionParms.h"
#include "ImageExtractionThread.h"
using namespace  ImageExtractionManager;


namespace  ImageExtractionManager
{
  class  DataBaseUpdateThread:  public  ImageExtractionThread
  {
  public:
    DataBaseUpdateThread (ExtractionParms&                 _parms,
                          ExtractionManagerPtr             _extractionManager,
                          const KKStr&                     _threadName,
                          MsgQueuePtr                      _msgQueue,
                          ExtractedImageQueuePtr           _imagesAwaitingUpdate,
                          SipperExtractionImageManagerPtr  _imageManager,
                          FileDescConstPtr                 _fileDesc
                         );

    virtual ~DataBaseUpdateThread ();

    virtual  ThreadTypes   ThreadType ()  {return  ThreadTypes::DataBaseUpdater;}

    void  GetRunTimeStats (kkuint32&  _imagesUpdated,
                           kkuint32&  _duplicatesDetected,
                           kkuint32&  _updateFailures
                          );
    
    virtual  void  Run ();

    kkuint32  DbUpdateFailures ()  const  {return updateFailures;}

  private:
    ExtractedImagePtr  GetNextImageToUpdate ();

    void  ReFreshDataBaseImage (const KKStr&      imageFileName, 
                                const KKStr&      sipperFileName, 
                                RasterSipperPtr   raster, 
                                ImageFeaturesPtr  featureVector,
                                kkuint64          byteOffset,
                                kkuint32          sipperTopRow,
                                kkuint32          sipperTopCol
                               );

    DataBasePtr                      dbConn;
    DuplicateImagesPtr               dupImageDetector;
    KKStr                            duplicateImageDir;
    ImageFeaturesListPtr             duplicateImages;        /**< We keep track of these images, otherwise they will not get deleted 
                                                              * and hence cause memory leaks.
                                                              */
    kkuint32                         duplicatesDetected;
    FileDescConstPtr                 fileDesc;
    SipperExtractionImageManagerPtr  imageManager;
    ExtractedImageQueuePtr           imagesAwaitingUpdate;
    kkuint32                         imagesUpdated;
    RunLog                           log;
    KKStr                            sipperRootName;
    kkuint32                         updateFailures;         /**< The number of updates to the database that failed. */
  };  /* DataBaseUpdateThread */

  typedef  DataBaseUpdateThread*  DataBaseUpdateThreadPtr;

}  /* ImageExtractionManager */

#endif


