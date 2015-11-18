#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;

#include  "InstrumentDataFileManager.h"
using namespace MLL;

#include  "PicesInstrumentData.h"
#include  "PicesSipperFile.h"
#include  "PicesRunLog.h"


namespace PicesInterface 
{

  ///<summary> Wrapper class for unmanaged InstrumentDataFileManager class in SipperInstruments library. </summary>
  public  ref class PicesInstrumentDataFileManager
  {
  public:
    PicesInstrumentDataFileManager ();
    ~PicesInstrumentDataFileManager ();
    !PicesInstrumentDataFileManager ();

    PicesInstrumentDataList^   ReExtractInstrumentDataForSipperFile (String^            _sipperFileName,
                                                                     PicesSipperFile^  _sipperFile,
                                                                     PicesRunLog^      _log
                                                                    );

    property  bool   Cancel     {bool  get ();  void  set (bool  _cancel);}


  private:
    bool*  cancelFlag;
  };
}
