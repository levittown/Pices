#include  "FirstIncludes.h"

#include  <stdlib.h>
#include  <memory>
#include  <math.h>

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "MemoryDebug.h"
#include "KKBaseTypes.h"

using namespace std;

#include "Compressor.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;

#include "PicesApplication.h"
using namespace MLL;


#include "ImportFullSizeFromSqlBackUp.h"


ImportFullSizeFromSqlBackUp::ImportFullSizeFromSqlBackUp ():
   PicesApplication (),
   srcFileName ()
{
}



ImportFullSizeFromSqlBackUp::~ImportFullSizeFromSqlBackUp ()
{
}



void  ImportFullSizeFromSqlBackUp::InitalizeApplication (kkint32 argc,
                                                         char**  argv
                                                        )
{
  DataBaseRequired (true);
  PicesApplication::InitalizeApplication (argc, argv);
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (srcFileName.Empty ())
  {
    log.Level (-1) << endl 
                   << "ImportFullSizeFromSqlBackUp::InitalizeApplication   ***ERROR***   you must provide at least the name of the SQL backup file." << endl
                   << endl;
    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }
}  /* InitalizeApplication */




bool  ImportFullSizeFromSqlBackUp::ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                                            const KKStr&  parmValue
                                                           )
{
  if  (parmSwitch.EqualIgnoreCase ("-S")        ||  
       parmSwitch.EqualIgnoreCase ("-SRC")      ||  
       parmSwitch.EqualIgnoreCase ("-SOURCE")   ||
       parmSwitch.EqualIgnoreCase ("-SrcFile")
      )
  {
    srcFileName = parmValue;
    if  (!osFileExists (srcFileName))
    {
      log.Level (-1) << "ImportFullSizeFromSqlBackUp::ProcessCmdLineParameter   ***ERROR***   Invalid '-SrcFile' [" << srcFileName << "] file." << endl;
      Abort (true);
    }
  }

	return  !Abort ();
}  /* ProcessCmdLineParameter */




/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   ImportFullSizeFromSqlBackUp::DisplayCommandLineParameters ()
{
  PicesApplication::DisplayCommandLineParameters ();
  cout << endl
       << "    -SrcFile  <Source File>  Specify the name of the file where the Full Size "  << endl
       << "              Images are to be restored from.  This is the SQL file that you "   << endl
       << "              have backed up to."                                                << endl
       << endl;
}  /* DisplayCommandLineParameters */



void  ImportFullSizeFromSqlBackUp::Main ()
{
  ifstream  i (srcFileName.Str ());

  int buffLen = 20 * 1024 * 1024;
  char*  buff = new char[buffLen];

  for  (int y = 0;  y < 28;  y++)
    i.getline (buff, buffLen);

  while  (i.getline (buff, buffLen))
  {
    char buffShort[32];
    for  (int x = 0;  x < sizeof (buffShort);  x++)
      buffShort[x] = buff[x];
    buffShort[sizeof (buffShort) - 1] = 0;
    cout << buffShort << endl;

    if  ((buff[0] == 'I')  &&  
         (buff[1] == 'N')  &&
         (buff[2] == 'S')  &&
         (buff[3] == 'E')  &&
         (buff[4] == 'R')
        )
    {
      kkint32  buffStrLen = (kkint32)strlen (buff);
      DB ()->QueryStatement2 (buff, buffStrLen);
    }
  }

}  /* ImportFullSizeImages */



int  main (int     argc,
           char**  argv
          )
{

  ImportFullSizeFromSqlBackUp  mergeFeatureFiles;
  mergeFeatureFiles.InitalizeApplication (argc, argv);
  if  (!mergeFeatureFiles.Abort ())
    mergeFeatureFiles.Main ();

  if  (mergeFeatureFiles.Abort ())
    return 1;
  else
    return 0;
}
