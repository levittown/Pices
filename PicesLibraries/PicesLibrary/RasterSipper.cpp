/* RasterSipper.cpp -- Class for one raster image.
 * Copyright (C) 1994-2011 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */
#include "FirstIncludes.h"
#include <memory>
#include <math.h>
#include <limits.h>
#include <fstream>
#include <map>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "Blob.h"
#include "BMPImage.h"
#include "Compressor.h"
#include "ConvexHull.h"
#include "Histogram.h"
#include "ImageIO.h"
#include "KKException.h"
#include "Matrix.h"
#include "MorphOpSobel.h"
#include "OSservices.h"
#include "SimpleCompressor.h"
using namespace KKB;

#include "RasterSipper.h"
using namespace MLL;


RasterSipper::RasterSipper (): Raster ()
{
}



RasterSipper::RasterSipper (kkint32  _height,
                            kkint32  _width
                           ):
  Raster (_height, _width)
{
}



RasterSipper::RasterSipper (kkint32 _height,
                            kkint32 _width,
                            bool  _color
                           ):
  Raster (_height, _width, _color)
{
}



/**
 *@brief  Constructs a RasterSipper from a BMP image loaded from disk.
 *@detais If BMP Image is a gray-scale value pixel values will be reversed.  See description of gray-scale constructor.
 */
RasterSipper::RasterSipper (const BmpImage&  _bmpImage):
  Raster (_bmpImage)
{
}




RasterSipper::RasterSipper (const RasterSipper&  _raster):
  Raster (_raster)
{
}



RasterSipper::RasterSipper (const Raster&  _raster):
  Raster (_raster)
{
}



RasterSipper::RasterSipper (const RasterSipper&  _raster,
                            kkint32              _row,
                            kkint32              _col,
                            kkint32              _height,
                            kkint32              _width
                           ):
  Raster (_raster, _row, _col, _height, _width)
{
}



RasterSipper::RasterSipper (const RasterSipper& _raster,
                            MaskTypes           _mask,
                            kkint32             _row,
                            kkint32             _col
                           ):
  Raster (_raster, _mask, _row, _col)
{
}



/**
 *@brief Constructs a raster object from a Image File.
 *@details If a color image the pixel values will be reversed, see constructor
 *         for gray-scale images.
 */
RasterSipper::RasterSipper (const KKStr&  _fileName,
                            bool&          validFile
                           ):
  Raster (_fileName, validFile)
{
}




RasterSipper::RasterSipper (kkint32  _height,
                            kkint32  _width,
                            uchar*   _grayScaleData,
                            uchar**  _grayScaleRows
                           ):
  Raster (_height, _width, _grayScaleData, _grayScaleRows)
{
}



RasterSipper::RasterSipper (kkint32       _height,
                            kkint32       _width,
                            const uchar*  _grayScaleData
                           ):
  Raster (_height, _width, _grayScaleData)
{
}


RasterSipper::RasterSipper (kkint32       _height,
                            kkint32       _width,
                            const uchar*  _redChannel,
                            const uchar*  _greenChannel,
                            const uchar*  _blueChannel
                           ):
  Raster (_height, _width, _redChannel, _greenChannel, _blueChannel)
{
}




RasterSipper::~RasterSipper ()
{
}



kkMemSize  RasterSipper::MemoryConsumedEstimated ()  const
{
  return  Raster::MemoryConsumedEstimated ();
}



RasterSipperPtr  RasterSipper::TurnIntoSipperRasterPtr (RasterPtr& r)
{
  if  (r == NULL)
    return NULL;

  if  (typeid (*r) == typeid (RasterSipper))
  {
    RasterSipperPtr  rs = dynamic_cast<RasterSipperPtr> (r);
    r = NULL;
    return rs;
  }
  else
  {
    RasterSipperPtr  rs = new RasterSipper ();
    rs->TakeOwnershipOfAnotherRastersData (*r);
    r->WeOwnRasterData (false);
    delete  r;
    r = NULL;
    return rs;
  }
}



RasterPtr  RasterSipper::AllocateARasterInstance (kkint32  _height,
                                                  kkint32  _width,
                                                  bool     _color
                                                 )  const
{
  return new RasterSipper (_height, _width, _color);
}



RasterPtr  RasterSipper::AllocateARasterInstance (const Raster& r)  const
{
  return new RasterSipper (r);
}



RasterPtr  RasterSipper::AllocateARasterInstance (const Raster& _raster,  /**<  Source Raster                                       */
                                                  kkint32       _row,     /**<  Starting Row in '_raster' to copy from.             */
                                                  kkint32       _col,     /**<  Starting Col in '_raster' to copy from.             */
                                                  kkint32       _height,  /**<  Height of resultant raster. Will start from '_row'  */
                                                  kkint32       _width    /**<  Width of resultant raster.                          */
                                                 )
{
  RasterPtr  r = new Raster (_raster, _row, _col, _height, _width);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}



RasterSipperPtr  RasterSipper::CreatePaddedRaster (BmpImage&  image,
                                                   kkint32    padding
                                                  )
{
  RasterPtr  r = Raster::CreatePaddedRaster (image, padding);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreatePaddedRaster */



RasterSipperPtr   RasterSipper::ReversedImage ()
{
  RasterPtr  r = Raster::ReversedImage ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ReversedImage */



RasterSipperPtr  RasterSipper::CreateDilatedRaster ()  const
{
  RasterPtr  r = Raster::CreateDilatedRaster ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateDilatedRaster */



RasterSipperPtr  RasterSipper::CreateDilatedRaster (MaskTypes  mask)  const
{
  RasterPtr  r = Raster::CreateDilatedRaster (mask);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateDilatedRaster */



RasterSipperPtr  RasterSipper::CreateErodedImage (MaskTypes  mask)  const
{
  RasterPtr  r = Raster::CreateErodedImage (mask);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* Erosion  */



/**
 @brief Returns image where each blob is labeled with a different color.
 @details Only useful if 'ExtractBlobs' was performed on this instance.
 */
RasterSipperPtr  RasterSipper::CreateColorWithBlobsLabeldByColor (BlobListPtr  blobs)
{
  RasterPtr  r = Raster::CreateColorWithBlobsLabeldByColor (blobs);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateColorWithBlobsLabeldByColor*/



RasterSipperPtr  RasterSipper::CreateFromOrginalImageWithSpecifidBlobsOnly (RasterSipperPtr  origImage,
                                                                            BlobListPtr      blobs
                                                                           )
{
  RasterPtr  r = Raster::CreateFromOrginalImageWithSpecifidBlobsOnly (origImage, blobs);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateFromOrginalImageWithSpecifidBlobsOnly*/



RasterSipperPtr  RasterSipper::ExtractABlobTightly (const BlobPtr  blob,
                                                    kkint32        padding
                                                   )  const
{
  if  (blob == NULL)
    return NULL;

  RasterPtr  r = Raster::ExtractABlobTightly (blob, padding);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ExtractABlobTightly */



RasterSipperPtr  RasterSipper::ExtractABlob (const BlobPtr  blob)  const
{
  RasterPtr  b = Raster::ExtractABlob (blob);
  if  (b)
    return  RasterSipper::TurnIntoSipperRasterPtr (b);
  else
    return  NULL;
}  /* ExtractABlob */



RasterSipperPtr  RasterSipper::FastFourierKK ()  const
{
  RasterPtr  r = Raster::FastFourierKK ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* FastFourierKK */



RasterSipperPtr  RasterSipper::FastFourier ()  const
{
  RasterPtr  r = Raster::FastFourier ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* FastFourier */



RasterSipperPtr  RasterSipper::SwapQuadrants ()  const
{
  RasterPtr  r = Raster::SwapQuadrants ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* SwapQuadrants  */



RasterSipperPtr  RasterSipper::Rotate (float  turnAngle)
{
  RasterPtr  rotatedRaster = Raster::Rotate (turnAngle);
  return RasterSipper::TurnIntoSipperRasterPtr (rotatedRaster);
}  /* Rotate */



RasterSipperPtr  RasterSipper::CreateGrayScale ()  const
{
  RasterPtr  r = Raster::CreateGrayScale ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateGrayScale */



RasterSipperPtr  RasterSipper::HistogramEqualizedImage ()  const
{
  RasterPtr  r = Raster::HistogramEqualizedImage ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* HistogramEqualizedImage */



RasterSipperPtr  RasterSipper::HistogramEqualizedImage (HistogramPtr  equalizedHistogram)  const
{
  RasterPtr  r = Raster::HistogramEqualizedImage (equalizedHistogram);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* HistogramEqualizedImage */



RasterSipperPtr  RasterSipper::HistogramGrayscaleImage ()  const
{
  RasterPtr  r = Raster::HistogramGrayscaleImage ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* HistogramGrayscaleImage */



RasterSipperPtr  RasterSipper::HistogramImage (ColorChannels  channel)  const
{
  RasterPtr  r = Raster::HistogramImage (channel);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* HistogramImage */



RasterSipperPtr  RasterSipper::CreateSmoothImage (kkint32 maskSize)  const
{
  RasterPtr  r = Raster::CreateSmoothImage (maskSize);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
} /* CreateSmoothImage */



RasterSipperPtr  RasterSipper::CreateSmoothedMediumImage (kkint32 maskSize)  const
{
  RasterPtr  r = Raster::CreateSmoothedMediumImage (maskSize);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
} /* CreateSmoothedMediumImage */



RasterSipperPtr  RasterSipper::HalfSize ()
{
  RasterPtr  r = Raster::HalfSize ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* HalfSize */



RasterSipperPtr  RasterSipper::ReduceByEvenMultiple (kkint32  multiple)  const
{
  RasterPtr  r = Raster::ReduceByEvenMultiple (multiple);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ReduceByEvenMultiple */



RasterSipperPtr  RasterSipper::ReduceByFactor (float factor)  const  //  0 < factor <= 1.0
{
  RasterPtr  r = Raster::ReduceByFactor (factor);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ReduceByFactor */



bool  RasterSipper::BlackPixel (kkint32  row,
                                kkint32  col
                               )
                               const
{
  return  (green[row][col] > 225);
}  /* BlackPixel */



bool  RasterSipper::RectangleAllBlack (kkint32  tlRow,
                                       kkint32  tlCol,
                                       kkint32  brRow,
                                       kkint32  brCol
                                      )
                                      const
{
  if  ((brRow < tlRow)  ||  (brCol < tlCol))
    return false;

  for  (kkint32 r = tlRow;  r <= brRow;  ++r)
  {
    for  (kkint32 c = tlCol;  c <= brCol;  ++c)
      if  (green[r][c] < 225)
        return  false;
  }
  return  true;
}



bool  RasterSipper::RectangleAllWhite (kkint32  tlRow,
                                       kkint32  tlCol,
                                       kkint32  brRow,
                                       kkint32  brCol
                                      )
                                      const
{
  if  ((brRow < tlRow)  ||  (brCol < tlCol))
    return false;
  for  (kkint32 r = tlRow;  r <= brRow;  ++r)
  {
    for  (kkint32 c = tlCol;  c <= brCol;  ++c)
      if  (green[r][c] > 32)
        return  false;
  }
  return  true;
}



RasterSipperPtr  RasterSipper::RemoveZooscanBrackets ()  const
{
  kkint32  row, col;

  // Locate Top Left Bracket.
  kkint32  tlbRow = -1;
  kkint32  tlbCol = -1;
  for  (row = 1;  (row < (height - 10))  &&  (tlbRow < 0);  ++row)
  {
    for  (col = 1;  col < (width - 10);  ++col)
    {
      if  (RectangleAllBlack (row, col, row, col + 10)              &&
           RectangleAllBlack (row, col, row + 10, col)              &&
           RectangleAllWhite (row - 1, col - 1, row - 1,  col + 11) &&
           RectangleAllWhite (row - 1, col - 1, row + 11, col - 1)
          )
      {
        tlbRow = row;
        tlbCol = col;
        break;
      }
    }
  }

  if  (tlbRow < 0)
    return  new RasterSipper (*this);

  // Locate  Bottom Right Bracket
  kkint32  brbRow = -1;
  kkint32  brbCol = -1;
  for  (row = height - 1;  (row >  tlbRow)  &&  (brbRow < 0);  --row)
  {
    for  (col = width - 1;  col >  tlbCol;  --col)
    {
      if  (RectangleAllBlack (row, col - 10, row, col)              &&
           RectangleAllBlack (row - 10, col, row, col)              &&
           RectangleAllWhite (row + 1, col - 11, row + 1,  col + 1) &&
           RectangleAllWhite (row - 11, col + 1, row + 1, col + 1)
          )
      {
        brbRow = row;
        brbCol = col;
        break;
      }
    }
  }

  if  (brbRow < 0)
    return  new RasterSipper (*this);

  tlbRow++;
  tlbCol++;
  brbRow--;
  brbCol--;

  kkint32  newHeight = (1 + brbRow - tlbRow) + 4;  // The additional "+ 4" is for padding of 2 rows top and bottom.
  kkint32  newWidth  = (1 + brbCol - tlbCol) + 4;  // The additional "+ 4" is for padding of 2 rows top and bottom.

  RasterSipperPtr  r = new RasterSipper (newHeight, newWidth, Color ());

  uchar**  destGreen = r->Green ();
  uchar**  destRed   = r->Red   ();
  uchar**  destBlue  = r->Blue  ();

  {
    kkint32  destRow = 2;
    for  (row = tlbRow;  row <= brbRow;  ++row)
    {
      kkint32  destCol = 2;
      for  (col = tlbCol;  col <= brbCol;  ++col)
      {
        destGreen[destRow][destCol] = green[row][col];
        if  (color)
        {
          destRed  [destRow][destCol] = red  [row][col];
          destBlue [destRow][destCol] = blue [row][col];
        }
        ++destCol;
      }
      ++destRow;
    }
  }


  return  r;
}  /* RemoveZooscanBrackets */



RasterSipperPtr  RasterSipper::SobelEdgeDetector ()
{
  RasterPtr  r = Raster::SobelEdgeDetector ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* SobelEdgeDetector */



RasterSipperPtr  RasterSipper::StreatchImage (float  rowFactor,
                                              float  colFactor
                                             )  const
{
  RasterPtr  r = Raster::StreatchImage (rowFactor, colFactor);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}



RasterSipperPtr  RasterSipper::BinarizeByThreshold (uchar  min,
                                                    uchar  max
                                                   )
{
  RasterPtr  r = Raster::BinarizeByThreshold (min, max);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* BinarizeByThreshold */



float  RasterSipper::ComputeSegmentLens (const PointListPtr  points,
                                         float               heightFactor,
                                         float               widthFactor
                                        )
{
  if  (points)
    return points->ComputeSegmentLens (heightFactor, widthFactor);
  else
    return 0.0f;
}



RasterSipperPtr  RasterSipper::ExtractChannel (ColorChannels  channel)
{
  RasterPtr  r = Raster::ExtractChannel (channel);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ExtractChannel */



//******************************************************************************
//*  Wrote this method to deal with pollution sample particles, needed to help *
//* segment out particles from a very noisy background.                        *
//******************************************************************************
RasterSipperPtr   RasterSipper::SegmentImage (bool  save)
{
  RasterPtr  r = Raster::SegmentImage (save);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* SegmentImage */



RasterSipperListPtr  RasterSipper::SplitImageIntoEqualParts (kkint32 numColSplits,
                                                             kkint32 numRowSplits
                                                            )  const
{
  RasterListPtr  rasterParts = Raster::SplitImageIntoEqualParts (numColSplits, numRowSplits);
  if  (!rasterParts)
    return NULL;

  RasterSipperListPtr  rasterSipperParts = new RasterSipperList (*rasterParts);
  rasterParts->Owner (false);
  delete  rasterParts;
  rasterParts = NULL;
  return rasterSipperParts;
}  /* SplitImageIntoEqualParts */



RasterSipperPtr  RasterSipper::ThinContour ()
{
  RasterPtr  r = Raster::ThinContour ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ThinContour */



RasterSipperPtr  RasterSipper::TightlyBounded (kkuint32 borderPixels)  const
{
  RasterPtr  r = Raster::TightlyBounded (borderPixels);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* TightlyBounded */



RasterSipperPtr  RasterSipper::ToColor ()  const
{
  RasterPtr  r = Raster::ToColor ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ToColor */



RasterSipperPtr  RasterSipper::BandPass (float  lowerFreqBound,    /**< Number's between 0.0 and 1.0  */
                                         float  upperFreqBound,    /**< Represent percentage.         */
                                         bool   retainBackground
                                        )
{
  RasterPtr  r = Raster::BandPass(lowerFreqBound, upperFreqBound, retainBackground);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* BandPass */



uchar*   RasterSipper::SimpleCompression (kkuint32&  buffLen) const // Will create a compress image using 'SimpleCompression'
{
  kkint32  totalPixs = Height () * Width ();

  SimpleCompressor  compressor (totalPixs);

  compressor.Add16BitInt (Height ());
  compressor.Add16BitInt (Width ());

  kkint32  x;
  for  (x = 0;  x < totalPixs;  x++)
    compressor.AddByte (greenArea[x]);

  return  compressor.CreateCompressedBuffer (buffLen);
}  /* SimpleCompression */
  


// Creates a raster from a compressedBuff created by 'SimpleCompression'
RasterSipperPtr  RasterSipper::FromSimpleCompression (const uchar*  compressedBuff,
                                                      kkuint32      compressedBuffLen
                                                     )  
{
  // I expect simple run length compressed data to be passed in.  The data was  originally 
  // compressed by SimpleCompressor and we will use the same class to decompress.

  // The format of the uncompressed data is very simple.
  //  Bytes    Description
  //  0 thru 1:    Height   2 byte integer.
  //  2 thru 3:    Width    2 byte integer.
  //
  //  4 thru (Hight * Width) - 1:   Raster Data
  //          Row 0          : (consists of 'Width' bytes.)
  //          Row 1          : (    ""    ""    ""   ""   )
  //                   --     ---     ---     ----   
  //          Row Height - 1 : (Last row of image) 
  //
  //          Each byte in the raster represents one pixel 0 - 255 gray-scale;  where 0=background.

  kkuint32  unCompressedSize = 0;

  if  (compressedBuff == NULL)
  {
    cerr << endl
         << "RasterSipper::FromSimpleCompression   ***ERROR***  (compressedBuff==NULL)" << endl
         << endl;
    return new RasterSipper (20, 20, false);
  }

  uchar*  uncompressedBuff = SimpleCompressor::Decompress (compressedBuff, compressedBuffLen, unCompressedSize);
  if  (uncompressedBuff == NULL)
  {
    cerr << endl
         << "RasterSipper::FromSimpleCompression   ***ERROR***  (uncompressedBuff==NULL)" << endl
         << endl;
    return new RasterSipper (20, 20, false);
  }

  // The first Four Bytes have the image Height, and Width.

  kkint32  height = uncompressedBuff[0] * 256 +  uncompressedBuff[1];
  kkint32  width  = uncompressedBuff[2] * 256 +  uncompressedBuff[3];

  if  ((height < 1)  ||  (height > 1000)  ||  (width < 1)  ||  (width > 1000))
  {
    cerr << endl
         << "RasterSipper::FromSimpleCompression   ***ERROR***   Height[" << height << "]  Width[" << width << "]  are out of bounds." << endl
         << endl;
    return new RasterSipper (20, 20, false);
  }

  kkuint32  totalPixels = height * width;
  if  (unCompressedSize > (totalPixels + 4))
  {
    cerr << endl
         << "RasterSipper::FromSimpleCompression   ***ERROR***   unCompressedSize[" << unCompressedSize << "] is greater than 'totalPixels + 4'[" << totalPixels + 4 << "]." << endl
         << endl;
  }

  RasterSipperPtr  result = new RasterSipper (height, width, false);
  uchar* greenArea = result->GreenArea ();

  kkuint32  nextIdx = 4;
  kkuint32  greanAreaIdx = 0;

  while  ((nextIdx < (kkuint32)unCompressedSize)  &&  (greanAreaIdx < totalPixels))
  {
    greenArea[greanAreaIdx] = uncompressedBuff[nextIdx];
    nextIdx++;
    greanAreaIdx++;
  }

  delete  uncompressedBuff;
  uncompressedBuff = NULL;

  return  result;
}  /* FromSimpleCompression */



RasterSipperPtr  RasterSipper::FromCompressor (const uchar*  compressedBuff,    // Creates a raster from a compressedBuff created by 'Compressor'(zlib)
                                               kkuint32      compressedBuffLen
                                              )
{
  kkuint32  unCompressedBuffLen = 0;
  uchar*  unCompressedBuff = NULL;
  try
  {
    unCompressedBuff = (uchar*)Compressor::Decompress (compressedBuff, compressedBuffLen, unCompressedBuffLen);
  }
  catch  (...)
  {
    KKStr errMsg = "RasterSipper::FromCompressor  Exception thrown calling 'Compressor::Decompress'.";
    cerr << errMsg << endl;
    throw KKException (errMsg);
  }

  if  (!unCompressedBuff)
    return NULL;

  if  (unCompressedBuffLen < 10)
  {
    cerr << std::endl << std::endl << "Raster::FromCompressor  Compressor did not return any data."  << std::endl;
    return NULL;
  }

  // 0 - 3:    Height:  high order to low order
  // 4 - 7:    Width:   high order to low order
  // 8 - 8:    Color    0 = Gray-scale,  1 = Color
  // 9 - 8 + (Height * Width) Green Channel
  // xxxxx                    Red  Channel
  // xxxxx                    Blue Channel

  kkuint32  height = 0;
  kkuint32  width  = 0;
  height = unCompressedBuff[0] + unCompressedBuff[1] * 256 + unCompressedBuff[2] * 256 * 256 + unCompressedBuff[3] * 256 * 256 * 256;
  width  = unCompressedBuff[4] + unCompressedBuff[5] * 256 + unCompressedBuff[6] * 256 * 256 + unCompressedBuff[7] * 256 * 256 * 256;

  bool color = (unCompressedBuff[8] == 1);

  kkuint32  totalPixels = height * width;
  if  ((totalPixels > (100 * 1024 * 1024))  ||  (height < 1)  ||  (width < 1))
  {
    cerr << std::endl << std::endl << "Raster::FromCompressor  Height[" << height << "]  Width[" << width << "]  is not valid." << std::endl << std::endl;
    delete  unCompressedBuff;
    unCompressedBuff = NULL;
    return NULL;
  }

  kkuint32  totalDataNeeded = totalPixels + (color ? (2 * totalPixels) : 0) + 9;
  if  (totalDataNeeded > unCompressedBuffLen)
  {
    cerr << std::endl << std::endl 
         << "Raster::FromCompressor  Height[" << height << "]  Width[" << width << "]  Color[" << (color ? "Yes" : "No") << "]" << std::endl
         << "              requires TotalDataNeeded[" << totalDataNeeded << "]  but UnCompressedBuffLen[" << unCompressedBuffLen << "]" << std::endl
         << std::endl;
    delete  unCompressedBuff;
    unCompressedBuff = NULL;
    return NULL;
  }

  RasterSipperPtr  r = new RasterSipper ((kkint32)height, (kkint32)width, color);

  kkuint32 nextIdx = 9;
  kkuint32 x;

  uchar*  greenArea = r->GreenArea ();
  for  (x = 0;  x < totalPixels;  x++, nextIdx++)
    greenArea[x] = unCompressedBuff[nextIdx];

  if  (color)
  {
    uchar*  redArea = r->RedArea ();
    for  (x = 0;  x < totalPixels;  x++, nextIdx++)
      redArea[x] = unCompressedBuff[nextIdx];

    uchar*  blueArea = r->BlueArea ();
    for  (x = 0;  x < totalPixels;  x++, nextIdx++)
      blueArea[x] = unCompressedBuff[nextIdx];
  }
 
  delete  unCompressedBuff;
  unCompressedBuff = NULL;

  return  r;
}  /* FromCompressor */




RasterSipperPtr   RasterSipper::Padded (kkint32 padding)
{
  RasterPtr  r = Raster::Padded (padding);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}



RasterSipperPtr  RasterSipper::CreateGaussianSmoothedImage (float sigma)  const
{
  RasterPtr  r = Raster::CreateGaussianSmoothedImage (sigma);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateGaussianSmoothedImage */



RasterSipperPtr  RasterSipper::ThresholdInHSI (float              thresholdH,
                                               float              thresholdS, 
                                               float              thresholdI, 
                                               float              distance,
                                               const PixelValue&  flagValue
                                              )
{
  RasterPtr  r = Raster::ThresholdInHSI (thresholdH, thresholdS, thresholdI, distance, flagValue);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* ThresholdingHSI */



RasterSipperPtr  RasterSipper::CreateGrayScaleKLT ()  const
{
  RasterPtr  r = Raster::CreateGrayScaleKLT ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateGrayScaleKLT */



RasterSipperPtr  RasterSipper::CreateGrayScaleKLTOnMaskedArea (const RasterSipper&  mask)  const
{
  RasterPtr  r = Raster::CreateGrayScaleKLTOnMaskedArea (mask);
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateGrayScaleKLTOnMaskedArea */



RasterSipperPtr  RasterSipper::CreateColorImageFromLabels ()
{
  RasterPtr  r = Raster::CreateColorImageFromLabels ();
  return  RasterSipper::TurnIntoSipperRasterPtr (r);
}  /* CreateColorImageFromLabels */



PointListPtr  RasterSipper::DeriveImageLength () const
{
  PointListPtr  results = NULL;

  float  eigenRatio;
  float  orientationAngle;

  RasterSipperPtr  workRaster = CreateErodedImage (MorphOp::MaskTypes::SQUARE3);
  workRaster->FillHole ();
  workRaster->Erosion (MorphOp::MaskTypes::SQUARE7);
  workRaster->ConnectedComponent (3);
  workRaster->CalcOrientationAndEigerRatio (eigenRatio,
                                            orientationAngle
                                           );
  if  ((orientationAngle > TwoPie)  ||  (orientationAngle < -TwoPie))
  {
    orientationAngle = 0.0;
    eigenRatio = 1.0;
  }
 
  RasterSipperPtr rotatedImage = workRaster->Rotate (orientationAngle);
  kkint32  tlRow;
  kkint32  tlCol;
  kkint32  brRow;
  kkint32  brCol;

  rotatedImage->FindBoundingBox (tlRow, tlCol, brRow, brCol);
  if  (tlRow >= 0)
  {
    uchar**  imageData = rotatedImage->Green ();

    kkint32  boxWidth  = brCol - tlCol;

    kkint32  mark1Col = (kkint32)((float)boxWidth * 0.05f + 0.5f) + tlCol;
    kkint32  mark2Col = (kkint32)((float)boxWidth * 0.95f + 0.5f) + tlCol;

    kkint32  a1RowTot   = 0;
    kkint32  a1ColTot   = 0;
    kkint32  a1PixCount = 0l;

    kkint32  a2RowTot   = 0;
    kkint32  a2ColTot   = 0;
    kkint32  a2PixCount = 0l;

    kkint32  a3RowTot   = 0;
    kkint32  a3ColTot   = 0;
    kkint32  a3PixCount = 0;

    for  (kkint32 row = tlRow;  row <= brRow;  ++row)
    {
      kkint32 col = 0;

      uchar*  rowData = imageData[row];

      for  (col = tlCol;  col <= mark1Col;  ++col)
      {
        if  (rowData[col] > 2)
        {
          ++a1PixCount;
          a1RowTot += row;
          a1ColTot += col;
        }
      }

      for  (col = mark1Col + 1;  col <= mark2Col;  ++col)
      {
        if  (rowData[col] > 2)
        {
          ++a2PixCount;
          a2RowTot += row;
          a2ColTot += col;
        }
      }

      for  (col = mark2Col + 1;  col <= brCol;  ++col)
      {
        if  (rowData[col] > 2)
        {
          ++a3PixCount;
          a3RowTot += row;
          a3ColTot += col;
        }
      }
    }

    a1PixCount = Max (1, a1PixCount);
    a2PixCount = Max (1, a2PixCount);
    a3PixCount = Max (1, a3PixCount);

    Point p1 ((int)(0.5f + a1RowTot / a1PixCount), (int)(0.5f + a1ColTot / a1PixCount));
    Point p2 ((int)(0.5f + a2RowTot / a2PixCount), (int)(0.5f + a2ColTot / a2PixCount));
    Point p3 ((int)(0.5f + a3RowTot / a3PixCount), (int)(0.5f + a3ColTot / a3PixCount));

   
    Point p1Orig = RotateDerivePreRotatedPoint (height, width, p1, orientationAngle);
    Point p2Orig = RotateDerivePreRotatedPoint (height, width, p2, orientationAngle);
    Point p3Orig = RotateDerivePreRotatedPoint (height, width, p3, orientationAngle);

    results = new PointList (true);
    results->PushOnBack (new Point (p1Orig));
    results->PushOnBack (new Point (p2Orig));
    results->PushOnBack (new Point (p3Orig));
  }
  
  delete rotatedImage;  rotatedImage = NULL;
  delete workRaster;    workRaster   = NULL;

  return  results;

}  /* DeriveImageLength */



RasterSipperList::RasterSipperList (const RasterList&  rasterList,
                                    bool               _owner
                                   ):
     KKQueue<RasterSipper> (_owner)
{
  for  (auto r: rasterList)
  {
    if  (typeid(*r) == typeid(RasterSipper))
    {
      RasterSipperPtr  sp = dynamic_cast<RasterSipperPtr> (r);
      if  (!_owner)
        PushOnBack (sp);
      else
        PushOnBack (new RasterSipper (*sp));
    }
    else
    {
      if  (!_owner)
        throw KKException ("RasterSipperList::RasterSipperList  instance in container is not a 'RasterSipperPtr'");
      else
        PushOnBack (new RasterSipper (*r));
    }
  }
}



RasterSipperList::RasterSipperList (const RasterList&  rasterList):
    KKQueue<RasterSipper> (rasterList.Owner ())
{
  RasterList::const_iterator  idx;
  for  (idx = rasterList.begin ();  idx != rasterList.end ();  ++idx)
  {
    RasterPtr  r = *idx;
    if  (typeid(*r) == typeid(RasterSipper))
    {
      RasterSipperPtr  sp = dynamic_cast<RasterSipperPtr> (r);
      if  (!Owner ())
        PushOnBack (sp);
      else
        PushOnBack (new RasterSipper (*sp));
    }
    else
    {
      if  (!Owner ())
        throw KKException ("RasterSipperList::RasterSipperList  instance in container is not a 'RasterSipperPtr'");
      else
        PushOnBack (new RasterSipper (*r));
    }
  }
}
