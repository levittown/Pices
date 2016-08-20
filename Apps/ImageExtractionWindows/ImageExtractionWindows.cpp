#include  "StdAfx.h"
#include "FirstIncludes.h"
// ImageExtractionWindows.cpp : Defines the class behaviors for the application.
//
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include  "MemoryDebug.h"

using namespace  std;

typedef wchar_t WCHAR;

#include "stdafx.h"

#include "KKBaseTypes.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace  KKB;

#include "MLClass.h"
using namespace  KKMLL;


#include "DataBase.h"
#include "Instrument.h"
#include "InstrumentDataFileManager.h"
#include "PicesVariables.h"
using namespace  MLL;

#include "ExtractionParms.h"
using namespace  ImageExtractionManager;


#include "ImageExtractionWindows.h"
#include "ImageExtractionWindowsDlg.h"



// -S D:\TrainingApp\SipperFiles\Panama\Station8D\Station8D_03.spr   -A 0  -CPD 3  -min 300  -max 0  -Pre On   -d  D:\TrainingApp\ExtractedImages\Panama\Station8D\Station8D_03    -R D:\TrainingApp\ExtractedImages\Panama\Station8D\Station8D_03\Station8D_03_c0.txt   -S D:\TrainingApp\SipperFiles\Panama\Station8D\Station8D_03.spr  -SF Sipper3    -x  -c D:\TrainingApp\DataFiles\TrainingModels\etpB_training.cfg
// -A 0  -CPD 3  -D D:\TrainingApp\ExtractedImages\Station1G_10-xxxx\  -ipd 1000  -min 500  -max 0  -Pre On  -R D:\TrainingApp\ExtractedImages\Station1G_10-xxxx\Station1G_10_c0.txt  -S D:\TrainingApp\SipperFiles\Station1G_10.spr  -SF Sipper3  -C D:\TrainingApp\DataFiles\TrainingModels\etpB_training.cfg  -X

// -A 0  -CPD 3  -D D:\Pices\ExtractedImages\GG1002005S_01  -ipd 1000  -min 150  -max 0  -Pre On  -R D:\Pices\ExtractedImages\GG1002005S_01\GG1002005S_01_c0.txt  -S F:\Pices\SipperFiles\GG1002\GG1002005S\GG1002005S_01.spr  -SF Sipper3  -C D:\Pices\DataFiles\TrainingModels\GulfOilBroad2_Discreate6_Dual.cfg  -X  -CountOnly  -MultiThreaded Yes 


// -c USF_MFS.cfg  -S F:\Pices\SipperFiles\USF\WB0512\PCB05\WB0512PCB05_03.spr  -Min 250  -sf Sipper3  -d F:\Pices\ExtractedImages\WB0512\PCB05\WB0512PCB05_03  -x

// -c USF_Dual_MFS-BFS_current -DataBase LOCAL_USF -s WB1008DSH09_03 -x  -min 100   -d F:\Pices\ExtractedImages\WB1008\WB1008DSH09\WB1008DSH09_03

// -D F:\Pices\ExtractedImages\WB1008DSH09_04\  -min  100  -S F:\Pices\SipperFiles\WB1008\DSH09\WB1008DSH09_04.spr  -C USF_Dual_MFS-BFS_current   X  -CountOnly  -DataBase Default    -t 38000000
// -D D:\Pices\ExtractedImages\ETP08_1A_01\  -min 100 -S F:\Pices\SipperFiles\ETP2008\ETP2008_1A\ETP08_1A_01.spr   -c USF_Dual_MFS-BFS_current  -X  -CountOnly  -MultiThreaded Yes  

// -D D:\Larcos\ExtractedImages\2016-08-02_PortJeff_019\  -min 100 -S D:\Larcos\ScannerFiles\2016-08-01_PortJeff\2016-08-02_PortJeff_019\PortJeff_019_20160802-105133.spr   -c USF_Dual_MFS-BFS_current  -X  -CountOnly  -MultiThreaded Yes  
                                                                                                                                         
// -D D:\Larcos\ExtractedImages\2016-08-02_PortJeff_019\  -min 90 -S D:\Larcos\ScannerFiles\2016-08-01_PortJeff\2016-08-02_PortJeff_019\PortJeff_019_20160802-105133.spr   -c USF_Dual_MFS-BFS_current  -X  -CountOnly  -MultiThreaded Yes    -F   -M 90
// -D D:\Larcos\ExtractedImages\2016-08-02_PortJeff_019\  -min 100 -S D:\Larcos\ScannerFiles\2016-08-01_PortJeff\2016-08-02_PortJeff_019\PortJeff_019_20160802-105133.spr   -MultiThreaded Yes    -FO   -M 90


/////////////////////////////////////////////////////////////////////////////
// CImageExtractionWindowsApp

BEGIN_MESSAGE_MAP(CImageExtractionWindowsApp, CWinApp)
	//{{AFX_MSG_MAP(CImageExtractionWindowsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CImageExtractionWindowsApp construction
CImageExtractionWindowsApp::CImageExtractionWindowsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

  #if  defined (WIN32)  &&  defined (_DEBUG)  &&  !defined(_NO_MEMORY_LEAK_CHECK_)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
  #endif

  kkuint32  zed = 0;
}


CImageExtractionWindowsApp::~CImageExtractionWindowsApp()
{

}



/////////////////////////////////////////////////////////////////////////////
// The one and only CImageExtractionWindowsApp object
CImageExtractionWindowsApp theApp;




/////////////////////////////////////////////////////////////////////////////
// CImageExtractionWindowsApp initialization
BOOL CImageExtractionWindowsApp::InitInstance()
{
  PicesVariables::InitializeEnvironment ();


	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


  bool   cmdLineGood = false;
  KKStr  errorMessage;

  osRunAsABackGroundProcess ();
  RunLog  log;

  ExtractionParmsPtr  multiThreadedExtractionParms = NULL;
  
  KKStr  progName = "ImageExtractionWindows";
  char  buff[10240];
  size_t pReturnValue;
  wcstombs_s (&pReturnValue, buff, sizeof (buff), m_lpCmdLine,  (size_t)sizeof (buff));
  KKStr  cmdLine = buff;

  multiThreadedExtractionParms 
       = new ExtractionParms (progName, 
                              cmdLine, 
                              true,               // Running as Windows App
                              errorMessage, 
                              cmdLineGood,
                              log
                             );

  if  (!errorMessage.Empty ())
  {
    size_t  errReturnValue;
    wchar_t buff[4096];
    errno_t errNum = mbstowcs_s (&errReturnValue, buff, 4096,   errorMessage.Str (), _TRUNCATE);
    int returnCd = MessageBox (NULL,
                               buff,
                               L"Invalid Parameters",
                               MB_OK
                              );
    multiThreadedExtractionParms->DisplayExampleCmdLine ();
    delete  multiThreadedExtractionParms;
    multiThreadedExtractionParms = NULL;
    return  false;
  }

  {
    CImageExtractionWindowsDlg dlg;

    dlg.MultiTreadedExtractionParms (multiThreadedExtractionParms);  /**< 'dlg' will take ownership of 'multiThreadedExtractionParms'.  */

	  m_pMainWnd = &dlg;
	  int nResponse = dlg.DoModal();
	  if (nResponse == IDOK)
	  {
		  // TODO: Place code here to handle when the dialog is
		  //  dismissed with OK
	  }
	  else if (nResponse == IDCANCEL)
	  {
		  // TODO: Place code here to handle when the dialog is
		  //  dismissed with Cancel
	  }
  }

  //delete  imageExtractionParms;

  //InstrumentData::FinalCleanUp ();
  //Instrument::FinalCleanUp ();
  //FeatureFileIO::FinalCleanUp ();
  //FileDesc::FinalCleanUp ();
  //InstrumentDataFileManager::FinalCleanUp ();
  //MLClass::FinalCleanUp ();
  //DataBase::FinalCleanUp ();
  //SipperVariables::FinalCleanUp ();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
