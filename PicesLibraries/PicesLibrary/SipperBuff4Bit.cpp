#include "FirstIncludes.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "GlobalGoalKeeper.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


#include "SipperBuff.h"

//#include "Variables.h"

#include "SipperBuff4Bit.h"

using namespace  MLL;

#pragma pack(push,1)
#pragma pack(show)
struct  SipperBuff4Bit::OpRecEndOfScanLine
{
  uchar opCode:  4;  /** 0 = End of Scan Line. */
  uchar filler:  4;
};



struct  SipperBuff4Bit::OpRecTextBlock1
{
  /** Will handle text blocks in one or more sections with 2^11 characters in each *
   *  The last section will have 'endOfText' == '1'                                 */

  uchar opCode:      4;  /**< 1 = Text Block.                                      */
  uchar endOfText:   1;  /**< 0 = More Text blocks to follow, 1 = last text block. */
  uchar lenHighBits: 3;  /**< Len = 16 * lenHighBits + lenLowBits                  */
};  /* OpRecTextBlock1 */
  


struct  SipperBuff4Bit::OpRecTextBlock2
{
  uchar lenLowBits: 8;     /**< Add this value to the previous rec 'lenHighBits' field to get the length of the text block. */
};



struct  SipperBuff4Bit::OpRecInstrumentDataWord1
{
  uchar  opCode: 4;  /**< 2 = InstrumentDataWord  32 Bit Instrument Data */
  uchar  idNum:  4;
};



struct  SipperBuff4Bit::OpRecInstrumentDataWord2
{
  OpRecInstrumentDataWord2 (): scanLineNum (0) {}
  kkuint32  scanLineNum;
};  /**<  OpCode 2  */



struct  SipperBuff4Bit::OpRecInstrumentDataWord3
{
  OpRecInstrumentDataWord3 (): word () {}
  WordFormat32Bits  word;
};  /**<  OpCode 2  */




struct  SipperBuff4Bit::OpRecRunLen
{
  /** For one or two raw pixels */
  uchar  opCode     :4;      /**< 4 =   2 Pixels
                              *   5 =   3 Pixels
                              *   6 =   4 Pixels
                              *   7 =   5 Pixels
                              *   8 =   6 Pixels
                              *   9 =   7 Pixels
                              */
  uchar  pixelValue :4;
};  /* OpRecRunLen */



struct  SipperBuff4Bit::OpRecRun256Len1
{
  /** For one or two raw pixels */
  uchar  opCode     :4;      /**< 10 = up to 256 run-length  */
  uchar  pixelValue :4;
};  /* OpRecRun256Len1 */


struct  SipperBuff4Bit::OpRecRun256Len2
{
  /** For one or two raw pixels */
  uchar  runLen     :8;      /**< 0= 1-Pixel, 1=2-Pixels, ... 255=256-Pixels */
};  /* OpRecRun256Len2 */



struct  SipperBuff4Bit::OpRecRaw1Pixel
{
  uchar  opCode     :4;      /**< 11 = One raw pixel      */
  uchar  pixelValue :4;
};  /* OpRecRaw1Pixel */



struct  SipperBuff4Bit::OpRecRaw32Pixels
{
  /** For one or two raw pixels */
  uchar  opCode  :4;      /**< 12 = Even run length 2 thru 32 pixels      */
  uchar  len     :4;      /**< Num-Raw-Pixels = (len + 1) * 2   */
  /** To be followed by 'len' RawPixel recs */
};  /* OpRecRunLen1Bit */



struct  SipperBuff4Bit::OpRecRaw513Pixels1
{
  /** For one or two raw pixels */
  uchar  opCode  :4;      /**< 13 = Odd number of pixels, up to 513 pixels.          */
  uchar  lenHigh :4;      /**< Num-Raw-Pixels = 1 + 2 * (1 + lenHigh * 16 + lenLow)  */
  /** To be followed by 'len' RawPixelRec's */
};  /* OpRecRaw513Pixels1 */


struct  SipperBuff4Bit::OpRecRaw513Pixels2
{
  /** For one or two raw pixels */
  uchar  lenLow :4;      /**< 13 = Odd number of pixes, up to 513 pixels.    */
  uchar  pix0   :4;      /**< 1st Raw pixel  */

  /** To be followed by (1 + lenHigh * 16 + lenLow) 'RawPixelRecs' */
};  /* OpRecRaw513Pixels2 */



struct  SipperBuff4Bit::RawPixelRec
{
  uchar  pix0   :4;
  uchar  pix1   :4;
};  /* OpRecord */



union  SipperBuff4Bit::OpRec
{
  OpRecEndOfScanLine         endOfScanLine;        /**< OpCode 0         */
  
  OpRecTextBlock1            textBlock1;           /**< OpCode 1         */
  OpRecTextBlock2            textBlock2;
  
  OpRecInstrumentDataWord1   instrumentDataWord1;  /**<  OpCode 2        */
  OpRecRunLen                runLen;               /**< OpCodes 4 thru 9 */

  OpRecRun256Len1            run256Len1;           /**< OpCode 10        */
  OpRecRun256Len2            run256Len2;
  
  OpRecRaw1Pixel             raw1Pixel;            /**< OpCode 11        */
  
  OpRecRaw32Pixels           raw32Pixels;          /**< OpCode 12        */

  OpRecRaw513Pixels1         raw513Pixels1;        /**< OpCode 13        */
  OpRecRaw513Pixels2         raw513Pixels2;

  RawPixelRec                rawPixels;            /* Two 4 bit pixels   */

  uchar                      textChar;
};  /* OpRec */



#pragma pack(pop)
#pragma pack(show)

uchar*  SipperBuff4Bit::convTable4BitTo8Bit = NULL;
uchar*  SipperBuff4Bit::convTable8BitTo4Bit = NULL;
uchar*  SipperBuff4Bit::compensationTable   = NULL;

SipperBuff4Bit::SipperBuff4Bit (InstrumentDataManagerPtr  _instrumentDataManager,
                                RunLog&                   _log
                               ):
  SipperBuff (_instrumentDataManager, _log),
  rawPixelRecBuffer     (NULL),
  rawPixelRecBufferSize (0),
  rawPixelRecBufferLen  (0),
  encodedBuff           (NULL),
  encodedBuffLen        (0),
  encodedBuffNext       (NULL),
  encodedBuffSize       (0),
  rawStr                (NULL),
  rawStrLen             (0),
  rawStrSize            (0),
  runLen                (0),
  runLenChar            (0),
  curCompStatus         (csNull),
  pixelsPerScanLine     (2048)
{
  BuildConversionTables ();
  PrintSizeInfo ();
  byteOffset = 0;
}



SipperBuff4Bit::SipperBuff4Bit (const KKStr&              _fileName,
                                InstrumentDataManagerPtr  _instrumentDataManager,
                                RunLog&                   _log
                               ):
  SipperBuff      (_fileName, _instrumentDataManager, _log),
  rawPixelRecBuffer     (NULL),
  rawPixelRecBufferSize (0),
  rawPixelRecBufferLen  (0),
  encodedBuff           (NULL),
  encodedBuffLen        (0),
  encodedBuffNext       (NULL),
  encodedBuffSize       (0),
  rawStr                (NULL),
  rawStrLen             (0),
  rawStrSize            (0),
  runLen                (0),
  runLenChar            (0),
  curCompStatus         (csNull),
  pixelsPerScanLine     (2048)
{
  BuildConversionTables ();
  AllocateEncodedBuff ();
  AllocateRawStr (pixelsPerScanLine + 100);
  byteOffset = 0;

  //if  (Opened ())
  //  ExtractSipperHeaderInfo ();
}



void  SipperBuff4Bit::PrintSizeInfo ()
{
  /*
  #pragma pack(1)
  int x01 = sizeof (OpRecEndOfScanLine  ) ;
  int x02 = sizeof (OpRecTextBlock1     ) ;
  int x03 = sizeof (OpRecTextBlock2     ) ;
  int x04 = sizeof (OpRecRunLen         ) ;
  int x05 = sizeof (OpRecRun256Len1     ) ;
  int x06 = sizeof (OpRecRun256Len2     ) ;
  int x07 = sizeof (OpRecRaw1Pixel      ) ;
  int x08 = sizeof (OpRecRaw32Pixels    ) ;
  int x09 = sizeof (OpRecRaw513Pixels1  ) ;
  int x10 = sizeof (OpRecRaw513Pixels2  ) ;
  int x11 = sizeof (RawPixelRec         ) ;
  int x12 = sizeof (OpRec );


  cout << endl
       <<   "OpRecEndOfScanLine  " << "\t" << sizeof (OpRecEndOfScanLine  ) << endl
       <<   "OpRecTextBlock1     " << "\t" << sizeof (OpRecTextBlock1     ) << endl
       <<   "OpRecTextBlock2     " << "\t" << sizeof (OpRecTextBlock2     ) << endl
       <<   "OpRecRunLen         " << "\t" << sizeof (OpRecRunLen         ) << endl
       <<   "OpRecRun256Len1     " << "\t" << sizeof (OpRecRun256Len1     ) << endl
       <<   "OpRecRun256Len2     " << "\t" << sizeof (OpRecRun256Len2     ) << endl
       <<   "OpRecRaw1Pixel      " << "\t" << sizeof (OpRecRaw1Pixel      ) << endl
       <<   "OpRecRaw32Pixels    " << "\t" << sizeof (OpRecRaw32Pixels    ) << endl
       <<   "OpRecRaw513Pixels1  " << "\t" << sizeof (OpRecRaw513Pixels1  ) << endl
       <<   "OpRecRaw513Pixels2  " << "\t" << sizeof (OpRecRaw513Pixels2  ) << endl
       <<   "RawPixelRec         " << "\t" << sizeof (RawPixelRec         ) << endl
       << endl;
  */
}    



SipperBuff4Bit::~SipperBuff4Bit (void)
{
  //if  (opened)
  //  Close ();

  delete encodedBuff;          encodedBuff       = NULL;
  delete rawPixelRecBuffer;    rawPixelRecBuffer = NULL;
  delete rawStr;               rawStr            = NULL;
}



void  SipperBuff4Bit::AllocateRawPixelRecBuffer (kkuint32 size)
{
  delete  rawPixelRecBuffer;
  rawPixelRecBuffer     = new RawPixelRec[size];
  rawPixelRecBufferSize = size;
  rawPixelRecBufferLen  = 0;
}  /* AllocateRawPixelRecBuffer */



void  SipperBuff4Bit::AllocateRawStr (kkuint16  size)
{
  if  (rawStr)
  {
    uchar*  newRawStr = new uchar[size];
    kkuint16  bytesToCopy = Min (rawStrLen, size);
    memcpy (newRawStr, rawStr, bytesToCopy);
    delete rawStr;
    rawStr = NULL;
    rawStrLen = bytesToCopy;
    rawStrSize = size;
  }
  else
  {
    rawStrSize = size;
    rawStrLen = 0;
    rawStr = new uchar[rawStrSize];
  }
}  /* AllocateRawStr */




void  SipperBuff4Bit::AllocateEncodedBuff ()
{
  delete  encodedBuff;  encodedBuff  = NULL;

  kkint32  frameWidth = Max<kkuint32> ((kkuint32)2048, 4 * PixelsPerScanLine ());
  
  encodedBuffSize = (int)(1.2f * (float)frameWidth);

  encodedBuff = new OpRec[encodedBuffSize];
  encodedBuffNext = encodedBuff;
  encodedBuffSize = encodedBuffSize;
  encodedBuffLen  = 0;
}  /* AllocateEncodedBuff */



void  SipperBuff4Bit::ReSizeEncodedBuff (kkuint32  newSize)
{
  OpRecPtr  newEncodedBuff = new OpRec[newSize];

  kkuint32  recsToMove = Min (newSize, encodedBuffLen);

  memcpy (newEncodedBuff, encodedBuff, recsToMove * sizeof (OpRec));
  encodedBuffNext = newEncodedBuff + (encodedBuffNext - encodedBuff);

  delete  encodedBuff;
  encodedBuff = newEncodedBuff;
  encodedBuffSize = newSize;
}  /* ReSizeEncodedBuff */



void  SipperBuff4Bit::BuildConversionTables ()
{
  GlobalGoalKeeper::StartBlock ();
  if  (!convTable4BitTo8Bit)
  {
    kkint32 x = 0;
    kkint32 y = 0;
    kkint32 inc = 256 / 16;
    convTable4BitTo8Bit = new uchar[16];
    convTable8BitTo4Bit = new uchar[256];
    for  (x = 0;  x < 16;  ++x)
    {
      kkint32  this8Bit = x * inc;
      kkint32  next8Bit = (x + 1) * inc;

      kkint32  fourBitTo8BitNum = (kkint32)(this8Bit + (kkint32)(((float)x / 16.0f)  * (float)inc));

      convTable4BitTo8Bit[x] = (uchar)fourBitTo8BitNum;
      for  (y = this8Bit;  y < next8Bit;  ++y)
        convTable8BitTo4Bit[y] = x;
    }

    compensationTable = new uchar[256];
    for  (kkint16 pv = 0;  pv < 256;  ++pv)
    {
      uchar encodedValue = convTable8BitTo4Bit[pv];
      compensationTable[pv] = convTable4BitTo8Bit[encodedValue];
    }
    atexit (SipperBuff4Bit::ExitCleanUp);
  }

  GlobalGoalKeeper::EndBlock ();
}  /* BuildConversionTables */



const uchar*  SipperBuff4Bit::CompensationTable ()
{
  GlobalGoalKeeper::StartBlock ();
  if  (!compensationTable)
    BuildConversionTables ();
  GlobalGoalKeeper::EndBlock ();
  return  compensationTable;
}  /* CompensationTable */



void  SipperBuff4Bit::ExitCleanUp ()
{
  GlobalGoalKeeper::StartBlock ();
  delete  convTable4BitTo8Bit;  convTable4BitTo8Bit = NULL;
  delete  convTable8BitTo4Bit;  convTable8BitTo4Bit = NULL;
  delete  compensationTable;    compensationTable   = NULL;
  GlobalGoalKeeper::EndBlock ();
}



bool  SipperBuff4Bit::FileFormatGood ()
{
  if  (!Opened ())
    return false;

  uchar  buff[10240];

  kkuint32  linesRead = 0;
  kkuint32  goodLinesInARow = 0;
  kkuint32  lineLen = 0;

  GetNextScanLine (buff, sizeof (buff), lineLen);

  while  (!Eof ()  &&  (linesRead < 100)  &&  (goodLinesInARow < 20))
  {
    if  (lineLen == pixelsPerScanLine)
      ++goodLinesInARow;
    else
      goodLinesInARow = 0;

    GetNextScanLine (buff, sizeof (buff), lineLen);

    ++linesRead;
  }

  return  goodLinesInARow > 19;
}  /* FileFormatGood */



/*****************************************************************************/
/*        Following routines are used for READING Scanner Files.             */
/*****************************************************************************/


void  SipperBuff4Bit::ProcessTextBlock (const OpRec&  rec)
{
  OpRec  rec2;

  auto  recsRead = fread (&rec2, sizeof (rec2), 1, inFile);
  if  (recsRead < 1)
  {
    eof = true;
    return;
  }
  byteOffset += sizeof (rec2);

  kkuint32  numTextBytes = 256 * rec.textBlock1.lenHighBits + rec2.textBlock2.lenLowBits;
  char* textMsgPtr = new char[numTextBytes + 1];  // "+ 1" for terminating NULL Character.
  kkuint32 textMsgLen = numTextBytes;
   
  recsRead = fread (textMsgPtr, 1, numTextBytes, inFile);
  if  (recsRead < numTextBytes)
     eof = true;
  else
  {
    byteOffset +=numTextBytes;
    textMsgPtr[recsRead] = 0;
    //ReportTextMsg (textMsgPtr, textMsgLen);
  }
  delete  textMsgPtr;  textMsgPtr = NULL;
  textMsgLen = 0;
}  /* ProcessTextBlock */



void  SipperBuff4Bit::ProcessInstrumentDataWord (const OpRec&  rec)
{
  uchar  idNum = rec.instrumentDataWord1.idNum;

  OpRecInstrumentDataWord2  rec2;
  OpRecInstrumentDataWord3  rec3;
  kkuint32  recsRead = (kkuint32)fread (&rec2, sizeof (rec2), 1, inFile);
  if  (recsRead < 1)
    eof = true;
  else
  {
    byteOffset += sizeof (rec2);
    recsRead = (kkuint32)fread (&rec3, sizeof (rec3), 1, inFile);
    if  (recsRead < 1)
      eof = true;
    else
    {
      byteOffset += sizeof (rec3);
      //ReportInstrumentDataWord (idNum, rec2.scanLineNum, rec3.word);
    }
  }
}  /* ProcessInstrumentDataWord */



void  SipperBuff4Bit::ProcessRawPixelRecs (kkuint16  numRawPixelRecs,
                                           uchar*  lineBuff,
                                           kkuint32  lineBuffSize,
                                           kkuint32& bufferLineLen
                                          )
{
  if  (numRawPixelRecs > rawPixelRecBufferSize)
    AllocateRawPixelRecBuffer (numRawPixelRecs + 30);

  size_t  recsRead = fread (rawPixelRecBuffer, sizeof (RawPixelRec), numRawPixelRecs, inFile);
  if  (recsRead < numRawPixelRecs)
  {
    eof = true;
    return;
  }

  byteOffset += sizeof (RawPixelRec) * numRawPixelRecs;

  for  (kkuint32 x = 0;  x < numRawPixelRecs;  ++x)
  {
    if  (bufferLineLen < lineBuffSize)  {lineBuff[bufferLineLen] = convTable4BitTo8Bit[rawPixelRecBuffer[x].pix0];   ++bufferLineLen;}
    if  (bufferLineLen < lineBuffSize)  {lineBuff[bufferLineLen] = convTable4BitTo8Bit[rawPixelRecBuffer[x].pix1];   ++bufferLineLen;}
  }

  return;
}  /* ProcessRawPixelRecs */



void  SipperBuff4Bit::GetNextScanLine (uchar*    lineBuff,
                                       kkuint32  lineBuffSize,
                                       kkuint32& lineLen
                                      )
{
  bool   eol = false;
  uchar  opCode = 0;
  OpRec  rec;
  OpRec  rec2;
  size_t recsRead = 0;

  kkuint32  bufferLineLen = 0;

  uchar*  textMsg     = NULL;            
  kkuint32  textMsgLen  = 0;

  lineLen = 0;

  curRowByteOffset = byteOffset;

  if (feof (inFile)) 
  {
    eof = true;
    return;
  }

  do
  {
    recsRead = (kkuint32)fread (&rec, sizeof (rec), 1, inFile);
    if  (recsRead == 0)
    {
      eol = true;
      break;
    }

    byteOffset += sizeof (rec);

    opCode = rec.textBlock1.opCode;

    if  (opCode == 0)
      eol = true;

    else if  (bufferLineLen >= lineBuffSize)
    {
      // Something has gone wrong,  we should have encountered a eol opCode before this point;  that is the
      // length of the encoded line is exceeding the scan line length.
      eol = true;
      ungetc (rec.textChar, inFile);
      byteOffset -= 1;
      break;
    }

    else if  (opCode == 1)
      ProcessTextBlock (rec);

    else if  (opCode == 2)
      ProcessInstrumentDataWord (rec);

    else if  ((opCode >= 4)  &&  (opCode <= 9))
    {
      // OpCode determines RunLen  (4=2, 5=3, ... 9=7)
      kkuint32 runLen = opCode - 2;
      runLenChar = convTable4BitTo8Bit [rec.runLen.pixelValue];
      kkuint32  newLineSize = bufferLineLen + runLen;

      if  (newLineSize > lineBuffSize)
      {
        cerr << "SipperBuff4Bit::GetNextScanLine   ***ERROR***  Exceeding 'bufferLineLen';  ScanLine[" << curRow << "]." << endl;
        cerr << "                         newLineSize: " << newLineSize << endl;
      }
      else
      {
        memset (&(lineBuff[bufferLineLen]), runLenChar,  runLen);
        bufferLineLen = newLineSize;
      }
    }

    else if  (opCode == 10)  /* OpRecRun256Len1 */
    {
      // Run-Length (1 thru 256 Pixels).
      recsRead = (kkuint32)fread (&rec2, sizeof (rec2), 1, inFile);
      if  (recsRead < 1)
        eol = true;
      else
      {
        byteOffset += sizeof (rec2);
        kkuint32  runLen = 1 + rec2.run256Len2.runLen;
        kkuint32  newLineSize = bufferLineLen + runLen;
        if  (newLineSize > lineBuffSize)
        {
          cerr << "SipperBuff4Bit::GetNextScanLine   ***ERROR***  Exceeding 'bufferLineLen';  ScanLine[" << curRow << "]." << endl;
        }
        else
        {
          uchar  runLenChar = convTable4BitTo8Bit [rec.run256Len1.pixelValue];
          memset (&(lineBuff[bufferLineLen]), runLenChar,  runLen);
          bufferLineLen = newLineSize;
        }
      }
    }

    else if  (opCode == 11)
    {
      // Raw-String (1 Pixel Length).
      if  (bufferLineLen < lineBuffSize)
      {
        lineBuff[bufferLineLen] = convTable4BitTo8Bit[rec.raw1Pixel.pixelValue];
        ++bufferLineLen;
      }
      else
      {
        cerr << "SipperBuff4Bit::GetNextScanLine   ***ERROR***  Exceeding 'bufferLineLen';  ScanLine[" << curRow << "]." << endl;
      }
    }

    else if  (opCode == 12)
    {
      // Raw-String (Even length 2 thru 32).
      kkuint16  numRawRecs = rec.raw32Pixels.len + 1;   // We add 1 to 'numRawRecs' because '1' was subtracted out when written to Scanner File.
      kkuint16  numRawPixels = numRawRecs * 2;
      kkuint16  newBufferLineLen = bufferLineLen + numRawPixels;
      if  (newBufferLineLen > lineBuffSize)
      {
        cerr << "SipperBuff4Bit::GetNextScanLine   ***ERROR***  Exceeding 'bufferLineLen';  ScanLine[" << curRow << "]." << endl;
      }
      ProcessRawPixelRecs (numRawRecs, lineBuff, lineBuffSize, bufferLineLen);
    }

    else if  (opCode == 13)
    {
      // Raw-String (Odd Length 1 thru 513).
      recsRead = (kkuint32)fread (&rec2, sizeof (rec2), 1, inFile);
      if  (recsRead < 1)
      {
        eol = true;
        eof = true;
      }
      else
      {
        byteOffset += sizeof (rec2);
        kkuint16  numRawRecs = 1 + 16 * (kkuint16)(rec.raw513Pixels1.lenHigh) + (kkuint16)(rec2.raw513Pixels2.lenLow);
        kkuint16  numRawPixels = 1 + 2 * numRawRecs;
        kkuint16  newBufferLineLen = bufferLineLen + numRawPixels;
        if  (newBufferLineLen > lineBuffSize)
        {
          cerr << "SipperBuff4Bit::GetNextScanLine   ***ERROR***  Exceeding 'bufferLineLen';  ScanLine[" << curRow << "]." << endl;
        }

        if  (bufferLineLen < lineBuffSize)  
        {
          lineBuff[bufferLineLen] = convTable4BitTo8Bit[rec2.raw513Pixels2.pix0];  
          ++bufferLineLen;
        }
      
        ProcessRawPixelRecs (numRawRecs, lineBuff, lineBuffSize, bufferLineLen);
      }
    }
  }  while  (!eol);

  lineLen = bufferLineLen;

  ++curRow;

  return;
}  /* GetNextScanLine */



void  SipperBuff4Bit::GetNextLine (uchar*     lineBuff,
                                   kkuint32   lineBuffSize,
                                   kkuint32&  lineSize,
                                   kkuint32   colCount[],
                                   kkuint32&  pixelsInRow,
                                   bool&      flow
                                  )
{
  GetNextScanLine (lineBuff, lineBuffSize, lineSize);

  for  (kkuint32  x = 0;  x < lineSize;  ++x)
  {
    if  (lineBuff[x] > 30)
    {
      ++pixelsInRow;
      ++colCount[x];
    }
  }

  return;
}  /* GetNextLine */
