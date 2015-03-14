#ifndef  _INSTRUMENTDATACTDPLUS_
#define  _INSTRUMENTDATACTDPLUS_

#include  "KKStr.h"
using namespace KKB;

#include  "InstrumentDataReport.h"

namespace  SipperHardware
{
  class InstrumentDataCTDplus;
  typedef  InstrumentDataCTDplus*  InstrumentDataCTDplusPtr;


  class InstrumentDataCTDplus: public  InstrumentDataReport
  {
  public:
    InstrumentDataCTDplus (InstrumentDataManagerPtr _manager,
                           const KKStr&             _reportDir,
                           bool                     _text,
                           kkint32                  _instrumentId
                          );

    virtual
      ~InstrumentDataCTDplus ();


    virtual  
      void  ReportInstrumentData (kkuint32 curScanLine,
                                  uchar  sensorData
                                 );
  
  private:
    void  AssignCtdExternalInstruments ();
  
    kkint32 LocateAMonthNameStr (const KKStr& str);
  
    void  ParseTxtBlock (KKStr&  block);
  
    void  ProcessCTDdata (const KKStr&  txt);
  
    KKStr   ctdExt0Code;
    KKStr   ctdExt1Code;
    KKStr   ctdExt2Code;
    KKStr   ctdExt3Code;
  
    KKStr   curTextLine;
    kkint32 curTextLineStartScanLine;
    KKStr   lastTextLine;
    char    prevEolChar;
  };
  

  typedef  InstrumentDataCTDplus*  InstrumentDataCTDplusPtr;

}  /* SipperHardware */

#endif
