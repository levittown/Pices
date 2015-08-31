#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "GPSDataPoint.h"
using namespace  MLL;

#include "PicesKKStr.h"
#include "PicesRunLog.h"


namespace  PicesInterface 
{
  public  ref class PicesGPSDataPoint
  {
  public:
    typedef  System::DateTime  DateTime;

    PicesGPSDataPoint (PicesGPSDataPoint^  stat);

    PicesGPSDataPoint (const GPSDataPoint&  stat);

    !PicesGPSDataPoint ();
    ~PicesGPSDataPoint ();

    property  DateTime  GpsUtcTime        {DateTime  get ()  {return  gpsUtcTime;}}
    property  double    Latitude          {double    get ()  {return  latitude;}}
    property  double    Longitude         {double    get ()  {return  longitude;}}
    property  float     CourseOverGround  {float     get ()  {return  courseOverGround;}}
    property  float     SpeedOverGround   {float     get ()  {return  speedOverGround;}}
       
  private:
    DateTime  gpsUtcTime;
    double    latitude;
    double    longitude;
    float     courseOverGround;
    float     speedOverGround;
  };  /* PicesGPSDataPoint */



  public  ref class  PicesGPSDataPointList:  List<PicesGPSDataPoint^>
  {
  public:
    PicesGPSDataPointList ();

    PicesGPSDataPointList (PicesGPSDataPointList^  stats);

    /**
     *@brief  Construct a list of managed PicesGPSDataPoint instances from the contents of the unmanaged list 'unmanagedImages'.
     *@details  Will NOT take ownership of the instances of 'GPSDataPoint' in 'stats but will create managed copies.
     *@param[in]  stats  List of unmanaged 'GPSDataPoint' objects; new Managed instances of 'GPSDataPoint' will be created and added to list.
     */
    PicesGPSDataPointList (const GPSDataPointList&  stats);

    ~PicesGPSDataPointList ();

    !PicesGPSDataPointList ();

    /**
     *@brief Duplicates the consists of 'list' and adds to this instance.
     */
    void  AddList (PicesGPSDataPointList^ list);

    void  AddList (const GPSDataPointList&  stats);

    void  CleanUpMemory ();

    ///<summary>
    ///Removes outliers based off GPS location.
    ///</summary>
    PicesGPSDataPointList^  FilterOutNoise ();



  private:
  };  /* PicesGPSDataPointList */



}  /* PicesInterface */

