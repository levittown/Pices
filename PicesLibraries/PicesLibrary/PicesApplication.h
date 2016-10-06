#if  !defined(_PICESAPPLICATION_)
#define  _PICESAPPLICATION_
/**
 *@class  MLL::PicesApplication
 *@brief Base class to All PICES applications.
 *@author  Kurt Kramer
 */

#include "Application.h"
#include "KKBaseTypes.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;

#include "FactoryFVProducer.h"
#include "FileDesc.h"
using namespace  KKMLL;


namespace MLL
{
#if  !defined(_DataBase_Defined_)
  class  DataBase;
  typedef  DataBase*  DataBasePtr;
  typedef  DataBase const *  DataBaseConstPtr;
#endif

#if  !defined(_DataBaseServer_Defined_)
  class  DataBaseServer;
  typedef  DataBaseServer*        DataBaseServerPtr;
  typedef  DataBaseServer const * DataBaseServerConstPtr;
#endif


#if  !defined(_PicesTrainingConfiguration_Defined_)
  class    PicesTrainingConfiguration;
  typedef  PicesTrainingConfiguration*  PicesTrainingConfigurationPtr;
#endif



  /** 
   *@class PicesApplication
   *@brief The base class for all standalone applications.
   *@details  This class is meant to be a general class that all standalone applications should be inherited 
   *from. If provides command line options that are common to all Pices Applications.
   * Supported Command Options:
   * -# -Config   <Model-Name>    The training model configuration file; only the root part needs to be specified; these files
   *                              can be found in the "$<PicesHomeDir>\Classifier\TrainingModels"  directory.
   * -# -DataBase <Description>   This  'Description' references the Description field in the MySQL.cfg file found in 
   *                              "${PicesHomeDir}\Configurations\" directory.
   * -# -LogFile  <File-Name>     File that logger messages (RunLog) are to be written to;  if not specified will default 
   *                              to standard-out (cout).
   * -# -CmdFile  <File-Name>     A text file that will contain additional command line options;  one of these options could
   *                              be another "-CmdFile" option.
   *
   * A derived class must implement the following methods:
   * - ApplicationName  Where you return the name of the derived application.
   * - InitalizeApplication  Where you implement the initialization code for the derived application.  One of the tasks required
   *                         by this method is to call the base class version of this method "PicesApplication::InitalizeApplication".  
   *                         If you find ant reason that the application may not be started; display an appropriate error message and
   *                         set the abort field to true by calling "Abort (true)".  If you want the base class to require the
   *                         "-Config" or "-DataBase" parameter then call "ConfigRequired (true)" and/or "DataBaseRequired (true)" 
   *                         before calling "PicesApplication::InitalizeApplication" and check the "Abort()" flag to see if errors
   *                         were detected.
   * - ProcessCmdLineParameter This method will be called once for each command line parameter and its associated value. If it 
   *                         is one of your parameters you get to process it in this method.  If you do not recognize the parameter
   *                         call the base class version of this method "PicesApplication::ProcessCmdLineParameter".
   * - 
   */
  class  PicesApplication: public  Application
  {
  public:
    /** 
     *@brief  Constructor for Application class that will start with a default logger(RunLog),
     *@details After creating an instance of this class you initialize it by calling InitalizeApplication.
     */
    PicesApplication ();


    /** 
     *@brief  Copy Constructor for PicesApplication class.
     *@param[in]  _application  PicesApplication instance to copy.
     */
    PicesApplication (const PicesApplication&  _application);


    /**
     *@brief  Constructor for PicesApplication class with no parameters.
     *@details This constructor is not interested in any command line parameters.
     *@param[in]  _log  A reference to a RunLog object.
     */
    PicesApplication (KKB::RunLog&  _log);



    virtual
    ~PicesApplication ();


    /** Specify the name of the application */
    virtual 
    const char*  ApplicationName ()  const;

    /** 
     *@brief  Initialized PicesApplication Instance; 1st method to be called after instance construction.
     *@see  RunLog
     *@see  ProcessCmdLineParameters
     *@param[in]  argc  Number of arguments in argv.
     *@param[in]  argv  List of asciiz strings; one string for each argument.
     */
    virtual
    void     InitalizeApplication (kkint32 argc,
                                   char**  argv
                                  );


    DataBasePtr   DB ();

    PicesTrainingConfigurationPtr  Config             ()  const  {return config;}
    const KKStr&                   ConfigFileName     ()  const  {return configFileName;}
    const KKStr&                   ConfigFileFullPath ()  const  {return configFileFullPath;}
    DataBaseServerPtr              DbServer           ()  const  {return dbServer;}
    FileDescConstPtr               FileDesc           ()  const  {return fileDesc;}

    void                           FvFactoryProducer  (FactoryFVProducerPtr _fvFactoryProducer)  {fvFactoryProducer = _fvFactoryProducer;}

    void  PrintStandardHeaderInfo (ostream&  o);


  protected:

    /**
     *@brief A derived class would call this method in its version of 'InitalizeApplication' just before calling
     * 'PicesApplication::InitalizeApplication' to let it know that the Training Model Configuration parameter
     * is required.
     */
    void  ConfigRequired (bool _configRequired)  {configRequired = _configRequired;}


    /**
     *@brief A derived class would call this method in its version of 'InitalizeApplication' just before calling
     * 'PicesApplication::InitalizeApplication'.
     *@details If set to true will require that a database connection be established otherwise the application will
     * abort.
     */
    void  DataBaseRequired (bool _dataBaseRequired)  {dataBaseRequired = _dataBaseRequired;}


    void  DisplayCommandLineParameters ();

    /**
     *@brief This method will get called once for each parameter specified in the command line.
     *@details  Derived classes should define this method to intercept parameters that they are interested in.
     *          Parameters are treated as pairs, Switched and Values where switches have a leading dash("-").
     *          The CmdLineExpander class will be used to expand any "-CmdFile" parameters.
     *@param[in] parmSwitch      The fill switch parameter.
     *@param[in] parmValue       Any value parameter that followed the switch parameter.
     */
    virtual 
    bool  ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                   const KKStr&  parmValue
                                  );

  private:
    bool  ProcessDataBaseParameter (const KKStr&  parmSwitch, 
                                    const KKStr&  parmValue
                                   );

    bool  ProcessConfigFileParameter (const KKStr&  parmSwitch, 
                                      const KKStr&  parmValue
                                     );

  protected:
    PicesTrainingConfigurationPtr  config;
    KKStr                          configFileName;
    KKStr                          configFileFullPath;
    FileDescConstPtr               fileDesc;
    FactoryFVProducerPtr           fvFactoryProducer;

  private:
    bool               configRequired;
    DataBasePtr        db;
    DataBaseServerPtr  dbServer;
    bool               dataBaseRequired;

  };  /* PicesApplication */
}  /* NameSpace KKB */

#endif
