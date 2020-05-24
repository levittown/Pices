delimiter ;

/**********************************************************************************************************************/
drop procedure  if exists StationsInsert;

delimiter //

create procedure StationsInsert (_cruiseName    char(10),
                                 _stationName   char(10),
                                 _description   varchar(255),
                                 _latitude      double,
                                 _longitude     double,
                                 _dateTimeStart datetime
                                )
begin
  Insert into Stations (CruiseName, StationName, Description, Latitude, Longitude, DateTimeStart)
         values (_cruiseName, _stationName, _description, _latitude, _longitude, _dateTimeStart);
end
//
delimiter ;
 
 
 
 
 /**********************************************************************************************************************/
drop procedure  if exists StationsUpdate;

delimiter //

create procedure StationsUpdate (_cruiseName    char(10),
                                 _stationName   char(10),
                                 _description   varchar(255),
                                 _latitude      double,
                                 _longitude     double,
                                 _dateTimeStart datetime
                                )
begin
  update Stations 
    set  Description   = _description, 
         Latitude      = _latitude, 
         Longitude     = _longitude, 
         DateTimeStart = _dateTimeStart
    where  (CruiseName = _cruiseName)  and  (StationName  = _stationName);
end
//
delimiter ;



 
 /**********************************************************************************************************************/
drop procedure  if exists StationsDelete;

delimiter //
create procedure StationsDelete (_cruiseName    char(10),
                                 _stationName   char(10)
                                )
begin
  delete from  Stations 
    where  (CruiseName = _cruiseName)  and  (StationName  = _stationName);
end
//
delimiter ;





/**********************************************************************************************************************/
drop procedure  if exists StationsLoadByGpsRange;

delimiter //
create procedure StationsLoadByGpsRange (latitudeMin    double,
                                         latitudeMax    double,
										 longitudeMin   double,
                                         longitudeMax   double
                                        )
begin
 select * from Stations s 
     where (s.Longitude >= longitudeMin)  and  (s.Longitude <= longitudeMax)  and
	       (s.Latitude  >= latitudeMin)   and  (s.Latitude  <= latitudeMax);
end
//
delimiter ;
