#include "FirstIncludes.h"
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace KKB;


#include "MLClass.h"
using namespace  KKMLL;


#include "DataBaseImage.h"
#include "InstrumentData.h"
#include "InstrumentDataFileManager.h"
using namespace MLL;



DataBaseImage::DataBaseImage ():
    byteOffset           (0),
    centroidCol          (0),
    centroidRow          (0),
    class1               (NULL),
    class1Prob           (0.0f),
    class2               (NULL),
    classLogEntryId      (0),
    class2Prob           (0.0f),
    connectedPixelDist   (0),
    depth                (0.0f),
    encodedThumbnail     (NULL),
    extractionLogEntryId (0),
    height               (0),
    imageFileName        (),
    imageId              (0),
    imageSize            (0.0f),
    pixelCount           (0),
    sipperFileId         (-1),
    sipperFileName       (),
    sizeCoordinates      (NULL),
    topLeftRow           (0),
    topLeftCol           (0),
    validatedClass       (NULL),
    width                (0)
{
}



DataBaseImageList::DataBaseImageList (DataBaseImageList& srcList, bool _owner):
    KKQueue<DataBaseImage>(srcList, _owner)
{
}



DataBaseImage::~DataBaseImage ()
{
  delete  encodedThumbnail;  encodedThumbnail = NULL;
  delete  sizeCoordinates;   sizeCoordinates  = NULL;
}



MLClassPtr  DataBaseImage::GetMlClass (MLClassField mlClassField) const
{
  switch (mlClassField)
  {
  case  MLClassField::Class1:     return class1;
  case  MLClassField::Class2:     return class2;
  case  MLClassField::Validated:  return validatedClass;
  default: throw KKException ("DataBaseImage::GetMlClass   Invalid 'mlClassField' specified!");
    break;
  }
}


float  DataBaseImage::BreakTie () const  
{
  if  (class1Prob > class2Prob)
    return  (class1Prob - class2Prob);
  else
    return  (class2Prob - class1Prob);
}



const KKStr&  DataBaseImage::Class1Name ()  const
{
  if  (class1)
    return class1->Name ();
  else
    return KKStr::EmptyStr ();
}



const KKStr&  DataBaseImage::Class2Name ()  const
{
  if  (class2)
    return class2->Name ();
  else
    return KKStr::EmptyStr ();
}



void  DataBaseImage::Class1Name (const char* _class1Name)
{
  if  (!_class1Name)
    class1 = NULL;
  
  else if  (strlen (_class1Name) == 0)
    class1 = NULL;

  else
    class1 = MLClass::CreateNewMLClass (_class1Name);
}  /* Class1Name */



void  DataBaseImage::Class2Name (const char*  _class2Name)
{
  if  (!_class2Name)
     class2 = NULL;

  else if  (strlen (_class2Name) == 0)
     class2 = NULL;

  else
    class2 = MLClass::CreateNewMLClass (_class2Name);
}  /* Class2Name */



const  KKStr&   DataBaseImage::ValidatedClassName () const
{
  if  (validatedClass)
    return  validatedClass->Name ();
  else
    return  KKStr::EmptyStr ();
}



void  DataBaseImage::ValidatedClassName (const char*  _validatedClassName)
{
  if  (!_validatedClassName)
    validatedClass = NULL;

  else if  (strlen (_validatedClassName) == 0)
    validatedClass = NULL;

  else
    validatedClass = MLClass::CreateNewMLClass (_validatedClassName);
}  /* ValidatedClassName */



/**
 *@brief Giving ownership of '_sizeCoordinates' to this instance of 'DataBaseImage'.
 */
void  DataBaseImage::SizeCoordinates (PointListPtr  _sizeCoordinates)
{
  delete sizeCoordinates;
  sizeCoordinates = _sizeCoordinates;
}



void  DataBaseImage::SizeCoordinates  (const KKStr  _sizeCoordinatesStr)
{
  delete  sizeCoordinates;
  sizeCoordinates = NULL;
  if  (_sizeCoordinatesStr.Empty ())
    return;

  sizeCoordinates = new PointList (true);
  VectorKKStr fields = _sizeCoordinatesStr.Split (',');
  kkuint32 x = 0;
  while  ((x + 1) < fields.size ())
  {
    kkint16 row = fields[x].ToInt16 ();
    ++x;
    kkint16 col = fields[x].ToInt16 ();
    ++x;
    sizeCoordinates->PushOnBack (new Point (row, col));
  }
}  /* SizeCoordinates */



void  DataBaseImage::SizeCoordinatesAdd (const Point& _point)
{
  if  (!sizeCoordinates)
    sizeCoordinates = new PointList (true);

  sizeCoordinates->PushOnBack (new Point (_point));
}



RasterSipperPtr  DataBaseImage::ThumbNail (RunLog&  log) const
{
  log.Level (60) << "DataBaseImage::ThumbNail"  << std::endl;
  if  (!encodedThumbnail)
    return  NULL;
  return  RasterSipper::FromSimpleCompression (this->encodedThumbnail, encodedThumbnailLen);
}  /* ThumbNail */



RasterSipperPtr  DataBaseImage::GetOrigImageFromSipperFile (RunLog&  log) const
{
  uchar  distToUse = 3;

  if  (connectedPixelDist < 1)
    distToUse = 3;
  else
    distToUse = connectedPixelDist;

  return  InstrumentDataFileManager::GetOrigSipperImage 
           (osGetRootName (sipperFileName), 
           this->byteOffset, 
           this->topLeftRow,
           this->topLeftCol,
           this->height,
           this->width,
           distToUse,
           log
          );
}  /* GetOrigImageFromSipperFile */



DataBaseImageList::DataBaseImageList (bool _owner):
      KKQueue<DataBaseImage> (_owner)
{
}



DataBaseImageList::~DataBaseImageList ()
{
}



MLClassListPtr  DataBaseImageList::ExtractListOfClasses ()
{
  MLClassListPtr  classes = new MLClassList ();
  DataBaseImageList::iterator  idx;

  map<MLClassPtr,MLClassPtr>  collectedClasses;
  map<MLClassPtr,MLClassPtr>::iterator  ccIDX;
  MLClassPtr  lastClass = NULL;

  for  (idx = begin ();  idx != end ();  idx++)
  {
    DataBaseImagePtr  i = *idx;

    MLClassPtr  ic = i->ValidatedClass ();
    if  (ic == NULL)
      ic = i->Class1 ();

    if  (ic == lastClass)
      continue;

    lastClass = ic;
    ccIDX = collectedClasses.find (ic);
    if  (ccIDX == collectedClasses.end ())
    {
      collectedClasses.insert (pair<MLClassPtr,MLClassPtr> (lastClass, lastClass));
    }
  }

  for  (ccIDX = collectedClasses.begin ();  ccIDX != collectedClasses.end ();  ++ccIDX)
    classes->PushOnBack (ccIDX->first);

  return  classes;
}  /* ExtractListOfClasses */



ClassStatisticListPtr  DataBaseImageList::ComputeClassStats (DataBaseImage::MLClassField mlClassField) const
{
  map<MLClassPtr, kkuint32> classCounts;
  map<MLClassPtr, kkuint32>::iterator  idx;

  for  (auto dbi: *this)
  {
    auto c = dbi->GetMlClass (mlClassField);
    idx = classCounts.find(c);
    if  (idx == classCounts.end ())
      classCounts.insert(pair<MLClassPtr, kkuint32>(c, 1));
    else
      ++(idx->second);
  }

  ClassStatisticListPtr stats = new ClassStatisticList (true);
  for  (auto stat: classCounts)
    stats->push_back (new ClassStatistic (stat.first, stat.second));
  return stats;
}



DataBaseImageListPtr  DataBaseImageList::ExtractExamplesForAGivenClass (MLClassPtr  _mlClass)
{ 
  DataBaseImageListPtr  images = new DataBaseImageList (false);
  DataBaseImageList::iterator  idx;

  for  (idx = begin ();  idx != end ();  idx++)
  {
    DataBaseImagePtr  i = *idx;
    MLClassPtr  ic = i->ValidatedClass ();
    if  (ic == NULL)
      ic = i->Class1 ();

    if  (ic == _mlClass)
      images->PushOnBack (i);
  }

  return   images;
}  /* ExtractExamplesForAGivenClass */



void  DataBaseImageList::SortBySpatialDistance ()
{
  sort (begin (), end (), [](DataBaseImagePtr p1, DataBaseImagePtr  p2) -> bool
  {
    if  (p1->SipperFileName () < p2->SipperFileName ())
      return true;

    else if  (p1->SipperFileName () > p2->SipperFileName ())
      return false;

    return  (p1->TopLeftRow () < p2->TopLeftRow ());
  });

}  /* SortBySpatialDistance */



float  FlowRate (InstrumentDataPtr  id,
                 float              defaultFlowRate
                )
{
  if  ((!id)  ||  (id->FlowRate1 () <= 0.0f))
    return  defaultFlowRate;

  return  id->FlowRate1 ();
}



VectorFloat  DataBaseImageList::CalculateDensitesByQuadrat (float        scanRate,         // Scan Lines per Sec.
                                                            float        quadratSize,      // Meters.
                                                            float        defaultFlowRate,  // Meters per Sec
                                                            const bool&  cancelFlag,
                                                            RunLog&      log
                                                           )
{
  InstrumentDataFileManager::InitializePush ();

  SortBySpatialDistance ();

  DataBaseImagePtr  firstOne = *(begin ());

  float  curFlowRate      = defaultFlowRate;
  float  distLastImage    = 0.0f;
  long   scanLineLastImage = firstOne->TopLeftRow ();
  KKStr  lastSipperFileName = firstOne->SipperFileName ();

  VectorInt  imagesPerQuadrat;
  imagesPerQuadrat.push_back (0);

  InstrumentDataPtr  id = InstrumentDataFileManager::GetClosestInstrumentData (firstOne->ImageFileName (), cancelFlag, log);
  curFlowRate = FlowRate (id, defaultFlowRate);

  DataBaseImageList::iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
  {
    DataBaseImagePtr  i = *idx;
    if  (i->SipperFileName () != lastSipperFileName)
    {
      // We started a new SIPPER file;  so we will now use the time difference between then to
      // determine distance,
      InstrumentDataPtr  lastId = id;
      id = InstrumentDataFileManager::GetClosestInstrumentData (i->ImageFileName (), cancelFlag, log);
      DateTime  deltaTime = id->CtdDate () - lastId->CtdDate ();
      float  avgFlowRate = (id->FlowRate1 () + lastId->FlowRate1 ()) / 2;
      kkuint64  deltaSecs = deltaTime.Seconds ();
      float  deltaDist = avgFlowRate * deltaSecs;
      long  deltaScanLines = (long)((float)scanRate * deltaDist + 0.5f);
      scanLineLastImage = i->TopLeftRow () - deltaScanLines;

      curFlowRate = FlowRate (id, defaultFlowRate);
    }

    if  ((i->TopLeftRow () - id->ScanLine ()) > 4096)
    {
      // Time to get a more UpToDate Instrument Data Item
      id = InstrumentDataFileManager::GetClosestInstrumentData (i->ImageFileName (), cancelFlag, log);
      curFlowRate = FlowRate (id, defaultFlowRate);
    }

    long  deltaScanLines = i->TopLeftRow () - scanLineLastImage;

    float  deltaDist = ((float)deltaScanLines / scanRate) * curFlowRate;

    float  newDist = distLastImage + deltaDist;

    kkuint32  quadratIdx = (kkint32)(newDist / quadratSize);
    while  (imagesPerQuadrat.size () <= quadratIdx)
      imagesPerQuadrat.push_back (0);
    imagesPerQuadrat[quadratIdx]++;

    distLastImage = newDist;
    scanLineLastImage = i->TopLeftRow ();
  }


  float  areaPerQuatdrat = quadratSize * 0.098f;
  VectorFloat  density (imagesPerQuadrat.size (), 0.0f);
  for  (kkuint32 x = 0;  x < imagesPerQuadrat.size ();  x++)
    density[x] = imagesPerQuadrat[x] / areaPerQuatdrat;

  InstrumentDataFileManager::InitializePop ();

  return  density;
}  /* CalculateDensitesByQuadrat */



