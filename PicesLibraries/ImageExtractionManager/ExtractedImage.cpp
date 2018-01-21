#include "FirstIncludes.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include "MemoryDebug.h"
using namespace  std;


#include "KKBaseTypes.h"
#include "RasterSipper.h"
#include "KKStr.h"
using namespace  KKB;


#include  "ExtractedImage.h"
using namespace  ImageExtractionManager;


ExtractedImage::ExtractedImage (RasterSipperPtr _image,
                                kkuint64        _byteOffsetFirstScanRow,
                                kkuint32        _sipperFileScanRow,
                                kkuint32        _sipperFileScanCol,
                                kkuint32        _pixelCount,
                                float           _imageArea,
                                float           _depth,
                                MLClassPtr      _predClass1,
                                float           _predClass1Prob,
                                MLClassPtr      _predClass2,
                                float           _predClass2Prob
                               ):
   image                  (_image),
   byteOffsetFirstScanRow (_byteOffsetFirstScanRow),
   featureVector          (NULL),
   imageArea              (_imageArea),
   sipperFileScanRow      (_sipperFileScanRow),
   sipperFileScanCol      (_sipperFileScanCol),
   pixelCount             (_pixelCount),
   depth                  (_depth),
   predClass1             (_predClass1),
   predClass1Prob         (_predClass1Prob),
   predClass2             (_predClass2),
   predClass2Prob         (_predClass2Prob)
{
}



ExtractedImage::~ExtractedImage ()
{
  delete  featureVector;  featureVector = NULL;
  delete  image;          image         = NULL;
}



kkMemSize  ExtractedImage::MemoryConsumedEstimated ()  const
{
  kkMemSize  m = sizeof (*this);
  if  (image)
    m += image->MemoryConsumedEstimated ();

  if  (featureVector)
    m += featureVector->MemoryConsumedEstimated ();

  return  m;
}



void  ExtractedImage::FeatureVector (ImageFeaturesPtr  _featureVector)
{
  if  (featureVector)
    delete  featureVector;
  featureVector = _featureVector;
}



void  ExtractedImage::Prediction (MLClassPtr  _predClass1,
                                  float       _predClass1Prob,
                                  MLClassPtr  _predClass2,
                                  float       _predClass2Prob
                                 )
{
  predClass1      = _predClass1;
  predClass1Prob  = _predClass1Prob;
  predClass2      = _predClass2;
  predClass2Prob  = _predClass2Prob;
}



ImageFeaturesPtr  ExtractedImage::FeatureVectorGiveOwnership ()
{
  ImageFeaturesPtr  fv = featureVector;
  featureVector = NULL;
  return  fv;
}  /* FeatureVectorGiveOwnership */



RasterSipperPtr  ExtractedImage::ImageGiveOwnership ()
{
  RasterSipperPtr  i = image;
  image = NULL;
  return  i;
}



/**@todo  need to make 'maxQueueSize' dynamic;  posibly factor in total of all contained image sizes. */
ExtractedImageQueue::ExtractedImageQueue ():
   goalie       (NULL),
   maxQueueSize (500)
{
  GoalKeeper::Create ("ExtractedImageQueue", goalie);
}



ExtractedImageQueue::~ExtractedImageQueue ()
{
  goalie->StartBlock ();
  while  (size () > 0)
  {
    ExtractedImagePtr  i = front ();
    pop_front ();

    delete  i;
    i = NULL;
  }
  goalie->EndBlock ();
  GoalKeeper::Destroy (goalie);
  goalie = NULL;
}



kkMemSize  ExtractedImageQueue::MemoryConsumedEstimated ()  const
{
  goalie->StartBlock ();
  kkMemSize  memoryConsumedEstimated = sizeof (*this);

  std::deque<ExtractedImagePtr>:: const_iterator idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ExtractedImagePtr  i = *idx;
    memoryConsumedEstimated += i->MemoryConsumedEstimated ();
  }
  goalie->EndBlock ();
  return  memoryConsumedEstimated;
}  /* MemoryConsumedEstimated */



void  ExtractedImageQueue::AddExtractedImage (ExtractedImagePtr  extractedImage)
{
  bool  added = false;
  while  (!added)
  {
    goalie->StartBlock ();
    if  (size () < maxQueueSize)
    {
      push_back (extractedImage);
      added = true;
    }
    goalie->EndBlock ();
    if  (!added)
      osSleepMiliSecs(10);
  }
}  /* AddExtractedImage */



void  ExtractedImageQueue::AddExtractedImages (VectorExtractedImage&  extractedImages)
{
  goalie->StartBlock ();

  VectorExtractedImage::const_iterator  idx;
  for  (idx = extractedImages.begin ();  idx != extractedImages.end ();  ++idx)
  {
    ExtractedImagePtr  i = *idx;
    push_back (i);
  }

  extractedImages.clear ();

  goalie->EndBlock ();
}  /* AddExtractedImages */



ExtractedImagePtr  ExtractedImageQueue::GetNextExtractedImage ()
{
  ExtractedImagePtr  nextImage = NULL;
  goalie->StartBlock ();
  if  (size () > 0)
  {
    nextImage = front ();
    pop_front ();
  }
  goalie->EndBlock ();
  return  nextImage;
}  /* GetNextExtractedImage */



kkuint32  ExtractedImageQueue::ImagesOnQueue ()
{
  kkuint32  imagesOnQueue = 0;
  goalie->StartBlock ();
  imagesOnQueue = (kkuint32)size ();
  goalie->EndBlock ();
  return  imagesOnQueue;
}
