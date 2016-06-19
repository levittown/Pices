#if  !defined(_IEM_EXTRACTEDIMAGE_)
#define  _IEM_EXTRACTEDIMAGE_


#include <deque>
using namespace  std;

#include "KKQueue.h"
#include "GoalKeeper.h"
#include "RasterSipper.h"
using namespace  KKB;


#include "MLClass.h"
#include "ImageFeatures.h"
using namespace  MLL;


namespace  ImageExtractionManager
{
  /**
   *@brief   Represents particle that was extracted by 'LogicalFrame' with its related data.
   */
  class  ExtractedImage
  {
  public:
    ExtractedImage (RasterSipperPtr _image,
                    kkuint64        _byteOffsetFirstScanRow,
                    kkuint32        _sipperFileScanRow,
                    kkuint32        _sipperFileScanCol,
                    kkuint32        _pixelCount,
                    float           _imageArea,
                    float           _depth,
                    MLClassPtr      _predClass1,
                    float           _predClass1Prob,
                    MLClassPtr      _predClass2,
                    float           _predClass2Prob
                   );

    ~ExtractedImage ();

    kkint32 MemoryConsumedEstimated ()  const;

    kkuint64            ByteOffsetFirstScanRow () const  {return byteOffsetFirstScanRow;}
    float               Depth                  () const  {return depth;}
    ImageFeaturesPtr    FeatureVector          () const  {return featureVector;}
    RasterSipperPtr     Image                  () const  {return image;}
    float               ImageArea              () const  {return imageArea;}
    kkuint32            PixelCount             () const  {return pixelCount;}
    MLClassPtr          PredClass1             () const  {return predClass1;}
    float               PredClass1Prob         () const  {return predClass1Prob;}
    MLClassPtr          PredClass2             () const  {return predClass2;}
    float               PredClass2Prob         () const  {return predClass2Prob;}
    kkuint32            SipperFileScanRow      () const  {return sipperFileScanRow;}
    kkuint32            SipperFileScanCol      () const  {return sipperFileScanCol;}

    void  FeatureVector (ImageFeaturesPtr  _featureVector);
    void  Depth         (float             _depth)           {depth     = _depth;}

    void  Prediction (MLClassPtr  _predClass1,
                      float       _predClass1Prob,
                      MLClassPtr  _predClass2,
                      float       _predClass2Prob
                     );
    

    /**@brief  Turns over ownership of FeatureVector to the caller.  */
    ImageFeaturesPtr  FeatureVectorGiveOwnership ();
    RasterSipperPtr   ImageGiveOwnership ();


  private:
    kkuint64          byteOffsetFirstScanRow;
    float             depth;
    ImageFeaturesPtr  featureVector;
    RasterSipperPtr   image;
    float             imageArea;
    kkuint32          pixelCount;
    MLClassPtr        predClass1;
    float             predClass1Prob;
    MLClassPtr        predClass2;
    float             predClass2Prob;
    kkuint32          sipperFileScanRow;
    kkuint32          sipperFileScanCol;
  };  /* ExtractedImage */

  typedef  ExtractedImage*  ExtractedImagePtr;

  typedef  vector<ExtractedImagePtr>  VectorExtractedImage;

  typedef  VectorExtractedImage*  VectorExtractedImagePtr;


  typedef  KKQueue<ExtractedImage>  ExtractedImageList;
  typedef  ExtractedImageList*  ExtractedImageListPtr;


  class  ExtractedImageQueue: public  deque<ExtractedImagePtr>
  {
  public:
    ExtractedImageQueue ();

    ~ExtractedImageQueue ();

    kkint32  MemoryConsumedEstimated ()  const;

    void  AddExtractedImage (ExtractedImagePtr  extractedImage);

    void  AddExtractedImages (VectorExtractedImage&  extractedImages);

    kkuint32  ImagesOnQueue ();

    ExtractedImagePtr  GetNextExtractedImage ();

  private:
    GoalKeeperPtr   goalie;
    kkuint32        maxQueueSize;
  };  /* ExtractedImageQueue */


  typedef  ExtractedImageQueue*  ExtractedImageQueuePtr;

}  /* ImageExtractionManager */


#endif

