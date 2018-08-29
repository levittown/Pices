#include "FirstIncludes.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


#include "SipperBuff.h"

#include "InstrumentDataManager.h"
#include "PicesVariables.h"
#include "SipperBuff3Bit.h"
#include "SipperBuff4Bit.h"
#include "SipperBuffBinary.h"
#include "Sipper3Buff.h"        // Came out with SIPPER III
#include "Sipper3RevBuff.h"     // Same as Sipper III, but accounts for some wires 
                                // being swapped, as per Bill Flanery.
#include "SipperBuff4Bit.h"
#include "SipperHeaderRec.h"
using namespace  MLL;


SipperBuff::SipperBuff (InstrumentDataManagerPtr  _instrumentDataManager,
                        RunLog&                   _log
                       ):

  bytesDropped              (0),
  byteOffset                (0),
  curRow                    (0),
  eof                       (false),
  fileName                  (),
  iOwnInstrumentDataManager (false),
  inFile                    (NULL),
  instrumentDataManager     (_instrumentDataManager),
  invalidLine               (false),
  log                       (_log),
  opened                    (false),
  recCount                  (0)

{
}



SipperBuff::SipperBuff (const KKStr&              _fileName,
                        InstrumentDataManagerPtr  _instrumentDataManager,
                        RunLog&                   _log
                       ):

  bytesDropped              (0),
  byteOffset                (0),
  curRow                    (0),
  eof                       (false),
  fileName                  (_fileName),
  iOwnInstrumentDataManager (false),
  inFile                    (NULL),
  instrumentDataManager     (_instrumentDataManager),
  invalidLine               (false),
  log                       (_log),
  opened                    (false),
  recCount                  (0)

{
  Open (fileName); 
}



SipperBuff::~SipperBuff ()
{
  if  (opened)
  {
    fclose (inFile);
    opened = false;
  }

  if  (iOwnInstrumentDataManager)
  {
    delete  instrumentDataManager;
    instrumentDataManager = NULL;
  }
}



void  SipperBuff::Reset ()
{
  if  (!opened)
    return;

  curRow       = 0;
  eof          = false;  
  recCount     = 0;
  bytesDropped = 0;
  rewind (inFile);
}  /* Reset */



void  SipperBuff::AssignSerialPorts (const SipperHeaderRecPtr  sipperHeaderRec)
{
  if  (instrumentDataManager)
    instrumentDataManager->AssignSerialPorts (sipperHeaderRec);
}   /* AssignSerialPorts */



void  SipperBuff::Open (const KKStr&  _fileName)
{
  eof = false;
  fileName = _fileName;

  inFile = osFOPEN (fileName.Str (), "rb");

  if  (inFile)
  {
    opened = true;
  }
  else
  {
    opened = false;
    cerr << "***ERROR***, SipperBuff Opening File[" << fileName << "]" << endl;
    //exit (-1);
  }
}  /* Open */



void  SipperBuff::ResetByteOffset (kkuint32  _byteOffset)
{
  if  (!inFile)
  {
    cerr << endl << endl
         << "SipperBuff::ResetByteOffset    SIPPER file is not opened." << endl
         << endl;
    return;
  }

#if defined(_MSC_VER)
  _fseeki64 (inFile, _byteOffset, SEEK_SET);
#else
  fseek (inFile, (kkint32)_byteOffset, SEEK_SET);
#endif

  byteOffset = _byteOffset;
}  /* ResetByteOffset */



void  SipperBuff::SkipOneByte ()
{
  if  (!inFile)
  {
    cerr << endl << endl
         << "SipperBuff::ResetByteOffset    SIPPER file is not opened." << endl
         << endl;
    return;
  }

  char  oneByte;
  fread (&oneByte, 1, 1, inFile);
  byteOffset++;
}  /* SkipOneByte */



void  SipperBuff::SkipToScanLine (kkuint32  scanLine)
{
  uchar*  lineBuff = new uchar[5000];
  kkuint32  lineSize;
  kkuint32  colCount[5000];
  kkuint32  pixelsInRow;
  bool    flow;
  
  lineSize = 1;

  while  ((curRow < scanLine) &&  (lineSize > 0))
  {
    GetNextLine (lineBuff, 5000, lineSize, colCount, pixelsInRow, flow);
  }

  delete  lineBuff;
  lineBuff = NULL;
}  /* SkipToScanLine */



void  SipperBuff::SkipToScanLine (kkuint32  _scanLine,
                                  kkuint64  _byteOffset
                                 )
{
#if defined(_MSC_VER)
  kkint32  returnCd = _fseeki64 (inFile, _byteOffset, SEEK_SET);
#else
  kkint32  returnCd = fseek (inFile, _byteOffset, SEEK_SET);
#endif


//kkint32 _fseeki64( 
//   FILE *stream,
//   __int64 offset,
//   kkint32 origin 
//);

  if  (returnCd != 0)
  {
    eof = true;
    return;
  }

  curRow = _scanLine - 1;
  byteOffset = _byteOffset;
};



#ifdef  WIN32
struct  SipperBuff::SipperRec
{
    uchar pix3      : 1;
    uchar pix2      : 1;
    uchar pix1      : 1;
    uchar pix0      : 1;

    uchar flow      : 1;  // 0 = Flow Meter in Position 0
                          // 1 = Flow Meter in Position 1

    uchar raw       : 1;  // 1 = Data is Raw 
                          // 0 = Compressed Run-Length

    uchar eol       : 1;  // 1 = End of Line Encountered, 
                          // 0 = End of Line not Encountered

    uchar cameraNum : 1;

    uchar pix11     : 1;
    uchar pix10     : 1;
    uchar pix9      : 1;
    uchar pix8      : 1;
    uchar pix7      : 1;
    uchar pix6      : 1;
    uchar pix5      : 1;
    uchar pix4      : 1;
};


#else


struct SipperBuff::SipperRec
{
    uchar cameraNum : 1;

    uchar eol       : 1;  // 1 = End of Line Encountered, 
                          // 0 = End of Line not Encountered

    uchar raw       : 1;  // 1 = Data is Raw 
                          // 0 = Compressed Run-Length

    uchar flow      : 1;  // 0 = Flow Meter in Position 0
                          // 1 = Flow Meter in Position 1

    uchar pix0      : 1;
    uchar pix1      : 1;
    uchar pix2      : 1;
    uchar pix3      : 1;

    uchar pix4      : 1;
    uchar pix5      : 1;
    uchar pix6      : 1;
    uchar pix7      : 1;
    uchar pix8      : 1;
    uchar pix9      : 1;
    uchar pix10     : 1;
    uchar pix11     : 1;
};
#endif



void  SipperBuff::DisplaySipperRec (SipperBuff::SipperRec&  sr)
{
  char  bs[30];

  kkint32  numOfBlanks;

  bs[7] = sr.cameraNum ? 'c' : '-';
  bs[6] = sr.eol       ? 'e' : '-';
  bs[5] = sr.raw       ? 'r' : '-';
  bs[4] = sr.flow      ? 'f' : '-';
  bs[3] = sr.pix0   + '0';
  bs[2] = sr.pix1   + '0';
  bs[1] = sr.pix2   + '0';
  bs[0] = sr.pix3   + '0';

  bs[15] = sr.pix4  + '0';
  bs[14] = sr.pix5  + '0';
  bs[13] = sr.pix6  + '0';
  bs[12] = sr.pix7  + '0';
  bs[11] = sr.pix8  + '0';
  bs[10] = sr.pix9  + '0';
  bs[9]  = sr.pix10 + '0';
  bs[8]  = sr.pix11 + '0';

  bs[16] = 0;

  numOfBlanks = sr.pix0 * 2048 + sr.pix1  * 1024 + sr.pix2 * 512 +
                sr.pix3 * 256  + sr.pix4  * 128  + sr.pix5 * 64  +
                sr.pix6 * 32   + sr.pix7  * 16   + sr.pix8 * 8   +
                sr.pix9 * 4    + sr.pix10 * 2    + sr.pix11;

  cout <<  bs << "  " << numOfBlanks; 
  cout << endl; 
}



SipperFileFormat  SipperBuff::GuessFormatOfNextLine (FILE*    in,
                                                     kkint32  _cameraNum,
                                                     RunLog&  _log
                                                    )
{
  SipperFileFormat  fileFormat;

  size_t  recsRead;

  SipperRec   sipperRec;

  kkint32  oneBitLen   = 0;   // Count for Binary Sipper
  kkint32  fourBitLen  = 0;   // Count for GrayScale sipper  or Sipper II
  kkint32  numOfBlanks = 0;

  kkint32  sipperRecsRead = 0;

  do  {
    do {
      //recsRead = fread (&sipperRec, sizeof (sipperRec), 1, in);  
      // sizeof can not work with bit fields
      recsRead = fread (&sipperRec, SIPPERRECSIZE, 1, in);
      // DisplaySipperRec  (sipperRec);

      sipperRecsRead++;
      if  ((sipperRecsRead > 5000)  ||  (recsRead == 0))
      {
        return  SipperFileFormat::UnKnown;
      }
    }  while  ((recsRead > 0)  &&  (sipperRec.cameraNum != _cameraNum));

    if  (sipperRec.raw == 0)
    {
      numOfBlanks = sipperRec.pix0 * 2048 + sipperRec.pix1  * 1024 + sipperRec.pix2 * 512 +
                    sipperRec.pix3 * 256  + sipperRec.pix4  * 128  + sipperRec.pix5 * 64  +
                    sipperRec.pix6 * 32   + sipperRec.pix7  * 16   + sipperRec.pix8 * 8   +
                    sipperRec.pix9 * 4    + sipperRec.pix10 * 2    + sipperRec.pix11;
      
      if  (numOfBlanks == 0)
        oneBitLen = oneBitLen + 4096;
      else
        oneBitLen = oneBitLen + numOfBlanks;

      fourBitLen = fourBitLen + numOfBlanks * 4;
    }
    else
    {
      fourBitLen = fourBitLen + 4;
      
      if  ((oneBitLen >= 4084)  &&  (oneBitLen < 4096))
        oneBitLen = 4096;
      else
        oneBitLen = oneBitLen + 12;
    }
  }  while  ((recsRead > 0)  &&  (sipperRec.eol == 0));
  
  if  (fourBitLen == 4096)
  {
    fileFormat = SipperFileFormat::ThreeBit;
  }

  else if  ((oneBitLen == 2048)  ||  (oneBitLen == 4096))
  {
    fileFormat = SipperFileFormat::Binary;
  }

  else
  {
    fileFormat = SipperFileFormat::UnKnown;
  }

  _log.Level (50) << "SipperBuff::GuessFormatOfNextLine  guessed fileFormat: " << SipperFileFormatToStr (fileFormat);

  return  fileFormat;
}  /*  GuessFormatOfFile */



SipperFileFormat  SipperBuff::GuessFormatOfFile (const KKStr&  _fileName,
                                                 kkint32       _cameraNum,
                                                 RunLog&       _log
                                                )
{
  // First we will see if file is the new SIPPER 3 format,  if it is not
  // then we will try to see in one of the other two formats.


  SipperFileFormat  fileFormat = SipperFileFormat::UnKnown;
  SipperFileFormat  lastFormat;
  {
    Sipper3BuffPtr  s3 = new Sipper3Buff (_fileName, NULL, _log);
    if  (s3->Opened ())
    {
      if  (s3->FileFormatGood ())
        fileFormat = SipperFileFormat::Sipper3;
    }

    delete s3;
  }

  if  (fileFormat == SipperFileFormat::Sipper3)
  {
    // Since we know it is a SIPPER 3  file,  we are done and can return 
    return  SipperFileFormat::Sipper3;
  }

  {
    SipperBuff4BitPtr  sf = new SipperBuff4Bit (_fileName, NULL, _log);
    if  (sf->Opened ())
    {
      if  (sf->FileFormatGood ())
        fileFormat = SipperFileFormat::Sipper4Bit;
    }

    delete sf;
  }

  if  (fileFormat == SipperFileFormat::Sipper4Bit)
  {
    // Since we know it is a SIPPER 4bit  file,  we are done and can return 
    return  SipperFileFormat::Sipper4Bit;
  }
  
  FILE*  in = osFOPEN (_fileName.Str (), "rb");

  if  (!in)
  {
    kkint32  errorCode = errno;

    cerr << "***ERROR[" << errorCode << "]***, SipperBuff::GuessFormatOfFile  Can not open file[" << _fileName << "]." 
         << endl;

    return  SipperFileFormat::UnKnown;
  }

  kkint32  numInARow = 0;

  fileFormat = SipperFileFormat::UnKnown;

  kkint32  outerTries = 0;

  while  ((numInARow < 3)  &&  (outerTries < 2))
  {
    kkint32  numOfTries = 0;
    rewind (in);
    if  (outerTries > 0)
    {
      char  buff[20];
      fread (buff, 1, 1, in);
    }

    while  ((numInARow < 3)  &&  (numOfTries < 2000))
    {
      lastFormat = fileFormat;
      fileFormat = GuessFormatOfNextLine (in, _cameraNum, _log);
      numOfTries++;
      // WaitForEnter ();

      if  ((fileFormat == SipperFileFormat::UnKnown)  ||  (lastFormat != fileFormat))
      {
        numInARow = 0;
      }
      else
      {
        numInARow++;
      }
    }
    outerTries++;
  }

  fclose (in);

  return  fileFormat;
}  /*  GuessFormatOfFile */



SipperBuffPtr  SipperBuff::CreateSipperBuff (const KKStr&              _fileName,
                                             kkint32                   _cameraNum,
                                             InstrumentDataManagerPtr  _instrumentDataManager,
                                             RunLog&                   _log
                                            )
{
  SipperFileFormat  format = GuessFormatOfFile (_fileName, _cameraNum, _log);

  return  CreateSipperBuff (format, _fileName, _cameraNum, _instrumentDataManager, _log);
}  /* CreateSipperBuff */



SipperBuffPtr  SipperBuff::CreateSipperBuff (SipperFileFormat          _format,
                                             const KKStr&              _fileName,
                                             kkint32                   _cameraNum,
                                             InstrumentDataManagerPtr  _instrumentDataManager,
                                             RunLog&                   _log
                                            )
{
  SipperBuffPtr  sipperBuff = NULL;

  if  (_format == SipperFileFormat::UnKnown)
  {
    // We are going to have to figure out the format 
    _format = GuessFormatOfFile (_fileName, _cameraNum, _log);
  }

  if  (_format == SipperFileFormat::Binary)
    sipperBuff = new SipperBuffBinary (_fileName, _cameraNum, _instrumentDataManager, _log);

  else if  (_format == SipperFileFormat::ThreeBit)
    sipperBuff = new SipperBuff3Bit   (_fileName, _instrumentDataManager, _log);

  else if  (_format == SipperFileFormat::Sipper3)
    sipperBuff = new Sipper3Buff      (_fileName, _instrumentDataManager, _log);

  else if  (_format == SipperFileFormat::Sipper4Bit)
    sipperBuff = new SipperBuff4Bit   (_fileName, _instrumentDataManager, _log);

  else if  (_format == SipperFileFormat::Sipper3)
    sipperBuff = new Sipper3Buff      (_fileName, _instrumentDataManager, _log);

  else if  (_format == SipperFileFormat::Sipper3Rev)
    sipperBuff = new Sipper3RevBuff   (_fileName, _instrumentDataManager, _log);

  if  ((sipperBuff != NULL)  &&  (!sipperBuff->Opened ()))
  {
    delete  sipperBuff;
    sipperBuff = NULL;
  }

  return  sipperBuff;
}  /* CreateSipperBuff */



void  SipperBuff::DetermineCropSettings  (kkuint32&  cropLeft,
                                          kkuint32&  cropRight
                                         )
{
  int  maxLineSize = 5000;
  uchar*  lineBuff         = new uchar[maxLineSize];
  uchar*  lineBuffSmoothed = new uchar[maxLineSize];
  kkuint32  lineSize;
  kkuint32  colCount[5000];
  kkuint32  pixelsInRow;
  bool    flow;
  
  lineSize = 1;

  kkint32  x = 0;

  uchar  maxVals[4096];
  kkuint32 totVals[4096];

  for  (x = 0;  x < 4096;  ++x)
  {
    maxVals[x] = 0;
    totVals[x] = 0;
  }

  vector<uchar>  smoothWindow(3, 0);

  // Skip first 50K lines
  for  (x = 0;  (x < 50000)  &&  (!eof);  ++x)
  {
    pixelsInRow = 0;
    GetNextLine (lineBuff, maxLineSize, lineSize, colCount, pixelsInRow, flow);
  }

  int  count = 0;
  pixelsInRow = 0;
  GetNextLine (lineBuff, maxLineSize, lineSize, colCount, pixelsInRow, flow);
  for  (x = 0;  (x < 300000)  &&  (!eof);  ++x)
  {
    ++count;

    kkuint32  y = 0;

    // Medium Smooth Line
    for  (y = 1;  y <  (4096 - 1);  ++y)
    {
      uchar sw0 = lineBuff[y - 1];
      uchar sw1 = lineBuff[y];
      uchar sw2 = lineBuff[y + 1];

      if  ((sw0 == sw1)  &&  (sw0 == sw2))
      {
        lineBuffSmoothed[y] = sw0;
      }
      else
      {
        smoothWindow[0] = sw0;
        smoothWindow[1] = sw1;
        smoothWindow[2] = sw2;
        sort (smoothWindow.begin (), smoothWindow.end ());
        lineBuffSmoothed[y] = smoothWindow[1];
      }
    }

    for  (y = 0;  y < 4096;  ++y)
    {
      maxVals[y] = Max (maxVals[y], lineBuffSmoothed[y]);
      //totVals[y] += lineBuffSmoothed[y];
    }

    GetNextLine (lineBuff, maxLineSize, lineSize, colCount, pixelsInRow, flow);
  }

  //vector<kkuint32>  avgVals (4096, 0);
  //for  (x = 0;  x < 4096;  ++x)
  //  avgVals[x] = totVals[x] / count;

  cropLeft = 1;
  while  ((cropLeft < 2500)  &&  (maxVals[cropLeft] < 50))
    ++cropLeft;

  cropRight = 4094;
  while  ((cropRight > 2000)  &&  (maxVals[cropRight] < 50))
    --cropRight;

  delete  lineBuff;          lineBuff         = NULL;
  delete  lineBuffSmoothed;  lineBuffSmoothed = NULL;
}  /* DetermineCropSettings */



void  SipperBuff::ReportInstrumentData (uchar  instrumentId,
                                        bool   text,
                                        uchar  data
                                       )
{
  if  (!instrumentDataManager)
  {
    // Will try to put Report directory in same directory with SIPPER file.

    KKStr  sipperFileDir = osGetPathPartOfFile (fileName);
    if  (sipperFileDir.Empty ())
      sipperFileDir = osGetCurrentDirectory ();
    KKStr  reportDir = osAddSlash (PicesVariables::ReportDir ()) + "InstrumentReports";

    instrumentDataManager = new InstrumentDataManager (fileName, NULL, reportDir, log);
    iOwnInstrumentDataManager = true;
  }

  instrumentDataManager->ReportInstrumentData (curRow, instrumentId, text, data);
}  /* ReportInstrumentData */



KKStr  MLL::SipperFileFormatToStr (SipperFileFormat  fileFormat)
{
  if  (fileFormat == SipperFileFormat::Binary)
    return  "Binary";

  if  (fileFormat == SipperFileFormat::ThreeBit)
    return  "3Bit";

  if  (fileFormat == SipperFileFormat::Sipper3)
    return  "Sipper3";

  if  (fileFormat == SipperFileFormat::Sipper4Bit)
    return  "Sipper4Bit";

  if  (fileFormat == SipperFileFormat::Sipper3Rev)
    return  "Sipper3Rev";

  return  "";
}  /* SipperFileFormatToStr */



SipperFileFormat  MLL::SipperFileFormatFromStr (KKStr  fileFormatStr)
{
  fileFormatStr.Upper ();

  if  ((fileFormatStr == "BINARY")      ||  (fileFormatStr == "1BIT"))
    return  SipperFileFormat::Binary;

  if  ((fileFormatStr == "3BIT")        ||  (fileFormatStr == "SIPPER2"))
    return  SipperFileFormat::ThreeBit;

  if  ((fileFormatStr == "SIPPER3")     ||  (fileFormatStr == "S3")  ||  (fileFormatStr == "3"))
    return  SipperFileFormat::Sipper3;

  if  (fileFormatStr.EqualIgnoreCase ("Sipper4Bit")  ||  (fileFormatStr == "S4")  ||  (fileFormatStr == "4"))
    return  SipperFileFormat::Sipper4Bit;

  if  ((fileFormatStr == "SIPPER3REV")  ||  (fileFormatStr == "SIPPER3REV")  ||  (fileFormatStr == "S3R"))
    return  SipperFileFormat::Sipper3Rev;

  return  SipperFileFormat::UnKnown;
}  /* SipperFileFormatFromStr */



KKStr  MLL::SipperFileAvailableOptions ()
{
  return  "Binary, 1Bit, 3Bit, Sipper2, Sipper3, S3, Sipper3Rev, S3R";
}
