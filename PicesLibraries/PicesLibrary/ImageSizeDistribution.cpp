#include "FirstIncludes.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include "MemoryDebug.h"
using namespace std;


#include "BasicTypes.h"
#include "KKException.h"
using namespace  KKU;


#include "ImageSizeDistribution.h"
using namespace  MLL;







ImageSizeDistribution::ImageSizeDistribution (float               _depthBinSize,
                                              float               _initialValue,
                                              float               _growtRate,
                                              float               _endValue,
                                              const VectorFloat&  _sizeStartValues,
                                              const VectorFloat&  _sizeEndValues,
                                              RunLog&             _runLog
                                             ):

    depthBinSize      (_depthBinSize),
    initialValue      (_initialValue),
    growthRate        (_growtRate),
    endValue          (_endValue),
    numSizeBuckets    (0),
    sizeStartValues   (_sizeStartValues),
    sizeEndValues     (_sizeEndValues),
    sizeDistribution  (),
    depths            (),
    imageCounts       (),
    pixelCounts       ()
{
  numSizeBuckets = sizeStartValues.size ();
  if  (sizeStartValues.size () != sizeEndValues.size ())
  {
    KKStr  errMsg = "Size of 'sizeStartValues' and 'sizeEndValues'  don't match.";
    _runLog.Level (-1) << endl << "ImageSizeDistribution   ***ERROR***   " << errMsg << endl << endl;
    throw KKException (errMsg);
  }
}





void  ImageSizeDistribution::DefineRow (float    _depth,
                                        kkint32  _imageCount,
                                        kkint32  _totalPixelCount
                                       )
{
  kkuint32  depthIdx = (int)(_depth / depthBinSize);

  while  (depths.size () <= depthIdx)
  {
    kkint32  x = depths.size ();
    depths.push_back ((x + 1) * depthBinSize);
    sizeDistribution.push_back (VectorInt32 (numSizeBuckets, 0));
    imageCounts.push_back (0);
    pixelCounts.push_back (0);
  }
  imageCounts[depthIdx] = _imageCount;
  pixelCounts[depthIdx] = _totalPixelCount;
}


 


 
void  ImageSizeDistribution::AddData  (kkint32  _sizeBucketIdx,
                                       kkint32  _count
                                      )
{
  kkint32  x = depths.size () - 1;
  sizeDistribution[x][_sizeBucketIdx] += _count;
}
                    
