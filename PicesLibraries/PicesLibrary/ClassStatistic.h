#if !defined(_CLASSSTATISTIC_)

#include "KKBaseTypes.h"
#include "KKStr.h"

#include "MLClass.h"


namespace MLL
{
  /**
   *@brief Used by routines that retrieve Class statistics from FeatureVectorList instances.
   */
  class  ClassStatistic 
  {
  public:
    typedef  KKB::kkuint32  kkuint32;

    ClassStatistic (const ClassStatistic&  right);

    ClassStatistic (MLClassConstPtr  _mlClass,
                    kkuint32         _count
                   );

    kkuint32            Count      ()  const {return  count;}
    MLClassConstPtr     MLClass    ()  const {return  mlClass;}

    void                Increment  ()        {count++;}

    const KKStr&        Name       ()  const;

    const  ClassStatistic&  operator+= (const ClassStatistic&  right);

  private:
    MLClassConstPtr     mlClass;      /**< Does not own the mlClass object.                 */

    kkuint32            count;        /**< Represents the number of imageFeatures objects in a a FeatureVectorList that point to MLClass */
  };

  typedef  ClassStatistic*  ClassStatisticPtr;


  class  ClassStatisticList:  public KKQueue<ClassStatistic> 
  {
  public:
    ClassStatisticList (bool  _owner);

    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;

    ClassStatisticPtr  LookUpByImageClass (MLClassConstPtr  mlClass)  const;

    void  SortByClassName  ();
    void  SortByCount  ();

    void  PushOnBack  (ClassStatisticPtr  stat);
    void  PushOnFront (ClassStatisticPtr  stat);

    void  PrintReport (ostream& r);

    kkint32 operator[]  (MLClassConstPtr  mlClass);
    const  ClassStatisticList&  operator+= (const ClassStatisticList&  right);

  private:
    class  ClassStatisticSortComparrison;

    std::map<MLClassConstPtr, ClassStatisticPtr>  imageClassIndex;

    class  ClassStatSortByCount;
  };  /* ClassStatisticList */

  typedef  ClassStatisticList*  ClassStatisticListPtr;
}  /* namespace MLL */

#define  _CLASSSTATISTIC_

#endif

