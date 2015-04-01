#include "StdAfx.h"
#include  "FirstIncludes.h"

#include <stdio.h>
#include <math.h>


#include  <ctype.h>
#include  <time.h>
#include  <fstream>
#include  <iostream>
#include  <istream>
#include  <map>
#include  <ostream>
#include  <string>
#include  <vector>
using namespace std;


using namespace System;
using namespace System::Collections;
using namespace System::Threading;
using namespace System::Windows::Forms;

#include "MemoryDebug.h"
#include "KKBaseTypes.h"
#include "Point.h"
using namespace KKB;


#include "PicesKKStr.h"
#include "PicesGPSDataPoint.h"
#include "PicesMethods.h"


using namespace  PicesInterface;

namespace  PicesInterface
{
  PicesGPSDataPoint::PicesGPSDataPoint (PicesGPSDataPoint^  stat):
     gpsUtcTime        (stat->gpsUtcTime),
     latitude          (stat->latitude),
     longitude         (stat->longitude),
     courseOverGround  (stat->courseOverGround),
     speedOverGround   (stat->speedOverGround)
  {
  }


  PicesGPSDataPoint::PicesGPSDataPoint (const GPSDataPoint&  stat):
     gpsUtcTime        (PicesMethods::DateTimeKKUtoSystem (stat.GpsUtcTime ())),
     latitude          (stat.Latitude         ()),
     longitude         (stat.Longitude        ()),
     courseOverGround  (stat.CourseOverGround ()),
     speedOverGround   (stat.SpeedOverGround  ())
  {
  }


  PicesGPSDataPoint::!PicesGPSDataPoint ()
  {
  }


  PicesGPSDataPoint::~PicesGPSDataPoint ()
  {
    this->!PicesGPSDataPoint ();
  }



  PicesGPSDataPointList::PicesGPSDataPointList ()
  {
  }



  PicesGPSDataPointList::PicesGPSDataPointList (PicesGPSDataPointList^  stats)
  {
    for each (PicesGPSDataPoint^ stat in stats)
    {
      Add (stat);
    }  
  }



  PicesGPSDataPointList::PicesGPSDataPointList (const GPSDataPointList&  stats)
  {
    GPSDataPointList::const_iterator  idx;
    for  (idx = stats.begin ();  idx != stats.end ();  ++idx)
      Add (gcnew PicesGPSDataPoint (*(*idx)));
  }


  PicesGPSDataPointList::~PicesGPSDataPointList ()
  {
    this->!PicesGPSDataPointList ();
  }



  PicesGPSDataPointList::!PicesGPSDataPointList ()
  {
    CleanUpMemory ();
  }



  void  PicesGPSDataPointList::AddList (PicesGPSDataPointList^ list)
  {
    for each (PicesGPSDataPoint^ stat in list)
    {
      Add (stat);
    }
  }



  void  PicesGPSDataPointList::AddList (const GPSDataPointList&  stats)
  {
    GPSDataPointList::const_iterator  idx;
    for  (idx = stats.begin ();  idx != stats.end ();  ++idx)
      Add (gcnew PicesGPSDataPoint (*(*idx)));
  }



  void  PicesGPSDataPointList::CleanUpMemory ()
  {
  }
}  /* PicesInterface */
