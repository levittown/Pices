#include "FirstIncludes.h"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "RunLog.h"
using namespace KKB;


#include "LLoydsEntry.h"





LLoydsEntry::LLoydsEntry (kkint32       _lloydsBinSize,
                          VectorIntPtr  _lloydsBins,
                          double        _lloydsIndex
                         )
{
  lloydsBinSize = _lloydsBinSize;
  lloydsBins    = _lloydsBins;
  lloydsIndex   = _lloydsIndex;

  if  (lloydsBins)
  {
    kkint32  total = 0;
    kkuint32  x;
    for  (x = 0;  x < lloydsBins->size ();  x++)
      total += (*lloydsBins)[x];
    
    lloydsBinsMean = float (total) / float (lloydsBins->size ());
  }
  else
  {
    lloydsBinsMean = 0.0f;
  }
}




LLoydsEntry::~LLoydsEntry ()
{
  delete  lloydsBins;
  lloydsBins = NULL;
}



kkint32  LLoydsEntry::LLoydsBin (size_t idx) const
{
  if  (!lloydsBins)
    return 0;

  if  ((idx < 0)  ||  (idx >= lloydsBins->size ()))
    return 0;

  return  (*lloydsBins)[idx];
}




LLoydsEntryList::LLoydsEntryList (RunLog&  _log):
  KKQueue<LLoydsEntry>  (true),
  log                   (_log)
{
}


void  LLoydsEntryList::PushOnBack (LLoydsEntryPtr  _lloydsEntry)
{
  kkint32  binSize = _lloydsEntry->LLoydsBinSize ();
  LLoydsBinEntryMap::iterator  idx;
  idx = binEntryMap.find (binSize);
  if  (idx != binEntryMap.end ())
  {
    if  (idx->second->LLoydsBinSize () == binSize)
    {
      // We have a duplicate entry;   This should never ever happen.  This means the caller
      // has a major logic error and the whole world needs to know.
      log.Level (-1) << endl << endl
                     << "LLoydsEntryList::PushOnBack     ***ERROR***" << endl
                     << endl
                     << "                   Duplicate LLoydsBinSize[" << binSize << "]" << endl
                     << endl;
      return;
    }
  }

  KKQueue<LLoydsEntry>::PushOnBack (_lloydsEntry);
  binEntryMap.insert (LLoydsBinEntryPair (_lloydsEntry->LLoydsBinSize (), _lloydsEntry));
}





LLoydsEntryPtr  LLoydsEntryList::LLoydsEntryByBinSize (kkint32 binSize)  const
{
  LLoydsBinEntryMap::const_iterator  idx;
  idx = binEntryMap.find (binSize);
  if  (idx == binEntryMap.end ())
    return NULL;

  if  (idx->second->LLoydsBinSize () != binSize)
    return NULL;

  return  idx->second;
}  /* LLoydsEntryByBinSize */



