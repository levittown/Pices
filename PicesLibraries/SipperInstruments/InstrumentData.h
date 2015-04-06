#if  !defined(_INSTRUMENTDATA_)
#define  _INSTRUMENTDATA_

#include <map>
#include "KKBaseTypes.h"
#include "DateTime.h"
#include "GoalKeeper.h"
#include "KKStrParser.h"
using namespace KKB;

#include "CTD_Plus_Data.h"


namespace SipperHardware
{
  class InstrumentData
  {
  public:
    typedef  KKB::kkint32  kkint32;
    typedef  KKB::kkuint32 kkuint32;
    typedef  KKB::kkint64  kkint64;
    typedef  KKB::kkuint64 kkuint64;


    InstrumentData ();
  
    InstrumentData (const InstrumentData&   id);

    InstrumentData (      KKStrParser&  line, 
                    const VectorInt&    fieldIndirections
                   );

    InstrumentData (kkuint32              _scanLine,
                    kkuint32              _byteOffset,
                    const KKB::DateTime&  _ctdDate,
                    char                  _activeBattery,
                    double                _latitude,
                    double                _longitude,
                    float                 _ctdBattery,
                    float                 _conductivity,
                    float                 _density,
                    float                 _depth,
                    float                 _flowRate1,
                    float                 _flowRate2,
                    float                 _fluorescence,
                    float                 _fluorescenceSensor,
                    float                 _cdomFluorescence,
                    float                 _cdomFluorescenceSensor,
                    float                 _illumination,
                    float                 _oxygen,
                    float                 _oxygenSensor,
                    float                 _pressure,
                    float                 _recordRate,
                    float                 _salinity,
                    float                 _soundVelocity,
                    float                 _temperature,
                    float                 _transmisivity,
                    float                 _transmisivitySensor,
                    float                 _turbidity,
                    float                 _turbiditySensor,
                    float                 _pitch,
                    float                 _roll,
                    float                 _bat1Level,
                    float                 _bat2Level,
                    float                 _bat3Level,
                    float                 _bat4Level,
                    kkuint16              _cropLeft,
                    kkuint16              _cropRight,
                    kkuint16              _activeColumns
                   );

    ~InstrumentData ();

    static  VectorIntPtr  CreateFieldIndirectionVectorFromTabDelStr (KKStrParser& s);
    static  VectorIntPtr  CreateDefaultFieldIndirectionList ();
    static  const KKStr&  FieldName                (kkint32 fieldIndex);
    static  const KKStr&  ShortFieldName           (kkint32 fieldIndex);
    static  const KKStr&  FieldUnitOfMeasure       (kkint32 fieldIndex);
    static  float         FieldMinValue            (kkint32 fieldIndex);
    static  const KKStr&  FieldFormatStr           (kkint32 fieldIndex);

    static  kkint32       GetFieldNum              (const KKStr& s);
    static  kkint32       GetFieldNumFromShortName (const KKStr&  s);
    static  kkint32       NumFields   ();



    static  kkint32 CurrentFileVersion ()  {return 13;}

    KKStr   FieldToStr (kkint32  fieldIndex)  const;  // 'fieldIndex' = Same number returned by 'GetFieldNum'

    KKB::kkuint64  ByteOffset  ()  const  {return  byteOffset;}

    const KKB::DateTime&  CtdDate ()  const  {return  ctdDate;}
 
    kkint32  ActiveBattery         ()  const  {return  activeBattery;                 }
    kkuint16 ActiveColumns         ()  const  {return  activeColumns;                 }
    float    Bat1Level             ()  const  {return  data[bat1LevelIndex          ];}
    float    Bat2Level             ()  const  {return  data[bat2LevelIndex          ];}
    float    Bat3Level             ()  const  {return  data[bat3LevelIndex          ];}
    float    Bat4Level             ()  const  {return  data[bat4LevelIndex          ];}
    float    CTDBattery            ()  const  {return  data[ctdBatteryIndex         ];}
    float    Conductivity          ()  const  {return  data[ConductivityIndex       ];}
    kkuint16 CropLeft              ()  const  {return  cropLeft;                      }
    kkuint16 CropRight             ()  const  {return  cropRight;                     }
    float    Depth                 ()  const  {return  data[depthIndex              ];}
    float    Density               ()  const  {return  data[densityIndex            ];}
    float    FlowRate1             ()  const  {return  data[flowRate1Index          ];}
    float    FlowRate2             ()  const  {return  data[flowRate2Index          ];}
    float    Fluorescence          ()  const  {return  data[fluorescenceIndex       ];}
    float    FluorescenceSensor    ()  const  {return  data[fluorescenceSensorIndex ];}
    float    CdomFluorescence      ()  const  {return  data[cdomFluorescenceIndex   ];}
    float    CdomFluorescenceSensor()  const  {return  data[cdomFluorescenceSensorIndex ];}
    float    Illumination          ()  const  {return  data[illuminationIndex       ];}
    float    Oxygen                ()  const  {return  data[oxygenIndex             ];}
    float    OxygenSensor          ()  const  {return  data[oxygenSensorIndex       ];}
    double   Latitude              ()  const  {return  latitude;                      }
    double   Longitude             ()  const  {return  longitude;                     }
    float    Pressure              ()  const  {return  data[pressureIndex           ];}
    float    RecordRate            ()  const  {return  data[recordRateIndex         ];}
    float    Salinity              ()  const  {return  data[salinityIndex           ];}
    kkuint32 ScanLine              ()  const  {return  scanLine;                      }
    float    SoundVelocity         ()  const  {return  data[soundVelocityIndex      ];}
    float    Temperature           ()  const  {return  data[temperatureIndex        ];}
    float    Transmisivity         ()  const  {return  data[transmisivityIndex      ];}
    float    TransmisivitySensor   ()  const  {return  data[transmisivitySensorIndex];}
    float    Turbidity             ()  const  {return  data[turbidityIndex          ];}
    float    TurbiditySensor       ()  const  {return  data[turbiditySensorIndex    ];}

    float    Pitch                 ()  const  {return  data[pitchIndex              ];}
    float    Roll                  ()  const  {return  data[rollIndex               ];}

    static  KKStr  DensityUnit          ()  {return  FieldUnitOfMeasure (densityIndex);}
    static  KKStr  FluorescenceUnit     ()  {return  FieldUnitOfMeasure (fluorescenceIndex);}
    static  KKStr  OxygenUnit           ()  {return  FieldUnitOfMeasure (oxygenIndex);}
    static  KKStr  SalinityUnit         ()  {return  FieldUnitOfMeasure (salinityIndex);}
    static  KKStr  TemperatureUnit      ()  {return  FieldUnitOfMeasure (temperatureIndex);}
    static  KKStr  TransmisivityUnit    ()  {return  FieldUnitOfMeasure (transmisivityIndex);}
    static  KKStr  TurbidityUnit        ()  {return  FieldUnitOfMeasure (turbidityIndex);}
    static  KKStr  CdomFluorescenceUnit ()  {return  FieldUnitOfMeasure (cdomFluorescenceIndex);}

    KKStr   ToTabDelStr   ()  const;


    void   CtdDate  (const KKB::DateTime&  _ctdDate)  {ctdDate = _ctdDate;}

    void   ByteOffset    (kkuint64 _byteOffset)    {byteOffset    = _byteOffset;}
    void   Latitude      (double   _latitude)      {latitude      = _latitude;}
    void   Longitude     (double   _longitude)     {longitude     = _longitude;}
    void   ScanLine      (kkuint32 _scanLine)      {scanLine      = _scanLine;}

    void   ActiveColumns (kkuint16 _activeColumns) {activeColumns = _activeColumns;}
    void   CropLeft      (kkuint16 _cropLeft)      {cropLeft      = _cropLeft;}
    void   CropRight     (kkuint16 _cropRight)     {cropRight     = _cropRight;}


    void   CTDBattery             (float _ctdBattery)             {data [ctdBatteryIndex            ] = _ctdBattery;}
    void   Conductivity           (float _conductivity)           {data [ConductivityIndex          ] = _conductivity;}
    void   Depth                  (float _depth)                  {data [depthIndex                 ] = _depth;}
    void   Density                (float _density)                {data [densityIndex               ] = _density;}
    void   FlowRate1              (float _flowRate1)              {data [flowRate1Index             ] = _flowRate1;}
    void   FlowRate2              (float _flowRate2)              {data [flowRate2Index             ] = _flowRate2;}
    void   Fluorescence           (float _fluorescence)           {data [fluorescenceIndex          ] = _fluorescence;}
    void   FluorescenceSensor     (float _fluorescenceSensor)     {data [fluorescenceSensorIndex    ] = _fluorescenceSensor;}
    void   CdomFluorescence       (float _cdomFluorescence)       {data [cdomFluorescenceIndex      ] = _cdomFluorescence;}
    void   CdomFluorescenceSensor (float _cdomFluorescenceSensor) {data [cdomFluorescenceSensorIndex] = _cdomFluorescenceSensor;}
    void   OxygenSensor           (float _oxygenSensor)           {data [oxygenSensorIndex          ] = _oxygenSensor;}
    void   Oxygen                 (float _oxygen)                 {data [oxygenIndex                ] = _oxygen;}
    void   Pressure               (float _pressure)               {data [pressureIndex              ] = _pressure;}
    void   RecordRate             (float _recordRate)             {data [recordRateIndex            ] = _recordRate;}
    void   Salinity               (float _salinity)               {data [salinityIndex              ] = _salinity;}
    void   SoundVelocity          (float _soundVelocity)          {data [soundVelocityIndex         ] = _soundVelocity;}
    void   Temperature            (float _temperature)            {data [temperatureIndex           ] = _temperature;}
    void   Transmisivity          (float _transmisivity)          {data [transmisivityIndex         ] = _transmisivity;}
    void   TransmisivitySensor    (float _transmisivitySensor)    {data [transmisivitySensorIndex   ] = _transmisivitySensor;}
    void   Turbidity              (float _turbidity)              {data [turbidityIndex             ] = _turbidity;}
    void   TurbiditySensor        (float _turbiditySensor)        {data [turbiditySensorIndex       ] = _turbiditySensor;}

    void   Pitch                  (float _pitch)                  {data [pitchIndex              ] = _pitch;}
    void   Roll                   (float _roll)                   {data [rollIndex               ] = _roll;}

    /**
     *@brief  refreshes data fields from another 'InstrumentData' instance.
     */
    void   RefreshDataFields (const  InstrumentData&  id);

    void   UpdateFromCtdData (const CTD_Plus_Data&  ctdData);

    static  void  FinalCleanUp ();


  private:
    class  FieldDesc
    {
    public:
      KKStr  name;
      KKStr  shortDesc;
      KKStr  unitOfMeasure;
      float  minVal;
      KKStr  formatStr;
    };

    static  FieldDesc  fieldDescriptions[];
  
    void    InitializeDataVariables ();

    void    ParseTabDelString (const KKStr& s);

    void    UpdateFromStr (kkint32       fieldIndex,
                           const KKStr&  s
                          );
  
    //static const char*   fieldNames[][3];

    static void  CreateBlocker ();

    static bool  needToRunFinalCleanUp;

    static GoalKeeperPtr       blocker;
    static map<KKStr,kkuint32>*  fieldNamesUpperIdx;
    static map<KKStr,kkuint32>*  shortFieldNamesIdx;

    static kkint32       numFields;
    static kkint32       numDataFields;    /**< The first fields will be float fields that will be in a array.
                                            * the rest will have there own individual fields.
                                            */

    static kkint32  scanLineIndex;
    static kkint32  byteOffsetIndex;
    static kkint32  ctdDateIndex;;
    static kkint32  activeBatteryIndex;
    static kkint32  latitudeIndex;
    static kkint32  longitudeIndex;

    static kkint32  cropLeftIndex;
    static kkint32  cropRightIndex;
    static kkint32  activeColumnsIndex;

    static kkint32  ctdBatteryIndex;    // This is the first field that will be stored in the array 'data'
    static kkint32  ConductivityIndex;
    static kkint32  densityIndex;
    static kkint32  depthIndex;
    static kkint32  flowRate1Index;
    static kkint32  flowRate2Index;
    static kkint32  fluorescenceIndex;
    static kkint32  fluorescenceSensorIndex;
    static kkint32  cdomFluorescenceIndex;
    static kkint32  cdomFluorescenceSensorIndex;
    static kkint32  illuminationIndex;
    static kkint32  oxygenIndex;
    static kkint32  oxygenSensorIndex;
    static kkint32  pressureIndex;
    static kkint32  recordRateIndex;
    static kkint32  salinityIndex;
    static kkint32  soundVelocityIndex;
    static kkint32  temperatureIndex;
    static kkint32  transmisivityIndex;
    static kkint32  transmisivitySensorIndex;
    static kkint32  turbidityIndex;
    static kkint32  turbiditySensorIndex;
    static kkint32  pitchIndex;
    static kkint32  rollIndex;
    static kkint32  bat1LevelIndex;
    static kkint32  bat2LevelIndex;
    static kkint32  bat3LevelIndex;
    static kkint32  bat4LevelIndex;
  
    char             activeBattery;
    kkuint64         byteOffset;
    KKB::DateTime    ctdDate;
    float*           data;
    double           latitude;
    double           longitude;
    kkuint32         scanLine;

    kkuint16         cropLeft;       /**< Left  column that contains imaged data;  any columns to the left  are occluded by imaging chamber or Frame. */
    kkuint16         cropRight;      /**< Right column that contains imaged data;  any columns to the Right are occluded by imaging chamber or Frame. */
    kkuint16         activeColumns;  /**<  (1 + rightCrop - leftCrop - Inactive_Columns);  ex: Inactive_Columns Can be caused by poor light source.   */
  };  /* InstrumentData */
  
  
  typedef  InstrumentData*  InstrumentDataPtr;
}  /* SipperHardware */
  
  
#endif

