#if  !defined(_GPSDATAPOINT_)
#define _GPSDATAPOINT_

#include <map>

#include "KKBaseTypes.h"
#include "DateTime.h"
#include "KKQueue.h"
using namespace KKB;


namespace  SipperHardware
{
  /**
   *@brief Represents a single GPS datapoint entry (GMT DateTime, Latitude and Longitude) from a imported GPS data source
   * provided by the research Vessels that SIPPER is deployed on.
   *@details
   * Research vessels will record GPS data into text files throughout the length of their cruises. Temporal granularity
   * will range from a couple seconds to a minute. This data is imported into the PICES database via the ImportGPSDataApp
   * application. The data is kept in the GpsData table of the PICES database.
   */
  class GPSDataPoint
  {
  public:
    typedef  KKB::kkint32   kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::DateTime  DateTime;
    typedef  KKB::TimeType  TimeType;
    typedef  GPSDataPoint*  GPSDataPointPtr;

    GPSDataPoint ();

    GPSDataPoint (const GPSDataPoint&  _dataPoint);

    GPSDataPoint (DateTime _ctdDateTime,
                  DateTime _gpsStartTime,
                  kkint32  _sipperFileId,
                  kkuint32 _avgScanLine,
                  double   _avgLatitude,
                  double   _avgLongitude,
                  float    _avgFlowRate
                 );

    ~GPSDataPoint ();

    DateTime  CtdDateTime         () const {return ctdDateTime;}
    DateTime  GPSStartTime        () const {return gpsStartTime;}
    kkint32   SipperFileId        () const {return sipperFileId;}
    kkuint32  AvgScanLine         () const {return avgScanLine;}
    double    AvgLatitude         () const {return avgLatitude;}
    double    AvgLongitude        () const {return avgLongitude;}
    float     AvgFlowRate         () const {return avgFlowRate;}

  private:
    DateTime  ctdDateTime;
    DateTime  gpsStartTime;   /**<  Time that this data point start covering from. */
    kkint32   sipperFileId;
    kkuint32  avgScanLine;
    double    avgLatitude;
    double    avgLongitude;
    float     avgFlowRate;
  };
  typedef  GPSDataPoint::GPSDataPointPtr  GPSDataPointPtr;





  class GPSDataPointList:  public KKQueue<GPSDataPoint>  
  {
  public:
    typedef  KKB::kkint32 kkint32;
    typedef  GPSDataPointList*  GPSDataPointListPtr;

    GPSDataPointList (bool _owner);
  
    ~GPSDataPointList ();

 private:
  };
  
  typedef  GPSDataPointList::GPSDataPointListPtr  GPSDataPointListPtr;
}  /* SipperHardware */

#endif


