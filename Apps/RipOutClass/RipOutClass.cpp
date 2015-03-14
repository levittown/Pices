#include  "FirstIncludes.h"

#include  <stdlib.h>
#include  <memory>

#include  <string>
#include  <iostream>
#include  <fstream>
#include  <vector>


#include  "MemoryDebug.h"

using namespace  std;


#include  "KKBaseTypes.h"
#include  "OSservices.h"
#include  "KKStr.h"
using namespace  KKB;


#include  "FeatureFileIO.h"
#include  "FeatureFileIOPices.h"
#include  "FeatureNumList.h"
#include  "FeatureVector.h"
#include  "FileDesc.h"
#include  "MLClass.h"
using namespace  MLL;

#include  "RipOutClass.h"


// -Src  C:\bigpine\Plankton\plankton\Apps\CrossValidation\NineClasses_TestData.data  -Dest C:\bigpine\Plankton\plankton\Apps\CrossValidation\Trich_and_Protist.data  -Class Trich  -Class Protist_all

// 2006-Jan-19,   Creeating Forest Cover data set as described in Torwards Scallable SVM, for bit reduction paper.
// K:\Plankton\Papers\BitReduction\DataSets\ForestCover
// -SrcFileName covtype.data -Format c45  -ClassName Lodgepole_Pine  -ClassName Spruce_Fir -DestFileName CovType_TwoClass.data

// 2006-02-09     Setting up for tuning web dada set.
// K:\Plankton\Papers\BitReduction\DataSets\WEB_Data
// -SrcFileName anonymous-msweb_Data-C45.data -Format c45  
// C:\Users\kkramer\SipperProject\Papers\BinaryFeatureSelection\Experiments\NineClasses_TrainTest_Acc_8Classes
// -SrcFileName NineClasses_TrainTest_c45.data -Format c45 -ClassName Marine_Snow_Dark  -DestFileName  EightClasses_TrainTest_c45.data




RipOutClass::RipOutClass ():
  PicesApplication (),
  cancelFlag      (false),
  classesToRipOut (),
  destFileName    (),
  destImages      (NULL),
  mlClasses       (),
  inFileFormat    (FeatureFileIOPices::Driver ()),
  numOfFolds      (10),
  outFileFormat   (FeatureFileIOPices::Driver ()),
  srcFileName     (),
  srcImages       (NULL),
  stratify        (false),
  successful      (true)
{
}




/******************************************************************************
 * Destructor                                                                 *
 ******************************************************************************/
RipOutClass::~RipOutClass ()
{
  delete  srcImages;
  delete  destImages;
}


void  RipOutClass::InitalizeApplication (kkint32 argc,
                                         char**  argv
                                        )
{
  PicesApplication::InitalizeApplication (argc, argv);
	if  (Abort ())
	{
		DisplayCommandLineParameters ();
		return;
	}

  if  (classesToRipOut.QueueSize () < 1)
  {
    log.Level (-1) << endl
                   << endl
                   << "You must specify at least one class name '-ClassName xxxxx'." << endl
                   << endl;
    Abort (true);
    return;
  }

  if  (srcFileName.Empty ())
  {
    log.Level (-1) << endl
                   << endl
                   << "You must specify a Source File" << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  bool successful  = true;
  bool changesMade = false;

  srcImages = inFileFormat->LoadFeatureFile (srcFileName, 
                                             mlClasses,
                                             -1,  // No limit to number to load.
                                             cancelFlag,
                                             successful,
                                             changesMade,
                                             log
                                            );

  if  (!srcImages)
  {
    log.Level (-1) << endl
                   << endl
                   << "Error Loading SrcFile[" << srcFileName << "]" << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  fileDesc = srcImages->FileDesc ();

  if  (destFileName.Empty ())
  {
    destFileName = osRemoveExtension (srcFileName);

    MLClassConstList::iterator idx;
    for  (idx = classesToRipOut.begin ();  idx != classesToRipOut.end ();  idx++)
      destFileName << "_" << (*idx)->Name ();

    destFileName << ".data";
  }

  if  (outFileFormat->CanWrite ())
  {
    destImages = outFileFormat->LoadFeatureFile (destFileName, 
                                                 mlClasses,
                                                 -1,  // No limit to number to load.
                                                 cancelFlag,
                                                 successful,
                                                 changesMade,
                                                 log
                                                );
    if  (destImages)
    {
      // There is an existing destination file.  We will be appending to 
      // this file.

      if  (destImages->FileDesc () != fileDesc)
      {
        // The src file and dest file have different file descriptions.  This is a NO NO

        log.Level (-1) << endl
                       << endl
                       << "SrcFile[" << srcFileName << "] and DestFile[" << destFileName <<"] have different file descriptions." << endl
                       << endl;
        Abort (true);
        return;
      }
    }
  }

  if  (!destImages)
  {
    destImages = new FeatureVectorList (fileDesc, 
                                        true,                    // Will own imageFeatures contained in this list
                                        log,     
                                        srcImages->QueueSize ()  // Will initialize allocation to size of srcImages.
                                       );
  }


  PicesApplication::PrintStandardHeaderInfo (cout);
  cout  << endl;
  cout  << "------------------------------------------------------------------------" << endl;
  cout  << "SrcFileName     [" << srcFileName                               << "]."   << endl;
  cout  << "DestFileName    [" << destFileName                              << "]."   << endl;
  cout  << "Classes         [" << classesToRipOut.ToCommaDelimitedStr ()    << "]."   << endl;
  cout  << "In File Format  [" << inFileFormat->DriverName ()               << "]."   << endl;
  cout  << "Out File Format [" << outFileFormat->DriverName ()              << "]."   << endl;
  cout  << "Stratify        [" << (stratify ? "Yes" : "No")                 << "]."   << endl;
  if  (stratify)
    cout <<  "NumOfFolds      [" << numOfFolds                              << "]."   << endl;
  cout  << "------------------------------------------------------------------------" << endl;
  cout  << endl;


}  /* InitalizeApplication */






/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  RipOutClass::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                            const KKStr&  parmValue
                                           )
{
  bool  parmValid = true;

  if  (parmSwitch.EqualIgnoreCase ("-CN")         ||
       parmSwitch.EqualIgnoreCase ("-CLASSNAME")  ||
       parmSwitch.EqualIgnoreCase ("-CLASS")
      )
  {
    if  (parmValue.Empty ())
    {
      log.Level (-1) << endl
                     << endl
                     << "-ClassName requires a name of a class."  << endl
                     << endl
                     << endl;
      Abort (true);
      parmValid = false;
    }
    else
    {
      classesToRipOut.PushOnBack (mlClasses.GetMLClassPtr (parmValue));
    }
  }

  else if  
      (parmSwitch.EqualIgnoreCase ("-DEST")         ||
       parmSwitch.EqualIgnoreCase ("-D")            ||
       parmSwitch.EqualIgnoreCase ("-DESTFILE")     ||  
       parmSwitch.EqualIgnoreCase ("-DF")           ||  
       parmSwitch.EqualIgnoreCase ("-D")            ||
       parmSwitch.EqualIgnoreCase ("-DESTFILENAME") ||  
       parmSwitch.EqualIgnoreCase ("-DFN")
      )
    destFileName = parmValue;

  else if  ((parmSwitch == "-F")  ||  (parmSwitch == "-FORMAT"))
  {
    inFileFormat = FeatureFileIO::FileFormatFromStr (parmValue, 
                                                     true,      // Valid Read Format
                                                     true       // Valid Write Format
                                                    );
    if  (inFileFormat == NULL)
    {
      log.Level (-1) << endl
        << "Invalid File Format Specified[" << parmValue << endl
        << endl
        << "Valid Formats are <" << FeatureFileIO::FileFormatsReadAndWriteOptionsStr () << ">" << endl
        << endl;
      Abort (true);
      parmValid = false;
    }
    outFileFormat = inFileFormat;
  }


  else if
      (parmSwitch.EqualIgnoreCase ("-DESTFORMAT")     ||  
       parmSwitch.EqualIgnoreCase ("-DF")             ||  
       parmSwitch.EqualIgnoreCase ("-DESTFILEFORMAT") ||
       parmSwitch.EqualIgnoreCase ("-DFF") 
      )
  {
    outFileFormat = FeatureFileIO::FileFormatFromStr (parmValue, 
                                                      false,      // Don't care if valid read format
                                                      true        // must be valid write format
                                                     );
    if  (outFileFormat == NULL)
    {
      log.Level (-1) << endl
        << "Invalid Write File Format Specified[" << parmValue << endl
        << endl
        << "Valid Formats are <" << FeatureFileIO::FileFormatsWrittenOptionsStr () << ">" << endl
        << endl;
      Abort (true);
      parmValid = false;
    }
  }


  else if  
      (parmSwitch.EqualIgnoreCase ("-SRC")            ||  
       parmSwitch.EqualIgnoreCase ("-SOURCFILE")      ||  
       parmSwitch.EqualIgnoreCase ("-SRCFILE")        ||  
       parmSwitch.EqualIgnoreCase ("-SF")             ||  
       parmSwitch.EqualIgnoreCase ("-SOURCFILENAME")  ||  
       parmSwitch.EqualIgnoreCase ("-SRCFILENAME")    ||  
       parmSwitch.EqualIgnoreCase ("-SFN")            ||  
       parmSwitch.EqualIgnoreCase ("-S")
      )
    srcFileName = parmValue;


  else if
      (parmSwitch.EqualIgnoreCase ("-SRCFORMAT")     ||  
       parmSwitch.EqualIgnoreCase ("-SF")            ||  
       parmSwitch.EqualIgnoreCase ("-SRCFILEFORMAT") ||  
       parmSwitch.EqualIgnoreCase ("-SFF")          
      )
  {
    inFileFormat = FeatureFileIO::FileFormatFromStr (parmValue, 
                                                     true,      // Valid Read Format
                                                     false      // Don't care if valid write format
                                                    );
    if  (inFileFormat == NULL)
    {
      log.Level (-1) << endl
        << "Invalid Input File Format Specified[" << parmValue << endl
        << endl
        << "Valid Formats are <" << FeatureFileIO::FileFormatsReadOptionsStr () << ">" << endl
        << endl;
      Abort (true);
      parmValid = false;
    }
  }  

  else if  (parmSwitch.EqualIgnoreCase ("-STRATIFY"))
  {
    stratify = true;
    if  (!parmValue.Empty ())
    {
      numOfFolds = atoi (parmValue.Str ());
      if  (numOfFolds < 1)
      {
        log.Level (-1) << endl
                       << endl
                       << "Stratify[" << numOfFolds << "] is number of folds and must be greater than 0." << endl
                       << endl;
        Abort (true);
        parmValid = false;
      }
    }
  }

  else
  {
    parmValid = PicesApplication::ProcessCmdLineParameter (parmSwitch, parmValue);
  }
  
  return  !Abort ();
}  /* ProcessCmdLineParameter */



/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   RipOutClass::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << "RipOutClass  "                                                                            << endl
       << "       -Src        <Source File Name>"                                                    << endl
       << "       -Dest       <Destination File Name>  Optional"                                     << endl
       << "       -Format     <" << FeatureFileIO::FileFormatsReadAndWriteOptionsStr () << ">   Defaults to 'RAW'"  << endl
       << "       -SrcFormat  <" << FeatureFileIO::FileFormatsReadOptionsStr ()         << ">   Defaults to 'RAW'"  << endl
       << "       -DestFormat  <" << FeatureFileIO::FileFormatsWrittenOptionsStr ()     << ">   Defaults to 'RAW'"  << endl
       << "       -ClassName  <Class Name>           Must have at least once"                        << endl
       << "       -Stratify   <Num Of Folds>"                                                        << endl
       << endl
       << endl
       << "ex:  RipOutClass  -Src Test.data  -ClassName Copepod"                                     << endl
       << endl
       << "             Will append to file 'Test_Copepod.data' all entries in"                      << endl
       << "             'Test.data' that belong to class 'Copepod'"                                  << endl
       << "             Both files must be in the file format 'RAW'."                                << endl
       << endl
       << endl
       << "ex: RipOutClass  -Src Test.data  -Dest TestOut.data -ClassName Larvacean  -SrcFormat C45  -OutFormat Sparse"   << endl
       << endl
       << "             Will append to file 'TestOut.data' all entries in 'Test.Data'"               << endl
       << "             that belong to class 'Larvacean'.  'Test.Data' is in C45 format"             << endl
       << "             while 'TestOut.data' is in Sparse format."                                   << endl
       << endl
       << endl
       << "ex: RipOutClass  -Src Test.data  -ClassName Larvacean  -ClassName Protist"                << endl
       << endl
       << "             Will append to file 'Test_Larvacean_Protist.data' all entries"               << endl
       << "             in 'Test.Data' that belong to the classes 'Larvacan' and"                    << endl
       << "             'Protist'."                                                                  << endl
       << endl;
}
  



void  RipOutClass::ExtractSpecifiedClass ()
{
  // The constrctor would have loaded both the source and the distination files.
  // so most of the work is alreadt done.

  FeatureVectorListPtr  resultList = new FeatureVectorList (*destImages, false);

  MLClassConstList::iterator idx;
  for  (idx = classesToRipOut.begin ();  idx != classesToRipOut.end ();  idx++)
  {
    MLClassConstPtr  mlClass = *idx;
    FeatureVectorListPtr  sepcifiedClassData = srcImages->ExtractImagesForAGivenClass (mlClass);
    cout << "Class [" << mlClass->Name () << "]  Entries Found[" << sepcifiedClassData->QueueSize () << "]" << endl;
    resultList->AddQueue (*sepcifiedClassData);
    delete  sepcifiedClassData;
  }


  if  (stratify)
  {
    FeatureVectorListPtr  stratifiedList 
             = resultList->StratifyAmoungstClasses (&classesToRipOut, -1, numOfFolds);
    delete  resultList;
    resultList = stratifiedList;
  }


  KKB::uint  numExamplesWritten = 0;
  outFileFormat->SaveFeatureFile (destFileName, 
                                  resultList->AllFeatures (),
                                  *resultList,
                                  numExamplesWritten,
                                  cancelFlag,
                                  successful,
                                  log
                                 );

  delete  resultList;
}  /* ExtractSpecifiedClass */



//  A one off function to remove one class from the NineClass Plankton dataset used in the Binary Feature Selection paper.
void  MakeEightClassDataSet (const KKStr&  srcFileName,
                             const KKStr&  destFileName
                            )
{
  RunLog  log;
  KKB::uint  numExamplesWritten = 0;
  bool  cancelFlag  = false;
  bool  successful  = false;
  bool  changesMade = false;
  MLClassConstListPtr  classes  = new MLClassConstList ();

  FeatureFileIOPtr  driver = FeatureFileIO::FileFormatFromStr ("C45");
  if  (!driver)
  {
    cerr << endl << endl << "MakeEightClassDataSet  ***ERROR***  Invalid drivetr." << endl << endl;
    exit (0);
  }
    
  FeatureVectorListPtr  srcData =     
    driver->LoadFeatureFile (srcFileName, *classes, -1, cancelFlag, successful, changesMade, log);

  if  (!srcData)
  {
    cerr << endl << endl << "MakeEightClassDataSet  ***ERROR***  Error Loading File." << endl << endl;
    exit (0);
  }

  MLClassConstPtr  excludeClass = MLClass::CreateNewMLClass ("Marine_Snow_Dark");

  FeatureVectorListPtr  destData = new FeatureVectorList (srcData->FileDesc (), false, log, -1);

  FeatureVectorList::iterator  idx;
  for  (idx = srcData->begin ();  idx != srcData->end ();  idx++)
  {
    FeatureVectorPtr  fv = *idx;

    if  (fv->MLClass () != excludeClass)
    {
      destData->PushOnBack (fv);
    }
  }

  
  FeatureVectorListPtr  stratifiedData = destData->StratifyAmoungstClasses (10);

  driver->SaveFeatureFile (destFileName, 
                           stratifiedData->AllFeatures (), 
                           *stratifiedData, 
                           numExamplesWritten, 
                           cancelFlag, 
                           successful, 
                           log
                          );

  delete stratifiedData;  stratifiedData = NULL;
  delete destData;        destData       = NULL;
  delete srcData;         srcData        = NULL;

}  /* MakeEightClassDataSet */



//  A one off function to remove one class from the NineClass Plankton dataset used in the Binary Feature Selection paper.
// Kurt Kramer   2010-June-03
void  MakeEightClassDataSet ()
{
  KKStr  srcFileName  = "C:\\Users\\kkramer\\SipperProject\\Papers\\BinaryFeatureSelection\\Experiments\\NineClasses_TrainTest_Acc_8Classes\\NineClasses_TrainTest_c45.data";
  KKStr  destFileName = "C:\\Users\\kkramer\\SipperProject\\Papers\\BinaryFeatureSelection\\Experiments\\NineClasses_TrainTest_Acc_8Classes\\NineClasses_Train_8Classes.data";
  MakeEightClassDataSet (srcFileName, destFileName);

  srcFileName  = "C:\\Users\\kkramer\\SipperProject\\Papers\\BinaryFeatureSelection\\Experiments\\NineClasses_TrainTest_Acc_8Classes\\NineClasses_ValidationData_c45.data";
  destFileName = "C:\\Users\\kkramer\\SipperProject\\Papers\\BinaryFeatureSelection\\Experiments\\NineClasses_TrainTest_Acc_8Classes\\NineClasses_Validation_8Classes.data";
  MakeEightClassDataSet (srcFileName, destFileName);
}


int  main (int     argc,
           char**  argv
          )
{
  RipOutClass  ripOutClass;
  ripOutClass.InitalizeApplication (argc, argv);
  if  (!ripOutClass.Abort ())
    ripOutClass.ExtractSpecifiedClass ();
}  /* main */



