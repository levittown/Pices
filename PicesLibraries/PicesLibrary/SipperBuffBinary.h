#ifndef  _SIPPERBUFFBINARY_
#define  _SIPPERBUFFBINARY_

#include  "RunLog.h"
using namespace KKB;


#include  "SipperBuff.h"

namespace  MLL
{
  #define  SipperBuffSize 10240

  #ifdef WIN32

  typedef struct 
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
  }  SipperBinaryRec;


  #else
  //  Since WIN32 not defined assuming UNIX environment,

  typedef struct 
  {
    uchar cameraNum  : 1;
    uchar eol        : 1;  // 1 = End of Line Encountered, 
                           // 0 = End of Line not Encountered

    uchar raw        : 1;  // 1 = Data is Raw 
                           // 0 = Compressed Run-Length

    uchar flow       : 1;  // 0 = Flow Meter in Position 0
                           // 1 = Flow Meter in Position 1

    uchar pix0       : 1;
    uchar pix1       : 1;
    uchar pix2       : 1;

    uchar pix3       : 1;
    uchar pix4       : 1;
    uchar pix5       : 1;

    uchar pix6       : 1;
    uchar pix7       : 1;
    uchar pix8       : 1;

    uchar pix9       : 1;
    uchar pix10      : 1;
    uchar pix11      : 1;
  }  SipperBinaryRec;

  #endif


  typedef  SipperBinaryRec*  SipperBinaryRecPtr;



  class  SipperBuffBinary:  public SipperBuff
  {
  public:
    SipperBuffBinary (InstrumentDataManagerPtr  _instrumentDataManager,
                      RunLog&                   _log
                     );

    SipperBuffBinary (KKStr                    _fileName,
                      uchar                     _cameraNum,
                      InstrumentDataManagerPtr  _instrumentDataManager,
                      RunLog&                   _log
                     );
    
    virtual
    ~SipperBuffBinary ();


    uchar    SelCameraNum () {return  selCameraNum;}

    void  SelCameraNum (uchar _selCameraNum)  {selCameraNum = _selCameraNum;}


    virtual
    void  GetNextLine  (uchar*     lineBuff,
                        kkuint32   lineBuffSize,
                        kkuint32&  lineSize,
                        kkuint32   colCount[],
                        kkuint32&  pixelsInRow,
                        bool&      flow
                       );

  private:

    inline
    void  GetNextSipperRec (kkint32&   spaceLeft,
                            uchar&     cameraNum,
                            bool&      raw,
                            bool&      eol,
                            bool&      flow,
                            uchar&     pix0,
                            uchar&     pix1,
                            uchar&     pix2,
                            uchar&     pix3,
                            uchar&     pix4,
                            uchar&     pix5,
                            uchar&     pix6,
                            uchar&     pix7,
                            uchar&     pix8,
                            uchar&     pix9,
                            uchar&     pix10,
                            uchar&     pix11,
                            kkint32&   numOfBlanks,
                            bool&      moreRecs
                           );


    uchar  selCameraNum;
  };

  typedef  SipperBuffBinary*  SipperBuffBinaryPtr;

}  /* MLL */

#endif

