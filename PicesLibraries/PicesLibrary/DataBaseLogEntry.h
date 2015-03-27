#ifndef  _DATABASELOGENTRY_
#define  _DATABASELOGENTRY_

/**
 @class  MLL::DataBaseLogEntry
 @details 
 @code
 ************************************************************************************************
 **  Used for Maintaining LogEntries; specifically is used to indicate when Processes started   *
 **  and ended along with the command line used.  Each entry will be assigned a unique ID that  *
 **  other elements can refer to.  For example each entry in the Images table can refer to the  *
 **  log entry that generated the image.  Programs such as "GrayImageExtraction" and            *
 **  "ImageClassification" will generate entries into this table.                               *
 ************************************************************************************************
 @endcode
 */

#ifndef  _mysql_h
struct  MYSQL;
struct  MYSQL_RES;
struct  MSQL_STRUCT; 
typedef  MSQL_STRUCT* MYSQL_ROW;
#endif

#include  "OSservices.h"
#include  "RunLog.h"
using namespace KKB;

namespace MLL 
{

  class  DataBaseLogEntry
  {
  public:
    typedef  DataBaseLogEntry*  DataBaseLogEntryPtr;

    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::ulong    ulong;
    typedef  KKB::uchar    uchar;

    DataBaseLogEntry ();
    
    DataBaseLogEntry (const KKStr&          _progCode,
                      const KKStr&          _progName,
                      const KKB::DateTime&  _dateTimeCompiled,
                      const KKStr&          _cmdLine,
                      const KKStr&          _compName,
                      const KKStr&          _compUserName,
                      const KKStr&          _sipperFileName
                     );

    ~DataBaseLogEntry ();

    kkuint32       LogEntryId        () const  {return logEntryId;};
    const KKStr&   ProgCode          () const  {return progCode;} 
    const KKStr&   ProgName          () const  {return progName;} 
    KKB::DateTime  DateTimeCompiled  () const  {return dateTimeCompiled;} 
    const KKStr&   CmdLine           () const  {return cmdLine;} 
    const KKStr&   CompName          () const  {return compName;} 
    const KKStr&   DataBaseUserName  () const  {return dataBaseUserName;} 
    const KKStr&   CompUserName      () const  {return compUserName;} 
    KKB::DateTime  DateTimeStart     () const  {return dateTimeStart;} 
    KKB::DateTime  DateTimeStartUtc  () const  {return dateTimeStartUtc;} 
    KKB::DateTime  DateTimeEnd       () const  {return dateTimeEnd;};
    float          CpuTimeUsed       () const  {return cpuTimeUsed;} 
    const KKStr&   CompletionStatus  () const  {return completionStatus;}
    const KKStr&   SipperFileName    () const  {return sipperFileName;}

    void   LogEntryId        (kkuint32              _logEntryId)       {logEntryId       = _logEntryId;}
    void   ProgCode          (const KKStr&          _progCode)         {progCode         = _progCode;}
    void   ProgName          (const KKStr&          _progName)         {progName         = _progName;}
    void   DateTimeCompiled  (const KKB::DateTime&  _dateTimeCompiled) {dateTimeCompiled = _dateTimeCompiled;}
    void   CmdLine           (const KKStr&          _cmdLine)          {cmdLine          = _cmdLine;} 
    void   CompName          (const KKStr&          _compName)         {compName         = _compName;} 
    void   DataBaseUserName  (const KKStr&          _dataBaseUserName) {dataBaseUserName = _dataBaseUserName;} 
    void   CompUserName      (const KKStr&          _compUserName)     {compUserName     = _compUserName;} 
    void   DateTimeStart     (const KKB::DateTime&  _dateTimeStart)    {dateTimeStart    = _dateTimeStart;}
    void   DateTimeStartUtc  (const KKB::DateTime&  _dateTimeStartUtc) {dateTimeStartUtc = _dateTimeStartUtc;}
    void   DateTimeEnd       (const KKB::DateTime&  _dateTimeEnd)      {dateTimeEnd      = _dateTimeEnd;}
    void   CpuTimeUsed       (float                 _cpuTimeUsed)      {cpuTimeUsed      = _cpuTimeUsed;}
    void   CompletionStatus  (const KKStr&          _completionStatus) {completionStatus = _completionStatus;}
    void   SipperFileName    (const KKStr&          _sipperFileName)   {sipperFileName   = _sipperFileName;}

  private:
    kkuint32          logEntryId;
    KKStr             progCode;          /**< "IE"=Image Extraction, "CL"=Classification.    */
    KKStr             progName;          /**< Name of application performing process.        */
    KKB::DateTime     dateTimeCompiled;  /**< DateTime program was compiled.                 */
    KKStr             cmdLine;           /**< Command line used to run program.              */
    KKStr             compName;          /**< Name of computer that program ran on.          */
    KKStr             dataBaseUserName;  /**< UserName used to connect to the database with. */
    KKStr             compUserName;      /**< UserNane used to log into computer with.       */
    KKB::DateTime     dateTimeStart;     /**< DateTime process started.                      */
    KKB::DateTime     dateTimeStartUtc;  /**< UTC DateTime process started.                  */
    KKB::DateTime     dateTimeEnd;
    float             cpuTimeUsed;       /**< Number of CPU seconds used during process.     */
    KKStr             completionStatus;  /**< Indicates status of process when completed.    */
    KKStr             sipperFileName;    /**< Root Name of Sipper file; if processed against one SIPPER file. */
  };  /* DataBaseLogEntry */



  typedef  DataBaseLogEntry::DataBaseLogEntryPtr  DataBaseLogEntryPtr;



  class  DataBaseLogEntryList: public  KKQueue<DataBaseLogEntry>
  {
  public:
    typedef  DataBaseLogEntryList*  DataBaseLogEntryListPtr;

    DataBaseLogEntryList (bool  _owner);

    ~DataBaseLogEntryList ();

    void  SortByProgCode ();

  private:
    class  ProgCodeComparison;
  };  /* DataBaseLogEntryList */


  typedef  DataBaseLogEntryList::DataBaseLogEntryListPtr  DataBaseLogEntryListPtr;


}  /* namespace MLL */


#endif
