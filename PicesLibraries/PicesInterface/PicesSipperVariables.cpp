#include "StdAfx.h"
#include "FirstIncludes.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <algorithm>
#include <functional>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "KKQueue.h"
using namespace  KKB;



#include "PicesVariables.h"
using namespace  MLL;



#include "OSservices.h"
#include "PicesKKStr.h"
#include "PicesSipperVariables.h"

using namespace  PicesInterface;

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace KKB;
using namespace System::Windows::Forms;





PicesSipperVariables::PicesSipperVariables (void)
{
}



void  PicesSipperVariables::InitializeEnvironment ()
{
  PicesVariables::InitializeEnvironment ();
}



bool PicesSipperVariables::AllowUpdates ()
{
  return PicesVariables::AllowUpdates ();
}


void  PicesSipperVariables::UpdatesNotAllowed (String^  caption)
{
  System::Windows::Forms::MessageBox::Show ("Updates are not allowed.", caption);
  return;
}


String^  PicesSipperVariables::InstrumentDataFilesDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::InstrumentDataFilesDir ());
}



String^  PicesSipperVariables::HomeDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::HomeDir ());
}


String^  PicesSipperVariables::ConfigurationDirectory ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::ConfigurationDirectory ());
}



String^  PicesSipperVariables::CmdLineProgDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::CmdLineProgDir ());
}



String^  PicesSipperVariables::DataFileDirectory ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::DataFileDirectory ());
}



String^  PicesSipperVariables::DataFilesDirectory ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::DataFilesDirectory ());
}



String^  PicesSipperVariables::ConfigrationFileName ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::ConfigrationFileName ());
}



String^  PicesSipperVariables::PicesReportDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::ReportDir ());
}



String^  PicesSipperVariables::TempDirectory ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::TempDirectory ());
}



String^  PicesSipperVariables::SipperDirectoryListFileName ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::SipperDirectoryListFileName ());
}


String^  PicesSipperVariables::SipperFilesDefaultDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::SipperFilesDefaultDir ());
}


String^  PicesSipperVariables::SipperFilesDirectory ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::SipperFilesDefaultDir ());
}


String^  PicesSipperVariables::SipperInterfaceDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::SipperInterfaceDir ());
}


String^  PicesSipperVariables::SipperInterfaceLogDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::SipperInterfaceLogDir ());
}  /* SipperInterfaceLogDir */


    

String^  PicesSipperVariables::TrainingModelsConfigurationDir ()
{
  return  PicesKKStr::KKStrToSystenStr (PicesVariables::TrainingModelsConfigurationDir ());
}
