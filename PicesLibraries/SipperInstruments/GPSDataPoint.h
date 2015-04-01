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
   *@brief Represents a single GPS data-point entry (GMT DateTime, Latitude and Longitude) from a imported GPS data source
   * provided by the research Vessels that SIPPER is deployed on.
   *@details
   * Research vessels will record GPS data into text files throughout the length of their cruises. Temporal granularity
   * will range from a couple seconds to a minute. This data is imported into the PICES database via the ImportGPSDataApp
   * application. The data is kept in the GpsData table of the PICES database.
   */
  class  GPSDataPoint
  {
  public:
    typedef  KKB::kkint32   kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::DateTime  DateTime;
    typedef  KKB::TimeType  TimeType;
    typedef  GPSDataPoint*  GPSDataPointPtr;

    GPSDataPoint ();

    GPSDataPoint (const GPSDataPoint&  _dataPoint);

    GPSDataPoint (DateTime _gpsUtcTime,
                  double   _latitude,
                  double   _longitude,
                  float    _courseOverGround,
                  float    _speedOverGround
                 );

    ~GPSDataPoint ();

    DateTime  GpsUtcTime       () const {return gpsUtcTime;}
    double    Latitude         () const {return latitude;}
    double    Longitude        () const {return longitude;}
    float     CourseOverGround () const {return courseOverGround;}
    float     SpeedOverGround  () const {return speedOverGround;}

  private:
    DateTime  gpsUtcTime;   /**<  Time that this data point start covering from. */
    double    latitude;
    double    longitude;
    float     courseOverGround;
    float     speedOverGround;
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


