#if  !defined(_RANDOMSPLITJOB_)
#define  _RANDOMSPLITJOB_

#include  "ClassificationBiasMatrix.h"
#include  "KKJob.h"


#if  !defined(_RandomSplitJobManagerDefined_)
class    RandomSplitJobManager;
typedef  RandomSplitJobManager*  RandomSplitJobManagerPtr;
#endif



class  RandomSplitJob:  public  KKJob
{
public:
  typedef  RandomSplitJob*  RandomSplitJobPtr;

  RandomSplitJob (JobManagerPtr    _manager,
                  int              _jobId,
                  int              _parentId,
                  int              _spltNum,
                  RunLog&          _log
                 );


  RandomSplitJob (JobManagerPtr  _manager);


  RandomSplitJob (const RandomSplitJob&  j);

  ~RandomSplitJob ();

  RandomSplitJobManagerPtr   Manager ();
  ConfusionMatrix2Ptr        RandomSplitsResults ()  {return randomSplitsResults;}
  int                        SplitNum            ()  {return splitNum;}
 
  // All classes derived from 'Job' will be required to implement this method 
  // and register it with Job::RegisterConstructor
  static  KKJobPtr  CreateEmptyInstance (JobManagerPtr  _manager);
    

  //**************************  Methods that derived classes must implement  ******************************
  virtual  RandomSplitJobPtr  Duplicate ()  const;  // Create a duplicate instance.


  virtual  const char*   JobType ()  const;     /**< This will allow us to know which specific implementation of 'Job'  an instance really is. */

  virtual  void   ProcessNode ();

  virtual  void   ReFresh (KKJob&  j);

  virtual  KKStr  ToStatusStr ();      /**< Any SubClass that implements this method must call its base class version 1st.  */


  /**
   * @brief Whenever the 'KKJob::ProcessStatusStr' method can not identify a field it will call this method to let the derived Class process the field.
   */
  virtual
    void  ProcessStatusField (const KKStr&  fieldName,
                              const KKStr&  fieldValue
                             );

  //                   *** CompletedJobData ***
  // This method will get called right after the "Job" status line gets written when a Job is completed.  
  // See 'KKJobManager::GetNextSetOfJobs'.  If a job needs to write more data to the Status file then you want to 
  // put on a single status line this is where you would do it.   You write all the text in a format that you 
  // want to support.  'KKJobManager' will bracket it with <Job JobType=Job::JobType, JobId=####>   and </Job>
  // ex:
  // <RandomSplitJob>
  //    Job Data;  
  //    Job Data,
  //    etc. etc. etc.
  // </RandomSplitJob>
  virtual  
    void  CompletedJobDataWrite (ostream& o);


  // Works with 'WriteCompletedJobData';  You use this to load in data written by 'WriteCompletedJobData'
  virtual  
    void  CompletedJobDataRead (istream& i);



  //*******************************************************************************************************


  void  GetClassCounts (VectorFloat&   classAccs,
                        VectorDouble&  knownCounts,
                        VectorDouble&  predCounts,
                        VectorDouble&  adjCounts,
                        VectorDouble&  adjCountsStdError,
                        VectorDouble&  predDelta,        // The difference between Predicted Counts and Known
                        VectorDouble&  adjDelta          // The difference between Adjusted  Counts and Known
                       );

  void  PrintClassCounts (ostream& o);


private:
  int  splitNum;

  // Results;

  ClassificationBiasMatrixPtr  biasParms;
  ConfusionMatrix2Ptr          randomSplitsResults;
};


typedef  RandomSplitJob::RandomSplitJobPtr  RandomSplitJobPtr;


#define  _RandomSplitJobDefined_



#endif
