#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <stdio.h>
#include  <fstream>
#include  <iostream>
#include  <vector>

#include  "MemoryDebug.h"


using namespace std;

#include  "KKBaseTypes.h"
#include  "KKStr.h"
using namespace KKB;


#include  "InstrumentDataUserMsgs.h"
#include  "InstrumentDataManager.h"
using namespace MLL;

InstrumentDataUserMsgs::InstrumentDataUserMsgs (InstrumentDataManagerPtr _manager,
                                                const KKStr&            _reportDir,
                                                bool                     _text,
                                                kkint32                  _instrumentId
                                               ):

    InstrumentDataReport     (_manager, _reportDir, _text, _instrumentId, false),
    curTextLine              (),
    curTextLineStartScanLine (-1),
    lastTextLine             (),
    prevEolChar              (0)
{
}



InstrumentDataUserMsgs::~InstrumentDataUserMsgs ()
{
  report << curTextLine;
}






void  InstrumentDataUserMsgs::ReportInstrumentData (kkuint32 curScanLine,
                                                    uchar  sensorData
                                                   )
{
  if  ((sensorData == '\n')  ||  (sensorData == '\r'))
  {
    char  lastEolChar = prevEolChar;
    prevEolChar = sensorData;

    if  (lastEolChar == '\n')
    {
      if  (sensorData == '\r')
        return;
    }

    else if  (lastEolChar == '\r')
      if  (sensorData == '\n')
        return;

    //report << endl;
    //report << curTextLineStartScanLine << "\t" << curTextLine << endl;
    lastTextLine = curTextLine;
    curTextLine = "";
    curTextLineStartScanLine = curScanLine;
  }

  report << (char)sensorData;
  report.flush ();

  prevEolChar = 0;
  if  (curTextLine.Empty ())
    curTextLineStartScanLine = curScanLine;

  curTextLine.Append ((char)sensorData);
}  /* ReportInstrumentData */
