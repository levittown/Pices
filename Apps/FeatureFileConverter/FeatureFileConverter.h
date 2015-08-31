#ifndef  _FEATUREFILECONVERTER_
#define  _FEATUREFILECONVERTER_

#include "RunLog.h"
#include "KKStr.h"

#include "FeatureNumList.h"
#include "FileDesc.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include  "PicesApplication.h"
#include "SVMparam.h"


class  FeatureFileConverter: public PicesApplication
{
public:
  typedef  KKB::uint  uint;

	FeatureFileConverter ();

	~FeatureFileConverter();

	virtual const char* ApplicationName () const {return  "FeatureFileConverter";}

  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );

  void  ConvertData ();

	void	DisplayCommandLineParameters ();

	virtual bool ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                        const KKStr&  parmValue
                                       );
private:
  void  EncodeFeatureData ();

  void  NormalizeExamples (ModelParam&         param,
                           FeatureVectorList&  examples
                          );

  bool                  cancelFlag;

  FeatureVectorListPtr  data;

  FileDescPtr           destFileDesc;

  FeatureFileIOPtr      destFileFormat;
  KKStr                 destFileName;

  bool                             encodeFeatureData;
  ModelParam::EncodingMethodType   encodingMethod;

  bool                             enumerateClasses;

  FeatureNumListPtr     features;    // When equial NULL means All features.
  KKStr                 featureStr;  

  MLClassList   mlClasses;

  bool                  normalizeData;
  KKStr                 nornParmsFileName;

  ostream*              report;
  ofstream*             reportFile;
  KKStr                 reportFileName;

  FileDescPtr           srcFileDesc;

  KKStr                 srcFileName;
  FeatureFileIOPtr      srcFileFormat;

  bool                  statistics;
};

#endif
