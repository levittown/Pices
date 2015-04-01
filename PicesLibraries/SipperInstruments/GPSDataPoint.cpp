#include "FirstIncludes.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include  "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
using namespace KKB;


#include "GPSDataPoint.h"
using  namespace  SipperHardware;


GPSDataPoint::GPSDataPoint ():
    gpsUtcTime        (),
    latitude          (0.0),
    longitude         (0.0),
    courseOverGround  (0.0f),
    speedOverGround   (0.0f)
{
}


GPSDataPoint::GPSDataPoint (const GPSDataPoint&  _dataPoint):
    gpsUtcTime        (_dataPoint.gpsUtcTime),
    latitude          (_dataPoint.latitude),
    longitude         (_dataPoint.longitude),
    courseOverGround  (_dataPoint.courseOverGround),
    speedOverGround   (_dataPoint.speedOverGround)
{
}



GPSDataPoint::GPSDataPoint (DateTime _gpsUtcTime,
                            double   _latitude,
                            double   _longitude,
                            float    _courseOverGround,
                            float    _speedOverGround
                           ):
    gpsUtcTime       (_gpsUtcTime),
    latitude         (_latitude),
    longitude        (_longitude),
    courseOverGround (_courseOverGround),
    speedOverGround  (_speedOverGround)
{
}



GPSDataPoint::~GPSDataPoint ()
{
}




GPSDataPointList::GPSDataPointList (bool _owner):
    KKQueue<GPSDataPoint> (_owner)
{
}

  
GPSDataPointList::~GPSDataPointList ()
{
}
