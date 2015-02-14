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
    ImageSizeDistributionRow (const ImageSizeDistributionRow&  row);

    ImageSizeDistributionRow (float     _depth,
                              kkuint32  _numSizeBuckets,
                              kkuint32  _imageCount,
                              kkuint32  _totalPixels,
                              kkuint32  _totalFilledArea,
                              float     _volumneSampled
                             );


    float                Depth           ()  const  {return depth;}
    kkuint32             ImageCount      ()  const  {return imageCount;}
    kkuint32             TotalPixels     ()  const  {return totalPixels;}
    kkuint32             TotalFilledArea ()  const  {return totalFilledArea;}
    float                VolumneSampled  ()  const  {return volumneSampled;}
    const VectorUint32&  Distribution    ()  const  {return distribution;}

    
    void  ImageCount      (kkuint32  _imageCount)      {imageCount      = _imageCount;}
    void  TotalPixels     (kkuint32  _totalPixels)     {totalPixels     = _totalPixels;}
    void  TotalFilledArea (kkuint32  _totalFilledArea) {totalFilledArea = _totalFilledArea;}
    void  VolumneSampled  (float     _volumneSampled)  {volumneSampled  = _volumneSampled;}

    void  AddData (kkuint32  _sizeIdx,
                   kkuint32  _count
                  );

    void  AddImageCountPixelCount (kkuint32  _imageCount,
                                   kkuint32  _pixelCount,
                                   kkuint32  _filledArea,
                                   float     _volumneSampled
                                  );

    void  AddIn (const ImageSizeDistributionRow&  right);

  private:
    float         depth;
    VectorUint32  distribution;
    kkuint32      imageCount;
    kkuint32      totalPixels;
    kkuint32      totalFilledArea;
    float         volumneSampled;
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

    ImageSizeDistributionRowPtr  GetDepthBin (kkuint32  depthBinIdx);

    const VectorFloat&  SizeStartValues ()  const  {return sizeStartValues;}
    const VectorFloat&  SizeEndValues   ()  const  {return sizeEndValues;}

    ImageSizeDistributionRow&   AllDepths ()  const  {return *allDepths;}


    void  DefineRow (float     _depth,
                     kkuint32  _imageCount,
                     kkuint32  _totalPixelCount,
                     kkuint32  _totalFilledArea,
                     float     _volumneSampled
                    );

    void  AddData  (float     _depth,
                    kkuint32  _sizeBucketIdx,
                    kkuint32  _count
                   );

    void  AddIn (const ImageSizeDistribution&  right,
                 RunLog&                       log
                );

    VectorFloat  DepthProfileForSizeBin (kkuint32  _sizeBucketIdx);

    void  GetSizeBucketStats (kkuint32   _sizeBucketIdx,
                              kkuint32&  _count,
                              float&     _sizeStart,
                              float&     _sizeEnd
                             );

    kkint32  IdentifySizeBucket (float  size);

    VectorFloat   IntegratedDensityDistribution ()  const;

    VectorFloat  VolumeSampledByDepthBucket ();


  private:
    void  PopulateDistributions (kkuint32  depthIdx);

    float                depthBinSize;
    float                initialValue;
    float                growthRate;
    float                endValue;
    kkuint32             numSizeBuckets;

    vector<ImageSizeDistributionRowPtr>  depthDistributions;
    ImageSizeDistributionRowPtr          allDepths;


    VectorFloat          sizeStartValues;
    VectorFloat          sizeEndValues;
  };  /* ImageSizeDistribution */

  typedef  ImageSizeDistribution*  ImageSizeDistributionPtr;





}  /* namespace MML */

#endif
