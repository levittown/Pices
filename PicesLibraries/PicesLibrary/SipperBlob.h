#ifndef  _SIPPERBLOB_
#define  _SIPPERBLOB_
/**
 @class MLL::SipperBlob
 @brief  Used by SipperImageExtrtaction to manage a single blob.
 @author  Kurt Kramer
 @details
 @code
 ***************************************************************************
 **                              SipperBlob                                *
 **                                                                        *
 **  Used by SipperImageExtraction to keep track of an individule conected *
 **  componets in a sipper frame.  We call these connected componets blobs.*
 **  The fuields rowTop, rowBot, colLeft, colRight indicate a tight bound- *
 **  box arround the blob.  SipperImageExtraction will use                 *
 **                                                                        *
 ***************************************************************************
 @endcode
 @see SipperImageExtraction
*/

#include  "KKBaseTypes.h"
#include  "KKQueue.h"
#include  "Raster.h"
#include  "KKStr.h"


namespace  MLL  {

#ifndef  _IMAGEFEATURES_
class  ImageFeatures;
class  ImageFeaturesList;

typedef  ImageFeatures*      ImageFeaturesPtr;
typedef  ImageFeaturesList*  ImageFeaturesListPtr;
#endif



#ifndef  _MLCLASS_
class  MLClass;
class  MLClassList;

typedef  MLClass*      MLClassPtr;
typedef  MLClassList*  MLClassListPtr;
#endif



class  SipperBlob
{

public:
  typedef  SipperBlob*            SipperBlobPtr;
  typedef  vector<SipperBlobPtr>  VectorSipperBlob;

  typedef  map<kkint32, SipperBlobPtr>  NeighborsList;


  SipperBlob ();

  ~SipperBlob ();

  kkint32  Id ()  {return  id;}

  kkuint32  ColLeft ()     {return colLeft;}
  kkuint32  ColRight ()    {return colRight;}


  void  AddNeighbor (SipperBlobPtr  _neighbor);

  void  DialateBlob (uchar** frame,
                     kkint32**   blobIds,
                     kkint32   size
                    );

  RasterPtr  GetRaster (uchar** frame,
                        kkint32**   blobIds,     
                        kkuint32  firstFrameRowScanLine
                       )  const;

  bool  Explored ()  const  {return explored;}

  void  Explored (bool _explored)  {explored = _explored;}

  kkuint32  Height ()      {return  abs ((1 + (kkint32)rowBot - (kkint32)rowTop));}

  NeighborsList&  Neighbors ()  {return  neighbors;}

  kkuint32  RowBot ()      {return rowBot;}
  kkuint32  RowTop ()      {return rowTop;}

  kkuint32  Width ()       {return  abs ((1 + (kkint32)colLeft - (kkint32)colRight));}

  kkuint32  PixelCount ()  {return pixelCount;}

  ImageFeaturesPtr  Save (uchar**        frame,
                          kkint32**        blobIds,     
                          KKStr          fileName,
                          bool           createImageFeaturesObject,
                          MLClassPtr  mlClass,
                          kkuint32       firstFrameRowScanLine,
                          bool           save,        /**< Set to true to save file false if only calculating features.      */
                          bool           colorize     /**< Set to true if you wish to save the 7 grayscale values as colors. */
                         );

  static void   MaxFrameHeight (kkuint32  _maxFrameHeight)  {maxFrameHeight = _maxFrameHeight;}
  static void   MaxFrameWidth  (kkuint32  _maxFrameWidth)   {maxFrameWidth  = _maxFrameWidth;}



  kkint32 id;

  bool  explored;

  kkuint32  rowBot;
  kkuint32  rowTop;
  kkuint32  colLeft;
  kkuint32  colRight;

  kkint32        neighborIdLastAdded;
  NeighborsList  neighbors;

  static  
    kkuint32  maxFrameHeight;

  static  
    kkuint32  maxFrameWidth;

    kkuint32  pixelCount;

};  /* SipperBlob */


typedef  SipperBlob::SipperBlobPtr     SipperBlobPtr;
typedef  SipperBlob::VectorSipperBlob  VectorSipperBlob;

}  /* namespace  MLL  */


#endif
