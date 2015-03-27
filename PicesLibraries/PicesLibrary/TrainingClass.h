#if  !defined(_TRAININGCLASS_)
#define  _TRAININGCLASS_
/**
 *@class  MLL::TrainingClass
 *@details
 * You create one instance of this object for each set of training data you want to 
 * specify.  There will be a 'many to one' relationship between 'TrainingClass'  and
 * 'MLClass'.  This is based on the assumption that there can be more than one set
 * of training data that you can use for the same class.
 */



#include "KKStr.h"
#include "KKQueue.h"
using namespace  KKB;

namespace MLL 
{
  #ifndef  _MLCLASS_
  class  MLClass;
  typedef  MLClass*  MLClassPtr;
  typedef  MLClass  const * MLClassConstPtr;
  class  MLClassList;
  typedef  MLClassList*  MLClassListPtr;
  #endif

  #if  !defined(_MLClassConstListDefined_)
    class  MLClassConstList;
    typedef  MLClassConstList*  MLClassConstListPtr;
  #endif




  class  TrainingClass
  {
  public:
    /**
     *@brief Constructor,  Creates a new instance of TrainingClass and populates fields with respective data from parameters.
     *@param[in] _directory
     *@param[in] _name
     *@param[in] _weight
     *@param[in] mlClasses list of MLClasses.  Constructor will update this set 'mlClass' to the MLClass in this list that
     *                     has the same name  as '_name'. If one does not exist it will create a new MLClass object and 
     *                     add it to 'mlClasses'.
     */
    TrainingClass (KKStr              _directory,
                   KKStr              _name,
                   float              _weight,
                   MLClassConstList&  mlClasses
                  );

    TrainingClass (const TrainingClass&  tc);


    const KKStr&        Directory       () const  {return  directory;}
    const KKStr&        FeatureFileName () const  {return  featureFileName;}
    MLClassConstPtr     MLClass         () const  {return  mlClass;}
    const KKStr&        Name            () const;
    float               Weight          () const  {return  weight;}

    KKStr               ExpandedDirectory (const KKStr&  rootDir);

    void  Directory       (const KKStr&     _directory)        {directory       = _directory;}
    void  FeatureFileName (const KKStr&     _featureFileName)  {featureFileName = _featureFileName;}
    void  MLClass         (MLClassConstPtr  _mlClass)          {mlClass         = _mlClass;}
    void  Weight          (float            _weight)           {weight          = _weight;}

  private:
    KKStr  SubstituteInEvironmentVariables (const KKStr&  src);

    KKStr            directory;        /**<  Directory where Training Images are stored. */
    KKStr            featureFileName;  /**<  Name of data file that is to contain feature data for this TrainingClass. */
    MLClassConstPtr  mlClass;          /**< Pointer to MLClass that this TrainingClass is for. */
    float            weight;           /**< Will be used in 'TrainingProcess::ExtractFeatures' to weight images.  
                                        * the SVM Cost parameter from examples in this class will be weighted by this value.
                                        */
  };


  typedef  TrainingClass*  TrainingClassPtr;


  class  TrainingClassList:  public KKQueue<TrainingClass>
  {
  public:
    TrainingClassList (const KKStr&  rootDir,
                       bool          owner
                      );

  private:
    TrainingClassList (const TrainingClassList&  tcl);

  public:
    TrainingClassList (const TrainingClassList&  tcl,
                       bool                      _owner
                      );


    const KKStr&      RootDir ()  {return  rootDir;}
    void              RootDir (const KKStr&  _rootDir)  {rootDir = _rootDir;}

    void              AddTrainingClass (TrainingClassPtr  trainingClass);


    TrainingClassList*  DuplicateListAndContents ()  const;

    TrainingClassPtr    LocateByMLClass (MLClassConstPtr  _mlClass)  const;

    TrainingClassPtr    LocateByMLClassName (const KKStr&  className);

    TrainingClassPtr    LocateByDirectory (const KKStr&  directory);

  private: 
    KKStr   rootDir;
  };  /* TrainingClassList */


  typedef  TrainingClassList*  TrainingClassListPtr;

}  /* namespace MLL */


#endif
