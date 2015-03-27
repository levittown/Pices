#ifndef  _DATABASEIMAGE_
#define  _DATABASEIMAGE_

/**
 @class  MLL::DataBaseImage
 @details 
 @code
 ************************************************************************************************
 **   Used for returning results of Queries against the Images table in the Pices database (See *
 **   DataBase::ImagesQuery).                                                                    *
 ************************************************************************************************
 @endcode
 */

#ifndef  _mysql_h
struct  MYSQL;
struct  MYSQL_RES;
struct  MSQL_STRUCT; 
typedef  MSQL_STRUCT* MYSQL_ROW;
#endif

#include "OSservices.h"
#include "RunLog.h"
using namespace KKB;

#include "RasterSipper.h"
using namespace  SipperHardware;

namespace MLL 
{


  #ifndef  _MLCLASS_
  class  MLClass;
  typedef  MLClass*  MLClassPtr;
  class  MLClassList;
  typedef  MLClassList*  MLClassListPtr;
  #endif




  class  DataBaseImage
  {
  public:
    typedef  DataBaseImage*  DataBaseImagePtr;

    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::ulong    ulong;
    typedef  KKB::uchar    uchar;

    DataBaseImage ();
    ~DataBaseImage ();

    float           BreakTie              () const;
    osFilePos       ByteOffset            () const  {return  byteOffset;}
    kkuint32        CentroidCol           () const  {return  centroidCol;}
    kkuint32        CentroidRow           () const  {return  centroidRow;}
    MLClassPtr      Class1                () const  {return  class1;}
    const KKStr&    Class1Name            () const;
    float           Class1Prob            () const  {return  class1Prob;}
    MLClassPtr      Class2                () const  {return  class2;}
    const KKStr&    Class2Name            () const;
    float           Class2Prob            () const  {return  class2Prob;}
    kkuint32        ClassLogEntryId       () const  {return  classLogEntryId;}
    uchar           ConnectedPixelDist    () const  {return  connectedPixelDist;}
    float           Depth                 () const  {return  depth;}
    const uchar*    EncodedThumbnail      () const  {return  encodedThumbnail;}
    kkuint32        EncodedThumbnailLen   () const  {return  encodedThumbnailLen;}
    kkuint32        ExtractionLogEntryId  () const  {return  extractionLogEntryId;}
    kkuint32        Height                () const  {return  height;}
    KKStr           ImageFileName         () const  {return  imageFileName;}
    kkint32         ImageId               () const  {return  imageId;}
    float           ImageSize             () const  {return  imageSize;}
    kkuint32        PixelCount            () const  {return  pixelCount;}
    kkint32         SipperFileId          () const  {return  sipperFileId;}
    const KKStr&    SipperFileName        () const  {return  sipperFileName;}
    PointListPtr    SizeCoordinates       () const  {return  sizeCoordinates;}
    RasterSipperPtr ThumbNail             (RunLog&  log) const;
    kkuint32        TopLeftRow            () const  {return  topLeftRow;}
    kkuint32        TopLeftCol            () const  {return  topLeftCol;}
    MLClassPtr      ValidatedClass        () const  {return  validatedClass;}
    const KKStr&    ValidatedClassName    () const;
    kkuint32        Width                 () const  {return  width;}


    RasterSipperPtr       GetOrigImageFromSipperFile (RunLog&  log) const;  /**< Will go to the original Sipper file and extract the image. */

    void    ByteOffset           (osFilePos         _byteOffset)           {byteOffset           = _byteOffset;}
    void    CentroidCol          (kkuint32          _centroidCol)          {centroidCol          = _centroidCol;}
    void    CentroidRow          (kkuint32          _centroidRow)          {centroidRow          = _centroidRow;}
    void    Class1               (MLClassPtr        _class1)               {class1               = _class1;}
    void    Class1Name           (const char*       _class1Name);
    void    Class1Prob           (float             _class1Prob)           {class1Prob           = _class1Prob;}
    void    Class2               (MLClassPtr        _class2)               {class2               = _class2;}
    void    Class2Name           (const char*       _class2Name);
    void    Class2Prob           (float             _class2Prob)           {class2Prob           = _class2Prob;}
    void    ClassLogEntryId      (kkuint32          _classLogEntryId)      {classLogEntryId      = _classLogEntryId;}
    void    ConnectedPixelDist   (uchar             _connectedPixelDist)   {connectedPixelDist   = _connectedPixelDist;}
    void    Depth                (float             _depth)                {depth                = _depth;}
    void    EncodedThumbnail     (uchar*            _encodedThumbnail)     {encodedThumbnail     = _encodedThumbnail;}
    void    EncodedThumbnailLen  (kkuint32          _encodedThumbnailLen)  {encodedThumbnailLen  = _encodedThumbnailLen;}
    void    ExtractionLogEntryId (kkuint32          _extractionLogEntryId) {extractionLogEntryId = _extractionLogEntryId;}
    void    Height               (kkuint32          _height)               {height               = _height;}
    void    ImageFileName        (const KKStr&      _imageFileName)        {imageFileName        = _imageFileName;}
    void    ImageId              (kkint32           _imageId)              {imageId              = _imageId;}
    void    ImageSize            (float             _imageSize)            {imageSize            = _imageSize;}
    void    PixelCount           (kkuint32          _pixelCount)           {pixelCount           = _pixelCount;}
    void    SipperFileId         (kkint32           _sipperFileId)         {sipperFileId         = _sipperFileId;}
    void    SipperFileName       (const KKStr&      _sipperFileName)       {sipperFileName       = _sipperFileName;}  
    void    SizeCoordinates      (PointListPtr      _sizeCoordinates);                                                    /**< Giving ownership of '_sizeCoordinates' to this instance of 'DataBaseImage'. */
    void    SizeCoordinates      (const KKStr       _sizeCoordinatesStr);                                                 /**< Giving ownership of '_sizeCoordinates' to this instance of 'DataBaseImage'. */
    void    SizeCoordinatesAdd   (const KKB::Point& _point);                                                              /**< Adds additional point to end of 'SizeCoordinates' list. */
    void    TopLeftRow           (kkuint32          _topLeftRow)           {topLeftRow           = _topLeftRow;}
    void    TopLeftCol           (kkuint32          _topLeftCol)           {topLeftCol           = _topLeftCol;}
    void    ValidatedClass       (MLClassPtr        _validatedClass)       {validatedClass       = _validatedClass;}
    void    ValidatedClassName   (const char*       _validatedClassName);
    void    Width                (kkuint32          _width)                {width                = _width;}

  private:
    osFilePos     byteOffset;
    kkuint32      centroidRow;
    kkuint32      centroidCol;
    MLClassPtr    class1;
    float         class1Prob;
    MLClassPtr    class2;
    float         class2Prob;
    kkuint32      classLogEntryId;
    uchar         connectedPixelDist;
    float         depth;
    uchar*        encodedThumbnail;
    kkuint32      encodedThumbnailLen;
    kkuint32      extractionLogEntryId;
    kkuint32      height;
    KKStr         imageFileName;
    kkint32       imageId;
    float         imageSize;
    kkuint32      pixelCount;
    kkint32       sipperFileId;
    KKStr         sipperFileName;
    PointListPtr  sizeCoordinates;
    kkuint32      topLeftRow;
    kkuint32      topLeftCol;
    MLClassPtr    validatedClass;
    kkuint32      width;
  };  /* DataBaseImage */



  typedef  DataBaseImage::DataBaseImagePtr  DataBaseImagePtr;



  class  DataBaseImageList: public  KKQueue<DataBaseImage>
  {
  public:
    typedef  DataBaseImageList*  DataBaseImageListPtr;

    DataBaseImageList (bool  _owner = true);
    ~DataBaseImageList ();

    void  SortBySpatialDistance ();


    MLClassListPtr  ExtractListOfClasses ();

    DataBaseImageListPtr  ExtractImagesForAGivenClass (MLClassPtr  _mlClass);


    VectorFloat  CalculateDensitesByQuadrat (float        scanRate,     // Scan Lines per Sec.
                                             float        quadratSize,  // Meters.
                                             float        flowRate,     // Meters per Sec
                                             const bool&  cancelFlag,
                                             RunLog&      log
                                            );

  private:
    class  SpatialComparison;
  };  /* DataBaseImageList */



  typedef  DataBaseImageList::DataBaseImageListPtr  DataBaseImageListPtr;


}  /* namespace MLL */


#endif
