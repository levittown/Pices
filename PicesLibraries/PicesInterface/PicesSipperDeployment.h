#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include  "DateTime.h"
using namespace KKB;

#include  "SipperDeployment.h"
using namespace SipperHardware;

#include  "PicesRunLog.h"


namespace PicesInterface 
{
  public  ref class PicesSipperDeployment
  {
  public:
    PicesSipperDeployment (String^  _cruiseName,
                           String^  _stationName
                          );

    PicesSipperDeployment (SipperDeploymentPtr  _deployment);
    
  private:
    ~PicesSipperDeployment ();

  protected:
    !PicesSipperDeployment ();

  public:
    property  String^  CruiseName       {String^  get ();  void  set (String^ _cruiseName);}
    property  String^  DeploymentNum    {String^  get ();  void  set (String^ _deploymentNum);}
    property  String^  StationName      {String^  get ();  void  set (String^ _stationName);}
    property  String^  Description      {String^  get ();  void  set (String^ _description);}
    property  String^  ShortDescription {String^  get ();}

    property  System::DateTime  DateTimeStart {System::DateTime  get ();  void  set (System::DateTime  _dateTimeStart);}
    property  System::DateTime  DateTimeEnd   {System::DateTime  get ();  void  set (System::DateTime  _dateTimeEnd);}


    property  double    Latitude     {double    get ()  {return  deployment->Latitude     ();}    void  set (double   _latitude)     {deployment->Latitude     (_latitude);}}
    property  double    Longitude    {double    get ()  {return  deployment->Longitude    ();}    void  set (double   _longitude)    {deployment->Longitude    (_longitude);}}
    property  kkuint32  CropLeft     {kkuint32  get ()  {return  deployment->CropLeft     ();}    void  set (kkuint32 _cropLeft)     {deployment->CropLeft     (_cropLeft);}}      
    property  kkuint32  CropRight    {kkuint32  get ()  {return  deployment->CropRight    ();}    void  set (kkuint32 _cropRight)    {deployment->CropRight    (_cropRight);}}      
    property  float     ChamberWidth {float     get ()  {return  deployment->ChamberWidth ();}    void  set (float    _chamberWidth) {deployment->ChamberWidth (_chamberWidth);}}

    property  System::DateTime   SyncTimeStampActual {System::DateTime   get ();   void  set (System::DateTime _syncTimeStampActual);}
    property  System::DateTime   SyncTimeStampCTD    {System::DateTime   get ();   void  set (System::DateTime _syncTimeStampCTD);}
    property  System::DateTime   SyncTimeStampGPS    {System::DateTime   get ();   void  set (System::DateTime _syncTimeStampGPS);}


    property  String^  GPSString  {String^  get ();}

    SipperDeploymentPtr  UnManagedClass () {return deployment;}

  private:
    SipperDeploymentPtr  deployment;
  };  /* PicesSipperDeployment */




  public ref class  PicesSipperDeploymentList:  public  List<PicesSipperDeployment^>
  {
  public:
    PicesSipperDeploymentList ();

    // Will take ownership of contents of 'deployments' but not the list itself.
    // the caller will still be responsible for deleting the list('deployments').
    PicesSipperDeploymentList (SipperDeploymentListPtr  deployments);

    
    array<String^>^  ExtractStationNames ();



  private:
    ~PicesSipperDeploymentList ();

  protected:
    !PicesSipperDeploymentList ();


  private:

  };


};
