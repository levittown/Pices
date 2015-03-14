#ifndef  _INSTRUMENTDATAPITCHANDROLL_
#define  _INSTRUMENTDATAPITCHANDROLL_

#include  "KKStr.h"
using namespace KKB;

#include  "InstrumentDataReport.h"


namespace SipperHardware
{
  class InstrumentDataPitchAndRoll: public  InstrumentDataReport
  {
  public:
    typedef  InstrumentDataPitchAndRoll*  InstrumentDataPitchAndRollPtr;

  
    InstrumentDataPitchAndRoll (InstrumentDataManagerPtr _manager,
                                const KKStr&            _reportDir,
                                bool                     _text,
                                kkint32                  _instrumentId
                               );
  
    virtual
      ~InstrumentDataPitchAndRoll ();
  
  
  
    virtual  
      void  ReportInstrumentData (kkuint32 curScanLine,
                                  uchar  sensorData
                                 );
  
  private:
    virtual void  ProcessData (const KKStr&  txt);
  
  
    KKStr  curTextLine;
    kkint32 curTextLineStartScanLine;
    KKStr  lastTextLine;
    char    prevEolChar;
  };
  
  
  typedef  InstrumentDataPitchAndRoll::InstrumentDataPitchAndRollPtr  InstrumentDataPitchAndRollPtr;
}  /* namespace SipperHardware */

#endif
