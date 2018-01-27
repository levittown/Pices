#include "FirstIncludes.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "KKException.h"
using namespace  KKB;


#include "ImageSizeDistribution.h"
using namespace  MLL;



ImageSizeDistributionRow::ImageSizeDistributionRow (float     _depth,
                                                    kkuint32  _numSizeBuckets,
                                                    kkuint32  _imageCount,
                                                    kkuint32  _totalPixels,
                                                    kkuint32  _totalFilledArea,
                                                    float     _volumneSampled
                                                   ):
   depth           (_depth),
   distribution    (_numSizeBuckets, 0),
   imageCount      (_imageCount),
   totalPixels     (_totalPixels),
   totalFilledArea (_totalFilledArea),
   volumneSampled  (_volumneSampled)
{
}



ImageSizeDistributionRow::ImageSizeDistributionRow (const ImageSizeDistributionRow&  row):
   depth            (row.depth),
   distribution     (row.distribution),
   imageCount       (row.imageCount),
   totalPixels      (row.totalPixels),
   totalFilledArea  (row.totalFilledArea),
   volumneSampled   (row.volumneSampled)
{
  
}



void  ImageSizeDistributionRow::AddImageCountPixelCount (kkuint32  _imageCount,
                                                         kkuint32  _pixelCount,
                                                         kkuint32  _filledArea,
                                                         float     _volumneSampled
                                                        )
{
  imageCount       += _imageCount;
  totalPixels      += _pixelCount;
  totalFilledArea  += _filledArea;
  volumneSampled   += _volumneSampled;
}  /* AddImageCountPixelCount */



void  ImageSizeDistributionRow::AddData (kkuint32  _sizeIdx,
                                         kkuint32  _count
                                        )
{
  while  (_sizeIdx >= distribution.size ())
    distribution.push_back (0);
  distribution[_sizeIdx] += _count;
}  /* AddData */



void  ImageSizeDistributionRow::AddIn (const ImageSizeDistributionRow&  right)
{
  auto  xL = distribution.size ();
  auto  xR = right.distribution.size ();

  auto  xM = Min (xL, xR);

  for  (auto  x = 0;  x < xM;  ++x)
    distribution[x] += right.distribution[x];

  imageCount      += right.imageCount;
  totalPixels     += right.totalPixels;
  totalFilledArea += right.totalFilledArea;
  volumneSampled  += right.volumneSampled;
}  /* AddIn */



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
    depthDistributions (),
    allDepths          (NULL)
{
  numSizeBuckets = (kkuint32)sizeStartValues.size ();
  if  (sizeStartValues.size () != sizeEndValues.size ())
  {
    KKStr  errMsg = "Size of 'sizeStartValues' and 'sizeEndValues'  don't match.";
    _runLog.Level (-1) << endl << "ImageSizeDistribution   ***ERROR***   " << errMsg << endl << endl;
    throw KKException (errMsg);
  }
  allDepths = new ImageSizeDistributionRow (-1.0f, numSizeBuckets, 0, 0, 0, 0.0f);
}



ImageSizeDistribution::~ImageSizeDistribution ()
{
  for  (kkuint32  x = 0;  x < depthDistributions.size ();  ++x)
  {
    delete  depthDistributions[x];
    depthDistributions[x] = NULL;
  }
  delete  allDepths;
  allDepths = NULL;
}



/**
 *@brief  Returns pointer to ImageSizeDistributionRow  that is index 'depthBinIdx'.
 *@details  Caller will not own the entry;  so do not delete it!
 */
ImageSizeDistributionRowPtr  ImageSizeDistribution::GetDepthBin (kkuint32  depthBinIdx)
{
  if  (depthBinIdx >= depthDistributions.size ())
    return NULL;
  else
    return depthDistributions[depthBinIdx];
}



void  ImageSizeDistribution::PopulateDistributions (kkuint32  depthIdx)
{
  while  (depthDistributions.size () <= depthIdx)
  {
    auto  idx = depthDistributions.size ();
    depthDistributions.push_back (new ImageSizeDistributionRow (idx * depthBinSize, numSizeBuckets, 0, 0, 0, 0.0f));
  }
}  /* PopulateDistributions */



void  ImageSizeDistribution::DefineRow (float     _depth,
                                        kkuint32  _imageCount,
                                        kkuint32  _totalPixelCount,
                                        kkuint32  _totalFilledArea,
                                        float     _volumneSampled
                                       )
{
  kkuint32  depthIdx = (int)(_depth / depthBinSize);
  PopulateDistributions (depthIdx);
  ImageSizeDistributionRowPtr  row = depthDistributions[depthIdx];
  row->ImageCount      (_imageCount);
  row->TotalPixels     (_totalPixelCount);
  row->TotalFilledArea (_totalFilledArea);
  row->VolumneSampled  (_volumneSampled);
  allDepths->AddImageCountPixelCount (_imageCount, _totalPixelCount, _totalFilledArea, _volumneSampled);
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
  allDepths->AddData (_sizeBucketIdx, _count);
}
                    

void  ImageSizeDistribution::AddIn (const ImageSizeDistribution&  right,
                                    RunLog&                       log
                                   )
{
  if  (numSizeBuckets != right.numSizeBuckets)
  {
    KKStr  errMsg (128);
    errMsg << "ImageSizeDistribution::AddIn   ***ERROR***  SizeBuckets Difference   left[" << numSizeBuckets << "]  right[" << right.numSizeBuckets << "].";
    log.Level (-1) << endl << errMsg << endl << endl;
    throw KKException (errMsg);
  }

  if  (this->numSizeBuckets != right.numSizeBuckets)
  {
    KKStr  errMsg (128);
    errMsg << "ImageSizeDistribution::AddIn   ***ERROR***  SizeBuckets Difference   left[" << numSizeBuckets << "]  right[" << right.numSizeBuckets << "].";
    log.Level (-1) << endl << errMsg << endl << endl;
    throw KKException (errMsg);
  }

  if  (depthDistributions.size () < right.depthDistributions.size ())
    PopulateDistributions ((kkuint32)right.depthDistributions.size ());

  for  (kkuint32 x = 0;  x < right.depthDistributions.size ();  ++x)
  {
    depthDistributions[x]->AddIn (*(right.depthDistributions[x]));
    allDepths->AddIn (*(right.depthDistributions[x]));
  }
}  /* AddIn */



VectorFloat  ImageSizeDistribution::DepthProfileForSizeBin (kkuint32  _sizeBucketIdx)
{
  VectorFloat result (NumDepthBins (), 0.0f);
  for  (kkuint32  depthIdx = 0;  depthIdx < NumDepthBins ();  ++depthIdx)
  {
    ImageSizeDistributionRowPtr row = depthDistributions[depthIdx];
    const VectorUint32&  distribution = row->Distribution ();
    if  (_sizeBucketIdx < distribution.size ())
    {
      if  (row->VolumneSampled () != 0.0)
        result[depthIdx] = distribution[_sizeBucketIdx] / row->VolumneSampled ();
    }
  }
  return  result;
}  /* DepthProfileForSizeBin */



VectorFloat  ImageSizeDistribution::VolumeSampledByDepthBucket ()
{
  VectorFloat result (depthDistributions.size (), 0.0f);
  for  (kkuint32  depthIdx = 0;  depthIdx < depthDistributions.size ();  ++depthIdx)
  {
    ImageSizeDistributionRowPtr row = depthDistributions[depthIdx];
    const VectorUint32&  distribution = row->Distribution ();
    result[depthIdx] = row->VolumneSampled ();
  }
  return  result;
}  /* DepthProfileForSizeBin */



void  ImageSizeDistribution::GetSizeBucketStats (kkuint32   _sizeBucketIdx,
                                                 kkuint32&  _count,
                                                 float&     _sizeStart,
                                                 float&     _sizeEnd
                                                )
{
  _count     = 0;
  _sizeStart = 0.0f;
  _sizeEnd   = 0.0f;
  if  ((_sizeBucketIdx < 0)  ||  (_sizeBucketIdx >= numSizeBuckets))
    return;

  if  (_sizeBucketIdx < sizeStartValues.size ())
    _sizeStart = sizeStartValues[_sizeBucketIdx];

  if  (_sizeBucketIdx < sizeEndValues.size ())
    _sizeEnd = sizeEndValues[_sizeBucketIdx];

  if  (_sizeBucketIdx < allDepths->Distribution ().size ())
    _count = allDepths->Distribution ()[_sizeBucketIdx];
}  /* GetSizeBucketStats */



kkint32  ImageSizeDistribution::IdentifySizeBucket (float  size)
{
  if  (sizeEndValues.size () < 1)
    return -1;
  if  ((size < sizeStartValues[0])  ||  (size > sizeEndValues[sizeEndValues.size () - 1]))
    return -1;

  kkuint32  left = 0;
  kkuint32  right = (kkuint32)sizeStartValues.size () - 1;
  kkuint32  middle = 0;
  while  (left <= right)
  {
    middle = (left + right) / 2;
    if  (size < sizeStartValues[middle])
      right = middle - 1;
    else if   (size >= sizeEndValues[middle])
      left = middle + 1;

    else
      return  (kkint32)middle;
  }

  return -1;
}  /* IdentifySizeBucket */



VectorFloat   ImageSizeDistribution::IntegratedDensityDistribution ()  const
{
  auto  numSizeBuckets = sizeStartValues.size ();
  VectorFloat  results (numSizeBuckets, 0.0);

  for  (auto  depthIdx = 0;   depthIdx < depthDistributions.size ();  ++depthIdx)
  {
    ImageSizeDistributionRowPtr  dr = depthDistributions[depthIdx];
    float   volSampled = dr->VolumneSampled ();
    if  (volSampled != 0.0f)
    {
      const VectorUint32&  distribution =  dr->Distribution ();
      auto zed = Min (numSizeBuckets, distribution.size ());
      for  (kkuint32 x = 0;  x < zed;  ++x)
      {
        results[x] += distribution[x] / volSampled;
      }
    }
  }
  return  results;
}  /* IntegratedDensityDistribution */


