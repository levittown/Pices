#if  !defined(_SIPPERTHREAD_)
#define  _SIPPERTHREAD_

#include "MsgQueue.h"


#if  !defined(_EXTRACTIONMANAGER_)
namespace  ImageExtractionManager
{ 
  class ExtractionManager;
  typedef  ExtractionManager*   ExtractionManagerPtr;
}
#endif


namespace  ImageExtractionManager
{
  /**
   *@brief   The Image extraction procedure will contain several threads perform various functions.
   *@details  This will be the base class for each one of the different types of processing that 
   * Sipper Image processing will be done. Some of threads will Extract logical frames from Sipper
   * files,  
   */

  typedef  enum  {tsNotStarted,
                  tsStarting,
                  tsRunning,
                  tsStopping,
                  tsStoped
                 } ThreadStatus;

  class  SipperThread
  {
  public:
    SipperThread (ExtractionManagerPtr  _extractionManager,
                  const KKStr&          _threadName,
                  MsgQueuePtr           _msgQueue,
                  const bool&           _cancelFlag,
                  const bool&           _terminateFlag
                 );

    virtual ~SipperThread ();

    volatile  const bool&   CancelFlag    ()  const  {return terminateFlag;}
    ThreadStatus            Status        ()  const  {return status;}
    volatile  const bool&   TerminateFlag ()  const  {return terminateFlag;}


    virtual  void  Run () = 0;


  private:
    void  AddMsg (KKStrPtr  msg);      /**<  Taking ownership of 'msg' and will append to 'msgQueue'.           */
    void  AddMsg (const KKStr&  msg);  /**<  A copy of the message 'msg' will be added to the end of msgQueue.  */

    volatile 
    const bool&            cancelFlag;         /**< Threads need to monitor this flag; if it goes 'true' they need
                                                * to terminate as quick as possible.
                                                */
    ExtractionManagerPtr   extractionManager;  /**< Pointer to instance that manages all the threads.  */

    MsgQueuePtr            msgQueue;           /**< This MsgQueue instance will be owned by 'ExtractionManager' we will just use
                                                * it to communicate messages to the controlling process.
                                                */
    ThreadStatus           status;

    volatile
    const bool&            terminateFlag;      /**< Threads need to monitor this flag; if it goes there they are to
                                                * complete all processing that is queued up and then terminate.
                                                */
    KKStr                  threadName;
  };  /* SipperThread */

}  /* ImageExtractionManager*/

#endif
