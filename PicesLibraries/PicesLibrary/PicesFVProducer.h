#if  !defined(_LARCOSFVPRODUCER_)
#define  _LARCOSFVPRODUCER_

#include "KKBaseTypes.h"
#include "Raster.h"
#include "RunLog.h"
using namespace  KKB;

#include "FactoryFVProducer.h"
#include "FeatureVector.h"
#include "FeatureVectorProducer.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;

#include "ImageFeatures.h"


#define _PicesFVProducer_VersionNum_  335

namespace  MLL
{
  class PicesFVProducer:  public FeatureVectorProducer
  {
  public:
    PicesFVProducer (FactoryFVProducerPtr  factory);

    virtual ~PicesFVProducer ();


    virtual  ImageFeaturesPtr  ComputeFeatureVector (const Raster&     srcImage,
                                                     const MLClassPtr  knownClass,
                                                     RasterListPtr     intermediateImages,
                                                     float             priorReductionFactor,
                                                     RunLog&           runLog
                                                    );


    virtual  ImageFeaturesPtr  ComputeFeatureVectorFromImage (const KKStr&      fileName,
                                                              const MLClassPtr  knownClass,
                                                              RasterListPtr     intermediateImages,
                                                              RunLog&           runLog
                                                             );


    static FileDescConstPtr  DefineFileDescStatic ();

    /**
     *@brief  Returns the 'type_info' of the Feature Vector that this instance of 'FeatureComputer' creates.
     */
    virtual  const type_info*  FeatureVectorTypeId () const;

    virtual  const type_info*  FeatureVectorListTypeId () const;

    virtual  FeatureVectorListPtr  ManufacturFeatureVectorList (bool  owner) const;

    virtual  kkint16  Version ()  const {return _PicesFVProducer_VersionNum_;}

    static   kkint32  MaxNumOfFeatures  ();

  protected:
    virtual  FileDescConstPtr  DefineFileDesc ()  const;


  private:
  };  /* PicesFVProducer */

  typedef  PicesFVProducer*  PicesFVProducerPtr;

#define  _PicesFVProducer_Defined_



  class  PicesFVProducerFactory:  public  FactoryFVProducer
  {
    typedef  PicesFVProducerFactory*  PicesFVProducerFactoryPtr;

    PicesFVProducerFactory ();

  protected:
    /**
     *@brief  A Factory can never be deleted until the application terminates; the atexit method will perform the deletes.
     */
    virtual ~PicesFVProducerFactory ();

  public:

    virtual  FeatureFileIOPtr  DefaultFeatureFileIO ()  const;

    virtual  const type_info*  FeatureVectorTypeId ()  const;

    virtual  const type_info*  FeatureVectorListTypeId ()  const;

    virtual  FileDescConstPtr  FileDesc ()  const;

    virtual  PicesFVProducerPtr  ManufactureInstance (RunLog&  runLog);


    /**
     *@brief Manufactures a instance of a 'ImageFeaturesList' class that will own its contents.
     */
    virtual  ImageFeaturesListPtr  ManufacturFeatureVectorList (bool owner)  const;

    /**
     *@brief  Returns a 'TrainingConfiguration2'  derived instance.
     */
    virtual  TrainingConfiguration2Ptr  ManufacturTrainingConfiguration ()  const;
  

    /**
     *@brief  Returns instance of "PicesFVProducerFactory"  that is registered with "FactoryFVProducer::RegisterFactory".
     *@details If instance does not exist yet out will create an instance and register it.
     */
    static  PicesFVProducerFactory*  Factory (RunLog*  runLog);


  private:
    /**
     *@brief  This instance of 'PicesFVProducerFactory'  will be registered with "FactoryFVProducer::RegisterFactory".
     *@details  It will deleted by the "FactoryFVProducer::FinaleCleanUp" upon application shutdown.
     */
    static  PicesFVProducerFactory*  factory;

  };  /* PicesFVProducerFactory */

  typedef  PicesFVProducerFactory*  PicesFVProducerFactoryPtr;

#define  _PicesFVProducerFactory_Defined_


}  /* MLL */


#endif
