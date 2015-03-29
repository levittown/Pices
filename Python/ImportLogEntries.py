import mysql.connector
from mysql.connector import errorcode
import datetime
import os



def  ToFloat(s):
  f=0.0
  try:
    f=float(s)
  except  ValueError:
    f=0.0
  return f



def  ToInt(s):
  i=0;
  try:
    i=int(s)
  except  ValueError:
    i=0
  return i



def  DateFromMDYStr(s):
  d = datetime.date(1,1,1)
  try:
    d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
  except  ValueError:
    d=datetime.date(1900,1,1)
  return d




def  DateFromYMDStr(s):
  d = datetime.date(1,1,1)
  try:
    d=datetime.datetime.strptime(s.strip(),"%Y/%m/%d").date()
  except  ValueError:
    d=datetime.date(1900,1,1)
  return d




def  DateFromStr(s):
  d = datetime.date(1,1,1)
  try:
    d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
  except  ValueError:
    try:
      d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
    except  ValueError:
      try:
        d=datetime.datetime.strptime(s.strip(),"%Y/%m/%d").date()
      except  ValueError:
        try:
          d=datetime.datetime.strptime(s.strip(),"%Y-%m-%d").date()
        except  ValueError:
          try:
            d=datetime.datetime.strptime(s.strip(),"%d-%m-%Y").date()
          except  ValueError:
            d=datetime.date(1900,1,1)
  return d




def  TimeFromStr(s):
  t=datetime.time(0,0,0)
  try:
    t=datetime.datetime.strptime(s.strip(),"%H:%M:%S").time()
  except  ValueError:
    t=datetime.time(0,0,0)
  return t




def  DateTimeFromStr (s):
  s = s.strip("\"")
  d = datetime.date (1,1,1)
  t = datetime.time (0,0,0)
  fields = s.split(' ')
  if  len(fields) > 1:
    d=DateFromStr(fields[0])
    t=TimeFromStr(fields[1])
  dt=datetime.datetime(d.year, d.month, d.day, t.hour, t.minute, t.second)
  return dt



def  TimeFromHHMMSS(s):
  i=int(ToFloat(s))
  secs = i % 100
  i = int (i / 100)
  mins = i % 100
  i = int (i / 100)
  hours = i
  return datetime.time(hours,mins,secs)



def  LoadSipperFileLookUp(fileName, db, c):
  try:
    sipperFilesData = open(fileName)
  except  OSError  as  err:
    print("Could not open \"" + fileName + "\".");
    return  None
  sipperFileDic = {}
  for  l in sipperFilesData:
    fields=l.split('\t')
    if  len(fields) > 1:
      sipperFileIdExt = ToInt (fields[0])
      sipperFileName  = fields[1]
      sqlStr=("select SipperFileId from SipperFiles where SipperFileName = " + sipperFileName)
      c.execute(sqlStr)
      for  (sipperFileId)  in  c:
        sipperFileDic[sipperFileIdExt] = sipperFileId[0]
  sipperFilesData.close()
  return  sipperFileDic





def  LogEntryExist(db, c, _logEntryIdExt, _progName, _cmdLine, _dateTimeStart):
  progNameOF      = ""
  cmdLineOF       = ""
  dateTimeStartOF = ""

  logEntryInDB = False

  sqlStr = ("select ProgName, CmdLine, DateTimeStart  from  LogEntries  where  LogEntryId = " + str(_logEntryIdExt))
  c.execute(sqlStr)
  for  (progNameOF, cmdLineOF, dateTimeStartOF)  in  c:
    if  (progNameOF == _progName):
      if  (cmdLineOF != _cmdLine):
        print(cmdLineOF)
        print(_cmdLine)
      else:
        if  (dateTimeStartOF == _dateTimeStart):
           logEntryInDB = True

  if  logEntryInDB != True:
    sqlStr = ("select LogEntryId, ProgName, CmdLine, DateTimeStart  from  LogEntries  where  externalLogEntryId = " + str(_logEntryIdExt))
    c.execute(sqlStr)
    for  (logEntryIdOF, progNameOF, cmdLineOF, dateTimeStartOF)  in  c:
      if  (progNameOF == _progName)  and  (cmdLineOF == _cmdLine)  and  (dateTimeStartOF == _dateTimeStart):
        logEntryInDB = True
  return  logEntryInDB






#Server	Description:Default	Embedded:No	HostName:localhost	UserName:root	PassWord:Dasani30!	PortNum: 3306	MySqlDataDir:	DataBaseName:pices_new



def  ImportLogEntries(dirName):
  try:
       #db = mysql.connector.Connect(user='kkramer',
       #                             password="tree10peach",
       #                             host='sipper-db2.marine.usf.edu',
       #                             database='pices_new'
       #                          )
              
       db = mysql.connector.Connect(user='kkramer',
                                    password="tree10peach",
                                    host='sipper-db2.marine.usf.edu',
                                    database='pices_new'
                                  )
       #db = mysql.connector.Connect(user='kkramer',
       #                             password="tree10peach",
       #                             host='localhost',
       #                             database='pices_new'
       #                           )
       #db = mysql.connector.Connect(user='root',
       #                             password="dasani30",
       #                             host='localhost',
       #                             database='pices_new'
       #                           )
       
  except  mysql.connector.Error as err:
    db = None
    if  err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
        print("Cound not conect to \"Sipper-d-marine.usf.edu\"")
    elif  err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exists")
    else:
        print(err)

  if db is None:
    return

  c = db.cursor()

  sipperFileName = os.path.join(dirName, "SipperFiles") + ".txt"
  sipperFileIdLookUp = LoadSipperFileLookUp(sipperFileName, db, c)
  
  fullLogEntryFileName = os.path.join(dirName, "LogEntries") + ".txt"
  logEntryData = open(fullLogEntryFileName)
  count = 0
  for  l in logEntryData:
    fields=l.split('\t')
    if  len(fields) > 13:
      logEntryIdExt       = ToInt(fields[0])
      progCode            = fields[1]
      progName            = fields[2]
      dateTimeCompiled    = fields[3]
      cmdLine             = eval('"' + fields[4].replace('"','') + '"')
      CompName            = fields[5]
      dataBaseUserName    = fields[6]
      compUserName        = fields[7]
      dateTimeStart       = fields[8]
      dateTimeStartUtc    = fields[9]
      dateTimeEnd         = fields[10]
      cpuTimeUsed         = fields[11]
      completionStatus    = fields[12]
      sipperFileIdExt     = ToInt(fields[13])
      sipperFileIdOur = 0
      if  sipperFileIdExt > 0:
        try:
          sipperFileIdOur = sipperFileIdLookUp[sipperFileIdExt]
        except:
          sipperFileIdOur = 0

      #Make sure that this entry is not already in the database.
      if  (LogEntryExist (db, c, logEntryIdExt, progName.strip("\""), cmdLine.strip("\""), DateTimeFromStr(dateTimeStart))):
        print(str(logEntryIdExt) + " already in database.")
      else:
        sqlStr = ("insert into LogEntries(ProgCode, ProgName, DateTimeCompiled, CmdLine, " +
                  "CompName, DataBaseUserName, CompUserName, DateTimeStart, DateTimeStartUtc, DateTimeEnd, " +
                  "CpuTimeUsed, CompletionStatus, SipperFileId, externalLogEntryId)" + " " +
           "values(" + progCode             + "," +
                       progName             + "," +
                       dateTimeCompiled     + "," +
                       "\"" + cmdLine +"\"" + "," +
                       CompName             + "," +
                       dataBaseUserName     + "," +
                       compUserName         + "," +
                       dateTimeStart        + "," +
                       dateTimeStartUtc     + "," +
                       dateTimeEnd          + "," +
                       cpuTimeUsed          + "," +
                       completionStatus     + "," +
                       str(sipperFileIdOur) + "," +
                       str(logEntryIdExt)   +
                  ")"
                  )
        print (sqlStr)
        try:
          c.execute(sqlStr)
          db.commit()
        except  mysql.connector.Error  as err:
          print(err)
  logEntryData.close ()



def  main():
  #rootDir="E:\\Users\\kkramer\\Dropbox\\Sipper\\FromAndrewToKurt\\Validation\\2014-09-16\\"
  #rootDir="F:\\Pices\\UpdatesFromOtherServers\\FromAndrews"
  #rootDir="C:\\Pices\\UpdatesFromOtherServers"
  #rootDir="F:\\Pices\\UpdatesFromOtherServers\\FromAndrews\\2015-02-11"
  rootDir="D:\\Users\\kkramer\\DropBox\\Dropbox\\ToKurt\\DataToKurt"
  ImportLogEntries(rootDir)
  print("\n\n    *** Import Completed ***\n\n")


main()

