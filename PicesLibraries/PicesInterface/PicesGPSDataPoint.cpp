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




  PicesGPSDataPointList^  PicesGPSDataPointList::FilterOutNoise ()
  {
    PicesGPSDataPointList^  fliteredGPSData = gcnew PicesGPSDataPointList ();
    double  lastLat = 0.0;
    double  lastLong = 0.0;

    for each (PicesGPSDataPoint^ dp in this)
    {
      if  (Math::Abs (dp->Latitude < 0.1) || (Math::Abs (dp->Longitude) < 0.1))
        continue;
      if  ((dp->Latitude == lastLat)  &&  (dp->Longitude == lastLong))
        continue;
      fliteredGPSData->Add (dp);
      lastLong = dp->Longitude;
      lastLat  = dp->Latitude;
    }

    int  count = fliteredGPSData->Count;
    if  (count < 1)
      return  nullptr;

    if  (count < 5)
      return  fliteredGPSData;

    double*  distBP = new double[count];
    double   totalDist = 0.0;
    double   totalDistSquare =  0.0;

    int  n = count - 1;
    PicesGPSDataPoint^ curDP  = fliteredGPSData[0];
    PicesGPSDataPoint^ nextDP = fliteredGPSData[1];
    for  (int x = 0;  x < n;  ++x)
    {
      nextDP = fliteredGPSData[x + 1];
      double  deltaLon = nextDP->Longitude - curDP->Longitude;
      double  deltaLat = nextDP->Latitude  - curDP->Latitude;
      double  distSquare = deltaLon * deltaLon + deltaLat * deltaLat;
      double  dist = Math::Sqrt (distSquare);
      totalDist += dist;
      totalDistSquare += distSquare;
      distBP[x] = dist;
      curDP = nextDP;
    }

    double  meanDist = totalDist / (double)n;
    double  variance = (totalDistSquare - (totalDist * totalDist) / (double)n)  / (double)n;
    double  stdDev = Math::Sqrt (variance);

    double  distTH = meanDist + stdDev * 3.5;
      
    List<PicesGPSDataPointList^>^  clusters = gcnew List<PicesGPSDataPointList^> ();
    PicesGPSDataPointList^  curCluster = gcnew PicesGPSDataPointList ();
    clusters->Add (curCluster);

    int  curIdx = 0;
    while  (curIdx < (count - 1))
    {
      PicesGPSDataPoint^  curDP = fliteredGPSData[curIdx];
      curCluster->Add (curDP);
      if  (distBP[curIdx] > distTH)
      {
          curCluster = gcnew PicesGPSDataPointList ();
          clusters->Add (curCluster);
      }
      ++curIdx;
    }

    curCluster->Add (fliteredGPSData[curIdx]);

    delete  distBP;
    distBP = NULL;

    // At this point we broken up all the GPS points into clusters such that points with in a cluster are 
    // within a threshold to their neighbors.  Now we need to decide if any of the clusters are just noise
    // and should be excluded from the filtered list.

    PicesGPSDataPointList^  finalFilteredList = gcnew PicesGPSDataPointList ();

    for each  (PicesGPSDataPointList^  cluster in clusters)
    {
      if  (cluster->Count < 5)
        continue;

      for each  (PicesGPSDataPoint^  dp in  cluster)
        finalFilteredList->Add (dp);
    }

    return  finalFilteredList;
  }  /* FilterOutNoise */




  void  PicesGPSDataPointList::CleanUpMemory ()
  {
  }
}  /* PicesInterface */
