#ifndef  _INSTRUMENTDATAREPORT_
#define  _INSTRUMENTDATAREPORT_

#include  "KKStr.h"
using namespace KKB;

namespace MLL
{
  #ifndef _INSTRUMENTDATAMANAGER_
  class InstrumentDataManager;
  typedef  InstrumentDataManager*  InstrumentDataManagerPtr;
  #endif


  class InstrumentDataReport
  {
  public:
    typedef  InstrumentDataReport*  InstrumentDataReportPtr;
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;

    InstrumentDataReport (InstrumentDataManagerPtr  _manager,
                          const KKStr&              _reportDir,
                          bool                      _text,
                          kkint32                   _instrumentId,
                          bool                      _useReportWithLineNumbers
                         );
    virtual
      ~InstrumentDataReport ();


    const KKStr&   CurTextLine  ()  {return curTextLine;}
    const KKStr&   LastTextLine ()  {return lastTextLine;}



    /**
     *@brief A virtual Constructor for InstrumentDataReport instances.
     *@details Based on the parameters will instantiate an instance of the correct derived.
     * In the future as different types of instruments are introduced we can make specialized versions of InstrumentDataReport.
     *@param[in] _manager       InstrumentDataManager instance that will this newly created instance will report to.
     *@param[in] _reportDir     Directory where report file is to be placed.  
     *@param[in] _text          Set to true = text data,  false = binary data.
     *@param[in] _instrumentId  Sensor/Instrument ID
     */
    static  
      InstrumentDataReportPtr  CreateInstrumentDataReport (InstrumentDataManagerPtr _manager,
                                                           const KKStr&             _reportDir,
                                                           bool                     _text,
                                                           kkint32                  _instrumentId
                                                          );

    virtual  
      void  ReportInstrumentData (kkuint32 curScanLine,
                                  uchar  sensorData
                                 );

  protected:
    KKStr  InstrumentName ();
    
    static  const char*  InstrumentNames[];
  
    KKStr    curTextLine;      /**< Used where "text == true",  Every time a character
                                * is read via 'ReportInstrumentData' will append to
                                * end of string.  When a '\n' is processed will move
                                * to 'lastTextLine' and then set to empty.
                                */
    kkint32  instrumentId;
    
    kkuint32 lastScanLineReported;
  
    KKStr    lastTextLine;    /**< Used where "text == true",  will keep an instance of last complete text line 
                               * read.  Will come from curText line when 
                               * ever a '\n' character is read.
                               */
  
    InstrumentDataManagerPtr  manager;
    KKStr                     reportFileName;
    KKStr                     reportFileNameWLN;
    ofstream                  report;
    ofstream                  reportWLN;
    bool                      text;
    bool                      useReportWithLineNumbers;
  };
  
  typedef  InstrumentDataReport::InstrumentDataReportPtr  InstrumentDataReportPtr;
}  /* MLL */

#endif
