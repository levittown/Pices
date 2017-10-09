#include "FirstIncludes.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <math.h>
#include <limits.h>
#include <float.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "MemoryDebug.h"

using namespace std;

#include "KKBaseTypes.h"
#include "KKStr.h"
using namespace KKB;

#include "MLClass.h"
using namespace KKMLL;

#include "TrainingImage.h"



TrainingImage::TrainingImage (const KKStr&  _imageFileName,
                              MLClassPtr  _mlClass,
                              float          _probability,
                              int            _nonStreamPlace,
                              float          _nonStreamPercentile
                             ):

      imageFileName       (_imageFileName),
      mlClass             (_mlClass),
      probability         (_probability),
      nonStreamPlace      (_nonStreamPlace),
      nonStreamPercentile (_nonStreamPercentile)
{
}


TrainingImage::TrainingImage (const TrainingImage&  trainingImage):
      imageFileName       (trainingImage.imageFileName),
      mlClass             (trainingImage.mlClass),
      probability         (trainingImage.probability),
      nonStreamPlace      (trainingImage.nonStreamPlace),
      nonStreamPercentile (trainingImage.nonStreamPercentile)
{
}



TrainingImage::~TrainingImage ()
{
}



TrainingImageList::TrainingImageList (bool  _owner):
     KKQueue<TrainingImage> (_owner)
{
}
