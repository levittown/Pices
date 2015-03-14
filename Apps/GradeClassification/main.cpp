#include "FirstIncludes.h"

#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

#include "MemoryDebug.h"


#include "KKBaseTypes.h"
#include "KKStr.h"
using namespace KKB;

#include  "GradeClassification.h"



int  main (int  argc, 
           char **argv
          )
{
  GradeClassification  gc;
  gc.InitalizeApplication (argc, argv);
  if  (gc.Abort ())
  {
    return (-1);
  }
  else
  {
    gc.Grade ();
    return (0);
  }
}
