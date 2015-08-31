import mysql.connector
from mysql.connector import errorcode
#import grt
#from grt import executeScript

import  datetime



def  ToFloat(s):
  f=0.0
  try:
    f=float(s)
  except  ValueError:
    f=0.0
  return f



#try:

#     db = mysql.connector.Connect(user='root',
#                                  password="dasani30",
#                                  host='sipper-db.marine.usf.edu',
#                                  database='pices_new')

try:
  passWord=""
  db = mysql.connector.Connect(user='root',
                               password=passWord,
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

fileNames = [
             "InstrumentDataCropSettings_B0710.txt",
             "InstrumentDataCropSettings_BE0412.txt",
             "InstrumentDataCropSettings_BE0413.txt",
             "InstrumentDataCropSettings_BE0612.txt",
             "InstrumentDataCropSettings_BE1112.txt",
             "InstrumentDataCropSettings_ETP2007.txt",
             "InstrumentDataCropSettings_ETP2008.txt",
             "InstrumentDataCropSettings_HRS02.txt",
             "InstrumentDataCropSettings_WB0111.txt",
             "InstrumentDataCropSettings_WB0211.txt",
             "InstrumentDataCropSettings_WB0212.txt",
             "InstrumentDataCropSettings_WB0213.txt",
             "InstrumentDataCropSettings_WB0511.txt",
             "InstrumentDataCropSettings_WB0512.txt",
             "InstrumentDataCropSettings_WB0611.txt",
             "InstrumentDataCropSettings_WB0812.txt",
             "InstrumentDataCropSettings_WB0813.txt",
             "InstrumentDataCropSettings_WB0814.txt",
             "InstrumentDataCropSettings_WB0911.txt",
             "InstrumentDataCropSettings_WB1008.txt",
             "InstrumentDataCropSettings_WB1012.txt",
             "InstrumentDataCropSettings_WB1101.txt",
             "InstrumentDataCropSettings_WB1111.txt",
             "InstrumentDataCropSettings_WFS2008.txt"
            ]


for  fileName in fileNames:
   print("FileNanme: " + fileName + "\n")

   fileOpened = False
   try:
     instrumentDataDataFile=open("D:\\Users\\kkramer\\DropBox\\Dropbox\\Sipper\\ForAndrew\\InstrumentDataCropSettings\\" + fileName)
     fileOpened = True

   except:
     print("Error opening file: " + fileName)
     fileOpened = False

   if  fileOpened:
      count = 0
      for line in instrumentDataDataFile:
        fields=line.split('\t')
        if  len(fields)>= 5:
            sipperFileName=fields[0]
            scanLine      = int(fields[1])
            cropLeft      = int(fields[2])
            cropRight     = int(fields[3])
            activeColumns = int(fields[4])
 
            sqlStr="call InstrumentDataUpdateCropSettings(" +  sipperFileName        + "," + \
                                                           repr(scanLine)            + "," + \
                                                           repr(scanLine + 4090)     + "," + \
                                                           repr(cropLeft)            + "," + \
                                                           repr(cropRight)           + "," + \
                                                           repr(activeColumns)       + \
                                                         ")"
            #result = grt.root.wb.sqlEditors[0].executeScript(sqlStr)

            if  (count % 1000) == 0:
              print(count, sqlStr)

            try:
              c.execute(sqlStr)
              db.commit()
            except  mysql.connector.Error  as err:
              print(err)
           
            count = count + 1
           
instrumentDataDataFile.close()
db.close()



