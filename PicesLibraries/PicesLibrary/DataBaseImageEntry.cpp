#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <memory>

#include  <string>
#include  <iostream>
#include  <fstream>
#include  <vector>
#include  <map>

#include  "MemoryDebug.h"

using namespace std;

#include  "KKBaseTypes.h"
#include  "OSservices.h"
#include  "KKStr.h"
using namespace KKB;


#include  "DataBaseImageEntry.h"
using namespace  MLL;


DataBaseImageEntry::DataBaseImageEntry (const KKStr&  _imageFileName,
                                        float         _depth
                                        ):
    imageFileName (_imageFileName),
    depth         (_depth)
{
}



DataBaseImageEntry::~DataBaseImageEntry ()
{
}




DataBaseImageEntryList::DataBaseImageEntryList (bool _owner):
      KKQueue<DataBaseImageEntry> (_owner)
{
}


DataBaseImageEntryList::~DataBaseImageEntryList ()
{
}


