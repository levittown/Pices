#include "FirstIncludes.h"

#include <stdlib.h>
#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "MemoryDebug.h"

using namespace  std;


#include "KKBaseTypes.h"
#include "BMPImage.h"
#include "OSservices.h"
#include "Raster.h"
#include "KKStr.h"
using namespace  KKB;


#include "Classifier2.h"
#include "FeatureFileIOC45.h"
#include "MLClass.h"
#include "NormalizationParms.h"
#include "TrainingProcess2.h"
using namespace  KKMLL;

#include "DataBase.h"
#include "DataBaseServer.h"
#include "FeatureFileIOPices.h"
#include "ImageFeatures.h"
#include "PicesVariables.h"
#include "PicesFVProducer.h"
using namespace  MLL;


#include  "ImageDimensionComputations.h"





SipperFileListPtr  GetListOfSipperFiles (DataBasePtr           dbConn,
                                         ImageFeaturesListPtr  examples,
                                         RunLog&               log
                                        )
{
  map<KKStr,KKStr>  sipperFiles;
  map<KKStr,KKStr>::iterator  sipperFilesIdx;
  SipperFileListPtr files = new SipperFileList (true);

  ImageFeaturesList::iterator  idx;
  for  (idx = examples->begin ();  idx != examples->end ();  idx++)
  {
    ImageFeaturesPtr  i = *idx;
    KKStr  imageFileName =  i->ExampleFileName ();
    KKStr   sipperFileName;
    kkuint32  scanLineNum = 0;
    kkuint32  scanCol     = 0;

    PicesVariables::ParseImageFileName (imageFileName, sipperFileName, scanLineNum, scanCol);
    sipperFilesIdx = sipperFiles.find (sipperFileName);
    if  (sipperFilesIdx == sipperFiles.end ())
    {
      sipperFiles.insert (pair<KKStr, KKStr>(sipperFileName, sipperFileName));
      SipperFilePtr sf = dbConn->SipperFileRecLoad (sipperFileName);
      if  (!sf)
      {
        sf = new SipperFile (sipperFileName);
        sf->CtdExt0 ("TRN");
        sf->CtdExt1 ("OXG");
        sf->CtdExt2 ("FLO");
        sf->CtdExt3 ("TUR");

        sf->Sp0 (Instrument::LookUpByShortName ("CTD"));
        sf->Sp1 (Instrument::LookUpByShortName ("P-R"));
        sf->Sp2 (Instrument::LookUpByShortName ("BAY"));

        dbConn->SipperFileInsert (*sf);
      }

      files->PushOnBack (sf);
    }
  }

  return  files;
}  /* GetListOfSipperFiles */







void  main (int argc,  char** argv)
{

                               
  ImageDimensionComputations extractFeatures;
  extractFeatures.InitalizeApplication (argc, argv);
  if  (!extractFeatures.Abort ())
    extractFeatures.Extract ();
}

