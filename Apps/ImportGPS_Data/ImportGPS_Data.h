#if  !defined(_IMPORTGPS_DATA_)
#define  _IMPORTGPS_DATA_

#include "PicesApplication.h"
#include "DataBase.h"
#include "RunLog.h"
#include "Str.h"


namespace  ImportGPS_DataNameSpace
{
  class  SipperFileEntryList;
  typedef  SipperFileEntryList*  SipperFileEntryListPtr;

  class  ImportGPS_Data:  public PicesApplication
  {
  public:
    typedef  enum  {GDF_NULL, GDF_WeatherBird, GDF_GPGGA, GDF_SpecialtyDiverI}  GpsDataFormat;

    ImportGPS_Data ();

    ~ImportGPS_Data ();

    virtual 
    const char*  ApplicationName () const  {return  "ImportGPS_Data";}


    virtual
    void  InitalizeApplication (int32   argc,
                                char**  argv
                               );
    
    void  Main ();


  private:
    GpsDataFormat  GpsDataFormatFromStr (const KKStr& str);
    KKStr          GpsDataFormatToStr (GpsDataFormat  format);

    void   DisplayCommandLineParameters ();

    void   ImportGPSDataWeatherBird (const KKStr&  fileName);

    void   ImportGPSDataGPGGA (const KKStr&  fileName);

    void   ImportGPSDataSpecialtyDiverI (const KKStr&  fileName);

    bool   ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                    const KKStr&  parmValue
                                   );

    void  UpdateInstrumentData (const DateTime&  gpsTimeStamp,
                                double           lattitude,
                                double           longitude
                               );


    SipperCruisePtr          cruise;
    KKStr                    cruiseStr;
    double                   deltaHours;
    SipperDeploymentListPtr  deployments;
    GpsDataFormat            fileFormat;
    SipperFileListPtr        sipperFiles;
    SipperFileEntryListPtr   sipperFileEntries;
    KKStr                    srcDirectory;
  };  /* ImportGPS_Data */
}  /* ImportGPS_DataNameSpace */

#endif
