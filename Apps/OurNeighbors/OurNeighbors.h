#ifndef  _OURNEIGHBORS_
#define  _OURNEIGHBORS_

#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;

#include "ImageFeatures.h"
#include "MLClass.h"
#include "PicesApplication.h"
using namespace  MLL;

#include "Neighbor.h"
#include "ClassSummary.h"


class  OurNeighbors: public PicesApplication
{
public:
	OurNeighbors ();

	~OurNeighbors();

  void  InitalizeApplication (kkint32 argc,
                              char**  argv
                             );

	void	LookForNeighbors ();

	virtual const char* ApplicationName () const  {return  "OurNeighbors";}

	void	DisplayCommandLineParameters ();

	virtual bool ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                        const KKStr&  parmValue
                                       );

  void    RandomReport (ImageFeaturesList&  images);

private:
  LLoydsEntryListPtr   DeriveAllLLoydsBins (const ImageFeaturesList&  examples);


  LLoydsEntryPtr       DeriveLLoydsBins (const ImageFeaturesList&  examples,
                                         kkint32                   lloydsBinSize
                                        );

  MLClassPtr           DetermineClassFromFileName (const  KKStr&  fileName);

  kkint32              LastScanLine (const ImageFeaturesList&  images)  const;

  void                 RemoveExcludedClasses (ImageFeaturesListPtr&  examples);

  void                 ReportDuplicates ();



  kkint32               bucketSize;
  bool                  cancelFlag;
  MLClassListPtr        excludedClasses;   // List of classes to exclude from report.
  FactoryFVProducerPtr  fvProducerFactory;
  KKStr                 featureFileName;
  KKStr                 fromPlanktonName;
  MLClassPtr            fromPlankton;
  ostream*              html;
	ofstream*             htmlFile;
	KKStr                 htmlFileName;
	MLClassPtr            mlClass;
	MLClassListPtr        mlClasses;
  kkint32               lastScanLine;  // Last scan line in SIPPER file;  
  kkint32               baseLLoydsBinSize;
  KKStr                 lloydsBinsFileName;
  NeighborType          neighborType;
  kkint32               numOfIterations;
  kkint32               numOfBuckets;
  bool                  randomizeLocations;
	ostream*              report;
	ofstream*             reportFile;
	KKStr                 reportFileName;
	KKStr                 sourceRootDirPath;
	KKStr                 startDirectory;
};

#endif
