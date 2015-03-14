#ifndef  _LLOYDSENTRY_
#define _LLOYDSENTRY_

#include  "KKBaseTypes.h"
using namespace KKB;

#include  "MLClass.h"
using namespace  MLL;


class  LLoydsEntry 
{
public:
  typedef  KKB::uchar   uchar;
  typedef  KKB::kkuint32  kkuint32;
  typedef  KKB::kkuint32  kkuint32;
  typedef  KKB::ushort  ushort;

  LLoydsEntry  (kkint32         _lloydsBinSize,
                VectorIntPtr  _lloydsBins,
                double        _lloydsIndex
               );

  ~LLoydsEntry ();

  kkint32   LLoydsBin (size_t idx) const;

  kkint32   LLoydsBinSize  ()  const  {return lloydsBinSize;}

  double  LLoydsIndex    ()  const  {return lloydsIndex;}

  kkuint32  NumOfLLoydsBins () const  {return  ((lloydsBins == NULL) ? 0 : (kkuint32)lloydsBins->size ());}

private:
  kkint32          lloydsBinSize;
  double         lloydsIndex;
  double         lloydsBinsMean;
  VectorIntPtr   lloydsBins;
};

typedef  LLoydsEntry*  LLoydsEntryPtr;



class  LLoydsEntryList: public  KKQueue<LLoydsEntry>
{
public:
  LLoydsEntryList (RunLog&  _log);

  LLoydsEntryPtr  LLoydsEntryByBinSize (kkint32 binSize)  const;

  void  PushOnBack (LLoydsEntryPtr  _lloydsEntry);


private:
  typedef  map<kkint32, LLoydsEntryPtr>  LLoydsBinEntryMap;
  typedef  pair<kkint32, LLoydsEntryPtr> LLoydsBinEntryPair;



  LLoydsBinEntryMap  binEntryMap;

  RunLog&  log;
};



typedef  LLoydsEntryList*  LLoydsEntryListPtr;


#endif

