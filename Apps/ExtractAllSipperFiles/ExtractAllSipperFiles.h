#pragma once


#include "PicesApplication.h"

#include "DataBase.h"


class ExtractAllSipperFiles: public PicesApplication
{
public:
  typedef  ExtractAllSipperFiles*  ExtractAllSipperFilesPtr;

  ExtractAllSipperFiles ();


  ~ExtractAllSipperFiles ();

  virtual 
  const char*  ApplicationName () const {return  "ExtractAllSipperFiles";}

  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );
  void  Run ();


private:
  // Will be classed by base class  'Application'  
  bool  ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                 const KKStr&  parmValue
                                );

  void  DisplayCommandLineParameters ();



  void  ProcessADir (VectorKKStr&  list,
                     KKStr         dirName
                    );

  void  RunOneImageExtraction (const KKStr&  sfn);


  KKStr             configFileName;
  MLL::DataBasePtr  dbConn;
  bool              firstOneFound;
  int               minSize;
  int               maxSize;
  KKStr             rootDir;
  KKStr             startSipperFileName;
};


typedef  ExtractAllSipperFiles::ExtractAllSipperFilesPtr  ExtractAllSipperFilesPtr;
