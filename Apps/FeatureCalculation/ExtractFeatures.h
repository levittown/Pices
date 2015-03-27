#ifndef  _EXTRACTFEATURES_
#define  _EXTRACTFEATURES_


#include "DataBase.h"
#include "FileDesc.h"
#include "ImageFeatures.h"
#include "MLClass.h"
#include "PicesApplication.h"


class  ExtractFeatures: public  PicesApplication
{
public:
  typedef  KKB::uint  uint;

  ExtractFeatures ();
  
  ~ExtractFeatures ();

  virtual 
  const char*  ApplicationName () const  {return  "ExtractFeatures";}

  virtual
  void  InitalizeApplication (kkint32   argc,
                              char**    argv
                             );

  void  ClassifyImage (ImageFeaturesPtr  image);

  void  Extract ();

  void  DisplayCommandLineParameters ();


  virtual 
  bool  ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                 const KKStr&  parmValue
                                );


private:
  void  AddInstrumentData (ImageFeaturesListPtr  data);

  void  ClassifyImages ();

  void  MoveImagesToDestinationDirectoryByClassName ();

  // One time only function to separate out images that originated from SIPPER 
  // file "SMP751001037_22".  This was the one SIPPER file that andrew extracted images
  // at a lower threshol;d than 250.
  void  MoveOver_SMP751001037_22_ImagesIntoSeparateDir (ImageFeaturesListPtr  examples);

  void  ReportDuplicates ();

  void  ReFreshInstrumentData (ImageFeaturesPtr  _featureVector);


  bool                      cancelFlag;
  Classifier2Ptr            classifier;
  KKStr                     destDirectory;
  KKStr                     featureFileName;
  KKStrListPtr              fileNameList;
  MLClassConstPtr           mlClass;
  MLClassConstListPtr       mlClasses;
  ImageFeaturesListPtr      images;
  bool                      purgeDuplicates;
  ostream*                  report;
  ofstream*                 reportFile;
  KKStr                     reportFileName;
  bool                      retrieveFeaturesFromFile;
  KKStr                     rootFeatureFileName;
  KKStr                     sourceRootDirPath;
  KKStr                     startDirectory;
  KKStr                     statusMessage;
  bool                      successful;
  TrainingProcess2Ptr       trainer;
  bool                      useDirectoryNameForClassName;
};





#endif