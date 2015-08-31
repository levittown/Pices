#if  !defined(_INSTRUMENTDATAMEANS_)
#define  _INSTRUMENTDATAMEANS

#include <map>

#include "KKBaseTypes.h"
#include "DateTime.h"
#include "GoalKeeper.h"
using namespace KKB;


namespace MLL
{
  class  InstrumentDataMeans
  {
  public:
    typedef  KKB::kkint32  kkint32;

    InstrumentDataMeans ();

    InstrumentDataMeans (bool   _downCast,
                         kkint32  _depthBin,
                         float  _binDepth,
                         kkint32  _scanLines,
                         float  _volumeSampled,
                         float  _temperatureMean,
                         float  _salinityMean,
                         float  _denisityMean,
                         float  _fluorescenceMean,
                         float  _fluorescenceSensorMean,
                         float  _oxygenMean,
                         float  _depthMean,
                         float  _transmisivityMean,
                         float  _turbidityMean,
                         float  _cdomFluorescenceMean
                        );

    float  Oxygen_molPerKg_Mean () const  {return  (oxygenMean * 44.64f) / 1.027f;}
        
    bool   downCast;
    kkint32  depthBin;
    float  binDepth;
    kkint32  scanLines;
    float  volumeSampled;
    float  temperatureMean;
    float  salinityMean;
    float  denisityMean;
    float  fluorescenceMean;
    float  fluorescenceSensorMean;
    float  oxygenMean;
    float  depthMean;
    float  transmisivityMean;
    float  turbidityMean;
    float  cdomFluorescenceMean;
  };

  typedef  InstrumentDataMeans*  InstrumentDataMeansPtr;



  class  InstrumentDataMeansList: public KKQueue<InstrumentDataMeans>
  {
  public:
    InstrumentDataMeansList (bool _owner);
    InstrumentDataMeansList (const InstrumentDataMeansList& data);

    InstrumentDataMeansPtr  LookUp (bool downCast, float  depth);

  };

  typedef  InstrumentDataMeansList*  InstrumentDataMeansListPtr;


}  /* MLL */
  
  
#endif

