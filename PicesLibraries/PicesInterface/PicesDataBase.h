#ifndef  _PICESDATABASE_
#define  _PICESDATABASE_

#include "DataBase.h"
#include "DateTime.h"
#include "PicesClassList.h"
#include "PicesClassStatistic.h"
#include "PicesDataBaseServer.h"
#include "PicesDataBaseImage.h"
#include "PicesDataBaseImageGroup.h"
#include "PicesDataBaseImageGroupEntry.h"
#include "PicesDataBaseLogEntry.h"
#include "PicesFeatureVector.h"
#include "PicesFeatureVectorList.h"
#include "PicesGPSDataPoint.h"
#include "PicesImageSizeDistribution.h"
#include "PicesInstrumentData.h"
#include "PicesPoint.h"
#include "PicesSipperFile.h"
#include "PicesSipperCruise.h"
#include "PicesSipperDeployment.h"
#include "PicesSipperStation.h"
#include "PicesRunLog.h"
#include "PicesVolumeSampledStat.h"



namespace PicesInterface 
{
  /*!
   \class PicesDataBase
   All access to the MySQL database is done through this class.  Each one of these methods calls
   the matching method in the unmanaged class DataBase in the PicesLibrary.
   */

  public ref class PicesDataBase
  {
  public:
    static  void  Initialization ();
    static  void  FinalCleanUp ();

    static  bool  EmbeddedServerRunning ();

    static  PicesDataBase^  globaDbConn = nullptr;

    static  PicesDataBaseServer^  defaultGlobalServer = nullptr;

    static  PicesDataBase^  GetGlobalDatabaseManagerNewInstance (PicesRunLog^  _runLog);  // Will instatiate a new instance of the current GlobalDatabase Connecetion.

    static  PicesDataBase^  SelectNewDataBaseServer (PicesRunLog^  _runLog);

    static  void            CloseGlobalDbConn ();


    PicesDataBase (PicesRunLog^  _runLog);

    PicesDataBase (PicesDataBaseServer^  _server,
                   PicesRunLog^          _log
                  );

    void  Close ();

  protected:
    !PicesDataBase ();

  private:
    ~PicesDataBase ();

  public:

    ///<summary>
    /// A general purpose SQL calling method;  It will execute the command in 'statement' with the 
    /// understanding that 'colsToReturn' will contain a list of columns in the result set that are to 
    /// be returned.
    ///</summary>
    ///<param name="statement"> SQL Statement that is to be executed.</param>
    ///<param name="colsToReturn"> A list of the columns from the results set that are to be returned
    /// The order that is specified will control the order in the returned results that they will appear.</param>
    ///<returns>If successful will return a two dimensional array of strings where each row represents a 
    ///single row from the results set.  The columns will correspond to the names in 'colsToReturn'.
    ///</returns>
    array<array<String^>^ >^  QueryStatement (String^          statement,
                                              array<String^>^  colsToReturn
                                             );

    property  bool                  CancelFlag  {bool  get ();  void  set (bool  _cancelFlag);}

    property  bool                  LastOpSuccessful  {bool get ()  {return  lastOpSuccessful;}}

    property  PicesDataBaseServer^  Server      {PicesDataBaseServer^  get ();}   //  Setting this property to 'True' causes what ever db operartion to cancel.


    bool  DuplicateKey ()  {return  dbConn->DuplicateKey ();}

    String^  ServerDescription ();

    String^  LastErrorDesc ();

    bool  Successful ()  {return  lastOpSuccessful;}

    static bool  ThreadInit ();  /**< Call at start of new thread.         */
    static void  ThreadEnd ();   /**< Call just before thread terminates.  */

    bool  Valid ();




    //*******************************************************************************************
    
    ///<summary>
    /// 
    ///</summary>
    ///<param  name ="cruiseName"> Cruse to retrieve GPS data for.</param>
    ///<param  name ="utcStart">Start Date-Time to collect data for; nullptr indicates to start with 1st entry for cruise..</param>
    ///<param  name ="utcEnd">End Date-Time to collect data for; nullptr indicates to start with 1st entry for cruise..</param>
    PicesGPSDataPointList^  GpsDataQuery (String^           cruiseName,
                                          System::DateTime  utcStart,
                                          System::DateTime  utcEnd
                                         );




    ///<summary>
    ///Returns Summary GPSData for a specified Cruies and time range.
    ///Each entry returned will be the avareage values for the range of entries that are with in 'timeInterval' seconds.
    /// This is done by grouping by using the function      "Floor(Unix_Time(GpsUtcTime) / timeInterval)"
    ///</summary> 
    PicesGPSDataPointList^   GpsDataQueryByIntervals (String^           cruiseName,
                                                      System::DateTime  utcStart,
                                                      System::DateTime  utcEndk,
                                                      int               timeInterval
                                                     );



    //*******************************************************************************************
    void  FeatureDataInsertRow (String^              _sipperFileName,
                                PicesFeatureVector^  _example
                               );
  

    PicesFeatureVector^  FeatureDataRecLoad (String^  imageFileName);

    PicesFeatureVector^  FeatureDataRecLoad (PicesDataBaseImage^  image);


    PicesFeatureVectorList^  FeatureDataGetOneSipperFile (String^       sipperFileName,
                                                          PicesClass^   mlClass,
                                                          System::Char  classKeyToUse,
                                                          bool          reExtractInstrumentData
                                                         );

    
    PicesFeatureVectorList^  FeatureDataForImageGroup (PicesDataBaseImageGroup^  imageGroup,
                                                       PicesClass^               mlClass,
                                                       System::Char              classKeyToUse
                                                      );



    ///<summary>
    /// Updates an existing Feature-Data-Vector in the database.
    ///</summary>
    ///<param name="databaseImage">Specifies that image in the database that the Feature-Vector was computed for;  if points 
    /// to nullptr will attempt to locate via 'ImageFileName' in the example' parameter.</param>
    ///<param name="example">The Feature-Vector to update the existing Feature-Vector in the database with.</param>
    void   FeatureDataUpdate (PicesDataBaseImage^  databaseImage,
                              PicesFeatureVector^  example
                             );



    //*******************************************************************************************
    void  ImageInsert (PicesRaster^    image,
                       String^         imageFileName,
                       String^         supperFileName,
                       kkuint64        byteOffset,     /**< byteOffset of SipperRow containing TopLeftRow */
                       kkuint32        topLeftRow,
                       kkuint32        topLeftCol,
                       kkuint32        height,
                       kkuint32        width,
                       kkuint32        pixelCount,
                       uchar           connectedPixelDist,
                       kkuint32        extractionLogEntryId,
                       kkuint32        classLogEntryId,
                       kkuint32        centroidRow,
                       kkuint32        centroidCol,
                       PicesClass^     class1,
                       float           class1Prob,
                       PicesClass^     class2,
                       float           class2Prob,
                       PicesClass^     validatedClass,
                       float           depth,
                       float           imageSize,
                       PicesPointList^ sizeCoordinates,
                       kkint32%          imageId,
                       bool%           successful
                      );


    PicesDataBaseImage^      ImageLoad (uint  imageId);


    PicesDataBaseImage^      ImageLoad (String^   imageFileName);  // Root name of image.


    PicesDataBaseImageList^  ImagesQuery (PicesDataBaseImageGroup^  group,
                                          bool                      includeThumbnail
                                         );

    PicesDataBaseImageList^  ImagesQuery (PicesDataBaseImageGroup^  imageGroup,
                                          String^                   sipperFileName,
                                          PicesClass^               mlClass,
                                          System::Char              classKeyToUse,
                                          float                     probMin,
                                          float                     probMax,
                                          int                       sizeMin,
                                          int                       sizeMax,
                                          float                     depthMin,
                                          float                     depthMax,
                                          uint                      restartImageId,
                                          int                       limit,             // Max # of rows to return.  -1 indicates no limit.
                                          bool                      includeThumbnail
                                         );


    PicesDataBaseImageList^  ImagesQuery (PicesDataBaseImageGroup^  imageGroup,
                                          String^                   cruiseName,
                                          String^                   stationName,
                                          String^                   deploymentNum,
                                          PicesClass^               mlClass,
                                          System::Char              classKeyToUse,
                                          float                     probMin,
                                          float                     probMax,
                                          int                       sizeMin,
                                          int                       sizeMax,
                                          float                     depthMin,
                                          float                     depthMax,
                                          uint                      restartImageId,
                                          int                       limit,             // Max # of rows to return.  -1 indicates no limit.
                                          bool                      includeThumbnail
                                         );



    PicesDataBaseImageList^  ImagesQueryByGrouop 
                                  (PicesDataBaseImageGroup^   imageGroup,
                                   String^                    cruiseName,
                                   String^                    stationName,
                                   String^                    deploymentNum,
                                   String^                    sipperFileName,
                                   PicesClass^                mlClass,
                                   System::Char               classKeyToUse,
                                   float                      probMin,
                                   float                      probMax,
                                   int                        sizeMin,
                                   int                        sizeMax,
                                   float                      depthMin,
                                   float                      depthMax,
                                   uint                       restartImageId,
                                   int                        limit,            // Max # of rows to return.  -1 indicates no limit.,
                                   bool                       includeThumbnail
                                  );



    PicesDataBaseImageList^  ImagesQueryDeploymentSizeRange (String^       cruiseName,
                                                             String^       stationName,
                                                             String^       deploymentNum,
                                                             PicesClass^   mlClass,
                                                             System::Char  cast,         /**< 'U' = UpCast, 'D' = DownCast,  'B' = Both' */
                                                             System::Char  statistic,    /**< '0' = Area mm^2,  '1' = Diameter,  '2' = Spheroid Volume and '3' = EBv ((4/3)(Pie)(Major/2)(Minor/2)^2) */
                                                             float         sizeStart,
                                                             float         sizeEnd,
                                                             float         depthMin,
                                                             float         depthMax,
                                                             int           sampleQty,
                                                             bool          includeChildren
                                                            );



    PicesClassStatisticList^  ImageGetClassStatistics (PicesDataBaseImageGroup^  imageGroup,
                                                       String^                   sipperFileName,
                                                       PicesClass^               mlClass,
                                                       System::Char              classKeyToUse,
                                                       float                     minProb,
                                                       float                     maxProb,
                                                       int                       minSize,
                                                       int                       maxSize,
                                                       float                     minDepth,
                                                       float                     maxDepth
                                                      );


    PicesClassStatisticList^  ImageGetClassStatistics (PicesDataBaseImageGroup^  imageGroup,
                                                       String^                   cruiseName,
                                                       String^                   stationName,
                                                       String^                   deploymentNum,
                                                       PicesClass^               mlClass,
                                                       System::Char              classKeyToUse,
                                                       float                     minProb,
                                                       float                     maxProb,
                                                       int                       minSize,
                                                       int                       maxSize,
                                                       float                     minDepth,
                                                       float                     maxDepth
                                                      );


    array<uint>^  ImageGetDepthStatistics (PicesDataBaseImageGroup^  imageGroup,
                                           String^                   sipperFileName,
                                           float                     depthIncrements,
                                           PicesClass^               mlClass,
                                           System::Char              classKeyToUse,
                                           float                     minProb,
                                           float                     maxProb,
                                           int                       minSize,
                                           int                       maxSize
                                          );


    array<uint>^  ImageGetDepthStatistics (PicesDataBaseImageGroup^  imageGroup,
                                           String^                   cruiseName,
                                           String^                   stationName,
                                           String^                   deploymentNum,
                                           float                     depthIncrements,
                                           PicesClass^               mlClass,
                                           System::Char              classKeyToUse,
                                           float                     probMin,
                                           float                     probMax,
                                           int                       sizeMin,
                                           int                       sizeMax
                                          );



    void  ImageUpdateInstrumentDataFields (PicesInstrumentData^  instumentData,
                                           String^               sipperFileName,
                                           uint                  scanLineStart,
                                           uint                  scanLineEnd
                                          );


    void  ImagesUpdatePredictions (String^      imageFileName,
                                   PicesClass^  class1Pred,
                                   float        class1Prob,
                                   PicesClass^  class2Pred,
                                   float        class2Prob,
                                   uint         logEntryId
                                  );


    /**
     *@brief  Updates prediction on multiple images at one time.
     *@param[in]  _logEntryId Log id of process that is doing the classification.
     *@param[in]  _predictionList  A tab delimited string that will contain the list of predictions.  Each
     *   image will be separated by line feed ('\n') characters while each field for each image will be
     *   separated by 'tabs'('\t').  So if there are two images with predictions the string will look like
     *   "<ImageFileRootName><\t><Class1Name><\t><Class1Prob><\t><Class2Name><\t><Class2Prob><\n><ImageFileRootName><\t><Class1Name><\t><Class1Prob><\t><Class2Name><\t><Class2Prob>"
     */
    void  ImagesUpdatePredictionsList (uint     _logEntryId,
                                       String^  _predictionList
                                      );


    void  ImageRemoveValidation (String^   imageFileName);


    void  ImagesDeleteOneImage (String^   imageFileName);


    void  ImagesUpdateValidatedClass (String^      imageFileName, 
                                      PicesClass^  mlClass
                                     );

    void  ImagesUpdateValidatedAndPredictClass (String^     imageFileName, 
                                                PicesClass^ mlClass, 
                                                float       class1Prob
                                               );

    void  ImagesUpdateImageSize (String^          imageFileName,
                                 PicesPointList^  sizeCoordinates, 
                                 float            imageSize
                                );

    void  ImagesSizeDistributionByDepth (String^                       cruiseName,
                                         String^                       stationName,
                                         String^                       deploymentNum,
                                         String^                       className,
                                         float                         depthBinSize,
                                         char                          statistic,
                                         float                         initialValue,
                                         float                         growthRate,
                                         float                         endValue,
                                         PicesImageSizeDistribution^%  downCast,
                                         PicesImageSizeDistribution^%  upCast
                                        );


    //***********************************************************************************
    PicesRaster^  ImageFullSizeLoad (String^  imageFileName);


    void       ImageFullSizeSave (String^       imageFileName,
                                  PicesRaster^  raster
                                 );

    /// Will locate a original size version of the image; it will try in order 3 diferent sources.
    /// 1) ThumbNail in Images Table.
    /// 2) FullSizeImage in ImagesFullSize table
    /// 3) will go to SipperFile
    PicesRaster^  ImageFullSizeFind (String^  imageFileName);




    //*******************************************************************************************
    PicesClass^            MLClassLoad (String^  className);  /**< Load instance on PicesClass with name = "ClassName"  
                                                               * if not in database will return nullptr
                                                               */

    PicesClassList^        MLClassLoadList ();  // Will load all classes from "Pices.Classes"  table.

    void                   MLClassInsert (PicesClass^  picesClass);

    void                   MLClassInsertReturn (PicesClass^  mlClass,
                                                bool%        successful
                                               );

    void                   MLClassUpdate (String^      oldClassName,   // if == nullptr  assumed that name has not changed.
                                          PicesClass^  picesClass
                                         );

    void                   MLClassDelete (String^  className);



    //***********************************************************************************
    void  ImageGroupInsert (PicesDataBaseImageGroup^  imageGroup);

    PicesDataBaseImageGroupList^  ImageGroupLoad ();   // returns a list of all Group Assignments

    PicesDataBaseImageGroup^  ImageGroupLoad (String^  name);
    
    void  ImageGroupDelete (int imageGroupId);





    //***********************************************************************************
    void  ImageGroupEntriesInsert (PicesDataBaseImageGroupEntry^  imageGroupEntry);

    void  ImageGroupEntriesInsert (PicesDataBaseImageGroupEntryList^  imageGroupList);


    /**
     *@brief Adds multiple images to a specified GroupId; the update is done in one MySql call to improve performance.
     *@returns  Two dimensional string matrix with one row for each update failure, 1st column "ImageFileName", 2nd column Eerror description.
     */
    array<array<String^>^ >^  ImageGroupEntriesInsert (int             groupId,
                                                       List<String^>^  imageFileNames
                                                      );

    /**
     *@brief Adds multiple images to a specified GroupId; the update is done in one MySql call to improve performance.
     *@returns  Two dimensional string matrix with one row for each update failure, 1st column "ImageFileName", 2nd column Eerror description.
     */
    array<array<String^>^ >^  ImageGroupEntriesInsert (int                      groupId, 
                                                       PicesDataBaseImageList^  images
                                                      );

    PicesDataBaseImageGroupEntryList^  ImageGroupEntriesLoad (int groupId);





    //*******************************************************************************************
    List<String^>^  InstrumentDataGetFieldNames ();  // Get the names of individule field names;  used by 
                                                     // applications that will query InstrumentData table 
                                                     // allowing users to select fields to query and display.


    PicesInstrumentData^  InstrumentDataGetByScanLine (String^   sipperFileName,   // Will retrieve the Instrument data that is 
                                                       uint      scanLine          // closest to 'ScanLine'
                                                      );


    PicesInstrumentDataList^  InstrumentDataLoad (String^  sipperFileName);


    PicesInstrumentDataList^  InstrumentDataLoad (String^  cruiseName,
                                                  String^  stationName,
                                                  String^  deploymentNum
                                                 );


    void  InstrumentDataSaveListForOneSipperFile (String^                   sipperFileName, 
                                                  PicesInstrumentDataList^  data
                                                 );

    /**
     *@brief Returns volume of water sampled, in cubic meters, by depth for a given sipper file.
     */
    PicesVolumeSampledStatList^  InstrumentDataGetVolumePerMeterDepth (String^  siperFileName,
                                                                       float    depthBinSize
                                                                      );


    PicesVolumeSampledStatList^  InstrumentDataGetVolumePerMeterDepth (String^  cruiseName,
                                                                       String^  stationName,
                                                                       String^  deploymentNum,
                                                                       float    depthBinSize
                                                                      );

    List<ulong>^  GetScanLinesPerMeterProfile (String^           sipperFileName,
                                               PicesSipperFile^  sipperFile
                                              );


    List<float>^  GetVolumePerMeterProfile (String^           sipperFileName,
                                            PicesSipperFile^  sipperFile
                                           );




    /**
     *@brief Returns 2d array consisting of the specific data fields from the Instrument data file 
     * for the specified SipperFile.
     *@returns 2D float array where each row will represent a Instrument row and each column will 
     *  represent a field specified in 'fieldNames'.
     */
    array<float, 2>^  InstrumentDataGetSpecificFields (String^           sipperFileName,
                                                       List<String^>^    fieldNames,
                                                       System::DateTime  dateTimeStart,
                                                       System::DateTime  dateTimeEnd
                                                      );


    array<float, 2>^  InstrumentDataGetSpecificFields (String^           cruiseName,
                                                       String^           stationName,
                                                       String^           deploymentNum,
                                                       List<String^>^    fieldNames,
                                                       System::DateTime  dateTimeStart,
                                                       System::DateTime  dateTimeEnd
                                                      );


    PicesGPSDataPointList^  InstrumentDataRetrieveGPSInfo (String^  cruiseName,
                                                           String^  stationName,
                                                           String^  deploymentNum,
                                                           kkint32  timeInterval   /**< Interval in seconds */
                                                          );


    //***********************************************************************************
    PicesDataBaseLogEntry^  LogEntriesProcessStart (String^           progCode,
                                                    String^           progName,
                                                    System::DateTime  dateTimeCompiled,
                                                    String^           cmdLine,
                                                    String^           sipperFileName
                                                   );
  

    void  LogEntriesProcessEnd (PicesDataBaseLogEntry^  logEntry,
                                float                   cpuTimeUsed,
                                String^                 completionResult
                               );


    PicesDataBaseLogEntry^  LogEntriesSelect (uint _logEntryId);

    //*******************************************************************************************
    PicesSipperDeploymentList^  SipperDeploymentLoad (String^  cruiseName,
                                                      String^  stationName
                                                     );


    PicesSipperDeployment^      SipperDeploymentLoad (String^  cruiseName,
                                                      String^  stationName,
                                                      String^  deploymentNum
                                                     );
  

    PicesSipperDeploymentList^  SipperDeploymentLoadByStation (String^ stationName);
    

    void                        SipperDeploymentInsert (PicesSipperDeployment^  deployment);
  

    void                        SipperDeploymentUpdate (PicesSipperDeployment^  deployment);
 

    void                        SipperDeploymentDelete (String^  cruiseName,
                                                        String^  stationName,
                                                        String^  deploymentNum
                                                       );





    //`*******************************************************************************************
    array<String^>^          SipperFileGetList (String^  cruiseName,
                                                String^  stationName,
                                                String^  deploymentNum
                                               );

    PicesSipperFileList^     SipperFileLoad (String^  cruiseName,
                                             String^  stationName,
                                             String^  deploymentNum
                                            );

    void                     SipperFileLoadOracle ();


    PicesSipperFile^         SipperFileRecLoad  (String^ _sipperFileName);

    void                     SipperFileInsert (PicesSipperFile^  sipperFile);

    void                     SipperFileUpdate (PicesSipperFile^  sipperFile);

    String^                  SipperFileGetFullPath (String^ _sipperFileName);



    //*******************************************************************************************
    void                     SipperCruiseDelete (String^  cruiseName);

    void                     SipperCruiseInsert (PicesSipperCruise^  cruise);

    PicesSipperCruise^       SipperCruiseLoad (String^  cruiseName);

    PicesSipperCruiseList^   SipperCruiseLoadAllCruises ();

    void                     SipperCruiseUpdate (PicesSipperCruise^  cruise);


    //*******************************************************************************************
    PicesSipperStationList^  SipperStationLoad (String^  cruiseName);

    PicesSipperStation^      SipperStationLoad (String^  cruiseName,
                                                String^  stationName
                                               );

    PicesSipperStationList^  SipperStationsLoadByGpsRange (double  latitudeMin,
                                                           double  latitudeMax,
                                                           double  longitudeMin,
                                                           double  longitudeMax
                                                          );

    void                     SipperStationInsert (PicesSipperStation^  station);

    void                     SipperStationUpdate (PicesSipperStation^  station);

    void                     SipperStationDelete (String^  cruiseName,
                                                  String^  stationName
                                                 );

  private:
    DataBasePtr   dbConn;
    PicesRunLog^  runLog;

    bool*         cancelFlag;   // Some operations need a cancelFlag;  for example "ImagesQuery".

    bool  lastOpSuccessful;
  };  /* PicesDataBase */
}
#endif

