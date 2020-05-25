#include "FirstIncludes.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "MemoryDebug.h"

using namespace  std;


#include "GlobalGoalKeeper.h"
#include "KKBaseTypes.h"
#include "KKException.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace  KKB;


#include "FeatureVector.h"
#include "FileDesc.h"
#include "MLClass.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;


#include "ImageFeatures.h"
#include "PicesTrainingConfiguration.h"
#include "PicesFVProducer.h"
#include "PicesVariables.h"
using namespace  MLL;



PicesTrainingConfiguration::PicesTrainingConfiguration ():
  TrainingConfiguration2 ()
{
}



PicesTrainingConfiguration::PicesTrainingConfiguration (MLClassListPtr  _mlClasses,
                                                        KKStr           _parameterStr,
                                                        RunLog&         _log
                                                       ):
  TrainingConfiguration2 (_mlClasses, PicesFVProducerFactory::Factory (&_log), _parameterStr, _log)
{
}



PicesTrainingConfiguration::PicesTrainingConfiguration (const TrainingConfiguration2&  tc):
  TrainingConfiguration2 (tc)
{
  if  (typeid (tc) == typeid (PicesTrainingConfiguration))
  {

  }
} 



PicesTrainingConfiguration::PicesTrainingConfiguration (const PicesTrainingConfiguration&  tc):
  TrainingConfiguration2 (tc)
{
} 



PicesTrainingConfiguration::~PicesTrainingConfiguration ()
{
}



void  PicesTrainingConfiguration::Load (const KKStr&  _configFileName,
                                        bool          _validateDirectories,  /**<  Used to default to 'true'. */
                                        RunLog&       _log
                                      )
{
  TrainingConfiguration2::Load (_configFileName, _validateDirectories, _log);
}



PicesTrainingConfigurationPtr  PicesTrainingConfiguration::Duplicate ()  const
{
  return new PicesTrainingConfiguration (*this);
}



FactoryFVProducerPtr   PicesTrainingConfiguration::DefaultFeatureVectorProducer (RunLog&  runLog)  const
{
  return PicesFVProducerFactory::Factory (&runLog);
}



void  PicesTrainingConfiguration::Save (const KKStr&  saveFileName)  const
{
  ofstream  o (saveFileName.Str ());

  TrainingConfiguration2::Save (o);

  o.close ();
}  /* Save */



PicesTrainingConfigurationPtr  PicesTrainingConfiguration::CreateFromFeatureVectorList
                                                    (FeatureVectorList&  _examples,
                                                     RunLog&             _log
                                                    )
{
  _log.Level (10) << "PicesTrainingConfiguration::CreateFromFeatureVectorList" << endl;

  MLClassListPtr  mlClasses = _examples.ExtractListOfClasses ();
  mlClasses->SortByName ();

  PicesTrainingConfigurationPtr  config 
      = new PicesTrainingConfiguration (mlClasses, 
                                        "-m 200 -s 0 -n 0.11 -t 2 -g 0.024717  -c 10  -u 100  -up  -mt OneVsOne  -sm P", 
                                        _log
                                       );

  config->SetFeatureNums (_examples.AllFeatures ());

  delete  mlClasses;  mlClasses = NULL;

  return  config;
}  /* CreateFromFeatureVectorList */




PicesTrainingConfigurationPtr  PicesTrainingConfiguration::CreateFromDirectoryStructure 
                                                    (const KKStr&  _existingConfigFileName,
                                                     const KKStr&  _subDir,
                                                     RunLog&       _log,
                                                     bool&         _successful,
                                                     KKStr&        _errorMessage
                                                    )
{
  _log.Level (10) << "PicesTrainingConfiguration::CreateFromDirectoryStructure"  << endl
                  << "                       Feature Data from SubDir[" << _subDir << "]." << endl
                  << endl;

  _successful = true;

  KKStr  directoryConfigFileName = osGetRootNameOfDirectory (_subDir);
  if  (directoryConfigFileName.Empty ())
    directoryConfigFileName = osAddSlash (PicesVariables::TrainingModelsConfigurationDir ()) + "Root.cfg";
  else
    directoryConfigFileName = osAddSlash (PicesVariables::TrainingModelsConfigurationDir ()) + directoryConfigFileName + ".cfg";

  PicesTrainingConfigurationPtr  config = NULL;

  if  (!_existingConfigFileName.Empty ())
  {
    if  (osFileExists (_existingConfigFileName))
    {
      config = new PicesTrainingConfiguration ();
      config->Load (_existingConfigFileName, false, _log);
      config->RootDir (_subDir);
      if  (!(config->FormatGood ()))
      {
        delete  config;
        config = NULL;
      }
    }
  }

  if  (!config)
  {
    if  (osFileExists (directoryConfigFileName))
    {
      config = new PicesTrainingConfiguration ();
      config->Load (directoryConfigFileName, false, _log);
      config->RootDir (_subDir);
      if  (!(config->FormatGood ()))
      {
        delete  config;
        config = NULL;
      }
    }
  }

  if  (!config)
  {
    config = new PicesTrainingConfiguration (NULL,      // Not supplying the MLClassList
                                             "=-s 0 -n 0.11 -t 2 -g 0.01507  -c 12  -u 100  -up  -mt OneVsOne  -sm P",
                                             _log
                                            );
    config->RootDir (_subDir);
  }

  config->BuildTrainingClassListFromDirectoryStructure (_subDir,
                                                        _successful, 
                                                        _errorMessage,
                                                        _log
                                                       );

  config->Save (directoryConfigFileName);

  return  config;
} /* CreateFromDirectoryTree */



void  PicesTrainingConfiguration::WriteXML (const KKStr&  varName,
                                            ostream&      o
                                           )  const
{
  XmlTag  startTag ("PicesTrainingConfiguration", XmlTag::TagTypes::tagStart);
  if  (!varName.Empty ())
    startTag.AddAtribute ("VarName", varName);
  startTag.WriteXML (o);
  o << endl;

  WriteXMLFields (o);

  XmlTag  endTag ("PicesTrainingConfiguration", XmlTag::TagTypes::tagEnd);
  endTag.WriteXML (o);
  o << endl;
}  /* WriteXML */



void   PicesTrainingConfiguration::ReadXML (XmlStream&      s,
                                            XmlTagConstPtr  tag,
                                            VolConstBool&   cancelFlag,
                                            RunLog&         log
                                           )

{
  XmlTokenPtr t = s.GetNextToken (cancelFlag, log);
  while  (t  &&  (!cancelFlag))
  {
    t = ReadXMLBaseToken (t, cancelFlag, log);
    if  (t)
    {
      const KKStr&  varName = t->VarName ();
      log.Level (50) << "PicesTrainingConfiguration::ReadXML  Token not processed varName: " << varName << "  tag->name" << tag->Name () << endl;
    }
    delete  t;
    if  (cancelFlag)
      t = NULL;
    else
      t = s.GetNextToken (cancelFlag, log);
  }
  delete  t;
  t = NULL;
  if  (!cancelFlag)
    ReadXMLPost (cancelFlag, log);
}  /* ReadXML */



//XmlFactoryMacro(PicesTrainingConfiguration)
