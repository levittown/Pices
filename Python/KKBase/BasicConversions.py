import datetime


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
