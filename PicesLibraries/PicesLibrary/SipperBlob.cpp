#include "FirstIncludes.h"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include  "MemoryDebug.h"
using namespace  std;


#include "KKBaseTypes.h"
#include "BMPImage.h"
#include "Raster.h"
#include "KKStr.h"
using namespace  KKB;


#include "MLClass.h"
using namespace  KKMLL;


#include "ImageFeatures.h"
#include "SipperBlob.h"
using namespace  MLL;


namespace  MLL
{
kkuint32  SipperBlob::maxFrameHeight = 0;
kkuint32  SipperBlob::maxFrameWidth  = 0;


SipperBlob::SipperBlob ():
  id (-1),
  explored            (false),
  rowBot              (0),
  rowTop              (0),
  colLeft             (0),
  colRight            (0),
  neighborIdLastAdded (-1),
  neighbors           (),
  pixelCount          (0)
  
{
}




SipperBlob::~SipperBlob ()
{
  neighbors.clear ();
}



ImageFeaturesPtr  SipperBlob::Save (uchar**     frame,
                                    kkint32**   blobIds,
                                    KKStr       fileName,
                                    bool        createImageFeaturesObject,
                                    MLClassPtr  mlClass,
                                    kkuint32    firstFrameRowScanLine,
                                    bool        save,     /**< If set to false will not save, just calculate features. */
                                    bool        colorize, /**< Set to true if you wish to save the 7 gray-scale values as colors.*/
                                    RunLog&     runLog
                                   )

{
  kkuint32 col;
  kkuint32 row;

  kkuint32 numOfRows = rowBot - rowTop  + 1;
  kkuint32 numOfCols;

  kkint32  colOffset;
  kkint32  bmpRow;
  kkint32  bmpCol;

  ImageFeaturesPtr  image = NULL;

/*  if  (numOfRows > 1000)
  {
     numOfRows = rowBot   - rowTop  + 1;
     numOfCols = colRight - colLeft + 1;
     bmpRow    = 0;
     colOffset = 0;
  }
  else
  */

  {
    numOfRows = rowBot   - rowTop  + 17;
    numOfCols = colRight - colLeft + 1;

    bmpRow = 8;

    kkint32 padding = 16;

    colOffset = padding / 2;

    numOfCols = numOfCols + padding;
  }

  BmpImage  bmpImage (numOfRows, numOfCols, 256);


  if  (colorize)
  {
    //                                          R    G    B
    bmpImage.SetPaletteEntry (  0, PixelValue (255, 255, 255));
    bmpImage.SetPaletteEntry ( 36, PixelValue (255, 255,   0));
    bmpImage.SetPaletteEntry ( 73, PixelValue (255,   0, 255));
    bmpImage.SetPaletteEntry (109, PixelValue (255,   0,   0));
    bmpImage.SetPaletteEntry (146, PixelValue (  0, 255, 255));
    bmpImage.SetPaletteEntry (182, PixelValue (  0, 255,   0));
    bmpImage.SetPaletteEntry (219, PixelValue (  0,   0, 255));
    bmpImage.SetPaletteEntry (255, PixelValue (  0,   0,   0));
  }

  for  (row = rowTop; row <= rowBot; row++)
  {
    bmpCol = colOffset;

    for  (col = colLeft; col <= colRight; col++)
    {
      if  (blobIds[row][col] == id)
      {
        bmpImage.AddPixel (bmpRow, bmpCol, frame[row][col]);
      }

      bmpCol++;
    }

    bmpRow++;
  }

  if  (save)
  {
    if  (colorize)
      bmpImage.Save (fileName);
    else
      bmpImage.SaveGrayscaleInverted4Bit (fileName);
  }

  if  (createImageFeaturesObject)
  {
    image = new ImageFeatures (bmpImage, mlClass, NULL, runLog);
    //image->CentroidCol (image->CentroidCol () + colLeft);     //  kk  2005-feb-02
    //image->CentroidRow (image->CentroidRow () + rowTop + firstFrameRowScanLine);
  }

  return image;
} /* Save */




RasterPtr  SipperBlob::GetRaster (uchar** frame,
                                  kkint32**   blobIds,     
                                  kkuint32  firstFrameRowScanLine
                                 )  const

{
  kkuint32 col;
  kkuint32 row;

  kkuint32 numOfRows;
  kkuint32 numOfCols;

  kkint32  colOffset;
  kkint32  bmpRow;
  kkint32  bmpCol;

  {
    numOfRows = rowBot   - rowTop  + 17;
    numOfCols = colRight - colLeft + 1;

    bmpRow = 8;

    kkint32 padding = 16;

    colOffset = padding / 2;

    numOfCols = numOfCols + padding;
  }

  RasterPtr  r = new Raster (numOfRows, numOfCols, false);  // false = GrayScale image
  //r->BackgroundPixelValue (255);
  //r->ForegroundPixelValue (0);

  uchar** grayArea = r->Green ();

  for  (row = rowTop; row <= rowBot; row++)
  {
    bmpCol = colOffset;

    for  (col = colLeft; col <= colRight; col++)
    {
      if  (blobIds[row][col] == id)
        grayArea[bmpRow][bmpCol] = frame[row][col];
      bmpCol++;
    }

    bmpRow++;
  }

  return r;
} /* GetRaster */



void  SipperBlob::DialateBlob (uchar** frame,
                               kkint32**   blobIds,
                               kkint32   size
                              )
{
  rowTop   = Max (0, kkint32 (rowTop) - size);
  rowBot   = Min (maxFrameHeight - 1, rowBot + size);
  colLeft  = Max (0, kkint32 (colLeft) - size);
  colRight = Min (maxFrameWidth - 1, colRight + size);

  kkuint32  height = 1 + rowBot   - rowTop;
  kkuint32  width  = 1 + colRight - colLeft;

  kkuint32  totPixels = height * width;

  kkint32*  workArea = new kkint32[totPixels];
  kkint32** workRows = new kkint32*[height];

  kkuint32 row, col;
  kkuint32 workRow = 0;
  kkuint32 workCol = 0;
  kkint32  curWorkPixel = 0;
  kkint32* curWorkRow = NULL;

  kkint32* curRow = NULL;

  kkuint32  windowRow, windowCol;

  for  (row = rowTop;  row <= rowBot;  row++)
  {
    workRows[workRow] = curWorkRow = &(workArea[curWorkPixel]);
    curRow = blobIds[row];
    workCol = 0;
    
    for  (col = colLeft;  col <= colRight;  col++)
    {
      curWorkRow[workCol] = curRow[col];
      workCol++;
    }

    curWorkPixel += width;
    workRow++;
  }


  workRow = 0;
  for  (row = rowTop;  row <= rowBot;  row++)
  {
    curWorkRow = workRows[workRow];
    curRow = blobIds[row];
    workCol = 0;
    
    for  (col = colLeft;  col <= colRight;  col++)
    {
      if  (curWorkRow[workCol] >= 0)
      {
        // This pixel assigned to a blob already
        continue;
      }

      if  (frame[row][col] == 0)
      {
        // This is a background pixel anyway, it would not be assigned to a blob.
        continue;
      }

      // Next lets determine if this pixel will be the recipient of dilation.
      bool  dialatePixel = false;
      for  (windowRow = Max ((kkint32 (workRow) - size), 0);  (windowRow < (workRow + size))  &&  (!dialatePixel);  windowRow++)
      {
        if  (windowRow >= height)
          break;

        for  (windowCol = Max (0, (kkint32 (workCol) - size));  windowCol < (workCol + size);  windowCol++)
        {
          if  (windowCol >= width)
            break;

          if  (workRows[windowRow][windowCol] == id)
          {
            dialatePixel = true;
            pixelCount++;
            blobIds[row][col] = id;
            break;
          }
        }
      }

      workCol++;
    }

    workRow++;
  }

  delete  workArea;
  delete  workRows;
}  /* DialateBlob */




void  SipperBlob::AddNeighbor (SipperBlobPtr  _neighbor)
{
  kkint32  neighborId = _neighbor->Id ();
  if  (neighborId == neighborIdLastAdded)
    return;

  neighbors.insert (pair<kkint32, SipperBlobPtr> (neighborId, _neighbor));
  neighborIdLastAdded = neighborId;
}  /* AddNeighbor */

} /* MLL */
