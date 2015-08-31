#ifndef  _SIPPERBUFF_
#define  _SIPPERBUFF_

#include  "RunLog.h"
#include  "KKStr.h"
using namespace KKB;

#include  "SipperHeaderRec.h"


namespace MLL
{
#define  MAXLINELEN  4096
#define  SipperBuffSize 10240

#ifndef  _INSTRUMENTDATAMANAGER_
  class InstrumentDataManager;
  typedef  InstrumentDataManager*  InstrumentDataManagerPtr;
#endif


  // We need to use a define for SippetrRec size because recs that
  // contain bit fields do not report correct size when using
  // sizeof 
  #define  SIPPERRECSIZE 2


  typedef  enum  {sfBinary,
                  sf3Bit,
                  sfSipper3,
                  sfSipper3Rev,
                  sfSipper4Bit,
                  sfUnKnown
                 }  SipperFileFormat;
  


  /**
   *@class SipperBuff  
   *@brief Base class to be used for all the different Sipper File Formats.
   *@details Each derived sub-class needs to implement 'GetNextSipperRec' and 'FileFormatGood'.
   *@see  InstrumentDataManager
   */
  class  SipperBuff
  {
  public:
    typedef  SipperBuff*  SipperBuffPtr;
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;

    SipperBuff (InstrumentDataManagerPtr  _instrumentDataManager,
                RunLog&                   _log);
  
    SipperBuff (const KKStr&              _fileName,
                InstrumentDataManagerPtr  _instrumentDataManager,
                RunLog&                   _log
               );
    
    virtual  ~SipperBuff ();
  
    const KKStr&  FileName () const {return  fileName;}
  
  
  
    void    InvalidLine  (bool _invalidLine)  {invalidLine = _invalidLine;}
    void    Open (const KKStr&  _fileName);
  

    kkuint32  BytesDropped     ()  const {return  bytesDropped;}
    kkuint64  ByteOffset       ()  const {return  byteOffset;}
  
    kkuint32  CurRow           ()  const {return  curRow;} 
    kkuint64  CurRowByteOffset ()  const {return  curRowByteOffset;}
    bool      Eof              ()  const {return  eof;}
    bool      InvalidLine      ()  const {return  invalidLine;}
    bool      Opened           ()  const {return  opened;}
    kkuint32  RecCount         ()  const {return  recCount;}
    kkint32   ScanRate         ()  const {return  25000;}
  
    virtual
    kkint32 LineWidth        ()  const {return (kkint32)MAXLINELEN;}  /**< Returns the width in pixels of the associated scanner file. */

    virtual 
    void  GetNextLine (uchar*     lineBuff,
                       kkuint32   lineBuffSize,
                       kkuint32&  lineSize,
                       kkuint32   colCount[],
                       kkuint32&  pixelsInRow,
                       bool&      flow
                      ) = 0;
  
    virtual
    void  SkipToScanLine (kkuint32  scanLine);
  
    virtual
    void  SkipToScanLine (kkuint32  _scanLine,
                          kkuint64  _byteOffset
                         );
  
  
    static
    SipperFileFormat  GuessFormatOfFile (const KKStr&  _fileName,
                                         kkint32       _cameraNum,
                                         RunLog&       _log
                                        );
  
    static
    SipperBuffPtr  CreateSipperBuff (SipperFileFormat          _format,
                                     KKStr                     _fileName,
                                     kkint32                   _cameraNum,
                                     InstrumentDataManagerPtr  _instrumentDataManager,
                                     RunLog&                   _log
                                    );
  
    static
    SipperBuffPtr  CreateSipperBuff (KKStr                     _fileName,
                                     kkint32                   _cameraNum,
                                     InstrumentDataManagerPtr  _instrumentDataManager,
                                     RunLog&                   _log
                                    );
  
    void  DetermineCropSettings  (kkuint32&  cropLeft,
                                  kkuint32&  cropRght
                                 );


    void  Reset ();
  
    void  ResetByteOffset (kkuint32  _byteOffset);
  
    void  ReportInstrumentData (uchar  sensorId,
                                bool   text,
                                uchar  sensorData
                               );
  
  
    void  SkipOneByte ();
  
    
  protected:
  
    void  AssignSerialPorts (const SipperHeaderRecPtr  sipperHeaderRec);
  
    static
    SipperFileFormat  GuessFormatOfNextLine (FILE*    in,
                                             kkint32  _cameraNum,
                                             RunLog&  _log
                                            );
  
    kkuint32                  bytesDropped;
    kkuint64                  byteOffset;
    kkuint32                  curRow;
    kkuint64                  curRowByteOffset;
    bool                      eof;
    KKStr                     fileName;
    bool                      iOwnInstrumentDataManager;
    FILE*                     inFile;
    InstrumentDataManagerPtr  instrumentDataManager;
    bool                      invalidLine;   /**< Set to true if invalid data is detected on 
                                              * a scan line. Such as exceeding line length.
                                              */
    RunLog&                   log;
    bool                      opened;
    kkuint32                  recCount;

    struct  SipperRec;
    static  void  DisplaySipperRec (SipperBuff::SipperRec&  sr);
  };  /* SipperBuff */

  typedef  SipperBuff::SipperBuffPtr  SipperBuffPtr;

  KKStr  SipperFileFormatToStr (SipperFileFormat  fileFormat);
  
  SipperFileFormat  SipperFileFormatFromStr (KKStr  fileFormatStr);
  
  KKStr  SipperFileAvailableOptions ();
}  /* MLL */


#endif
