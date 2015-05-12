#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "ImageSizeDistribution.h"
#include "PicesRunLog.h"


namespace PicesInterface 
{
  public  ref class  PicesImageSizeDistributionRow
  {
  public:
    PicesImageSizeDistributionRow (ImageSizeDistributionRowPtr  _imageSizeDistributionRow,
                                   bool                         _willOwn
                                  );

  private:
    ~PicesImageSizeDistributionRow ();

  protected:
    !PicesImageSizeDistributionRow ();

  public:
    property  float  Depth           {float  get ()  {return imageSizeDistributionRow->Depth           ();}}
    property  uint   ImageCount      {uint   get ()  {return imageSizeDistributionRow->ImageCount      ();}}
    property  uint   TotalPixels     {uint   get ()  {return imageSizeDistributionRow->TotalPixels     ();}}
    property  uint   TotalFilledArea {uint   get ()  {return imageSizeDistributionRow->TotalFilledArea ();}}
    property  float  VolumneSampled  {float  get ()  {return imageSizeDistributionRow->VolumneSampled  ();}}
    
    array<uint>^  Distribution ();

    MLL::ImageSizeDistributionRowPtr  UnmanagedClass ()  {return  imageSizeDistributionRow;}

    void  AddIn (PicesImageSizeDistributionRow^  right);

  private:
    MLL::ImageSizeDistributionRowPtr  imageSizeDistributionRow;
    bool                              owner;
  };  /* PicesImageSizeDistributionRow */



  public  ref class  PicesImageSizeDistribution  
  {
  public:
    PicesImageSizeDistribution (ImageSizeDistributionPtr  _imageSizeDistributionPtr);
  
    PicesImageSizeDistribution (PicesImageSizeDistribution^  _imageSizeDistribution);

  private:
    ~PicesImageSizeDistribution ();

  protected:
    !PicesImageSizeDistribution ();

  public:
    property  float   DepthBinSize    {float   get ()  {return  imageSizeDistribution->DepthBinSize   ();}}
    property  float   InitialValue    {float   get ()  {return  imageSizeDistribution->InitialValue   ();}}
    property  float   GrowthRate      {float   get ()  {return  imageSizeDistribution->GrowthRate     ();}}
    property  float   EndValue        {float   get ()  {return  imageSizeDistribution->EndValue       ();}}
    property  int     NumSizeBuckets  {int     get ()  {return  imageSizeDistribution->NumSizeBuckets ();}}
    property  int     NumDepthBins    {int     get ()  {return  imageSizeDistribution->NumDepthBins   ();}}

    MLL::ImageSizeDistributionPtr  UnmanagedClass ()  {return  imageSizeDistribution;}

    array<float>^  SizeStartValues ();
    array<float>^  SizeEndValues   ();

    void  AddIn (PicesImageSizeDistribution^  right,
                 PicesRunLog^                 log
                );

    PicesImageSizeDistributionRow^   AllDepths ();

    array<float>^  DepthProfileForSizeBin (kkuint32  _sizeBucketIdx);

    PicesImageSizeDistributionRow^   GetDepthBin (uint  depthBinIdx);

    void  GetSizeBucketStats (uint       _sizeBucketIdx,
                              uint%      _count,
                              float%    _sizeStart,
                              float%    _sizeEnd
                             );

    kkint32  IdentifySizeBucket (float  size);

    array<float>^  IntegratedDensityDistribution ();

    array<float>^  VolumeSampledByDepthBucket ();

  private:
    MLL::ImageSizeDistributionPtr  imageSizeDistribution;
  };  /* PicesImageSizeDistribution */


}

