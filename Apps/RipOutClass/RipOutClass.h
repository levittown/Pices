#ifndef  _RIPOUTCLASS_
#define  _RIPOUTCLASS_

#include "PicesApplication.h"
#include "FeatureVector.h"
#include "FileDesc.h"
#include "MLClass.h"


class  RipOutClass:  public PicesApplication
{
public:
	RipOutClass ();
	~RipOutClass ();

  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );
  
  virtual const char* ApplicationName () const {return  "RipOutClass";}

	void	DisplayCommandLineParameters ();

  void  ExtractSpecifiedClass ();
  
  virtual bool ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                  											const KKStr&  parmValue
                                       );

private:
  bool                  cancelFlag;
  MLClassConstList      classesToRipOut;
	KKStr                 destFileName;
  FeatureVectorListPtr  destImages;
	MLClassConstList      mlClasses;
  FeatureFileIOPtr      inFileFormat;
  int                   numOfFolds;
  FeatureFileIOPtr      outFileFormat;
  bool                  stratify;
	KKStr                 srcFileName;
  FeatureVectorListPtr  srcImages;
  bool                  successful;
};

#endif