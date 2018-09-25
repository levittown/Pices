/* RasterSipper.h -- A specialized RasterSipper object for Sipper Images.
 * Copyright (C) 1994-2011 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */
#ifndef _RASTERSIPPER_
#define _RASTERSIPPER_

#include <map>

#include "KKQueue.h"
#include "Raster.h"
#include "RunLog.h"
using namespace  KKB;


namespace MLL
{
  /**
   *@file  RasterSipper.h
   *@author  Kurt Kramer
   *@details
   *@code
   ***************************************************************************
   **                             RasterSipper                               *
   **                                                                        *
   **  A specialized RasterSipper object for managing Sipper Images.               *
   ***************************************************************************
   *@endcode
   *@sa RasterSipper
   */



  class  RasterSipperList;
  typedef  RasterSipperList*  RasterSipperListPtr;


  /**
   *@class RasterSipper 
   *@brief  A class that is used by to represent a single Sipper Image in memory.  
   *@details  A specialized RasterSipper object for managing Sipper Images.  
   *@see Blob
   *@see ContourFollower
   *@see ConvexHull
   *@see PixelValue
   *@see Point
   *@see PointList
   *@see RasterSipperList()
   *@see Sobel
   */
  class  RasterSipper: public Raster
  {
  public:
    typedef  RasterSipper*  RasterSipperPtr;
    typedef  KKB::uchar  uchar;

    RasterSipper ();

    RasterSipper (const RasterSipper&  _raster);  /**< @brief Copy Constructor */

    RasterSipper (const Raster&  _raster);        /**< @brief Copy Constructor */


    /**
     *@brief Constructs a blank GrayScale image with given dimensions; all pixels will be initialized to 0.  
     *@details  When working with Gray-scale images pixel value of '0' = Background and '255'= foreground.
     *          The green channel will be used to represent the gray-scale value.  When these raster images
     *          are saved to a image file such as a BMP file the pixel value of 0 will point to the color
     *          value of (255, 255, 255) and  pixel value 255 will point to the color value of (0, 0, 0).
     *          This way when displaying the image background will appear as white.
     */
    RasterSipper (kkint32  _height,
                  kkint32  _width
                 );


    /**
     *@brief  Constructs a blank image with given dimensions.  
     *@details  The third parameter determines whether it will be a color or gray-scale image, if a Color
     *          image then all three color channel will be set to = 255 which stands for white.  If
     *          Gray-scale the green channel will be set to 0.
     */
    RasterSipper (kkint32 _height,
                  kkint32 _width,
                  bool    _color
                 );

    /**
     *@brief  Constructs a RasterSipper from a BMP image loaded from disk.
     *@details If BMP Image is a gray-scale value pixel values will be reversed. See description of
     *        gray-scale constructor.
     */
    RasterSipper (const BmpImage&  _bmpImage);


    /**
     *@brief Constructs a new RasterSipper using a subset of the specified RasterSipper as its source.  The
     *      dimensions of the resultant raster will be '_height', and '_width'
     */
    RasterSipper (const RasterSipper& _raster,  /**<  Source RasterSipper                                 */
                  kkint32             _row,     /**<  Starting Row in '_raster' to copy from.             */
                  kkint32             _col,     /**<  Starting Col in '_raster' to copy from.             */
                  kkint32             _height,  /**<  Height of resultant raster. Will start from '_row'  */
                  kkint32             _width    /**<  Width of resultant raster.                          */
                 );

    /**
     *@brief Constructs a RasterSipper that will be the same size as the specified '_mask' with the top left specified by '_row' and '_col'.
     *@details The Height and Width of the resultant image will come from the bias of the specified mask.  The Image data will come from
     * the specified raster using '_row' and '_col' to specify the top left column.
     *@param[in]  _raster  Source Raster to extract data from.
     *@param[in]  _mask  Used to derive height and with of resultant image.
     *@param[in]  _row  Starting row where image data is to be extracted from.
     *@param[in]  _col  Starting column where image data is to be extracted from.
     *@see KKB::MorphOp::MaskTypes
     */
    RasterSipper (const RasterSipper&  _raster,
                  MaskTypes            _mask,
                  kkint32              _row,
                  kkint32              _col
                 );

    /**
     *@brief  Constructs a RasterSipper image from by reading an existing image File such as a BMP file.
     *@details  Will read from the specified file (fileName) the existing image.  If the load fails then 
     *          the contents of this object will be undefined.
     *@param[in]  fileName  Name of Image file to read.
     *@param[out] validFile  If image successfully loaded will be set to 'True' otherwise 'False'.
     */
    RasterSipper (const KKStr&  fileName,    /*!<  \param  fileName  name of image file to load/                    */
                  bool&         validFile    /*!<  \param  validFile will return true if image successfully loaded. */
                 );


    /**
     *@brief  Construct a raster object that will utilize a image already in memory.
     *@details  This instance will not own the raster data.  It will only point to it.
     */
    RasterSipper (kkint32  _height,
                  kkint32  _width,
                  uchar*   _grayScaleData,
                  uchar**  _grayScaleRows
                 );


    /**
     *@brief  Construct a Gray-scale RasterSipper object using provided raw data,
     */
    RasterSipper (kkint32       _height,
                  kkint32       _width,
                  const uchar*  _grayScaleData
                 );

    /**
     *@brief  Construct a Color RasterSipper object using provided raw data,
     */
    RasterSipper (kkint32       _height,
                  kkint32       _width,
                  const uchar*  _redChannel,
                  const uchar*  _greenChannel,
                  const uchar*  _blueChannel
                 );


    virtual
    ~RasterSipper ();


    kkMemSize  MemoryConsumedEstimated ()  const;

    
    ///<summary> Turn an instance of &quot;Raster&quot; into &quot;RasterSipper&quot;. </summary>
    ///<remarks> 
    /// Will utilize the dynamically allocated raster data of the &quot;Raster&quot; instance <paramref name="r"/> to build a new instance of 
    /// a &quot;SipperRaster&quot;. If the underlying type of &quot;r&quot; is a instance of &quot;RasterSipper&quot; will then return a dynamic_cast 
    /// of the pointer; otherwise will create a new instance of a &quot;RasterSipper&quot; class and give it ownership of ALL dynamically allocated members 
    /// of &quot;r&quot;.
    ///</remarks>
    ///<param name="r">The raster data will be given to the new instance of &quot;RasterSipper&quot; and &quot;r&quot; will then be deleted and set to NULL. </param>
    ///<returns>A SipperRaster instance File.</returns>
    static
      RasterSipperPtr  TurnIntoSipperRasterPtr (RasterPtr& r);



    /**
     *@brief Returns a image that is the result of a BandPass using Fourier Transforms.
     *@details A 2D Fourier transform is performed. The range specified is from 0.0 to 1.0 where range is determined from the center of the image 
     * to the farthest corner where the center is 0.0 and the farthest corner is 1.0.  Pixels in the resultant 2D Transform that are "NOT" in the
     * specified range are set to 0.0.  A reverse transform is then performed and the resultant image is returned.
     *@param[in] lowerFreqBound  Lower range of frequencies to retain; between 0.0 and 1.0.
     *@param[in] upperFreqBound  Upper range of frequencies to retain; between 0.0 and 1.0.
     *@param[in] retainBackground  Indicates whether pixes that don't meet the background-foreground threshold 
     * should be included as part of the Fourier transform; if set to false.
     *@return The result image.
     */
    RasterSipperPtr     BandPass (float  lowerFreqBound,
                                  float  upperFreqBound,
                                  bool   retainBackground
                                 );


    RasterSipperPtr     BinarizeByThreshold (uchar  min,
                                             uchar  max
                                            );

    static
    float               ComputeSegmentLens (const PointListPtr  points,
                                            float               heightFactor,
                                            float               widthFactor
                                           );

    RasterSipperPtr     CreateDilatedRaster ()  const;


    RasterSipperPtr     CreateDilatedRaster (MaskTypes  mask)  const;

    RasterSipperPtr     CreateErodedImage (MaskTypes  mask)  const;


    RasterSipperPtr     CreateGrayScale ()  const;

    /**
     *@brief Creates a gray-scale image using a KLT Transform with the goal of weighting in favor the color
     * channels with greatest amount of variance.
     *@details The idea is to weight each color channel by the amount of variance.  This is accomplished by
     *  producing a covariance matrix of the three color channels and then taking the Eigen vector with the
     *  largest Eigen value and using its components to derive weights for each channel for the conversion
     *  from RGB to gray-scale.
     */
    RasterSipperPtr     CreateGrayScaleKLT ()  const;

    /**
     *@brief  Same as 'CreateGrayScaleKLT' except it will only take into account 
     *        pixels specified by the 'mask' image.
     *@param[in]  mask  RasterSipper object where pixels that are greater than 'backgroundPixelTH' are to be considered.
     */
    RasterSipperPtr     CreateGrayScaleKLTOnMaskedArea (const RasterSipper&  mask)  const;

    static
    RasterSipperPtr     CreatePaddedRaster (BmpImage&  image,
                                            kkint32    padding
                                           );

    RasterSipperPtr     CreateSmoothImage (kkint32  maskSize = 3)  const;
    
    RasterSipperPtr     CreateSmoothedMediumImage (kkint32 maskSize)  const;

    RasterSipperPtr     CreateGaussianSmoothedImage (float sigma)  const;

    RasterSipperPtr     CreateColorImageFromLabels ();

    RasterSipperPtr     CreateColorWithBlobsLabeldByColor (BlobListPtr  blobs);  /**< Only useful if 'ExtractBlobs' was performed on this instance, the returned image will be color with each blob labeled a different color. */


    /**
     *@brief Returns a copy of 'origImage' where only the blobs specified in 'blobs' are copied over.
     *@details 
     *@code
     *  Example:
     *      origImage = image that we want to segment and get list of discrete blobs from.
     *      RasterSipperPtr  segmentedImage = origImage->SegmentImage ();
     *      BlobListPtr  blobs = segmentedImage->ExtractBlobs (1);
     *      RasterSipperPtr  imageWithBlobOnly = segmentedImage->CreateFromOrginalImageWithSpecifidBlobsOnly (blobs);
     *@endcode
     *@param[in]  origImage  Image that this instance was derived for, must have same dimensions.
     *@param[in]  blobs      A list of Blob instances that you want copied to the new instance of 'RasterSipper' returned,
     *@returns Image consisting of specified blobs only.
     */
    RasterSipperPtr     CreateFromOrginalImageWithSpecifidBlobsOnly (RasterSipperPtr    origImage,
                                                                     BlobListPtr  blobs
                                                                    );


    PointListPtr        DeriveImageLength () const;

    /**
     *@brief  Extracts a specified blob from this image;  useful to extract individual detected blobs.
     *@details  The 'ExtractBlobs' method needs to have been performed on this instance first.  You
     * would use this method after calling 'ExtractBlobs'. The extracted image will be of the same 
     * dimensions as the original image except it will extract the pixels that belong to the specified
     * blob only.
     *@code
     *   // Example of processing extracted blobs
     *   void  ProcessIndividulConectedComponents (RasterSipperPtr  image)
     *   {
     *     BlobListPtr blobs = image->ExtractBlobs (3);
     *     BlobList::iterator  idx;
     *     for  (idx = blobs->begin ();  idx != end ();  ++idx)
     *     {
     *       RasterSipperPtr  individuleBlob = image->ExtractABlob (*idx);
     *       DoSomethingWithIndividuleBlob (individuleBlob);
     *       delete  individuleBlob;
     *       individuleBlob = NULL;
     *     )
     *     delete  blobs;
     *     blobs = NULL;
     *   }
     *@endcode
     */
    RasterSipperPtr     ExtractABlob (const BlobPtr  blob)  const;


    /**
     *@brief  Extracts a specified blob from this image into a tightly bounded image.
     *@details
     *        Similar to 'ExtractABlob' except that the returned image will have the dimension necessary
     *        to contain the specified blob with the specified number of padded row and columns.
     */
    RasterSipperPtr     ExtractABlobTightly (const BlobPtr  blob,
                                             kkint32        padding
                                            ) const;


    /**
     *@brief Will return a gray-scale image consisting of the specified color channel only.
     */
    RasterSipperPtr     ExtractChannel (ColorChannels  channel);

    RasterSipperPtr     FastFourier ()  const;
    RasterSipperPtr     FastFourierKK ()  const;


    ///<summary> Creates a raster from a compressedBuff created by "SimpleCompression";. </summary>
    static
    RasterSipperPtr     FromSimpleCompression (const uchar*  compressedBuff,
                                               kkuint32      compressedBuffLen
                                              ); 

     ///<summary>
     /// Creates a new instance of RasterSipper object from zLib compressed data; if performs the inverse operation of RasterSipper::ToCompressor.</summary>
     ///<param name="compressedBuff"> Pointer to buffer area containing compressed data originally created by @quot;ToCompressor@quot;.</param>
     ///<param name="compressedBuffLen"> Length in bytes of 'compressedBuff'. </param>
     ///<returns> If successful a pointer to a new instance of 'RasterSipper'; if there is an error will return NULL. </returns>
    static
    RasterSipperPtr     FromCompressor (const uchar*  compressedBuff,
                                        kkuint32      compressedBuffLen
                                       ); 

    RasterSipperPtr     HalfSize ();

    RasterSipperPtr     HistogramEqualizedImage ()  const;

    RasterSipperPtr     HistogramEqualizedImage (HistogramPtr  equalizedHistogram) const;

    RasterSipperPtr     HistogramImage (ColorChannels  channel)  const;

    RasterSipperPtr     HistogramGrayscaleImage ()  const;

    RasterSipperPtr     Padded (kkint32 padding);  // Creates a Padded raster object.

    RasterSipperPtr     ReversedImage ();

    RasterSipperPtr     ReduceByEvenMultiple (kkint32  multiple)  const;

    RasterSipperPtr     ReduceByFactor (float factor)  const;  //  0 < factor <= 1.0  ex: 0.5 = Make raster half size

    RasterSipperPtr     RemoveZooscanBrackets ()  const;

    RasterSipperPtr     Rotate (float  turnAngle);

    RasterSipperPtr     SegmentImage (bool  save = false);

    uchar*              SimpleCompression (kkuint32&  buffLen) const; // Will create a compress image using 'SimpleCompression'

    RasterSipperPtr     SobelEdgeDetector ();

    RasterSipperListPtr SplitImageIntoEqualParts (kkint32 numColSplits,
                                                  kkint32 numRowSplits
                                                 )  const;


    RasterSipperPtr     StreatchImage (float  rowFactor,
                                       float  colFactor
                                      )  const;

    RasterSipperPtr     SwapQuadrants ()  const;

    
    ///<summary>Thresholds image in HSI space.</summary>
    ///<remarks>
    /// Returns an image with only the pixels that are within a specified distance in HSI space to the supplied HSI 
    /// parameters. All pixels that are not within the specified distance will be set to &quot;flagValue&quot;.
    ///</remarks>
    ///<param name="thresholdH">  Hue in radians(0.0 thru 2Pie) .</param>
    ///<param name="thresholdS">  Saturation (0.0 thru 1.0). .</param>
    ///<param name="thresholdI">  Intensity (0.0 thru 1.0). .</param>
    ///<param name="distance">    Threshold Distance (0.0 thru 1.0) that a pixel must be within in HSI space to be included. .</param>
    ///<param name="flagValue">   PixelValue to set for pixels that are NOT within &quot;distance&quot; of threshold. .</param>
    ///<returns> A image where pixels that are within the threshold will retain their original pixel values and the ones that are 
    /// not will be set to &quot;flagValue&quot;.</returns>
    RasterSipperPtr     ThresholdInHSI (float              thresholdH,
                                        float              thresholdS, 
                                        float              thresholdI, 
                                        float              distance,
                                        const PixelValue&  flagValue
                                       );

    RasterSipperPtr     ThinContour ();

    RasterSipperPtr     TightlyBounded (kkuint32 borderPixels)  const;

    RasterSipperPtr     ToColor ()  const;

    virtual
    RasterPtr  AllocateARasterInstance (kkint32 _height,  kkint32  _width,  bool _color)  const;

    virtual
    RasterPtr  AllocateARasterInstance (const Raster& r)  const;


    virtual
      RasterPtr  AllocateARasterInstance (const Raster& _raster,  /**<  Source Raster. */
                                          kkint32       _row,     /**<  Starting Row in '_raster' to copy from.             */
                                          kkint32       _col,     /**<  Starting Col in '_raster' to copy from.             */
                                          kkint32       _height,  /**<  Height of resultant raster. Will start from '_row'  */
                                          kkint32       _width    /**<  Width of resultant raster.                          */
                                         );  private:

  private:
    bool  BlackPixel (kkint32 row,  kkint32  col) const;

    bool  RectangleAllBlack (kkint32 tlRow,  kkint32 tlCol,  kkint32 brRow,  kkint32 brCol)  const;

    bool  RectangleAllWhite (kkint32 tlRow,  kkint32 tlCol,  kkint32 brRow,  kkint32 brCol)  const;

  };  /* RasterSipper */


  typedef  RasterSipper::RasterSipperPtr  RasterSipperPtr;





  class  RasterSipperList:  public  KKQueue<RasterSipper>
  {
  public:
    RasterSipperList (bool  owner):
        KKQueue<RasterSipper> (owner)
        {}

  private:
    RasterSipperList (const RasterSipperList&  rasterList):
        KKQueue<RasterSipper> (rasterList)
        {}

  public:
    RasterSipperList (const RasterSipperList&  rasterList,
                      bool                     _owner
                     ):
        KKQueue<RasterSipper> (rasterList, _owner)
        {}

    RasterSipperList (const RasterList&  rasterList);

    RasterSipperList (const RasterList&  rasterList,
                      bool               _owner
                     );
  };  /* RasterSipperList */


  typedef  RasterSipperList*  RasterSipperListPtr;


}  /* namespace KKB; */
#endif
