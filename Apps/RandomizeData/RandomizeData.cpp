#include  "FirstIncludes.h"

//********************************************************************************
//* Copyright (c) 2002-2004 Universdity of South Florida                         *
//*                                                                              *
//* This program was developed under a grant from                                *
//*                                                                              *
//*  This program was originally designed to sort feature data into sorted order *
//*  since then it has been modified to support several other functions.         *
//*                                                                              *
//*     Splitting data into two files by percentage by class.                    *
//*     Normalizing Data                                                         *
//*     Converting between Different Formats                                     *
//*==============================================================================*
//*  History                                                                     *
//*                                                                              *
//*  Programmer       Date     Description                                       *
//*  ------------  ----------  --------------------------------------------------*
//*  Kurt Kramer   2002-06-??  Original development, sorts sparse or raw format  *
//*                            data,                                             *
//*                                                                              *
//*  Kurt Kramer   2002-07-??  Split files by percentage by class.               *
//*                                                                              *
//*  Kurt Kramer   2003-??-??  Convert data between different Formats            *
//*                                                                              *
//*  Kurt Kramer   2004-??-??  Allow for specifying specific features on         *
//*                            the output files.                                 *
//*                                                                              *
//*  Kurt Kramer   2004-08-01  Added the comment section.                        *
//*                                                                              *
//*  Kurt Kramer   2005-01-20  Added a one-off Function that will build Data     *
//*                            for Active Learning Experiment that will         *
//*                            require Diff Prior for diff classes.              *
//*                            CreateDiffPriorDataSet ()                         *
//*                                                                              *
//*  Kurt Kramer   2005-10-22  Added TrimedClassed Action.                       *
//*                                                                              *
//*                                                                              *
//********************************************************************************
//* This program is distributed in the hope that it will be useful,
//* but WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//* GNU General Public License for more details.
//*
//* You should have received a copy of the GNU General Public License
//* along with this program; if not, write to the Free Software
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//*
//******************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <vector>
#include  "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "CmdLineExpander.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;

#include "InstrumentData.h"

#include "DuplicateImages.h"
#include "FeatureFileIO.h"
#include "FeatureFileIOArff.h"
#include "FeatureFileIOC45.h"
#include "FeatureFileIOPices.h"
#include "FeatureFileIOSparse.h"
#include "FeatureNumList.h"
#include "MLClass.h"
#include "FeatureVector.h"
#include "NormalizationParms.h"
#include "TrainingConfiguration2.h"
using namespace KKMLL;


// -f K:\v1\Plankton\TempBeamSearch\2004_March_02\2004_03_02.data  -A TEST_VALIDATE  -I RAW   -O RAW  -L 1500  -P 66.6666

//  -f "K:\Classes\DIA\TermProject\2004_03_02.data"  -o Roberts  -a Convert -i RAW

//  -f "C:\users\kkramer\Grad School\Plankton\ActiveLearning\Results\2003-12-05_200-IPC_05NewImagesPerRetraining\ActiveLearning_MasterTestImages.data"  -o Phore  -a Convert -i RAW  -Features 1,8,9,18,20,22,24,28,29,32,34,39,40,41,42,43,46

//  -f ExtractedImagess.data  -a Convert  -i Raw  -o Phore  -Features  9,19,21,23,32,35,39,41,42,43,44,45,46,48,49,50,51,52,53,54


// Command line for Phores request   2004-09-20
// -f K:\v1\Plankton\ExtractedImages\ExtractedImages.data -i Raw  -action Convert  -o Phore  -features 1,8,9,18,20,22,24,28,29,32,34,39,40,41,42,43,46,48,49,50,51,52,53,54,55,56

// Command line for Converting Active Learning Data to Sparse Format,  at Tongs request,  2004-09-20
//     -f ActiveLearning_MasterTestImages.data   -i raw  -o sparse -action convert 
// ActiveLearning_ValidationImages.data

//  2004-09-30
// Create test and Validation files for Thesis 9 class Test Data
//  -f C:\users\kkramer\GradSchool\Thesis\TestSet9Classes\NineClasses.data  -i raw  -a TEST_VALIDATE  -p 80


// 2004-10-04
// -f K:\Thesis\TestSet9Classes\NineClasses.data  -i raw  -a TEST_VALIDATE  -p 66.666

//*******  Parameters  ************************************************
//*                                                                   *
//*  RandomizeData  -f <Root_Data_FileName>                           *
//*                 -features  <List of Features for Output>          *
//*                 -i <RAW | SPARSE>                                 *
//*                 -a <SPLIT | SHUFFLE | TEST_VALIDATE | CONVERT     *
//*                 -p <% to split file by>                           *
//*                 -l <Num_Of_Rows to Keep by Class>                 *
//*                 -n /* Normalize Data */                           *
//*                 -o <RAW | SPARSE | C45 | PHORE | ROBERTS>         *
//*********************************************************************


// 2005-01-14    
//   test the new Arrf format
//  C:\users\kkramer\GradSchool\Thesis\TestSet9Classes\300_ImagesPerClass\UsingOld5ContourFeatures
//  -a  Convert   -i Raw  -o Arff  -f  NineClasses_TestData.data
//  -a  Convert   -i Raw  -o Arff  -f  NineClasses_ValidationData.data


// 2005-01-24
// Create data Set for Grid Search
// -a SHUFFLE -f K:\v1\Plankton\ActiveLearning\Results\2004_Sep_Expirements_Diff_Prior\ActiveLearning_MasterTest_Diff_Prior.data -lo 200

// -a TEST_VALIDATE -f C:\users\kkramer\GradSchool\Thesis\TestSet9Classes\NineClasses.data  -i raw -o arff

// 2005-03-17
// -a Shuffle -f c:\Temp\OnePercentModelValidationData.data  -LopOff 500

// -a SHUFFLE  -f K:\v1\Plankton\BeamSearches\OnePercentModel\OnePercentModelTestData.data  -l 150

// -a TEST_VALIDATE -f c:\Temp\OnePercentModelTestData.data  -i raw -p 50.0

// K:\v1\Adult\Kevin
// -File new_adult.data2  -Norm

// K:\v1\Thesis\TestSet9Classes\DataFor_5_2-Fold-CV
// -A SHUFFLE -f NineClasses_TestData.data 

// 2005-July-25
// -a TEST_VALIDATE -f C:\bigpine\Plankton\plankton\Apps\RandomSearch\survival.data  -i c45 -p 80.0

// 2005-08-01
// Create spase file, and normalize data.
// -a Convert  -f NineClasses_TestData  -i Raw  -o Sparse -N

// 2005-08-21
// -A SHUFFLE -f C:\bigpine\Plankton\plankton\Apps\BeamSearchSVM\covtype.data -LopOff 800 -i C45Format  -o C45Format

// 2005-10-22
// C:\TrainingApp\TrainingImages
// -File AllTrainingImages.data  -LopOff 300  -Action SHUFFLE -Folds 5


// 2005-12-09
// C:\TrainingApp\TrainingImages
// -File C:\TrainingApp\TrainingImages\TrainingImages.data  -LopOff 500  -Action SHUFFLE -Folds 10


// 2006-01-22
// K:\Plankton\Papers\BitReduction\DataSets\Banana
// -File banana.all.txt -if Sparse -Action split  -p 40

// 2006-02-02
// -File banana.all.txt -if Sparse -Action split  -p 20 

// 2006-02-09     Setting up for tuning web dada set.
// K:\Plankton\Papers\BitReduction\DataSets\WEB_Data
// -File anonymous-msweb_Data-C45.data -if c45  -Action split -p 30

// K:\Plankton\Papers\BitReduction\DataSets\WaveForm
// -file waveform-+noise.data  -IF UCI -Action split -p 80 -of c45

// K:\Plankton\Papers\BitReduction\DataSets\SatImage\Tunning
// -file SatAll.data -if UCI -of c45 -Action Shuffle 


// 2006-Feb-15      Chopping Adult down to size for tunning purposes.
// K:\Plankton\Papers\BitReduction\DataSets\Adult\Tunning
// -file Adult.data -if C45 -of c45 -Action Split -p 30 -Folds 10


// 2006-02-17
// K:\Plankton\Papers\BitReduction\DataSets\ForestCover
//  -File CovType_TwoClass.data  -format c45 -action SPLIT  -p 10 -Folds 10

// K:\Plankton\Papers\BitReduction\DataSets\Page
// -file ad.data  -IF c45 -Action split -p 80 -of c45

// K:\Plankton\Papers\BitReduction\DataSets\Letter
// -file letter-sparse.data  -IF sparse  -Action split -p 80 -of c45

//  K:\Plankton\Papers\BitReduction\DataSets\Phoneme
// -file phoneme.data  -if c45  -action split -p 80 -folds 10

// K:\Plankton\Papers\BitReduction\DataSets\ForestCover
// -file CovType_TwoClass.all  -if c45  -action split -p 5 -folds 10

// C:\users\kkramer\GradSchool\Plankton\Papers\BitReduction\DataSets\Page
// -file page-blocks.data  -if uci  -action split -p 5 -folds 10  -of c45

// 2006-02-21     Setting up for tuning web dada set.
// K:\Plankton\Papers\BitReduction\DataSets\WEB_Data
// -File anonymous-msweb_All-C45.data -if c45  -Action split -p 5


// 2006-03-15
// Y:\FeatureSelection\2006-03-14
// -DataFile ActiveLearning_ValidationImages.data  -Action shuffle -folds 10

// K:\v1\Plankton\ActiveLearning\Results\2004_Sep_Expirements
// -DataFile ActiveLearning_MasterTestImages.data -Action TEST_VALIDATE -P 86.5591398 -Folds 10


// K:\Plankton\Papers\FastFeatureSelAlgorithm
// -DataFile ActiveLearning_ValidationImages.data -Action SHUFFLE -Folds 10
// -DataFile FiveClassPaper_Main.data -Action SHUFFLE -Folds 10

// C:\bigpine\Plankton\Papers\BitReduction\DataSets\Shuttle\Tuning
// -DataFile  Shuttle_Test_c45.data -Action Shuffle  -LO 2000 -if  c45


// C:\bigpine\Plankton\Papers\BitReduction\DataSets\Letter\Tuning
// -DataFile letter-c45_80p.data -Action Shuffle -LO 300  -if c45


// K:\v1\Thesis\TestSet9Classes\1000_ImagesPerClass\ActiveLearning
// -DataFile  NineClasses_shuffled.data  -Action Shuffle -folds 10

// <<<<<<< RandomizeData.cpp
// C:\Documents and Settings\djgarcia\Desktop\plankton\Apps\RandomizeData\Debug
// -DataFile FiveClassPaper_Main.data -action TEST_VALIDATE -p 23.696682  -folds 10

//=======
// C:\TrainingApp\TrainingImages2
// -DataFile
//>>>>>>> 1.43

// C:\bigpine\LI\Data
// -DataFile colon.train.scale -action TEST_VALIDATE -p 20  -folds 10 -if Sparse

// K:\Plankton\Papers\BitReduction\DataSets\KDDCup
// -DataFile  kddcup.data -action TEST_VALIDATE -p 80  -if c45 -folds 1


// C:\temp\FeatureSearch_BinaryClasses
// -DataFile  D:\TrainingApp\etpC_training\etpC_training.data  -action  SHUFFLE  -Folds 10

// C:\bigpine\Plankton\Papers\BinaryFeatureSelection\Experiments\Plankton9Classes
// -DataFile  NineClasses_TestData_c45.data -if c45 -action TEST_VALIDATE  -p 50.0  -folds 10

// C:\bigpine\Plankton\Papers\BinaryFeatureSelection\Experiments\Letter\AllClasses
// -DataFile letter-All_c45.data  -if c45  -action TEST_VALIDATE  -folds 10  -p 33.3333

// -DataFile letter-TrainAndTest.data -if c45  -action TEST_VALIDATE  -folds 10  -p 75.0000

// C:\Users\kkramer\Plankton\BinaryFeaureSelectionExperiments
// -DataFile etp_hierarchy.data  -action TEST_VALIDATE  -folds 10  -p 70.0000
// D:\TrainingApp\SennaisTrainingModel
// -DataFile SennaisTrainingModel.data  -action TEST_VALIDATE  -folds 10   -p 70.0000

// D:\Users\kkramer\GradSchool\Plankton\Papers\BinaryFeatureSelection\ETP-C
// -DataFile  ETP-c.data -action TEST_VALIDATE  -folds 10   -p 66.6666

// -DataFile D:\Users\kkramer\GradSchool\Plankton\Papers\BinaryFeatureSelection\Experiments\Vic\svm_d7_127_30.data  -format Sparse  -action SHUFFLE  -folds 10

// C:\Users\kkramer\Plankton\Papers\BinaryFeatureSelection\Experiments\WFS
// -DataFile Wfs.data  -TEST_VALIDATE -p 60


// D:\Users\kkramer\GradSchool\Plankton\Papers\BinaryFeatureSelection\Experiments\Vic
// -DataFile svm_d7_all_1.data -format Sparse  -Action TEST_VALIDATE -p 50  -folds 10  

// -DataFile Vic_TestValidation.data  -format Sparse  -Action TEST_VALIDATE -p 50  -folds 10  


// -format C45  -folds 10  


// -DataFile D:\Users\kkramer\GradSchool\SipperProject\Papers\BinaryFeatureSelection\Experiments\ForestCover_1500_Acc\CovType-TrainTest.data  -format c45  -Action SHUFFLE -folds 10


// C:\Users\kkramer\SipperProject\Papers\BinaryFeatureSelection\Experiments\WFS2
// -DataFile -format Pices  Wfs_All.data  -Action TrainAndTest  -p 20



// C:\Users\kkramer\SipperProject\Papers\BinaryFeatureSelection\Experiments\Yuhua
// -format Sparse  -DataFile Yuhua_Train.data  -Action Shuffle  -folds 20  


// D:\Users\kkramer\GradSchool\SipperProject\Papers\BinaryFeatureSelection\Experiments\Baishali
// -DataFile neurons.data  -format Sparse  -Action TrainAndTest -p 33 -folds 10 


// 05/16/2011 -DataFile I:\Pices\TrainingLibraries\36Class34_35_37_MoreThan20_100percent.data  -Action SHUFFLE -folds 10 

// 05/23/2011 -DataFile I:\Pices\TrainingLibraries\RandomSampling2ndStage.data  -Action SHUFFLE -folds 10 

// C:\Users\kkramer\SipperProject\Papers\BinaryFeatureSelection\Experiments\Stereologer
// -format Sparse  -DataFile CellFeatures.data  -Action Shuffle  -folds 20  


// -datafile  D:\Pices\TrainingLibraries\ETP_08_SubSet\ETP_08_SubSet.data  -Action SHUFFLE -Norm  -OutputFormat C45  -OutputFileName D:\Pices\TrainingLibraries\ETP_08_SubSet\ETP_08_SubSetC45.data  -Features 0-84


#define  MaxClassCount  500


enum  class  Action: int
{
  Shuffle,
  Split,
  CreateTrainAndTest,
  Convert,
  TrimClasses
};  



KKStr  ActionToStr (Action _action)
{
  KKStr  actionStr = "";
  switch  (_action)
  {
  case  Action::Shuffle:             actionStr = "Shuffle";       break;
  case  Action::Split:               actionStr = "Split";         break;
  case  Action::CreateTrainAndTest:  actionStr = "TrainAndTest";  break;
  case  Action::Convert:             actionStr = "Convert";       break;
  case  Action::TrimClasses:         actionStr = "TrimClasses";   break;
  }

  return  actionStr;
}



Action                action;

bool                  cancelFlag = false;

KKStr                 classNames[MaxClassCount];

int                   classCount;

bool                  deleteDuplicates = false;

FileDescConstPtr      fileDesc = NULL;

KKStr                 inputFileName;

FeatureFileIOPtr      inputFormat = FeatureFileIOPices::Driver ();

int                   lineCount;

int                   lopOff;    // Number of rows that you want to keep.
                                 // lessThan 0 indicates  keep all.

bool                  normalizeData = false;

int                   numOfFolds = 10;

KKStr                 outputFileName;

FeatureFileIOPtr      outputFormat = FeatureFileIOPices::Driver ();

double                percentSplit;

KKStr                 reportFileName;

ofstream*             report = NULL;

RunLog                runLog; 

KKStr                 rootName;

KKStr                 selectedFeaturesStr;

FeatureNumListPtr     selectedFeatures = NULL;

bool                  successful = false;

int                   trainLimitPerClass = -1;   // if > 0 then limites the numbetr of examples that can go into the traning set.

MLClassListPtr        mlClasses = NULL;




void  ShuffleData (FeatureVectorListPtr  examples)
{
  srand  ((unsigned)time (NULL));

  FeatureVectorListPtr  shuffledData = 
          examples->StratifyAmoungstClasses (mlClasses, lopOff, numOfFolds, runLog);

  KKStr  outFileName = rootName + "_shuffled.data";
  KKStr  statFile    = rootName + ".statistics";

  ofstream  stats (statFile.Str ());


  stats << "Input File[" << inputFileName.Str () << "]  Records[" << examples->QueueSize () << "]" << endl
        << endl
        << "Input Class Statistics" << endl
        << examples->ClassStatisticsStr ()
        << endl
        << endl;


  stats << "Output File[" << outFileName.Str () << "]  Records[" << shuffledData->QueueSize () << "]" << endl
        << endl
        << "Output Class Statistics" << endl
        << shuffledData->ClassStatisticsStr ()
        << endl
        << endl;


  KKB::uint  numExamplesWritten = 0;
  outputFormat->SaveFeatureFile (outFileName, *selectedFeatures, *shuffledData, numExamplesWritten, cancelFlag, successful, runLog);

  stats.close ();

  delete  shuffledData;

}  /* ShuffleData */




void  SplitExamples (FeatureVectorList*  examples)
{
  int  p1 = (int)(percentSplit * 100.0);
  int  p2 = 100 - p1;

  KKStr  outFileName1 (rootName);
  if  (normalizeData)
    outFileName1 << "_norm";
  outFileName1 << "_" << p1 << "p.data";

  KKStr  outFileName2 (rootName);
  if  (normalizeData)
    outFileName2 << "_norm";
  outFileName2 << "_" << p2 << "p.data";

  KKStr  statFile (rootName);
  rootName << ".statistics";
  ofstream  stats (rootName.Str ());

  stats << "Input File["   << inputFileName  << "]  Records[" << examples->QueueSize ()  << "]"  << endl; 
  stats << "Output Files[" << outFileName1   << ", " << outFileName2 << "]." << endl;
  stats << endl;

  FeatureVectorList  file1Examples (fileDesc, false);
  FeatureVectorList  file2Examples (fileDesc, false);

  MLClassListPtr  classes = examples->ExtractListOfClasses ();
    
  classes->SortByName ();
  MLClassList::iterator  idx = classes->begin ();

  stats << endl;
  stats << "ClassName" << "\t" << "Count" << endl;

  for  (idx = classes->begin ();  idx != classes->end ();  idx++)
  {
    MLClassPtr  mlClass = *idx;
    FeatureVectorListPtr  imagesThisClass = examples->ExtractExamplesForAGivenClass (mlClass, -1, -1);
    imagesThisClass->RandomizeOrder ();

    stats << mlClass->Name () << "\t" << imagesThisClass->QueueSize () << endl;

    int  divPoint = (int)((double)imagesThisClass->QueueSize () * percentSplit + 0.5);

    for  (int x = 0;  x < imagesThisClass->QueueSize ();  x++)
    {
      FeatureVectorPtr  i = imagesThisClass->IdxToPtr (x);
      if  (x < divPoint)
        file1Examples.PushOnBack (i);
      else
        file2Examples.PushOnBack (i);
    }

    delete  imagesThisClass;
  }

  file1Examples.RandomizeOrder ();
  file2Examples.RandomizeOrder ();

  stats << endl
        << endl
        << "File1[" << outFileName1 << "]" << endl
        << file1Examples.ClassStatisticsStr () << endl
        << endl;

  FeatureVectorListPtr  file1Shuffeled
         = file1Examples.StratifyAmoungstClasses (mlClasses, lopOff, numOfFolds, runLog);

  FeatureVectorListPtr  file2Shuffeled
         = file2Examples.StratifyAmoungstClasses (mlClasses, lopOff, numOfFolds, runLog);


  stats << "Output File[" << outFileName1.Str () << "]  Records[" << file1Shuffeled->QueueSize () << "]" << endl;
  stats << "Output File[" << outFileName2.Str () << "]  Records[" << file2Shuffeled->QueueSize () << "]" << endl;
  
  KKB::uint  numExamplesWritten = 0;
  outputFormat->SaveFeatureFile (outFileName1, file1Shuffeled->AllFeatures (), *file1Shuffeled, numExamplesWritten, cancelFlag, successful, runLog); 
  outputFormat->SaveFeatureFile (outFileName2, file2Shuffeled->AllFeatures (), *file2Shuffeled, numExamplesWritten, cancelFlag, successful, runLog); 

  stats << endl
        << "File1 Class Stats" << endl
        << file1Shuffeled->ClassStatisticsStr ()
        << endl
        << endl;

  stats << endl
        << "File2 Class Stats" << endl
        << file2Shuffeled->ClassStatisticsStr ()
        << endl
        << endl;

  stats.close ();

  delete  file1Shuffeled;
  delete  file2Shuffeled;
}  /* SplitExamples */





void  NormalizeData (FeatureVectorListPtr  examples)
{
  runLog.Level (10) << "NormalizeData: Normalizing Data" << endl;
  TrainingConfiguration2 config (mlClasses, fileDesc, "",  runLog); 
  NormalizationParms normalizationParms (&config, *examples, runLog);
  normalizationParms.NormalizeExamples (examples, runLog);
}  /* NormalizeData */





void  CreateTrainAndTestDataSets (FeatureVectorListPtr  examples)
{
  // Comment out to create the same files every time.
  SRand48  ((unsigned)time (NULL));
  examples->RandomizeOrder ();

  int  numOfClasses = mlClasses->QueueSize ();
  FeatureVectorListPtr  classExamples = NULL;

  if  (lopOff < 1)
    lopOff = INT_MAX;

  FeatureVectorList  trainingExamples (fileDesc, false);
  FeatureVectorList  testExamples     (fileDesc, false);

  for  (int cIDX = 0;  cIDX < numOfClasses;  cIDX++)
  {
    examples->RandomizeOrder ();

    classExamples = examples->ExtractExamplesForAGivenClass (mlClasses->IdxToPtr (cIDX), lopOff, -1);

    int  maxNumOfTrainExamples = (int)(classExamples->QueueSize () * percentSplit + 0.5);
    if  ((maxNumOfTrainExamples > trainLimitPerClass)  &&  (trainLimitPerClass > 0))
      maxNumOfTrainExamples = trainLimitPerClass;

    classExamples->RandomizeOrder ();

    FeatureVectorList::iterator iIDX;
    FeatureVectorPtr  image = NULL;

    for  (iIDX= classExamples->begin ();  (iIDX != classExamples->end ());  iIDX++)
    {
      image = *iIDX;

      if  (trainingExamples.size () < (KKB::uint)maxNumOfTrainExamples)
        trainingExamples.PushOnBack (image);
      else
        testExamples.PushOnBack (image);
    }
  }

  {
    trainingExamples.RandomizeOrder ();
    testExamples.RandomizeOrder ();
  }


  {
    if  (numOfFolds > 1)
    {
      FeatureVectorListPtr  testExamplesStratified = trainingExamples.StratifyAmoungstClasses (mlClasses, -1, numOfFolds, runLog);
      KKB::uint  numExamplesWritten = 0;
      outputFormat->SaveFeatureFile (rootName + "_Train.data",  *selectedFeatures, *testExamplesStratified, numExamplesWritten, cancelFlag, successful, runLog);
      delete  testExamplesStratified;  testExamplesStratified = NULL;
    }
    else
    {
      KKB::uint  numExamplesWritten = 0;
      outputFormat->SaveFeatureFile (rootName + "_Train.data",  *selectedFeatures, trainingExamples, numExamplesWritten, cancelFlag, successful, runLog);
    }
  }
  {
    KKB::uint  numExamplesWritten = 0;
    outputFormat->SaveFeatureFile (rootName + "_Test.data", *selectedFeatures, testExamples, numExamplesWritten, cancelFlag, successful, runLog);
  }
}  /* CreateTrainAndTestDataSets */





void  DisplayCommandLineUssage ()
{
  cout << endl;
  cout << "RandomizeData  -File               <Root_Data_FileName>"                                        << endl;
  cout << "               -Features           <Features to Include  defaults to all>"                      << endl;
  cout << "               -Folds              <Num of Folds>"                                              << endl;
  cout << "               -Format             [" << FeatureFileIO::FileFormatsReadAndWriteOptionsStr () << "]" << endl;
  cout << "               -InputFormat        [" << FeatureFileIO::FileFormatsReadOptionsStr () << "]"     << endl;
  cout << "               -Action             <SPLIT | SHUFFLE | TEST_VALIDATE | CONVERT> | TrimClasses>"  << endl;
  cout << "               -p                  <% to split file by>"                                        << endl;
  cout << "               -LopOff             <Num_Of_Rows to Keep by Class>"                              << endl;
  cout << "               -TrainLimitPerClass <Training examples per class"                                << endl;
  cout << "               -Norm               /* Normalize Data */"                                        << endl;
  cout << "               -OutputFormat       [" << FeatureFileIO::FileFormatsWrittenOptionsStr () << "]"  << endl;
  cout << "               -OutputFileName     <File Name>"                                                 << endl;
  cout << "               -ReportFile         <Report File Name>"                                          << endl;
  cout << "               -DeleteDuplicates   <Yes | No>"                                                  << endl;
  cout << endl;
}






void  ProcessParms (int    argcXXX,  
                    char** argvXXX
                   )
{
  KKB::uint x = 0;

  CmdLineExpander  cmdLineExpander (KKStr ("RandomizeData"),  runLog,  argcXXX, argvXXX);

  if  (!cmdLineExpander.ParmsGood ())
  {
    DisplayCommandLineUssage ();
    osWaitForEnter ();
    exit (-1);
  }

  const VectorKKStr  parameters = cmdLineExpander.ExpandedParameters ();

  inputFormat  = FeatureFileIOPices::Driver ();
  action       = Action::Split;
  percentSplit = 0.75;
  lopOff       = -1;

  bool  outputFormatSpecified = false;

  for  (x = 0; x < parameters.size (); x++)
  {
    KKStr  cmdSwitch (parameters[x]);

    if  (cmdSwitch.FirstChar () != '-')
    {
      cerr << "*** ERROR ***  Invalid Parameter[" << cmdSwitch << "]" << endl;
      exit (-1);
    }

    KKStr  parm;

    if  ((x + 1) < parameters.size ())
    {
      if  (parameters[x + 1][0] != '-')
      {
        x++;
        parm = parameters[x]; 
      }
    }

    KKStr  parmOrig (parm);
    parm.Upper ();

    cmdSwitch.Upper ();

    if  ((cmdSwitch == "-A")  ||  (cmdSwitch == "-ACTION"))
    {
      if  (parm == "SPLIT")
        action = Action::Split;
                    
      else if  (parm == "SHUFFLE")
        action = Action::Shuffle;
 
      else if  (parm.EqualIgnoreCase ("TrainAndTest")  ||  parm.EqualIgnoreCase ("TrainTest"))
        action = Action::CreateTrainAndTest;
 
      else if  (parm == "CONVERT")
        action = Action::Convert;

      else if  ((parm == "TRIMCLASSES")  ||  (parm == "TC"))
        action = Action::TrimClasses;
 
      else
      {
         cerr << "*** ERROR *** Invalid Action[" << parm << "]" << endl;
         DisplayCommandLineUssage ();
         exit (-1);
      }
    }

    else if  (cmdSwitch == "-DELETEDUPLICATES")
    {
      if  ((parm == "")  ||(parm == "YES")  ||  (parm == "Y")  ||  (parm == "TRUE")  ||  (parm == "T"))
        deleteDuplicates = true;

      else if  ((parm == "NO")  ||  (parm == "N")  ||  (parm == "FASLE")  ||  (parm == "F"))
        deleteDuplicates = false;

      else
      {
        cerr << endl << endl 
             << "*** ERROR ***  Invalid 'DeleteDuplicates' parm[" << parmOrig << "]" << endl 
             << endl;
        DisplayCommandLineUssage ();
        exit (1);
      }
    }

    else if  ((cmdSwitch == "-F")  ||  (cmdSwitch == "-FILE")  ||  (cmdSwitch == "-DATAFILE"))
    {
      if  (parmOrig.Empty ())
      {
        cerr << "*** ERROR ***  Must provide root file name with -F parameter." << endl;
        DisplayCommandLineUssage ();
        exit (1);
      }

      {
        KKStr  dirPath = "";
        KKStr  ext     = "";

        osParseFileName (parmOrig, dirPath, rootName, ext);
        if  (!dirPath.Empty ())
        {
          rootName = dirPath + DS + rootName;
        }
        inputFileName = parmOrig;
      }
    }

    else if  (cmdSwitch == "-FEATURES")
    {
      selectedFeaturesStr = parmOrig;
    }

    else if  ((cmdSwitch == "-FORMAT")       ||  
              (cmdSwitch == "-FILEFORMAT")   ||  
              (cmdSwitch == "-IF")           ||  
              (cmdSwitch == "-INPUTFORMAT")
             )
    {
      inputFormat = FeatureFileIO::FileFormatFromStr (parm, true, false);
      if  (inputFormat == NULL)
      {
        cerr << "*** ERROR *** Invalid File format  " << FeatureFileIO::FileFormatsReadOptionsStr () << endl;
        DisplayCommandLineUssage ();
        osWaitForEnter ();
        exit (-1);
      } 
    }

    else if  ((cmdSwitch == "-LO")  ||  ((cmdSwitch == "-LOPOFF")))
    {
      lopOff = atoi (parm.Str ());
      if  (lopOff < 1)
      {
        cerr << "*** ERROR ***  Num of lines to keep must be greater than 0." << endl;
        DisplayCommandLineUssage ();
        osWaitForEnter ();
        exit (-1);
      }
    }

    else if  ((cmdSwitch == "-N")  ||  (cmdSwitch == "-NORM")  ||  (cmdSwitch == "-NORMALIZE"))
    {
      normalizeData = true;
    }


    else if  ((cmdSwitch == "-O")             ||  
              (cmdSwitch == "-OF")             ||  
              (cmdSwitch == "-OUTPUTFORMAT")
             )
    {
      outputFormatSpecified = true;
      outputFormat = FeatureFileIO::FileFormatFromStr (parm, false, true);
      if  (outputFormat == NULL)
      {
        cerr << "*** ERROR *** Invalid File format " << FeatureFileIO::FileFormatsWrittenOptionsStr () << endl;
        DisplayCommandLineUssage ();
        osWaitForEnter ();
        exit (-1);
      } 
    }

    else if  ((cmdSwitch == "-OUTPUTFILENAME")  ||  
              (cmdSwitch == "-OFN")
             )
    {
      outputFileName = parm;
    }

    else if  ((cmdSwitch == "-REPORT")  ||  (cmdSwitch == "REPORTFILE")  ||  (cmdSwitch == "RF"))
      reportFileName = parmOrig;

    else if  ((cmdSwitch == "-P")  ||  (cmdSwitch == "PERCENTSPLIT")  ||  (cmdSwitch == "PS"))
    {
      percentSplit = atof (parm.Str ()) / 100;
      if  ((percentSplit < 0.01)  &&  (percentSplit > 1.0))
      {
        cerr << "*** ERROR ***  Unreasonable Split Percentage Specified." << endl;
        DisplayCommandLineUssage ();
        osWaitForEnter ();
        exit (-1);
      }
    }

    else if  (cmdSwitch.EqualIgnoreCase ("-TrainLimitPerClass"))
    {
      trainLimitPerClass = parm.ToInt ();
    }


    else if  (cmdSwitch == "-FOLDS")  
    {
      numOfFolds = parm.ToInt ();
    }


    else
    {
      cerr << "*** ERROR ***  Invalid Parameter[" << cmdSwitch << "]" << endl;
      DisplayCommandLineUssage ();
      osWaitForEnter ();
      exit (-1);
    }
  }

  if  (rootName.Empty ())
  {
    cerr << "*** ERROR ***  No Root File specified." << endl;
    DisplayCommandLineUssage ();
    osWaitForEnter ();
    exit (1);
  }

  if  (!outputFormatSpecified)
  {
    outputFormat = inputFormat;
  }


  if  ((reportFileName.Empty ())  &&  (!inputFileName.Empty ()))
  {
    reportFileName = osRemoveExtension (inputFileName) + "_Report.txt";
  }

  if  (!reportFileName.Empty ())
    report = new ofstream (reportFileName.Str ());
 
} /* ProcessParms */



void  DisplayCmdLineParameters (ostream*  r)
{
  *r << endl << endl
     << "Command Line Parameters Specified" << endl
     << "=================================" << endl
     << endl;
  
  *r << "RunDate            [" << osGetLocalDateTime ()                 << "]"  << endl;
  *r << "DataFile           [" << inputFileName                         << "]"  << endl;
  *r << "DataFileTimeStamp  [" << osGetFileDateTime (inputFileName)     << "]"  << endl;
  *r << "Format             [" << inputFormat->DriverName ()            << "]"  << endl;
  *r << "Action             [" << ActionToStr (action)                  << "]"  << endl;
  *r << "DeleteDuplicates   [" << (deleteDuplicates ? "Yes":"No")       << "]"  << endl;
  *r << "Features           [" << selectedFeaturesStr                   << "]"  << endl;
  *r << "LopOff             [" << lopOff                                << "]"  << endl;
  *r << "Normalize          [" << (normalizeData ? "Yes":"No")          << "]"  << endl;
  *r << "OutputFormat       [" << outputFormat->DriverName ()           << "]"  << endl;
  *r << "OutputFileName     [" << outputFileName                        << "]"  << endl;
  *r << "ReportFile         [" << reportFileName                        << "]"  << endl;
  *r << "PercentSplit       [" << (percentSplit * 100) << "%"           << "]"  << endl;
  *r << "Folds              [" << numOfFolds                            << "]"  << endl;

}  /* DisplayCmdLineParameters */





void  ConvertData (FeatureVectorListPtr  examples)
{
  runLog.Level (10)  << "Converting [" << inputFileName << "]  From["
                     << inputFormat->DriverName ()  << "]  To["
                     << outputFormat->DriverName () << "]" 
                     << endl;

  if  (normalizeData)
    NormalizeData (examples);

  lineCount = 0;

  KKStr  newFileName (rootName);
  if  (normalizeData)
    newFileName << "_norm";

  if  (outputFormat == FeatureFileIOArff::Driver ())
  {
    newFileName << "." << outputFormat->DriverName ();
  }
  else
  {
    newFileName << "_" << outputFormat->DriverName () << ".data";
  }

  cout << "Writting out [" << newFileName << "]." << endl;

  KKB::uint  numExamplesWritten = 0;
  outputFormat->SaveFeatureFile (newFileName, *selectedFeatures, *examples, numExamplesWritten, cancelFlag, successful, runLog);
}  /* ConvertData */




void   TrimClasses (FeatureVectorListPtr  examples)
{
  runLog.Level (10) << "TrimClassesAction" << endl;

  MLClassListPtr  classes = examples->ExtractListOfClasses ();
  MLClassList::iterator  idx;


  KKStr  configFileName = osRemoveExtension (inputFileName) + "_Trimmed.cfg";
  ofstream  configFile (configFileName.Str ());

  FeatureVectorList  trimedList (examples->FileDesc (), false);

  MLClassList  newClassList;

  int  minRequired = numOfFolds * 2;

  for  (idx = classes->begin ();  idx != classes->end ();  idx++)
  {
    FeatureVectorListPtr  imagesThisClass = examples->ExtractExamplesForAGivenClass (*idx, lopOff, -1);

    if  (imagesThisClass->QueueSize () > minRequired)
    {
      trimedList.AddQueue (*imagesThisClass);

      configFile << "[Training_Class]" << endl
                 << "dir=c:\\TrainingApp\\TrainingImages\\" << (*idx)->Name () << endl
                 << "class_name=" << (*idx)->Name () << endl
                 << endl
                 << endl;
      newClassList.PushOnBack (*idx);
    }
    delete  imagesThisClass;
  }

  delete  classes;

  configFile.close ();

  if  (outputFileName.Empty ())
    outputFileName = osRemoveExtension (inputFileName) + "_Trimmed." + osGetFileExtension (inputFileName);

  FeatureVectorListPtr  straifiedList = trimedList.StratifyAmoungstClasses (&newClassList, lopOff, numOfFolds, runLog);

  KKB::uint  numExamplesWritten = 0;
  outputFormat->SaveFeatureFile (outputFileName, *selectedFeatures, *straifiedList, numExamplesWritten, cancelFlag, successful, runLog);
  delete  straifiedList;
}  /* TrimClassesAction */



void  ConverShuttle ()
{
  FILE* in = osFOPEN ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\Pendigits\\pendigits.tra.txt", "r");

  char  buff[10240];
  ofstream  out ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\Pendigits\\pendigits_train_sparse.txt");

  while  (fgets (buff, sizeof (buff), in))
  {
    KKStr  ln (buff);
    ln.TrimLeft ();
    ln.TrimRight ();
    VectorKKStr  fields;

    KKStr field = ln.ExtractToken (", \n\t\r");
    while  (!field.Empty ())
    {
      fields.push_back (field);
      field = ln.ExtractToken (", \n\t\r");
    }

    KKStr  className = fields[fields.size () - 1];
/*
    if  (className == "1")
      className = "RadFlow";

    else if  (className == "2")
      className = "FpvClose";

    else if  (className == "3")
      className = "FpvOpen";

    else if  (className == "4")
      className = "High";

    else if  (className == "5")
      className = "Bypass";

    else if  (className == "6")
      className = "BpvClose";

    else if  (className == "7")
      className = "BpvOpen";

    else
    {
      cout << "*** Invalid Class Number[" << className << "]" << endl;
      osWaitForEnter ();
    }
*/

    int  x;
    out << className;
    for  (x = 0; x < (int)(fields.size () - 1); x++)
      out << " " << x << ":" << fields[x];
    out << endl;
  }

  fclose (in);
  out.close ();
}  /* ConverShuttle */



void  ConverLetter ()
{
  FILE* in = osFOPEN ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\Letter\\letter-recognition.data", "r");

  char  buff[10240];
  ofstream  out ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\Letter\\letter-sparse.data");

  while  (fgets (buff, sizeof (buff), in))
  {
    KKStr  ln (buff);
    ln.TrimLeft ();
    ln.TrimRight ();
    VectorKKStr  fields;

    KKStr field = ln.ExtractToken (", \n\t\r");
    while  (!field.Empty ())
    {
      fields.push_back (field);
      field = ln.ExtractToken (", \n\t\r");
    }
    KKStr  className = fields[0];
/*
    if  (className == "1")
      className = "RadFlow";

    else if  (className == "2")
      className = "FpvClose";

    else if  (className == "3")
      className = "FpvOpen";

    else if  (className == "4")
      className = "High";

    else if  (className == "5")
      className = "Bypass";

    else if  (className == "6")
      className = "BpvClose";

    else if  (className == "7")
      className = "BpvOpen";

    else
    {
      cout << "*** Invalid Class Number[" << className << "]" << endl;
      osWaitForEnter ();
    }
*/

    int  x;
    out << className;
    for  (x = 1; x < (int)(fields.size ()); x++)
      out << " " << (x - 1) << ":" << fields[x];
    out << endl;
  }

  fclose (in);
  out.close ();
}  /* ConverLetter */




void  ForestCover ()
{
  bool            successful  = true;
  bool            changesMade = false;
  MLClassList  classes;
  FeatureVectorListPtr trainData = FeatureFileIOC45::Driver ()->LoadFeatureFile 
                                                   ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\ForestCover\\CovType_TwoClass.data", 
                                                    classes,
                                                    -1,
                                                    cancelFlag,
                                                    successful,
                                                    changesMade,
                                                    runLog
                                                  );

  FeatureVectorListPtr testData = FeatureFileIOC45::Driver ()->LoadFeatureFile 
                                                  ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\ForestCover\\CovType_TwoClass.test", 
                                                    classes,
                                                    -1,
                                                    cancelFlag,
                                                    successful,
                                                    changesMade,
                                                    runLog
                                                  );

  FeatureVectorListPtr allData =  new FeatureVectorList (*trainData, false);
  allData->AddQueue (*testData);

  KKB::uint  numExamplesWritten = 0;
  FeatureFileIOC45::Driver ()->SaveFeatureFile 
                  ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\ForestCover\\CovType_TwoClass.all", 
                   allData->AllFeatures (), *allData, 
                   numExamplesWritten, 
                   cancelFlag,
                   successful,
                   runLog
                  );

  delete  allData;
  delete  testData;
  delete  trainData;
}  /* ForestCover */




void  CheckWebData ()
{
  runLog.Level (50);

  ofstream  r ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\WEB_Data\\anonymous-msweb_Differences.txt");

  bool            successful  = true;
  bool            changesMade = false;
  MLClassList  classes;
  FeatureVectorListPtr sparseData = FeatureFileIOSparse::Driver ()->LoadFeatureFile 
                                                   ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\WEB_Data\\anonymous-msweb_sparse.data", 
                                                    classes,
                                                    -1,
                                                    cancelFlag,
                                                    successful,
                                                    changesMade,
                                                    runLog
                                                  );

  FeatureVectorListPtr trainData = FeatureFileIOC45::Driver ()->LoadFeatureFile 
                                                   ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\WEB_Data\\anonymous-msweb_Data-C45.data", 
                                                    classes,
                                                    -1,
                                                    cancelFlag,
                                                    successful,
                                                    changesMade,
                                                    runLog
                                                  );

  FeatureVectorListPtr testData = FeatureFileIOC45::Driver ()->LoadFeatureFile 
                                                   ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\WEB_Data\\anonymous-msweb_Test-C45.data", 
                                                    classes,
                                                    -1,
                                                    cancelFlag,
                                                    successful,
                                                    changesMade,
                                                    runLog
                                                  );


  FeatureVectorListPtr allData =  new FeatureVectorList (*trainData, false);
  allData->AddQueue (*testData);

  KKB::uint  numExamplesWritten = 0;
  FeatureFileIOC45::Driver ()->SaveFeatureFile 
                  ("K:\\Plankton\\Papers\\BitReduction\\DataSets\\WEB_Data\\anonymous-msweb_All-C45.data", 
                   allData->AllFeatures (), *allData, 
                   numExamplesWritten, 
                   cancelFlag, successful, 
                   runLog
                  );
  return

  classes.SortByName ();

  MLClassList::iterator  cIDX;
  for  (cIDX = classes.begin ();  cIDX != classes.end ();  cIDX++)
  {
    MLClassPtr  c = *cIDX;

    r << endl
      << endl
      << "ClassName" << "\t" << c->Name () << endl;

    KKStr  allMean;
    KKStr  sparseMean;

    FeatureVectorListPtr allClass    = allData->ExtractExamplesForAGivenClass (c);
    FeatureVectorListPtr sparseClass = sparseData->ExtractExamplesForAGivenClass (c);

    allMean    << allClass->QueueSize ();
    sparseMean << sparseClass->QueueSize ();

    int  fn;

    r << "Count";

    for  (fn = 0;  fn < sparseData->NumOfFeatures ();  fn++)
    {
      r << "\t" << fn;
      int    countA;
      float  totalA;
      float  meanA;
      float  varA;
      float  stdDevA;
      int    countS;
      float  totalS;
      float  meanS;
      float  varS;
      float  stdDevS;

      allClass->CalcStatsForFeatureNum (fn, countA, totalA, meanA, varA, stdDevA);
      sparseClass->CalcStatsForFeatureNum (fn, countS, totalS, meanS, varS, stdDevS);

      allMean    << "\t" << meanA;
      sparseMean << "\t" << meanS;
    }

    delete  allClass;
    delete  sparseClass;

    r << endl;
    r << allMean    << endl;
    r << sparseMean << endl;
    r << endl       << endl;
  }

  r.close ();
}

void  CreateFolds (int  numOfFolds)
{
  RunLog  log;

  KKStr outputRootDir = "C:\\Temp\\OilExperiment\\";

  bool  cancelFlag = false;
  bool  changesMade = true;
  bool  successfull = false;
  FeatureFileIOPtr  inputFormat = FeatureFileIOPices::Driver ();
  MLClassListPtr classes = new MLClassList ();
  FeatureVectorListPtr  zed = inputFormat->LoadFeatureFile
	  ("I:\\Pices\\may_BP_2010\\features_34_35_37.data",
	     *classes, 
	     -1, 
		 cancelFlag, 
		 successfull, 
		 changesMade, 
		 runLog
			 );

  ImageFeaturesListPtr examples = dynamic_cast<ImageFeaturesListPtr> (zed);
  zed = NULL;


  log.Level (10) << "CreateFolds  Creating 'exampleTrimmed'" << endl;
  ImageFeaturesListPtr examplesTrimmed 
	  = new ImageFeaturesList (examples->FileDesc (), false, 0);

  ClassStatisticListPtr stats = examples->GetClassStatistics ();
  ClassStatisticList::iterator idx;
  for  (idx = stats->begin ();  idx != stats->end ();  idx++)
  {
    if  ((*idx)->Count () < 10)
      continue;

	MLClassPtr c = (*idx)->MLClass ();
    ImageFeaturesListPtr  examplesThisClass = examples->ExtractExamplesForAGivenClass (c);
	examplesTrimmed->AddQueue (*examplesThisClass);
	delete  examplesThisClass; examplesThisClass = NULL;
  }

  ImageFeaturesListPtr  stratifiedExamples = examplesTrimmed->StratifyAmoungstClasses (numOfFolds, runLog);
  double  examplesPerFold = (double)stratifiedExamples->size () / numOfFolds;

  
  kkuint32  startIdx = 0;
  kkuint32  endIdx = (int)floor(startIdx + examplesPerFold);
  for  (int  foldNum = 0;  foldNum < numOfFolds;  ++foldNum)
  {
    if  (foldNum == (numOfFolds - 1))
	  endIdx = (kkuint32)stratifiedExamples->size ();

	ImageFeaturesListPtr trainThisFold = new ImageFeaturesList (examples->FileDesc (), false, 0);
	ImageFeaturesListPtr testThisFold  = new ImageFeaturesList (examples->FileDesc (), false, 0);

	for  (kkuint32 curIdx = 0;  curIdx < (int)stratifiedExamples->size ();  curIdx++)
	{
	  ImageFeaturesPtr  example = stratifiedExamples->IdxToPtr (curIdx);
      if  ((curIdx < startIdx)  || (curIdx >= endIdx))
		trainThisFold->PushOnBack (example);
	  else
        testThisFold->PushOnBack  (example);
	}

	ImageFeaturesListPtr trainStratified = trainThisFold->StratifyAmoungstClasses (10, runLog);

	KKStr  foldDirName = osAddSlash (outputRootDir) + "Fold_" + StrFormatInt (foldNum, "@@");
	osCreateDirectoryPath (foldDirName);

	KKStr trainFileName = osAddSlash (foldDirName) + "Oil_Fold_" + StrFormatInt (foldNum, "@@") + "_Train.data";
	KKStr testFileName  = osAddSlash (foldDirName) + "Oil_Fold_" + StrFormatInt (foldNum, "@@") + "_Test.data";
  KKB::uint  numExampleWritten = 0;
	inputFormat->SaveFeatureFile (trainFileName, 
		                          trainStratified->AllFeatures (), 
								  *trainStratified, 
								  numExampleWritten,
								  cancelFlag,
								  successfull,
								  log
								 );

	inputFormat->SaveFeatureFile (testFileName, 
		                          trainThisFold->AllFeatures (), 
								  *testThisFold, 
								  numExampleWritten,
								  cancelFlag,
								  successfull,
								  log
								 );

  startIdx = endIdx;
	endIdx = (int)floor (endIdx + examplesPerFold);
    
	delete trainStratified;  trainStratified = NULL; 
    delete testThisFold;     testThisFold    = NULL; 
    delete trainThisFold;    trainThisFold   = NULL; 
  }

  delete  stratifiedExamples;  stratifiedExamples = NULL;
  delete  classes;             classes            = NULL;
  delete  examples;            examples           = NULL;

  return;
}  /* CreateFolds */


void  FilterLessThan10 ()
{
  RunLog  log;

  KKStr outputRootDir = "C:\\Temp\\OilExperiment\\";

  bool  cancelFlag = false;
  bool  changesMade = true;
  bool  successfull = false;
  FeatureFileIOPtr  inputFormat = FeatureFileIOPices::Driver ();
  MLClassListPtr classes = new MLClassList ();
  FeatureVectorListPtr  zed = inputFormat->LoadFeatureFile
	  ("I:\\Pices\\may_BP_2010\\features_34_35_37.data",
	     *classes, 
	     -1, 
		 cancelFlag, 
		 successfull, 
		 changesMade, 
		 runLog
			 );

  ImageFeaturesListPtr examples = dynamic_cast<ImageFeaturesListPtr> (zed);
  zed = NULL;


  log.Level (10) << "CreateFolds  Creating 'exampleTrimmed'" << endl;
  ImageFeaturesListPtr examplesTrimmed 
	  = new ImageFeaturesList (examples->FileDesc (), false, 0);

  ClassStatisticListPtr stats = examples->GetClassStatistics ();
  ClassStatisticList::iterator idx;
  for  (idx = stats->begin ();  idx != stats->end ();  idx++)
  {
    if  ((*idx)->Count () < 10)
      continue;

	MLClassPtr c = (*idx)->MLClass ();
    ImageFeaturesListPtr  examplesThisClass = examples->ExtractExamplesForAGivenClass (c);
	examplesTrimmed->AddQueue (*examplesThisClass);
	delete  examplesThisClass; examplesThisClass = NULL;
  }


  //////
  //ImageFeaturesListPtr  stratifiedExamples = examplesTrimmed->StratafyAmoungstClasses (numOfFolds);
  //double  examplesPerFold = (double)stratifiedExamples->size () / numOfFolds;

  
  //int  startIdx = 0;
  //int  endIdx = (int)floor(startIdx + examplesPerFold);
  //for  (int  foldNum = 0;  foldNum < numOfFolds;  ++foldNum)
  //{
    //if  (foldNum == (numOfFolds - 1))
	//  endIdx = stratifiedExamples->size ();

	//ImageFeaturesListPtr trainThisFold = new ImageFeaturesList (examples->FileDesc (), false, log, 0);
	//ImageFeaturesListPtr testThisFold  = new ImageFeaturesList (examples->FileDesc (), false, log, 0);

	//for  (int curIdx = 0;  curIdx < (int)stratifiedExamples->size ();  curIdx++)
	//{
	  //ImageFeaturesPtr  example = stratifiedExamples->IdxToPtr (curIdx);
      //if  ((curIdx < startIdx)  || (curIdx >= endIdx))
		//trainThisFold->PushOnBack (example);
	  //else
        //testThisFold->PushOnBack  (example);
	//}

	//ImageFeaturesListPtr trainStratified = trainThisFold->StratafyAmoungstClasses (10);

	/*KKStr  foldDirName = osAddSlash (outputRootDir) + "Fold_" + StrFormatInt (foldNum, "@@");
	osCreateDirectoryPath (foldDirName);

	KKStr trainFileName = osAddSlash (foldDirName) + "Oil_Fold_" + StrFormatInt (foldNum, "@@") + "_Train.data";
	KKStr testFileName  = osAddSlash (foldDirName) + "Oil_Fold_" + StrFormatInt (foldNum, "@@") + "_Test.data";
    uint  numExampleWritten = 0;*/


	///
	KKStr filterLess10 = osAddSlash (outputRootDir) + "34_35_37_lessThan10.data";
	KKB::uint  numExampleWritten = 0;
	inputFormat->SaveFeatureFile (filterLess10, 
		                          examplesTrimmed->AllFeatures (), 
								  *examplesTrimmed, 
								  numExampleWritten,
								  cancelFlag,
								  successfull,
								  log
								 );
	///



    //startIdx = endIdx;
	//endIdx = (int)floor (endIdx + examplesPerFold);
    
	//delete trainStratified;  trainStratified = NULL; 
    //delete testThisFold;     testThisFold    = NULL; 
    //delete trainThisFold;    trainThisFold   = NULL; 
  //}

  //delete  stratifiedExamples;  stratifiedExamples = NULL;
  delete  classes;             classes            = NULL;
  delete  examples;            examples           = NULL;

  return;
}  /* CreateFolds */





//1) Load feature vector
//2) Get Class Stats
//3) Intiaize 10 Trainin and Test Lists
//4) For each class
 //  if  num in class >= 10
 //  {
//
//   }

//}


int  main (int argc, char **argv)
{
  //ForestCover ();
  //exit (-1);

  //CheckWebData ();
  //exit (-1);
  //FilterLessThan10();
  //CreateFolds (10);
  //exit (-1);

  mlClasses = new MLClassList ();

  ProcessParms (argc, argv);
  if  (report)
    DisplayCmdLineParameters (report);
  else
    DisplayCmdLineParameters (&cout);


  bool  successfull = false;
  bool  changesMade = false;
  FeatureVectorListPtr  examples = inputFormat->LoadFeatureFile (inputFileName, *mlClasses, -1, cancelFlag, successfull, changesMade, runLog);
  if  (!successfull)
  {
    runLog.Level (-1) << endl
                      << "   *** ERROR ***,   Reading Input File[" << inputFileName << "]." << endl
                      << endl;
    osWaitForEnter ();
    exit (-1);
  }
   
  fileDesc = examples->FileDesc ();
  if  (fileDesc == NULL)
  {
    runLog.Level (-1) << endl
                      << endl
                      << "main    *** ERROR ***      FileDesc == NULL" << endl
                      << endl;
    osWaitForEnter ();
    exit (-1);
  }

 
  if  (selectedFeaturesStr.Empty ())
  {
    selectedFeatures = new FeatureNumList (FeatureNumList::AllFeatures (fileDesc));
  }
  else
  {
    bool valid;
    selectedFeatures = new FeatureNumList (selectedFeaturesStr, valid);
    if  (!valid)
    {
      cerr << endl
           << endl
           << "*** ERROR ***    Invaid -Features[" << selectedFeaturesStr << "]" << endl
           << endl;
      DisplayCommandLineUssage ();
      exit (-1);
    }
  }

  if  (deleteDuplicates)
  {
    DuplicateImages  dupDetector (examples, runLog);
    if  (dupDetector.DuplicatesFound ())
    {
      dupDetector.ReportDuplicates (cout);
      dupDetector.PurgeDuplicates (examples, false, NULL);
    }
  }


  if  (normalizeData)
  {
    NormalizeData (examples);
  }

  lineCount = 0;

  switch  (action)
  {
    case  Action::CreateTrainAndTest:
        CreateTrainAndTestDataSets (examples);
        break;

    case  Action::Convert:
        ConvertData (examples);
        break;

    case  Action::TrimClasses:
        TrimClasses (examples);
        break;

    case  Action::Shuffle:
        ShuffleData (examples);
        break;

    case  Action::Split:
        SplitExamples (examples);
        break;

  }

  if  (report)
  {
    report->close ();
    delete  report;
    report = NULL;
  }

  return 0;
}
