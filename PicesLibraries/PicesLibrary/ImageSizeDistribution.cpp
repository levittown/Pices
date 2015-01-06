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



ImageSizeDistributionRow::ImageSizeDistributionRow (float     _depth,
                                                    kkuint32  _numSizeBuckets,
                                                    kkuint32  _imageCount,
                                                    kkuint32  _totalPixels
                                                   ):
   depth        (_depth),
   distribution (_numSizeBuckets, 0),
   imageCount   (_imageCount),
   totalPixels  (_totalPixels)
{
}


void  ImageSizeDistributionRow::AddData (kkuint32  _sizeIdx,
                                         kkuint32  _count
                                        )
{
  while  (_sizeIdx >= distribution.size ())
    distribution.push_back (0);
  distribution[_sizeIdx] = _count;
}  /* AddData */







ImageSizeDistribution::ImageSizeDistribution (float               _depthBinSize,
                                              float               _initialValue,
                                              float               _growtRate,
                                              float               _endValue,
                                              const VectorFloat&  _sizeStartValues,
                                              const VectorFloat&  _sizeEndValues,
                                              RunLog&             _runLog
                                             ):

    depthBinSize       (_depthBinSize),
    initialValue       (_initialValue),
    growthRate         (_growtRate),
    endValue           (_endValue),
    numSizeBuckets     (0),
    sizeStartValues    (_sizeStartValues),
    sizeEndValues      (_sizeEndValues),
    depthDistributions ()
{
  numSizeBuckets = sizeStartValues.size ();
  if  (sizeStartValues.size () != sizeEndValues.size ())
  {
    KKStr  errMsg = "Size of 'sizeStartValues' and 'sizeEndValues'  don't match.";
    _runLog.Level (-1) << endl << "ImageSizeDistribution   ***ERROR***   " << errMsg << endl << endl;
    throw KKException (errMsg);
  }
}



void  ImageSizeDistribution::PopulateDistributions (kkuint32  depthIdx)
{
  while  (depthDistributions.size () <= depthIdx)
  {
    kkuint32  idx = depthDistributions.size ();
    depthDistributions.push_back (new ImageSizeDistributionRow (idx * depthBinSize, numSizeBuckets, 0, 0));
  }
}  /* PopulateDistributions */




void  ImageSizeDistribution::DefineRow (float     _depth,
                                        kkuint32  _imageCount,
                                        kkuint32  _totalPixelCount
                                       )
{
  kkuint32  depthIdx = (int)(_depth / depthBinSize);
  PopulateDistributions (depthIdx);
  ImageSizeDistributionRowPtr  row = depthDistributions[depthIdx];
  row->ImageCount  (_imageCount);
  row->TotalPixels (_totalPixelCount);
}  /* DefineRow */





 
void  ImageSizeDistribution::AddData  (float     _depth,
                                       kkuint32  _sizeBucketIdx,
                                       kkuint32  _count
                                      )
{
  kkuint32  depthIdx = (kkuint32)(_depth / depthBinSize);
  PopulateDistributions (depthIdx);
  ImageSizeDistributionRowPtr  row = depthDistributions[depthIdx];
  row->AddData (_sizeBucketIdx, _count);
}
                    
