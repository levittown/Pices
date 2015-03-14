#include  "FirstIncludes.h"
#include  <fstream>
#include  <iostream>
#include  <map>
#include  <vector>

#include  "MemoryDebug.h"

using namespace  std;


#include "KKBaseTypes.h"
#include "RasterSipper.h"
#include "KKStr.h"
using namespace  KKB;



#include "LogicalFrameBlob.h"
using namespace  ImageExtractionManager;



LogicalFrameBlob::LogicalFrameBlob (kkuint32  _maxFrameHeight,
                                    kkuint32  _maxFrameWidth
                                   ):
  id (-1),
  explored            (false),
  maxFrameHeight      (_maxFrameHeight),
  maxFrameWidth       (_maxFrameWidth),
  rowBot              (0),
  rowTop              (0),
  colLeft             (0),
  colRight            (0),
  neighborIdLastAdded (-1),
  neighbors           (),
  pixelCount          (0)
  
{
}


LogicalFrameBlob::~LogicalFrameBlob ()
{
  neighbors.clear ();
}



void  LogicalFrameBlob::DialateBlob (uchar**   frame,
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




void  LogicalFrameBlob::AddNeighbor (LogicalFrameBlobPtr  _neighbor)
{
  kkint32  neighborId = _neighbor->Id ();
  if  (neighborId == neighborIdLastAdded)
    return;

  neighbors.insert (pair<kkint32, LogicalFrameBlobPtr> (neighborId, _neighbor));
  neighborIdLastAdded = neighborId;
}  /* AddNeighbor */


