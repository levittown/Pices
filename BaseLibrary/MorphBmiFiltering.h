/* MorphBmiFiltering.cpp -- Morphological operator used to Binarize image.
 * Copyright (C) 1994-2014 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */
#if !defined(_MORPHBMIFILTERING_)
#define  _MORPHBMIFILTERING_

#include "BasicTypes.h"
#include "MorphOp.h"


#if  !defined(_RASTER_)
namespace  KKB
{
  class  Raster;
  typedef  Raster*  RasterPtr;
  typedef  Raster const *  RasterConstPtr;
}
#endif



namespace  KKU
{
  class  MorphBmiFiltering :  public MorphOp
  {
  public:
    MorphBmiFiltering ();
    
    virtual ~MorphBmiFiltering ();

    virtual  OperationType   Operation ()  const  {return  moBinarize;}

    virtual  RasterPtr   PerformOperation (RasterConstPtr  _image);

    kkint32  MemoryConsumedEstimated ()  const;

    float  EigenRatio        () const  {return eigenRatio;}
    float  OrientationAngle  () const  {return orientationAngle;}

    int32  FirstQtrWeight    () const  {return firstQtrWeight;}
    int32  ForthQtrWeight    () const  {return forthQtrWeight;}

    int32  BeltWidth         () const  {return beltWidth;}
    int32  BoundingLength    () const  {return boundingLength;}
    int32  BoundingWidth     () const  {return boundingWidth;}

    float  LengthVsWidth     () const  {return lengthVsWidth;}

    float  HeadTailRatio     () const  {return headTailRatio;}

    float  EstimatedBmi      () const  {return estimatedBmi;}


  private:
    RasterPtr   ProcessBmi (uchar  minBackgroundTH);

    float  eigenRatio;
    float  orientationAngle;

    int32  firstQtrWeight;
    int32  forthQtrWeight;

    int32  beltWidth;
    int32  boundingLength;
    int32  boundingWidth;

    float  lengthVsWidth;

    float  headTailRatio;

    float  estimatedBmi;
  };

  typedef  MorphBmiFiltering*  MorphBmiFilteringPtr;

#define _MorphBmiFiltering_Defined_
};  /* KKB */



#endif
