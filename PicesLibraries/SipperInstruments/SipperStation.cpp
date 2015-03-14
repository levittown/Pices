#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <memory>

#include  <string>
#include  <iostream>
#include  <fstream>
#include  <vector>
#include  <map>

#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"

using namespace std;

#include  "OSservices.h"
#include  "KKStr.h"
using namespace KKB;


#include "SipperStation.h"
using  namespace  SipperHardware;


SipperStation::SipperStation ():
    cruiseName     (),
    stationName    (),
    description    (),
    latitude       (0),
    longitude      (0),
    dateTimeStart  (osGetLocalDateTime ())
{

}



SipperStation::~SipperStation ()
{
}





SipperStationList::SipperStationList (bool _owner):
   KKQueue<SipperStation> (_owner)
{
}



SipperStationList::~SipperStationList ()
{
}

