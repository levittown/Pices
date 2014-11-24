#ifndef  _REFRESHINSTRUMENTDATA_
#define  _REFRESHINSTRUMENTDATA_

#include "PicesApplication.h"
#include "DataBase.h"
#include "FeatureFileIO.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "RandomNumGenerator.h"
#include "RunLog.h"
#include "SipperFile.h"
#include "Str.h"




class  ReFreshInstrumentData: public PicesApplication
{
public:
  ReFreshInstrumentData ();

  ~ReFreshInstrumentData ();

  virtual 
  const char*  ApplicationName () const  {return  "ReFreshInstrumentData";}

  virtual
  void  InitalizeApplication (int32   argc,
                              char**  argv
                             );
  void  Main ();

private:
  void   DisplayCommandLineParameters ();

  bool   ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                  const KKStr&  parmValue
                                 );

  KKStr  cruiseName;
  KKStr  deploymentNum;
  KKStr  stationName;

  KKStr  reportFileName;
};
#endif
