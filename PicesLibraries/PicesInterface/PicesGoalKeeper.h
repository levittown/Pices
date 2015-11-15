#ifndef  _PICESGOALKEEPER_
#define  _PICESGOALKEEPER_


namespace PicesInterface 
{

  ///<summary>This class is used to managed synchronization between threads in the same application.</summary>
  public ref class PicesGoalKeeper
  {
  public:
    PicesGoalKeeper (String^  _name);

   
    ///<summary>Initiates a Block so that no other thread can lock this instance of PicesGoalKeeper.</summary>
    ///<remarks>
    /// If this instance of PicesGoalKeeper is already locked (blocked = false)  then will in this function sleeping until 
    /// it is no longer blocked(blocked = false).  If we are the thread that holds the block then we will be allowed to
    /// proceed.  For each time a thread calls this method in a row the variable 'blockerDepth' will be incremented. The
    /// block will not be released until the thread that has the lock has called the method 'EndBlock' enough times
    /// to reduce 'blockerDepth' to zero.
    ///</remarks>
    void   StartBlock ();

    void   EndBlock ();

    property  bool  Blocked {bool  get ()  {return  blocked;}}
    property  int   NumBlockedThreads  {int  get ()  {return numBlockedThreads;}}  /*!< Indicates the number of threads waiting in 'StartBlock' */

  private:
    bool  blocked;           /**< 'true' = Currently Blocked. */
  
    int   blockerThreadId;   /**< Managed ThreadId of thread that currently holds the Block  -1 indicates no Block */

    int   blockerDepth;      /**< Indicates how many times the thread that currently holds the black has called  "StartBlock".  For every time 
                              * the thread that holds the Block calls "StartBlock"  it will have to call "EndBlock"  before the block is actually 
                              * released.
                              */
    
    String^  name;           /**< Name that the instance of PicesGoalKeeper will be known as. */
  
  
    int      numBlockedThreads;
  
  };  /* PicesGoalKeeper */

}

#endif
