#import mysql.connector
#from mysql.connector import errorcode
import grt
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

#try:
#      db = mysql.connector.Connect(user='root',
#                             password="dasani30",
#                             host='localhost',
#                             database='pices_new')


#except  mysql.connector.Error as err:
#    if  err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
#        print("Cound notconect to \"Sipper-d-marine.usf.edu\"")
#    elif  err.errno == errorcode.ER_BAD_DB_ERROR:
#        print("Database does not exists")
#    else:
#        print(err)

#c=db.cursor()

instrumentDataDataFile=open("D:\\Users\\kkramer\\DropBox\\Dropbox\\Sipper\\ForAndrew\\InstrumentDataCropSettings.txt")

count = 0

for line in instrumentDataDataFile:
     fields=line.split('\t')
     if  len(fields)>= 5  and  len(fields[0])>19:
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
         #executeScript(sqlStr)
         result = grt.root.wb.sqlEditors[0].executeScript(sqlStr)

         if  (count % 1000) == 0:
           print(count, sqlStr)

         #try:
         #  c.execute(sqlStr)
         #  db.commit()
         #except  mysql.connector.Error  as err:
         #  print(err)
           
         count = count + 1
           
instrumentDataDataFile.close()
db.close()



