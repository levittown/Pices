#ifndef  __CLASSCOUNTSEARCH__
#define  __CLASSCOUNTSEARCH__

#include "Application.h"
#include "HTMLReport.h"
using namespace  KKB;

#include "ConfusionMatrix2.h"
#include "FactoryFVProducer.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "TrainingConfiguration2.h"
using  namespace  KKMLL;

#include "PicesApplication.h"
#include "PicesTrainingConfiguration.h"
using  namespace  MLL;


class  ClassCountSearch: public PicesApplication
{
public:
  typedef  KKB::uint  uint;

  class  ClassStats;
  typedef  ClassStats*  ClassStatsPtr;

  class  ClassStatsList;
  typedef  ClassStatsList*  ClassStatsListPtr;


  ClassCountSearch ();
  ~ClassCountSearch ();

  virtual
  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );

  void  Main ();

  virtual const char* ApplicationName () const  {return  "ClassCountSearch";}

  virtual bool  ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                         const KKStr&  parmValue
                                        );

private:
  void  DisplayCommandLineParameters ();

  MLClassPtr  SelectNextClassToRemove (ConfusionMatrix2Ptr  cm);

  ConfusionMatrix2Ptr  GradeClassList (MLClassListPtr  classes,
                                       kkint32&        numExamples, 
                                       kkint32&        numIgnored
                                      );

  ClassStatsListPtr  ComputeClassStatsList (ConfusionMatrix2Ptr  cm);

  void       ProcessClassCombo (MLClassListPtr  classes);

  void       ReportResults ();
   

  bool                   cancelFlag;

  KKStr                  groundTruthDirName;
  FeatureVectorListPtr   groundTruth;
  MLClassListPtr         groundTruthClasses;

  MLClassListPtr         mlClasses;

  ofstream*              report;
  ofstream*              report1Type1Errors;
  KKStr                  reportFileName;

  FeatureVectorListPtr   trainExamples;
  MLClassListPtr         trainExamplesClasses;

  queue<MLClassListPtr>  combinationsToTry;           

};

#endif