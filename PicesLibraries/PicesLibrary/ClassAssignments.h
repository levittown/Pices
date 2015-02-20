#if  !defined(_CLASSASSIGNMENTS_)
#define  _CLASSASSIGNMENTS_

/**
 *@class MLL::ClassAssignments
 *@author Kurt Kramer
 *@brief Binds MLClass objects to the appropriate number that the Learning Algorithm expects.
 *@details This class will bind MLClass objects to the appropriate num that the learning 
 *         algorithm knows for this class.  When originally implementing this system, this info was 
 *         kept in the MLClass object.  This would cause problems when doing multi-level
 *         classification.  The same MLClass might be known to the L.A. as one number in one level 
 *         and another number in a different level.  This caused us to have to maintain a separate
 *         instantiation of these classes on the SvmModel objects. 
 *
 *         It is sub-classed from multimap<int16, MLClassConstPtr>.  This allows for fast lookup by
 *         number.  There is also a secondary index by MLClass.  This allows us to do a fast 
 *         lookup by class.
 */

#include <map>
#include "KKQueue.h"
#include "RunLog.h"
#include "Str.h"

namespace MLL 
{

  #ifndef  _MLCLASS_
  class MLClass;
  typedef  MLClass*         MLClassPtr;
  typedef  MLClass const *  MLClassConstPtr;
  class MLClassList;
  typedef  MLClassList*  MLClassListPtr;
  #endif


  #if  !defined(_MLClassConstListDefined_)
  class  MLClassConstList;
  typedef  MLClassConstList*  MLClassConstListPtr;
  #endif



  class  ClassAssignments: public  std::multimap<int16, MLClassConstPtr>
  {
  public:
    ClassAssignments (RunLog&  _log);


    /**
    *@brief Creates assignment for all classes in 'classes.
    *@details Constructs a list assigning a number to each class sequentially from 'classes'.
    *         Use this constructor if each class is treated uniquely by the learning algorithm.
    *@param[in] classes, List of classes to make assignments for.
    *@param[in,out] _log, Run log to write messages to
    */
    ClassAssignments (const MLClassConstList&  classes,
                      RunLog&                  _log
                     );



    void  AddMLClass (MLClassConstPtr  mlClass,
                      int16            num
                     );

    MLClassConstPtr      GetMLClassByIndex (size_t idx);

    MLClassConstPtr      GetMLClass    (int16 num)  const;

    MLClassList          GetMLClasses (int16 num)  const;

    int16                GetNumForClass (MLClassConstPtr  mlClass)  const;

    VectorShort          GetUniqueListOfAssignments ()  const;

    void                 Load (const KKStr&  fileName,
                               bool&         successful
                              );

    int32                MemoryConsumedEstimated ()  const;

    /**
    *@brief Loads class assignments from string that was originally generated by ToString
    *@param[in] _toString,  KKStr containing class assignments info, will expect to be formated
    *                       the way ToString() creates them.
    */
    void            ParseToString (const KKStr&  _toString);


    void            Save (const KKStr&  fileName,
                          bool&         successful
                         );

    KKStr           ToString ()  const;

  private:
    typedef  std::map<MLClassConstPtr, int16>  ClassLookUp;
    typedef  ClassLookUp::iterator             ClassLookUpIterator;

    ClassLookUp  classLookUp;
    RunLog&      log;
  };  /* ClassAssignments */;


  typedef  ClassAssignments*  ClassAssignmentsPtr;

}  /* namespace MLL */

#endif
