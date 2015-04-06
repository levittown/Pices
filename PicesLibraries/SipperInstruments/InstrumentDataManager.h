#ifndef  _INSTRUMENTDATAMANAGER_
#define  _INSTRUMENTDATAMANAGER_

#include "KKStr.h"
#include "RunLog.h"
using  namespace  KKB;

#include "CTD_Plus_Data.h"

#define  MaxNumOfInstruments  64


namespace SipperHardware
{
  #ifndef  _INSTRUMENTDATAREPORT_
  class  InstrumentDataReport;
  typedef  InstrumentDataReport*  InstrumentDataReportPtr;
  #endif


  #ifndef  _SIPPERHEADERREC_
  class  SipperHeaderRec;
  typedef  SipperHeaderRec*  SipperHeaderRecPtr;
  #endif


  #ifndef  _SIPPERFILE_
  class  SipperFile;
  typedef  SipperFile*  SipperFilePtr;
  #endif


  #ifndef  _INSTRUMENTDATA_
  class  InstrumentData;
  typedef  InstrumentData*  InstrumentDataPtr;
  #endif


  /**
   *@class  InstrumentDataManager  Manages instrumentation data supplied by SipperBuff derived classes
   * forwarding to appropriate 'InstrumentDataReport' derived objects.
   *@details  Responsible for managing instrument data that is collected by SipperBuff or one of its decedents.
   * Originally developed when SIPPER3 format came out. This format includes instrument data. 
   * Will maintain a list of 'InstrumentDataReport' instances.  One for each Instrument/Sensor and Data or Text.
   * SipperBuff will call the 'ReportInstrumentData' method which will decide the correct 'InstrumentdataReport' instance to send
   * it to, and if necessary, create a new instance of one.
   */
  class InstrumentDataManager
  {
  public:
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;

    /**
     *@brief Creates new instance,
     *@param[in]  _sipperFileName Name of Sipper file that is being processed.
     *@param[in]  _sipperFile An instance of a SipperFile object that contains SIPPER configuration data for specific SIPPER file.
     *@param[in]  _reportDir Directory where to place Instrument/Sensor data/text files.
     *@param[in]  _log       A log-file stream. All important events will be output to this stream
     */
    InstrumentDataManager (const KKStr&   _sipperFileName,
                           SipperFilePtr  _sipperFile,
                           const KKStr&   _reportDir,
                           RunLog&        _log
                          );
  
  
    /**
     *@brief Use this constructor if you only need to parse out the data.
     *@param[in]  _sipperFileName Name of Sipper file that is being processed.
     *@param[in]  _sipperFile An instance of a SipperFile object that contains SIPPER configuration data for specific SIPPER file.
     *@param[in]  _log       A log-file stream. All important events will be output to this stream
    */
    InstrumentDataManager (const KKStr&   _sipperFileName,
                           SipperFilePtr  _sipperFile,
                           RunLog&        _log
                          );
  

    ~InstrumentDataManager ();
  
  
    void  AssignSerialPorts (const SipperHeaderRecPtr  sipperHeaderRec);
  
    SipperFilePtr   SipperFile ()  {return  sipperFile;}
  
  
    /**
     *@brief When ever SipperBuff gets instrument data,  reports thru this method.
     *@param[in]  curScanLine   Scan line that was last read when encountering this "InstrumentData".
     *@param[in]  InstrumentID  Sensor/Instrument ID
     *@param[in]  text          Set to true = text data,  false = binary data.
     *@param[in]  data          One byte of data from the SIPPER file.
    */
    void  ReportInstrumentData (kkuint32 curScanLine,
                                uchar  instrumentId,
                                bool   text,
                                uchar  data
                               );
  
    void   BatteryData (kkint32             _scanLine,
                        kkint32             _activeBattery, 
                        const VectorFloat&  _batteryLevels
                       );
  
    void   CompReportLine (kkint32  scanLine);
  
    InstrumentDataPtr  CreateInstrumentData ()  const;
  
    void   CTDdataPlus    (kkint32               _scanLine,
                           const CTD_Plus_Data&  _ctdData
                          );
  
    void   FlowRates      (kkint32 _scanLine,
                           float _meter1FlowRate,
                           float _meter2FlowRate
                          );
  
    void   Location       (kkint32          _scanLine,
                           double           _latitude,
                           double           _longitude,
                           const TimeType&  _gpsTime
                          );
  
    void   Meter1FlowRate (kkint32  _scanLine, 
                           float  _meter1FlowRate
                          );
  
    void   Meter2FlowRate (kkint32  _scanLine, 
                           float  _meter2FlowRate
                          );
  
    void   PitchAndRollData (kkint32  _scanLine,
                             float  _pitch,
                             float  _roll
                            );

    VectorUlong*  ScanLinesPerMeterDepth ()  const;
  
    bool       CtdDataRead    ()  const  {return ctdDataRead;}
  
    const
    CTD_Plus_Data&  CtdData   ()  const  {return ctdData;}
  
    float      Depth          ()  const  {return ctdData.Depth ();}
    const 
    TimeType&  GpsTime        ()  const  {return gpsTime;}
    double     Latitude       ()  const  {return latitude;} 
    double     Longitude      ()  const  {return longitude;} 
    float      Meter1FlowRate ()  const  {return meter1FlowRate;}
    float      Meter2FlowRate ()  const  {return meter2FlowRate;}
    float      RecordRate     ()  const  {return recordRate;}
    float      Salinity       ()  const  {return ctdData.Salinity ();}
    float      Oxygen         ()  const  {return ctdData.Oxygen ();}
    float      Fluorescence   ()  const  {return ctdData.Fluorescence ();}
  
    SipperHeaderRecPtr  HeaderRec ()   const  {return  headerRec;}
  
    void    Latitude    (double _latitude)        {latitude    = _latitude;}
    void    Longitude   (double _longitude)       {longitude   = _longitude;}
    void    RecordRate  (float  _recordRate)      {recordRate  = _recordRate;}
  
    const KKStr&  SipperFileName ()  const {return sipperFileName;}
  
  private:
    void  InitializeLatitude ();

    kkint32                   activeBattery;
    VectorFloat               batteryLevels;
    ofstream*                 compReport;
    kkuint64                  compReportLastCTDSecs;
    kkint32                   compReportLastScanLine;
    CTD_Plus_Data             ctdData;
    bool                      ctdDataRead;            // true indicates that a complete CTD record has been processed.
    InstrumentDataReportPtr*  dataReports;
    TimeType                  gpsTime;
    SipperHeaderRecPtr        headerRec;
    double                    latitude;
    RunLog&                   log;
    double                    longitude;
    float                     meter1FlowRate;
    float                     meter2FlowRate;
    float                     pitch;
    float                     recordRate;
    KKStr                     reportDir;
    float                     roll;
    SipperFilePtr             sipperFile;
    KKStr                     sipperFileName;
    InstrumentDataReportPtr*  textReports;
  
    // Fields to support tracking scan lines per meter depth.
    kkint32                   depthLastReported;
    kkint32                   depthMax;
    kkuint32                  lastScanLine;
    kkuint32*                   scanLinesPerMeterDepth;
  };
  
  typedef  InstrumentDataManager*  InstrumentDataManagerPtr;
}  /* SipperHardware */
  
#endif
