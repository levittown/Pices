#ifndef  _FEATUREFILESTATS_
#define  _FEATUREFILESTATS_

#include  "PicesApplication.h"
#include  "RunLog.h"
#include  "KKStr.h"

#include  "FileDesc.h"
#include  "MLClass.h"

#ifndef  _IMAGEFEATURES_
class    ImageFeatures;
typedef  ImageFeatures*  ImageFeaturesPtr;
class    FeatureVectorList;
typedef  FeatureVectorList*  FeatureVectorListPtr;
#endif



class  FeatureFileStats: public PicesApplication
{
public:

	FeatureFileStats ();

	~FeatureFileStats();

	virtual const char* ApplicationName ()  const  {return  "FeatureFileStats";}

  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );

  void  ReportStats ();

	void	DisplayCommandLineParameters ();

	virtual bool ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                        const KKStr&  parmValue
                                       );
private:
  bool                  cancelFlag;
  FeatureVectorListPtr  data;

  MLClassConstList      mlClasses;

	ostream*              report;
	ofstream*             reportFile;
	KKStr                 reportFileName;

  KKStr                 srcFileName;
  FeatureFileIOPtr      srcFileFormat;
};

#endif
