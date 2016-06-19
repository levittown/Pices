#if  !defined(_DATABASE_)
#define  _DATABASE_


#include "DateTime.h"
#include "GoalKeeper.h"
#include "KKStrMatrix.h"
#include "RasterSipper.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;



#include "ClassStatistic.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "DataBaseImage.h"
#include "DataBaseImageGroup.h"
#include "DataBaseImageGroupEntry.h"
#include "DataBaseImageValidatedEntry.h"
#include "DataBaseLogEntry.h"
#include "GpsDataPoint.h"
#include "ImageSizeDistribution.h"
#include "InstrumentData.h"
#include "InstrumentDataList.h"
#include "InstrumentDataMeans.h"
#include "SipperCruise.h"
#include "SipperDeployment.h"
#include "SipperFile.h"
#include "SipperStation.h"
#include "VolumeSampledStat.h"
using namespace  MLL;



#ifndef  _mysql_h
struct  MYSQL;
struct  MYSQL_RES;
struct  MSQL_STRUCT; 
typedef  MSQL_STRUCT* MYSQL_ROW;
struct  MYSQL_FIELD;
#endif

namespace MLL 
{

  #ifndef  _IMAGEFEATURES_
  class  ImageFeatures;
  typedef  ImageFeatures*  ImageFeaturesPtr;

  class  ImageFeaturesList;
  typedef  ImageFeaturesList*  ImageFeaturesListPtr;
  #endif


  #if  !defined(_DATABASESERVER_)
  class  DataBaseServer;
  typedef  DataBaseServer*         DataBaseServerPtr;
  typedef  DataBaseServer const *  DataBaseServerConstPtr;

  class  DataBaseServerList;
  typedef  DataBaseServerList*  DataBaseServerListPtr;
  #endif


  /**
   @class  DataBase
   @brief DataBase connection management object.  All database communications will go through this class.
   @author Kurt Kramer
   @date Dec/01/2008
   */
  class  DataBase
  {
  public:
    typedef  DataBase*  DataBasePtr;
    typedef  DataBase const *  DataBaseConstPtr;

    typedef  KKB::uchar   uchar;
    typedef  KKB::ulong   ulong;
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::kkint64 kkint64;
    typedef  KKB::kkuint64  kkuint64;

    static  void  Initialization ();
    static  void  FinalCleanUp ();

    static  bool  EmbeddedServerRunning ();  /**< Indicates that the Embedded server has been initialize and consuming resources */

    static  DataBaseServerListPtr  GetListOfServers (RunLog&  _log);  /**< Returns a list of previously defined MySQL servers that we can connect to. */


   
    static  DataBaseServerPtr  GetDefaultServer (RunLog&  _log);   /**< Returns an instance of the current server;  caller will own it. */


    /** @brief  returns an instance of the server with 'description';  caller will own it. */
    static  DataBaseServerPtr  GetServer (const KKStr&  description,
                                          RunLog&       _log
                                         );

    static  void   ErrorLogMsg (const KKStr&  msg);


    DataBase (RunLog&  _log);


    DataBase (DataBaseServerConstPtr  _server,   /**< Will make own local copy of "_server" */
              RunLog&                 _log
             );

    ~DataBase ();

    void  Close ();

    KKStr  ServerDescription ()  const;   /**< return description of server that this instance is connected to.  */

    KKStr  LastErrorDesc ()  const;

    bool                     DuplicateKey () const {return duplicateKey;}
    const DataBaseServerPtr  Server       () const {return server;}
    bool                     Valid        () const {return valid;}



    kkint32  QueryStatement (const char* queryStr,
                             kkint32     queryStrLen
                            );

    kkint32  QueryStatement (const KKStr&  statement);



    ///  Will make MySQL call contained in 'statement'.  and return back results as a Vector List of KKStr objects.
    ///  colsToReturn  - A list of columns to return from the result set.
    KKStrMatrixPtr  QueryStatementKKStr (const KKStr&        statement,
                                         ConstCharStarArray  colsToReturn
                                        );

    /// Will make SQL Query call; if there is a disconnect from the database;  it will not try to
    /// Reconnect like the other 'QueryStatement2' calls.
    kkint32  QueryStatement2 (const char*  queryStr,
                              kkint32      queryStrLen
                             );

    KKStrMatrixPtr  QueryStatementReturnAllColumns (const char*   statement,
                                                    kkint32       statementLen,
                                                    VectorKKStr&  columnsReturned
                                                   );

    static bool  ThreadInit ();  /**< Call at start of new thread.        */

    static void  ThreadEnd ();   /**< Call just before thread terminates. */



    //***********************************************************************************
    void  FeatureDataCreateTable ();

    void  FeatureDataInsertRow (const KKStr&          _sipperFileName,
                                const ImageFeatures&  example
                               );
    

    /// Retrieves FeatureData for a given 'DataBaseImage' object.  It will also refresh the Instrument data fields
    /// of the 'ImageFeatures' objects from the 'InstruentData' table
    ImageFeaturesPtr      FeatureDataRecLoad (DataBaseImagePtr  image);


    /// Retrieves FeatureData for a given ImageFileName It will also refresh the Instrument data fields
    /// of the 'ImageFeatures' objects from the 'InstruentData' table
    ImageFeaturesPtr      FeatureDataRecLoad (const KKStr&  imageFileName);



    /**
     *@brief Returns a List of FeatureVector's for specified SipperFile and if specified for Specified ImaegClass.
     *@details The retrieval is driven by the Images table and a Left-Join is done with the FeatureData table; so if
     * there is no corresponding FeatureData table entry for a image the FeaterData will be filled with 0's.
     *@param[in] classKeyToUse  If equal to 'V' will only return Validated examples.
     *@param[in] reExtractInstrumentData If set to true will Re-Extract Instrument data from the SipperFile.
     */
    ImageFeaturesListPtr  FeatureDataGetOneSipperFile (const KKStr&  sipperFileRootName,
                                                       MLClassPtr    mlClass,
                                                       char          classKeyToUse,
                                                       bool          reExtractInstrumentData,
                                                       bool&         cancelFlag
                                                      );

    ImageFeaturesListPtr  FeatureDataForImageGroup (const DataBaseImageGroupPtr  imageGroup,
                                                    MLClassPtr                   mlClass,
                                                    char                         classKeyToUse,
                                                    const bool&                  cancelFlag
                                                   );

    void   FeatureDataUpdate (DataBaseImagePtr  dataBaseImage,
                              ImageFeaturesPtr  example
                             );

    void   FeatureDataUpdateInstrumentDataFields (const KKStr&       imageFileName,
                                                  InstrumentDataPtr  instumentData
                                                 );

    std::vector<ulong>*  FeatureDataGetScanLinesPerMeterProfile (const KKStr&  sipperFileName);





    //***********************************************************************************
    void    GpsDataInsert (const KKStr&         cruiseName,
                           const GPSDataPoint&  gpsData
                          );

    GPSDataPointListPtr   GpsDataQuery (const KKStr&          cruiseName,
                                        const KKB::DateTime&  utcDateTimeStart,
                                        const KKB::DateTime&  utcDateTimeEnd
                                       );

    /**
     *@brief  Returns Summary GPSData for a specified Cruises and time range.
     *@details  Each entry returned will be the average values for the range of entries that are with in 'timeInterval' seconds.
     *  This is done by grouping by using the function      "Floor(Unix_Time(GpsUtcTime) / timeInterval)"
     */
    GPSDataPointListPtr   GpsDataQueryByIntervals (const KKStr&          cruiseName,
                                                   const KKB::DateTime&  utcDateTimeStart,
                                                   const KKB::DateTime&  utcDateTimeEnd,
                                                   kkint32               timeInterval
                                                  );


    void  GpsDataDelete (const KKStr&          cruiseName,
                         const KKB::DateTime&  utcDateTimeStart,
                         const KKB::DateTime&  utcDateTimeEnd
                        );




    //***********************************************************************************



    ///  Creates and entry in the Images table for 'image',
    ///  if either dimension of 'image' exceeds 100 it will save a reduced version of it in the thumbNail field of 'Images' 
    ///  and a full size version in the ImagesFullSize table.
    void  ImageInsert (const RasterSipper&    image,
                       const KKStr&           imageFileName,
                       const KKStr&           sipperFileName,
                             kkuint64         byteOffset,     // byteOffset of SipperRow containing TopLeftRow
                             kkuint32         topLeftRow,
                             kkuint32         topLeftCol,
                             kkuint32         height,
                             kkuint32         width,
                             kkuint32         pixelCount,
                             uchar            connectedPixelDist,
                             kkuint32         extractionLogEntryId,
                             kkuint32         classLogEntryId,
                             kkuint32         centroidRow,
                             kkuint32         centroidCol,
                             MLClassPtr       class1,
                             float            class1Prob,
                             MLClassPtr       class2,
                             float            class2Prob,
                             MLClassPtr       validatedClass,
                             float            depth,
                             float            imageSize,
                             PointListPtr     sizeCoordinates,
                             kkint32&         imageId,
                             bool&            successful
                       );


    /**
     *@brief Erases all data related to a single SipperFile except the entry in the SipperFiles table.
     *@details  Calls stored procedure 'ImagesEraseSipperFile' which will delete all data related to 
     * sipper file '_sipperFileName' from the tables 'ImagesFullSize', 'ImageGroupEntries', 
     * 'InstrumentData', 'FeatureData', and 'Images'.
     */
    void  ImagesEraseSipperFile (const KKStr&  _sipperFileName);

    void  ImagesEraseSipperFileSizeRange (const KKStr&  _sipperFileName,
                                          kkuint32      _minSize,
                                          kkuint32      _maxSize
                                         );

    DataBaseImagePtr      ImageLoad (kkuint32  imageId);  

    DataBaseImagePtr      ImageLoad (const KKStr&   imageFileName);  // Root name of image.

    DataBaseImagePtr      ImagesLocateClosestImage (const KKStr&  imageFileName);

    DataBaseImagePtr      ImagesLocateClosestImage (const KKStr&  sipperFileName,
                                                    kkuint32      scanLine,
                                                    kkuint32      scanCol
                                                   );


    DataBaseImageListPtr  ImagesQuery (DataBaseImageGroupPtr  group,
                                       bool                   includeThumbnail,
                                       const bool&            cancelFlag
                                      );
      
   
    DataBaseImageListPtr  ImagesQuery (DataBaseImageGroupPtr  imageGroup,
                                       const KKStr&           sipperFileName,
                                       MLClassPtr             mlClass,
                                       char                   classKeyToUse,   /**< 'P' - Use Predicted Class Key,  'V' - Validated Class */
                                       float                  probMin,
                                       float                  probMax,
                                       kkint32                sizeMin,
                                       kkint32                sizeMax,
                                       float                  depthMin,
                                       float                  depthMax,
                                       kkuint32               restartImageId,
                                       kkint32                limit,            // Max # of rows to return.  -1 indicates no limit.
                                       bool                   includeThumbnail,
                                       const bool&            cancelFlag
                                      );



    DataBaseImageListPtr  ImagesQuery (DataBaseImageGroupPtr imageGroup,
                                       const KKStr&          cruiseName,
                                       const KKStr&          stationName,
                                       const KKStr&          deploymentNum,
                                       MLClassPtr            mlClass,
                                       char                  classKeyToUse,
                                       float                 probMin,
                                       float                 probMax,
                                       kkint32               sizeMin,
                                       kkint32               sizeMax,
                                       float                 depthMin,
                                       float                 depthMax,
                                       kkuint32              restartImageId,
                                       kkint32               limit,            // Max # of rows to return.  -1 indicates no limit.
                                       bool                  includeThumbnail,
                                       const bool&           cancelFlag
                                      );


    DataBaseImageListPtr  ImagesQuery (const KKStr&    cruiseName,
                                       const KKStr&    stationName,
                                       const KKStr&    deploymentNum,
                                       MLClassPtr      mlClass,
                                       char            classKeyToUse,
                                       float           minProb,
                                       float           minSize,
                                       const KKStr&    dataField1Name,
                                       float           dataField1Min,
                                       float           dataField1Max,
                                       const KKStr&    dataField2Name,
                                       float           dataField2Min,
                                       float           dataField2Max,
                                       const KKStr&    dataField3Name,
                                       float           dataField3Min,
                                       float           dataField3Max,
                                       kkuint32        restartImageId,
                                       kkint32         limit            // Max # of rows 2 return.  -1 indicates no limit.
                                      );


    DataBaseImageListPtr  ImagesQueryByGrouop 
                                    (DataBaseImageGroupPtr  imageGroup,
                                     const KKStr&           cruiseName,
                                     const KKStr&           stationName,
                                     const KKStr&           deploymentNum,
                                     const KKStr&           sipperFileName,
                                     MLClassPtr             mlClass,
                                     char                   classKeyToUse,
                                     float                  probMin,
                                     float                  probMax,
                                     kkint32                sizeMin,
                                     kkint32                sizeMax,
                                     float                  depthMin,
                                     float                  depthMax,
                                     kkuint32               restartImageId,
                                     kkint32                limit,            // Max # of rows to return.  -1 indicates no limit.,
                                     bool                   includeThumbnail
                                    );
    

    DataBaseImageListPtr  ImagesQueryForScanLineRange (const KKStr&  sipperFileName,
                                                       kkuint32      scanLineStart,
                                                       kkuint32      scanLineEnd
                                                      );

    
    DataBaseImageListPtr  ImagesQueryDeploymentSizeRange (const KKStr&   cruiseName,
                                                          const KKStr&   stationName,
                                                          const KKStr&   deploymentNum,
                                                          MLClassPtr     mlClass,
                                                          char           cast,         /**< 'U' = UpCast, 'D' = DownCast,  'B' = Both' */
                                                          char           statistic,    /**< '0' = Area mm^2,  '1' = Diameter,  '2' = Spheroid Volume and '3' = EBv ((4/3)(Pie)(Major/2)(Minor/2)^2) */
                                                          float          sizeStart,
                                                          float          sizeEnd,
                                                          float          depthMin,
                                                          float          depthMax,
                                                          kkint32        sampleQty,
                                                          bool           includeChildren,
                                                          VolConstBool&  cancelFlag
                                                         );


    VectorKKStr*          ImageListOfImageFileNamesByScanLineRange (const KKStr&  sipperFileName,
                                                                    kkuint32      scanLineStart,
                                                                    kkuint32      scanLineEnd
                                                                   );




    // Will update the instrument data fields in both  'Images' and 'FeatureData' tables for 
    // sipper files and scan range specified.
    void  ImageUpdateInstrumentDataFields (InstrumentDataPtr  instumentData,
                                           const KKStr&       sipperFileName,
                                           kkuint32           scanLineStart,
                                           kkuint32           scanLineEnd
                                          );


    void  ImagesUpdatePredictions (const KKStr&  imageFileName,
                                   MLClassPtr    class1Pred,
                                   float         class1Prob,
                                   MLClassPtr    class2Pred,
                                   float         class2Prob,
                                   kkuint32      logEntryId
                                  );


    /**
     *@brief  Updates prediction on multiple images at one time.
     *@param[in]  _logEntryId Log id of process that is doing the classification.
     *@param[in]  _predictionList  A tab delimited string that will contain the list of predictions. Each image will be separated by
     *   line feed line-feed characters while each field for each usage will be separated by tabs. So if there are two images with 
     *   predictions the string will look like<br/> 
     *   ImageFileRootName | tab | Class1Name | tab |Class1Prob | tab |Class2Name | tab |Class2Prob | tab | ImageFileRootName | tab | Class1Name | tab |Class1Prob | tab | Class2Name | tab | Class2Prob"
     */
    void  ImagesUpdatePredictionsList (kkuint32      _logEntryId,
                                       const KKStr&  _predictionList
                                      );


    void  ImagesUpdateValidatedClass (const KKStr&  imageFileName,
                                      MLClassPtr    mlClass
                                     );

    void  ImagesUpdateImageSize (const KKStr&        imageFileName,
                                 const PointListPtr  sizeCoordinates,
                                 float               imageSize
                                );

    void  ImageRemoveValidation (const KKStr&  imageFileName);

    DataBaseImageValidatedEntryListPtr
          ImagesRetrieveValidatedClass (const KKStr&  sipperFileName);


    void  ImagesDeleteOneImage (const KKStr&  imageFileName);


    /// Update both the Validates and Class1Name
    void  ImagesUpdateValidatedAndPredictClass (const KKStr&  imageFileName,
                                                MLClassPtr    mlClass, 
                                                float         class1Prob
                                               );

    void  ImageUpdate (DataBaseImage&  dbImage,
                       RasterSipper&   image
                      );

    void  ImagesGetGpsData (const KKStr& imageRootName,
                            double&      latitude,
                            double&      longitude
                           );
    
    void  ImagesSizeDistributionByDepth (const KKStr&               cruiseName,
                                         const KKStr&               stationName,
                                         const KKStr&               deploymentNum,
                                         const KKStr&               className,
                                         float                      maxDepth,
                                         float                      depthBinSize,
                                         char                       statistic,
                                         double                     initialValue,
                                         double                     growthRate,
                                         double                     endValue,
                                         ImageSizeDistributionPtr&  downCast,
                                         ImageSizeDistributionPtr&  upCast
                                        );



    ClassStatisticListPtr  ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                    const KKStr&           sipperFileName,
                                                    MLClassPtr             mlClass,
                                                    char                   classKeyToUse,
                                                    float                  minProb,
                                                    float                  maxProb,
                                                    kkint32                minSize,
                                                    kkint32                maxSize,
                                                    float                  minDepth,
                                                    float                  maxDepth
                                                   );


    ClassStatisticListPtr  ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                    const KKStr&           cruiseName,
                                                    const KKStr&           stationName,
                                                    const KKStr&           deploymentNum,
                                                    MLClassPtr             mlClass,
                                                    char                   classKeyToUse,
                                                    float                  minProb,
                                                    float                  maxProb,
                                                    kkint32                minSize,
                                                    kkint32                maxSize,
                                                    float                  minDepth,
                                                    float                  maxDepth
                                                   );


    /// <summary> 
    ///   Returns pointer to vector-of-unsigned-int instance that will contain counts by depth; each element in vector will represent a depth range
    ///   as indicated by <paramref name="depthIncrements"/>.
    /// </summary> 
    VectorUint*  ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                          const KKStr&           sipperFileName,
                                          float                  depthIncrements,
                                          MLClassPtr             mlClass,
                                          char                   classKeyToUse,
                                          float                  minProb,
                                          float                  maxProb,
                                          kkint32                minSize,
                                          kkint32                maxSize
                                         );


    VectorUint*  ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                          const KKStr&           cruiseName,
                                          const KKStr&           stationName,
                                          const KKStr&           deploymentNum,
                                          float                  depthIncrements,
                                          MLClassPtr             mlClass,
                                          char                   classKeyToUse,
                                          float                  minProb,
                                          float                  maxProb,
                                          kkint32                minSize,
                                          kkint32                maxSize,
                                          bool&                  cancelFlag
                                         );


    
    //***********************************************************************************
    RasterSipperPtr  ImageFullSizeLoad (const KKStr&  imageFileName);

    RasterSipperPtr  ImageFullSizeLoad (kkint32  imageId);


    void       ImageFullSizeDelete (const KKStr&  imageFileName);


    void       ImageFullSizeSave (const KKStr&         imageFileName,
                                  const RasterSipper&  raster
                                 );

    /**
     * Will locate a original size version of the image; it will try in order 3 different sources:
     *   -# ThumbNail in Images Table.
     *   -# FullSizeImage in ImagesFullSize table
     *   -# will go to SipperFile
     *
     */
    RasterSipperPtr  ImageFullSizeFind (const KKStr&  imageFileName);



    //***********************************************************************************
    MLClassPtr  MLClassLoad (const KKStr&  className);

    MLClassListPtr  MLClassLoadList ();

    void  MLClassInsert (MLClass&  mlClass,
                         bool&     successful
                        );

    void  MLClassUpdate (const KKStr&    oldClassName,
                         const MLClass&  mlClass,
                         bool&           successful
                        );


    MLClassListPtr  MLClassLoadChildren (const KKStr&  className);


    /**
     *@brief  Inserts 'mlClass' into database if it is not already in it and then updates all the fields
     * in 'mlClass' with the data in the database.
     */
    void  MLClassInsertReturn (MLClass&  mlClass,
                               bool&     successful
                              );

    void  MLClassDelete (const KKStr&  mlClassName);




    //***********************************************************************************
    void  ImageGroupInsert (DataBaseImageGroup&  imageGroup);

    DataBaseImageGroupListPtr  ImageGroupLoad ();   // returns a list of all Group Assignments

    DataBaseImageGroupPtr      ImageGroupLoad (const KKStr&  name);  

    void  ImageGroupDelete (kkint32 imageGroupId);



    //***********************************************************************************
    void  ImageGroupEntriesInsert (const DataBaseImageGroupEntry&  imageGroupEntry);

    void  ImageGroupEntriesInsert (const DataBaseImageGroupEntryList&  imageGroupList);

    /**
     *@brief  
     *@details
     * Will insert a list of images into the same group in ImageGroupEntries. A list of ImageFileNames that failed to be inserted with a 
     * description of the error will be returned. If NULL is returned the insertion failed completely. A 2 x n matrix will be returned, 
     * One row for each image that failed insertion:
     *  - [0,0] = ImageFileName for row '0' that failed insertion
     *  - [0,1] = Description of error that caused insertion to fail.
     *
     * Since there is a limitation on the length of the MySQL parameter of 64K  we need to make sure that the
     * length of all the imageFileNames will be less than 64K that is sum_i (ImageFileNamess[i].Len ()) + imageFileNams.size () less 64000.
     */
    KKStrMatrixPtr  ImageGroupEntriesInsert (kkint32             groupId,
                                             const VectorKKStr&  imageFileNames
                                            );

    DataBaseImageGroupEntryListPtr  ImageGroupEntriesLoad (kkint32 groupId);





    //***********************************************************************************

    /**
     *@brief Retrieves the closest Instrument Data that precedes or equals 'ScanLine' but
     * not more that 8192 scan-lines before.
     */
    InstrumentDataPtr  InstrumentDataGetByScanLine (const KKStr&  sipperFileName,
                                                    kkuint32      scanLine
                                                   );


    void  InstrumentDataInsert (const KKStr&           _sipperFileName,
                                const InstrumentData&  id
                               );



    void  InstrumentDataUpdateCropSettings (const KKStr&  sipperFileName,
                                            kkuint32      scanLineStart,
                                            kkuint32      scanLineEnd,
                                            kkuint16      cropLeft,
                                            kkuint16      cropRight,
                                            kkuint16      activeColumns
                                           );




    void  InstrumentDataSaveListForOneSipperFile (const KKStr&               _sipperFileName,
                                                  const InstrumentDataList&  instrumentData,
                                                  const bool&                _cancelFlag
                                                 );



    /**
     *@brief  Will extract Instrument Data from "ALL" known Sipper files and update the database tables.
     *@details  For each known SIPPER file;  will load instrument data from Sipper file and save
     * into 'InstrumentData' table. useful in populating 'InstrumentData' table.  Make sure that
     * the Instrument assignments are correct in 'SipperFiles'.<br />
     *
     * Also make sure that all the Sipper Files are in one of the directory trees specified by
     * 'SipperDirectoryList.txt' in "%PicesHomeDir%\DataFiles"
     */
    void  InstrumentDataLoadAllSipperFiles (const bool&  cancelFlag);


    /**
     *@brief Returns a list of all the field names in the Instrument Data table.
     */
    VectorKKStr  InstrumentDataGetFieldNames ();


    InstrumentDataListPtr  InstrumentDataLoad (const KKStr&  sipperFileName,
                                               const bool&   cancelFlag
                                              );


    InstrumentDataListPtr  InstrumentDataLoad (const KKStr&  cruiseName,
                                               const KKStr&  stationName,
                                               const KKStr&  deploymentNum,
                                               const bool&   cancelFlag
                                              );


    /**
     *@brief  Re-Extracts Instrument data from SIPPER file and updates DataBase tables to make sure they are correct.
     *@details
     * This method re-extracts instrument data for the given SipperFile and updates the
     * appropriate tables.  This way all tables are in agreement with each other.
     * For the Specified SIPPER File "siperFileName".
     *@code
     *  - 1) Extract Instrument Data from original SipperFile.
     *  - 2) Replaces all entries in IntrumentData table for 'sipperFile' with what was just extracted. 
     *  - 3) Updates the InstrumentData fields in Images with data just extracted.
     *@endcode
     *@param[in]  sipperFileName  Name of Sipper files that instrument data is to be extracted from; the root name will be used.
     *@param[in]  cancelFlag      The method will monitor this flag; if it goes true it will exit gracefully and return to caller.
     *@param[in]  msgBuff         Pointer to message buffer that the method will write NULL terminated strings to this field indicating progress;
     *@param[in]  msgBuffLen      Length in characters of 'msgBuff'.
     */
    void  InstrumentDataReFreshSipperFile (const KKStr&  sipperFileName,
                                           const bool&   cancelFlag,
                                           char*         msgBuff,
                                           kkint32       msgBuffLen
                                          );

    /**
     *@brief Returns volume of water sampled, in cubic meters, by depth for a given sipper file.
     */
    VolumeSampledStatListPtr  InstrumentDataGetVolumePerMeterDepth (const KKStr&  siperFileName,
                                                                    float         depthBinSize
                                                                   );

    void  InstrumentDataDeploymentVolumeSampled (const KKStr&               cruiseName,
                                                 const KKStr&               stationName,
                                                 const KKStr&               deploymentNum,
                                                 float                      depthBinSize,
                                                 VolumeSampledStatListPtr&  downCast,
                                                 VolumeSampledStatListPtr&  upCast
                                                );



    /**
     *@brief Returns volume of water sampled, in cubic meters, by depth for a complete deployment.
     */
    VolumeSampledStatListPtr  InstrumentDataGetVolumePerMeterDepth (const KKStr&  cruiseName,
                                                                    const KKStr&  stationName,
                                                                    const KKStr&  deploymentNum,
                                                                    float         depthBinSize,
                                                                    const bool&   cancelFlag
                                                                   );


    /**
     *@brief  Update all InstrumentData entries that fall within the dateTime range with the specified Latitude and Longitude.
     */
    void  InstrumentDataUpdateLatitudeAndLongitude (const KKB::DateTime&  dateTimeStart,
                                                    const KKB::DateTime&  dateTimeEnd,
                                                    double                latitude,
                                                    double                longitude
                                                   );


    /**
     *@brief Returns 2d array consisting of the specific data fields from the Instrument data file 
     * for the specified SipperFile.
     *@returns 2D float array where each row will represent a Instrument row and each column will 
     *  represent a field specified in 'fieldNames'.
     */
    std::vector<std::vector<float> >*  InstrumentDataGetSpecificFields (const KKStr&          sipperFileName,
                                                                        const KKStrListPtr    fieldNames,
                                                                        const KKB::DateTime&  dateTimeStart,
                                                                    const KKB::DateTime&  dateTimeEnd
                                                             );


     std::vector<std::vector<float> >*  InstrumentDataGetSpecificFields (const KKStr&          cruiseName,
                                                                         const KKStr&          stationName,
                                                                         const KKStr&          deploymentNum,
                                                                         const KKStrListPtr    fieldNames,
                                                                         const KKB::DateTime&  dateTimeStart,
                                                                         const KKB::DateTime&  dateTimeEnd
                                                                        );


    GPSDataPointListPtr InstrumentDataRetrieveGPSInfo (const KKStr&  cruiseName,
                                                       const KKStr&  stationName,
                                                       const KKStr&  deploymentNum,
                                                       kkint32       timeInterval   /**< Interval in seconds */
                                                      );



    InstrumentDataMeansListPtr  InstrumentDataBinByMeterDepth (const KKStr&  cruiseName,
                                                               const KKStr&  stationName,
                                                               const KKStr&  deploymentNum,
                                                               float         depthBinSize,
                                                               const bool&   cancelFlag
                                                              );


    //***********************************************************************************
    DataBaseLogEntryPtr  LogEntriesProcessStart (const KKStr&          progCode,
                                                 const KKStr&          progName,
                                                 const KKB::DateTime&  dateTimeCompiled,
                                                 const KKStr&          cmdLine,
                                                 const KKStr&          sipperFileName   /**< Root name of Sipper file; only if specific to a single Sipper File */
                                                );
  

    void  LogEntriesProcessEnd (DataBaseLogEntryPtr   logEntry,
                                float                 cpuTimeUsed,
                                const KKStr&          completionResult
                               );


    DataBaseLogEntryPtr  LogEntriesSelect (kkuint32 _logEntryId);






    //***********************************************************************************
    void  InsertCruise (const KKStr&     shipName, 
                        const KKStr&     description,
                        const DateType&  dateStart,
                        const DateType&  dateEnd,
                        const KKStr&     location,
                        const KKStr&     objective,
                        const KKStr&     principal,
                        const KKStr&     researchOrg
                       );


    void                  SipperCruiseDelete (const KKStr&  cruiseName);

    void                  SipperCruiseInsert (SipperCruisePtr  cruise,
                                              bool&            successful
                                             );

    SipperCruisePtr       SipperCruiseLoad (const KKStr&  cruiseName);

    SipperCruiseListPtr   SipperCruiseLoadAllCruises ();

    void                  SipperCruiseUpdate (SipperCruisePtr  cruise,
                                              bool&            successful
                                             );


    //***********************************************************************************
    SipperDeploymentListPtr  SipperDeploymentLoad (const KKStr& cruiseName,
                                                   const KKStr& stationName
                                                  );
    
    SipperDeploymentPtr      SipperDeploymentLoad (const KKStr&  cruiseName,
                                                   const KKStr&  stationName,
                                                   const KKStr&  deploymentNum
                                                  );
    
    SipperDeploymentListPtr  DeploymentLoadByStation (const KKStr& stationName);

    void                     SipperDeploymentInsert (const SipperDeployment&  deployment);

    void                     SipperDeploymentUpdate (const SipperDeployment&  deployment);

    void                     SipperDeploymentDelete (const KKStr&  cruiseName,
                                                     const KKStr&  stationName,
                                                     const KKStr&  deploymentNum
                                                    );



    //***********************************************************************************
    VectorKKStr*          SipperFileGetList (const KKStr& cruiseName,
                                             const KKStr& stationName,
                                             const KKStr& deploymentNum
                                            );

    SipperFileListPtr     SipperFileLoad      (const KKStr& cruiseName,
                                               const KKStr& stationName,
                                               const KKStr& deploymentNum
                                              );

    void                  SipperFileLoadOracle ();

    SipperFilePtr         SipperFileRecLoad   (const KKStr& _sipperFileName);


    void                  SipperFileInsert (SipperFile&   sipperFile);

    void                  SipperFileUpdate (SipperFile&   sipperFile);

    void                  SipperFilesUpdateExtractionStatus (const KKStr&  sipperFileName,
                                                             char          extractionStatus
                                                            );

    /**
     *@brief  Updates the ExtractionStatus only if the existing extractioStatus is what was expected.
     *@param[in]  sipperFileName
     *@param[in]  extractionStatusExpected  Status of existing SipperFiles table entry;  if not existing extractionStatus
     *            this value then no update will take place.
     *@param[in]  extractionStatusNew  The new ExtractionStatus the specified SipperFile will take on if current 
     *            value equal extractionStatusExpected.
     *@param[out] ExtractionStatusBeforeUpdate  Status found in table entry before update.
     *@param[out] extractionStatusResult  The new value of ExtractedStatus.
     *@param[out] extrtactionSatusUpdated  Indicates if the extraction status was updated.
     */
     void  SipperFilesUpdateExtractionStatusIfExpected (const KKStr&  sipperFileName,
                                                        char          extractionStatusExpected,
                                                        char          extractionStatusNew,
                                                        char&         ExtractionStatusBeforeUpdate,
                                                        char&         extractionStatusResult,
                                                        bool&         extrtactionSatusUpdated
                                                       );

    void                  SipperFileUpdateFileSizeStats (const KKStr&  _sipperFileName, 
                                                         kkint64       _sizeInBytes,
                                                         kkuint32      _numScanLines
                                                        );

    void                  SipperFilesDelete (const KKStr&  _sipperFileName);  /**< Delete a Sipper File and all its related data from all tables in database. */


    void                  SipperFilesGetCTDDateTime (const KKStr&    _sipperFileName,
                                                     kkint32&          _sipperFileId, 
                                                     kkuint32&         _numScanLines, 
                                                     KKB::DateTime&  _CTDDateTimeStart, 
                                                     KKB::DateTime&  _CTDDateTimeEnd
                                                    );



    //***********************************************************************************
    SipperStationListPtr  SipperStationLoad (const KKStr&  cruiseName);
    
    SipperStationPtr      SipperStationLoad (const KKStr&  cruiseName,
                                             const KKStr&  stationName
                                            );
    
     SipperStationListPtr  SipperStationsLoadByGpsRange (double  latitudeMin,
                                                         double  latitudeMax,
                                                         double  longitudeMin,
                                                         double  longitudeMax
                                                        );
    
    void                  SipperStationInsert (const SipperStation&  station);

    void                  SipperStationUpdate (const SipperStation&  station);

    void                  SipperStationDelete (const KKStr&  cruiseName,
                                               const KKStr&  stationName
                                              );
    void   ResultSetsClear ();

  private:
    static  GoalKeeperPtr  blocker;   /*!< used to handle multi-threading issues. */

    static volatile kkint32  numDataBaseInstances;  /**< Will be incremented when the constructor is called and
                                                   * decremented when the destructor is called.  This way the
                                                   * Constructor and Destructor's will know if they should call
                                                   * "mysql_library_init" and "mysql_library_end" respectively.
                                                   */

    static KKStr           mySqlDataDir;

    static volatile bool   mySqlEmbeddedServerInitailzied;  /**< true indicates that embedded server is available. */

    static KKStr           mySqlErroLogFileName;

    static KKStr           mySqlHomeDir;

    static volatile bool   staticVariablesInialized;

    typedef  std::map<DataBasePtr,DataBasePtr>   DataBaseIndex;
    typedef  std::pair<DataBasePtr,DataBasePtr>  DataBaseIndexPair;
    static DataBaseIndex   existingInstances;


    /** Call this method when a update is called that is not allowed when 'allowUpdates == false'. */
    static  void   UpdatesNotAllowed (const KKStr&  methodName);


    static  void   CreateBlocker ();  /*!< Call this to make sure that Blocker exists.  Will handle race conditions. */
    static  void   InitializeDataBaseStaticVariables ();
    static  void   InitializeMySqlLibrary ();
    static  void   InitializeMySqlLibraryEmbedded ();
    static  void   InitializeMySqlLibraryServer ();
    static  void   DataBaseAddInstance (DataBasePtr  _instance);
    static  void   DataBaseDeleteInstance (DataBasePtr  _instance);



    DataBaseServerPtr  GetDefaultMySqlParameters ();


    kkint32  Connect ();

    kkint32  ConnectEmbedded ();

    kkint32  ConnectServer ();

    kkint32  ReConnect (const KKStr&  funcCall,
                      const KKStr&  queryStrMsg,
                      kkint32       numReTries
                     );

    void  ValidateConnection (const char*  funcCall);



    // Routines to format MySql datastat types.
    KKB::DateTime  DateTimeFromMySqlDateTimeField (KKStr  field);

    KKB::TimeType  TimeFromMySqlDateTimeField (KKStr field);

    KKB::DateType  DateFromMySqlDateTimeField (KKStr  field);

    KKStr   DateTimeToQuotedStr (const KKB::DateTime&  dt);

    KKStr    FloatToStr (float f);

    SipperDeploymentListPtr  SipperDeploymentProcessResults ();

    GPSDataPointListPtr   GpsDataProcessResults ();


    static char**         featureDataFieldNames;
    static ConstCharStarArray     GetFeatureDataFieldNames ();

    ImageFeaturesListPtr  FeatureDataProcessResults ();  // Used by the FeatureData retrieval routines.


    uchar*  EncodeARasterImageIntoAThumbNail (const RasterSipper&  image,
                                              kkuint32             maxDimSizeAllowed,
                                              kkuint32&              buffLen
                                             );

    MLClassListPtr  MLClassProcessResults ();

    DataBaseImageListPtr   ImageQueryProcessResults ();

    ClassStatisticListPtr  ImageProcessClassStaticsResults ();

    InstrumentDataListPtr  InstrumentDataProcessResults (const bool&  cancelFlag);

    SipperCruiseListPtr  SipperCruiseProcessResults ();

    SipperFileListPtr  SipperFileProcessResults ();
    static char const *  sipperFileFieldNames[];

    SipperStationListPtr  SipperStationProcessResults ();


    /** Copies String from src to dest making sure that it does not exceed 'destSize'. */
    void  WriteBuff (char*        dest,
                     KKB::kkint32 destSize,
                     const char*  src
                    );


    RunLog&  log;

    bool               allowUpdates;        /**< Indicates that no updates are allowed; controlled by 
                                             * 'Permissions.cfg' file in ${PicesHomeDir}\Configurations        */

    bool               useEmbeddedServer;   /**< When set to 'true' we are to operate as an embedded server.    */
    MYSQL*             conn;
    DataBaseServerPtr  server;              /**< We will own this instance and be responsible for deleting.    */
    bool               duplicateKey;        /**< Is set to true during insert if a duplicate key error occurs. */
    bool               valid;               /**< Will be set to false if connection is bad.                    */
    kkuint32           lastMySqlErrorNo;
    KKStr              lastMySqlErrorDesc;
    KKStr              prevQueryStatement;

    FileDescPtr        featureFileDesc;


    MYSQL_FIELD*       resultSetFieldDefs;
    MYSQL_RES*         resultSet;
    kkuint32           resultSetNumFields;
    kkuint32           resultSetNumRows;
    bool               resultSetMore;   
    MYSQL_ROW          resultSetNextRow;
    VectorInt          resultSetFieldIndex;
    ulong*             resultSetLengths;


    void   ResultSetBuildFieldIndexTable (ConstCharStarArray    fieldNames);
    bool   ResultSetLoad    (ConstCharStarArray   fieldNames);
    bool   ResultSetLoadFieldNames (VectorKKStr&  fieldNames);
    bool   ResultSetGetNext (const char**  fieldNames);
    bool   ResultSetFetchNextRow ();
    void   ResulSetFree ();

    
    bool             ResultSetGetBool          (kkuint32 fieldIdx);
    char             ResultSetGetCharField     (kkuint32 fieldIdx);
    const char*      ResultSetGetField         (kkuint32 fieldIdx);
    ulong            ResultSetGetFieldLen      (kkuint32 fieldIdx);
    KKStr            ResultSetGetKKStrField    (kkuint32 fieldIdx);
    KKB::DateTime    ResultSetGetDateTimeField (kkuint32 fieldIdx);
    KKB::TimeType    ResultSetGetTimeField     (kkuint32 fieldIdx);
    double           ResultSetGetDoubleField   (kkuint32 fieldIdx);
    float            ResultSetGetFloatField    (kkuint32 fieldIdx);
    kkint32          ResultSetGetIntField      (kkuint32 fieldIdx);
    KKB::kkint64     ResultSetGetInt64Field    (kkuint32 fieldIdx);
    RasterSipperPtr  ResultSetGetRasterField   (kkuint32 fieldIdx);
    kkuint16         ResultSetGetUint16Field   (kkuint32 fieldIdx);
    kkuint32         ResultSetGetUintField     (kkuint32 fieldIdx);
    KKB::ulong       ResultSetGetUlongField    (kkuint32 fieldIdx);
    KKB::kkuint64    ResultSetGetUint64Field   (kkuint32 fieldIdx);
  };  /* DataBase */

  typedef  DataBase::DataBasePtr  DataBasePtr;

#define  _DataBase_Defined_

}  /* namespace MLL */





#endif
