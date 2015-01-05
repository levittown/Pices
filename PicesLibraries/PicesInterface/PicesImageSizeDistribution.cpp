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


  PicesImageSizeDistribution::PicesImageSizeDistribution (ImageSizeDistributionPtr  _imageSizeDistribution)
  {
    imageSizeDistribution = _imageSizeDistribution;
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


}  /* PicesInterface */
