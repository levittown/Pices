#if  !defined(_GPSDATAPOINT_)
#define _GPSDATAPOINT_

#include <map>

#include "BasicTypes.h"
#include "DateTime.h"
#include "KKQueue.h"
using namespace KKU;


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
    typedef  KKU::int32     int32;
    typedef  KKU::uint32    uint32;
    typedef  KKU::DateTime  DateTime;
    typedef  KKU::TimeType  TimeType;
    typedef  GPSDataPoint*  GPSDataPointPtr;

    GPSDataPoint ();

    GPSDataPoint (const GPSDataPoint&  _dataPoint);

    GPSDataPoint (DateTime _ctdDateTime,
                  TimeType _gpsStartTime,
                  int32    _sipperFileId,
                  uint32   _avgScanLine,
                  double   _avgLatitude,
                  double   _avgLongitude,
                  float    _avgFlowRate
                 );

    ~GPSDataPoint ();

    DateTime  CtdDateTime  () const {return ctdDateTime;}
    TimeType  GPSStartTime () const {return gpsStartTime;}
    int32     SipperFileId () const {return sipperFileId;}
    uint32    AvgScanLine  () const {return avgScanLine;}
    double    AvgLatitude  () const {return avgLatitude;}
    double    AvgLongitude () const {return avgLongitude;}
    float     AvgFlowRate  () const {return avgFlowRate;}

  private:
    DateTime  ctdDateTime;
    TimeType  gpsStartTime;   /**<  Time that this data point start covering from.  */
    int32     sipperFileId;
    uint32    avgScanLine;
    double    avgLatitude;
    double    avgLongitude;
    float     avgFlowRate;
  };
  typedef  GPSDataPoint::GPSDataPointPtr  GPSDataPointPtr;





  class GPSDataPointList:  public KKQueue<GPSDataPoint>  
  {
  public:
    typedef  KKU::int32   int32;
    typedef  GPSDataPointList*  GPSDataPointListPtr;

    GPSDataPointList (bool _owner);
  
    ~GPSDataPointList ();

 private:
  };
  
  typedef  GPSDataPointList::GPSDataPointListPtr  GPSDataPointListPtr;
}  /* SipperHardware */

#endif


