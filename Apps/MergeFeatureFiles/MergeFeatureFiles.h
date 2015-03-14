#ifndef  _MERGEFEATUREFILES_
#define  _MERGEFEATUREFILES_

#include "PicesApplication.h"
#include "DataBase.h"
#include "FeatureFileIO.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "RandomNumGenerator.h"
#include "RunLog.h"
#include "SipperFile.h"
#include "KKStr.h"




class  MergeFeatureFiles:  public PicesApplication
{
public:
  typedef  KKB::uchar  uchar;
  typedef  KKB::uint   uint;
  typedef  KKB::ulong  ulong;
  typedef  KKB::ushort ushort;

  MergeFeatureFiles ();

  ~MergeFeatureFiles ();

  virtual 
  const char*  ApplicationName () const  {return  "MergeFeatureFiles";}

  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );

  void  Main ();

private:
  void   DisplayCommandLineParameters ();

  bool   ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                  const KKStr&  parmValue
                                 );

  FeatureFileIOPtr           currentDefaultFormat;    // Will change as each '-format' option is processed.

  KKStr                      destFileName;
  FeatureFileIOPtr           destFormat;

  int                        numOfFolds;

  bool                       randomize;

  VectorKKStr                srcFileNames;
  vector<FeatureFileIOPtr>   srcFormats;

  FeatureVectorListPtr       srcData;

  bool                       stratify;


};
#endif
