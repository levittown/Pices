#include  "FirstIncludes.h"

#include  <stdio.h>

#include  <iostream>
#include  <fstream>
#include  <vector>


#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"

using namespace std;
using namespace KKB;

#include <sys/types.h>

#include  "KKQueue.h"
#include  "KKStr.h"


#include  "ActiveLearningReport.h"
#include  "ClassGroupHistory.h"
#include  "MLClass.h"
#include  "OSservices.h"
#include  "RunLog.h"
#include  "SortOrderType.h"





//********************************************************************************************
//*                                ActiveLearningReport                                      *
//*                                                                                          *
//*  Purpose:  Combine results from several different ClassGroupToalts where they all have   *
//*            the same IPC(Image Per Class)  and  IPR(Images Per Retraining). These results *
//*            can then be saved into a file, for later use with out having to extract them  *
//*            again.                                                                        *
//*                                                                                          *
//*  This object will scan the contents of a given directory where the results of an         *
//*  ActiveLearning run are stored and create the appropriate instances of ClassGroupTotals. *
//*  Using the totals stored in the objects  Various summary reports can be generated.       *
//*  And a summary file can also be generated.                                               *
//*                                                                                          *
//********************************************************************************************


class  RetrainingResults
{
public:
  RetrainingResults ();

  RetrainingResults (float  numOfTrainingImages,
                     float  accuracy,
                     float  numSupportVectors
                    );

  float  NumOfTrainingImages () {return numOfTrainingImages;}
  float  Accuracy            () {return accuracy;}
  float  NumSupportVectors   () {return numSupportVectors;}

private:
 float   numOfTrainingImages;
 float   accuracy;
 float   numSupportVectors;
};


typedef  RetrainingResults*  RetrainingResultsPtr;

class  RetrainingResultsList: public KKQueue<RetrainingResults>
{
public:
  RetrainingResultsList (bool _owner):
    KKQueue<RetrainingResults> (_owner)  {}
};


typedef  RetrainingResultsList*    RetrainingResultsListPtr;


class  SortOrderResults: public  RetrainingResultsList
{
public:
  SortOrderResults (SortOrderType  _sortOrder);
  SortOrderType    SortOrder ()  {return  sortOrder;}

private:
  SortOrderType  sortOrder;
};


typedef  SortOrderResults*  SortOrderResultsPtr;


typedef  KKQueue<SortOrderResults>  SortOrderResultsList;

typedef  SortOrderResultsList::iterator  SortOrderResultsListIterator;




RetrainingResults::RetrainingResults ():
  numOfTrainingImages (0.0f),
  accuracy            (0.0f),
  numSupportVectors   (0.0f)
{
}



RetrainingResults::RetrainingResults (float  numOfTrainingImages,
                                      float  accuracy,
                                      float  numSupportVectors
                                      ):
  numOfTrainingImages (numOfTrainingImages),
  accuracy            (accuracy),
  numSupportVectors   (numSupportVectors)
{
}




SortOrderResults::SortOrderResults (SortOrderType  _sortOrder):
  RetrainingResultsList (true),
  sortOrder             (_sortOrder)
{
}

  



ActiveLearningReport::ActiveLearningReport (RunLog&       _log,
                                            MLClassList&  _mlClasses,
                                            KKStr         _subDirName
                                           ):
  mlClasses           (_mlClasses),
  log                 (_log),
  subDirName          (_subDirName),
  results             (true),
  baseResultsFileName ("ActiveLearningResults")
{
  if  (!subDirName.Empty ())
  {
    KKStr  w (subDirName);
    osAddLastSlash (w);
    w << baseResultsFileName;
    baseResultsFileName = w;
  }


// C:\users\kkramer\GradSchool\Plankton\ActiveLearning\Results\010-IPC\2003-12-03_AllOrders_010-IPC_50-IPR

  auto x = subDirName.LocateLastOccurrence ('_');

  if  (x  &&  (x.value () > 5))
  {
    KKStr leftSide = subDirName.SubStrSeg (0, x.value ());
    KKStr rightSide = subDirName.SubStrPart (x.value () + 1);
    x = rightSide.LocateCharacter ('-');
    KKStr  IPR = rightSide.SubStrSeg (0, x.value ());
    imagesPerRetraining = atoi (IPR.Str ());

    x = leftSide.LocateLastOccurrence ('_');
    rightSide = subDirName.SubStrPart (x.value () + 1);
    x = rightSide.LocateCharacter ('-');
    KKStr  IPC = rightSide.SubStrSeg (0, x.value ());
    initialImagesPerClass = atoi (IPC.Str ());
  }
}  



ActiveLearningReport::~ActiveLearningReport ()
{
}



void  ActiveLearningReport::TryLoadingResultsForASpecificSortOrder (SortOrderType    sortOrder,
                                                                    KKStr           qualifier,
                                                                    MLClassList&  classes
                                                                   )
{
  log.Level (10) << "ActiveLearningReport::TryLoadingResultsForASpecificSortOrder["
                 << SortOrderDescr (sortOrder) 
                 << "]"  
                 << endl;

  KKStr  fileSpec = baseResultsFileName        + "_" 
                   + qualifier                  + "_"
                   + SortOrderDescr (sortOrder)
                   + "*";

  log.Level (20) << "TryLoadingResultsForASpecificSortOrder  FileSpec[" << fileSpec << "]" << endl;

  KKStrListPtr   dataFiles = osGetListOfFiles (fileSpec);

  if  (dataFiles)
  {
    ClassGroupTotalsPtr  totals   = NULL;

    for  (auto fileName: *dataFiles)
    {
      KKStr  fullFileName (subDirName);
      osAddLastSlash (fullFileName);
      fullFileName << *fileName;

      if  (osFileExists (fullFileName))
      {
        log.Level (10) << "TryLoadingResultsForASpecificSortOrder  Loading File[" << *fileName << "]." << endl;

        if  (!totals)
        {
          log.Level (10) << "Creating new 'ClassGroupTotals' object." << endl;
          totals = new ClassGroupTotals (fullFileName, 
                                         classes, 
                                         initialImagesPerClass,
                                         imagesPerRetraining,
                                         log
                                        );
        }
        else
        {
          log.Level (10) << "Including in Existing in 'ClassGroupTotals' object." << endl;
          bool  successful = false;
          totals->ReadInDataFile (fullFileName, successful);
          if  (!successful)
          {
            log.Level (-1) << endl
                           << endl
                           << "ActiveLearningReport::TryLoadingResultsForASpecificSortOrder      *** ERROR ***" << endl
                           << endl
                           << "Could not load ClassGroupTotals[" << fullFileName << "]." << endl
                           << endl;
            osWaitForEnter ();
            exit (-1);
          }
        }
      }
    }
  
    if  (totals)
    {
      log.Level (10) << "Done loading all Files into 'ClassGroupTotals' object." << endl;

      SortOrderResultsPtr  sortOrderResults = new SortOrderResults (sortOrder);

      for  (int retraining = 0;  retraining < totals->NumOfRetrainings ();  retraining++)
      {
        float  mean, variance, stdDev, min, max, accuracyDelta;
        totals->CalcRetrainingAccuracyStats (retraining, mean, variance, stdDev, min, max, accuracyDelta);
        float  numOfSupportVectors = totals->NumOfSupportVectors (retraining);

        sortOrderResults->PushOnBack (new RetrainingResults ((float)(retraining * imagesPerRetraining), mean, numOfSupportVectors));
      }

      results.PushOnBack (sortOrderResults);
    }
    else
    {
      log.Level (10) << "TryLoadingResultsForASpecificSortOrder, no Totals found for [" 
                     <<  SortOrderDescr (sortOrder) 
                     << "]." 
                     << endl;
    }
  }

  log.Level (10) << "ActiveLearningReport::TryLoadingResultsForASpecificSortOrder["
                 << SortOrderDescr (sortOrder) 
                 << "]  Done Loading"  
                 << endl;

}  /* TryLoadingResultsForASpecificSortOrder */



void   ActiveLearningReport::LoadResultsFiles ()
{
  ClassGroupTotalsVector  listOfResults;

  TryLoadingResultsForASpecificSortOrder (MixedRandomProbability,   "Combined", mlClasses);
  TryLoadingResultsForASpecificSortOrder (ProbabilityOrder,         "Combined", mlClasses);
  TryLoadingResultsForASpecificSortOrder (BreakTieOrder,            "Combined", mlClasses);
  TryLoadingResultsForASpecificSortOrder (ProbabilityLittleRandom,  "Combined", mlClasses);
  TryLoadingResultsForASpecificSortOrder (RandomOrder,              "Combined", mlClasses);
}  /* LoadResultsFiles */



void  ActiveLearningReport::PrintReport (ostream&  report)
{
  log.Level (10) << "ActiveLearningReport::PrintReport  Start" << endl;

  report << endl
         << endl
         << "****      Multi Class Results     ****" << endl
         << endl;

  kkuint32  numOfRetrainings = 0;

  for  (auto sortOrderResults: results)
  {
    report << SortOrderDescr (sortOrderResults->SortOrder ())  << "\t" << "\t" << "\t";
    numOfRetrainings = Max (numOfRetrainings, sortOrderResults->QueueSize ());
  }
  report << endl;
  
  for  (auto sortOrderResults : results)
  {
    report << "Image_Count"  << "\t" << "Accuracy" << "\t" << "NumSV" << "\t";
  }
  report << endl;

  for  (kkuint32 retraining = 0;  retraining < numOfRetrainings;  retraining++)
  {
    for  (auto sortOrderResults : results)
    {
      if  (retraining >= sortOrderResults->QueueSize ())
      {
        report << "\t\t\t";
      }
      else
      {
        RetrainingResultsPtr  retrainingResults = sortOrderResults->IdxToPtr (retraining);
        report << retrainingResults->NumOfTrainingImages ()                                     << "\t" 
               << StrFormatDouble (retrainingResults->Accuracy (),          "zzz,zz0.0") << "%" << "\t"
               << StrFormatDouble (retrainingResults->NumSupportVectors (), "zzz,zz0.0")        << "\t";
      }
    }

    report << endl;
  }

  log.Level (10) << "ActiveLearningReport::PrintReport  Done" << endl;
} /* PrintReport */



void  ActiveLearningReport::PrintReportWithIncrements (ostream&  report,
                                                       int       increment
                                                      )
{
  log.Level (10) << "ActiveLearningReport::PrintReportWithIncrements  Start" << endl;

  report << endl
         << endl
         << "****      Multi Class Results   Increment[" << increment << "]  ****" << endl
         << endl;

  int**  imageCounts    = new int*[results.QueueSize ()];
  int*   numImageCounts = new int[results.QueueSize ()];
  int    maxImageCount = 0;

  kkuint32  numOfRetrainings = 0;
  kkuint32  soCount          = 0;
  for  (auto sortOrderResults : results)
  {
    report << SortOrderDescr (sortOrderResults->SortOrder ())  << "\t" << "\t" << "\t" << "\t";
    numOfRetrainings = Max (numOfRetrainings, sortOrderResults->QueueSize ());
    GetImageCountsByIncrements (sortOrderResults->SortOrder (), increment, numImageCounts[soCount], imageCounts[soCount]);
    maxImageCount = Max (maxImageCount, numImageCounts[soCount]);
    ++soCount;
  }
  report << endl;
  
  for  (auto sortOrderResults : results)
  {
    report << "Image_Count"  << "\t" << "Accuracy" << "\t" << "NumSV" << "\t" << "\t";
  }
  report << endl;

  for  (int count = 0;  count < maxImageCount;  count++)
  {
    soCount = 0;
    for  (auto sortOrderResults : results)
    {
      if  (count >= numImageCounts[soCount])
      {
        report << "\t\t\t\t";
      }
      else
      {
        float  numOfSupportVectors;
        float  accuracy;
        bool   found;

        GetStatsForNumOfImages (sortOrderResults->SortOrder (), 
                                imageCounts[soCount][count], 
                                numOfSupportVectors, 
                                accuracy, 
                                found
                               );

        if  (!found)
        {
          report << "\t\t\t\t";
        }
        else
        {
          report << imageCounts[soCount][count]                                << "\t" 
                 << StrFormatDouble (100 * accuracy,      "zzz,zz0.00") << "%" << "\t"
                 << StrFormatDouble (numOfSupportVectors, "zzz,zz0.0")         << "\t"
                 << "\t";
        }
      }

      ++soCount;
    }
    report << endl;
  }

  {
    // Lets clean up dynamically allocated memory.
    for  (soCount = 0;  soCount < results.QueueSize ();  ++soCount)
      delete  imageCounts[soCount];

    delete  imageCounts;
    delete  numImageCounts;
  }

  log.Level (10) << "ActiveLearningReport::PrintReportWithIncrements  Done" << endl;
} /* PrintReport */



void  ActiveLearningReport::Save (KKStr  fileName)
{
  ofstream f (fileName.Str ());

  if  (!f.is_open ())
  {
    log.Level (-1) << endl
                   << "ActiveLearningReport::Save        *** ERROR ***" << endl
                   << endl
                   << "                      Could not open File[" << fileName << "]" << endl
                   << endl;
    osWaitForEnter ();
    exit (-1);
  }

  f << "\\InitialTrainingImagesPerClass" << "\t" << initialImagesPerClass << endl;
  f << "\\ImagesPerRetraining"           << "\t" << imagesPerRetraining   << endl;

  for  (auto sortOrderResults : results)
  {
    f << "\\SortOrder"         << "\t" << SortOrderDescr (sortOrderResults->SortOrder ()) << endl;
    f << "\\NumOfRetrainings"  << "\t" << sortOrderResults->QueueSize ()                  << endl;
    f << "\\Fields"            << "\t" << "NumOfTrainingImages" 
                               << "\t" << "Accuracy"
                               << "\t" << "NumSupportVectors"
                               << endl;

    for  (auto retraining: *sortOrderResults)
    {
      f << retraining->NumOfTrainingImages () << "\t" 
        << retraining->Accuracy            () << "\t"
        << retraining->NumSupportVectors   ()
        << endl;
    }
  }

  f.close ();
}  /* Save */



void  ActiveLearningReport::Load (KKStr   fileName,
                                  bool&    validFormat
                                 )
{
  validFormat = false;

  FILE*  f = osFOPEN (fileName.Str (), "r");

  if  (!f)
  {
    log.Level (-1) << endl
                   << "ActiveLearningReport::Load        *** ERROR ***" << endl
                   << endl
                   << "                      Could noty open File[" << fileName << "]" << endl
                   << endl;
    return;
  }

  char  buff[10240];

  bool  fileParmsLoaded = false;

 
  {
    // Get Frst Row wicj should be InitialTrainingImagesPerClass 
    if  (!fgets (buff, sizeof (buff), f))
    {
      log.Level (-1) << endl
                     << "ActiveLearningReport::Load        *** ERROR ***" << endl
                     << endl
                     << "                      *** No Data ***" << endl
                     << endl;
      fclose (f);
      return;
    }

    KKStr l (buff);
    l.Upper ();

    KKStr  field = l.ExtractToken ("\n\t\r ");
    if  (field != "\\INITIALTRAININGIMAGESPERCLASS")
    {
      log.Level (-1) << endl
                     << "ActiveLearningReport::Load        *** ERROR ***" << endl
                     << endl
                     << "  First Line Not Valid[" << l << "]" << endl
                     << endl;
      fclose (f);
      return;
    }
    
    initialImagesPerClass = l.ExtractTokenInt ("\n\t\r ");
  }

  {
    // Get second Row wicj should be ImagesPerRetraining
    if  (!fgets (buff, sizeof (buff), f))
    {
      log.Level (-1) << endl
                     << "ActiveLearningReport::Load        *** ERROR ***" << endl
                     << endl
                     << "              *** Missing Second Line ***" << endl
                     << endl;
      fclose (f);
      return;
    }
    
    KKStr l (buff);
    l.Upper ();

    KKStr  field = l.ExtractToken ("\n\t\r ");
    if  (field != "\\IMAGESPERRETRAINING")
    {
      log.Level (-1) << endl
                     << "ActiveLearningReport::Load        *** ERROR ***" << endl
                     << endl
                     << "  Second Liine Not Valid[" << l << "]" << endl
                     << endl;
      fclose (f);
      return;
    }
    
    imagesPerRetraining = l.ExtractTokenInt ("\n\t\r ");
  }


  SortOrderResultsPtr  curSortOrderResults = NULL;

  while  (fgets (buff, sizeof (buff), f))
  {
    KKStr l (buff);

    if  (l.StartsWith ("\\"))
    {
      if  (curSortOrderResults)
      {
        results.PushOnBack (curSortOrderResults);
        curSortOrderResults = NULL;
      }

      SortOrderType  nextSortOrder = NULL_SortOrder;
      int            numOfRetrainings = 0;

      while  (l.StartsWith ("\\"))
      {
        l = l.SubStrPart (1);
        l.Upper ();

        KKStr  fieldName = l.ExtractToken ("\n\t\r ");
        if  (fieldName == "SORTORDER")
        {
          KKStr  sortOrderDesc = l.ExtractToken ("\n\t\r ");
          nextSortOrder = SortOrderTypeFromStr (sortOrderDesc);
        }
        else if  (fieldName == "NUMOFRETRAININGS")
        {
          numOfRetrainings = l.ExtractTokenInt ("\n\t\r ");
        }

        if  (fgets (buff, sizeof (buff), f))
          l = buff;
        else
          l = "";
      }
      
      if  (nextSortOrder == NULL_SortOrder)
      {
        log.Level (-1) << "ActiveLearningReport::Load        *** ERROR ***" << endl
                       << endl
                       << "   Invalid Sort Order Defined." << endl
                       << endl;
        fclose (f);
        return;
      }

      curSortOrderResults = new SortOrderResults (nextSortOrder);
    }

    if  (!curSortOrderResults)
    {
      log.Level (-1) << "ActiveLearningReport::Load        *** ERROR ***" << endl
                     << endl
                     << "   Invalid Sort Order Defined." << endl
                     << endl;
      fclose (f);
      return;
    }

    float numOfTrainingImages  = (float)l.ExtractTokenDouble ("\n\t\r ");
    float accuracy             = (float)l.ExtractTokenDouble ("\n\t\r ");
    float numOfSupportVectors  = (float)l.ExtractTokenDouble ("\n\t\r ");

    RetrainingResultsPtr  retrainingResults = new RetrainingResults (numOfTrainingImages,
                                                                     accuracy,
                                                                     numOfSupportVectors
                                                                    );
    curSortOrderResults->PushOnBack (retrainingResults);
  }

  if  (curSortOrderResults)
  {
    results.PushOnBack (curSortOrderResults);
    curSortOrderResults = NULL;
  }


  fclose (f);
  validFormat = true;
  return;
}  /* Load */




RetrainingResultsListPtr  ActiveLearningReport::GetRetrainingsForASortOrder (SortOrderType  sortOrder)
{
  for  (auto sortOrderResults: results)
  {
    if  (sortOrderResults->SortOrder () == sortOrder)
      return sortOrderResults;
  }

  return NULL;
}  /* GetRetrainingsByImageIncrements */



void  ActiveLearningReport::GetImageCountsByIncrements (SortOrderType  sortOrder,
                                                        int            increment,
                                                        int&           count,
                                                        int*&          imageCounts
                                                        )
{
  int x;

  RetrainingResultsListPtr retrainings = GetRetrainingsForASortOrder (sortOrder);

  if  (!retrainings)
  {
    count = 0;
    imageCounts = NULL;
    return;
  }

  if  (retrainings->QueueSize () < 1)
  {
    count = 0;
    imageCounts = NULL;
    return;
  }


  int*  counts = new int[retrainings->QueueSize ()];
  int   nextAllowableCount = 0;
  count = 0;

  for  (auto retraining: *retrainings)
  {
    if  (retraining->NumOfTrainingImages () >= nextAllowableCount)
    {
      counts[count] = (int)retraining->NumOfTrainingImages ();
      count++;
      nextAllowableCount = (int)retraining->NumOfTrainingImages () + increment;
    }
  }

  imageCounts = new int[count + 1];
  for (x = 0;  x < count;  x++)
    imageCounts[x] = counts[x];
  imageCounts[count] = -1;

  delete  counts;

  return;
}  /* GetImageCountsByIncrements */



void  ActiveLearningReport::GetStatsForNumOfImages (SortOrderType  sortOrder, 
                                                    int            numOfImages,
                                                    float&         numOfSupportVectors,
                                                    float&         accuracy,
                                                    bool&          found
                                                   )
{
  found = false;

  SortOrderResultsPtr sortOrderResults = NULL;

  for (auto temp: results)
  {
    if  (temp->SortOrder () == sortOrder)
    {
      sortOrderResults = temp;
      break;
    }
  }

  if (!sortOrderResults)
  {
    found = false;
    return;
  }

  RetrainingResultsPtr  retrainingResults = NULL;
  for  (auto temp: *sortOrderResults)
  {
    if  (temp->NumOfTrainingImages () > numOfImages)
    {
      // There are no results for this numOfImages, so we will return with found = false;     
      return;
    }

    if (temp->NumOfTrainingImages () == numOfImages)
    {
      retrainingResults = temp;
      break;
    }
  }

  if  (!retrainingResults)
    return;

  numOfSupportVectors = retrainingResults->NumSupportVectors ();
  accuracy            = retrainingResults->Accuracy ();
  found = true;
  return;
}  /* GetStatsForNumOfImages */



ActiveLearningReportList::ActiveLearningReportList (bool _owner):
     KKQueue<ActiveLearningReport> (_owner)
{
}
