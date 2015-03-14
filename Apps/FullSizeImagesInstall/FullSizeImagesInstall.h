#if  !defined(_IMPORTGPS_DATA_)
#define  _IMPORTGPS_DATA_

#include "PicesApplication.h"
#include "DataBase.h"
#include "RunLog.h"
#include "KKStr.h"


namespace  FullSizeImagesInstall_DataNameSpace
{
  /**
   *@brief  Application that wil scan through all images in the MySQL Images table and verify that a FullSize image 
   * exists for those that are larger than their thumbnail version.  The data will be proecssed by SipperFile entries.
   */
  class  FullSizeImagesInstall:  public PicesApplication
  {
  public:
    FullSizeImagesInstall ();

    ~FullSizeImagesInstall ();

    virtual 
    const char*  ApplicationName () const  {return  "FullSizeImagesInstall";}

    virtual
    void  InitalizeApplication (kkint32 argc,
                                char**  argv
                               );

    void  Main ();

  private:
   class  SipperFileResults
   {
   public:
     SipperFileResults (KKStr  _sipperFileName,
                        kkint32  _sipperFileImageCount,
                        kkint32  _sipperFileThumbNail,
                        kkint32  _sipperFileAlreadyInFullSizeTable,
                        kkint32  _sipperFileMissingFromFullSize,
                        kkint32  _sipperFileImagesLost,
                        kkint32  _sipperFileImagesRecoverd
                       ):
         sipperFileName                   (_sipperFileName),
         sipperFileImageCount             (_sipperFileImageCount),
         sipperFileThumbNail              (_sipperFileThumbNail),
         sipperFileAlreadyInFullSizeTable (_sipperFileAlreadyInFullSizeTable),
         sipperFileMissingFromFullSize    (_sipperFileMissingFromFullSize),
         sipperFileImagesLost             (_sipperFileImagesLost),
         sipperFileImagesRecoverd         (_sipperFileImagesRecoverd)
     {}


     static  void  PrintHeader (ostream& o);
     void  Print (ostream&  o);

     SipperFileResults&  operator+= (const SipperFileResults& right);


     KKStr  sipperFileName;
     kkint32  sipperFileImageCount;
     kkint32  sipperFileThumbNail;              // number of images where the ThumbNail is the true extracted image.
     kkint32  sipperFileAlreadyInFullSizeTable;
     kkint32  sipperFileMissingFromFullSize;
     kkint32  sipperFileImagesLost;
     kkint32  sipperFileImagesRecoverd;
   };



    void   DisplayCommandLineParameters ();

    bool   ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                    const KKStr&  parmValue
                                   );

    void   ProcessSipperFile (SipperFilePtr  sipperFile);

    void   DiagnoseImage (DataBaseImagePtr  image, 
                          RasterSipperPtr   fullSizeImage
                         );


    bool                        cancelFlag;
    vector<SipperFileResults>   results;
    KKStr                       reportFileName;
    ostream*                    report;
    SipperFileListPtr           sipperFiles;
    kkint32                     sipperFilesProcessed;
  };  /* FullSizeImagesInstall */
}  /* FullSizeImagesInstall_DataNameSpace */

#endif
