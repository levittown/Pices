#include "FirstIncludes.h"

 
#include <string>
#include <ctype.h>
#include <math.h>
#include <stdio.h> 
#include <time.h>

#ifndef  WIN32
#include  <unistd.h>
#endif

#include <iomanip>
#include <iostream>
#include <fstream>
//#include  <ostream>
#include <iostream>
#include <map>
#include <vector>

#include "MemoryDebug.h"


#ifdef WIN32
#include <windows.h>
#else
#endif

using namespace  std;


#include "BasicTypes.h"
#include "CmdLineExpander.h"
using namespace  KKU;

#include "Instrument.h"
#include "InstrumentDataFileManager.h"
#include "InstrumentDataManager.h"
#include "SipperBuff.h"
#include "SipperVariables.h"
using namespace SipperHardware;


#include "DataBase.h"
#include "SipperCruise.h"
#include "SipperStation.h"
#include "SipperDeployment.h"
using namespace  MLL;



#include "SvnVersion.h"
#include "MarineSnowReport.h"


#define Q(x) #x
#define QUOTE(x) Q(x)



KKStr  marineSnowReportDirectory = "D:\\Users\\kkramer\\DropBox\\Dropbox\\USF_OilSpillGroup\\MarineSnow_2014-12-10";



class  DeploymentSummary
{
public:
  DeploymentSummary (const KKStr&  _cruiseName,
                     const KKStr&  _stationName,
                     const KKStr&  _deploymentNum,
                     double        _volumeSampled,
                     VectorFloat   _sizeThresholds,
                     VectorDouble  _integratedAbundance,
                     VectorInt32   _integratedCounts
                    ):
      cruiseName          (_cruiseName),
      stationName         (_stationName),
      deploymentNum       (_deploymentNum),
      volumeSampled       (_volumeSampled),
      sizeThresholds      (_sizeThresholds),
      integratedAbundance (_integratedAbundance),
      integratedCounts    (_integratedCounts)
  {
  }

  KKStr  cruiseName;
  KKStr  stationName;
  KKStr  deploymentNum;

  double volumeSampled;

  VectorFloat   sizeThresholds;
  VectorDouble  integratedAbundance;
  VectorInt32   integratedCounts;
};


void  CreateThresholdHeaders (VectorFloat&  sizeThresholds,
                              KKStr&        h1,
                              KKStr&        h2
                             )
{
  h1 = "";
  h2 = "";
  for  (uint32 c = 0;  c < sizeThresholds.size ();  ++c)
  {
    KKStr  mStr = "";
    KKStr  sStr = "";
    if (c == 0)
    {
      mStr = "";
      sStr = "<" + StrFormatDouble (sizeThresholds[c], "0.0000");
    }

    else if  (c < (sizeThresholds.size () - 1))
    {
      float  m = (sizeThresholds[c] + sizeThresholds[c + 1]) / 2.0f;
      mStr = StrFormatDouble (m, "0.0000");
      sStr = ">=" + StrFormatDouble (sizeThresholds[c], "0.0000");
    }

    else
    {
      mStr = "";
      sStr = ">=" + StrFormatDouble (sizeThresholds[c], "0.0000");
    }

    h1 << "\t" << mStr;
    h2 << "\t" << sStr;
  }
}  /* CreateThresholdHeaders */




DeploymentSummary*  MarineSnowReportDeployment (SipperDeploymentPtr  deployment,
                                                DataBase&            db
                                               )
{
  cout << deployment->CruiseName () << "\t" << deployment->StationName () << "\t" << deployment->DeploymentNum () << endl;


  // 1)Determines Midpoint of deploymet from Instruments table.
  KKStr  sqlStr = "Call ImagesSizeDataByDepthSipper11(" + deployment->CruiseName ().QuotedStr () + "," + deployment->StationName ().QuotedStr () + "," + deployment->DeploymentNum ().QuotedStr () + ",1, '2', 0.005, 1.2, 20.0)";

  VectorKKStr  columnNames;
  KKStrMatrixPtr results = db.QueryStatementReturnAllColumns (sqlStr.Str (), sqlStr.Len (), columnNames);
  if  (results == NULL)
  {
    cout << "No results returend" << endl;
    return NULL;
  }

  int32  numRows = results->NumRows ();
  int32  numCols = results->NumCols ();
  int32  numCountCols = numCols - 5;

  if  (numRows == 0)
  {
    delete  results;
    results = NULL;
    return  NULL;
  }

  VectorFloat  sizeThresholds;

  VectorKKStr  countHeader (numCountCols);
  for  (int32 x = 0;  x < numCountCols;  ++x)
  {
    KKStr  columnName = columnNames[5 + x];
    countHeader[x] = columnName;

    float  sizeThreshold = 0;

    if  (x == 0)
    {
      sizeThreshold = columnName.SubStrPart (1).ToFloat ();
    }

    else if  (x <= (numCountCols - 1))
    {
      int32 idx = columnName.LocateCharacter ('_');
      if  (idx > 0)
        sizeThreshold = columnName.SubStrPart (idx + 1).ToFloat ();
    }

    else if  (x == (numCountCols - 1))
    {
      sizeThreshold = columnName.SubStrPart (2).ToFloat ();
    }

    sizeThresholds.push_back (sizeThreshold);
  }

  bool  cancelFlag = false;
  InstrumentDataMeansListPtr  instData = db.InstrumentDataBinByMeterDepth (deployment->CruiseName (), deployment->StationName (), deployment->DeploymentNum (), 1.0f, cancelFlag);
  if  (!instData)
  {
    cout << "no Instrument data." << endl << endl;
    return NULL;
  }

  KKStr svnVersionStr = QUOTE (_SVNVERSION_);

  KKStr  reportRootName = deployment->CruiseName () + "-" + deployment->StationName () + "-" + deployment->DeploymentNum ();
  KKStr  reportFileRootName1 =  osAddSlash (marineSnowReportDirectory) + reportRootName + ".txt";
  ofstream  r1 (reportFileRootName1.Str ());

  r1 << "Cruise"     << "\t" << deployment->CruiseName    () << endl
     << "Station"    << "\t" << deployment->StationName   () << endl
     << "Deployment" << "\t" << deployment->DeploymentNum () << endl
     << "DateTime"   << "\t" << osGetLocalDateTime ()        << endl
     << "DataBase"   << "\t" << db.ServerDescription ()      << endl
     << "ProgName"   << "\t" << osGetProgName ()             << endl
     << "BuildTime"  << "\t" << __DATE__ << " " << __TIME__  << endl
     << "SvnVersion" << "\t" << svnVersionStr                << endl
     << "HostName"   << "\t" << osGetHostName ()             << endl
     << "UserName"   << "\t" << osGetUserName ()             << endl
     << endl
     << endl;

  KKStr  h1 (1024);
  KKStr  h2 (1024);

  KKStr  thH1 (1024);
  KKStr  thH2 (1024);
  CreateThresholdHeaders (sizeThresholds, thH1, thH2);

  h1 << ""        << "\t" << "Depth"  << "\t" << "VolumeSampled" << "\t" << "Image"     << "\t"  << "Pixel" << thH1;
  h2 << "Down/Up" << "\t" << "(m)"    << "\t" << "m^3"           << "\t" << "Abundance" << "\t"  << "Count" << thH2;

  VectorDouble  integratedAbundance (sizeThresholds.size (), 0.0);
  VectorInt32   integratedCounts    (sizeThresholds.size (), 0);
  double        totalVolumeSampled = 0.0;

  KKStr  temperatureUOM   = InstrumentData::TemperatureUnit   ();
  KKStr  salinityUOM      = InstrumentData::SalinityUnit      ();
  KKStr  denisityUOM      = InstrumentData::DensityUnit       ();
  KKStr  fluorescenceUOM  = InstrumentData::FluorescenceUnit  ();
  KKStr  oxygenUOM        = InstrumentData::OxygenUnit        ();
  KKStr  transmisivityUOM = InstrumentData::TransmisivityUnit ();
  KKStr  turbidityUOM     = InstrumentData::TurbidityUnit     ();

  h1 << "\t" << "Temperature"   << "\t" << "Salinity"  << "\t" << "Denisity"  << "\t" << "Fluorescence"  << "\t" << "Fluorescence-Sensor" << "\t" << "Oxygen"  << "\t" << "Oxygen"  << "\t" << "transmisivity"  << "\t" << "turbidity";
  h2 << "\t" << temperatureUOM  << "\t" << salinityUOM << "\t" << denisityUOM << "\t" << fluorescenceUOM << "\t" << "Volts"               << "\t" << oxygenUOM << "\t" << "umol/kg" << "\t" << transmisivityUOM << "\t" << turbidityUOM;

  r1 << endl << endl
     << "Abundance (Count/m^3)" << endl
     << endl
     << h1 << endl
     << h2 << endl;

  for  (int32 rowIdx = 0;  rowIdx < numRows;  ++rowIdx)
  {
    KKStrList&  row = (*results)[rowIdx];

    bool  downCast     = row[0].ToBool  ();
    int32  bucketIdx   = row[1].ToInt32 ();
    float  bucketDepth = row[2].ToFloat ();
    int32  imageCount  = row[3].ToInt32 ();
    int32  pixelCount  = row[4].ToInt32 ();

    VectorInt32  counts (numCountCols, 0);
    for  (int32 x = 0;  x < numCountCols;  ++x)
      counts[x] = row[5 + x].ToInt32 ();

    float  volumeSampled = 0.0f;
    InstrumentDataMeansPtr  idm = instData->LookUp (downCast, bucketDepth);
    if  (idm)
      volumeSampled = idm->volumeSampled;

    totalVolumeSampled += volumeSampled;

    KKStr  begOfLine (128);
    begOfLine << (downCast ? "Down" : "Up") << "\t" << bucketDepth << "\t" << volumeSampled << "\t" << imageCount << "\t" << pixelCount;

    r1 << begOfLine;

    for  (int32 x = 0;  x < numCountCols;  ++x)
    {
      float  density = 0.0f;
      if  (volumeSampled != 0.0)
        density = (float)counts[x] / volumeSampled;
      r1 << "\t" << density;
      integratedAbundance[x] += density;
      integratedCounts   [x] += counts[x];
    }

    if  (idm)
    {
      KKStr  instDataStr (256);

      instDataStr << "\t" << idm->temperatureMean
                  << "\t" << idm->salinityMean 
                  << "\t" << idm->denisityMean
                  << "\t" << idm->fluorescenceMean
                  << "\t" << idm->fluorescenceSensorMean
                  << "\t" << idm->oxygenMean
                  << "\t" << idm->Oxygen_molPerKg_Mean ()
                  << "\t" << idm->transmisivityMean 
                  << "\t" << idm->turbidityMean;

      r1 << instDataStr;
    }

    r1 << endl;
  }

  r1 << endl;

  r1 << "" << "\t" << "" << "\t" << "" << "\t" << "Integrated Abundance" << "\t" << "";
  for  (uint32 x = 0;  x < integratedAbundance.size ();  ++x)
  {
    r1 << "\t" << integratedAbundance[x];
  }
  r1 << endl;

  r1 << "" << "\t" << "" << "\t" << "" << "\t" << "Integrated log10(Abundance)" << "\t" << "";
  for  (uint32 x = 0;  x < integratedAbundance.size ();  ++x)
  {
    double  zed = integratedAbundance[x] + 1.0;
    double  zed2 = 0.0;
    if  (zed != 0.0)
       zed2 = log10 (zed);
    r1 << "\t" << zed2;
  }
  r1 << endl;


  r1 << endl << endl << endl
     << "Count" << endl
     << endl;

  r1 << h1 << endl;
  r1 << h2 << endl;

  for  (int32 rowIdx = 0;  rowIdx < numRows;  ++rowIdx)
  {
    KKStrList&  row = (*results)[rowIdx];

    bool  downCast     = row[0].ToBool  ();
    int32  bucketIdx   = row[1].ToInt32 ();
    float  bucketDepth = row[2].ToFloat ();
    int32  imageCount  = row[3].ToInt32 ();
    int32  pixelCount  = row[4].ToInt32 ();

    VectorInt32  counts (numCountCols, 0);
    for  (int32 x = 0;  x < numCountCols;  ++x)
      counts[x] = row[5 + x].ToInt32 ();

    float  volumeSampled = 0.0f;
    InstrumentDataMeansPtr  idm = instData->LookUp (downCast, bucketDepth);
    if  (idm)
      volumeSampled = idm->volumeSampled;

    KKStr  begOfLine (128);
    begOfLine << (downCast ? "Down" : "Up") << "\t" << bucketDepth << "\t" << volumeSampled << "\t" << imageCount << "\t" << pixelCount;

    r1 << begOfLine;

    for  (int32 x = 0;  x < numCountCols;  ++x)
      r1 << "\t" << counts[x];

    if  (idm)
    {
      KKStr  instDataStr (256);

      instDataStr << "\t" << idm->temperatureMean
                  << "\t" << idm->salinityMean 
                  << "\t" << idm->denisityMean
                  << "\t" << idm->fluorescenceMean
                  << "\t" << idm->fluorescenceSensorMean
                  << "\t" << idm->oxygenMean
                  << "\t" << idm->Oxygen_molPerKg_Mean ()
                  << "\t" << idm->transmisivityMean 
                  << "\t" << idm->turbidityMean;

      r1 << instDataStr;
    }

    r1 << endl;
  }

  r1 << "" << "\t" << "" << "\t" << "" << "\t" << "Integrated Counts" << "\t" << "";
  for  (uint32 x = 0;  x < integratedCounts.size ();  ++x)
    r1 << "\t" << integratedCounts[x];

  delete  instData;  instData = NULL;
  delete  results;   results  = NULL;

  return  new DeploymentSummary (deployment->CruiseName (), deployment->StationName (), deployment->DeploymentNum (), totalVolumeSampled, sizeThresholds, integratedAbundance, integratedCounts);
}  /* MarineSnowReportDeployment */




void  PrintSummaryReports (DataBasePtr                  db,
                           vector<DeploymentSummary*>&  summaries
                          )
{
  uint32  x = 0;

  KKStr  sumFileName = osAddSlash (marineSnowReportDirectory) + "Summary.txt";
  ofstream  r (sumFileName.Str ());

  KKStr svnVersionStr = QUOTE (_SVNVERSION_);

  r  << "DateTime"   << "\t" << osGetLocalDateTime ()        << endl
     << "DataBase"   << "\t" << db->ServerDescription ()     << endl
     << "ProgName"   << "\t" << osGetProgName ()             << endl
     << "BuildTime"  << "\t" << __DATE__ << " " << __TIME__  << endl
     << "SvnVersion" << "\t" << svnVersionStr                << endl
     << "HostName"   << "\t" << osGetHostName ()             << endl
     << "UserName"   << "\t" << osGetUserName ()             << endl
     << endl
     << endl
     << endl;

  r << "Summary  Integrated Counts" << endl
    << endl;

  vector<DeploymentSummary*>::iterator  idx;

  idx = summaries.begin ();

  VectorFloat  sizeThresholds = (*idx)->sizeThresholds;

  KKStr h1 (1024);
  KKStr h2 (1024);

  CreateThresholdHeaders (sizeThresholds, h1, h2);

  r << ""       << "\t" << ""        << "\t" << ""           << "\t" << "VolumeSampled" << h1 << endl;
  r << "Cruise" << "\t" << "Station" << "\t" << "Deployment" << "\t" << "m^3"           << h2 << endl;
  for  (idx = summaries.begin ();  idx != summaries.end ();  ++idx)
  {
    DeploymentSummary*  ds = *idx;

    r << ds->cruiseName << "\t" << ds->stationName << "\t" << ds->deploymentNum << "\t" << ds->volumeSampled;

    for  (x = 0;  x < ds->integratedCounts.size ();  ++x)
    {
      r << "\t" << ds->integratedCounts[x];
    }
    r << endl;
  }
  r << endl << endl << endl << endl;



  r << "Summary  Integrated Abundance" << endl
    << endl;

  r << ""       << "\t" << ""        << "\t" << ""           << "\t" << "VolumeSampled" << h1 << endl;
  r << "Cruise" << "\t" << "Station" << "\t" << "Deployment" << "\t" << "m^3"           << h2 << endl;
  for  (idx = summaries.begin ();  idx != summaries.end ();  ++idx)
  {
    DeploymentSummary*  ds = *idx;

    r << ds->cruiseName << "\t" << ds->stationName << "\t" << ds->deploymentNum << "\t" << ds->volumeSampled;

    for  (x = 0;  x < ds->sizeThresholds.size ();  ++x)
    {
      r << "\t" << ds->integratedAbundance[x];
    }
    r << endl;
  }
  r << endl << endl << endl << endl;




  r << "Summary  Integrated log10(Abundance)" << endl
    << endl;

  r << ""       << "\t" << ""        << "\t" << ""           << "\t" << "VolumeSampled" << h1 << endl;
  r << "Cruise" << "\t" << "Station" << "\t" << "Deployment" << "\t" << "m^3"           << h1 << endl;
  for  (idx = summaries.begin ();  idx != summaries.end ();  ++idx)
  {
    DeploymentSummary*  ds = *idx;

    r << ds->cruiseName << "\t" << ds->stationName << "\t" << ds->deploymentNum << "\t" << ds->volumeSampled;

    for  (x = 0;  x < ds->sizeThresholds.size ();  ++x)
    {
      double  zed = (double)(ds->integratedAbundance[x] + 1.0);
      double log10AbundanceSize = 0.0;
      if (zed != 0.0)
        log10AbundanceSize = log10 (zed);
      r << "\t" << log10AbundanceSize;
    }
    r << endl;
  }
  r << endl << endl << endl << endl;
}  /* PrintSummaryReports */




void  MarineSnowReport ()
{
  RunLog  runLog;
  DataBasePtr  db = new DataBase (runLog);

  marineSnowReportDirectory = osAddSlash (osAddSlash (SipperVariables::PicesReportDir ()) + "MarineSnowReports") + KKU::osGetLocalDateTime ().Date ().YYYYMMDD ();
  //marineSnowReportDirectory = "D:\\Users\\kkramer\\DropBox\\Dropbox\\USF_OilSpillGroup\\MarineSnow_" + KKU::osGetLocalDateTime ().Date ().YYYYMMDD ();
  KKU::osCreateDirectoryPath (marineSnowReportDirectory);

  vector<DeploymentSummary*>  summaries;

  SipperDeploymentListPtr  deployments = db->SipperDeploymentLoad ("","");

  int  loopCount = 0;

  SipperDeploymentList::const_iterator  idx;
  for  (idx = deployments->begin ();  idx != deployments->end ();  ++idx)
  {
    SipperDeploymentPtr  deployment = *idx;

    if  ((deployment->CruiseName ().EqualIgnoreCase ("ETP2007"))  ||
         (deployment->CruiseName ().EqualIgnoreCase ("ETP2008"))  ||
         (deployment->CruiseName ().SubStrPart (0, 2).EqualIgnoreCase ("HRS"))
        )
     continue;
         
    DeploymentSummary*  sumary = MarineSnowReportDeployment (*idx, *db);
    if  (sumary)
      summaries.push_back (sumary);
    ++loopCount;
  }

  PrintSummaryReports (db, summaries);

  delete  db;
  db = NULL;
}  /* MarineSnowReport */









int  main (int    argc, 
           char** argv
          )
{

  #if  defined (WIN32)  &&  defined (_DEBUG)  &&  !defined(_NO_MEMORY_LEAK_CHECK_)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
  #endif

  MarineSnowReport ();

  return 0;
}

