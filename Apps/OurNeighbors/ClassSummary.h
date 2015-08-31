#ifndef  _CLASSSUMMARY_
#define _CLASSSUMMARY_


#include "KKBaseTypes.h"
#include "RunLog.h"
using namespace  KKB;


#include "MLClass.h"
#include "LLoydsEntry.h"
using namespace  KKMLL;


class  ClassSummary
{
public:
  typedef  KKB::uchar   uchar;
  typedef  KKB::kkuint32  kkuint32;
  typedef  KKB::kkuint32  kkuint32;
  typedef  KKB::ushort  ushort;

  typedef  ClassSummary*   ClassSummaryPtr;

  ClassSummary ();

  ClassSummary (MLClassPtr          _mlClass,
                LLoydsEntryListPtr  _lloydsBins,
                float               _realDataU2Stat,
                float               _z_Score
               );

  ~ClassSummary ();

  const KKStr&               ClassName ()       const;

  const  MLClassPtr  MLClass ()  const  {return mlClass;}

  kkint32                    LLoydsBin (kkuint32 binNum) const;

  kkint32                    LLoydsBinSize (kkuint32 lloydsEntryIDX) const;

  LLoydsEntryPtr             LLoydsEntryByIndex (kkuint32 idx);

  LLoydsEntryPtr             LLoydsEntryByBinSize (kkint32 binSize);

  kkuint32                   NumOfLLoydsEntries ()  const;

  kkuint32                   NumOfLLoydsBins () const; // Returns the largest number of lloydsBins in 'lloydsEntries'


  float                      U2Stat ()  const  {return realDataU2Stat;}

  float                      ZScore ()  const  {return z_Score;}



private:
  MLClassPtr  mlClass;
  LLoydsEntryListPtr  lloydsEntries;

  float               realDataU2Stat;
  float               z_Score;
};


typedef  ClassSummary::ClassSummaryPtr  ClassSummaryPtr;



class  ClassSummaryList:  public  KKQueue<ClassSummary>
{
public:
  typedef  KKB::uchar   uchar;
  typedef  KKB::kkuint32  kkuint32;
  typedef  KKB::kkuint32  kkuint32;
  typedef  KKB::ushort  ushort;

  ClassSummaryList (RunLog&  _log);


  MLClassListPtr  GetListOfClasses ()  const;

  VectorInt  LLoydsBinSizes ()  const;

  void  SaveLLoydsBinsData (const KKStr&  fileName,
                            const KKStr&  srcDirName,
                            kkint32       lastScanLine,
                            kkint32         baseLLoydsBinSize
                           );


  void  SpatialOverlapReport (ostream&  o)  const;

  void  SummaryReport (ostream&  r)  const;


private:
  kkuint32  NumOfLLoydsBins ()  const;


  float SpatialOverlapIndex (kkint32            binSize,
                             ClassSummaryPtr  c1,
                             ClassSummaryPtr  c2
                            )  const;


  RunLog& log;
};


#endif
