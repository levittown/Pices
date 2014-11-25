#ifndef  _RANDOMNND_
#define  _RANDOMNND_

#include "RunLog.h"
#include "Str.h"

#include "PicesApplication.h"


#include  "..\OurNeighbors\Neighbor.h"



#ifndef  _IMAGEFEATURES_
class    ImageFeatures;
typedef  ImageFeatures*  ImageFeaturesPtr;
class    ImageFeaturesList;
typedef  ImageFeaturesList*  ImageFeaturesListPtr;
#endif

#ifndef  _IMAGECLASS_
class  ImageClass;
typedef  ImageClass*  ImageClassPtr;
class  ImageClassList;
typedef  ImageClassList*  ImageClassListPtr;
#endif





class  RandomNND: public PicesApplication
{
public:

	RandomNND ();

	~RandomNND();

  virtual
  void  InitalizeApplication (int32   argc,
                              char**  argv
                             );

	virtual const char* ApplicationName ()  const  {return  "RandomNND";}

  void	GenerateReport ();

	void	DisplayCommandLineParameters ();

	virtual bool ProcessCmdLineParameter (const KKStr&  parmSwitch, 
                                        const KKStr&  parmValue
                                       );
private:

  void  DeleteData ();
  void	LookForNeighbors (int  iteration);
  void  PopulateDataRandomly ();
 

	ostream*     report;
	ofstream*    reportFile;
	KKStr        reportFileName;


  NeighborListPtr*  data;

  int      bucketSize;
  int      scanLines;
  double   minRadius;
  double   minSize;
  int      numOfBuckets;
  int      numOfParticles;
  int      numIterations;

  double   colLeft;
  double   colRight;
  double   width;
  double   widthMax;
};

#endif
