#ifndef  _INSTRUMENTDATABATTERYMETER_
#define  _INSTRUMENTDATABATTERYMETER_

#include  "KKStr.h"
using namespace  KKB;


#include  "InstrumentDataReport.h"

namespace SipperHardware
{
class InstrumentDataBatteryMeter: public InstrumentDataReport
{
public:
  typedef  InstrumentDataBatteryMeter*  InstrumentDataBatteryMeterPtr;

  InstrumentDataBatteryMeter (InstrumentDataManagerPtr _manager,
                              const KKStr&            _reportDir,
                              bool                     _text,
                              kkint32                  _instrumentId
                             );
  virtual
    ~InstrumentDataBatteryMeter ();


  virtual  
    void  ReportInstrumentData (kkuint32 curScanLine,
                                uchar  sensorData
                               );

protected:
  void  ProcessBatteryData (const KKStr&  txt);

  VectorFloat  batteryLevels;
  KKStr        curTextLine;
  kkint32      curTextLineStartScanLine;
  KKStr        lastTextLine;
  kkuint32     numOfBatteries;
  char         prevEolChar;
};


typedef  InstrumentDataBatteryMeter::InstrumentDataBatteryMeterPtr  InstrumentDataBatteryMeterPtr;
}  /* namespace SipperHardware */

#endif
