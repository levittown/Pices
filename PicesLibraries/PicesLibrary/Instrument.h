#ifndef  _INSTRUMENT_
#define  _INSTRUMENT_

#include  "KKStr.h"
using namespace KKB;



namespace  MLL
{

#ifndef  _INSTRUMENTDATAREPORT_
  class  InstrumentDataReport;
  typedef  InstrumentDataReport*  InstrumentDataReportPtr;
#endif


#ifndef  _INSTRUMENTDATAMANAGER_
  class  InstrumentDataManager;
  typedef  InstrumentDataManager*  InstrumentDataManagerPtr;
#endif


  class Instrument
  {
  public:
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::kkint64 kkint64;
    typedef  KKB::kkuint64  kkuint64;
  
    typedef  Instrument*  InstrumentPtr;

    Instrument (const KKStr&  _shortName,
                const KKStr&  _description
               );
    
    virtual  ~Instrument ();
    
    const KKStr&  ShortName   () const  {return shortName;}
    const KKStr&  Description () const  {return description;}
  
    static  InstrumentPtr  LookUpByShortName (const KKStr&  _shortName);
  
    static  InstrumentPtr  OtherInstrument ();
  
    virtual bool  Other ()  const;
  
    virtual  InstrumentDataReportPtr  CreateInstrumentDataReporter (InstrumentDataManagerPtr _manager,
                                                                    const KKStr&             _reportDir,
                                                                    kkint32                  _instrumentId
                                                                   ) = 0;
    
    static  void  FinalCleanUp (); /**< Deletes objects pointed to by 'instruments[]';
                                    * this way the don't confuse the memory dump 
                                    * that occurs at the end of a program in Debug Mode.
                                    */
  
  private:
    //static  void  BuildInstrumentList ();
  
    static  InstrumentPtr   instruments[];
    static  bool            needToRunFinalCleanUp;
  
    KKStr  shortName;
    KKStr  description;
  };  /* Instrument */
  
  
  typedef  Instrument::InstrumentPtr  InstrumentPtr;
  
  
  
  class  InstrumentCTD: public  Instrument 
  {
  public:
    InstrumentCTD (): Instrument ("CTD", "CTD")
    {
    }
  
    virtual  ~InstrumentCTD ();
    
    virtual  InstrumentDataReportPtr  CreateInstrumentDataReporter (InstrumentDataManagerPtr _manager,
                                                                    const KKStr&            _reportDir,
                                                                    kkint32                  _instrumentId
                                                                   );
  
  };  /* InstrumentCTD */
  


  class  InstrumentGPS: public Instrument 
  {
  public:
    InstrumentGPS (): Instrument ("GPS", "GPS")
    {
    }
  
    virtual  ~InstrumentGPS ();
  
    virtual  InstrumentDataReportPtr  CreateInstrumentDataReporter (InstrumentDataManagerPtr _manager,
                                                                    const KKStr&            _reportDir,
                                                                    kkint32                  _instrumentId
                                                                   );
  };  /* InstrumentGPS */
  
  
  
  class  InstrumentInclinationMeter: public Instrument 
  {
  public:
    InstrumentInclinationMeter (): Instrument ("P-R", "Inclination Meter")
    {
    }
  
    virtual  ~InstrumentInclinationMeter ();
  
    virtual  InstrumentDataReportPtr  CreateInstrumentDataReporter (InstrumentDataManagerPtr _manager,
                                                                    const KKStr&            _reportDir,
                                                                    kkint32                  _instrumentId
                                                                   );
  };  /* InstrumentOther */
  
  
  
  
  class  InstrumentBatteryMeters: public Instrument
  {
  public:
    InstrumentBatteryMeters (): Instrument ("BAT", "Battery Meters")
    {
    }
  
    virtual  ~InstrumentBatteryMeters ();
  
    virtual  InstrumentDataReportPtr  CreateInstrumentDataReporter (InstrumentDataManagerPtr _manager,
                                                                    const KKStr&            _reportDir,
                                                                    kkint32                  _instrumentId
                                                                   );
  
  };  /* InstrumentOther */
  
  
  
  
  
  class InstrumentOther: public Instrument 
  {
  public:
    InstrumentOther ();
  
    virtual  ~InstrumentOther ();
  
    virtual bool  Other ()  {return true;}  
  
  
    virtual  InstrumentDataReportPtr  CreateInstrumentDataReporter (InstrumentDataManagerPtr _manager,
                                                                    const KKStr&            _reportDir,
                                                                    kkint32                  _instrumentId
                                                                   );
  
  };  /* InstrumentOther */

}  /* MLL */
#endif
    



