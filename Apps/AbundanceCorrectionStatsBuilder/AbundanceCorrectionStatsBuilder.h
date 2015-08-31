#if  !defined(_ABUNDANCECORRECTIONSTATSBUILDER_)
#define  _ABUNDANCECORRECTIONSTATSBUILDER_

#include "MsgQueue.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


#include "AbundanceCorrectionMatrix.h"
#include "FeatureFileIO.h"
#include "FeatureVector.h"
#include "FileDesc.h"
#include "ImageFeatures.h"
#include "MLClass.h"
#include "NormalizationParms.h"
#include "TrainingConfiguration2.h"
using namespace KKMLL;

#include "PicesApplication.h"
using namespace MLL;

namespace  AbundanceCorrectionApplication
{
  #if  !defined(_TRAINTESTTHREAD_)
  class  TrainTestThread;
  typedef  TrainTestThread*          TrainTestThreadPtr;
  typedef  KKQueue<TrainTestThread>  TrainTestThreadList;
  typedef  TrainTestThreadList*      TrainTestThreadListPtr;
  #endif

  class  AbundanceCorrectionStatsBuilder: public  PicesApplication
  {
  public:
    typedef  KKB::uchar  uchar;
    typedef  KKB::uint   uint;
    typedef  KKB::ulong  ulong;
    typedef  KKB::ushort ushort;

    AbundanceCorrectionStatsBuilder ();

    ~AbundanceCorrectionStatsBuilder ();

    const char*  ApplicationName () const  {return "AbundanceCorrectionStatsBuilder";}

    virtual
    void  InitalizeApplication (kkint32 argc,
                                char**  argv
                               );

    void  Main ();

  private:
    void   CreateInitialThreadInstaces ();

    void   DisplayCommandLineParameters ();

    void   GenerateCrashReport ();

    void   GenerateReportAndStats ();

    void   ManageThreads ();

    void   PrintComandLineParameters ();

    void   PrintStartStatistics ();

    bool   ProcessCmdLineParameter (const KKStr&   parmSwitch, 
                                    const KKStr&   parmValue
                                   );

    void   RemoveDuplicateImages ();

    void   TerminateThreads ();


    MLClassListPtr                allClasses;           /**< Contains  trainLibDataClasses + otherClass               */

    MLClassListPtr                configClasses;

    int                           maxNumActiveThreads;

    MsgQueuePtr                   msgQueue;

    NormalizationParmsPtr         normalizationParms;

    int                           numOfFolds;

    bool                          oneOrMoreThreadsCrashed;

    MLClassPtr                    otherClass;

    FeatureVectorListPtr          otherClassData;

    TrainTestThreadListPtr        queueDone;

    TrainTestThreadListPtr        queueReady;

    TrainTestThreadListPtr        queueRunning;

    KKStr                         reportFileName;

    ostream*                      report;

    FeatureVectorListPtr          trainLibData;         /**< All the examples that were retrieved from the configuration file. */

    MLClassListPtr                trainLibDataClasses;
  };  /* AbundanceCorrectionStatsBuilder */

  typedef  AbundanceCorrectionStatsBuilder*  AbundanceCorrectionStatsBuilderPtr;
}
#endif
