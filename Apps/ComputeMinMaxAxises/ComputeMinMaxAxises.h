#ifndef  _COMPUTEMMINMAXAXISES_
#define  _COMPUTEMMINMAXAXISES_

#include "PicesApplication.h"
#include "DataBase.h"
#include "FeatureFileIO.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "RandomNumGenerator.h"
#include "RunLog.h"
#include "SipperFile.h"
#include "KKStr.h"


class  ComputeMinMaxAxises : public PicesApplication
{
public:
	ComputeMinMaxAxises ();

	~ComputeMinMaxAxises ();

	virtual
		const char*  ApplicationName () const { return  "ComputeMinMaxAxises"; }

	virtual
    void  InitalizeApplication (kkint32 argc,
                               char**  argv
		);
	void  Main ();

private:
	void DisplayCommandLineParameters ();

  bool ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                const KKStr&  parmValue
                               );

  KKStr  cruiseName;
  KKStr  deploymentNum;
  KKStr  stationName;
  KKStr  reportFileName;
};
#endif
