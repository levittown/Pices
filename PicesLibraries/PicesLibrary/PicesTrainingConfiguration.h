#if  !defined(_LARCOSTRAININGCONFIGURATION_)
#define  _LARCOSTRAININGCONFIGURATION_

#include "XmlStream.h"
using namespace  KKB;

#include "FileDesc.h"
#include "TrainingConfiguration2.h"
using namespace KKMLL;

namespace  MLL 
{

  class  PicesTrainingConfiguration:  public  TrainingConfiguration2
  {
  public:
    typedef  PicesTrainingConfiguration*  PicesTrainingConfigurationPtr;
    typedef  PicesTrainingConfiguration const  PicesTrainingConfigurationConst;
    typedef  PicesTrainingConfigurationConst  *  PicesTrainingConfigurationConstPtr;


    PicesTrainingConfiguration ();

    PicesTrainingConfiguration (const TrainingConfiguration2&  tc);

    PicesTrainingConfiguration (const PicesTrainingConfiguration&  tc);


    /**
     *@brief Use this one if you want to create a default Configuration object.
     */
    PicesTrainingConfiguration (MLClassListPtr  _mlClasses,
                                KKStr           _parameterStr,
                                RunLog&         _log
                               );

    ~PicesTrainingConfiguration ();


    virtual
    void  Load (const KKStr&  _configFileName,
                bool          _validateDirectories,  /**<  Used to default to 'true'. */
                RunLog&       _log
               );


    virtual
    PicesTrainingConfigurationPtr  Duplicate ()  const;


    virtual
    FactoryFVProducerPtr   DefaultFeatureVectorProducer (RunLog&  runLog)  const;

    static
    PicesTrainingConfiguration*  CreateFromDirectoryStructure 
                                            (const KKStr&  _existingConfigFileName,
                                             const KKStr&  _subDir,
                                             RunLog&       _log,
                                             bool&         _successful,
                                             KKStr&        _errorMessage
                                            );

    static
    PicesTrainingConfiguration*  CreateFromFeatureVectorList (FeatureVectorList&  _examples,
                                                              RunLog&             _log
                                                             );

    virtual
    void  Save (const KKStr&  fileName)  const;

    virtual void  ReadXML (XmlStream&      s,
                           XmlTagConstPtr  tag,
                           VolConstBool&   cancelFlag,
                           RunLog&         log
                          );

    void  WriteXML (const KKStr&  varName,
                    std::ostream&  o
                   )  const;


  private:
  };  /* PicesTrainingConfiguration */


  #define  _PicesTrainingConfiguration_Defined_

  typedef  PicesTrainingConfiguration::PicesTrainingConfigurationPtr       PicesTrainingConfigurationPtr;
  typedef  PicesTrainingConfiguration::PicesTrainingConfigurationConst     PicesTrainingConfigurationConst;
  typedef  PicesTrainingConfiguration::PicesTrainingConfigurationConstPtr  PicesTrainingConfigurationConstPtr;



  class  XmlElementLarcosTrainingConfiguration: public XmlElementTrainingConfiguration2
  {
  public:
    XmlElementLarcosTrainingConfiguration (XmlTagPtr   tag,
                                           XmlStream&  s,
                                           VolConstBool&  cancelFlag,
                                           RunLog&     log
                                          ):
          XmlElementTrainingConfiguration2 (tag, s, cancelFlag, log)
    {}

  };
  
  typedef  XmlElementLarcosTrainingConfiguration*  XmlElementLarcosTrainingConfigurationPtr;

}  /* namespace LarcosCounterUnManaged */

#endif
