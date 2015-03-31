delimiter ;


drop procedure  if exists GpsDataInsert;

delimiter //

create procedure  GpsDataInsert (in  _CruiseName          char(10),
                                 in  _UtcDateTime         datetime,
                                 in  _Latitude            double,
                                 in  _Longitude           double,
                                 in  _CourseOverGround    float,   /* True North */
                                 in  _SpeedOverGround     float    /* Knots      */
                                )

begin

  insert into GpsData(CruiseName, UtcDateTime, Latitude, Longitude, CourseOverGround, SpeedOverGround)
         values(_CruiseName, _UtcDateTime, _Latitude, _Longitude, _CourseOverGround, _SpeedOverGround);

end
//
delimiter ;





/**********************************************************************************************************************/
drop procedure   if exists  GpcDataQuery;
drop procedure   if exists  GpsDataQuery;

delimiter //

create procedure  GpsDataQuery (in  _CruiseName         char(10),
                                in  _UtcDateTimeStart   DateTime,
                                in  _UtcDateTimeEnd     DateTime
                               )
begin
  select  * from  GPSData gd  
     where  (gd.CruiseName  =  _CruiseName)         and
            (gd.UtcDateTime >= _UtcDateTimeStart)  and
            (gd.UtcDateTime <= _UtcDateTimeStart);
end
//
delimiter ;







/**********************************************************************************************************************/
drop procedure   if exists  GpsDataDelete;

delimiter //

create procedure  GpsDataDelete (in  _CruiseName         char(10),
                                 in  _UtcDateTimeStart   datetime,
                                 in  _UtcDateTimeEnd     dateTime
                                )
begin
  delete from  GPSData
     where  (CruiseName  =  _CruiseName)        and
            (UtcDateTime >= _UtcDateTimeStart)  and
            (UtcDateTime <= _UtcDateTimeStart);
end
//
delimiter ;





/**********************************************************************************************************************/
drop procedure   if exists  GpsDataDelete;

delimiter //

create procedure  GpsDataDelete (in  _CruiseName         char(10),
                                 in  _UtcDateTimeStart   datetime,
                                 in  _UtcDateTimeEnd     dateTime
                                )
begin
  delete from  GPSData
     where  (CruiseName  =  _CruiseName)        and
            (UtcDateTime >= _UtcDateTimeStart)  and
            (UtcDateTime <= _UtcDateTimeStart);
end
//
delimiter ;


/*
select distinct  id.SipperFileId, id.CTDDateTime from InstrumentData id
      join(SipperFiles sf)  on(sf.SipperFileId = id.SipperFileId)
      where sf.CruiseName="WB0814"
      order by id.CTDDateTime;
*/

drop procedure   if exists  GpsDataGetEstimate;

delimiter //

create procedure  GpsDataGetEstimate (in  _CruiseName        char(10),
                                      in  _UtcDateTime       datetime,
                                      out _latitude          double,
                                      out _longitude         double,
                                      out _courseOverGround  double,
                                      out _speedOverGround   double
                                     )
begin
 
  declare  latEnd       double default 0.0;
  declare  longEnd      double default 0.0;
  declare  cogEnd       double default 0.0;
  declare  sogEnd       double default 0.0;
  declare  dateTimeEnd  datetime;

  declare  latStart       double default 0.0;
  declare  longStart      double default 0.0;
  declare  cogStart       double default 0.0;
  declare  sogStart       double default 0.0;
  declare  dateTimeStart  datetime;


  select  gd.Latitude, gd.longitude, gd.courseOverGround, gd.speedOverGround, gd.UtcDateTime into latStart, longStart, cogStart, sogStart, dateTimeStart
       from  GpsData gd
       where gd.CruiseName=_CruiseName  and  gd.UtcDateTime<=_UtcDateTime  order by gd.UtcDateTime desc
       limit 1;

  select  gd.Latitude, gd.longitude, gd.courseOverGround, gd.speedOverGround, gd.UtcDateTime into latEnd, longEnd, cogEnd, sogEnd, dateTimeEnd
       from  GpsData gd
       where gd.CruiseName=_CruiseName  and  gd.UtcDateTime>=_UtcDateTime
       limit 1;


  /* select  latStart, longStart, cogStart, sogStart, dateTimeStart, latEnd, longEnd, cogEnd, sogEnd, dateTimeEnd; */

  if  dateTimeStart = dateTimeEnd  then
    select latStart, longStart, cogEnd, sogEnd  into _latitude, _longitude, _courseOverGround, _speedOverGround;
  else
    /* select  _UtcDateTime, latStart, longStart, dateTimeStart, latEnd, longEnd, dateTimeEnd; */

    set  @secsStart = to_seconds(dateTimeStart);
    set  @secsEnd   = to_seconds(dateTimeEnd);
    set  @secsPoint = to_seconds(_UtcDateTime);

    set  @secsRange = @secsEnd - @secsStart;

    set  @beforeRange = @secsPoint - @secsStart;
    set  @afterRange  = @secsEnd   - @secsPoint;

    set  @beforeFrac = @beforeRange / @secsRange;
    set  @afterFrac  = @afterRange  / @secsRange;
  
    /* select  @secsStart, @secsEnd, @secsPoint, @secsRange, @beforeFrac, @afterFrac; */

    set  @latDelta  = latEnd  - latStart;
    set  @longDelta = longEnd - longStart;
    set  @cogDelta  = cogEnd  - cogStart;
    set  @sogDelta  = sogEnd  - sogStart;
  
    set  @latPoint  = latStart  + @latDelta  * @beforeFrac;
    set  @longPoint = longStart + @longDelta * @beforeFrac;
    set  @cogPoint  = cogStart  + @cogDelta  * @beforeFrac;
    set  @sogPoint  = sogStart  + @sogDelta  * @beforeFrac;

    select @latPoint, @longPoint, @cogPoint, @sogPoint  into _latitude, _longitude, _courseOverGround, _speedOverGround;
  end if;

end
//
delimiter ;




drop procedure   if exists  GpsDataGetEstimate2;

delimiter //

create procedure  GpsDataGetEstimate2 (in  _CruiseName        char(10),
                                       in  _UtcDateTime       datetime
                                      )
begin
  call   GpsDataGetEstimate (_CruiseName, _UtcDateTime, @latitude, @longitude, @courseOverGround, @speedOverGround);
  select  @latitude          as  "Latitude",
          @longitude         as  "longitude",
          @courseOverGround  as  "CourseOverGround", 
          @speedOverGround   as  "SpeedOverGround";
end
//
delimiter ;






drop procedure   if exists  GpsDataGetEstimateByCTDDateTime;

delimiter //

create procedure  GpsDataGetEstimateByCTDDateTime (in  _CruiseName        char(10),
                                                   in  _stationName       varChar(64),
                                                   in  _deploymentNum     varChar(4),
                                                   in  _ctdDateTime       datetime,
                                                   out _latitude          double,
                                                   out _longitude         double,
                                                   out _courseOverGround  double,
                                                   out _speedOverGround   double
                                                  )
begin
  declare  _syncGpsDataTime   datetime;
  declare  _syncCtdDateTime   datetime;
  declare  _deltaGpsCtdSecs   int default 0;
  declare  _gpsDataTime       datetime;

  select  d.SyncTimeStampCTD, d.SyncTimeStampGPS  into  _syncCtdDateTime, _syncGpsDataTime
          from Deployments d
          where (d.CruiseName = _cruiseName)  and  (d.StationName = _stationName)  and  (d.DeploymentNum = _deploymentNum);

  set  _deltaGpsCtdSecs = to_seconds(_syncGpsDataTime) - to_seconds(_syncCtdDateTime);

  set  _gpsDataTime = timestampadd(second, _deltaGpsCtdSecs, _ctdDateTime);

  call   GpsDataGetEstimate (_CruiseName, _gpsDataTime, _latitude, _longitude, _courseOverGround, _speedOverGround);
end
//
delimiter ;




drop procedure   if exists  GpsDataGetEstimateByCTDDateTime2;

delimiter //

create procedure  GpsDataGetEstimateByCTDDateTime2 (in  _CruiseName     char(10),
                                                    in  _stationName    varChar(64),
                                                    in  _deploymentNum  varChar(4),
                                                    in  _ctdDateTime    datetime
                                                   )
begin
  call   GpsDataGetEstimateByCTDDateTime (_CruiseName, _stationName, _deploymentNum,  _ctdDateTime, @latitude, @longitude, @courseOverGround, @speedOverGround);

  select  @latitude          as  "Latitude",
          @longitude         as  "Longitude",
          @courseOverGround  as  "CourseOverGround", 
          @speedOverGround   as  "SpeedOverGround";
end
//
delimiter ;



















  
drop procedure  if exists GpsDataForDeployment;
delimiter //

create procedure  GpsDataForDeployment (in  _cruiseName      varChar(64),
                                        in  _stationName     varChar(64),
                                        in  _deploymentNum   varChar(4),
                                        in  _timeInterval    int
                                       )
begin
  declare  _syncGpsDataTime   datetime;
  declare  _syncCtdDateTime   datetime;
  declare  _deltaGpsCtdSecs   int  default 0;
  declare  _adjCtdDateTime    datetime;

  declare  _startCtdDataTime  datetime;
  declare  _endCtdDataTime    datetime;

  declare  _startGpsDataTime  datetime;
  declare  _endGpsDataTime    datetime;


  select  d.SyncTimeStampCTD, d.SyncTimeStampGPS  into  _syncCtdDateTime, _syncGpsDataTime
          from Deployments d
          where (d.CruiseName = _cruiseName)  and  (d.StationName = _stationName)  and  (d.DeploymentNum = _deploymentNum);

  set _deltaGpsCtdSecs = to_seconds(_syncGpsDataTime) - to_seconds(_syncCtdDateTime);


  select  min(id.CtdDateTime), max(id.CtdDateTime)  into  _startCtdDataTime, _endCtdDataTime
         from  InstrumentData id
         join(SipperFiles sf)  on(sf.SipperFileId = id.SipperFileId)
         where  (sf.CruiseName = _cruiseName)  and   (sf.stationName = _stationName)  and  (sf.deploymentNum =  _deploymentNum) and
                (id.CTDDateTime > "2002-01-01 00:00:00")  and  (id.CtdDateTime < "2020-12-31 23:59:59");

  set  _startGpsDataTime = timestampadd(second, _deltaGpsCtdSecs, _startCtdDataTime);
  set  _endGpsDataTime   = timestampadd(second, _deltaGpsCtdSecs, _endCtdDataTime);

  select  gd.UtcDateTime, avg(gd.latitude), avg(gd.longitude), avg(gd.courseOverGround), avg(gd.speedOverGround)
  from  GpsData gd
  where  (gd.CruiseName = _cruiseName)  and
         (gd.UtcDateTime >= _startGpsDataTime)  and
         (gd.UtcDateTime <= _endGpsDataTime)
     group by floor(Time_To_Sec(Time(gd.UtcDateTime)) / _timeInterval);
end
//
delimiter ;





drop procedure  if exists GpsDataByCtdDateTime;
delimiter //

create procedure  GpsDataByCtdDateTime (in  _cruiseName      varChar(64),
                                        in  _stationName     varChar(64),
                                        in  _deploymentNum   varChar(4),
                                        in  _ctdDateTime     datetime
                                       )
begin
  declare  _syncGpsDataTime   datetime;
  declare  _syncCtdDateTime   datetime;
  declare  _deltaGpsCtdSecs   int  default 0;
  declare  _adjCtdDateTime    datetime;

  declare  _gpsDataTime  datetime;

  select  d.SyncTimeStampCTD, d.SyncTimeStampGPS  into  _syncCtdDateTime, _syncGpsDataTime
          from Deployments d
          where (d.CruiseName = _cruiseName)  and  (d.StationName = _stationName)  and  (d.DeploymentNum = _deploymentNum);

  set _deltaGpsCtdSecs = to_seconds(_syncGpsDataTime) - to_seconds(_syncCtdDateTime);


  set  _gpsDataTime = timestampadd(second, _deltaGpsCtdSecs, _ctdDataTime);


  call  GpsDataGetEstimate (_cruiseName, _gpsDataTime, @latitude, @longitude, @cog, @sog);


  select  gd.UtcDateTime, avg(gd.latitude), avg(gd.longitude), avg(gd.courseOverGround), avg(gd.speedOverGround)
  from  GpsData gd
  where  (gd.CruiseName = _cruiseName)  and
         (gd.UtcDateTime >= _startGpsDataTime)  and
         (gd.UtcDateTime <= _endGpsDataTime)
     group by floor(Time_To_Sec(Time(gd.UtcDateTime)) / _timeInterval);
end
//
delimiter ;


