#ifndef  _SIPPERVARIABLES_
#define  _SIPPERVARIABLES_


#include "GoalKeeper.h"
#include "KKStr.h"



/** @namespace  SipperHardware
 *  @brief Contains Classes that are specific to SIPPER's physical characteristics such as Instrument Data.
 *  @details Classes that represent Instruments, SIPPER File Formats, Cruises, Stations, and Deployments are grouped here.
 *           The PICES software installation specifics are also dealt within this namespace.
 */
namespace  SipperHardware  
{
  /**
    *@class  SipperVariables
    *@brief Variables that specify where we can locate SIPPER and PICES directories and files are grouped here.
    *@details  All methods and applications are to use these variables to locate files and directories that they need.
    *         This way all the knowledge or where to locate things is kept in this one class.
    */
  class  SipperVariables
  {
  public:
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::kkint64 kkint64;
    typedef  KKB::kkuint64  kkuint64;

    SipperVariables ();
    ~SipperVariables ();

    static  KKB::KKStrPtr       picesHomeDir;
    static  KKB::GoalKeeperPtr  blocker;

    static  void  SetPicesHomeDir (const KKB::KKStr&  _picesHomeDir);
    static  void  FinalCleanUp ();

    static  bool        AllowUpdates                   ();  /**< 'True' indicates that user is allowed to peofrom updates.                           */
    static  KKB::KKStr  InstrumentDataFilesDir         ();  /**< @brief Directory where Instrument Data files are stored.                            */
    static  KKB::KKStr  InstrumentDataReportDir        ();  /**< @brief Directory where Instrument Data Reports are written to.                      */
    static  KKB::KKStr  PicesHomeDir                   ();  /**< @brief Pices Home Directory.  All other directories will be off this directory.     */
    static  KKB::KKStr  PicesConfigurationDirectory    ();  /**< @brief Where application configuration files go;  NOT training models.              */
    static  KKB::KKStr  PicesCmdLineProgDir            ();  /**< @brief Directory where you can Command Line programs such as "CrossValidation.exe"  */
    static  KKB::KKStr  PicesDataFilesDirectory        ();
    static  KKB::KKStr  PicesTempDirectory             ();
    static  KKB::KKStr  SipperConfigrationFileName     ();
    static  KKB::KKStr  PicesReportDir                 ();
    static  KKB::KKStr  PicesDataFileDirectory         ();
    static  KKB::KKStr  SipperDirectoryListFileName    ();  /**< @brief Specifies the file where we can list directories that containing Sipper Files. */
    static  KKB::KKStr  SipperFilesDefaultDir          ();
    static  KKB::KKStr  SipperInterfaceDir             ();
    static  KKB::KKStr  SipperInterfaceLogDir          ();  /**< @brief Where the SipperInterface program will write its log files to.               */
    static  KKB::KKStr  TrainingModelsConfigurationDir ();  /**< @brief Directory where all Training Model Configuration files are stored.           */
    static  KKB::KKStr  TrainingModelsSaveFilesDir     ();  /**< @brief Directory where all built trained models are stored.                         */
    static  KKB::KKStr  TrainingLibrariesDir           ();  /**< @brief Directory where training library directories will be under                   */
    static  KKB::KKStr  ValidationInfoDir              ();  /**< @brief Directory where Validated Image File Names are stored during image extraction. */



    /**
     *@brief  Expands the supplied string by substituting in Environment variable values.
     *@details  Similar to the 'osSubstituteInEvironmentVariables' method defined in 'osServices' except that there is some special
     *          processing for 'PicesHomeDir'.  If  'PicesHomeDir' is not defined then the default location "C:\\Pices" will be used.
     *          If a environment variable other than "PicesHomeDir" is not defined then the original string will be returned.<br />
     *          ex:  "$(PicesHomeDir)\WFS\Copepod"  will expand to: "c:\Pices\WFS\Copepod".
     *
     *@param[in]  src  String that is to be search for environment variables and expanded.
     *@return  The expanded string.
     */
    static  KKB::KKStr  SubstituteInEvironmentVariables (const KKB::KKStr&  src);

    static  KKB::KKStr  LatitudeToStr  (double latitude);

    static  KKB::KKStr  LongitudeToStr (double longitude);


    /**
     *@brief  Will parse the name of a Image that was extracted from a SIPPER file into SipperFileName, ScanLineNum, amd ScanCol
     *@param[in] fullFileName Name of file that was assigned by the SIPPER image extraction program. from this name the three fields 
     *  SipperFileName, ScanLineNum, amd ScanCol can be extractd.
     *@param[in] sipperFileName Name of SIPPERfile that the image was orignally extracted from.
     *@param[in] scanLineNum The scan line with respect to the SIPPER file where the first row in the mage was extracted.
     *@param[in] scanCol The left most column in the original SIPPER file that this image  was extracted.
     */
    static  void  ParseImageFileName (const KKB::KKStr&  fullFileName,
                                      KKB::KKStr&        sipperFileName, 
                                      KKB::kkuint32&     scanLineNum,
                                      KKB::kkuint32&     scanCol
                                     );


  private:
    static  void   ReadPermissions ();

    static  kkint32  LocateEnvStrStart (const KKB::KKStr&  str);
    static  bool   ValidPicesHomeDir (const KKB::KKStr&  dir);

    static  bool   allowUpdates;
    static  bool   picesHomeDirInitialized;
  };
}


#endif

