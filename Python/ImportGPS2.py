import mysql.connector
passWord=""
db = mysql.connector.Connect(user='root',password=passWord,host='sipper-db.marine.usf.edu',database='pices_new')
c=db.connect()
