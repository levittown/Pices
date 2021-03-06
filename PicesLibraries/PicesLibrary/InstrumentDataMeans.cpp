#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <memory>

#include  <string>
#include  <iostream>
#include  <fstream>
#include  <vector>

#include  "MemoryDebug.h"

using namespace std;

#include  "KKBaseTypes.h"
#include  "DateTime.h"
using namespace KKB;



#include "InstrumentDataMeans.h"
#include "PicesVariables.h"
using namespace MLL;



InstrumentDataMeans::InstrumentDataMeans ():
      downCast               (true),
      depthBin               (0),
      binDepth               (0.0f),
      scanLines              (0),
      volumeSampled          (0.0f),
      temperatureMean        (0.0f),
      salinityMean           (0.0f),
      denisityMean           (0.0f),
      fluorescenceMean       (0.0f),
      fluorescenceSensorMean (0.0f),
      oxygenMean             (0.0f),
      depthMean              (0.0f),
      transmisivityMean      (0.0f),
      turbidityMean          (0.0f),
      cdomFluorescenceMean   (0.0f)
{}


InstrumentDataMeans::InstrumentDataMeans (bool    _downCast,
                                          kkint32 _depthBin,
                                          float   _binDepth,
                                          kkint32 _scanLines,
                                          float   _volumeSampled,
                                          float   _temperatureMean,
                                          float   _salinityMean,
                                          float   _denisityMean,
                                          float   _fluorescenceMean,
                                          float   _fluorescenceSensorMean,
                                          float   _oxygenMean,
                                          float   _depthMean,
                                          float   _transmisivityMean,
                                          float   _turbidityMean,
                                          float   _cdomFluorescenceMean
                                         ):
      downCast               (_downCast),
      depthBin               (_depthBin),
      binDepth               (_binDepth),
      scanLines              (_scanLines),
      volumeSampled          (_volumeSampled),
      temperatureMean        (_temperatureMean),
      salinityMean           (_salinityMean),
      denisityMean           (_denisityMean),
      fluorescenceMean       (_fluorescenceMean),
      fluorescenceSensorMean (_fluorescenceSensorMean),
      oxygenMean             (_oxygenMean),
      depthMean              (_depthMean),
      transmisivityMean      (_transmisivityMean),
      turbidityMean          (_turbidityMean),
      cdomFluorescenceMean   (_cdomFluorescenceMean)
{
}


InstrumentDataMeans*  InstrumentDataMeans::Merge (InstrumentDataMeans*  other)  const
{
  float  lSL = (float)scanLines;
  float  rSL = (float)(other->scanLines);
  float  tSL = (float)(lSL + rSL);

  InstrumentDataMeans*  result = new InstrumentDataMeans ();

  result->binDepth               = (binDepth               * lSL + other->binDepth               * rSL) / tSL;
  result->volumeSampled          = (volumeSampled          * lSL + other->volumeSampled          * rSL) / tSL;
  result->temperatureMean        = (temperatureMean        * lSL + other->temperatureMean        * rSL) / tSL;
  result->salinityMean           = (salinityMean           * lSL + other->salinityMean           * rSL) / tSL;
  result->denisityMean           = (denisityMean           * lSL + other->denisityMean           * rSL) / tSL;
  result->fluorescenceMean       = (fluorescenceMean       * lSL + other->fluorescenceMean       * rSL) / tSL;
  result->fluorescenceSensorMean = (fluorescenceSensorMean * lSL + other->fluorescenceSensorMean * rSL) / tSL;
  result->oxygenMean             = (oxygenMean             * lSL + other->oxygenMean             * rSL) / tSL;
  result->depthMean              = (depthMean              * lSL + other->depthMean              * rSL) / tSL;
  result->transmisivityMean      = (transmisivityMean      * lSL + other->transmisivityMean      * rSL) / tSL;
  result->turbidityMean          = (turbidityMean          * lSL + other->turbidityMean          * rSL) / tSL;
  result->cdomFluorescenceMean   = (cdomFluorescenceMean   * lSL + other->cdomFluorescenceMean   * rSL) / tSL;
  return  result;
}



InstrumentDataMeansList::InstrumentDataMeansList (bool _owner):
    KKQueue<InstrumentDataMeans> (_owner)
{

}



InstrumentDataMeansList::InstrumentDataMeansList (const InstrumentDataMeansList& data):
    KKQueue<InstrumentDataMeans> (data)
{
}



InstrumentDataMeansPtr  InstrumentDataMeansList::LookUp (bool downCast, float  depth)
{
  InstrumentDataMeansList::iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    InstrumentDataMeansPtr  idm = *idx;
    if  ((idm->downCast == downCast)  &&  (idm->depthBin == depth))
      return  idm;
  }

  return NULL;
}  /* LookUp */



