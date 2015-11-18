#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "MsgQueue.h"
using namespace  KKB;


namespace PicesInterface 
{

  public  ref class PicesMsgQueue
  {
  public:
    PicesMsgQueue (String^  name);

    !PicesMsgQueue ();
    ~PicesMsgQueue ();

    void          AddMsg (String^  msg);

    void          AddMsgs (List<String^>^  msgs);

    void          Close ();

    ///<summary>
    /// Returns a copy of the string last added to the message queue; will not effect the current copy 
    /// of the message queue. The returned string will be owned by the caller who will be responsible 
    /// for deleting it.
    ///</summary>
    String^       GetCopyOfLastMsg ();

    ///<summary> Removes from the queue the oldest message added to the queue that has not been removed. </summary>
    String^       GetNextMsg ();

    MsgQueuePtr   MsgQueue ()  {return msgQueue;}


  private:
      MsgQueuePtr  msgQueue;
  };

}  /* namespace  PicesInterface */

