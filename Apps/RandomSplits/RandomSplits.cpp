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


#include "Compressor.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "FeatureFileIO.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace KKMLL;


#include "DataBase.h"
#include "FeatureFileIOPices.h"
#include "InstrumentDataFileManager.h"
#include "ImageFeatures.h"
using namespace MLL;


#include "JobManager.h"
using namespace  JobManagment;


#include "RandomSplits.h"
#include "RandomSplitJob.h"
#include "RandomSplitJobManager.h"


// C:\Pices\DataFiles\FeatureSelection\Simple
// -C Simple.cfg  -DataFile  Simple.data  -NumSplits 10 -SplitPercentage 70%

// C:\Users\kkramer\Dissertation\BiasAdjustmentExperiment\NineClassPlankton
// -c Plankton9Classes.cfg  -Format C45 -DataFile NineClassPlankton.data -NumSplits 30  -SplitPercentage 66%



RandomSplits::RandomSplits ():
   PicesApplication (),
   dataFileName   (),
   format         (FeatureFileIOPices::Driver ()),
   numFolds       (10),
   numSplits      (30),
   restart        (false),
   splitFraction  (0.7f)
{
}


   RandomSplits::RandomSplits (RunLog&  log):
   PicesApplication (log),
   dataFileName   (),
   format         (FeatureFileIOPices::Driver ()),
   numFolds       (10),
   numSplits      (30),
   restart        (false),
   splitFraction  (0.7f)
{
}



RandomSplits::~RandomSplits ()
{
}



void  RandomSplits::InitalizeApplication (kkint32 argc,
                                          char**  argv
                                         )
{
  ConfigRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
	if  (Abort ())
	{
		DisplayCommandLineParameters ();
		return;
  }
}  /* InitalizeApplication */




void  RandomSplits::DisplayCommandLineParameters ()
{

  PicesApplication::DisplayCommandLineParameters ();
  cout << endl << endl
       << "RandomSplits   -c <ConfigFileName>  -DataFile <FileName>  -format <Data File Format>  -NumSplits <Defaults to 30>  -SplitPercentage <Defaults to 70%>"  << endl
       << endl
       << "   -DataFile        : Name a file that contains all the feature data." << endl
       << "   -Format          : " << FeatureFileIO::FileFormatsReadOptionsStr () << endl
       << "   -NumFolds        : Number of folds used to make Bias Matrix"        << endl
       << "   -NumSplits       : Number of Random Splits to generate."            << endl
       << "   -SplitPercentage : Percentage to be used for Training."             << endl
       << endl;
}



bool  RandomSplits::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                             const KKStr&  parmValue
                                            )
{
  bool  validParm = true;

  if  (parmSwitch.EqualIgnoreCase ("-DATAFILE")  ||
       parmSwitch.EqualIgnoreCase ("-DF")
      )
  {
    dataFileName = parmValue;
  }

  else if  
      (parmSwitch.EqualIgnoreCase ("-F")  ||
       parmSwitch.EqualIgnoreCase ("-FORMAT")
      )
  {
    format = FeatureFileIO::FileFormatFromStr (parmValue);
    if  (!format)
    {
      log.Level (-1) << endl << endl 
                     << "ProcessCmdLineParameter   ***ERROR***    No such format as[" << parmValue<< "]." << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if
      (parmSwitch.EqualIgnoreCase ("-NumFolds")   ||  
       parmSwitch.EqualIgnoreCase ("-NF")         ||  
       parmSwitch.EqualIgnoreCase ("-NumOfFolds") ||
       parmSwitch.EqualIgnoreCase ("-NOF")
      )
  {
    numFolds =  parmValue.ToInt ();
    if  ((numFolds != 0)  &&  (numFolds < 2))
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter   ***ERROR***    Invalid NumFolds[" << parmValue<< "]." << endl
                     << "                                         Must be '0' or greter than '1'." << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }

  else if  
      (parmSwitch.EqualIgnoreCase ("-NUMSPLITS")  ||
       parmSwitch.EqualIgnoreCase ("-NS")         ||
       parmSwitch.EqualIgnoreCase ("-SPLITS")
      )
  {
    numSplits =  parmValue.ToInt ();
  }


  else if
      (parmSwitch.EqualIgnoreCase ("-SPLITPERCENTAGE")  ||
       parmSwitch.EqualIgnoreCase ("-SP")
      )
  {
    splitFraction = parmValue.ToPercentage () / 100.0f;
    if  (splitFraction >= 1.0f)
    {
      log.Level (-1) << endl
        << "ProcessCmdLineParameter    ***ERROR***     Invalid Split Percentage[" << splitFraction << "]"  << endl
        << endl
        << "     If you want to enter as percentage include a '%' sign otherwise it is assumed to be a fraction." << endl
        << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("Restart"))
  {
    restart = true;
  }


  else
  {
    validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }

	return  validParm;
}  /* ProcessCmdLineParameter */





void  RandomSplits::Run ()
{
  bool  successful = false;
  RandomSplitJobManagerPtr  manager = new RandomSplitJobManager 
                                     (configFileName, 
                                      format, 
                                      dataFileName, 
                                      numSplits, 
                                      splitFraction,
                                      numFolds,
                                      log
                                     );
 
  manager->InitilizeJobManager (successful);
  if  (successful)
    manager->Run ();

  delete  manager;
  manager = NULL;
}




int  main (int     argc, 
           char**  argv
          )
{
  RunLog  log;

  InstrumentDataFileManager::InitializePush ();
  // Need to register the different types of Jobs.
  Job::RegisterConstructor ("RandomSplitJob",  RandomSplitJob::CreateEmptyInstance);
  
  RandomSplits  application (log);
  application.InitalizeApplication (argc, argv);
  if  (!application.Abort ())
  {
    application.Run ();
  }

  InstrumentDataFileManager::InitializePop ();

  return  0;
}
