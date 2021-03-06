#include "FirstIncludes.h"
#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include  "DataBaseLogEntry.h"
using namespace MLL;




DataBaseLogEntry::DataBaseLogEntry ():
  logEntryId        (0),
  progCode          (""),
  progName          (""),
  dateTimeCompiled  (),
  cmdLine           (""),
  compName          (""),
  dataBaseUserName  (""),
  compUserName      (""),
  dateTimeStart     (),
  dateTimeStartUtc  (),
  dateTimeEnd       (),
  cpuTimeUsed       (),
  completionStatus  (""),
  sipperFileName    ("")
{
}


DataBaseLogEntry::DataBaseLogEntry (const KKStr&     _progCode,
                                    const KKStr&     _progName,
                                    const DateTime&  _dateTimeCompiled,
                                    const KKStr&     _cmdLine,
                                    const KKStr&     _compName,
                                    const KKStr&     _compUserName,
                                    const KKStr&     _sipperFileName   /**<  Root name of Sipper File */
                                   ):
  logEntryId        (0),
  progCode          (_progCode),
  progName          (_progName),
  dateTimeCompiled  (_dateTimeCompiled),
  cmdLine           (_cmdLine),
  compName          (_compName),
  dataBaseUserName  (""),
  compUserName      (_compUserName),
  dateTimeStart     (),
  dateTimeStartUtc  (),
  dateTimeEnd       (),
  cpuTimeUsed       (),
  completionStatus  (""),
  sipperFileName    (_sipperFileName)

{
}





DataBaseLogEntry::~DataBaseLogEntry ()
{
}



DataBaseLogEntryList::DataBaseLogEntryList (bool _owner):
      KKQueue<DataBaseLogEntry> (_owner)
{
}


DataBaseLogEntryList::~DataBaseLogEntryList ()
{
}


void  DataBaseLogEntryList::SortByProgCode ()
{
  sort (begin (), end (), [](DataBaseLogEntryPtr p1, DataBaseLogEntryPtr  p2) -> bool
  {
    if  (p1->ProgCode () < p2->ProgCode ())
      return true;

    else if  (p1->ProgCode () > p2->ProgCode ())
      return false;

    return  (p1->DateTimeStart () < p2->DateTimeStart ());
  });
}  /* SortByProgCode */
