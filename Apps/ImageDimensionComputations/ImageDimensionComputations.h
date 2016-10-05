#ifndef  _EXTRACTFEATURES_
#define  _EXTRACTFEATURES_


#include "DataBase.h"
#include "FileDesc.h"
#include "ImageFeatures.h"
#include "MLClass.h"
#include "PicesApplication.h"
#include "SipperFile.h"


class  ImageDimensionComputations: public  PicesApplication
{
public:
  typedef  KKB::uint  uint;

  ImageDimensionComputations ();
  
  ~ImageDimensionComputations ();

  virtual 
  const char*  ApplicationName () const  {return  "ImageDimensionComputations";}

  virtual
  void  InitalizeApplication (kkint32   argc,
                              char**    argv
                             );

  void  Extract ();

  void  DisplayCommandLineParameters ();


  virtual 
  bool  ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                 const KKStr&  parmValue
                                );


private:
  void  ProcessDeployment (SipperDeploymentPtr  deployment);

  void  ProcessImage(SipperFilePtr sf,
                     const KKStr&   imageFileName
                    );

  void ProcessSipperFile (SipperFilePtr sf);


  bool       cancelFlag;
  ostream*   report;
  ofstream*  reportFile;
  KKStr      reportFileName;
  bool       successful;
};





#endif
