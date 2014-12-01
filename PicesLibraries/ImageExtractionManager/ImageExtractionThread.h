#if  !defined(_IEM_IMAGEEXTRACTIONTHREAD_)
#define  _IEM_IMAGEEXTRACTIONTHREAD_

#include "KKThread.h"

#include "MsgQueue.h"

#include "PreDefinedClasses.h"
#include "ExtractionParms.h"


namespace  ImageExtractionManager
{
  typedef  enum  {ttFrameExtractor,
                  ttFrameProcessor,
                  ttDataBAseUpdater
                 }  
                   ThreadTypes;

  /**
   *@brief   The base class to be used by all Extraction Manager threads.
   *@details  This will be the base class for each one of the different types of processing that 
   * Sipper Image processing will be done. Some of threads will Extract logical frames from Sipper
   * files,  
   */
  class  ImageExtractionThread:  public KKThread
  {
  public:
    ImageExtractionThread (ExtractionParms&     _parms,
                           ExtractionManagerPtr _extractionManager,
                           const KKStr&         _threadName,
                           MsgQueuePtr          _msgQueue
                          );

    virtual ~ImageExtractionThread ();

    virtual  ThreadTypes   ThreadType () = 0;



  protected:
    ExtractionManagerPtr   extractionManager;  /**< Pointer to instance that manages all the threads.  */

    ExtractionParms&       parms;


  private:
  };  /* ImageExtractionThread */


  typedef  ImageExtractionThread*          ImageExtractionThreadPtr;
  typedef  KKQueue<ImageExtractionThread>  ImageExtractionThreadList;
  typedef  ImageExtractionThreadList*      ImageExtractionThreadListPtr;


}  /* ImageExtractionManager*/

#endif
