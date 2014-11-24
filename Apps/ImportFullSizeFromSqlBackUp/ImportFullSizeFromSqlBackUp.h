#ifndef  _IMPORTFULLSIZEFROMSQLBACKUP_
#define  _IMPORTFULLSIZEFROMSQLBACKUP_

#include "PicesApplication.h"
#include "DataBase.h"
#include "RunLog.h"


class  ImportFullSizeFromSqlBackUp: public PicesApplication
{
public:

  ImportFullSizeFromSqlBackUp ();

  ~ImportFullSizeFromSqlBackUp ();

  const char*  ApplicationName () const  {return "ImportFullSizeFromSqlBackUp";}

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

  KKStr  srcFileName;

};
#endif
