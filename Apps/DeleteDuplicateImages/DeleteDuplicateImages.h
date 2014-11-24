#ifndef  _DELETEDUPLICATEIMAGES_
#define  _DELETEDUPLICATEIMAGES_

#include "MLClass.h"
#include "PicesApplication.h" 



class  DeleteDuplicateImages: public  PicesApplication
{
public:
  DeleteDuplicateImages ();


  ~DeleteDuplicateImages ();


  virtual
  void  InitalizeApplication (int32   argc,
                              char**  argv
                             );

  virtual const 
  char*    ApplicationName () const   {return  "DeleteDuplicateImages";}


  void     DeleteImages ();


private:
  void    DisplayCmdLine ();

  virtual 
  bool     ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                    const KKStr&  parmValue
                                   );


  bool              cancelFlag;
  MLClassConstList  mlClasses;
  ofstream*         r;
  KKStr             reportFileName;
  KKStr             rootDir;
  KKStr             duplicateImagesDir;
};


typedef  DeleteDuplicateImages*  DeleteDuplicateImagesPtr;


#endif
