#if  !defined(_IEM_LOGICALFRAMEBLOB_)
#define  _IEM_LOGICALFRAMEBLOB_
/**
 @class MLL::LogicalFrameBlob
 @brief  Used by the ImageExtractionManager to manage a single blob.
 @author  Kurt Kramer
 @details
 @code
 *****************************************************************************
 **                           LogicalFrameBlob                               *
 **                                                                          *
 **  Used by ImageExtractionManager to keep track of an individual connected *
 **  components in a sipper frame. We call these connected components blobs. *
 **  The fields rowTop, rowBot, colLeft, colRight indicate a tight bounding  *
 **  box around the blob.                                                    *
 *****************************************************************************
 @endcode
*/

#include  "KKBaseTypes.h"
#include  "KKQueue.h"
#include  "RasterSipper.h"
#include  "KKStr.h"


namespace  ImageExtractionManager  
{

  class  LogicalFrameBlob
  {

  public:
    typedef  LogicalFrameBlob*               LogicalFrameBlobPtr;
    typedef  vector<LogicalFrameBlobPtr>     VectorLogicalFrameBlob;

    typedef  map<kkint32, LogicalFrameBlobPtr>  NeighborsList;


    LogicalFrameBlob (kkuint32  _maxFrameHeight,
                      kkuint32  _maxFrameWidth
                     );

    ~LogicalFrameBlob ();

    kkint32  Id ()  {return  id;}

    kkuint32  ColLeft ()     {return colLeft;}
    kkuint32  ColRight ()    {return colRight;}


    void  AddNeighbor (LogicalFrameBlobPtr  _neighbor);

    void  DialateBlob (uchar**  frame,
                       kkint32**  blobIds,
                       kkint32  size
                      );

    bool  Explored ()  const  {return explored;}

    void  Explored (bool _explored)  {explored = _explored;}

    kkuint32  Height ()      {return  abs ((1 + (kkint32)rowBot - (kkint32)rowTop));}

    NeighborsList&  Neighbors ()  {return  neighbors;}

    kkuint32  RowBot ()      {return rowBot;}
    kkuint32  RowTop ()      {return rowTop;}

    kkuint32  Width ()       {return  abs ((1 + (kkint32)colLeft - (kkint32)colRight));}

    kkuint32  PixelCount ()  {return pixelCount;}

    kkint32 id;

    bool  explored;

    kkuint32  rowBot;
    kkuint32  rowTop;
    kkuint32  colLeft;
    kkuint32  colRight;

    kkint32        neighborIdLastAdded;
    NeighborsList  neighbors;

    kkuint32  maxFrameHeight;
    kkuint32  maxFrameWidth;

    kkuint32  pixelCount;

  };  /* LogicalFrameBlob */


  typedef  LogicalFrameBlob::LogicalFrameBlobPtr     LogicalFrameBlobPtr;
  typedef  LogicalFrameBlob::VectorLogicalFrameBlob  VectorLogicalFrameBlob;

}  /* namespace  ImageExtractionManager  */


#endif
