#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
using namespace std;

#include "MemoryDebug.h"
#include "BasicTypes.h"
using namespace KKU;


#include "ImageSizeDistribution.h"
using namespace MLL;

#include "PicesKKStr.h"
#include "PicesMethods.h"
#include "PicesImageSizeDistribution.h"
using namespace  PicesInterface;

using namespace System;


namespace  PicesInterface
{


  PicesImageSizeDistributionRow::PicesImageSizeDistributionRow (ImageSizeDistributionRowPtr  _imageSizeDistributionRow,
                                                                bool                         _willOwn
                                                               )
  {
    imageSizeDistributionRow = _imageSizeDistributionRow;
    owner = _willOwn;
  }


  PicesImageSizeDistributionRow::~PicesImageSizeDistributionRow ()
  {
    this->~PicesImageSizeDistributionRow ();
  }


  PicesImageSizeDistributionRow::!PicesImageSizeDistributionRow ()
  {
    if  (owner)
      delete  imageSizeDistributionRow;
    imageSizeDistributionRow = NULL;
  }

  
  array<uint>^  PicesImageSizeDistributionRow::Distribution ()
  {
    const VectorUint32&  dist = imageSizeDistributionRow->Distribution ();

    array<uint>^  result = gcnew array<uint> (dist.size ());

    for  (uint x = 0;  x < dist.size ();  ++x)
      result[x] = dist[x];
    return  result;
  }


  void  PicesImageSizeDistributionRow::AddIn (PicesImageSizeDistributionRow^  right)  
  {
    imageSizeDistributionRow->AddIn (*(right->UnmanagedClass ()));
  }



  PicesImageSizeDistribution::PicesImageSizeDistribution (ImageSizeDistributionPtr  _imageSizeDistribution)
  {
    imageSizeDistribution = _imageSizeDistribution;
  }



  PicesImageSizeDistribution::PicesImageSizeDistribution (PicesImageSizeDistribution^  _imageSizeDistribution)
  {
    imageSizeDistribution = new ImageSizeDistribution (*_imageSizeDistribution->UnmanagedClass ());
  }



  PicesImageSizeDistribution::~PicesImageSizeDistribution ()
  {
    this->!PicesImageSizeDistribution ();
  }



  PicesImageSizeDistribution::!PicesImageSizeDistribution ()
  {
    delete  imageSizeDistribution;
    imageSizeDistribution = NULL;
  }



  array<float>^  PicesImageSizeDistribution::SizeStartValues ()
  {
    const VectorFloat&  startValues = imageSizeDistribution->SizeStartValues ();
    array<float>^  a = gcnew array<float> (startValues.size ());
    for  (uint x = 0;  x <  startValues.size ();  ++x)
      a[x] = startValues[x];

    return a;
  }


  array<float>^  PicesImageSizeDistribution::SizeEndValues   ()
  {
    const VectorFloat&  endValues = imageSizeDistribution->SizeEndValues ();
    array<float>^  a = gcnew array<float> (endValues.size ());
    for  (uint x = 0;  x <  endValues.size ();  ++x)
      a[x] = endValues[x];

    return a;
  }



  void  PicesImageSizeDistribution::AddIn (PicesImageSizeDistribution^  right,
                                           PicesRunLog^                 log
                                          )
  {
    imageSizeDistribution->AddIn (*right->UnmanagedClass (), log->Log ());
  }  /* AddIn */



  PicesImageSizeDistributionRow^  PicesImageSizeDistribution::AllDepths ()
  {
    return gcnew PicesImageSizeDistributionRow (new ImageSizeDistributionRow (imageSizeDistribution->AllDepths ()), true);
  }  /* AllDepths */



  kkint32  PicesImageSizeDistribution::IdentifySizeBucket (float  size)
  {
    return  imageSizeDistribution->IdentifySizeBucket (size);
  }

}  /* PicesInterface */
