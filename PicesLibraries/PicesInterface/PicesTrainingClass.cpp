#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>


#include "MemoryDebug.h"
#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "OSservices.h"
using namespace  KKB;


#include "TrainingClass.h"
using namespace  KKMLL;

#include "PicesTrainingClass.h"
#include "PicesClassList.h"
#include "PicesKKStr.h"
#include "PicesRunLog.h"
using namespace  PicesInterface;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;



PicesTrainingClass::PicesTrainingClass (PicesClass^  _class,
                                        String^      _directory
                                       )
{
  MLClassList  classes;

  VectorKKStr  directories;
  directories.push_back (PicesKKStr::SystemStringToKKStr (_directory));

  trainingClass = new TrainingClass (directories,
                                     PicesKKStr::SystemStringToKKStr (_class->Name),
                                     1.0f,     // WeightFactor
                                     1.0f,     // CountFactor
                                     NULL,     // Sub-Classifier
                                     classes
                                    );
}


PicesTrainingClass::PicesTrainingClass (TrainingClassPtr  _trainingClass):
  trainingClass (_trainingClass)
{
}




TrainingClassPtr  PicesTrainingClass::UnManagedClass ()
{
  if  (trainingClass)
    return trainingClass;
  return  NULL;
}




String^  PicesTrainingClass::Directory::get ()
{
  if (trainingClass == NULL)
    return  gcnew String ("");

  return  PicesKKStr::KKStrToSystenStr (trainingClass->Directory (0));
}




void  PicesTrainingClass::Directory::set (String^  _directory)
{
  trainingClass->Directory (0, PicesKKStr::SystemStringToKKStr (_directory));
}



PicesClass^  PicesTrainingClass::MLClass::get ()
{
  return   PicesClassList::GetUniqueClass (trainingClass->MLClass ());
}


void  PicesTrainingClass::MLClass::set (PicesClass^  _mlClass)
{
  trainingClass->MLClass (_mlClass->UnmanagedMLClass());
}



String^   PicesTrainingClass::ExpandedDirectory (String^  rootDir)
{
  if  (rootDir == nullptr)
    rootDir = String::Empty;
  return  PicesKKStr::KKStrToSystenStr (trainingClass->ExpandedDirectory (PicesKKStr::SystemStringToKKStr (rootDir), 0));
}  /* ExpandedDirectory */




PicesTrainingClassList::PicesTrainingClassList ()
{
}



///<summary>
/// Will take ownership of the contents of 'trainingClasses'.  If 'trainingClasses'
/// does not own its own contents; will then create new instances of 'TrainingClass' 
/// objects.
///</summary>
PicesTrainingClassList::PicesTrainingClassList (TrainingClassListPtr  trainingClasses)
{
  TrainingClassList::iterator  idx;
  for  (idx = trainingClasses->begin ();  idx != trainingClasses->end ();  idx++)
  {
    TrainingClassPtr  tc = *idx;
    if  (!trainingClasses->Owner ())
      tc = new TrainingClass (*tc);
    Add (gcnew PicesTrainingClass (tc));
  }

  trainingClasses->Owner (false);
}
