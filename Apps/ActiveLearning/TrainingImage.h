#ifndef  _TRAININGIMAGE_
#define  _TRAININGIMAGE_

#include "KKQueue.h"
#include "KKStr.h"

#include "MLClass.h"

using namespace  KKMLL;

class TrainingImage
{
public:
  TrainingImage (const KKStr&  _imageFileName,
                 MLClassPtr     _mlClass,
                 float          _probability,
                 int            _nonStreamPlace,
                 float          _nonStreamPercentile
                );

  TrainingImage (const TrainingImage&  _trainingImage);

  ~TrainingImage ();


  MLClassPtr     MLClass             ()  {return mlClass;}

  const
    KKStr&       ImageFileName       ()  {return imageFileName;}

  int            NonStreamPlace      ()  {return nonStreamPlace;}

  float          NonStreamPercentile ()  {return nonStreamPercentile;}

  float          Probability         ()  {return probability;}



private:
  MLClassPtr  mlClass;
  float       probability;    // Probability example had 
  KKStr       imageFileName;
  int         nonStreamPlace;
  float       nonStreamPercentile;
};


typedef  TrainingImage*  TrainingImagePtr;


class  TrainingImageList:  public KKQueue<TrainingImage>
{
public:
  TrainingImageList (bool _owner = true);
};

typedef  TrainingImageList*  TrainingImageListPtr;

#endif
