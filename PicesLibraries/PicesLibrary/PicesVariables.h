#if  !defined(_PICESVARIABLES_)
#define  _PICESVARIABLES_
/**
 *@class  MLL::PicesVariables
 *@brief Base class to All PICES applications.
 *@author  Kurt Kramer
 */

#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


#include "KKMLVariables.h"
using namespace  KKMLL;


namespace MLL
{
  /** 
   *@class PicesVariables
   *@brief Variables specific to PICES applications will be located in this class.
   */
  class  PicesVariables
  {
  public:
    static  void  InitializeEnvironment ();

    static  void  SetHomeDir (const KKB::KKStr&  _homeDir);

    static  bool          AllowUpdates                   ();  /**< 'True' indicates that user is allowed to perform updates.                           */
    static  KKStr         ConfigurationDirectory         ();  /**< @brief Where application configuration files go;  NOT training models.              */
    static  KKStr         CmdLineProgDir                 ();  /**< @brief Directory where you can Command Line programs such as "CrossValidation.exe"  */
    static  KKStr         ConfigrationFileName           ();
    static  KKStr         DataFileDirectory              ();
    static  KKStr         DataFilesDirectory             ();
    static  const KKStr&  HomeDir                        ();  /**< @brief Pices Home Directory.  All other directories will be off this directory.     */
    static  KKStr         InstrumentDataFilesDir         ();
    static  KKStr         InstrumentDataReportDir        ();
    static  KKStr         ReportDir                      ();
    static  KKStr         SipperInterfaceDir             ();
    static  KKStr         SipperInterfaceLogDir          ();
    static  KKStr         SipperDirectoryListFileName    ();
    static  KKStr         SipperFilesDefaultDir          ();
    static  KKStr         TempDirectory                  ();
    static  KKStr         TrainingLibrariesDir           ();  /**< @brief Directory where training library directories will be under                   */
    static  KKStr         TrainingModelsConfigurationDir ();  /**< @brief Directory where all Training Model Configuration files are stored.           */
    static  KKStr         TrainingModelsSaveFilesDir     ();  /**< @brief Directory where all built trained models are stored.                         */
    static  KKStr         ValidationInfoDir              ();

    static  KKStr  LatitudeToStr   (double latitude);

    static  KKStr  LongitudeToStr  (double longitude);

    static  void   ParseImageFileName (const KKStr&  fullFileName, 
                                       KKStr&        sipperFileName,
                                       kkuint32&     scanLineNum,
                                       kkuint32&     scanCol
                                      );

  private:
    static  void  ReadPermissions ();
    static  bool  ValidHomeDir (const KKStr&  dir);

    static  bool          allowUpdates;
    static  KKStr         homeDir;
  };  /* PicesVariables */
}  /* NameSpace MLL */

#endif












