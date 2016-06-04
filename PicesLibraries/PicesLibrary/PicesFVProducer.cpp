/* CameraAcquisition.cpp -- Base class for the Acquisition threads.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <queue>
#include <map>
#include <vector>
#if  defined(WIN32)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include "Blob.h"
#include "ConvexHull.h"
#include "KKBaseTypes.h"
#include "GlobalGoalKeeper.h"
#include "Raster.h"
#include "RunLog.h"
using namespace KKB;


#include "FactoryFVProducer.h"
#include "FeatureVectorProducer.h"
#include "FileDesc.h"
#include "PicesTrainingConfiguration.h"
using namespace  KKMLL;


#include "PicesFVProducer.h"
#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
using  namespace  MLL;




PicesFVProducer::PicesFVProducer (FactoryFVProducerPtr  factory):
    FeatureVectorProducer ("PostLarvaeFV",
                           factory
                          )
{
}



PicesFVProducer::~PicesFVProducer ()
{
}



ImageFeaturesPtr  PicesFVProducer::ComputeFeatureVector (const Raster&     srcImage,
                                                         const MLClassPtr  knownClass,
                                                         RasterListPtr     intermediateImages,
                                                         float             priorReductionFactor,
                                                         RunLog&           runLog
                                                        )
{
  ImageFeaturesPtr  fv = NULL;
  RasterSipperListPtr  sipperRasterIntermediateImages = NULL;
  if  (intermediateImages)
    sipperRasterIntermediateImages = new RasterSipperList (false);

  if  (typeid (srcImage) != typeid (MLL::RasterSipper))
  {
    runLog.Level (-1) << endl
      << "PicesFVProducer::ComputeFeatureVector   ***ERROR***   'srcImage' not a 'RasterSipper' derived class." << endl
      << "   ExampleFileName[" << srcImage.FileName () << "]" << endl
      << endl;

    RasterSipperPtr sipperRaster = new RasterSipper (srcImage);
    fv = new ImageFeatures (*sipperRaster, knownClass, sipperRasterIntermediateImages, runLog);
    delete  sipperRaster;
    sipperRaster = NULL;
  }
  else
  {
    MLL::RasterSipperPtr  sipperImage = dynamic_cast<MLL::RasterSipperPtr> (&(Raster&)srcImage);
    fv = new ImageFeatures (*sipperImage, knownClass, sipperRasterIntermediateImages, runLog);
  }

  if  (sipperRasterIntermediateImages)
  {
    for  (auto intermdiateIDX: *sipperRasterIntermediateImages)
      intermediateImages->PushOnBack (intermdiateIDX);
    sipperRasterIntermediateImages->Owner (false);
  }

  delete  sipperRasterIntermediateImages;
  sipperRasterIntermediateImages = NULL;

  return fv;
}  /* ComputeFeatureVector */



ImageFeaturesPtr  PicesFVProducer::ComputeFeatureVectorFromImage (const KKStr&      fileName,
                                                                  const MLClassPtr  knownClass,
                                                                  RasterListPtr     intermediateImages,
                                                                  RunLog&           runLog
                                                                 )
{
  ImageFeaturesPtr  fv = NULL;

  bool  validFile = false;
  RasterSipperPtr  i = new RasterSipper (fileName, validFile);
  if  (!validFile)
  {
    delete i;
    i = NULL;
  }

  if  (i == NULL)
  {
    runLog.Level (-1) << "PicesFVProducer::ComputeFeatureVectorFromImage   ***ERROR***   Error loading ImageFile: " << fileName << endl << endl;
  }
  else
  {
    fv = ComputeFeatureVector (*i, knownClass, intermediateImages, 1.0f, runLog);
    delete  i;
    i = NULL;
  }

  return  fv;
}  /* ComputeFeatureVectorFromImage */



const type_info*   PicesFVProducer::FeatureVectorTypeId () const
{
  return  &(typeid (ImageFeatures));
}



const type_info*  PicesFVProducer::FeatureVectorListTypeId () const
{
  return  &(typeid (ImageFeaturesList));
}



kkint32  PicesFVProducer::MaxNumOfFeatures  ()  
{
  return FeatureFileIOPices::PlanktonMaxNumOfFields ();
}



FileDescPtr  PicesFVProducer::DefineFileDesc ()  const
{
  return  DefineFileDescStatic ();
}  /* DefineFileDesc */



FileDescPtr  PicesFVProducer::DefineFileDescStatic ()
{
  return  FeatureFileIOPices::NewPlanktonFile ();
}



FeatureVectorListPtr  PicesFVProducer::ManufacturFeatureVectorList (bool     owner,
                                                                    RunLog&  runLog
                                                                   )
                                                                   const
{
  return  new ImageFeaturesList (FileDesc (), owner);
}



PicesFVProducerFactory::PicesFVProducerFactory ():
   FactoryFVProducer("PicesFVProducerFactory", "ImageFeatures", "ImageFeatures")
{
}



PicesFVProducerFactory::~PicesFVProducerFactory ()
{
}



FeatureFileIOPtr  PicesFVProducerFactory::DefaultFeatureFileIO ()  const
{
  return  FeatureFileIOPices::Driver ();
}



const type_info*  PicesFVProducerFactory::FeatureVectorTypeId ()  const  
{
  return  &typeid (ImageFeatures);
}



const type_info*  PicesFVProducerFactory::FeatureVectorListTypeId  () const  
{
  return  &typeid (ImageFeaturesList);
}



FileDescPtr  PicesFVProducerFactory::FileDesc ()  const
{
  return  PicesFVProducer::DefineFileDescStatic ();
}



PicesFVProducerPtr  PicesFVProducerFactory::ManufactureInstance (RunLog&  runLog)
{
  return new PicesFVProducer (this);
} /* ManufactureInstance */



ImageFeaturesListPtr  PicesFVProducerFactory::ManufacturFeatureVectorList (bool     owner,
                                                                           RunLog&  runLog
                                                                          )
                                                                          const
{
  return new ImageFeaturesList (PicesFVProducer::DefineFileDescStatic (), owner);
}



TrainingConfiguration2Ptr  PicesFVProducerFactory::ManufacturTrainingConfiguration ()  const
{
  return new PicesTrainingConfiguration ();
}



PicesFVProducerFactory*  PicesFVProducerFactory::factory = Factory (NULL);



PicesFVProducerFactory*  PicesFVProducerFactory::Factory (RunLog*  runLog)
{
  GlobalGoalKeeper::StartBlock ();

  if  (factory == NULL)
  {
    factory = new PicesFVProducerFactory ();
    FactoryFVProducer::RegisterFactory (factory, runLog);
  }
  GlobalGoalKeeper::EndBlock ();

  return  factory;
}

