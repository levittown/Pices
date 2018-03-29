#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;

#include  "RasterSipper.h"

#include  "PicesKKStr.h"
#include  "PicesPoint.h"
#include  "PicesRunLog.h"


namespace PicesInterface 
{
  public  ref class PicesRaster
  {
  public:
    ref class  Point;

    PicesRaster (int  height,
                 int  width
                );

    PicesRaster (System::Array^   _raster);

    ///<summary> Will take ownership of "_raster"  and delete it when done. </summary>
    PicesRaster  (RasterSipperPtr  _raster);

    PicesRaster  (String^   _fileName);

    !PicesRaster ();
    ~PicesRaster ();


    System::Array^  BuildRasterArray ();

    array<array<System::Byte>^ >^  BuildRasterArray2 ();

    static
      RasterSipperPtr  BuildRasterObj (System::Array^  raster);
  


    ///<summary> Will go back to the original SIPPER file and recover the source image. </summary>
    static  PicesRaster^  GetOrigSipperImage (String^       sipperFileRootName,
                                              kkuint64      byteOffset,
                                              uint          topLeftRow,
                                              uint          topLeftCol,
                                              uint          height,
                                              uint          width,
                                              uchar         connectedComponentDist,
                                              PicesRunLog^  log
                                             );

    property String^  FileName  {String^  get ();  void  set (String^ fileName);}
    property int      Height    {int      get () {return  raster->Height ();}}
    property int      Width     {int      get () {return  raster->Width  ();}}


    enum  class  OperationType  
     {
       Null,
       BandPass,
       Binarize,
       BinarizeTH,
       Closing,
       ConnectedComponent,
       ConvexHull,
       Dilation, 
       Edge,
       Erosion,
       FillHoles,
       FourierTransform,
       Opening, 
       SmoothAveraging,
       SmoothMedium,
       Thinning
      };


     enum  class  MaskType: int  
     {
       CROSS3   = 0,
       CROSS5   = 1,
       SQUARE3  = 2,
       SQUARE5  = 3,
       SQUARE7  = 4,
       SQUARE9  = 5,
       SQUARE11 = 6
     };


     static  System::String^  OperationTypeToStr   (OperationType    ot);
     static  OperationType    OperationTypeFromStr (System::String^  otStr);

     static  System::String^        MaskTypeToStr   (MaskType         mt);
     static  PicesRaster::MaskType  MaskTypeFromStr (System::String^  mtStr);

     static  int              MaskSize (MaskType  mt);


     void    DrawGrid (float                   pixelsPerMinor,
                       int                     minorsPerMajor,
                       System::Drawing::Color  hashColor,
                       System::Drawing::Color  gridColor
                      );


     PicesRaster^     BandPass (float  lowerFreqBound,
                                float  upperFreqBound,
                                bool   retainBackground
                               );
     PicesRaster^     Binarize ();

     PicesRaster^     BinarizeByThreshold (uint  min,
                                           uint  max
                                          );

     Bitmap^          BuildBitmap ();

     PicesRaster^     CloseImage (MaskType  mt);

     bool             Color ();

     static
     float            ComputeSegmentLens (PicesPointList^  points,
                                          float            heightFactor,
                                          float            widthFactor
                                         );

     PicesRaster^     ConnectedComponent (uchar connectedComponentDist);  // returns the largest connected component in the image.

     PicesRaster^     ConvexHull ();

     PicesPointList^  DeriveImageLength ();

     PicesRaster^     DialateImage (MaskType  mt);

     PicesRaster^     EdgeImage ();

     PicesRaster^     ErodeImage (MaskType  mt);

     PicesRaster^     FillHoles ();

     PicesRaster^     FourierTransform ();

     PicesRaster^     OpenImage (MaskType  mt);

     PicesRaster^     Padded (kkint32 padding);  /**< Duplicates image with extract padding on all four sides. */

     PicesRaster^     RemoveZooscanBrackets ();

     PicesRaster^     ReversedImage ();

     void             Save (String^  fileName);

     PicesRaster^     SmoothAveraging (int  maskSize);

     PicesRaster^     SmoothMedium (int  maskSize);

     PicesRaster^     Thinning ();

     PicesRaster^     ToColor ();

     RasterSipperPtr  UnmanagedClass ()   {return  raster;}

/*
     ref class  Point
     {
     public:
       property int   Row {int get ()  {return  row;}  void  set (int  _row)  {row = _row;}}
       property int   Col {int get ()  {return  col;}  void  set (int  _col)  {col = _col;}}

       Point  (int _row, int _col):  row (_row), col (_col) {}
       
     private:
       int  row;
       int  col;
     };
*/

  private:
     RasterSipperPtr  raster;
     kkMemSize        memoryPressure;
  };  /* PicesRaster */





  public  ref  class  PicesRasterList:  List<PicesRaster^>
  {
  public:
    PicesRasterList ();

    
    ///<summary> Construct a list of managed PicesRaster instances from the contents of the unmanaged list 'unmanagedImages'. </summary>
    ///<remarks>
    /// Will take ownership of the instances of Raster in <paramref name='unmanagedImages'/>.  The list will be deleted and the unmanaged
    /// code should not try to access any of 'unmanagedImages' contents after this call.
    ///</remarks>
    ///<param name='unmanagedImages'> List of unmanaged Raster objects; the ownership of these instances will be given 
    /// over to the new Managed instance of PicesRasterList; will be deleted and set to NULL. </param>
    PicesRasterList (RasterSipperListPtr&  unmanagedImages);

    ~PicesRasterList ();

    !PicesRasterList ();

    void  AddList (PicesRasterList^ list);

    ///<summary> Give ownership of the contents of 'list' to this Managed list. </summary>
    ///<remarks>
    /// Will take ownership of the unmanaged instances of 'Raster' in 'list'.  These instances will be deleted
    /// by the ".net" garbage collector so make sure that no unmanaged code makes use of these instances after making 
    /// this call.
    ///</remarks>
    /// <param name='list'> The list of 'Raster' objects to turn ownership of; will be deleted and be set to NULL; do not
    /// make use of any of these instances in the unmanaged code after making this call. </param>
    void  AddList (RasterSipperListPtr&  list);

    void  CleanUpMemory ();

  private:
  };  /* PicesRasterList */
}
