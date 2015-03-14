#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;

#include  "DateTime.h"
#include  "KKStr.h"


namespace PicesInterface 
{
  public  ref class PicesKKStr
  {
  public:
    PicesKKStr ();

    static
      KKB::KKStr  SystemStringToKKStr (System::String^  s);

    static
      KKB::KKStrPtr  SystemStringToKKStrPtr (System::String^  s);

    static
      KKB::KKStrListPtr  SystemStringArrayToKKStrListPtr (array<String^>^  list);

    static
      KKB::KKStrListPtr  SystemStringListToKKStrListPtr (List<String^>^  list);

    static
      System::String^  KKStrToSystenStr (const KKB::KKStr&  s);

    static System::Byte      StrToByte      (String^ s);
    static bool              StrToBool      (String^ s);
    static char*             StrToCharStar  (String^ s);
    static System::DateTime  StrToDateTime  (String^ s);
    static double            StrToDouble    (String^ s);
    static float             StrToFloat     (String^ s);
    static int               StrToInt       (String^ s);
    static double            StrToLatitude  (String^ s);
    static long              StrToLong      (String^ s);
    static double            StrToLongitude (String^ s);
    static KKB::uint         StrToUint      (String^ s);

    static System::Int64     StrToInt64     (String^ s);
    static System::UInt64    StrToUInt64    (String^ s);

    static String^           LatitudeToStr  (double latitude);
    static String^           LongitudeToStr (double longitude);
  };


  KKB::KKStr&  operator<< (KKB::KKStr&      left,
                           System::String^  right
                          );

  String^ operator<< (String^      left,
                      KKB::KKStr&  right
                     );
}  /* PicesInterface */
