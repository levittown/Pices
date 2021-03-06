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

using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
#include "StatisticalFunctions.h"
#include "KKStr.h"
using namespace KKB;


#include "FeatureFileIO.h"
using namespace KKB;

#include "Neighbor.h"
#include "RandomNND.h"


// -s C:\TrainingApp\ClassifiedImages\hrs020701 -r NeighborReport.txt


/******************************************************************************
 * command line parameters that have been used for testing purposes
 ******************************************************************************/

// -s "C:\TrainingApp\ClassifiedImages" -r "report.txt"


// 2005-06-03
// -src C:\TrainingApp\ClassifiedImages\HRS030208b  -Report TestTeport.txt  -random

// -src C:\TrainingApp\ClassifiedImages\hrs020404   -Report TestTeport.txt  -random


// -Report TestReport.txt  -Buckets 50  -BS 10  -I 20  

RandomNND::RandomNND () :
  PicesApplication  (),
  data           (NULL),
  report         (NULL),
  reportFile     (NULL),

  scanLines      (1000000),
  minRadius      (),
  minSize        (500.0),
  numOfParticles (10000),
  numIterations  (20),
  widthMax       (4096.0f),

  colLeft        (12.61f),
  colRight       (4082.39f),

  numOfBuckets   (50),
  bucketSize     (10)

{
}








/******************************************************************************
 * Destructor
 ******************************************************************************/
RandomNND::~RandomNND ()
{
	if  (reportFile)
	{
		reportFile->close ();
		delete  reportFile;
	}

  DeleteData ();
}






void  RandomNND::InitalizeApplication (kkint32 argc,
                                       char**  argv
                                      )
{
  PicesApplication::InitalizeApplication (argc, argv);
	if  (Abort ())
	{
		DisplayCommandLineParameters ();
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

  if  (numIterations < 1)
  {
    log.Level (-1) << "RandomNND::RandomNND   Invalid numIterations[" << numIterations << "]" << endl;
    DisplayCommandLineParameters ();
    osWaitForEnter ();
    exit (-1);
  }

  minRadius = sqrt (minSize / 3.1415926f);
  colLeft   = minRadius;
  colRight  = (widthMax - 1) - minRadius;
  width     = colRight - colLeft;

	*report << endl;
	*report << "------------------------------------------------------------------------" << endl;
  PrintStandardHeaderInfo (*report);
  *report << "Report File Name  [" << reportFileName                         << "]."    << endl;
  *report << "Scan Lines        [" << scanLines                              << "]."    << endl;
  *report << "Min Size          [" << minSize                                << "]."    << endl;
  *report << "Num of Particles  [" << numOfParticles                         << "]."    << endl;
  *report << "Num of Iterations [" << numIterations                          << "]."    << endl;
  *report << "Number of Buckets [" << numOfBuckets                           << "]."    << endl;
  *report << "Bucket Size       [" << bucketSize                             << "]."    << endl;
  *report << "------------------------------------------------------------------------" << endl;
	*report << endl;
}  /* InitalizeApplication*/




/******************************************************************************
 * ProcessCmdLineParamters
 * DESC: Extracts parameters from the command line
 ******************************************************************************/
bool  RandomNND::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                          const KKStr&  parmValue
                                         )
{
  if  (parmSwitch.EqualIgnoreCase ("-BUCKETS")  ||  parmSwitch.EqualIgnoreCase ("-B"))
  {
    numOfBuckets = parmValue.ToInt32 ();
    if  (numOfBuckets < 3)
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter    *** ERROR ***" << endl
                     << "             NumOfBuckets[" << numOfBuckets << "] Must be breater than 2" << endl
                     << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-BUCKETSIZE")  ||  parmSwitch.EqualIgnoreCase ("-BS"))
  {
    bucketSize = parmValue.ToInt32 ();
    if  (bucketSize < 1)
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter    *** ERROR ***" << endl
                     << "             BucketSize[" << bucketSize << "] Must be breater than 0" << endl
                     << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-MINSIZE")  ||  parmSwitch.EqualIgnoreCase ("-MS"))
  {
    minSize = parmValue.ToDouble ();
    if  (minSize < 1.0)
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter    *** ERROR ***" << endl
                     << "                MinSize[" << minSize << "] Must be breater than 1.0" << endl
                     << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-PARTICLES")  ||  parmSwitch.EqualIgnoreCase ("-P"))
  {
    numOfParticles = parmValue.ToInt32 ();
    if  (numOfParticles < 2)
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter    *** ERROR ***" << endl
                     << "                numOfParticles[" << numOfParticles << "] Must be breater than 1." << endl
                     << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-ITERATIONS")  ||  parmSwitch.EqualIgnoreCase ("-I"))
  {
    numIterations = parmValue.ToInt32 ();
    if  (numIterations < 1)
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter    *** ERROR ***" << endl
                     << "                NumIterations[" << numIterations << "] Must be breater than 0." << endl
                     << endl;
      Abort (true);
    }
  }

  else if  (parmSwitch.EqualIgnoreCase ("-R")  ||  parmSwitch.EqualIgnoreCase ("-REPORT"))
		reportFileName = parmValue;

  else if  (parmSwitch.EqualIgnoreCase ("-SCANLINES")  ||  parmSwitch.EqualIgnoreCase ("-SL"))
  {
    scanLines = parmValue.ToInt32 ();
    if  (scanLines < 100)
    {
      log.Level (-1) << endl
                     << "ProcessCmdLineParameter    *** ERROR ***" << endl
                     << "                scanLines[" << scanLines << "] Must be breater than 100." << endl
                     << endl;
      Abort (true);
    }
  }

  else
  {
    log.Level (-1) << endl
                   << "ProcessCmdLineParameter    Invalid Parameter[" << parmSwitch << "]" << endl
                   << endl;
    Abort (true);
  }


	return  !Abort ();
}  /* ProcessCmdLineParameter */




/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   RandomNND::DisplayCommandLineParameters ()
{
	cout << "RandomNND  -Report <xxx>  -Iterations <xxxx>  -ScanLines <xxxx>  -Particles <xxxx>  -MinSize <xxxx>"     << endl
	                                                                              << endl
	     << "    -Report(R)       Report File,  Defaults to Command Line."        << endl
	                                                                              << endl
       << "    -Buckets(B)      Number of Buckets in Histogram (50)."           << endl
	                                                                              << endl
       << "    -BucketSize(BS)  Size of each Bucket (10)."                      << endl
	                                                                              << endl
       << "    -Iterations(I)   Number of Nearest Neighbors to generate(20)."   << endl
	                                                                              << endl
       << "    -MinSize(MS)     Minimum Size of each plankton. (500)."          << endl
	                                                                              << endl
	     << "    -Particles(P)    Number of particles (10,000)."                  << endl
                                                                                << endl
	     << "    -ScanLines(SL)   Number of scan lines (1,000,000)."              << endl
       << endl;
}



void  RandomNND::DeleteData ()
{
  int  x;

  if  (data)
  {
    for  (x = 0;  x < numIterations;  x++)
    {
      delete data[x];
      data[x] = NULL;
    }
    delete[] data;
  }
}  /* DeleteData */




void  RandomNND::PopulateDataRandomly ()
{
  log.Level (10) << "RandomNND::PopulateDataRandomly" << endl;

  DeleteData ();

  int  x, y;

  data = new NeighborListPtr[numIterations];
  for  (x = 0;  x < numIterations;  x++)
  {
    cout << "*";
    data[x] = new NeighborList (true, numOfParticles, log);
    for  (y = 0;  y < numOfParticles; y++)
    {
      double  col = rand () % (int)(width + 0.5f) + colLeft;
      double  row = (double)((rand () * 10000 + rand ()) % scanLines);
      data[x]->PushOnBack (new Neighbor (row, col, "", NULL, minSize));
    }
  }
  cout << endl;
}  /* PopulateDataRandomly */





void	RandomNND::LookForNeighbors (int  iteration)
{
  log.Level (10) << "RandomNND::LookForNeighbors    iteration[" << iteration << "]" << endl;
  data[iteration]->FindNearestNeighbors (NeighborType::AnyPlankton, NULL);
}  /* LookForNeighbors */




void	RandomNND::GenerateReport ()
{
  int bucket;
  int iteration;
  int particle;

  PopulateDataRandomly ();

  for  (iteration = 0;  iteration < numIterations;  iteration++)
    LookForNeighbors (iteration);


  *report << "Index";
  for  (iteration = 0;  iteration < numIterations;  iteration++)
    *report << "\t" << iteration;
  *report << endl;

  for  (particle = 0;  particle < numOfParticles;  particle++)
  {
    *report << particle;
    for  (iteration = 0;  iteration < numIterations;  iteration++)
    {
      NeighborPtr n = data[iteration]->IdxToPtr (particle);
      *report << "\t" << n->Dist ();
    }
    *report << endl;
  }

  // Print out Stats for each iteration,  mean, stddev
  {
    *report << endl;

    VectorDouble  means   (numIterations, 0);
    VectorDouble  stdDevs (numIterations, 0);
    VectorDouble  mins    (numIterations, 0);
    VectorDouble  maxs    (numIterations, 0);

    KKStr  meanStr   (200);  meanStr   << "Mean";
    KKStr  stdDevStr (200);  stdDevStr << "StdDev";
    KKStr  minStr    (200);  minStr    << "Min";
    KKStr  maxStr    (200);  maxStr    << "Max";


    for  (iteration = 0;  iteration < numIterations;  iteration++)
    {
      data[iteration]->CalcStatistics (means   [iteration],
                                       stdDevs [iteration],
                                       mins    [iteration],
                                       maxs    [iteration]
                                      );
      
      meanStr   << "\t" << means   [iteration];
      stdDevStr << "\t" << stdDevs [iteration];
      minStr    << "\t" << mins    [iteration];
      maxStr    << "\t" << maxs    [iteration];
    }
  
    *report << "Stat";
    for  (iteration = 0;  iteration < numIterations;  iteration++)
      *report << "\t" << iteration;
    *report << "\t" << "Mean" << "\t" << "StdDev" << endl;

    double  mean, stdDev;
    CalcMeanAndStdDev (means, mean, stdDev);
    *report << meanStr << "\t" << mean << "\t" << stdDev << endl;;

    CalcMeanAndStdDev (stdDevs, mean, stdDev);
    *report << stdDevStr << "\t" << mean << "\t" << stdDev << endl;
    
    CalcMeanAndStdDev (mins, mean, stdDev);
    *report << minStr << "\t" << mean << "\t" << stdDev << endl;
    
    CalcMeanAndStdDev (maxs, mean, stdDev);
    *report << maxStr << "\t" << mean << "\t" << stdDev << endl;
  }



  *report << endl << endl << endl << endl << endl << endl << endl;

  *report << "Distance Histograms" << endl << endl;

  VectorIntPtr*  distHistograms = new VectorIntPtr[numIterations];
  for  (iteration = 0;  iteration < numIterations;  iteration++)
    distHistograms[iteration] = data[iteration]->HistogramByDistance (numOfBuckets, bucketSize);

  *report << "Bucket";
  for  (iteration = 0;  iteration < numIterations;  iteration++)
    *report << "\t" << iteration;
  *report << "\t" << "Min" << "\t" << "Max" << "\t" << "Avg" << "\t" << "StdDev";
  *report << endl;

  for  (bucket = 0;  bucket < numOfBuckets;  bucket++)
  {
    int  distMin   = INT_MAX;
    int  distMax   = INT_MIN;
    int  distTotal = 0;

    int  bucketValStart = bucket * bucketSize;
    int  bucketValEnd   = bucketValStart + bucketSize - 1;

    if  (bucket == (numOfBuckets - 1))
    {
      *report << ">=" << bucketValStart;
    }
    else
    {
      *report << bucketValStart << "-" << bucketValEnd;
    }
    for  (iteration = 0;  iteration < numIterations;  iteration++)
    {
      int  dist = (*(distHistograms[iteration]))[bucket];
      
      *report << "\t" << dist;

      distMin   =  Min (distMin, dist);
      distMax   =  Max (distMax, dist);
      distTotal += dist;
    }

    double distMean = (double)distTotal / (double)numIterations;

    double  deltaSquareTotal = 0.0f;
    for  (iteration = 0;  iteration < numIterations;  iteration++)
    {
      int  dist = (*(distHistograms[iteration]))[bucket];
      double  delta = (double)dist - distMean;
      deltaSquareTotal += delta * delta;
    }

    double  variance = deltaSquareTotal / numIterations;
    double  distStdDev = sqrt (variance);

    *report << "\t" << distMin << "\t" << distMax << "\t" << distMean << "\t" << distStdDev;
    *report << endl;
  }

  for  (iteration = 0;  iteration < numIterations;  iteration++)
  {
    delete  distHistograms[iteration];
    distHistograms[iteration] = NULL;
  }

  delete[]  distHistograms;
}  /* GenerateReport */





int  main (kkint32 argc,
           char**  argv
          )
{
  time_t     long_time;
  SRand48 ((KKB::uint)time (&long_time));
  RandomNND  neraestNeighborReport;
  neraestNeighborReport.InitalizeApplication (argc, argv);
  if  (!neraestNeighborReport.Abort ())
    neraestNeighborReport.GenerateReport ();
}
