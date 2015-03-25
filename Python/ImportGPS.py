import mysql.connector
from mysql.connector import errorcode
import  datetime
from os import listdir
from os import path


def LatitudeFromStr(s):
  fields=s.split(' ')
  lat=0
  if  len(fields)>=3:
      deg=float(fields[0])
      mins=float(fields[1])
      sign=1.0
      if  fields[2]=='S':
        sign=-1.0
      lat=sign*(deg+mins/60.0)
  return lat


def LatitudeFromStrings(s, hemisphere):
  if  len(s) < 7:
    return -999.999
  x = s.find(".")
  if  x < 2:
    return -999.999
  degrees = ToInt (s[0:(x - 2)])
  minutes = ToFloat (s[(x - 2):])
  sign = 1.0
  if  (hemisphere == "S")  or  (hemisphere == "s"):
      sign = -1.0
  return  sign * (degrees + minutes / 60.0)




def LongitudeFromStr(s):
  fields=s.split(' ')
  long=0
  if  len(fields)>=3:
      deg=float(fields[0])
      mins=float(fields[1])
      sign=1.0
      if  (fields[2]=='W')  or  (fields[2]=='w'):
        sign=-1.0
      long=sign*(deg+mins/60.0)
  return long




def LongitudeFromStrings(s, hemisphere):
  if  len(s) < 7:
    return -999.999
  x = s.find(".")
  if  x < 2:
    return -999.999
  degrees = ToInt (s[0:(x - 2)])
  minutes = ToFloat (s[(x - 2):])
  sign = 1.0
  if  (hemisphere == "W")  or  (hemisphere == "w"):
      sign = -1.0
  return  sign * (degrees + minutes / 60.0)




def  DateTimeFromStr(s):
  fields=s.split(' ')
  if  len(fields) != 3:
     return datetime.datetime(1,1,1,0,0,0)
  dateParts=fields[0].split('/')
  if  len(dateParts)!=3:
     return datetime.datetime(1,1,1,0,0,0)
  month=int(dateParts[0])
  day=int(dateParts[1])
  year=int(dateParts[2])
  timeParts=fields[1].split(':')
  if  len(timeParts)!=3:
     return datetime.datetime(year,month,day,0,0,0)
  hours=int(timeParts[0])
  mins=int(timeParts[1])
  secs=int(timeParts[2])

  if  fields[2]=="AM"  and  hours==12:
    hours=0
  elif  fields[2]=="PM"  and  hours<12:
    hours = hours + 12     
  return  datetime.datetime(year,month,day, hours,mins,secs)


def  TimeFromHHMMSS (s):
  if  len(s) < 6:
    return datetime.time(0, 0, 0)
  return  datetime.time (ToInt(s[0:2]), ToInt(s[2:4]), ToInt(s[4:6]))




def  DateFromDDMMYY (s):
  if  len(s) < 6:
    return datetime.date(1900, 1, 1)
  dd = ToInt (s[0:2])
  mm = ToInt (s[2:4])
  yy = ToInt (s[4:6])
  if  yy < 70:
    yy = yy + 2000
  else:
    yy = yy + 1900
  return  datetime.date(yy,mm,dd)



def  ToFloat(s):
  f=0.0
  try:
    f=float(s)
  except  ValueError:
    f=0.0
  return f



def  ToInt(s):
    i = 0
    try:
      i=int(s)
    except  ValueError:
      i=0
    return i




def  ProcessSumaryGPSFile (fileName,
                           cruiseName,
                           startTime
                          ):
  try:
       db = mysql.connector.Connect(user='root',
                                    password="Dasani30!",
                                    host='localhost',
                                    database='pices_new')
  except  mysql.connector.Error as err:
      if  err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
          print("Cound notconect to \"Sipper-d-marine.usf.edu\"")
      elif  err.errno == errorcode.ER_BAD_DB_ERROR:
          print("Database does not exists")
      else:
          print(err)

  c=db.cursor()

  gpsDataFile=open(fileName)

  #startTime = datetime.datetime.strptime('2012-05-13 23:56:55', "%Y-%m-%d %H:%M:%S")

  for line in gpsDataFile:
     fields=line.split('\t')
     if  len(fields)>3  and  len(fields[0])>19:
          dateTime=DateTimeFromStr(fields[0])
          dateTime = dateTime + datetime.timedelta(hours=4)
          if  (dateTime > startTime):
            dateTimeStr=dateTime.strftime("%Y/%m/%d %H:%M:%S")
            latitude=LatitudeFromStr(fields[1])
            longitude=LongitudeFromStr(fields[2])
            courseOverGround=0.0
            courseOverGroundStr=fields[3]
            if  len(courseOverGroundStr)>3:
              idx=courseOverGroundStr.index('°')
              if  idx>2:
                courseOverGround=float(courseOverGroundStr[0:idx])
            speedOverGround=0.0
            speedOverGround=ToFloat(fields[4])
            sqlStr="call GpsDataInsert(" + "\"" + cruiseName  + "\"" + "," + \
                                           "\"" + dateTimeStr + "\"" + "," + \
                                           repr(latitude)            + "," + \
                                           repr(longitude)           + "," + \
                                           repr(courseOverGround)    + "," + \
                                           repr(speedOverGround)           + \
                                     ")"
            print(sqlStr)
            try:
              c.execute(sqlStr)
              db.commit()
            except  mysql.connector.Error  as err:
              print(err)
  gpsDataFile.close()
  db.close()




def  ProcessNMEAFile (fileName,
                      cruiseName
                     ):
  try:
       db = mysql.connector.Connect(user='root',
                                    password="Dasani30!",
                                    host='localhost',
                                    database='pices_new')
  except  mysql.connector.Error as err:
      if  err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
          print("Cound notconect to \"Sipper-d-marine.usf.edu\"")
      elif  err.errno == errorcode.ER_BAD_DB_ERROR:
          print("Database does not exists")
      else:
          print(err)

  c=db.cursor()

  x = fileName.find(".dat")
  if  (x < 9):
    return;

  dateStr = fileName[(x - 6):x]
  year = ToInt (dateStr[0:2])
  if  year < 30:
    year = year + 2000
  else:
    year = year + 1900

  month = ToInt(dateStr[2:4])
  day   = ToInt(dateStr[4:6])


  try:
    #gpsDataFile=open(fileName, encoding="utf8")
    gpsDataFile=open(fileName, encoding="latin-1")
  except:
    return


  #"$GPGGA,185302,2745.616,N,08238.004,W,2,07,1.20,0,M,,,1,0244*14,"

  lastDateTime = datetime.datetime (1990, 1, 1, 0, 0, 0);
  latestDateTime = datetime.datetime (1990, 1, 1, 0, 0, 0);
  lastLatitude = 0.0
  lastLongitue = 0.0
  lastSOG      = 0.0
  lastCoarse   = 0.0

  for line in gpsDataFile:
    x = line.find("$GPGGA")
    if  x >= 0:
      gpggaStr = line[(x + 7):]
      fields = gpggaStr.split(",")
      if  len(fields) > 6:
        time = TimeFromHHMMSS(fields[0])
        temp = LatitudeFromStrings(fields[1], fields[1])
        if  temp > -999.99:
          lastLatitude = temp;
        temp = LongitudeFromStrings(fields[3], fields[4])
        if  temp > -999.999:
          lastLongitude = temp
        fixQuality    = fields[5]
        numSats       = ToInt (fields[6])
        latestDateTime = datetime.datetime (year, month, day, time.hour, time.minute, time.second)

    x = line.find("$GPRMC")
    if  x >= 0:
      gprmcStr = line[(x + 7):]
      fields = gprmcStr.split(",")
      if  len(fields) > 6:
        time = TimeFromHHMMSS(fields[0])
        temp = LatitudeFromStrings(fields[2], fields[3])
        if  temp > -999.99:
          lastLatitude = temp;
        temp = LongitudeFromStrings(fields[4], fields[5])
        if  temp > -999.999:
          lastLongitude = temp
        lastSOG = ToFloat (fields[6])
        lastCoarse = ToFloat (fields[7])
        date = DateFromDDMMYY (fields[8])
        year = date.year
        month = date.month
        day = date.day
        latestDateTime = datetime.datetime (year, month, day, time.hour, time.minute, time.second)


    if  (latestDateTime - lastDateTime).seconds > 0:
          sqlStr="call GpsDataInsert(" + "\"" + cruiseName  + "\"" + "," + \
                                         "\"" + latestDateTime.strftime ("%Y-%m-%d %H:%M:%S") + "\"" + "," + \
                                         repr(lastLatitude)            + "," + \
                                         repr(lastLongitude)           + "," + \
                                         repr(lastCoarse)              + "," + \
                                         repr(lastSOG)                 + \
                                   ")"
          print(sqlStr)
          try:
            c.execute(sqlStr)
            db.commit()
          except  mysql.connector.Error  as err:
            print(err)
          lastDateTime = latestDateTime
  gpsDataFile.close()
  db.close()






def  ProcessCruise(cruiseName, dir):
  files = listdir(dir)
  for  fileName in files:
    if  fileName[-4:] == ".dat":
      fullName = path.join(dir, fileName)
      ProcessNMEAFile (fullName, cruiseName)

  


#ProcessCruise("WB1111", "F:\\Pices\\SipperFiles\\WB1111\\GPS-Data")
#ProcessCruise("WB1012", "F:\\Pices\\SipperFiles\\WB1012\\GPS-Data")
#ProcessCruise("WB1008", "F:\\Pices\\SipperFiles\\WB1008\\GPS-Data")
#ProcessCruise("WB0911", "F:\\Pices\\SipperFiles\\WB0911\\GPS-Data")
ProcessCruise("WB0814", "F:\\Pices\\SipperFiles\\WB0814\\GPS-Data")
#ProcessCruise("WB0813", "F:\\Pices\\SipperFiles\\WB0813\\GpsData\\Daly Aug 2013 Met")
#ProcessCruise("WB0611", "F:\\Pices\\SipperFiles\\WB0611\\GPS-Data")

#We only hat Met files thru GMT 2012-05-13  23:56:55  So we need to get th elast two days from shjip log file
#ProcessCruise("WB0512", "F:\\Pices\\SipperFiles\\WB0512\\GPS-Data")
#startTime = datetime.datetime.strptime('2012-05-13 23:56:55', "%Y-%m-%d %H:%M:%S")
#ProcessSumaryGPSFile ("F:\\Pices\\SipperFiles\\WB0512\\GPS-Data\\Daly May 2012 ship log.txt", "WB0512", startTime)

#We only have met data thru 2012-08-08 20:59:38(GMT)  So will us Sjip Log for further data.
#ProcessCruise("WB0812", "F:\\Pices\\SipperFiles\\WB0812\\GPS-Data")
#startTime = datetime.datetime.strptime('2012-08-08 20:59:38', "%Y-%m-%d %H:%M:%S")
#ProcessSumaryGPSFile ("F:\\Pices\\SipperFiles\\WB0812\\GPS-Data\\Daly Aug 2012 ship log.txt", "WB0812", startTime)


#ProcessCruise("WB0511", "F:\\Pices\\SipperFiles\\WB0511\\GPS-Data")
#ProcessCruise("WB0213", "F:\\Pices\\SipperFiles\\WB0213\\GPS-Data")
#ProcessCruise("WB0212", "F:\\Pices\\SipperFiles\\WB0212\\GPS-Data")
#ProcessCruise("WB0211", "F:\\Pices\\SipperFiles\\WB0211\\GPS-Data")
#ProcessCruise("WB0111", "F:\\Pices\\SipperFiles\\WB0111\\GPS-Data")

#ProcessCruise("", "")
#ProcessCruise("", "")
#ProcessCruise("", "")






