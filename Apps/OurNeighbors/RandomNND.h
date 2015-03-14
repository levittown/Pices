#ifndef  _RANDOMNND_
#define  _RANDOMNND_

#include "Application.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;

#include "MLClass.h"
#include "ImageFeatures.h"
using namespace  MLL;


#include "Neighbor.h"




class  RandomNND
{
public:

	RandomNND (kkint32             _scanLines,
             ImageFeaturesList&  _images,
             kkint32             _numIterations,
             kkint32             _numOfBuckets,
             kkint32             _bucketSize,
             ostream&            _report,
             RunLog&             _log
            );

	~RandomNND();

  void	GenerateReport ();


  double  NND_Mean       ()  const  {return  nnd_Mean;}       /**< Mean NND of all iterations combined.       */
  double  NND_StdDev     ()  const  {return  nnd_StdDev;}     /**< StdDev of NND of all iterations combined.  */
  double  RealDataU2Stat ()  const  {return  realDataU2Stat;}


private:

  void          DeleteData ();

  VectorIntPtr  GetHistogramForARandomDistribution (VectorDouble& allDists,
                                                    double&       mean,
                                                    double&       stdDev,
                                                    double&       min,
                                                    double&       max
                                                   );


  kkint32             bucketSize;
  ImageFeaturesList&  images;
  RunLog&             log;
  double              nnd_Mean;       /**< mean 'nnd' of all iterations combined.             */
  double              nnd_StdDev;     /**< std deviation of 'nnd' of all iterations combined. */
  kkint32             numOfBuckets;
  kkint32             numOfParticles;
  kkint32             numIterations;
  NeighborListPtr     realData;
  double              realDataU2Stat;
	ostream&            report;
  kkint32             scanLines;
  double              widthMax;
};

#endif
