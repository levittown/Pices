#ifndef  _FEATUREFILEIOPICES_
#define  _FEATUREFILEIOPICES_

#include "FeatureFileIO.h"
#include "FeatureNumList.h"
using namespace  KKMLL;

#include "ImageFeatures.h"


namespace MLL 
{

#if  !defined(_DataBase_Defined_)
  class  DataBase;
  typedef DataBase*  DataBasePtr;
#endif


  /**
    @class  FeatureFileIOPices
    @brief  Supports the reading and writing of feature data from and to Pices feature data files.
    @details
    @code
    * *************************************************************************************************
    * *  The PICES feature data file is a very simple format where the first few rows contain a       *
    * *  description of the rest of the data file.  Any row that starts with "//" will be treated as  *
    * *  comment line.                                                                                *
    * *  First Row:  Identifies the file as a feature Data File.  An example on the next line.        *
    * *              "FEATURE_DATA_FILE Version  51  NumOfFeatures  88  NumOfImages  258"             *
    * *              Each field will be separated by a tab character.  This line says that the        *
    * *              version number is 51. There are 88 features and 258 examples or feature vectors. *
    * *              The version number gets incremented when ever there is a change in the way       *
    * *              that feature data is computed.                                                   *
    * *                                                                                               *
    * *  Second Row: List all the fields in the feature data file.  The first fields are feature      *
    * *              data.  The first non feature field will be: "ClassName".  There are several no   *
    * *              feature data fields.  See the SipperSoftware documentation for details.          *
    * *************************************************************************************************
    @endcode
    @see  FeatureFileIO
    */
  class  FeatureFileIOPices:  public  FeatureFileIO
  {
  public:
    typedef  FeatureFileIOPices*  FeatureFileIOPicesPtr;

    typedef  KKB::kkuint32 kkuint32;

    FeatureFileIOPices ();
    virtual ~FeatureFileIOPices ();

    virtual  FileDescConstPtr  GetFileDesc (const KKStr&    _fileName,
                                            std::istream&   _in,
                                            MLClassListPtr  _classes,
                                            kkint32&        _estSize,
                                            KKStr&          _errorMessage,
                                            RunLog&         _runLog
                                           );


    virtual  ImageFeaturesListPtr  LoadFile (const KKStr&      _fileName,
                                             FileDescConstPtr  _fileDesc,
                                             MLClassList&      _classes, 
                                             std::istream&     _in,
                                             kkint32           _maxCount,    // Maximum # images to load.
                                             VolConstBool&     _cancelFlag,
                                             bool&             _changesMade,
                                             KKStr&            _errorMessage,
                                             RunLog&           _log
                                            );


    virtual  void   SaveFile (FeatureVectorList&     _data,
                              const KKStr&           _fileName,
                              const FeatureNumList&  _selFeatures,
                              std::ostream&          _out,
                              kkuint32&              _numExamplesWritten,
                              VolConstBool&          _cancelFlag,
                              bool&                  _successful,
                              KKStr&                 _errorMessage,
                              RunLog&                _log
                             );


    static  FeatureFileIOPicesPtr  Driver                 ()                  {return &driver;}
    static  FileDescConstPtr       NewPlanktonFile        ();
    static  KKStr                  PlanktonFieldName      (kkuint32  fieldNum);
    static  kkuint32               PlanktonMaxNumOfFields ();


    /**                       FeatureDataReSink
     *@brief Synchronizes the contents of a feature data file with a directory of images.
     *@details Used with plankton applications to verify that feature file is up-to-date.
     * Was specifically meant to work with training libraries, to account for
     * images being added and deleted from training library.  If there are no 
     * changes, then function will run very quickly.
     *
     * A change in feature file version number would also cause all entries in the feature 
     * file to be recomputed.  The feature file version number gets incremented whenever we change
     * the feature file computation routine.
     *
     *@param[in] _dirName       Directory where source images are located.
     *@param[in] _fileName      Feature file that is being synchronized.
     *@param[in] _unknownClass  Class to be used when class is unknown
     *@param[in] _useDirectoryNameForClassName  if true then class name of each entry
     *           will be set to directory name.
     *@param[in] _database If not NULL retrieves Instrument data from the PICES database table InstrumentData.
     *@param[in] _mlClasses  List of classes
     *@param[in] _cancelFlag  Boolean variable that will be monitored; if it goes True then will terminate 
     * processing right away and return to caller.
     *@param[out] _changesMade  If returns as true then there were changes made to the 
     *            feature file 'fileName'.  If set to false, then no changes were made.
     *@param[out] _timeStamp Timestamp of feature file.
     *@param[in] _log  where to send diagnostic messages to.
     *@returns - A ImageFeaturesList container object.  This object will own all the examples loaded
     */
    virtual
    ImageFeaturesListPtr  FeatureDataReSink (FactoryFVProducerPtr  _fvProducerFactory,
                                             const KKStr&          _dirName, 
                                             const KKStr&          _fileName, 
                                             MLClassPtr            _unknownClass,
                                             bool                  _useDirectoryNameForClassName,
                                             DataBasePtr           _dataBase,
                                             MLClassList&          _mlClasses,
                                             VolConstBool&         _cancelFlag,    // will be monitored,  if set to True  Load will terminate.
                                             bool&                 _changesMade,
                                             KKB::DateTime&        _timeStamp,
                                             RunLog&               _log
                                            );

    /**
     * Same as 'FeatureDataReSink' except does not include '_dataBase'; will call above method passing in 
     * NULL  for  '_dataBase'.
     */
    virtual
    ImageFeaturesListPtr  FeatureDataReSink (FactoryFVProducerPtr  _fvProducerFactory,
                                             const KKStr&          _dirName, 
                                             const KKStr&          _fileName, 
                                             MLClassPtr            _unknownClass,
                                             bool                  _useDirectoryNameForClassName,
                                             MLClassList&          _mlClasses,
                                             VolConstBool&         _cancelFlag,    // will be monitored,  if set to True  Load will terminate.
                                             bool&                 _changesMade,
                                             KKB::DateTime&        _timeStamp,
                                             RunLog&               _log
                                            );





    /**                       LoadInSubDirectoryTree
     *@brief Loads in the contents of a sub-directory tree.
     *@details Meant to work with SIPPER plankton images, it starts at a specified subdirectory and 
     *       transverses all subdirectories.  It makes use of FeatureDataReSink for each specific
     *       sub-directory.
     *@param[in] _rootDir  Starting directory.
     *@param[in,out] _mlClasses List of classes, any new classes in fileName will be added.
     *@param[in] _useDirectoryNameForClassName  if true set class names to sub-directory name.
     *           This happens because the user may manually move images between directories using
     *           the sub-directory name as the class name.
     *@param[in] _dataBase  If not null retrieve Instrument data from the Instruments database table.
     *@param[in] _cancelFlag  Will be monitored; if set to True Load will terminate.
     *@param[in] _rewiteRootFeatureFile  If true rewrite the feature file in the specified 'rootDir'.  This
     *           feature file will contain all entries from all sub-directories below it.
     *@param[in,out] _log  Logger where messages are to be written to.
     *@returns - A ImageFeaturesList container object that will own all the examples loaded.
     */
    virtual
      ImageFeaturesListPtr  LoadInSubDirectoryTree (FactoryFVProducerPtr  _fvProducerFactory,
                                                    KKStr                 _rootDir,
                                                    MLClassList&          _mlClasses,
                                                    bool                  _useDirectoryNameForClassName,
                                                    DataBasePtr           _dataBase,
                                                    VolConstBool&         _cancelFlag,
                                                    bool                  _rewiteRootFeatureFile,
                                                    RunLog&               _log
                                                   );


    virtual
      ImageFeaturesListPtr  LoadInSubDirectoryTree (FactoryFVProducerPtr  _fvProducerFactory,
                                                    KKStr                 _rootDir,
                                                    MLClassList&          _mlClasses,
                                                    bool                  _useDirectoryNameForClassName,
                                                    VolConstBool&         _cancelFlag,
                                                    bool                  _rewiteRootFeatureFile,
                                                    RunLog&               _log
                                                   );



  private:
    static FeatureFileIOPices  driver;
    static FileDescConstPtr    planktonFileDesc;
    static kkuint32            MaxNumPlanktonRawFields;
    static const char*         PlanktonRawFeatureDecriptions[];


    typedef  enum  {rfFeatureData,
                    rfClassName,
                    rfMLClassIDX,
                    rfImageFileName, 
                    rfOrigSize,
                    rfNumOfEdgePixels,
                    rfProbability,
                    rfCentroidRow,
                    rfCentroidCol,
                    rfPredictedClass,
                    rfPredictedClassIDX,
                    rfBreakTie,
                    rfSfCentroidRow,
                    rfSfCentroidCol,
                    rfLaditude,
                    rfLongitude,
                    rfDepth,
                    rfUnKnown
                   }  
                   PicesFieldTypes;


    /**
     *@brief Will refresh the Instrument Data fields of the FeatureVector instance.
     *@details  If 'dataBase' if provided will first try to retrieve from there otherwise
     * will retrieve from the InstrumentDataFileManager::GetClosestInstrumentData method.
     *@param[in] _imageFileName  Name of Plankton image that featureVector represents.
     *@param[in,out]  _featureVector  FeatureVector instance that is named by 'imageFileName'
     *@param[in]  _dataBase  If not NULL will 1st try to retrieve instrument data from here.
     *@param[in] _cancelFlag  Will be monitored; if set to True Load will terminate.
     *@param[in] _changesMade If the '_featureVector' instance is updated will be set to true.
     *@param[in] _log  Logger to send messages to.
     */
    static
    void  ReFreshInstrumentData (const KKStr&      _imageFileName,
                                 ImageFeaturesPtr  _featureVector,
                                 DataBasePtr       _dataBase,
                                 VolConstBool&     _cancelFlag,
                                 bool&             _changesMade,
                                 RunLog&           _log
                                );



    void  Parse_FEATURE_DATA_FILE_Line (KKStr&     line,
                                        kkuint32&  version,
                                        kkuint32&  numOfFields,
                                        kkuint32&  numOfExamples
                                       );


    VectorInt  CreateIndirectionTable (const VectorKKStr&  fields,
                                       kkint32               numOfFeatures
                                      );

  };

  typedef  FeatureFileIOPices::FeatureFileIOPicesPtr  FeatureFileIOPicesPtr;

}  /* namespace MLL */


#endif
