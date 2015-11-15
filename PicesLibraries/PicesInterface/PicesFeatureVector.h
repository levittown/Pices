#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


#include  "ImageFeatures.h"

#include  "PicesClass.h"
#include  "PicesClassList.h"
#include  "PicesInstrumentData.h"
#include  "PicesRaster.h"

#include  "PicesRunLog.h"


namespace PicesInterface 
{
  public  ref class  ParsedImageFileName
  {
  public:
    ParsedImageFileName (System::String^  _sipperFileName, 
                         uint             _scanLineNum,  
                         uint             _scanColNum
                        )
    {
      sipperFileName = _sipperFileName;
      scanLineNum    = _scanLineNum;
      scanColNum     = _scanColNum;
    }

     System::String^  sipperFileName;
     uint             scanLineNum; 
     uint             scanColNum;
  };



  public  ref class PicesFeatureVector
  {
  public:
    ///<summary>Instantiates a new Creates a new PicesFeatureVector instance of picesFeatureVector by duplicating its contents.</summary>
    PicesFeatureVector (PicesFeatureVector^  picesFeatureVector);  


    
    ///<summary>
    /// Computes a feature vector image that is in "raster". This is a wrapper class for MLL::ImageFeatures allowing the managed word to 
    // work with the unmanaged world. Features that are based off instrumentation data will come from the IntrumentDataManager the 
    /// "imageFileName" will be parsed to get SipperFileName, ScanLine, and ScanCol.
    ///</summary>
    ///<param name='raster'> Raster in the form of a 2 dimensional byte array.</param>
    ///<param name='imageFileName'> Name of image that is stored in 'raster'; should follow Sipper file name conventions.</param>
    ///<param name='log'> logging file.</param>
    ///<seealso cref='MLL::ImageFeatures::CalcFeatures'/>
    PicesFeatureVector (System::Array^   raster,
                        System::String^  imageFileName,
                        PicesRunLog^     log                        
                       );


    ///<summary>
    /// Computes features from the image that is in "raster"; Features that are based off instrumentation data that will come 
    /// from the IntrumentDataManager. The "imageFileName" will be parsed to get SipperFileName and ScanLine.
    ///</summary>
    ///<seealso cref='PicesLibrary::ImageFeatures::CalcFeatures'/>.
    PicesFeatureVector (PicesRaster^     raster,
                        System::String^  imageFileName,
                        PicesRasterList^ intermediateImages,
                        PicesRunLog^     log
                       );

    PicesFeatureVector (ImageFeaturesPtr  _features);  // will take ownership of '_features'  from caller

    
    PicesFeatureVector (String^           _imageFileName,
                        PicesClass^       _mlClass,
                        PicesRasterList^  _intermediateImages,
                        PicesRunLog^      log
                       );

  private:
    ~PicesFeatureVector ();

  protected:
    !PicesFeatureVector ();


  public:
    property float             AreaMMSquare       {float       get ();}
    property float             CentroidCol        {float       get ();}
    property float             CentroidRow        {float       get ();}
    property String^           ClassName          {String^     get ();}
    property System::DateTime  CtdDateTime        {System::DateTime    get ();}
    property float             Depth              {float       get ();  void  set (float        _depth);}
    property bool              FeatureDataMissing {bool        get ();}  // If feature values are 0.0f then will return 'true' else 'false'.
    property float             FilledArea         {float       get ();}
    property PicesClass^       MLClass            {PicesClass^ get ();  void  set (PicesClass^  _mlClass);}
    property String^           ExampleFileName    {String^     get ();  void  set (String^      _exampleFileName);}
    property int               NumFeatures        {int         get ();}
    property float             OrigSize           {float       get ();}
    property float             Probability        {float       get ();  void  set (float        _probability);}
    property bool              Validated          {bool        get ();  void  set (bool         _validated);}



    void  AddInstrumentData (System::Single^  depth,
                             System::Single^  salinity, 
                             System::Single^  oxygen, 
                             System::Single^  fluorescence
                            );

    void  AddInstrumentData (PicesInstrumentData^  pid);
    
    
    ImageFeaturesPtr  Features ()  {return  features;}


    String^  FeatureName (int featureNum);


    double  FeatureValue (int featureNum);


    static  ParsedImageFileName^  ParseImageFileName (System::String^  fullFileName);


    static  void  ParseImageFileName (String^   _fullFileName,
                                      String^%  _sipperFileName,
                                      uint%     _scanLine,
                                      uint%     _scanCol
                                     );

    ImageFeaturesPtr  UnManagedClass ()  {return  features;}



  private:
    void  CleanUpUnmanagedResources ();


    ImageFeaturesPtr  features;
    PicesClass^       mlClass;
  };  /* PicesFeatureVector */
}  /* PicesInterface */