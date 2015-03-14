#if  !defined(_DATABASEIMAGEGROUPENTRY_)
#define  _DATABASEIMAGEGROUPENTRY_


#include "KKBaseTypes.h"
#include "KKStr.h"
#include "KKQueue.h"
using namespace  KKB;

namespace MLL 
{
  #if  !defined(_DATABASEIMAGE_)
    class  DataBaseImage;
    typedef  DataBaseImage*  DataBaseImagePtr;
    class  DataBaseImageList;
    typedef  DataBaseImageList*  DataBaseImageListPtr;
  #endif


  class DataBaseImageGroupEntry
  {
  public:
    typedef  DataBaseImageGroupEntry*  DataBaseImageGroupEntryPtr;

    DataBaseImageGroupEntry (const DataBaseImageGroupEntry&  imageGroupEntry);

    DataBaseImageGroupEntry (kkint32       _imageGroupId,
                             const KKStr&  _imageFileName
                            );

    ~DataBaseImageGroupEntry ();


    kkint32       ImageGroupId  ()  const  {return  imageGroupId;}
    const KKStr&  ImageFileName ()  const  {return  imageFileName;}

    void    ImageGroupId  (kkint32       _imageGroupId)   {imageGroupId  = _imageGroupId;}
    void    ImageFileName (const KKStr&  _imageFileName)  {imageFileName = _imageFileName;}

  private:
    kkint32  imageGroupId;
    KKStr    imageFileName;
  };  /* DataBaseImageGroupEntry */

  typedef  DataBaseImageGroupEntry::DataBaseImageGroupEntryPtr  DataBaseImageGroupEntryPtr;



  class  DataBaseImageGroupEntryList:  public  KKB::KKQueue<DataBaseImageGroupEntry>
  {
  public:
    typedef  DataBaseImageGroupEntryList*  DataBaseImageGroupEntryListPtr;

    DataBaseImageGroupEntryList (bool  _owner);

    DataBaseImageGroupEntryList (const DataBaseImageGroupEntryList&  imageGroupEntries);

    DataBaseImageGroupEntryList (kkint32                     groupId,
                                 const DataBaseImageListPtr  images
                                );

    ~DataBaseImageGroupEntryList ();
  };

  typedef  DataBaseImageGroupEntryList::DataBaseImageGroupEntryListPtr  DataBaseImageGroupEntryListPtr;

}  /* namespace MLL { */


#endif
