#ifndef  _ACCBYFEATURESEL_
#define  _ACCBYFEATURESEL_

#include  "KKQueue.h"
#include  "RunLog.h"
#include  "KKStr.h"

#include  "MLClass.h"
using namespace  KKMLL;

namespace FeatureSelectionApp
{
  class  AccByFeatureSel
  {
  public:
    typedef  AccByFeatureSel*  AccByFeatureSelPtr;

    AccByFeatureSel (MLClassPtr  class1,
                     MLClassPtr  class2,
                     int                 featureNum,
                     float               avgAccGain,
                     float               stdDevAccGain
                    );

    ~AccByFeatureSel ();

    MLClassPtr  Class1        () const {return class1;}
    MLClassPtr  Class2        () const {return class2;}
    const KKStr&        Class1Name    () const;
    const KKStr&        Class2Name    () const;
    int                 FeatureNum    () const {return featureNum;}
    float               AvgAccGain    () const {return avgAccGain;}
    float               StdDevAccGain () const {return stdDevAccGain;}

  private:
    MLClassPtr  class1;
    MLClassPtr  class2;
    int                 featureNum;
    float               avgAccGain;
    float               stdDevAccGain;
  };  /* AccByFeatureSel */

  typedef  AccByFeatureSel::AccByFeatureSelPtr  AccByFeatureSelPtr;

#define  _AccByFeatureSel_Defined_


  class  AccByFeatureSelList:  public KKQueue<AccByFeatureSel>
  {
  public:
    AccByFeatureSelList (bool  _owner);

    void  SortByFeatureNumThenByClassCombo ();

  private:
    class  FeatureNumComparator
    {
    public:
      bool  operator() (AccByFeatureSelPtr  abfs1,
                        AccByFeatureSelPtr  abfs2
                       );
    };
  };

  typedef  AccByFeatureSelList*  AccByFeatureSelListPtr;

}

#endif
