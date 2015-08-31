#ifndef  _INSTRUMENTDATAGPS_
#define  _INSTRUMENTDATAGPS_

#include  "KKStr.h"
using namespace  KKB;


#include  "InstrumentDataReport.h"


namespace  MLL
{
  class InstrumentDataGPS;
  typedef  InstrumentDataGPS*  InstrumentDataSerialPort0Ptr;


  class InstrumentDataGPS: public  InstrumentDataReport
  {
  public:
    InstrumentDataGPS (InstrumentDataManagerPtr _manager,
                       const KKStr&             _reportDir,
                       bool                     _text,
                       kkint32                  _instrumentId
                      );

    virtual
      ~InstrumentDataGPS ();
  


    virtual  
      void  ReportInstrumentData (kkuint32 curScanLine,
                                  uchar  sensorData
                                 );

  private:
    void  ParseDegreeMinutesStr (const KKStr&  str,
                                 double&        degrees,
                                 double&        minutes
                                )  const;

    void  ProcessGPGLL    (const KKStr& _str);
    void  ProcessNMEAInfo (const KKStr& _str);

    KKStr  curTextLine;
    kkint32  curTextLineStartScanLine;
    KKStr  lastTextLine;
  };
  

  typedef  InstrumentDataGPS*  InstrumentDataSerialPort0Ptr;
}  /* MLL */

#endif
