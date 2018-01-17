#pragma once
#if  !defined(_BUILDSYNTHOBJDET_)
#define  _BUILDSYNTHOBJDET_

#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;

#include "DataBase.h"
#include "PicesApplication.h"

using namespace  MLL;;

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

    int  Main (int argc, char** argv);

  private:

    void   DisplayCommandLineParameters ();

    DataBaseImageListPtr  GetListyOfValidatedImages (
      float    minSize, 
      float    maxSize, 
      kkuint32 restartImageId, 
      kkint32  limit);


    virtual
    bool   ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                    const KKStr&  parmValue
                                   );

    bool     cancelFlag;

    kkint32  maxCandidates;
  };  /* FullSizeImagesInstall */
}  /* FullSizeImagesInstall_DataNameSpace */

#endif
