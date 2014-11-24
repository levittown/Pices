#ifndef  __CLASSCOUNTSEARCH__
#define  __CLASSCOUNTSEARCH__

#include "Application.h"
#include "HTMLReport.h"
using namespace  KKU;

#include "ConfusionMatrix2.h"
#include "MLClass.h"
#include "ImageFeatures.h"
#include "PicesApplication.h"
#include "TrainingConfiguration2.h"
using  namespace  MLL;

class  ClassCountSearch: public PicesApplication
{
public:
  typedef  KKU::uint  uint;

  class  ClassStats;
  typedef  ClassStats*  ClassStatsPtr;

  class  ClassStatsList;
  typedef  ClassStatsList*  ClassStatsListPtr;


  ClassCountSearch ();
  ~ClassCountSearch ();

  virtual
  void  InitalizeApplication (int32   argc,
                              char**  argv
                             );

  void  Main ();

  virtual const char* ApplicationName () const  {return  "ClassCountSearch";}

  virtual bool  ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                         const KKStr&  parmValue
                                        );

private:
  void  DisplayCommandLineParameters ();

  MLClassConstPtr  SelectNextClassToRemove (ConfusionMatrix2Ptr  cm);

  ConfusionMatrix2Ptr  GradeClassList (MLClassConstListPtr  classes,
                                       int32&               numExamples, 
                                       int32&               numIgnored
                                      );

  ClassStatsListPtr  ComputeClassStatsList (ConfusionMatrix2Ptr  cm);

  void       ProcessClassCombo (MLClassConstListPtr  classes);

  void       ReportResults ();



  bool                        cancelFlag;

  KKStr                       groundTruthDirName;
  FeatureVectorListPtr        groundTruth;
  MLClassConstListPtr         groundTruthClasses;

  MLClassConstListPtr         mlClasses;

  ofstream*                   report;
  ofstream*                   report1Type1Errors;
  KKStr                       reportFileName;

  FeatureVectorListPtr        trainExamples;
  MLClassConstListPtr         trainExamplesClasses;

  queue<MLClassConstListPtr>  combinationsToTry;           

};

#endif