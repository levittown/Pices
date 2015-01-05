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
  public  ref class PicesImageSizeDistribution  
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
    property  int     NumDepthBins    {int     get ()  {return  imageSizeDistribution->->NumDepthBins ();}}

    MLL::ImageSizeDistributionPtr  UnmanagedClass ()  {return  imageSizeDistribution;}

  private:
    MLL::ImageSizeDistributionPtr  imageSizeDistribution;
  };  /* PicesImageSizeDistribution */


}

