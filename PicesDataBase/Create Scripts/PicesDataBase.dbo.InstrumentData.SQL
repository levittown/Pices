/****** Object:  Table [dbo].[InstrumentData]    Script Date: 05/23/2008 18:21:57 ******/

drop table if exists InstrumentData;

create table   InstrumentData
(
    SipperFileId           int unsigned     not null                references SiperFiles(SipperFileId),
    ScanLine               int unsigned     not null  default 0,
    ByteOffset             bigint unsigned  not null  default 0  comment 'Byte offset of start of ScanlLne in "SipperFileName" ',
    CTDDateTime            datetime         null,
    Latitude                double           null,
    Longitude              double           null,
    CTDBattery             float            null   default 0.0,
    Conductivity           float            null   default 0.0,
    Density                float            null   default 0.0,
    Depth                  float            null   default 0.0,
    FlowRate1              float            null   default 0.0,
    FlowRate2              float            null   default 0.0,
    Fluorescence           float            null   default 0.0,
    FluorescenceSensor     float            null   default 0.0,
    CdomFluorescenceSensor float            null   default 0.0,
    CdomFluorescence       float            null   default 0.0,
    Oxygen                 float            null   default 0.0,
    OxygenSensor           float            null   default 0.0,
    Pressure               float            null   default 0.0,
    RecordRate             float            null   default 0.0,
    Salinity               float            null   default 0.0,
    SoundVelocity          float            null   default 0.0,
    Temperature            float            null   default 0.0,
    Transmisivity          float            null   default 0.0,
    TransmisivitySensor    float            null   default 0.0,
    Turbidity              float            null   default 0.0,
    TurbiditySensor        float            null   default 0.0,
    Pitch                  float            null   default 0.0,
    Roll                   float            null   default 0.0,
    CropLeft               int unsigned     null   default 0,
    CropRight              int unsigned     null   default 4095,
    ActiveColumns          int unsigned     null   default 3900,
    	
    primary key (SipperFileId asc,  ScanLine asc),
    key          CTDDateTimeKey      (CTDDateTime   asc),
    
    foreign key (SipperFileId)  references  SipperFiles (SipperFileId)
) 
;