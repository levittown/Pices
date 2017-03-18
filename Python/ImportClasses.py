import mysql.connector
from mysql.connector import errorcode
import datetime
import os
import KKBase.BasicConversions
from KKBase.BasicConversions import ToInt
from KKBase.BasicConversions import ToFloat



##...
##def  ToFloat(s):
##  f=0.0
##  try:
##    f=float(s)
##  except  ValueError:
##    f=0.0
##  return f
##
##
##
##def  ToInt(s):
##  i=0;
##  try:
##    i=int(s)
##  except  ValueError:
##    i=0
##  return i
##
##
##
##def  DateFromMDYStr(s):
##  d = datetime.date(1,1,1)
##  try:
##    d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
##  except  ValueError:
##    d=datetime.date(1900,1,1)
##  return d
##
##
##
##
##def  DateFromYMDStr(s):
##  d = datetime.date(1,1,1)
##  try:
##    d=datetime.datetime.strptime(s.strip(),"%Y/%m/%d").date()
##  except  ValueError:
##    d=datetime.date(1900,1,1)
##  return d
##
##
##
##
##def  DateFromStr(s):
##  d = datetime.date(1,1,1)
##  try:
##    d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
##  except  ValueError:
##    try:
##      d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
##    except  ValueError:
##      try:
##        d=datetime.datetime.strptime(s.strip(),"%Y/%m/%d").date()
##      except  ValueError:
##        try:
##          d=datetime.datetime.strptime(s.strip(),"%Y-%m-%d").date()
##        except  ValueError:
##          try:
##            d=datetime.datetime.strptime(s.strip(),"%d-%m-%Y").date()
##          except  ValueError:
##            d=datetime.date(1900,1,1)
##  return d
##
##
##
##
##def  TimeFromStr(s):
##  t=datetime.time(0,0,0)
##  try:
##    t=datetime.datetime.strptime(s.strip(),"%H:%M:%S").time()
##  except  ValueError:
##    t=datetime.time(0,0,0)
##  return t
##
##
##
##
##def  DateTimeFromStr (s):
##  s = s.strip("\"")
##  d = datetime.date (1,1,1)
##  t = datetime.time (0,0,0)
##  fields = s.split(' ')
##  if  len(fields) > 1:
##    d=DateFromStr(fields[0])
##    t=TimeFromStr(fields[1])
##  dt=datetime.datetime(d.year, d.month, d.day, t.hour, t.minute, t.second)
##  return dt
##
##
##
##def  TimeFromHHMMSS(s):
##  i=int(ToFloat(s))
##  secs = i % 100
##  i = int (i / 100)
##  mins = i % 100
##  i = int (i / 100)
##  hours = i
##  return datetime.time(hours,mins,secs)
##...



def  LoadClassExtenalTranslationTable(fileName):
  try:
    classData = open(fileName)
  except  OSError  as  err:
    print("Could not open \"" + fileName + "\".");
    return  None
  classDic = {}
  for  l in classData:
    fields=l.split('\t')
    if  len(fields) > 3:
      classId   = ToInt (fields[0])
      classMame = fields[1].strip("\"")
      classDic[classId] = classMame
  classData.close()
  return  classDic





def  IdentifyClass(db, c, className):
  ourClassId = -1
  sqlStr=('select ClassId from Classes where ClassName = "' + className + '"')
  c.execute(sqlStr)
  for  (classId)  in  c:
     ourClassId = classId[0]
  return  ourClassId




#Server	Description:Default	Embedded:No	HostName:localhost	UserName:root	PassWord:Dasani30!	PortNum: 3306	MySqlDataDir:	DataBaseName:pices_new



def  ImportClassEntries(dirName):
  passWord="Dasani!20"
  try:
     #db = mysql.connector.Connect(user='kkramer',
     #                             password=passWord,
     #                             host='sipper-db2.marine.usf.edu',
     #                             database='pices_new'
     #                          )
     #db = mysql.connector.Connect(user='kkramer',
     #                             password=passWord,
     #                             host='sipper-db2.marine.usf.edu',
     #                             database='pices_new'
     #                           )
     db = mysql.connector.Connect(user='root',
                                  password=passWord,
                                  host='localhost',
                                  database='pices_iec'
                                )
  except  mysql.connector.Error as err:
    db = None
    if  err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
        print('Could not connect to "Sipper-d-marine.usf.edu"')
    elif  err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exists")
    else:
        print(err)

  if db is None:
    return

  c = db.cursor()

  classesFileName = os.path.join(dirName, "Classes") + ".txt"
  classNameLookUp = LoadClassExtenalTranslationTable (classesFileName)

  classesData = open(classesFileName)

  count = 0

  for  l in classesData:
    l = l.strip('\n\r')
    fields=l.split('\t')
    if  len(fields) > 5:
      classIdExt  = ToInt(fields[0])
      className   = fields[1].strip('"')
      parentIdExt = ToInt(fields[2])
      description = fields[3]
      mandatory   = fields[4]
      summarize   = fields[5]
      parentIdOurs = 1
      try:
        parentName = classNameLookUp[parentIdExt]
      except:
        parentName = ""
      if  (parentName != ""):
        parentIdOurs = IdentifyClass (db, c, parentName)
      if  (parentIdOurs < 1):
        parentIdOurs = 1
      classIdOurs = IdentifyClass (db, c, className)
      if  (classIdOurs < 0):
        sqlStr = ('insert into  classes(ClassName, ParentId)' + ' ' +
                  'values(' + '"' + className + '"'  + ', ' +
                             str(parentIdOurs)      +
                        ')'
                  )
        print (sqlStr)
        try:
          c.execute(sqlStr)
          db.commit()
        except  mysql.connector.Error  as err:
          print(err)
      else:
        sqlStr = ('update Classes c  '  +
                     'set c.ParentId = '  + str(parentIdOurs) + ', ' +
                         'c.Mandatory = ' + mandatory         + ', ' +
                         'c.Summarize = ' + summarize         + ' '  +
                     'where c.ClassName = ' + '"' + className + '"'
                  )
        print(sqlStr)
        try:
          c.execute(sqlStr)
          db.commit()
        except  mysql.connector.Error  as err:
          print(err)
        
  classesData.close ()



def  main():
  #rootDir="E:\\Users\\kkramer\\Dropbox\\Sipper\\FromAndrewToKurt\\Validation\\2014-09-16\\"
  #rootDir="F:\\Pices\\UpdatesFromOtherServers\\FromAndrews"
  #rootDir="C:\\Pices\\UpdatesFromOtherServers\\2015-03-11"
  #rootDir="D:\\Users\\kkramer\\DropBox\\Dropbox\\ToKurt\\DataToKurt"
  #rootDir="D:\\Users\\kkramer\\DropBox\\Dropbox\\ToKurt\\DataToKurt"
  #rootDir="C:\\Users\\Kurt\Dropbox\\ToCotPices\\2015-06-23"
  rootDir="D:\\Users\\kkramer\\DropBox\\Dropbox\\Pices\\SipperFiles\\NOAA\\2016-08-20_UpdatesFromAndrewsPC"
  #rootDir="C:\\Pices\\UpdatesFromOtherServers"
  ImportClassEntries(rootDir)
  print("\n\n    *** Import Completed ***\n\n")


main()

