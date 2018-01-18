#pragma once
#if  !defined(_BUILDSYNTHOBJDET_)
#define  _BUILDSYNTHOBJDET_

#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;

#include "DataBase.h"
#include "PicesApplication.h"

using namespace  MLL;;

namespace  PicesUtilityApps
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

    DataBaseImageListPtr  GetListOfValidatedImages (
      float    minSize, 
      float    maxSize, 
      kkuint32 restartImageId, 
      kkint32  limit);

    RasterPtr  ReduceToMinimumSize (RasterPtr&  src);


    void  PopulateRaster (Raster&  raster, DataBaseImageList& workingList, int numToPlace);


    virtual
    bool   ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                    const KKStr&  parmValue
                                   );

    bool     cancelFlag;

    kkint32  maxCandidates;

    RandomNumGenerator rng;
  };  /* BuildSynthObjDetData */
}  /* PicesUtilityApps */

#endif
