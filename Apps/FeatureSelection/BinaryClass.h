#ifndef  _BINARYCLASS_
#define  _BINARYCLASS_


enum class  ClassPairStatus 
{
  Null, 
  NotStarted, 
  Started, 
  Done
};


#include "KKQueue.h"
using namespace  KKB;


#include "MLClass.h"
#include "SVMparam.h"
using namespace  KKMLL;


#include "FeatureSelectionTypes.h"


namespace FeatureSelectionApp
{
  class BinaryClass
  {
  public:
    BinaryClass (MLClassPtr  _class1,
                 MLClassPtr  _class2,
                 int         _processorId
                );

    BinaryClass (int         _numProcessors,
                 bool        _finalReportJobs,
                 MajorSteps  _majorSetp
                );

    BinaryClass (KKStr           statusStr,
                 MLClassListPtr  mlClasses
                );

    // For creating a Final Results Processor;  that will manage random splits.
    BinaryClass (int                  _numProcessors,
                 FinalResultType      _resultType,
                 SVM_SelectionMethod  _selectionMehod,
                 const KKStr&         _finalResultsFileName,
                 const KKStr&         _configFilename
                );

    ~BinaryClass ();

    MLClassPtr          Class1        () const  {return class1;}
    MLClassPtr          Class2        () const  {return class2;}
    int                 NumProcessors () const  {return numProcessors;}
    ClassPairStatus     Status        () const  {return status;}

    const KKStr&        Class1Name    () const;
    const KKStr&        Class2Name    () const;

    const KKStr&        ConfigFileName        () const  {return configFileName;}
    const KKStr&        FinalResultsFileName  () const  {return finalResultsFileName;}
    FinalResultType     ResultType            () const  {return resultType;}
    SVM_SelectionMethod SelectionMethod       () const  {return selectionMethod;}

    void  DecrementNumProcessors ();
    void  IncrementNumProcessors ();

    void  NumProcessors (int              _numProcessors) {numProcessors = _numProcessors;}
    void  Status        (ClassPairStatus  _status)        {status        = _status;}

    void  ProcessStatusStr  (KKStr           statusStr,
                             MLClassListPtr  mlClasses
                            );

    virtual  KKStr   ToStr ();

    static  KKStr            BinaryClassStatusToStr   (ClassPairStatus  status);
    static  ClassPairStatus  BinaryClassStatusFromStr (KKStr            statusStr);


  private:
    MLClassPtr  class1;
    MLClassPtr  class2;  
    int                 numProcessors;  // The number of processors that are working on this combination.
    ClassPairStatus     status;


    // Fields for Final Report Processes
    KKStr                configFileName;          // Configuration File that should be used.
    FinalResultType      resultType; 
    KKStr                finalResultsFileName;   // name of file where the results of each split will be stored.
    SVM_SelectionMethod  selectionMethod;
  };

  typedef  BinaryClass*  BinaryClassPtr;



  class  BinaryClassList: public  KKQueue<BinaryClass>
  {
  public:
    BinaryClassList ();

  private:

  };

  typedef  BinaryClassList*  BinaryClassListPtr;
}

#endif
