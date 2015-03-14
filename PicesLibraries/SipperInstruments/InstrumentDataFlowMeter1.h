#ifndef  _INSTRUMENTDATAFLOWMETER1_
#define  _INSTRUMENTDATAFLOWMETER1_

#include  "KKStr.h"
using namespace KKB;

#include  "InstrumentDataReport.h"

namespace SipperHardware
{
  class InstrumentDataFlowMeter1;
  typedef  InstrumentDataFlowMeter1*  InstrumentDataFlowMeter1Ptr;

  #define  NumTurnSignalsToTrack 4
  #define  ScanLinesPerSecond    25000


  class InstrumentDataFlowMeter1: public  InstrumentDataReport
  {
  public:
    InstrumentDataFlowMeter1 (InstrumentDataManagerPtr _manager,
                              const KKStr&            _reportDir,
                              bool                     _text,
                              kkint32                  _instrumentId
                             );
  
    virtual
      ~InstrumentDataFlowMeter1 ();
  
    virtual  
      void  ReportInstrumentData (kkuint32 curScanLine,
                                  uchar  sensorData
                                 );
  
  private:
    kkint32  flowMeterCurSlot;        // Indicates the slot last used.
  
    kkint32  flowMeterTrailingSlot;   // Indicates the slot that is behind current slot.
  
    kkuint32*  flowMeterTurnScanLines;  // A circular buffer that will keep track of scan lines.
  
   };
  
  
  typedef  InstrumentDataFlowMeter1*  InstrumentDataFlowMeter1Ptr;
}  /* SipperHardware */

#endif
