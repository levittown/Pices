#include "FirstIncludes.h"
#include <ctype.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace  std;


#include "KKBaseTypes.h"
#include "BMPImage.h"
#include "ContourFollower.h"
#include "ConvexHull.h"
#include "DateTime.h"
#include "KKException.h"
#include "KKStr.h"
#include "ImageIO.h"
#include "ImageDirTree.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace  KKB;


#include "FeatureNumList.h"
#include "FeatureFileIO.h"
#include "FeatureVector.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "InstrumentData.h"
#include "InstrumentDataList.h"
#include "InstrumentDataFileManager.h"
#include "RasterSipper.h"
#include "PicesVariables.h"
#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
using namespace  MLL;



const kkint32  ImageFeatures::SizeThreshold = 100000;  /**< Size of image in number of foreground pixels before we decide to reduce the
                                                        * image to improve feature calculation.
                                                        */

kkint32  ImageFeatures::FirstInstrumentDataField = 84;  /**< see 'NumSeperateInstrumentDataFields' below.  */

/**
 * This is the number of fields that are not going to be stored in the regular feature file
 * but in separate instrument data files that will be accessed by SiperFile name and scan
 * line number.
 */
kkint32  ImageFeatures::NumSeperateInstrumentDataFields = 4;  

              
short  ImageFeatures::SizeIndex             = 0;
short  ImageFeatures::Moment1Index          = 1;
short  ImageFeatures::Moment2Index          = 2;
short  ImageFeatures::Moment3Index          = 3;
short  ImageFeatures::Moment4Index          = 4;
short  ImageFeatures::Moment5Index          = 5;
short  ImageFeatures::Moment6Index          = 6;
short  ImageFeatures::Moment7Index          = 7;

short  ImageFeatures::EdgeSizeIndex         = 8;
short  ImageFeatures::EdgeMoment1Index      = 9;
short  ImageFeatures::EdgeMoment2Index      = 10;  
short  ImageFeatures::EdgeMoment3Index      = 11;
short  ImageFeatures::EdgeMoment4Index      = 12;
short  ImageFeatures::EdgeMoment5Index      = 13;
short  ImageFeatures::EdgeMoment6Index      = 14;
short  ImageFeatures::EdgeMoment7Index      = 15;



short  ImageFeatures::TransparancyConvexHullIndex = 16;
short  ImageFeatures::TransparancyPixelCountIndex = 17;
short  ImageFeatures::TransparancyOpen3Index      = 18;
short  ImageFeatures::TransparancyOpen5Index      = 19;
short  ImageFeatures::TransparancyOpen7Index      = 20;
short  ImageFeatures::TransparancyOpen9Index      = 21;
short  ImageFeatures::TransparancyClose3Index     = 22;
short  ImageFeatures::TransparancyClose5Index     = 23;
short  ImageFeatures::TransparancyClose7Index     = 24;



short  ImageFeatures::EigenRatioIndex       = 25;
short  ImageFeatures::EigenHeadIndex        = 26;
short  ImageFeatures::ConvexAreaIndex       = 27;
short  ImageFeatures::TransparancySizeIndex = 28;
short  ImageFeatures::TransparancyWtdIndex  = 29;
short  ImageFeatures::WeighedMoment0Index   = 30;
short  ImageFeatures::WeighedMoment1Index   = 31;
short  ImageFeatures::WeighedMoment2Index   = 32;
short  ImageFeatures::WeighedMoment3Index   = 33;
short  ImageFeatures::WeighedMoment4Index   = 34;
short  ImageFeatures::WeighedMoment5Index   = 35;
short  ImageFeatures::WeighedMoment6Index   = 36;
short  ImageFeatures::WeighedMoment7Index   = 37;


short  ImageFeatures::Fourier0Index         = 38;
short  ImageFeatures::Fourier1Index         = 39;
short  ImageFeatures::Fourier2Index         = 40;
short  ImageFeatures::Fourier3Index         = 41;
short  ImageFeatures::Fourier4Index         = 42;


short  ImageFeatures::FdRight1Index          = 43;
short  ImageFeatures::FdLeft2Index           = 44;
short  ImageFeatures::FdRight2Index          = 45;
short  ImageFeatures::FdLeft3Index           = 46;
short  ImageFeatures::FdRight3Index          = 47;
short  ImageFeatures::FdLeft4Index           = 48;
short  ImageFeatures::FdRight4Index          = 49;
short  ImageFeatures::FdLeft5Index           = 50;
short  ImageFeatures::FdRight5Index          = 51;
short  ImageFeatures::FdLeft6Index           = 52;
short  ImageFeatures::FdRight6Index          = 53;
short  ImageFeatures::FdLeft7Index           = 54;
short  ImageFeatures::FdRight7Index          = 55;
short  ImageFeatures::FdLeft8Index           = 56;
short  ImageFeatures::FdRight8Index          = 57;



short  ImageFeatures::ContourFourierDescriptor0 = 58;
short  ImageFeatures::ContourFourierDescriptor1 = 59;
short  ImageFeatures::ContourFourierDescriptor2 = 60;
short  ImageFeatures::ContourFourierDescriptor3 = 61;
short  ImageFeatures::ContourFourierDescriptor4 = 62;


short  ImageFeatures::IntensityHist1Index   = 63;
short  ImageFeatures::IntensityHist2Index   = 64;
short  ImageFeatures::IntensityHist3Index   = 65;
short  ImageFeatures::IntensityHist4Index   = 66;
short  ImageFeatures::IntensityHist5Index   = 67;
short  ImageFeatures::IntensityHist6Index   = 68;
short  ImageFeatures::IntensityHist7Index   = 69;

short  ImageFeatures::LengthVsWidthIndex    = 70;
short  ImageFeatures::LengthIndex           = 71;
short  ImageFeatures::WidthIndex            = 72;
short  ImageFeatures::FilledAreaIndex       = 73;

short  ImageFeatures::FlowRate1Index        = 74;
short  ImageFeatures::FlowRate2Index        = 75;

short  ImageFeatures::IntensityHistWS0Index = 76;
short  ImageFeatures::IntensityHistWS1Index = 77;
short  ImageFeatures::IntensityHistWS2Index = 78;
short  ImageFeatures::IntensityHistWS3Index = 79;
short  ImageFeatures::IntensityHistWS4Index = 80;
short  ImageFeatures::IntensityHistWS5Index = 81;
short  ImageFeatures::IntensityHistWS6Index = 82;
short  ImageFeatures::IntensityHistWS7Index = 83;

short  ImageFeatures::DepthIndex            = 84;
short  ImageFeatures::SalinityIndex         = 85;
short  ImageFeatures::OxygenIndex           = 86;
short  ImageFeatures::FluorescenceIndex     = 87;



ImageFeatures::ImageFeatures (kkint32  _numOfFeatures):
       FeatureVector (_numOfFeatures),
        areaMMSquare     (0.0f),
        centroidCol      (-1),
        centroidRow      (-1),
        ctdDateTime      (),
        latitude         (0.0),
        longitude        (0.0),
        numOfEdgePixels  (-1),
        sfCentroidCol    (-1.0f),
        sfCentroidRow    (-1.0),
        version          (0)
{
}



ImageFeatures::ImageFeatures (const ImageFeatures&  _image):
  FeatureVector (_image),
  areaMMSquare     (_image.areaMMSquare),
  centroidCol      (_image.centroidCol),
  centroidRow      (_image.centroidRow),
  ctdDateTime      (_image.ctdDateTime),
  latitude         (_image.latitude),
  longitude        (_image.longitude),
  numOfEdgePixels  (_image.numOfEdgePixels),
  sfCentroidCol    (_image.sfCentroidCol),
  sfCentroidRow    (_image.sfCentroidRow),
  version          (_image.version)

{
}




ImageFeatures::ImageFeatures (const BmpImage&      _image,
                              MLClassPtr           _mlClass,
                              RasterSipperListPtr  _saveImages,
                              RunLog&              _log
                             ):
  FeatureVector (FeatureFileIOPices::PlanktonMaxNumOfFields ()),
  areaMMSquare     (0.0f),
  centroidCol      (-1),
  centroidRow      (-1),
  ctdDateTime      (),
  latitude         (0.0),
  longitude        (0.0),
  numOfEdgePixels  (-1),
  sfCentroidCol    (-1.0f),
  sfCentroidRow    (-1.0),
  version          (0)

{
  MLClass       (_mlClass);
  ExampleFileName (_image.FileName ());

  RasterSipperPtr raster = new RasterSipper (_image);
  CalcFeatures (*raster, _saveImages, _log);
  delete  raster;
}




ImageFeatures::ImageFeatures (RasterSipper&        _raster,
                              MLClassPtr           _mlClass,
                              RasterSipperListPtr  _saveImages,
                              RunLog&              _log
                             ):
  FeatureVector (FeatureFileIOPices::PlanktonMaxNumOfFields ()),

  areaMMSquare     (0.0f),
  centroidCol      (-1),
  centroidRow      (-1),
  ctdDateTime      (),
  latitude         (0.0),
  longitude        (0.0),
  numOfEdgePixels  (-1),
  sfCentroidCol    (-1.0f),
  sfCentroidRow    (-1.0),
  version          (0)

{
  MLClass    (_mlClass);
  ExampleFileName (_raster.FileName ());

  KKStr  imageRootName = osGetRootName (_raster.FileName ()).ToUpper ();
  if  (imageRootName == "STATION1C_04_00474734_2979")
  {
    cout << "We are here." << imageRootName << endl;
  }


  CalcFeatures  (_raster, _saveImages, _log);
}



ImageFeatures::ImageFeatures (KKStr                _fileName,
                              MLClassPtr           _mlClass,
                              bool&                _successfull,
                              RasterSipperListPtr  _saveImages,
                              RunLog&              _log
                             ):

FeatureVector (FeatureFileIOPices::PlanktonMaxNumOfFields ()),
  areaMMSquare     (0.0f),
  centroidCol      (-1),
  centroidRow      (-1),
  ctdDateTime      (),
  latitude         (0.0),
  longitude        (0.0),
  numOfEdgePixels  (-1),
  sfCentroidCol    (-1.0f),
  sfCentroidRow    (-1.0),
  version          (0)

{
  MLClass         (_mlClass);
  ExampleFileName (_fileName);

  KKStr  ext = osGetFileExtension (_fileName);
  ext.Upper ();

  RasterSipperPtr  raster = NULL;

  if  (ext == "BMP")
  {
    BmpImagePtr image = new BmpImage (_fileName, _successfull);

    if  (_successfull)
    {
      if  (image->AreThereEdgePixels ())
      {
        // There are pixels along the edge of the image,  To help avoid 
        // problems with feature calculation, we will pad the image with 
        // a few extra row's and columns.
        image->ReAllocateForBiggerScreen ();
        image->Save (_fileName);
      }
      raster = new RasterSipper (*image);
    }
    delete  image;
  }
  else
  {
    raster = new RasterSipper (_fileName, _successfull);
  }

  if  (raster)
  {
    raster->ConnectedComponent (3);
    CalcFeatures (*raster, _saveImages, _log);
    delete  raster;
  }
  else
  {
    kkint32  x;
    _successfull = false;

    for  (x = 0; x < this->NumOfFeatures (); x++)
      featureData[x] = 0;

    cerr  << "ImageFeatures::ImageFeatures  ***ERROR***, Opening File[" << _fileName << "]." << endl;
  }
}



ImageFeatures::ImageFeatures (const FeatureVector&  featureVector):
   FeatureVector    (featureVector),
   areaMMSquare     (0.0f),
   centroidCol      (-1),
   centroidRow      (-1),
   ctdDateTime      (),
   latitude         (0.0),
   longitude        (0.0),
   numOfEdgePixels  (-1),
   sfCentroidCol    (-1.0f),
   sfCentroidRow    (-1.0),
   version          (0)

{
  if  (typeid (featureVector) == typeid (ImageFeatures))
  {
    // The underlying class is another ImageFeatures object.
    const ImageFeatures&  image = dynamic_cast<const ImageFeatures&>(featureVector);

    areaMMSquare     = image.AreaMMSquare    ();
    centroidCol      = image.CentroidCol     ();
    centroidRow      = image.CentroidRow     ();
    ctdDateTime      = image.CtdDateTime     ();
    latitude         = image.Latitude        ();
    longitude        = image.Longitude       ();
    numOfEdgePixels  = image.NumOfEdgePixels ();
    sfCentroidCol    = image.SfCentroidCol   ();
    sfCentroidRow    = image.SfCentroidRow   ();
    version          = image.Version         ();
  }
}



ImageFeatures::~ImageFeatures ()
{
}


kkint32  ImageFeatures::MemoryConsumedEstimated ()  const
{
  kkint32  memoryConsumedEstimated = FeatureVector::MemoryConsumedEstimated () 
    +  sizeof (float)   * 3
    +  sizeof (double)  * 3
    +  sizeof (kkint32)   * 1
    +  sizeof (short)   * 1;

  return  memoryConsumedEstimated;
}



ImageFeaturesPtr  ImageFeatures::Duplicate ()  const
{
  return new ImageFeatures (*this);
}



void  ImageFeatures::ResetVersion (short  newVersion)
{
  Version (newVersion);
}  /* ResetVersionMum */



bool   ImageFeatures::atExitDefined = false;


void  ImageFeatures::FinalCleanUp ()
{
}  /* FinalCleanUp */





void  ImageFeatures::Save (RasterSipper&        raster, 
                           const KKStr&         desc,
                           RasterSipperListPtr  saveImages
                          )
{
  if  (!saveImages)
    return;

  RasterSipperPtr  newRaster = NULL;

  kkint32  largestDim = Max (raster.Height (), raster.Width ());
  if  (largestDim < 300)
  {
    newRaster = new RasterSipper (raster);
  }
  else
  {
    kkint32  reductionMultiple = 2;
    while  ((largestDim / reductionMultiple) > 300)
     reductionMultiple++;
    newRaster = raster.ReduceByEvenMultiple (reductionMultiple);
  }

  newRaster->FileName (desc);
  saveImages->PushOnBack (newRaster);

  //KKStr newName (baseName);
  //newName << "_" << desc << ".bmp";
  //
  //BmpImagePtr  bmpImage = new BmpImage (raster);
  //bmpImage->Save (newName);
  //delete  bmpImage;
}



//#define  DEBUB_CalcFeatures

void  ImageFeatures::CalcFeatures (RasterSipper&        srcRaster,
                                   RasterSipperListPtr  saveImages,
                                   RunLog&              log
                                  )
{
  #if defined (DEBUB_CalcFeatures)
  cout << std::endl
       << std::endl
       << "ImageFeatures::CalcFeatures" << std::endl;
  #endif

  if  (NumOfFeatures () != FeatureFileIOPices::PlanktonMaxNumOfFields ())
  {
    numOfFeatures = FeatureFileIOPices::PlanktonMaxNumOfFields ();
    AllocateFeatureDataArray ();
  }


  #if defined (DEBUB_CalcFeatures)
  cout << "CalcFeatures   After 'AllocateFeatureDataArray'" << std::endl;
  #endif

  version = CurrentFeatureFileVersionNum;
  kkuint32  intensityHistBuckets[8];
  kkuint32  intensityHistBucketsB[8];  // These buckets will include white space

  bool  weOwnRaster = false;
  RasterSipperPtr  reversedImage = NULL;

  if  (srcRaster.BackgroundPixelValue () > srcRaster.ForegroundPixelValue ())
  {
    weOwnRaster = true;
    reversedImage = srcRaster.ReversedImage ();
  }

  RasterSipperPtr raster = (weOwnRaster ? reversedImage : &srcRaster);

  kkint32 areaBeforeReduction;
  kkint32 areaWithWhiteSpace = 0;
  FVFloat  weighedSizeBeforeReduction    = 0.0f;
 
  raster->CalcAreaAndIntensityFeatures (areaBeforeReduction, 
                                        weighedSizeBeforeReduction,
                                        intensityHistBuckets,
                                        areaWithWhiteSpace, 
                                        intensityHistBucketsB
                                       );


  #if defined (DEBUB_CalcFeatures)
  cout << "CalcFeatures   After 'CalcAreaAndIntensityFeatures'" << std::endl;
  #endif

  kkint32  reductionFactor = 1;
  kkint32  reductionFactorSquared = 1;

  if  (areaBeforeReduction > SizeThreshold)
  {
    kkint32  reducedMinDim = Min (raster->Height (), raster->Width ());

    kkint32  reducedSize = areaBeforeReduction;
    reductionFactor = 1;
    while  ((reducedSize > SizeThreshold)  &&  (reducedMinDim > 20))
    {
      reductionFactor = reductionFactor + 1;
      reductionFactorSquared = reductionFactor * reductionFactor;
      reducedSize = areaBeforeReduction / reductionFactorSquared;
      reducedMinDim = Min (raster->Height (), raster->Width ()) / reductionFactor;
    }

    RasterSipperPtr  reducedRaster = raster->ReduceByEvenMultiple (reductionFactor);

    if  (saveImages)
      Save (*reducedRaster, "Reduced_" + StrFormatInt (reductionFactor, "ZZZZZZ0"), saveImages);

    if  (weOwnRaster)
    {
      delete  raster;
      raster = NULL;
    }

    raster = reducedRaster;
    weOwnRaster = true;
  }

  if  (areaBeforeReduction < 20)
  {
    for  (kkint32 tp = 0; tp < numOfFeatures; tp++)
      featureData[tp] = 9999999;
    
    if  (weOwnRaster)
    {
      delete  raster;
      raster = NULL;
    }
    return;
  }
  
  KKStr  driveLetter, path, root, extension;

  FVFloat  fourierFeatures[5];

  FVFloat  convexf = 0.0;

  osParseFileSpec (ExampleFileName (), driveLetter, path, root, extension);
  KKStr  baseName = "c:\\Temp\\TestImages2\\" + root + "\\" + root;

  if  (saveImages)
  {
    //osCreateDirectoryPath ("c:\\Temp\\TestImages2\\" + root);
    Save (*raster, "Original_" + StrFormatInt (areaBeforeReduction, "ZZZZZZ0"), saveImages);
  }

  //raster->ConnectedComponent ();
  //Save (*raster, baseName, "ConnectedComponent", saveImage);


  // Baishali added eccentricity moments calculations.
  float  momentf[9];
  raster->CentralMoments (momentf);

  FVFloat  momentfWeighted[9];
  raster->CentralMomentsWeighted (momentfWeighted);

  #if defined (DEBUB_CalcFeatures)
  cout << "CalcFeatures   After Moments and Weighted Moments Calculated." << std::endl;
  #endif



  RasterSipperPtr  edgeImage = new RasterSipper (*raster);

  edgeImage->Dilation ();
  //if  (saveImages)
  //{
  //  kkint32  a = (kkint32)edgeImage->CalcArea ();
  //  Save (*edgeImage, "Edge_Dialation_1_" + StrFormatInt (a, "ZZZZZZ0"), saveImages);
  //}

  edgeImage->Dilation ();
  //if  (saveImages)
  //{
  //  kkint32  a = (kkint32)edgeImage->CalcArea ();
  //  Save (*edgeImage, "Edge_Dialation_2_" + StrFormatInt (a, "ZZZZZZ0"), saveImages);
  //}

  edgeImage->FillHole ();
  //if  (saveImages)
  //{
  //  kkint32  a = (kkint32)edgeImage->CalcArea ();
  //  Save (*edgeImage, "Edge_FillHole_" + StrFormatInt (a, "ZZZZZZ0"), saveImages);
  //}

  edgeImage->Erosion ();
  //if  (saveImages)
  //{
  //  kkint32  a = (kkint32)edgeImage->CalcArea ();
  //  Save (*edgeImage, "Edge_Erosion_1_" + StrFormatInt (a, "ZZZZZZ0"), saveImages);
  //}

  edgeImage->Erosion ();
  if  (saveImages)
  {
    kkint32  a = (kkint32)edgeImage->CalcArea ();
    Save (*edgeImage, "Edge_JustBefore" + StrFormatInt (a, "ZZZZZZ0"), saveImages);
  }

  edgeImage->Edge ();
  kkint32  numEdgePixelsFound = (kkint32)edgeImage->CalcArea ();
  Save (*edgeImage, "Edge_Image_" + StrFormatInt (numEdgePixelsFound, "ZZZZ0"), saveImages);

  float  edgeMomentf[9];
  edgeImage->CentralMoments (edgeMomentf);
  delete  edgeImage;  edgeImage = NULL;

  kkint32 area = (kkint32)(momentf[0] + 0.5f);  // Moment-0 is the same as the number of foreground pixels in image.
  {
    ConvexHullPtr  ch = new ConvexHull();
    RasterPtr  convexImage = ch->Filter (*raster);
    RasterSipperPtr  convexImageSipper = RasterSipper::TurnIntoSipperRasterPtr (convexImage);
    convexImage = NULL;

    convexf = (FVFloat)ch->ConvexArea ();

    if  (saveImages)
    {
      KKStr convexImageFileName = "ConvexHull_" +
                                   StrFormatInt ((kkint32)convexf, "ZZZZZ0");
      Save (*convexImageSipper, convexImageFileName, saveImages);
    }

    delete  convexImageSipper;
    convexImageSipper = NULL;
    delete  ch;
    ch = NULL;
  }

  RasterSipperPtr open3Raster = new RasterSipper (*raster);
  open3Raster->Opening ();
  FVFloat  areaOpen3 = FVFloat (open3Raster->ForegroundPixelCount ());
  if  (saveImages)
    Save (*open3Raster, "Opening3_" + StrFormatInt ((kkint32)areaOpen3, "ZZZZZZ0"), saveImages);
  delete  open3Raster;
  open3Raster = NULL;
  

  RasterSipperPtr open5Raster = new RasterSipper (*raster);
  open5Raster->Opening (MorphOp::MaskTypes::SQUARE5);
  FVFloat  areaOpen5 = FVFloat (open5Raster->ForegroundPixelCount ());
  if  (saveImages)
    Save (*open5Raster, "Opening5_" + StrFormatInt ((kkint32)areaOpen5, "ZZZZZZ0"), saveImages);
  delete  open5Raster;
  open5Raster = NULL;
  

  RasterSipperPtr open7Raster = new RasterSipper (*raster);
  open7Raster->Opening (MorphOp::MaskTypes::SQUARE7);
  FVFloat  areaOpen7 = FVFloat (open7Raster->ForegroundPixelCount ());
  if  (saveImages)
    Save (*open7Raster, "Opening7_" + StrFormatInt ((kkint32)areaOpen7, "ZZZZZZ0"), saveImages);
  delete  open7Raster;
  open7Raster = NULL;
  
  RasterSipperPtr open9Raster = new RasterSipper (*raster);
  open9Raster->Opening (MorphOp::MaskTypes::SQUARE9);
  FVFloat  areaOpen9 = (FVFloat)open9Raster->ForegroundPixelCount ();
  if  (saveImages)
    Save (*open9Raster, "Opening9_" + StrFormatInt ((kkint32)areaOpen9, "ZZZZZZ0"), saveImages);
  delete  open9Raster;
  open9Raster = NULL;

  RasterSipperPtr close3Raster = new RasterSipper (*raster);
  close3Raster->Closing ();
  FVFloat  areaClose3 = FVFloat (close3Raster->ForegroundPixelCount ());
  if  (saveImages)
    Save (*close3Raster, "Close3_" + StrFormatInt ((kkint32)areaClose3, "ZZZZZZ0"), saveImages);

  close3Raster->FillHole ();
  FVFloat  tranf = (FVFloat)close3Raster->CalcArea ();
  if  (saveImages)
    Save (*close3Raster, "FillHole_" + StrFormatInt ((kkint32)tranf, "ZZZZZZ0"), saveImages);
  delete  close3Raster;
  close3Raster = NULL;
  
  RasterSipperPtr close5Raster = new RasterSipper (*raster);
  close5Raster->Closing (MorphOp::MaskTypes::SQUARE5);
  FVFloat  areaClose5 = FVFloat (close5Raster->ForegroundPixelCount ());
  if  (saveImages)
    Save (*close5Raster, "Close5_" + StrFormatInt ((kkint32)areaClose5, "ZZZZZZ0"), saveImages);
  delete  close5Raster;
  close5Raster = NULL;
  
  RasterSipperPtr close7Raster = new RasterSipper (*raster);
  close7Raster->Closing (MorphOp::MaskTypes::SQUARE7);
  FVFloat  areaClose7 = FVFloat (close7Raster->ForegroundPixelCount ());
  if  (saveImages)
    Save (*close7Raster, "Close7_" + StrFormatInt ((kkint32)areaClose7, "ZZZZZZ0"), saveImages);
  delete  close7Raster;
  close7Raster = NULL;


  #if defined (DEBUB_CalcFeatures)
  cout << "CalcFeatures   After Morphological Operations." << std::endl;
  #endif

  {
    featureData[SizeIndex]    = FVFloat (momentf[0]);
    featureData[Moment1Index] = FVFloat (momentf[1]);
    featureData[Moment2Index] = FVFloat (momentf[2]);
    featureData[Moment3Index] = FVFloat (momentf[3]);
    featureData[Moment4Index] = FVFloat (momentf[4]);
    featureData[Moment5Index] = FVFloat (momentf[5]);
    featureData[Moment6Index] = FVFloat (momentf[6]);
    featureData[Moment7Index] = FVFloat (momentf[7]);

    featureData[WeighedMoment0Index] = momentfWeighted[0];
    featureData[WeighedMoment1Index] = momentfWeighted[1];
    featureData[WeighedMoment2Index] = momentfWeighted[2];
    featureData[WeighedMoment3Index] = momentfWeighted[3];
    featureData[WeighedMoment4Index] = momentfWeighted[4];
    featureData[WeighedMoment5Index] = momentfWeighted[5];
    featureData[WeighedMoment6Index] = momentfWeighted[6];
    featureData[WeighedMoment7Index] = momentfWeighted[7];

    featureData[EdgeSizeIndex]    = (float)edgeMomentf[0];
    featureData[EdgeMoment1Index] = (float)edgeMomentf[1];
    featureData[EdgeMoment2Index] = (float)edgeMomentf[2];
    featureData[EdgeMoment3Index] = (float)edgeMomentf[3];
    featureData[EdgeMoment4Index] = (float)edgeMomentf[4];
    featureData[EdgeMoment5Index] = (float)edgeMomentf[5];
    featureData[EdgeMoment6Index] = (float)edgeMomentf[6];
    featureData[EdgeMoment7Index] = (float)edgeMomentf[7];

    // Need to adjust for any reduction in Image Size
    featureData[SizeIndex]           = FVFloat (areaBeforeReduction);
    featureData[EdgeSizeIndex]       = FVFloat (edgeMomentf[0] * (FVFloat)reductionFactor);
    featureData[WeighedMoment0Index] = weighedSizeBeforeReduction;
  }

  if ((area > convexf)  &&  (convexf > 0))
     featureData[TransparancyConvexHullIndex] = 1.0;
  else 
     featureData[TransparancyConvexHullIndex] = (FVFloat)area / (FVFloat)convexf;

  featureData[TransparancyPixelCountIndex] = (FVFloat)area / (FVFloat)tranf;
  featureData[TransparancyOpen3Index]      = (FVFloat)(area - areaOpen3)  / (FVFloat)area;
  featureData[TransparancyOpen5Index]      = (FVFloat)(area - areaOpen5)  / (FVFloat)area;
  featureData[TransparancyOpen7Index]      = (FVFloat)(area - areaOpen7)  / (FVFloat)area;                
  featureData[TransparancyOpen9Index]      = (FVFloat)(area - areaOpen9)  / (FVFloat)area;
  featureData[TransparancyClose3Index]     = (FVFloat)(area - areaClose3) / (FVFloat)area;
  featureData[TransparancyClose5Index]     = (FVFloat)(area - areaClose5) / (FVFloat)area;
  featureData[TransparancyClose7Index]     = (FVFloat)(area - areaClose7) / (FVFloat)area;
       
  {
    float  eigenRatio;
    float  orientationAngle;

    featureData[EigenRatioIndex] = 0.0;
    featureData[EigenHeadIndex]  = 0.0;

    raster->CalcOrientationAndEigerRatio (eigenRatio,
                                          orientationAngle
                                         );
    if  ((orientationAngle > TwoPie)  ||  (orientationAngle < -TwoPie))
    {
      orientationAngle = 0.0;
      eigenRatio = 1.0;
    }
    
    featureData[EigenRatioIndex] = eigenRatio;

    // Image will be rotated such that it longest dimension will be parallel to the x-axis

    RasterSipperPtr rotatedImage = raster->Rotate (orientationAngle);

    kkint32  tlRow;
    kkint32  tlCol;
    kkint32  brRow;
    kkint32  brCol;

     rotatedImage->FindBoundingBox (tlRow, tlCol, brRow, brCol);
    if  (tlRow < 0)
    {
      featureData[EigenHeadIndex] = 9999;  
      // featureData[59]  = 9999;      // Prior 2 Format 18
      // featureData[151] = 9999;      // Format 18
    }
    else
    {
      kkint32 firstQtrWeight = 0;
      kkint32 forthQtrWeight = 0;

      kkint32 boundingLength = brCol - tlCol + 1;
      kkint32 boundingWidth  = brRow - tlRow + 1;

      featureData[LengthVsWidthIndex] = (FVFloat)boundingWidth / (FVFloat)boundingLength;  // Prior 2 Format 18
      featureData[LengthIndex]        = (FVFloat)boundingLength * reductionFactor;
      featureData[WidthIndex]         = (FVFloat)boundingWidth  * reductionFactor;
      featureData[FilledAreaIndex]    = (FVFloat)areaWithWhiteSpace;  // Same as holeFilledArea
      
      kkint32 firstQtr = tlCol +  (boundingLength / 4);
      kkint32 thirdQtr = tlCol + ((boundingLength * 3) / 4);

      kkint32  row;
      kkint32  col;
      
      uchar** grey = rotatedImage->Green ();

      for  (row = tlRow;  row <= brRow;  row++)
      {
        for (col = tlCol;  col <= firstQtr;  col++)
        {
          if  (grey[row][col] > 0)
          {
            firstQtrWeight++;
          }
        }

        for (col = thirdQtr;  col <= brCol;  col++)
        {
          if  (grey[row][col] > 0)
          {
            forthQtrWeight++;
          }
        }
      }

      if  (saveImages)  
        Save (*rotatedImage, 
              "Rotated_" + StrFormatDouble (orientationAngle * 57.2958f, "zz0") + "_" 
              + StrFormatInt (firstQtrWeight, "####0") + "-" 
              + StrFormatInt (forthQtrWeight, "####0"), 
              saveImages
             );

      grey = NULL;

      if  ((firstQtrWeight < 1)  || (forthQtrWeight < 1))
      {
        // This should not happen;  but if it does will assign '0' to EigenHead feature.
        featureData[EigenHeadIndex] = 0;
      }
      else
      {
        if  (firstQtrWeight > forthQtrWeight)
        {
          featureData[EigenHeadIndex] = (FVFloat)firstQtrWeight / (FVFloat)forthQtrWeight;
        }
        else
        {
          featureData[EigenHeadIndex] = (FVFloat)forthQtrWeight / (FVFloat)firstQtrWeight;
        }
      }
    }

    delete  rotatedImage;
  }


  {
    // This part has to be done after 'CalcOrientationAndEigerRatio' is called; that is where the image centroid is computed.
    centroidCol = raster->CentroidCol () * reductionFactor;
    centroidRow = raster->CentroidRow () * reductionFactor;

    // sfCentroid,  will use ImageFileName to calculate Centroid with respect to sipper file.
    //  FileName format is 
    sfCentroidCol = -1.0f;
    sfCentroidRow = -1.0;

    if  (!ExampleFileName ().Empty ())
    {
      KKStr  sipperFileName (30);
      kkuint32 scanLineNum = 0;
      kkuint32 scanColNum  = 0;

      PicesVariables::ParseImageFileName (ExampleFileName (), sipperFileName, scanLineNum, scanColNum);

      if  (!sipperFileName.Empty ())
      {
        sfCentroidCol = float  (centroidCol) + float  (scanColNum);
        sfCentroidRow = double (centroidRow) + double (scanLineNum);
      }
    }
  }


  #if defined (DEBUB_CalcFeatures)
    cout << "CalcFeatures   After Image Rotation." << std::endl;
  #endif


  featureData[ConvexAreaIndex]       = convexf * reductionFactorSquared;
  featureData[TransparancySizeIndex] = (float)(momentf[0] / convexf);
  featureData[TransparancyWtdIndex]  = (float)(momentfWeighted[0] / convexf);

  {
    //RasterSipperPtr  Fourier  = raster->FastFourierKK ();
    RasterSipperPtr  fourier  = raster->FastFourier ();

    Save (*fourier, "Fourier", saveImages);
   
    fourier->FourierExtractFeatures (fourierFeatures);
    featureData[Fourier0Index] = fourierFeatures[0];
    featureData[Fourier1Index] = fourierFeatures[1];
    featureData[Fourier2Index] = fourierFeatures[2];
    featureData[Fourier3Index] = fourierFeatures[3];
    featureData[Fourier4Index] = fourierFeatures[4];

    delete  fourier;
  }


  {
    bool  successful;
    FVFloat  countourFreq[5];
    FVFloat  fourierDescriptors[15];

    {
      RasterSipper  countourImage (*raster);
      //countourImage.Dilation (SQUARE5);  


      kkint32  minExpectedLen = (kkint32)(float (numEdgePixelsFound) * 0.5f);

      successful = false;
      kkint32  numOfTries = 0;
      while  (!successful)
      {
        countourImage.Dilation (); // We want the image to be completely connected.
        {
          Save  (countourImage, "_JustBeforeContour", saveImages);
          ContourFollower contourFollower (countourImage, log);
          numOfEdgePixels = contourFollower.FollowContour (countourFreq, fourierDescriptors, (kkint32)featureData[SizeIndex], successful) * reductionFactor;

          if  (successful)
          {
            if  (numOfEdgePixels < minExpectedLen)
            {
              // We probably found a detached piece;  so we need to do another dilation  and try again.
              successful = false;
            }
          }
          if  (numOfTries > 10)
          {
            // We have already tried 3 times;  we will just go with what we have.
            successful = true;
          }
          numOfTries++;
        }
      }

      if  (saveImages)
      {
        ContourFollower contourFollower2 (countourImage, log);
        PointListPtr  contourEdgePixels = contourFollower2.GenerateContourList ();
        if  (!contourEdgePixels)
        {
          cerr << endl << "ImageFeatures::CalcFeatures     ***ERROR***     contourFollower2   returned NULL"  << endl << endl;    
        }
        else
        {
          RasterSipperPtr  r = new RasterSipper (countourImage.Height (), countourImage.Width (), false);
          r->DrawPointList (*contourEdgePixels, PixelValue (255, 255, 255));
          Save  (*r, "_JustBeforeContour_Edge", saveImages);
          delete  r;                  r                 = NULL;
          delete  contourEdgePixels;  contourEdgePixels = NULL;
        }
      }
    }


    {
      featureData[ContourFourierDescriptor0] = countourFreq[0];
      featureData[ContourFourierDescriptor1] = countourFreq[1];
      featureData[ContourFourierDescriptor2] = countourFreq[2];
      featureData[ContourFourierDescriptor3] = countourFreq[3];
      featureData[ContourFourierDescriptor4] = countourFreq[4];

      featureData[FdRight1Index] = fourierDescriptors[0];

      featureData[FdLeft2Index]  = fourierDescriptors[1];
      featureData[FdRight2Index] = fourierDescriptors[2];

      featureData[FdLeft3Index]  = fourierDescriptors[3];
      featureData[FdRight3Index] = fourierDescriptors[4];

      featureData[FdLeft4Index]  = fourierDescriptors[5];
      featureData[FdRight4Index] = fourierDescriptors[6];

      featureData[FdLeft5Index]  = fourierDescriptors[7];
      featureData[FdRight5Index] = fourierDescriptors[8];

      featureData[FdLeft6Index]  = fourierDescriptors[9];
      featureData[FdRight6Index] = fourierDescriptors[10];

      featureData[FdLeft7Index]  = fourierDescriptors[11];
      featureData[FdRight7Index] = fourierDescriptors[12];

      featureData[FdLeft8Index]  = fourierDescriptors[13];
      featureData[FdRight8Index] = fourierDescriptors[14];
    }
  }


  #if defined (DEBUB_CalcFeatures)
    cout << "CalcFeatures   After   Fourier Descriptors." << std::endl;
  #endif


  FVFloat  areaD = FVFloat (areaBeforeReduction);
  FVFloat  areaWithWhiteSpaceD = FVFloat (areaWithWhiteSpace);

  featureData[IntensityHist1Index] = ((FVFloat)intensityHistBuckets[1] / areaD);
  featureData[IntensityHist2Index] = ((FVFloat)intensityHistBuckets[2] / areaD);
  featureData[IntensityHist3Index] = ((FVFloat)intensityHistBuckets[3] / areaD);
  featureData[IntensityHist4Index] = ((FVFloat)intensityHistBuckets[4] / areaD);
  featureData[IntensityHist5Index] = ((FVFloat)intensityHistBuckets[5] / areaD);
  featureData[IntensityHist6Index] = ((FVFloat)intensityHistBuckets[6] / areaD);
  featureData[IntensityHist7Index] = ((FVFloat)intensityHistBuckets[7] / areaD);

  featureData[IntensityHistWS0Index] = ((FVFloat)intensityHistBucketsB[0] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS1Index] = ((FVFloat)intensityHistBucketsB[1] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS2Index] = ((FVFloat)intensityHistBucketsB[2] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS3Index] = ((FVFloat)intensityHistBucketsB[3] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS4Index] = ((FVFloat)intensityHistBucketsB[4] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS5Index] = ((FVFloat)intensityHistBucketsB[5] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS6Index] = ((FVFloat)intensityHistBucketsB[6] / areaWithWhiteSpaceD);
  featureData[IntensityHistWS7Index] = ((FVFloat)intensityHistBucketsB[7] / areaWithWhiteSpaceD);

  if  (saveImages)
  {
    RasterSipperPtr  thinnedImage = raster->ThinContour ();
    Save  (*thinnedImage, "_Thinned", saveImages);
    delete  thinnedImage;  
    thinnedImage = NULL;
  }


  featureData[0] = (float)areaBeforeReduction;  // In case the image was reduced.
  OrigSize ((float)areaBeforeReduction);


  #if defined (DEBUB_CalcFeatures)
    cout << "CalcFeatures   After   Thinned Images." << std::endl;
    cout.flush ();
  #endif

  Probability (-1.0f);

  if  (weOwnRaster)
  {
    delete  raster;
    raster = NULL;
  }


  #if defined (DEBUB_CalcFeatures)
    cout << "CalcFeatures   Exiting CalcFeatures." << std::endl;
  #endif

}  /* CalcFeatures */






ImageFeaturesList::ImageFeaturesList (FileDescPtr  _fileDesc,
                                      bool         _owner,
                                      kkint32      _size
                                     ):

    FeatureVectorList (_fileDesc, _owner),
    version           (0)
{

}




ImageFeaturesList::ImageFeaturesList (RunLog&       _log,
                                      MLClassPtr    _mlClass,
                                      const KKStr&  _dirName,
                                      const KKStr&  _fileName
                                     ):

  FeatureVectorList (FeatureFileIOPices::NewPlanktonFile (), true),
  version           (0)
{
  FeatureExtraction (_dirName, _fileName, _mlClass, _log);
}




ImageFeaturesList::ImageFeaturesList (const ImageFeaturesList&  images):

   FeatureVectorList (images.FileDesc (), images.Owner ()),
   version           (images.version)
{
  const_iterator  idx;
  for  (idx = images.begin ();  idx != images.end ();  idx++)
  {
    const ImageFeaturesPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new ImageFeatures (*imageExample));
    else
      PushOnBack (imageExample);
  }
}




ImageFeaturesList::ImageFeaturesList (const ImageFeaturesList&  images,
                                      bool                      _owner
                                     ):

   FeatureVectorList (images.FileDesc (), _owner),
   version           (images.version)
{
  const_iterator  idx;
  for  (idx = images.begin ();  idx != images.end ();  idx++)
  {
    const ImageFeaturesPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new ImageFeatures (*imageExample));
    else
      PushOnBack (imageExample);
  }
}


ImageFeaturesList::ImageFeaturesList (const FeatureVectorList&  featureVectorList,
                                      bool                      _owner
                                     ):

  FeatureVectorList (featureVectorList.FileDesc (),
                     _owner
                    ),
  version (0)

{
  if  (typeid (featureVectorList) == typeid (ImageFeaturesList))
  {
    const ImageFeaturesList&  images = dynamic_cast<const ImageFeaturesList&> (featureVectorList);
    version = images.Version ();
  }


  if  (_owner)
  {
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'ImageFeatures' object)  then
      //   | Information that is particular to a 'ImageFeatures' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'ImageFeatures' object will be set to
      //   | default values.
      ImageFeaturesPtr  image = new ImageFeatures (*featureVector);
      PushOnBack (image);
    }
  }
  else
  {
    // Since we will not own the contents but just point to an existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'ImageFeatures'.
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      if  (typeid (*featureVector) == typeid (ImageFeatures))
      {
        ImageFeaturesPtr image = dynamic_cast<ImageFeaturesPtr>(featureVector);
        PushOnBack (image);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  msg (200);
        msg << "ImageFeaturesList Copy Constructor for FeatureVectorList  Example[" <<  featureVector->ExampleFileName () 
            << "] does not have a underlying type of 'ImageFeatures'.";
        cerr << endl << msg << endl;
        throw KKException (msg);
      }
    }
  }
}









//****************************************************************************
//*  Will Create a list of images that are a subset of the ones in _images.  *
//* The subset will consist of the images who's mlClass is one of the     *
//* ones in mlClasses.                                                    *
//****************************************************************************
ImageFeaturesList::ImageFeaturesList (MLClassList&        _mlClasses,
                                      ImageFeaturesList&  _images
                                     ):
  FeatureVectorList (_mlClasses, _images),
  version           (_images.version)

{
}




ImageFeaturesList::ImageFeaturesList (const FeatureVectorList&  featureVectorList):
  FeatureVectorList (featureVectorList.FileDesc (),
                     featureVectorList.Owner ()
                    ),
  version (0)

{
  if  (typeid (featureVectorList) == typeid (ImageFeaturesList))
  {
    const ImageFeaturesList&  images = dynamic_cast<const ImageFeaturesList&> (featureVectorList);
    version = images.Version ();
  }


  if  (featureVectorList.Owner ())
  {
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'ImageFeatures' object)  then
      //   | Information that is particular to a 'ImageFeatures' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'ImageFeatures' object will be set to
      //   | default values.
      ImageFeaturesPtr  image = new ImageFeatures (*featureVector);
      PushOnBack (image);
    }
  }
  else
  {
    // Since we will not own the contents but just point to existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'ImageFeatures'.
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      if  (typeid (*featureVector) == typeid (ImageFeatures))
      {
        ImageFeaturesPtr image = dynamic_cast<ImageFeaturesPtr>(featureVector);
        PushOnBack (image);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg (128);
        errMsg << "ImageFeaturesList ::ImageFeaturesList  ***ERROR***   Elements in 'featureVectorList' is not of 'ImageFeatures'  type;   FileName[" << featureVector->ExampleFileName () << "]";
        cerr << endl << errMsg << endl << endl;
        throw KKException (errMsg);
      }
    }
  }
}





ImageFeaturesList::~ImageFeaturesList ()
{
}




ImageFeaturesListPtr  ImageFeaturesList::Duplicate (bool _owner)  const
{
  return new ImageFeaturesList (*this, _owner);
}




ImageFeaturesPtr  ImageFeaturesList::IdxToPtr (kkint32 idx)  const
{
  return  (ImageFeaturesPtr)FeatureVectorList::IdxToPtr (idx);
}  /* IdxToPtr */




void  ImageFeaturesList::AddSingleImageFeatures (ImageFeaturesPtr  _imageFeatures)
{
  PushOnBack (_imageFeatures);
}



ImageFeaturesPtr  ImageFeaturesList::BackOfQueue ()
{
  if  (size () < 1)
    return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (ImageFeatures))
    return  dynamic_cast<ImageFeaturesPtr> (fv);

  cerr << endl 
       << "ImageFeaturesList::BackOfQueue ()    ***ERROR***   Entry at back of Queue is not a 'ImageFeatures' object." << endl
       << endl;

  return NULL;
}  /* BackOfQueue */




ImageFeaturesPtr  ImageFeaturesList::PopFromBack ()
{
  if  (size () < 1)  return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) != typeid (ImageFeatures))
  {
    cerr << endl 
         << "ImageFeaturesList::PopFromBack ()    ***ERROR***        Entry popped from back of Queue is not a 'ImageFeatures' object." << endl
         << endl;
    return NULL;
  }

  return  dynamic_cast<ImageFeaturesPtr> (FeatureVectorList::PopFromBack ());
}  /* PopFromBack */






void  ImageFeaturesList::AddQueue (ImageFeaturesList&  imagesToAdd)
{
  if  (QueueSize () < 1)
  {
    // Since we don't have any images in the existing queue then the version number
    // of the queue that we are adding will be our version number.
    Version (imagesToAdd.Version ());
  }
  else
  {
    // Since there are images in both queues,  then they should have the same version number
    // otherwise the version number of the resulting KKQueue will be undefined (-1).
    if  (imagesToAdd.Version () != Version ())
      Version (0);
  }

  FeatureVectorList::AddQueue (imagesToAdd);
}  /* AddQueue */






ImageFeaturesPtr  ImageFeaturesList::BinarySearchByName (const KKStr&  _imageFileName)  const
{
  return  (ImageFeaturesPtr)FeatureVectorList::BinarySearchByName (_imageFileName);
}  /* BinarySearchByName */





ImageFeaturesPtr  ImageFeaturesList::LookUpByRootName (const KKStr&  _rootName)
{
  return  (ImageFeaturesPtr)FeatureVectorList::LookUpByRootName (_rootName);
}  /* LookUpByRootName */




ImageFeaturesListPtr   ImageFeaturesList::ManufactureEmptyList (bool _owner)  const
{
  return new ImageFeaturesList (FileDesc (), _owner);
}



ImageFeaturesPtr  ImageFeaturesList::LookUpByImageFileName (const KKStr&  _imageFileName)  const
{
  return  (ImageFeaturesPtr)FeatureVectorList::LookUpByImageFileName (_imageFileName);
}  /* LookUpByImageFileName */





ImageFeaturesListPtr  ImageFeaturesList::OrderUsingNamesFromAFile (const KKStr&  fileName,
                                                                   RunLog&       runLog
                                                                  )
{
  FeatureVectorListPtr  examples = FeatureVectorList::OrderUsingNamesFromAFile (fileName, runLog);
  examples->Owner (false);
  ImageFeaturesListPtr  orderedImages = new ImageFeaturesList (*examples);
  delete  examples;
  examples = NULL;
  return  orderedImages;
}  /* OrderUsingNamesFromAFile */






void   ImageFeaturesList::FeatureExtraction (const KKStr&  _dirName, 
                                             const KKStr&  _fileName, 
                                             MLClassPtr    _mlClass,
                                             RunLog&       _runLog
                                            )
{
  KKStr  className = _mlClass->Name ();
  _runLog.Level (10) << "FeatureExtraction,  dirName   [" << _dirName    << "]." << endl;
  _runLog.Level (10) << "                    fileName  [" << _fileName   << "]." << endl;
  _runLog.Level (10) << "                    className [" << className   << "]." << endl;

  bool  cancelFlag  = false;
  bool  successful  = false;

  KKStr  fullFeatureFileName = osAddSlash (_dirName) + _fileName;

  KKStrListPtr   fileNameList;
  
  KKStr  fileSpec = osAddSlash (_dirName) + "*.*";
  fileNameList = osGetListOfFiles (fileSpec);
  if  (!fileNameList)
    return;

  bool  successfull;

  kkint32  numOfImages = fileNameList->QueueSize ();
  kkint32  count = 0;

  KKStrList::const_iterator  fnIDX;
  for  (fnIDX = fileNameList->begin ();  fnIDX != fileNameList->end ();  ++fnIDX)
  {
    if  ((count % 100) == 0)
      cout << className << " " << count << " of " << numOfImages << endl;

    KKStrPtr imageFileName = *fnIDX;
    
    bool validImageFileFormat = SupportedImageFileFormat (*imageFileName);
    if  (!validImageFileFormat)
      continue;

    KKStr  fullFileName = osAddSlash (_dirName) + (*imageFileName);
    ImageFeaturesPtr image = new ImageFeatures (fullFileName, _mlClass, successfull, NULL, _runLog);

    if  (!image)
    {
      KKStr  msg (100);
      msg  << "ImageFeaturesList::FeatureExtraction  ***ERROR***, FeatureExtraction Could not Allocate Space for ImageFeatures object.";
      cerr << endl << endl << msg << endl << endl;
      osDisplayWarning (msg);
    }

    if  (!successfull)
    {
      _runLog.Level (-1) << "ImageFeaturesList::FeatureExtraction  ***ERROR***, Processing file[" << imageFileName << "]." << endl;
      delete  image;
    }

    else
    {
      image->ExampleFileName (*imageFileName);
      _runLog.Level (30) << image->ExampleFileName () << "  " << image->OrigSize () << endl;
      PushOnBack (image);
    }

    count++;
  }

  Version (CurrentFeatureFileVersionNum);

  kkuint32  numExamplesWritten = 0;

  // WriteImageFeaturesToFile (fullFeatureFileName, RawFormat, FeatureNumList::AllFeatures (fileDesc));
  FeatureFileIOPices::Driver ()->SaveFeatureFile (fullFeatureFileName,
                                                  this->AllFeatures (),
                                                  *this, 
                                                  numExamplesWritten,
                                                  cancelFlag,
                                                  successful,
                                                  _runLog
                                                 );

  delete  fileNameList;  fileNameList = NULL;
}  /* FeatureExtraction */




VectorDouble  ImageFeaturesList::ExtractPositionsByMeter (InstrumentDataListPtr  instrumentData,
                                                          float                  defaultScanRate,
                                                          float                  defaultFlowRate
                                                         )
{
  SortBySpatialDistance ();

  if  (defaultScanRate <= 0.0f)
    defaultScanRate = 24950.0f;

  if  (defaultFlowRate <= 0.0f)
    defaultFlowRate = 1.0f;

  kkuint32  frameSize = 4096;

  VectorDouble  frameOffsets;
  if  (instrumentData != NULL)
  {
    frameOffsets = instrumentData->FrameOffsetsInMeters (frameSize, defaultScanRate, defaultFlowRate);
  }

  VectorDouble  positions;
  double        position = 0.0;

  double  flowRate = defaultFlowRate;
  double  scanRate = defaultScanRate;


  ImageFeaturesList::iterator  idx;

  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ImageFeaturesPtr  example = *idx;
    position = 0.0;
    kkuint32  frameIdx = (kkuint32)(example->SfCentroidRow () / frameSize);
    kkuint32  scanLineToCalcFrom = 0;
    float lastFlowRate = defaultFlowRate;
    if  (frameIdx < frameOffsets.size ())
    {
      position = frameOffsets[frameIdx];
      scanLineToCalcFrom = frameIdx * frameSize;

      InstrumentDataPtr  id = instrumentData->SearchForNearestScanLine ((kkuint32)example->SfCentroidRow ());
      if  (!id)
        flowRate = defaultFlowRate;
      else
      {
        flowRate = id->FlowRate1 ();
        if  (flowRate <= 0.0)
          flowRate = defaultFlowRate;
      }
    }
    {
      // Add in the difference from the start of the frame.
      double  deltaScanLines = example->SfCentroidRow () - scanLineToCalcFrom;
      double  deltaTime = deltaScanLines / defaultScanRate;
      double  deltaDist = deltaTime * flowRate;
      position += deltaDist;
    }

    positions.push_back (position);
  }

  return  positions;
}  /* ExtractPositionsByMeter */






VectorInt   ImageFeaturesList::CreateSpatialDistributionHistogram (InstrumentDataListPtr  instrumentData,
                                                                   float                  defaultScanRate,   // Scan rate to use if it can not be located from 
                                                                   float                  defaultFlowRate,   // Flow rate to use if no instrumentation data.
                                                                   float                  bucketSize         // Size of each histogram bucket.
                                                                  )
{
  VectorDouble  positions = ExtractPositionsByMeter (instrumentData, defaultScanRate, defaultFlowRate);

  VectorInt     histogram;
  kkuint32      bucketIdx = 0;

  for  (auto idx: positions)
  {
    bucketIdx = (kkint32)floor (idx / bucketSize);

    if  (bucketIdx < 0)
      bucketIdx= 0;

    while  (histogram.size () <= bucketIdx)
      histogram.push_back (0);

    histogram[bucketIdx]++;
  }

  return  histogram;
}  /* CreateSpatialDistributionHistogram */





void  ImageFeaturesList::PrintSpatialHistogramReport (ostream&               r,
                                                      InstrumentDataListPtr  instrumentData,
                                                      float                  defaultScanRate,   // Scan rate to use if it can not be located from 
                                                      float                  defaultFlowRate,   // Flow rate to use if no instrumentation data.
                                                      float                  bucketSize
                                                     )
{
  VectorInt  allClassesHistogram = CreateSpatialDistributionHistogram (instrumentData, defaultScanRate, defaultFlowRate, bucketSize);

  vector<VectorInt>  histogramsByClass;

  MLClassListPtr  classes = ExtractListOfClasses ();
  classes->SortByName ();

  kkuint32  maxHistogramSize = (kkuint32)allClassesHistogram.size ();


  for  (auto idx: *classes)
  {
    MLClassPtr ic = idx;

    ImageFeaturesListPtr  imagesThisClass = ExtractExamplesForAGivenClass (ic);

    VectorInt  histogramThisClass = imagesThisClass->CreateSpatialDistributionHistogram (instrumentData, defaultScanRate, defaultFlowRate, bucketSize);
    maxHistogramSize = Max (maxHistogramSize, (kkuint32)histogramThisClass.size ());

    histogramsByClass.push_back (histogramThisClass);

    delete  imagesThisClass;   imagesThisClass = NULL;
  }

  KKStr  h1, h2, h3;
  classes->ExtractThreeTitleLines (h1, h2, h3);

  r << ""        << "\t" << ""        << "\t" << h1 << endl;
  r << ""        << "\t" << "All"     << "\t" << h2 << endl;
  r << "Bucket"  << "\t" << "Classes" << "\t" << h3 << endl;

  kkuint32  bucketIdx = 0;
  for  (bucketIdx = 0;  bucketIdx < maxHistogramSize;  bucketIdx++)
  {
    r << bucketSize * (float)bucketIdx;

    r << "\t";
    if  (bucketIdx < (kkuint32)allClassesHistogram.size ())
      r << allClassesHistogram[bucketIdx];
    else
      r << "";

    kkuint32  classIdx = 0;
    for  (classIdx = 0;  classIdx < (kkuint32)histogramsByClass.size ();  classIdx++)
    {
      r << "\t";
      if  ((bucketIdx < (kkuint32)histogramsByClass[classIdx].size ()))
        r << histogramsByClass[classIdx][bucketIdx];
      else
        r << "";
    }

    r << endl;
  }
   
  delete  classes; classes = NULL;
}  /* PrintSpatialHistogramReport */






/**
 * @brief  Creates a duplicate of list and also duplicates it contents.
 * @return Duplicated list with hard copy of its contents.
 */
ImageFeaturesListPtr  ImageFeaturesList::DuplicateListAndContents ()  const
{
  ImageFeaturesListPtr  copyiedList = new ImageFeaturesList (FileDesc (), true, QueueSize ());

  for  (kkint32 idx = 0;  idx < QueueSize ();  idx++)
  {
    ImageFeaturesPtr  curImage = (ImageFeaturesPtr)IdxToPtr (idx);
    copyiedList->AddSingleImageFeatures (new ImageFeatures (*curImage));
  }
  
  copyiedList->Version (Version ());

  return  copyiedList;
}  /* DuplicateListAndContents */






void  ImageFeaturesList::RecalcFeatureValuesFromImagesInDirTree (KKStr    rootDir,
                                                                 bool&    successful,
                                                                 RunLog&  runLog
                                                                )
{
  runLog.Level (20) << "RecalcFeatureValuesFromImagesInDirTree   RootDir[" << rootDir << "]." << endl;

  successful = false;

  ImageDirTree   fileDirectory (rootDir);

  if  (QueueSize () < 1)
  {
    successful = true;
    return;
  }

  if  (fileDirectory.Size () < 1)
  {
    runLog.Level (10) << "RecalcFeatureValuesFromImagesInDirTree  No Image Files in[" << rootDir << "]" << endl;
    return;
  }

  KKStrConstPtr  dirPath = NULL;

  ImageFeaturesPtr  image = NULL;

  ImageFeaturesList::iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    image = *idx;
    dirPath = fileDirectory.LocateImage (image->ExampleFileName ());
    if  (!dirPath)
    {
      runLog.Level (10) << "RecalcFeatureValuesFromImagesInDirTree  Could not locate Image[" << image->ExampleFileName () << "] in Subdirectory Tree." << endl;
      return;
    }

    KKStr  fullFileName (*dirPath);
    osAddLastSlash (fullFileName);
    fullFileName << image->ExampleFileName ();

    bool   validFile;
    RasterSipperPtr  raster = new RasterSipper (fullFileName, validFile);
    if  (!validFile)
    {
      delete  raster;
      return;
    }
    image->CalcFeatures (*raster, NULL, runLog);
    delete  raster;  raster = NULL;
  }
}  /* RecalcFeatureValuesFromImagesInDirTree */





ImageFeaturesListPtr  ImageFeaturesList::ExtractDuplicatesByRootImageFileName ()
{
  FeatureVectorListPtr  duplicateFeatureVectorObjects = FeatureVectorList::ExtractDuplicatesByRootImageFileName ();
  ImageFeaturesListPtr  duplicateImageFeaturesObjects = new ImageFeaturesList (*duplicateFeatureVectorObjects);
  duplicateFeatureVectorObjects->Owner (false);
  delete  duplicateFeatureVectorObjects;  duplicateFeatureVectorObjects = NULL;
  return  duplicateImageFeaturesObjects;
}  /* ExtractDuplicatesByRootImageFileName */






ImageFeaturesListPtr   ImageFeaturesList::ExtractExamplesForAGivenClass (MLClassPtr  _mlClass,
                                                                       kkint32     _maxToExtract,
                                                                       FVFloat     _minSize
                                                                      )  const
{
  FeatureVectorListPtr  featureVectorList = FeatureVectorList::ExtractExamplesForAGivenClass (_mlClass, _maxToExtract, _minSize);
  ImageFeaturesListPtr  imageFeaturesList = new ImageFeaturesList (*featureVectorList);
  featureVectorList->Owner (false);
  delete  featureVectorList;  featureVectorList = NULL;
  return  imageFeaturesList;
}  /*  ExtractExamplesForAGivenClass  */





ImageFeaturesListPtr  ImageFeaturesList::StratifyAmoungstClasses (MLClassListPtr  mlClasses,
                                                                  kkint32         maxImagesPerClass,
                                                                  kkint32         numOfFolds,
                                                                  RunLog&         runLog
                                                                 )
{
  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (mlClasses, maxImagesPerClass, numOfFolds, runLog);
  ImageFeaturesListPtr  stratifiedImagefeatures  = new ImageFeaturesList (*stratifiedFeatureVectors);
  stratifiedFeatureVectors->Owner (false);
  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */




ImageFeaturesListPtr  ImageFeaturesList::StratifyAmoungstClasses (kkint32  numOfFolds,
                                                                  RunLog&  runLog
                                                                 )
{
  MLClassListPtr  classes = this->ExtractListOfClasses ();

  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (classes, -1, numOfFolds, runLog);
  ImageFeaturesListPtr  stratifiedImagefeatures  = new ImageFeaturesList (*stratifiedFeatureVectors);
  
  stratifiedFeatureVectors->Owner (false);

  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  delete  classes;                  classes                  = NULL;

  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */







/**
 * @brief Fixed the two fields  sfCentroidCol & sfCentroidRow.
 *        This has to be done because the original calculations for them assumed that  there
 *        would be no '_' characters in the sipper file name.
 */
void  ImageFeaturesList::FixSipperFileScanLineAndColFields ()
{
  ImageFeaturesList::iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ImageFeaturesPtr  i = *idx;

    KKStr  rootName = osGetRootName (i->ExampleFileName ());
    kkint32 x = rootName.LocateLastOccurrence ('_');
    if  (x > 0)
    {
      KKStr  colStr = rootName.SubStrPart (x + 1);
      KKStr  temp = rootName.SubStrPart (0, x - 1);
      x = temp.LocateLastOccurrence ('_');
      if  (x > 0)
      {
        KKStr  rowStr = temp.SubStrPart (x + 1);
        float  sfCentroidCol = float  (i->CentroidCol ()) + float  (atoi (colStr.Str ()));
        double sfCentroidRow = double (i->CentroidRow ()) + double (atoi (rowStr.Str ()));
        i->SfCentroidCol (sfCentroidCol);
        i->SfCentroidRow (sfCentroidRow);
      }
    }
  }
}  /* FixSipperFileScanLineAndColFields */




void  ImageFeaturesList::SortBySpatialDistance ()
{
  ImageFeaturesList::SpatialComparison  c;
  sort (begin (), end (), c);
}  /* SortBySpatialDistance */




float  ImageFeaturesList::FlowRate (InstrumentDataPtr  id,
                                    float              defaultFlowRate
                                   )
{
  if  ((!id)  ||  (id->FlowRate1 () <= 0.0f))
    return  defaultFlowRate;

  return  id->FlowRate1 ();
}



VectorFloat   ImageFeaturesList::CalculateDensitesByQuadrat (float        scanRate,         // Scan Lines per Sec.
                                                             float        quadratSize,      // Meters.
                                                             float        defaultFlowRate,  // Meters per Sec
                                                             const bool&  cancelFlag,
                                                             RunLog&      log
                                                            )
{
  InstrumentDataFileManager::InitializePush ();

  SortBySpatialDistance ();

  iterator  idx;
  idx = begin ();
  ImageFeaturesPtr  firstOne = *idx;

  float  curFlowRate      = defaultFlowRate;
  float  distLastImage    = 0.0f;
  double   scanLineLastImage = 0;

  VectorInt  imagesPerQuadrat;
  imagesPerQuadrat.push_back (0);

  InstrumentDataPtr  id = InstrumentDataFileManager::GetClosestInstrumentData (firstOne->ExampleFileName (), cancelFlag, log);
  curFlowRate = FlowRate (id, defaultFlowRate);

  for  (idx = begin ();  idx != end ();  idx++)
  {
    ImageFeaturesPtr  i = *idx;

    if  (id  &&  ((i->SfCentroidRow () - id->ScanLine ()) > 4096))
    {
      // Time to get a more UpToDate Instrument Data Item
      id = InstrumentDataFileManager::GetClosestInstrumentData (i->ExampleFileName (), cancelFlag, log);
      curFlowRate = FlowRate (id, defaultFlowRate);
    }

    double  deltaScanLines = i->SfCentroidRow () - scanLineLastImage;

    float  deltaDist = ((float)deltaScanLines / scanRate) * curFlowRate;

    float  newDist = distLastImage + deltaDist;

    kkuint32  quadratIdx = (kkint32)(newDist / quadratSize);
    while  (imagesPerQuadrat.size () <= quadratIdx)
      imagesPerQuadrat.push_back (0);
    imagesPerQuadrat[quadratIdx]++;

    distLastImage = newDist;
    scanLineLastImage = i->SfCentroidRow ();
  }


  //float  areaPerQuatdrat = quadratSize * 0.098f;
  float  volPerQuatdrat = quadratSize * 0.098f * 0.098f;

  VectorFloat  density (imagesPerQuadrat.size (), 0.0f);
  for  (kkuint32 x = 0;  x < imagesPerQuadrat.size ();  x++)
    density[x] = imagesPerQuadrat[x] / volPerQuatdrat;

  {
    kkuint32  i = 0;
    ofstream r ("c:\\Temp\\CalculateDensitesByQuadrat.txt");
    for  (i = 0; i < imagesPerQuadrat.size ();  i++)
      r << "i = " << i << "\t" << imagesPerQuadrat[i] << "\t" << "Density = " << "\t" << density[i] << endl;
    r << endl;

    r.close ();
  }
  

  InstrumentDataFileManager::InitializePop ();

  return  density;
}  /* CalculateDensitesByQuadrat */
