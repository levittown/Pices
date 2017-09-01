#ifndef  _SIPPEREXTRACTIONIMAGEMANAGER_
#define  _SIPPEREXTRACTIONIMAGEMANAGER_

#include "Raster.h"
#include "KKStr.h"

#include "MLClass.h"

#include "MLLTypes.h"
#include "ImageFeatures.h"
#include "SizeDistribution.h"


namespace  MLL
{
  class  SipperExtractionImageManager
  {
  public:
    class  ManagedClass;
    typedef  ManagedClass* ManagedClassPtr;
    typedef  map<MLClassPtr, ManagedClassPtr>  ManagedClassList;
    typedef  ManagedClassList*  ManagedClassListPtr;


    SipperExtractionImageManager (FileDescConstPtr  _fileDesc,
                                  const KKStr&      _rootDir,
                                  bool              _saveFeatureData,
                                  kkint32           _veryLargeImageSize,
                                  bool              _imagesAreClassified,
                                  bool              _countOnly,
                                  kkuint32          _imagesPerDirectory,
                                  RunLog&           _log
                                 );

    ~SipperExtractionImageManager ();

    void  AddImage (const KKStr&     fileName,
                    MLClassPtr       predClass,
                    kkint32          size,
                    float            depth,
                    ImageFeaturesPtr example,
                    RasterPtr        raster,
                    bool             colorize
                   );


    ClassStatisticListPtr  ClassStats ()  const;

    void  Flush ();

    void  ReportStatistics (ostream&  r)  const;

    void  ReportStatsByDepth (ostream&      r,
                              VectorUlong*  scanLinesPerMeterDepth
                             )  const;

    void  SaveFrame (const KKStr&     _fileName,
                           RasterPtr  _raster,
                           bool       colorize
                    );

    vector<ManagedClassPtr>  SortByClassName ()  const;


  private:
    ManagedClassPtr  ManagedClassLookUp (MLClassPtr       mlClass);

    ImageFeaturesListPtr  allExamples;
    bool                  countOnly;
    SizeDistribution      depthDistribution_1;     /**< using a SizeDistribution structure to implement counts by depth by 1 meter increments.  */
    SizeDistribution      depthDistribution_10;    /**< using a SizeDistribution structure to implement counts by depth by 10 meter increments. */
    FileDescConstPtr      fileDesc;
    bool                  imagesAreClassified;
    kkuint32              imagesPerDirectory;
    RunLog&               log;
    ManagedClassPtr       largeImages;
    ManagedClassPtr       lastManagedClassUsed;
    ManagedClassList      managedClasses;
    KKStr                 rootDir;
    bool                  saveFeatureData;
    SizeDistribution      sizeDistributionLarge;
    SizeDistribution      sizeDistributionMed;
    SizeDistribution      sizeDistributionSmall;

    kkint32               veryLargeImageSize;

    class  ImageEntry;
    typedef  ImageEntry*          ImageEntryPtr;
    typedef  KKQueue<ImageEntry>  ImageEntryList;
    typedef  ImageEntryList*      ImageEntryListPtr;
  };  /* SipperExtractionImageManager */

  typedef  SipperExtractionImageManager* SipperExtractionImageManagerPtr;
} /* MLL */

#endif
