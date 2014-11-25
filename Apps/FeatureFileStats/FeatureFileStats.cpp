#include "FirstIncludes.h"

#include <stdlib.h>
#include <memory>
#include <math.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "MemoryDebug.h"
using namespace  std;



#include "BasicTypes.h"
#include "OSservices.h"
#include "Str.h"
using namespace  KKU;


#include "FeatureFileIO.h"
#include "FeatureFileIOPices.h"
#include "FeatureVector.h"
#include "MLClass.h"
using namespace  MLL;


#include "FeatureFileStats.h"



//  -FeatureFile  F:\Users\kkramer\GradSchool\Plankton\Papers\BitReduction\DataSets\Adult\adult.data -Format c45 - Report adult_stats.txt

// K:\Plankton\Papers\BitReduction\DataSets\ForestCover
// -FeatureFile covtype.data  -Format c45  -report  covtype_stats.txt

// K:\v1\Plankton\BeamSearches\Feb-19-2004_29Features
// -FeatureFile TEST_OCT_24_80p.data  -Format raw -report TEST_OCT_24_80p_stats.txt


// -FeatureFile  C:\TrainingApp\SipperFiles\hrs020501_man_small.data  
// -FeatureFile C:\users\LI\Data\colon.train.scale -Format Sparse -Report C:\users\LI\Data\colon.train.scale.stats

//C:\Users\kkramer\GradSchool\Papers\BinaryFeatureSelection\Experiments\Yuhua
// -FeatureFile  -Format Sparse -FeatureFile  Yuhua_Train.data   -report Yuhua_Train.Stats

// -FeatureFile C:\Users\kkramer\SipperProject\Papers\BinaryFeatureSelection\Experiments\ETP08\etp_08_Train.data  -Format Pices

FeatureFileStats::FeatureFileStats () :
  PicesApplication (),

  data           (NULL),
  cancelFlag     (false),
  mlClasses      (),
  report         (NULL),
  reportFile     (NULL),
  srcFileName    (),
  srcFileFormat  (FeatureFileIOPices::Driver ())
{
}  /* FeatureFileStats */






/******************************************************************************
 * Destructor
 ******************************************************************************/
FeatureFileStats::~FeatureFileStats ()
{
  if  (reportFile)
  {
     reportFile->close ();
	 delete  reportFile;
  }

  delete  data;
}




void  FeatureFileStats::InitalizeApplication (int32   argc,
                                              char**  argv
                                             )
{
  PicesApplication::InitalizeApplication (argc, argv);
	if  (Abort ())
	{
		DisplayCommandLineParameters ();
		return;
	}

  if  (srcFileName.Empty ())
  {
    Abort (true);
    log.Level (-1) << endl
                   << endl
                   << "*** ERROR ***  There was no Feature File specified." << endl
                   << endl;

    DisplayCommandLineParameters ();

    osWaitForEnter ();
    return;
  }

  if  (reportFileName.Empty ())
	{
		reportFile = NULL;
		report = &cout;
	}
	else
	{
		reportFile = new ofstream (reportFileName.Str ());
		report = reportFile;
	}
  

  bool  successful;
  bool  changesMade = false;

  data = srcFileFormat->LoadFeatureFile (srcFileName, 
                                         mlClasses,
                                         -1,          // Max number to load,  -1 means load all.
                                         cancelFlag,
                                         successful,
                                         changesMade,
                                         log
                                        );

  if  (!data  ||  !successful)
  {
    log.Level (-1) << endl
                   << endl
                   << "Error loading source file[" << srcFileName << "]" << endl
                   << endl;
    Abort (true);
    return;
  }

  fileDesc = data->FileDesc ();

  PrintStandardHeaderInfo (*report);

  *report << endl;
  *report << "-----------------------------------------------------------"  << endl;
  *report << "Source File Name"   << "\t" << srcFileName                    << endl;
  *report << "Source File Format" << "\t" << srcFileFormat->DriverName ()   << endl;
  *report << "Report File Name"   << "\t" << reportFileName                 << endl;
  *report << "-----------------------------------------------------------"  << endl;
  *report << endl;
}  /* InitalizeApplication */





/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  FeatureFileStats::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                 const KKStr&  parmValue
                                                )
{
  bool  validParm = true;
  if  (parmSwitch.EqualIgnoreCase ("-SrcFileFormat") ||
       parmSwitch.EqualIgnoreCase ("-SFF")           ||
       parmSwitch.EqualIgnoreCase ("-FORMAT")        ||
       parmSwitch.EqualIgnoreCase ("-F")             ||
       parmSwitch.EqualIgnoreCase ("-INPUTFORMAT")   ||
       parmSwitch.EqualIgnoreCase ("-IF")            ||
       parmSwitch.EqualIgnoreCase ("-INPUTFORMAT")   ||
       parmSwitch.EqualIgnoreCase ("-IFF")         
      )
  {
    srcFileFormat = FeatureFileIO::FileFormatFromStr (parmValue, 
                                                      true,      // Valid Read Format
                                                      false      // Don't care if valid write format
                                                     );
    if  (srcFileFormat == NULL)
    {
      log.Level (-1) << endl
                     << "Invalid Source File Format Specified[" << parmValue << endl
                     << endl
                     << "Valid Formats are <" << FeatureFileIO::FileFormatsReadOptionsStr () << ">" << endl
                     << endl;
      Abort (true);
      validParm = false;
    }
  }  


  else if  (parmSwitch.EqualIgnoreCase ("-R")  ||  parmSwitch.EqualIgnoreCase ("-REPORT"))
		reportFileName = parmValue;

 
  else if  (parmSwitch.EqualIgnoreCase ("-FEATUREFILE")   ||
            parmSwitch.EqualIgnoreCase ("-FF")            ||
            parmSwitch.EqualIgnoreCase ("-SRC")           ||
            parmSwitch.EqualIgnoreCase ("-S")             ||
            parmSwitch.EqualIgnoreCase ("-SOURCFILE")     ||
            parmSwitch.EqualIgnoreCase ("-SF")            ||
            parmSwitch.EqualIgnoreCase ("-SOURCFILENAME") ||
            parmSwitch.EqualIgnoreCase ("-SFN")
           )
    srcFileName = parmValue;

  else
  {
    validParm = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }


	return  validParm;
}  /* ProcessCmdLineParameter */






/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   FeatureFileStats::DisplayCommandLineParameters ()
{
  cout << "FeatureFileStats"                                                                        << endl
       << "       -FeatureFile    <Feature File Name>"                                              << endl
       << "       -Format         <"  << FeatureFileIO::FileFormatsReadOptionsStr ()   << ">   Defaults to 'Pices'"  << endl
       << "       -Report         <Report file name>   Defaults to 'cout'"                          << endl
       << endl
       << endl
       << "ex:  FeatureFileStats  -FeatureFile Test.data  -Format c45  -Report  Test_stats.txt"     << endl 
       << endl
       << "             Will retrieve statistics of 'Test.data' and 'Test.names' and write results" << endl
       << "             to 'Test_stats.txt'."                                                       << endl
       << endl
       << endl;
}




void   FeatureFileStats::ReportStats ()
{
  *report << endl << endl << endl;

  MLClassConstListPtr  mlClasses = data->ExtractMLClassConstList ();

  *report << "Feature File Name  [" << srcFileName                << "]" << endl;
  *report << "Number of Classes  [" << mlClasses->QueueSize () << "]" << endl;
  *report << "Number of Features [" << fileDesc->NumOfFields ()   << "]" << endl;

  *report << endl;

  *report << "Class Statistics:"  << endl;
  data->PrintClassStatistics (*report);

  *report << endl << endl;

  *report << "Feature Statistics:"  << endl;
  data->PrintFeatureStatisticsByClass (*report);

  *report << endl << endl;
}  /* ConvertData */




int  main (int     argc,
           char**  argv
          )
{
  FeatureFileStats fileConverter;
  fileConverter.InitalizeApplication (argc, argv);
  if  (!fileConverter.Abort ())
    fileConverter.ReportStats ();
}
