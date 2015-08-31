#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

//using namespace PicesCSharpClasses;


#include "OSservices.h"
#include "PicesRunLog.h"


namespace PicesInterface 
{

  public ref class PicesSipperVariables
  {
  public:
    PicesSipperVariables ();

    static  void  InitializeEnvironment ();

    static  bool     AllowUpdates                   ();  /**< 'True' indicates that user is allowed to peofrom updates.  */
    static  void     UpdatesNotAllowed (String^  caption);

    static  String^  InstrumentDataFilesDir         ();
    static  String^  ConfigurationDirectory         ();  // Where application configuration files go;  NOT training models.
    static  String^  CmdLineProgDir                 ();  // Directory where you can find Command Line programs such as "CrossValidation.exe"
    static  String^  DataFilesDirectory             ();
    static  String^  HomeDir                        ();
    static  String^  TempDirectory                  ();
    static  String^  DataFileDirectory              ();
    static  String^  PicesReportDir                 ();
    static  String^  ConfigrationFileName           ();
    static  String^  SipperDirectoryListFileName    ();
    static  String^  SipperFilesDefaultDir          ();
    static  String^  SipperFilesDirectory           ();   // Same as 'SipperFilesDefaultDir'
    static  String^  SipperInterfaceDir             ();
    static  String^  SipperInterfaceLogDir          ();
    static  String^  TrainingModelsConfigurationDir ();
  };
}