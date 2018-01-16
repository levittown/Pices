#pragma once
#if  !defined(_BUILDSYNTHOBJDET_)
#define  _BUILDSYNTHOBJDET_

#include "PicesApplication.h"
#include "DataBase.h"
#include "RunLog.h"
#include "KKStr.h"


namespace  PicesUtilittyApps
{
  /**
  */
  class  BuildSynthObjDetData : public PicesApplication
  {
  public:
    BuildSynthObjDetData ();

    virtual
    ~BuildSynthObjDetData ();

    virtual
      const char*  ApplicationName () const { return  "BuildSynthObjDetData"; }

    virtual
      void  InitalizeApplication (kkint32 argc,
                                  char**  argv
      );

    void  Main ();

  private:

    void   DisplayCommandLineParameters ();

    virtual
    bool   ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                    const KKStr&  parmValue
                                   );

    bool                        cancelFlag;
  };  /* FullSizeImagesInstall */
}  /* FullSizeImagesInstall_DataNameSpace */

#endif
