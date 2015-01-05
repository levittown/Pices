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

    float    DepthBinSize   () const  {return  depthBinSize;}
    float    InitialValue   () const  {return  initialValue;}
    float    GrowthRate     () const  {return  growthRate;}
    float    EndValue       () const  {return  endValue;}
    kkint32  NumSizeBuckets () const  {return  numSizeBuckets;}
    kkint32  NumDepthBins   () const  {return  (kkint32)depths.size ();}

    const VectorFloat&  SizeStartValues ()  const  {return sizeStartValues;}
    const VectorFloat&  sizeEndValues   ()  const  {return sizeStartValues;}




    void  DefineRow (float    _depth,
                     kkint32  _imageCount,
                     kkint32  _totalPixelCount
                    );

    void  AddData  (kkint32   _sizeBucketIdx,
                    kkint32   _count
                   );
                    

  private:
    float                depthBinSize;
    float                initialValue;
    float                growthRate;
    float                endValue;
    kkint32              numSizeBuckets;
    VectorFloat          sizeStartValues;
    VectorFloat          sizeEndValues;
    vector<VectorInt32>  sizeDistribution;   /**<  Each row represents a depth range,  and each column a Size Range */
    VectorFloat          depths;
    VectorInt32          imageCounts;        /**<  Each entr represents number images in the coresponding depth range. */
    VectorInt32          pixelCounts;        /**<  Total number of piels in coresponding depth range. */

  };  /* ImageSizeDistribution */

  typedef  ImageSizeDistribution*  ImageSizeDistributionPtr;





}  /* namespace MML */

#endif
