#ifndef  _INSTRUMENTDATAUSERMSGS_
#define  _INSTRUMENTDATAUSERMSGS_

#include  "KKStr.h"
using namespace  KKB;


#include  "InstrumentDataReport.h"

namespace  SipperHardware
{
  class InstrumentDataUserMsgs: public  InstrumentDataReport
  {
  public:
    typedef  InstrumentDataUserMsgs*  InstrumentDataUserMsgsPtr;


    InstrumentDataUserMsgs (InstrumentDataManagerPtr _manager,
                                const KKStr&            _reportDir,
                                bool                     _text,
                                kkint32                  _instrumentId
                               );

    virtual
      ~InstrumentDataUserMsgs ();


    virtual  
      void  ReportInstrumentData (kkuint32 curScanLine,
                                  uchar  sensorData
                                 );

  private:
    KKStr  curTextLine;
    kkint32 curTextLineStartScanLine;
    KKStr  lastTextLine;
    char    prevEolChar;
  };

  typedef  InstrumentDataUserMsgs::InstrumentDataUserMsgsPtr  InstrumentDataUserMsgsPtr;
}  /* SipperHardware */

#endif
