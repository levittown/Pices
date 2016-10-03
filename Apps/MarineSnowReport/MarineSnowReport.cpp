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


#include "KKBaseTypes.h"
#include "CmdLineExpander.h"
using namespace  KKB;


#include "DataBase.h"
#include "Instrument.h"
#include "InstrumentDataFileManager.h"
#include "InstrumentDataManager.h"
#include "PicesVariables.h"
#include "SipperBuff.h"
#include "SipperCruise.h"
#include "SipperDeployment.h"
#include "SipperStation.h"
using namespace  MLL;



//#include "SvnVersion.h"
#include "MarineSnowReport.h"


#define Q(x) #x
#define QUOTE(x) Q(x)




//Aug 2010,   "WB1008"
//Sept 2011,  "WB0911"
//Aug 2012,   "WB0812"
//Aug 2013,   "WB0813"
//Aug 2014,   "WB0814"




KKStr  marineSnowReportDirectory = "D:\\Users\\kkramer\\DropBox\\Dropbox\\USF_OilSpillGroup\\MarineSnowPaper\\Data\\20160619_Area";

enum class CastType : char { null = 0, Up = 1, Down = 2, Combined = 3};

KKStr  CastTypeToStr(CastType c)
{
  if  (c == CastType::Combined)   return "Combined";
  else if  (c == CastType::Up)    return "Up";
  else if  (c == CastType::Down)  return "Down";
  return "";
}


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
  for  (kkuint32 c = 0;  c < sizeThresholds.size ();  ++c)
  {
    KKStr  mStr = "";
    KKStr  sStr = "";
    if (c == 0)
    {
      mStr = "";
      sStr = ">=" + StrFormatDouble (sizeThresholds[c], "0.0000");
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



const char*  UnitLabel(const KKStr& statistic) 
{
  if       (statistic == "0")  return "mm^2";
  else if  (statistic == "1")  return "mm"; 
  else if  (statistic == "2")  return "mm^3";
  else return "UnKnown";
}


DeploymentSummary*  MarineSnowReportDeployment (SipperDeploymentPtr  deployment,
                                                DataBase&            db,
                                                const KKStr&         statistic,
                                                const KKStr&         statisticStr,
                                                CastType             cast,
                                                RunLog&              runLog
                                               )
{
  cout << deployment->CruiseName () << "\t" << deployment->StationName () << "\t" << deployment->DeploymentNum () << endl;

  DeploymentSummary*  result = NULL;

  MLClassListPtr  allClasses = db.MLClassLoadList ();

  ImageSizeDistributionPtr  downCast = NULL;
  ImageSizeDistributionPtr  upCast   = NULL;

  ImageSizeDistributionPtr  totalCast = NULL;

  MLClassPtr  detritusClass = allClasses->LookUpByName ("Detritus");
  MLClassListPtr  classes = detritusClass->BuildListOfDecendents (detritusClass);
  if  (classes->PtrToIdx (detritusClass) < 0)
    classes->AddMLClass (detritusClass);

  delete  allClasses;
  allClasses = NULL;

  char  statisticCode = '0';
  double  startValue = 0.005;
  double  growthRate = 1.2;
  double  endValue   = 170.0;

  if  (statistic == "0")
  {
    statisticCode = '0';
    //startValue    = 0.00785398;
    startValue    = 0.0506275743;  // Equivelent to 100 pixes, scanRate = 24950.0, flowRate = 0.5f, scanLineWidthPixels = 3800.0, scanLineWidthmm = 96.0f;  see 'ExtractionParms::ComputeAreaFromPixelsUsingDefaults'
    growthRate    = 1.21; // k = 2^(1/6)
    endValue      = 1000.0;
  }

  else if  (statistic == "1")
  {
    statisticCode = '1';
    startValue    = 0.10;
    growthRate    = 1.059463094;  // k = 2^(1/12)
    endValue      = 100.0;
  }

  else if  (statistic == "2")
  {
    statisticCode = '2';
    startValue    = 0.001;
    growthRate    = 1.189207115;  // k = 2^(1/4)
    endValue      = 10000.0;
  }


  MLClassList::const_iterator  idx;
  for  (idx = classes->begin ();  idx != classes->end ();  ++idx)
  {
    MLClassPtr c = *idx;
    ImageSizeDistributionPtr  downCast = NULL;
    ImageSizeDistributionPtr  upCast   = NULL;
    db.ImagesSizeDistributionByDepth (deployment->CruiseName (), deployment->StationName (), deployment->DeploymentNum (), c->Name (), 
                                      0.0f, 1.0f, statisticCode, startValue, growthRate, endValue, 
                                      downCast, upCast
                                    );

    if  (downCast  &&  ((cast == CastType::Down) ||  (cast == CastType::Combined)))
    {
      if  (!totalCast)
      {
        totalCast = new ImageSizeDistribution (downCast->DepthBinSize (), downCast->InitialValue (), downCast->GrowthRate (), downCast->EndValue (), downCast->SizeStartValues (), downCast->SizeStartValues (), runLog);
      }
      totalCast ->AddIn (*downCast, runLog);
    }

    if  (upCast  &&  ((cast == CastType::Up) ||  (cast == CastType::Combined)))
    {
      if  (!totalCast)
      {
        totalCast = new ImageSizeDistribution (upCast->DepthBinSize (), upCast->InitialValue (), upCast->GrowthRate (), upCast->EndValue (), upCast->SizeStartValues (), upCast->SizeStartValues (), runLog);
      }
      totalCast ->AddIn (*upCast, runLog);
    }


    delete  downCast;  downCast = NULL;
    delete  upCast;    upCast   = NULL;
  }
  
  if  (!totalCast)
  {
    runLog.Level (-1) << "MarineSnowReportDeployment    ***ERROR***    No results returend" << endl;
    return NULL;
  }
  
  kkint32  numRows = totalCast->NumDepthBins ();         // numRows = results->NumRows ();
  //kkint32  numCols =                                       // numCols = results->NumCols ();
  kkint32  numCountCols = totalCast->NumSizeBuckets ();  // numCountCols = numCols - 6;

  if  (numRows == 0)
  {
    delete  totalCast;
    totalCast = NULL;
    return  NULL;
  }

  VectorFloat  sizeThresholds = totalCast->SizeStartValues ();

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

  DataBase::DeploymentTimeResult*  deploymentTimes = db.InstrumentDataGetDeploymentTimes (deployment);
  if  (deploymentTimes == NULL)
  {
    KKStr msg = "MarineSnowReportDeployment  ***ERROR***   No DeploymentTime results returnded   Deployment: " +
                deployment->CruiseName() + "-" + deployment->StationName() + "-" + deployment->DeploymentNum();
    runLog.Level(-1) << endl << msg << endl << endl;
    r1 << endl << msg << endl << endl;
  }
  else
  {
    r1 << "Cruise"                   << "\t" << deployment->CruiseName    () << endl
       << "Station"                  << "\t" << deployment->StationName   () << endl
       << "Deployment"               << "\t" << deployment->DeploymentNum () << endl
       << "  CTD Time (Start-End)*1" << "\t" << deploymentTimes->ctdDateTimeStart << "\t" << deploymentTimes->ctdDateTimeEnd << endl
       << "  UTC Time (Start-End)"   << "\t" << deploymentTimes->utcDateTimeStart << "\t" << deploymentTimes->utcDateTimeEnd << endl
       << "DateTime Report Ran"      << "\t" << osGetLocalDateTime ()        << endl
       << "DataBase"                 << "\t" << db.ServerDescription ()      << endl
       << "ProgName"                 << "\t" << osGetProgName ()             << endl
       << "BuildTime"                << "\t" << __DATE__ << " " << __TIME__  << endl
       << "HostName"                 << "\t" << osGetHostName ()             << endl
       << "UserName"                 << "\t" << osGetUserName ()             << endl
       << "StatisticCode"            << "\t" << statistic << "\t" << statisticStr << endl
       << endl;

    r1 << "*1 Deployment Start and End  times  determined from CTD-Clock  when first and last times depth >= 0.5 meters." << endl;
    r1 << endl << endl;

    r1 << "\t\t\t\t\t" << "Particle size bins (" << UnitLabel (statistic) << ")" << endl;

    KKStr  h1 (1024);
    KKStr  h2 (1024);

    KKStr  thH1 (1024);
    KKStr  thH2 (1024);
    CreateThresholdHeaders (sizeThresholds, thH1, thH2);

    h1 << ""        << "\t" << "Depth"  << "\t" << "VolumeSampled" << "\t" << "Image"     << "\t"  << "Pixel" << "\t" << "Filled" << thH1;
    h2 << "Down/Up" << "\t" << "(m)"    << "\t" << "m^3"           << "\t" << "Abundance" << "\t"  << "Count" << "\t" << "Area"   << thH2;

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

    h1 << "\t" << "Temperature"   << "\t" << "Salinity"  << "\t" << "Density"   << "\t" << "Fluorescence"  << "\t" << "Fluorescence-Sensor" << "\t" << "Oxygen"  << "\t" << "Oxygen"  << "\t" << "Transmissionmeter"  << "\t" << "turbidity";
    h2 << "\t" << temperatureUOM  << "\t" << salinityUOM << "\t" << denisityUOM << "\t" << fluorescenceUOM << "\t" << "Volts"               << "\t" << oxygenUOM << "\t" << "umol/kg" << "\t" << transmisivityUOM     << "\t" << turbidityUOM;

    r1 << endl << endl
       << "Abundance (Count/m^3)" << endl
       << endl
       << h1 << endl
       << h2 << endl;

    for  (kkint32 rowIdx = 0;  rowIdx < numRows;  ++rowIdx)
    {

      ImageSizeDistributionRowPtr  row = totalCast->GetDepthBin ((kkuint32)rowIdx);
      if  (!row)
        continue;

      KKStr  castStr = CastTypeToStr (cast);

      kkint32  bucketIdx   = rowIdx;
      float    bucketDepth = row->Depth ();
      kkint32  imageCount  = (kkint32)row->ImageCount ();
      kkint32  pixelCount  = row->TotalPixels ();
      kkint32  filledArea  = row->TotalFilledArea ();

      const VectorUint32&  counts = row->Distribution ();

      float  volumeSampled = 0.0f;


      InstrumentDataMeansPtr  idm = NULL;
      InstrumentDataMeansPtr  upIdm = NULL;
      InstrumentDataMeansPtr  downIdm = NULL;
      InstrumentDataMeansPtr  combinedIdm = NULL;

      upIdm = instData->LookUp (false, bucketDepth);
      downIdm = instData->LookUp (true, bucketDepth);

      if  (upIdm) {
        if  (downIdm)
          combinedIdm = upIdm->Merge(downIdm);
        else
          combinedIdm = new InstrumentDataMeans(*upIdm);
      }
      else if  (downIdm) {
        combinedIdm = new InstrumentDataMeans(*downIdm);
      }

      if  (cast == CastType::Up)
        idm = upIdm;
      else if  (cast == CastType::Down)
        idm = downIdm;
      else  if  (cast == CastType::Combined)
        idm = combinedIdm;

      if  (idm)
        volumeSampled = idm->volumeSampled;

      totalVolumeSampled += volumeSampled;

      KKStr  begOfLine (128);
      begOfLine << castStr << "\t" << bucketDepth << "\t" << volumeSampled << "\t" << imageCount << "\t" << pixelCount << "\t" << filledArea;

      r1 << begOfLine;

      for  (kkint32 x = 0;  x < numCountCols;  ++x)
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

      delete  combinedIdm;
      combinedIdm = NULL;

      r1 << endl;
    }

    r1 << endl;

    r1 << "" << "\t" << "" << "\t" << "" << "\t" << "" << "\t" << "Integrated Abundance" << "\t" << "";
    for  (kkuint32 x = 0;  x < integratedAbundance.size ();  ++x)
    {
      r1 << "\t" << integratedAbundance[x];
    }
    r1 << endl;

    r1 << "" << "\t" << "" << "\t" << "" << "\t" << "" << "\t" << "Integrated log10(Abundance)" << "\t" << "";
    for  (kkuint32 x = 0;  x < integratedAbundance.size ();  ++x)
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

    for  (kkint32 rowIdx = 0;  rowIdx < numRows;  ++rowIdx)
    {
      ImageSizeDistributionRowPtr  row = totalCast->GetDepthBin ((kkuint32)rowIdx);
      if  (!row)
        continue;

      bool     downCast    = true;
      kkint32  bucketIdx   = rowIdx;
      float    bucketDepth = row->Depth ();
      kkint32  imageCount  = (kkint32)row->ImageCount ();
      kkint32  pixelCount  = row->TotalPixels ();
      kkint32  filledArea  = row->TotalFilledArea ();

      const VectorUint32&  counts = row->Distribution ();

      float  volumeSampled = 0.0f;

      InstrumentDataMeansPtr  idm = NULL;
      InstrumentDataMeansPtr  upIdm = NULL;
      InstrumentDataMeansPtr  downIdm = NULL;
      InstrumentDataMeansPtr  combinedIdm = NULL;

      upIdm = instData->LookUp (false, bucketDepth);
      downIdm = instData->LookUp (true, bucketDepth);

      if  (upIdm) {
        if  (downIdm)
          combinedIdm = upIdm->Merge(downIdm);
        else
          combinedIdm = new InstrumentDataMeans(*upIdm);
      }
      else if  (downIdm) {
        combinedIdm = new InstrumentDataMeans(*downIdm);
      }

      if  (cast == CastType::Up)
        idm = upIdm;
      else if  (cast == CastType::Down)
        idm = downIdm;
      else  if  (cast == CastType::Combined)
        idm = combinedIdm;

      if  (idm)
        volumeSampled = idm->volumeSampled;

      KKStr  begOfLine (128);
      begOfLine << CastTypeToStr (cast) << "\t" << bucketDepth << "\t" << volumeSampled << "\t" << imageCount << "\t" << pixelCount << "\t" << filledArea;

      r1 << begOfLine;

      for  (kkint32 x = 0;  x < numCountCols;  ++x)
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

      delete  combinedIdm;
      combinedIdm = NULL;
    }

    r1 << "" << "\t" << "" << "\t" << "" << "\t" << "" << "\t" <<  "" << "\t" <<  "" << "\t" <<  "" << "\t" << "Integrated Counts" << "\t" << "";
    for  (kkuint32 x = 0;  x < integratedCounts.size ();  ++x)
      r1 << "\t" << integratedCounts[x];

    result = new DeploymentSummary (deployment->CruiseName (), deployment->StationName (), deployment->DeploymentNum (), 
                                    totalVolumeSampled, 
                                    sizeThresholds, 
                                    integratedAbundance, 
                                    integratedCounts);
  }

  delete  instData;    instData = NULL;
  delete  totalCast;   totalCast  = NULL;

  return  result;
}  /* MarineSnowReportDeployment */



void  PrintSummaryReports (DataBasePtr                  db,
                           vector<DeploymentSummary*>&  summaries,
                           const KKStr&                 statistic,
                           const KKStr&                 statisticStr
                          )
{
  kkuint32  x = 0;

  sort (summaries.begin (), summaries.end (), [](DeploymentSummary*  left,  DeploymentSummary*  right) -> bool
    {
      if  (left->stationName < right->stationName)
        return true;
      else if  (left->stationName > right->stationName)
        return false;
      else
        return  (left->cruiseName < right->cruiseName);
    });

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
     << "Statistic"  << "\t" << statistic << "\t" << statisticStr << endl
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
  r << "Cruise" << "\t" << "Station" << "\t" << "Deployment" << "\t" << "m^3"           << h2 << endl;
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




void   CleanUpSummaries (vector<DeploymentSummary*>&  summaries)
{
  vector<DeploymentSummary*>::iterator  idx;
  for  (idx = summaries.begin ();  idx != summaries.end ();  ++idx)
  {
    DeploymentSummary* s = *idx;
    delete  s;
    s = NULL;
  }
  summaries.clear ();
}




void  MarineSnowReport (const KKStr&  statistic)
{
  RunLog  runLog;
  DataBasePtr  db = new DataBase (runLog);

  KKStr  statisticStr = "UnKnown";
  if  (statistic == "0")
    statisticStr = "Area";
  else if  (statistic == "1")
    statisticStr = "Diameter";
  else
    statisticStr = "Volume";

  marineSnowReportDirectory = osAddSlash (osAddSlash (PicesVariables::ReportDir ()) + "MarineSnowReports") + KKB::osGetLocalDateTime ().Date ().YYYYMMDD () + "_" + statisticStr;
  //marineSnowReportDirectory = "D:\\Users\\kkramer\\DropBox\\Dropbox\\USF_OilSpillGroup\\MarineSnow_" + KKB::osGetLocalDateTime ().Date ().YYYYMMDD ();
  KKB::osCreateDirectoryPath (marineSnowReportDirectory);

  vector<DeploymentSummary*>  summaries;

  SipperDeploymentListPtr  deployments = db->SipperDeploymentLoad ("","");
  int  loopCount = 0;

  SipperDeploymentList::const_iterator  idx;
  for  (idx = deployments->begin ();  idx != deployments->end ();  ++idx)
  {
    SipperDeploymentPtr  deployment = *idx;

//    if  (deployment->StationName ().EqualIgnoreCase ("DWH") ||
//         deployment->StationName ().StartsWith ("DSH")  ||
 //        deployment->StationName ().StartsWith ("PCB")
//        )
//    if  (deployment->CruiseName ().EqualIgnoreCase ("WB1008") &&
//         deployment->StationName ().StartsWith ("PCB01")
//        )
//    {
//      runLog.Level (10) << "MarineSnowReport    Found Cruise: " << deployment->CruiseName ()  << endl;
//    }
//    else
//    {
//      continue;
//    }

    DeploymentSummary*  sumary = MarineSnowReportDeployment (*idx, *db, statistic, statisticStr, CastType::Combined,  runLog);
    if  (sumary)
      summaries.push_back (sumary);
    ++loopCount;
  }

  PrintSummaryReports (db, summaries, statistic, statisticStr);


  CleanUpSummaries (summaries);

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

  MarineSnowReport ("0");

  return 0;
}

