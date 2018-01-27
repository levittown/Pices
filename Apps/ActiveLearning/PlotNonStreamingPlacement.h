#ifndef  _PLOTNONSTREAMINGPLACEMENT_
#define  _PLOTNONSTREAMINGPLACEMENT_

#include "ClassGroupTotals.h"
#include "MLClass.h"
#include "SortOrderType.h"


class  PlotNonStreamingPlacement: public  Application
{
public:
  PlotNonStreamingPlacement ();

  ~PlotNonStreamingPlacement ();

  virtual const  char*  ApplicationName ()  {return  "PlotNonStreamingPoints";}


  virtual  void  InitalizeApplication (kkint32 argc,  char**  argv);

  void  ReportNonStreamPlacesMean ();

  virtual bool ProcessCmdLineParameter (KKStr  parmSwitch, 
                                        KKStr  parmValue
                                       );


private:

  MLClassList          mlClasses;
  KKStr                rootDir;
  SortOrderType        sortOrder;
  ClassGroupTotalsPtr  totals;  
};


typedef  PlotNonStreamingPlacement*  PlotNonStreamingPlacementPtr;


#endif