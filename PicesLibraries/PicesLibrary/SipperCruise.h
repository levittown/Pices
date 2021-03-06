#if  !defined(_SIPPERCRUISE_)
#define  _SIPPERCRUISE_

#include "DateTime.h"
#include "RunLog.h"
using namespace KKB;

#include "Instrument.h"
#include "InstrumentDataFileManager.h"


namespace  MLL
{
  class SipperCruise
  {
  public:
    typedef  SipperCruise*  SipperCruisePtr;

    SipperCruise ();
    ~SipperCruise ();

    const  KKStr&      CruiseName   () const  {return  cruiseName;}
    const  KKStr&      ShipName     () const  {return  shipName;}
    const  KKStr&      Description  () const  {return  description;}
    const  DateType&   DateStart    () const  {return  dateStart;}
    const  DateType&   DateEnd      () const  {return  dateEnd;}
    const  KKStr&      Location     () const  {return  location;}
    const  KKStr&      Objective    () const  {return  objective;}
    const  KKStr&      Principal    () const  {return  principal;}
    const  KKStr&      ResearchOrg  () const  {return  researchOrg;}

    void  CruiseName   (const KKStr&          _cruiseName)    {cruiseName  = _cruiseName;}
    void  ShipName     (const KKStr&          _shipName)      {shipName    = _shipName;}
    void  Description  (const KKStr&          _description)   {description = _description;}
    void  DateStart    (const KKB::DateType&  _dateStart)     {dateStart   = _dateStart;}
    void  DateEnd      (const KKB::DateType&  _dateEnd)       {dateEnd     = _dateEnd;}
    void  Location     (const KKStr&          _location)      {location    = _location;}
    void  Objective    (const KKStr&          _objective)     {objective   = _objective;}
    void  Principal    (const KKStr&          _principal)     {principal   = _principal;}
    void  ResearchOrg  (const KKStr&          _researchOrg)   {researchOrg = _researchOrg;}

    static  KKStr ValidateCruiseName (const KKStr&  _cruiseName);

  private:
	  KKStr           cruiseName;
    KKStr           shipName;
	  KKStr           description;
    KKB::DateType   dateStart;
    KKB::DateType   dateEnd;
	  KKStr           location;
	  KKStr           objective;
	  KKStr           principal;
	  KKStr           researchOrg;
  };

  typedef  SipperCruise::SipperCruisePtr  SipperCruisePtr;



  class  SipperCruiseList:  public KKQueue<SipperCruise>
  {
  public:
    typedef  SipperCruiseList*  SipperCruiseListPtr;

    SipperCruiseList (bool _owner = false);
    ~SipperCruiseList ();

    void  SortByCruiseName ();
  };  /* SipperCruiseList */

  typedef  SipperCruiseList::SipperCruiseListPtr  SipperCruiseListPtr;
}



#endif
