// BuildSynthObjDetData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FirstIncludes.h"
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "MemoryDebug.h"
#include "KKBaseTypes.h"
using namespace std;


#include "ImageIO.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "DataBase.h"
#include "ImageFeatures.h"
#include "SipperCruise.h"
#include "SipperDeployment.h"
#include "SipperFile.h"
#include "PicesVariables.h"
using namespace  MLL;


#include "BuildSynthObjDetData.h"


using namespace  PicesUtilittyApps;

// -SrcDir  D:\Pices\Cruises\WB1101\GpsData


BuildSynthObjDetData::BuildSynthObjDetData () :

  PicesApplication (),
  cancelFlag (false)
{
}



BuildSynthObjDetData::~BuildSynthObjDetData ()
{
}



void  BuildSynthObjDetData::InitalizeApplication (kkint32 argc,
                                                  char**  argv
                                                 )
{
  this->DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
}  /* InitalizeApplication */



bool  BuildSynthObjDetData::ProcessCmdLineParameter (const KKStr&  parmSwitch,
                                                     const KKStr&  parmValue
                                                    )
{
  bool  validParm = true;
  validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);

  return  validParm;
}  /* ProcessCmdLineParameter */



void  BuildSynthObjDetData::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
    << "   Application that will scan through all images in the MySQL Images table" << endl
    << "   and verify that a FullSize image exists for those that are larger than" << endl
    << "   their thumbnail version.  The data will be processed by SipperFile entries." << endl
    << endl;
}  /* DisplayCommandLineParameters */



int  main (int argc,  char** argv)
{
  BuildSynthObjDetData  app;
  app.InitalizeApplication (argc, argv);
  if (app.Abort ())
    return 1;

  app.Main ();
  if (app.Abort ())
    return 1;
  else
    return 0;
}
