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



def  DateFromStr(s):
  d = datetime.date(1,1,1)
  try:
    d=datetime.datetime.strptime(s.strip(),"%m/%d/%Y").date()
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



def  TimeFromHHMMSS(s):
  i=int(ToFloat(s))
  secs = i % 100
  i = int (i / 100)
  mins = i % 100
  i = int (i / 100)
  hours = i
  return datetime.time(hours,mins,secs)





def  LoadClassList(fileName, db, c):
  try:
    classData = open(fileName)
  except  OSError  as  err:
    print("Could not open \"" + fileName + "\".");
    return  None
  classDict = {}
  for  l in classData:
    fields=l.split('\t')
    if  len(fields) > 3:
      extClassId = ToInt (fields[0])
      className  = fields[1].strip('"')
      sqlStr=('select ClassId from Classes where ClassName = "' + className + '"')
      c.execute(sqlStr)
      for  (ClassId)  in  c:
        classDict[extClassId] = ClassId[0]
  classData.close()
  return  classDict




def  LoadLogEntryLookup(db, c):
  logEntryDict = {}
  sqlStr=("select  externalLogEntryID, logEntryId  from  LogEntries  where  ((externalLogEntryID is not null)  and   (externalLogEntryID > 0))")
  c.execute(sqlStr)
  for  (externalLogEntryID, logEntryId) in c:
    logEntryDict[externalLogEntryID] = logEntryId
  return  logEntryDict






#Server	Description:Default	Embedded:No	HostName:localhost	UserName:root	PassWord:Dasani30!	PortNum: 3306	MySqlDataDir:	DataBaseName:pices_new



def  ImportClassificationDataOneFile(fileNum, fullFileName, classDic, logEntryDict, db, c):
  updateErrorFileName = fullFileName + ".UpdateErrors.txt"
  updateErrorFile = open(updateErrorFileName, mode='w');

  classificationData = open(fullFileName)

  count = 0
  errorCount = 0

  for  l in classificationData:
    fields=l.split('\t')
    if  len(fields) > 1:
      imageId        = ToInt    (fields[0])
      imageFileName  = fields[1].strip("\"")
      extLogEntryId  = ToInt    (fields[2])
      extClass1Id    = ToInt    (fields[3])
      class1Prob     = ToFloat  (fields[4])
      try:
        ourClass1Id    = classDic [extClass1Id]
      except:
        print("Image :" + imageFileName + "  ExtClassID :" + str(extClass1Id) + " Missing Entry in Translation table.")
        ourClass1Id = 0
        
      ourLogEntryId = 0
      if  (extLogEntryId > 0):
        ourLogEntryId = logEntryDict[extLogEntryId]

      sqlStr = ("update Images i "
                +  "set  i.Class1Id = " + str(ourClass1Id)  + ", "
                +       "i.Class1Prob = " + str(class1Prob) + ", " 
                +       "i.ClassLogEntryId = " + str(ourLogEntryId) 
                +  " where  i.ImageFileName = " + "\"" + imageFileName + "\"")

      if  ((count % 100) == 0):
        print(fileNum, " " , count, ": ", sqlStr)
      try:
        c.execute(sqlStr)
        db.commit()
      except  mysql.connector.Error  as err:
        updateErrorFile.write(str(imageId) + "\t" + imageFileName + "\t" + err + "\n");
        print(err)
        errorCount = errorCount + 1
      count = count + 1
    fields = None

  updateErrorFile.close ()
  classificationData.close ()





#Only class id's provided no probability or log entries.
# file format
# <ImageFileName><\t><classId1><\classId2><\t><validatedId>
def  ImportClassificationDataOneFile2(fileNum, fullFileName, classDic, logEntryDict, db, c):
  statusFileName = fullFileName + ".UpdateStatus.txt"
  statusFile = open(statusFileName, mode='w');

  updateErrorFileName = fullFileName + ".UpdateErrors.txt"
  updateErrorFile = open(updateErrorFileName, mode='at');

  classificationData = open(fullFileName)

  count = 0
  errorCount = 0

  for  l in classificationData:
    fields=l.split('\t')
    if  len(fields) > 1:
      imageFileName  = fields[0].strip("\"")
      extSipperid    = ToInt    (fields[1])
      extClass1Id    = ToInt    (fields[2])
      extClass2Id    = ToInt    (fields[3])
      try:
        ourClass1Id    = classDic [extClass1Id]
      except:
        print("Image :" + imageFileName + "  ExtClassID :" + str(extClass1Id) + " Missing Entry in Translation table.")
        ourClass1Id = 0

      try:
        ourClass2Id    = classDic [extClass2Id]
      except:
        print("Image :" + imageFileName + "  ExtClassID :" + str(extClass2Id) + " Missing Entry in Translation table.")
        ourClass2Id = 0

      sqlStr = ("update Images i "
                +  "set  i.Class1Id = " + str(ourClass1Id)  + ", "
                +       "i.Class1Prob = 0.70"               + ", "
                +       "i.Class2Id = " + str(ourClass1Id)  + ", "
                +       "i.Class2Prob = 0.30"
                +  " where  i.ImageFileName = " + "\"" + imageFileName + "\"")

      try:
        c.execute(sqlStr)
        db.commit()
        if  ((count % 100) == 0):
          print(fileNum, " " , count, ": ", sqlStr)
          statusFile.write(imageFileName + "\t" + str(count) + "\n")
          statusFile.flush()

      except  mysql.connector.Error as err:
        updateErrorFile.write(imageFileName + "\t" + err + "\n");
        print(err)
        errorCount = errorCount + 1

      except Exception as err2:
        print(err2)

      count = count + 1
    fields = None

  statusFile.close()
  updateErrorFile.close ()
  classificationData.close ()







def  ImportClassificationData(dirName):
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
        print("Cound not conect to \"Sipper-d-marine.usf.edu\"")
    elif  err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exists")
    else:
        print(err)

  if db is None:
    return

  c = db.cursor()
  
  fullClassFileName = os.path.join(dirName, "Classes") + ".txt"
  classDic = LoadClassList(fullClassFileName, db, c)
  if  classDic is None:
    return

  logEntryDict = LoadLogEntryLookup(db, c)
  if  logEntryDict is None:
    print("***ERROR***  Failed to load 'Log Entries'")
    return

  ImportClassificationDataOneFile2(1, (os.path.join(dirName, "ImageAssignment")  + ".txt"), classDic, logEntryDict, db, c)

  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_B0710")   + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_BE0412")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_BE0413")  + ".txt"), classDic, logEntryDict, db, c)


  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_BE0612")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_BE1112")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_ETP2007") + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_ETP2008") + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_HRS02")   + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0111")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0211")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0212")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0213")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0511")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0512")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0611")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0812")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB0813")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(6, (os.path.join(dirName, "ImagesClassificationCruise_WB0814")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(7, (os.path.join(dirName, "ImagesClassificationCruise_WB0911")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(8, (os.path.join(dirName, "ImagesClassificationCruise_WB1008")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB1012")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB1012")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB1101")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WB1111")  + ".txt"), classDic, logEntryDict, db, c)
  #ImportClassificationDataOneFile(1, (os.path.join(dirName, "ImagesClassificationCruise_WFS2008") + ".txt"), classDic, logEntryDict, db, c)


def  main():
  #rootDir="E:\\Users\\kkramer\\Dropbox\\Sipper\\FromAndrewToKurt\\Validation\\2014-09-16\\"
  #rootDir="F:\\Pices\\UpdatesFromOtherServers\\FromAndrews"
  #rootDir="C:\\Pices\\UpdatesFromOtherServers\\2015-03-11"
  #rootDir="C:\\Users\\Kurt\\Dropbox\\ToKurt\\DataToKurt"
  #rootDir="D:\\Temp"

  rootDir = "D:\\Users\\kkramer\\DropBox\\Dropbox\\ShareWithKurt\\2016-10-09"
  ImportClassificationData(rootDir)


main()

  
