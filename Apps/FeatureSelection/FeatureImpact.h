#ifndef  _FEATUREIMPACT_
#define  _FEATUREIMPACT_

#include  "KKQueue.h"
#include  "RunLog.h"
#include  "KKStr.h"



#include  "BinaryJobList.h"

namespace FeatureSelectionApp
{

#if  !defined(_AccByFeatureSel_Defined_)
class  AccByFeatureSel;
typedef  AccByFeatureSel*  AccByFeatureSelPtr;
class  AccByFeatureSelList;
typedef  AccByFeatureSelList*  AccByFeatureSelListPtr;
#endif


  class FeatureImpact
  {
  public:
    typedef  KKB::uint  uint;
    typedef  FeatureImpact*  FeatureImpactPtr;

    FeatureImpact (BinaryJobList&  jobs);

    ~FeatureImpact ();


    void   Report (ostream&  r);

    void   ReportHTML (ostream&  r);

    FeatureNumList  FeatureThatDontHurt ();

    void  UpdateAccByFeatureSelList (AccByFeatureSelListPtr  accByFeatureSelList,
                                     MLClassPtr      class1,
                                     MLClassPtr      class2
                                    );

  private:
    FileDescPtr   fileDesc;
    RunLog&       log;
    VectorFloat   accuracyMeans;
    VectorFloat   accuracyStdDevs;

    VectorFloat   fMeasureMeans;
    VectorFloat   fMeasureStdDevs;
    vector<uint>  counts;

    vector<VectorFloat>  accuracyImpacts;
    vector<VectorFloat>  fMeasureImpacts;
  };  /* FeatureImpact */


  typedef  FeatureImpact::FeatureImpactPtr  FeatureImpactPtr;
}


#endif

