#if  !defined(_DATABASEUPDATE_)
#define  _DATABASEUPDATE_

/**
 *@class  MLL::DataBaseUpdate
 *@brief  Routines used to update the MySql database to the current version.
 *
 */



#include  "DateTime.h"
#include  "GoalKeeper.h"
#include  "Raster.h"
#include  "RunLog.h"
#include  "KKStr.h"
using namespace KKB;



#include  "DataBaseUpdate.h"


namespace MLL 
{
  #if  !defined(_DATABASESERVER_)
  class  DataBaseServer;
  typedef  DataBaseServer*  DataBaseServerPtr;

  class  DataBaseServerList;
  typedef  DataBaseServerList*  DataBaseServerListPtr;
  #endif


  #if  !defined(_DATABASE_)
  class  DataBase;
  typedef  DataBase*  DataBasePtr;
  #endif



  /**
   *@class  DataBaseUpdate
   *@brief DataBaseUpdate connection management object; all database communications will go through this class.
   *@author Kurt Kramer
   *@date Dec/01/2008
   */
  class  DataBaseUpdate
  {
  public:
    typedef  DataBaseUpdate*  DataBaseUpdatePtr;

    typedef  KKB::uchar   uchar;
    typedef  KKB::ulong   ulong;
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;
    typedef  KKB::kkint64 kkint64;
    typedef  KKB::kkuint64  kkuint64;


    DataBaseUpdate (RunLog&  _log);

    ~DataBaseUpdate ();

    void  LoadSourceFile (const KKStr&  fileName,
                          DataBasePtr   dbConn,
                          bool&         successful,
                          KKStrListPtr  errorMsgs,
                          RunLog&       log
                         );


  };  /* DataBaseUpdate */


  typedef  DataBaseUpdate::DataBaseUpdatePtr  DataBaseUpdatePtr;

}  /* namespace MLL */


#endif
