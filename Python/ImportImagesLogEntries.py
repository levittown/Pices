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


def  QuoteIt(s):
  r = '"' + s.strip('"') + '"'
  return r



def  LoogUpImageFileName(db, c, _imageFileName):
  imageIdOurs = -1
  sqlStr = ("select ImageId  from  Images  where  ImageFileName = " + QuoteIt(_imageFileName))
  c.execute(sqlStr)
  for  (imageIdOF)  in  c:
    imageIdOurs = imageIdOF[0]
  return imageIdOurs
  



def  LogEntryExist(db, c, _imagesLogEntryId, _imageIdOurs, _imageFileName, _validatedClassName, _dateTimeOccuredStr, _action):
  logEntryInDB = False

  # We will check two situations
  # 1) First check if this ImagesLogEntry is in common with the one in our database.
  # 2) See if their is an entry for the same ImageId, ValidationClass and action; this would happen if added durting a prev import session.

  _dateTimeOccured = DateTimeFromStr(_dateTimeOccuredStr)
  sqlStr = ("select ImageFileName, ValidatedClassName, DateTimeOccured, Action from  ImagesLogEntries  where  ImagesLogEntryId = " + str(_imagesLogEntryId))
  c.execute(sqlStr)
  for  (imageFileNameOF, validatedClassNameOF, dateTimeOccuredOF, actionOF)  in  c:
    if  ((imageFileNameOF      == _imageFileName)       and
         (validatedClassNameOF == _validatedClassName)  and
         (dateTimeOccuredOF    == _dateTimeOccured)     and  
         (actionOF             == _action)
        ):
      logEntryInDB = True
  if  (logEntryInDB == False):
    sqlStr = ("select ImagesLogEntryId  from  ImagesLogEntries  " +
                      "where  ImageId = "            + str(_imageIdOurs)            + "  and  " +
                             "ImageFileName = "      + QuoteIt(_imageFileName)      + "  and  " +
                             "ValidatedClassName = " + QuoteIt(_validatedClassName) + "  and  " +
                             "Action = "             + QuoteIt(_action)
              )
    c.execute(sqlStr)
    for  (imagesLogEntryIdOF)  in  c:
      logEntryInDB = True;

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

  countEntries = 0
  countShortLines = 0
  countExistingEntries = 0
  countValidationEntries = 0
  countValidationsRemoved = 0
  countUpdateErrors = 0
  countMissingImages = 0
  
  fullLogEntryFileName = os.path.join(dirName, "ImagesLogEntries") + ".txt"
  imagesLogEntryData = open(fullLogEntryFileName)

  errorFileName = os.path.join(dirName, "ImagesLogEntries") + ".txt.Errors.txt"
  errorFile = open(errorFileName, mode='w')
  count = 0
  for  l in imagesLogEntryData:
    countEntries = countEntries + 1
    fields=l.split('\t')
    if  len(fields) < 6:
      countShortLines = countShortLines + 1
      errMsg = "Short-Line\t" + countShortLines + "\t"+ l;
      print(errmsg)
      errorFile.write(errMsg)
    else:
      imagesLogEntryIdExt = ToInt(fields[0])
      imageIdExt          = ToInt(fields[1])
      imageFileName       = fields[2].strip('"')
      validatedClassName  = fields[3].strip('"')
      dateTimeOccuredStr  = fields[4].strip('"')
      dateTimeOccured     = DateTimeFromStr (dateTimeOccuredStr)
      action              = fields[5].strip('"')
      dataBaseUserName    = fields[6].strip('"')

      imageIdOurs = LoogUpImageFileName (db, c, imageFileName)

      if  ((countEntries % 50) == 0):
        msg = (str(countEntries) + "\t" + "Entries"    + "\t" + str(countExistingEntries)
                                 + "\t" + "Validation" + "\t" + str(countValidationEntries)
                                 + "\t" + "Removals"   + "\t" + str(countValidationsRemoved)
                                 + "\t" + "Errors"     + "\t" + str(countUpdateErrors)
               )
        print(msg)
      
      if  (imageIdOurs < 0):
        #This entry is for an Image thatis not in our database!!!
        countMissingImages = countMissingImages + 1
        errMsg = str(imagesLogEntryIdExt) + "\t" + imageFileName + "\t" + "Missing Image ImageFileName"
        print(errMsg)
        errorFile.write(errMsg)
      else:
        existsInOurDatabase = LogEntryExist(db, c, imagesLogEntryIdExt, imageIdOurs, imageFileName, validatedClassName, dateTimeOccuredStr, action)
        if  (existsInOurDatabase):
          #print("Entry: " + str(imagesLogEntryIdExt) + "\t" + imageFileName + " Already Exists in our Database")
          countExistingEntries = countExistingEntries + 1
        else:
          if  (action == "V"):
            countValidationEntries = countValidationEntries + 1
            sqlStr = "call  ImagesUpdateValidatedClass(" + QuoteIt(imageFileName) + ", " + QuoteIt(validatedClassName) + ", 1.0)"
            try:
              c.execute(sqlStr)
              db.commit()
            except  mysql.connector.Error  as err:
              errMsg = str(imagesLogEntryIdExt) + "\t" + sqlStr + "\t" + str(err)
              print(errMsg)
              errorFile.write(errMsg)
              countUpdateErrors = countUpdateErrors + 1
          elif  (action == "R"):
            countValidationsRemoved = countValidationsRemoved + 1
            sqlStr = "call  ImagesRemoveValidation(" + QuoteIt(imageFileName) + ")"
            try:
              c.execute(sqlStr)
              db.commit()
            except  mysql.connector.Error  as err:
              errMsg = str(imagesLogEntryIdExt) + "\t" + sqlStr + "\t" + str(err)
              print(errMsg)
              errorFile.write(errMsg)
              countUpdateErrors = countUpdateErrors + 1
          else:
            errMsg = str(imagesLogEntryIdExt) + "\tUnrecognized Code\t" + l
            errorFile.write(errMsg)
            print(errMsg)

  errorFile.write("countEntries"            + "\t" + str(countEntries))
  errorFile.write("countMissingImages"      + "\t" + str(countMissingImages))
  errorFile.write("countExistingEntries"    + "\t" + str(countExistingEntries))
  errorFile.write("countValidationEntries"  + "\t" + str(countValidationEntries))
  errorFile.write("countValidationsRemoved" + "\t" + str(countValidationsRemoved))
  errorFile.write("countUpdateErrors"       + "\t" + str(countUpdateErrors))
  errorFile.close ()
  imagesLogEntryData.close ()


def  main():
  #rootDir="E:\\Users\\kkramer\\Dropbox\\Sipper\\FromAndrewToKurt\\Validation\\2014-09-16\\"
  #rootDir="F:\\Pices\\UpdatesFromOtherServers\\FromAndrews"
  #rootDir="C:\\Pices\\UpdatesFromOtherServers"
  #rootDir="C:\\Pices\\UpdatesFromOtherServers\\2015-03-11"
  rootDir="D:\\Users\\kkramer\\DropBox\\Dropbox\\ToKurt\\DataToKurt"
  ImportLogEntries(rootDir)
  print("\n\n    *** Import Completed ***\n\n")


main()

