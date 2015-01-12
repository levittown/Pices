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
    property  float  Depth       {float  get ()  {return imageSizeDistributionRow->Depth       ();}}
    property  uint   ImageCount  {uint   get ()  {return imageSizeDistributionRow->ImageCount  ();}}
    property  uint   TotalPixels {uint   get ()  {return imageSizeDistributionRow->TotalPixels ();}}

    array<uint>^  Distribution ();

  private:
    MLL::ImageSizeDistributionRowPtr  imageSizeDistributionRow;
    bool                              owner;
  };  /* PicesImageSizeDistributionRow */



  public  ref class  PicesImageSizeDistribution  
  {
  public:
    PicesImageSizeDistribution (ImageSizeDistributionPtr  _imageSizeDistributionPtr);

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

  private:
    MLL::ImageSizeDistributionPtr  imageSizeDistribution;
  };  /* PicesImageSizeDistribution */


}

