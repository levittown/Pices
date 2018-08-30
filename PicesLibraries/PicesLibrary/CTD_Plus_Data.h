#ifndef  _CTD_PLUS_DATA_
#define  _CTD_PLUS_DATA_

#include  "DateTime.h"
#include  "KKStr.h"
using  namespace  KKB;


namespace MLL
{
  class CTD_Plus_Data
  {
  public:
    CTD_Plus_Data (bool  _checkCDOM);
   
    CTD_Plus_Data (const KKStr&   _txt,
                   double         _latitude,
                   bool&          valid,
                   const KKStr&   ext0CCode,
                   const KKStr&   ext1CCode,
                   const KKStr&   ext2CCode,
                   const KKStr&   ext3CCode
                  );
  
    ~CTD_Plus_Data ();
  
  
    float            BatteryVoltage ()  const  {return  batteryVoltage;}
    float            Conductivity   ()  const  {return  conductivity;}
    const 
      KKB::DateTime& Date                   ()  const  {return  date;}
    float            Density                ()  const  {return  density;}
    float            Depth                  ()  const  {return  depthMeters;}
    float            Fluorescence           ()  const  {return  fluorescence;}
    float            FluorescenceSensor     ()  const  {return  fluorescenceSensor;}
    float            CdomFluorescence       ()  const  {return  cdomFluorescence;}
    float            CdomFluorescenceSensor ()  const  {return  cdomFluorescenceSensor;}
    float            Oxygen                 ()  const  {return  oxygen;}
    float            OxygenSensor           ()  const  {return  oxygenSensor;}
    float            Pressure               ()  const  {return  pressure;}
    float            Salinity               ()  const  {return  salinity;}
    float            SoundVelocity          ()  const  {return  soundVelocity;}
    float            Temperature            ()  const  {return  temperature;}
    float            Transmisivity          ()  const  {return  transmisivity;}
    float            TransmisivitySensor    ()  const  {return  transmisivitySensor;}
    float            Turbidity              ()  const  {return  turbidity;}
    float            TurbiditySensor        ()  const  {return  turbiditySensor;}
  
    static
    KKStr  Header1Str ();
  
    static
    KKStr  Header2Str ();
  
    KKStr  ToStrNoLabels ()  const;

    bool   ValidData ()  const;   /*!< Returns 'true'  if the CTD data fields appear to be valid. */
  
    CTD_Plus_Data&  operator=  (const CTD_Plus_Data&  right);
  
  private:
    void  CalcFluorescence ();
    void  CalcCdomFluorescence ();
    void  CalcTransmisivity ();
    void  CalcTurbidity ();
  
    void  UpdateExternalVoldtageField (const KKStr&  extCode,
                                       const KKStr&  amtField,
                                       kkint32&        numExtenalVoltsProcessed,
                                       kkint32&        fieldNum,
                                       bool&         valid
                                      );
  
    float           batteryVoltage;
    bool            checkCDOM;    /**< Indicates if CDOM is one of the external instruments. */
    float           conductivity;
    KKB::DateTime   date;
    float           density;
    float           depthMeters;
    float           fluorescence;
    float           fluorescenceSensor;
    float           cdomFluorescence;
    float           cdomFluorescenceSensor;
    float           oxygen;
    float           oxygenSensor;
    float           pressure;        // psia
    float           salinity;
    float           soundVelocity;
    float           temperature;
    float           transmisivity;
    float           transmisivitySensor;
    float           turbidity;
    float           turbiditySensor;

  
  //"Transmisivity",   // 17
  //"Turbidity",     // 15
  //"Flourences",      // 16
  //"OxygenSensor",    // 18
  
   
    float  DissolvedOxygenConcentration (float  _voltage,
                                         float  _temperature,
                                         float  _salinity,
                                         float  _pressure
                                        );
  
  
    
  };
}  /* MLL */  
  
#endif
