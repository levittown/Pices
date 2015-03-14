#if  !defined(_SIPPERDEPLOYMENT_)
#define  _SIPPERDEPLOYMENT_

#include  "DateTime.h"
#include  "KKQueue.h"
#include  "RunLog.h"
using  namespace  KKB;

namespace  SipperHardware
{
  class SipperDeployment
  {
  public:
    typedef  SipperDeployment*  SipperDeploymentPtr;
  
    SipperDeployment ();
    ~SipperDeployment ();
  
    const   KKStr&           CruiseName          () const  {return cruiseName;}
    const   KKStr&           StationName         () const  {return stationName;}
    const   KKStr&           DeploymentNum       () const  {return deploymentNum;}
    const   KKStr&           Description         () const  {return description;}
    double                   Latitude            () const  {return latitude;}
    double                   Longitude           () const  {return longitude;}
    const   KKB::DateTime&   DateTimeStart       () const  {return dateTimeStart;}
    const   KKB::DateTime&   DateTimeEnd         () const  {return dateTimeEnd;}
    const   KKB::DateTime&   SyncTimeStampActual () const  {return syncTimeStampActual;}
    const   KKB::DateTime&   SyncTimeStampCTD    () const  {return syncTimeStampCTD;}
    const   KKB::DateTime&   SyncTimeStampGPS    () const  {return syncTimeStampGPS;}
    KKB::kkuint32            CropLeft            () const  {return cropLeft;}
    KKB::kkuint32            CropRight           () const  {return cropRight;}
    float                    ChamberWidth        () const  {return chamberWidth;}           /**< Width of sampling caber in Mili-Meters.  */
  
    void   CruiseName          (const KKStr&          _cruiseName)          {cruiseName          = _cruiseName;}
    void   StationName         (const KKStr&          _stationName)         {stationName         = _stationName;}
    void   DeploymentNum       (const KKStr&          _deploymentNum)       {deploymentNum       = _deploymentNum;}
    void   Description         (const KKStr&          _description)         {description         = _description;}
    void   Latitude            (double                _latitude)            {latitude            = _latitude;}
    void   Longitude           (double                _longitude)           {longitude           = _longitude;}
    void   DateTimeStart       (const KKB::DateTime&  _dateTimeStart)       {dateTimeStart       = _dateTimeStart;}
    void   DateTimeEnd         (const KKB::DateTime&  _dateTimeEnd)         {dateTimeEnd         = _dateTimeEnd;}
    void   SyncTimeStampActual (const KKB::DateTime&  _syncTimeStampActual) {syncTimeStampActual = _syncTimeStampActual;}
    void   SyncTimeStampCTD    (const KKB::DateTime&  _syncTimeStampCTD)    {syncTimeStampCTD    = _syncTimeStampCTD;}
    void   SyncTimeStampGPS    (const KKB::DateTime&  _syncTimeStampGPS)    {syncTimeStampGPS    = _syncTimeStampGPS;}
    void   CropLeft            (KKB::kkuint32         _cropLeft)            {cropLeft            = _cropLeft;}
    void   CropRight           (KKB::kkuint32         _cropRight)           {cropRight           = _cropRight;}
    void   ChamberWidth        (float                 _chamberWidth)        {chamberWidth        = _chamberWidth;}

  private:
    KKStr          cruiseName;
    KKStr          stationName;
    KKStr          deploymentNum;
    KKStr          description;
    KKB::DateTime  dateTimeStart;
    KKB::DateTime  dateTimeEnd;
    double         latitude;
    double         longitude;
    KKB::DateTime  syncTimeStampActual;
    KKB::DateTime  syncTimeStampCTD;
    KKB::DateTime  syncTimeStampGPS;
    KKB::kkuint32  cropLeft;
    KKB::kkuint32  cropRight;
    float          chamberWidth;   /**< Width of sampling caber in Mili-Meters.  */
  };  /* SipperDeployment */



  typedef  SipperDeployment::SipperDeploymentPtr   SipperDeploymentPtr;



  class  SipperDeploymentList:  public  KKQueue<SipperDeployment>
  {
  public:
    typedef  SipperDeploymentList*  SipperDeploymentListPtr;
  
    SipperDeploymentList (bool  _owner = true);
    ~SipperDeploymentList ();
  
  };


  typedef  SipperDeploymentList::SipperDeploymentListPtr   SipperDeploymentListPtr;


}  /* SipperHardware */

#endif
