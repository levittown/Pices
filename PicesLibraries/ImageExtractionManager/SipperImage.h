#if  !defined(_IEM_SIPPERIMAGE_)
#define  _IEM_SIPPERIMAGE_

#include "RasterSipper.h"
using namespace  SipperHardware;

#include "LogicalFrameBlob.h"


namespace  ImageExtractionManager
{
  class  SipperImage
  {
  public:
    typedef  SipperImage*  SipperImagePtr;

    SipperImage ();
    SipperImage (LogicalFrameBlobPtr  blob);

    ~SipperImage ();

    void  AddBlob (LogicalFrameBlobPtr  _blob);
    void  Explore (LogicalFrameBlobPtr  _blob);

    kkuint64  ByteOffset () const  {return byteOffset;}
    kkuint32  ColLeft    () const  {return colLeft;}
    kkuint32  ColRight   () const  {return colRight;}
    kkuint32  PixelCount () const  {return pixelCount;}
    kkuint32  RowBot     () const  {return rowBot;}
    kkuint32  RowTop     () const  {return rowTop;}

    kkuint32  Width      () const  {return (1 + colRight - colLeft);}
    kkuint32  Height     () const  {return (1 + rowBot   - rowTop);}


    void  Dialate (uchar** frame,
                   int**   blobIds,
                   int     size
                  );

    RasterSipperPtr  GetRaster (uchar**  frame,
                                int**    blobIds,     
                                kkuint32 firstFrameRowScanLine,
                                kkuint64*  frameRowByteOffsets
                               );



  private:
    vector<LogicalFrameBlobPtr>   blobList;
    kkuint64                      byteOffset;  /**< Sipper File ByteOffset of scan-line that contains the 1st row in this image.  */
    kkuint32                      colLeft;
    kkuint32                      colRight;
    kkuint32                      pixelCount;
    kkuint32                      rowBot;
    kkuint32                      rowTop;

  };

  typedef  SipperImage::SipperImagePtr  SipperImagePtr;
  typedef  vector<SipperImage*>         VectorSipperImage;
  typedef  VectorSipperImage*           VectorSipperImagePtr;
}  /* ImageExtractionManager */

#endif
