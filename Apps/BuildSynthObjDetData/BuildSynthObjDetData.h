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

    class  BoundBoxEntry
    {
    public:
      BoundBoxEntry (kkint32 _topLeftRow, kkint32 _topLeftCol, kkint32 _height, kkint32 _width, MLClassPtr _mlClass);
      BoundBoxEntry (const BoundBoxEntry& bbe);

      KKStr ToJsonStr ()  const;

      kkint32 topLeftRow;
      kkint32 topLeftCol;
      kkint32 height;
      kkint32 width;
      MLClassPtr mlClass;
    };

    class  BoundBoxEntryList : public KKQueue<BoundBoxEntry>
    {
    public:
      BoundBoxEntryList ();
      BoundBoxEntryList (const BoundBoxEntryList& list);

      KKStr ToJsonStr ()  const;
    };


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
      kkint32  minSize, 
      kkint32  maxSize,
      kkuint32 restartImageId, 
      kkint32  limit);

    RasterPtr  ReduceToMinimumSize (RasterPtr&  src);


    struct PopulateRasterResult
    {
      PopulateRasterResult (RasterPtr  _raster, BoundBoxEntryList* _boundBoxes) :
        raster (_raster), boundBoxes (_boundBoxes)
      {}

      ~PopulateRasterResult ()
      {
        delete raster; raster = NULL;
        delete boundBoxes; boundBoxes = NULL;
      }

      RasterPtr          raster;
      BoundBoxEntryList* boundBoxes;
    };


    PopulateRasterResult*  PopulateRaster (DataBaseImageList& workingList, int numToPlace);

    bool SeeIfItFits (Raster& target, const Raster& obj, Raster& marker, kkint32 topLeftRow, kkint32 topLeftCol);

    virtual
    bool   ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                    const KKStr&  parmValue
                                   );

    bool     cancelFlag;
    kkint32  imageHeight;
    kkint32  imageWidth;
    kkint32  maxCandidates;
    kkint32  maxImages;
    KKStr    targetDir;

    RandomNumGenerator rng;
  };  /* BuildSynthObjDetData */
}  /* PicesUtilityApps */

#endif
