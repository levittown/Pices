#ifndef  _DUMPSIPPER3INSTRUMENTDATA_
#define  _DUMPSIPPER3INSTRUMENTDATA_


#include "PicesApplication.h"



class DumpSipper3InstrumentData:  public PicesApplication
{
public:
  DumpSipper3InstrumentData ();

  ~DumpSipper3InstrumentData ();


  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );

  virtual const 
  char*   ApplicationName () const {return  "DumpSipper3InstrumentData";}



  void  ProcessSipperFile ();


private:

  void  DisplayCmdLine ();



  uchar  GetNextSipperRec (bool&  moreRecs);

  void  OpenSipperFile ();


  virtual 
  bool   ProcessCmdLineParameter (char    parmSwitchCode, 
                                  KKStr  parmSwitch, 
                                  KKStr  parmValue
                                 );


  int        byteOffset;
  KKStr      reportFileName;
  ofstream*  report;
  FILE*      sipperFile;
  KKStr      sipperFileName;
};


typedef  DumpSipper3InstrumentData*  DumpSipper3InstrumentDataPtr;


#endif