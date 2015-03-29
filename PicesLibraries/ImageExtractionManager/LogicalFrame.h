#if  !defined(_IEM_LOGICALFRAME_)
#define  _IEM_LOGICALFRAME_

#include <queue>

#include "GoalKeeper.h"

#include "PreDefinedClasses.h"
#include "ExtractionParms.h"


namespace  ImageExtractionManager
{
  /**
   *@brief   The Image extraction procedure will contain several threads perform various functions.
   *@details  This will be the base class for each one of the different types of processing that 
   * Sipper Image processing will be done.  Some of threads will Extract logical frames from Sipper
   * files,  
   */
  class  LogicalFrame
  {
  public:
    LogicalFrame (const ExtractionParms&  _parms,
                  kkuint32                _frameHeightMax,
                  kkuint32                _frameWidth,
                  const bool&             _cancelFlag
                 );

    ~LogicalFrame ();

    kkint32 MemoryConsumedEstimated ()  const;

    kkuint32  FrameHeight   ()  const  {return frameHeight;}
    kkuint32  FrameNum      ()  const  {return frameNum;}
    kkuint32  ImagesInFrame ()  const  {return imagesInFrame;}

    void  PopulateFrame (kkuint32 _frameNum,
                         kkuint32 _frameHeight,
                         uchar*   _frameArea,
                         kkuint32 _frameSipperRow,
                         kkuint64*  _frameRowByteOffset,
                         kkuint32*  _pixelsPerRow
                        );

    ExtractedImageListPtr  ProcessFrame ();


  private:
    void  AllocateFrame ();
    void  DeAllocateFrame ();
    void  PerformDialation ();
    void  PerformErosion ();
    void  PerformOpening ();
    void  PerformClosing ();
    void  PerformMorphOperations ();
    void  InitailzieBlobIds ();
    void  LocateBlobsUsingConnectedDistanceOf3 ();
    void  LocateBlobsUsingConnectedDistanceNotOf3 ();
    
    VectorSipperImagePtr  BuildListOfSipperImages (kkuint32&  imagesInFrame);
    
    int  GetMaxBlobIdInUpperLeft (kkuint32 row,
                                  kkuint32 col
                                 );

    int  GetMaxBlobIdInUpperRight (kkuint32 row,
                                   kkuint32 col
                                  );

    LogicalFrameBlobPtr  NewBlob (kkuint32  rowTop,
                                  kkuint32  colLeft
                                 );

    bool  ForegroundPixel (uchar  pixValue);

    void  CountAndTotal (KKB::uchar    c,
                         KKB::ushort&  count,
                         KKB::ushort&  total
                        );

    static
    int  CountPixs (int x1,  int x2,  int x3,  
                    int x4,  int x5,  int x6,  
                    int x7,  int x8,  int x9
                   );

    static  inline
    int  Max (int x1,  int x2,  int x3, 
              int x4,  int x5,  int x6
             );

    static  inline  
    int  Max (int x1,  int x2,  int x3,  
              int x4,  int x5,  int x6,  
              int x7,  int x8,  int x9
             );


    kkuint32        frameHeight;            /**< Number of rows in current frame.                         */
    kkuint32        frameHeightMax;         /**< Number of scan lines that have been actually allocated.  */
    uchar**         frame;                  /**< Row pointers into 'frameArea'.                           */
    kkuint32        frameWidth;             /**< Number of pixels per row in frame.                      */
    uchar*          frameArea;              /**< Pointer to block of memory that will contain frame       */
    kkuint32        frameSipperRow;         /**< Row within the Sipper File That Frame starts at.         */
    kkuint64*         frameRowByteOffset;
    kkuint32        frameTotalPixels;       /**< Total number of pixels that frame could contain.         */
    kkuint32*         pixelsPerRow;           /**< Number of foreground pixels in each row.                  */

    uchar**         origFrame;              /**< 'origFrame' and 'origFrameArea' are only used when       */
    uchar*          origFrameArea;          /**<  morphological operations are performed.                 */
   
    uchar**         workFrame;              /**< 'workFrame' & 'workFrameArea' are used to perform        */
    uchar*          workFrameArea;          /**< morphological operations with.                           */

    kkuint32        frameNum;               /**< Sequential number assigned to each frame.                */
    kkuint32        imagesInFrame;          /**< Number of discreet images located by the connected component analysis. */

    // Structures to support Blob Processing.
    LogicalFrameBlobPtr*  blobs;
    kkint32*          blobIdsArea;            /**< Continuous chunk of memory that was allocated to hold 'blobIds' in.   */
    kkint32**         blobIds;                /**< Two dimensional array that will work with frame.  For each pixel that is 
                                             *   determined to belong to an image, the ID of that image that it is part of
                                             *   will be kept here.
                                             */
    kkuint32        connectedPixelDist;     /**< Distance that two foreground pixels have to be to be considered part of same connected image. */
    kkuint32        maxBlobsPerFrame;       /**< Maximum number of blobs that can be tracked; 'blobs' will be allocated to this size.          */
    kkuint32        numOfBlobsInFrame;      /**< Number of blobs located in current frame so far.                                              */

    const
    ExtractionParms&  parms;

    uchar           backgroundPixelTH;
    kkuint32        cropLeft;
    kkuint32        cropRight;
                            
    volatile
    const bool&     cancelFlag;
  };  /* LogicalFrame */

  typedef  LogicalFrame*  LogicalFramePtr;

}  /* ImageExtractionManager */


#endif
