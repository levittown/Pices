#if  !defined(_ImageSizeDistribution_)
#define  _ImageSizeDistribution_
/**
 *@class  MLL::ImageSizeDistribution
 *@brief Used for retirning results of call to "ImagesSizeSizeHistogramByDepth".
 */

#include "BasicTypes.h"
#include "RunLog.h"
#include "Str.h"


namespace MLL
{
  class  ImageSizeDistributionRow
  {
  public:
    ImageSizeDistributionRow (float     _depth,
                              kkuint32  _numSizeBuckets,
                              kkuint32  _imageCount,
                              kkuint32  _totalPixels
                             );


    float                Depth        ()  const  {return depth;}
    kkuint32             ImageCount   ()  const  {return imageCount;}
    kkuint32             TotalPixels  ()  const  {return totalPixels;}
    const VectorUint32&  Distribution ()  const  {return distribution;}

    void  ImageCount   (kkuint32  _imageCount)  {imageCount  = _imageCount;}
    void  TotalPixels  (kkuint32  _totalPixels) {totalPixels = _totalPixels;}

    void  AddData (kkuint32  _sizeIdx,
                   kkuint32  _count
                  );

    void  AddImageCountPixelCount (kkuint32  _imageCount,
                                   kkuint32  _pixelCount
                                  );

    void  AddIn (const ImageSizeDistributionRow&  right);

  private:
    float         depth;
    VectorUint32  distribution;
    kkuint32      imageCount;
    kkuint32      totalPixels;
  };  /* ImageSizeDistributionRow */

  typedef  ImageSizeDistributionRow*  ImageSizeDistributionRowPtr;




  class  ImageSizeDistribution
  {
  public:
    ImageSizeDistribution (float               _depthBinSize,
                           float               _initialValue,
                           float               _growtRate,
                           float               _endValue,
                           const VectorFloat&  _sizeStartValues,
                           const VectorFloat&  _sizeEndValues,
                           RunLog&             _runLog
                          );

    ~ImageSizeDistribution ();

    float     DepthBinSize   () const  {return  depthBinSize;}
    float     InitialValue   () const  {return  initialValue;}
    float     GrowthRate     () const  {return  growthRate;}
    float     EndValue       () const  {return  endValue;}
    kkuint32  NumSizeBuckets () const  {return  numSizeBuckets;}
    kkuint32  NumDepthBins   () const  {return  depthDistributions.size ();}

    const VectorFloat&  SizeStartValues ()  const  {return sizeStartValues;}
    const VectorFloat&  SizeEndValues   ()  const  {return sizeStartValues;}



    ImageSizeDistributionRow&   AllDepths ()  const  {return *allDepths;}


    void  DefineRow (float     _depth,
                     kkuint32  _imageCount,
                     kkuint32  _totalPixelCount
                    );

    void  AddData  (float     _depth,
                    kkuint32  _sizeBucketIdx,
                    kkuint32  _count
                   );

    void  AddIn (const ImageSizeDistribution&  right,
                 RunLog&                       log
                );


  private:
    void  PopulateDistributions (kkuint32  depthIdx);

    float                depthBinSize;
    float                initialValue;
    float                growthRate;
    float                endValue;
    kkint32              numSizeBuckets;

    vector<ImageSizeDistributionRowPtr>  depthDistributions;
    ImageSizeDistributionRowPtr          allDepths;


    VectorFloat          sizeStartValues;
    VectorFloat          sizeEndValues;
  };  /* ImageSizeDistribution */

  typedef  ImageSizeDistribution*  ImageSizeDistributionPtr;





}  /* namespace MML */

#endif
