#if  !defined(_ABUNDANCECORRECTIONMATRIX_)
#define  _ABUNDANCECORRECTIONMATRIX_

#include "KKBaseTypes.h"
#include "DateTime.h"
#include "RunLog.h"
using namespace  KKB;

#include "MLClass.h"
using namespace  KKMLL;

namespace MLL 
{
  #if !defined(_CLASSSTATISTIC_)
  class  ClassStatistic;
  typedef  ClassStatistic*  ClassStatisticPtr;
  class  ClassStatisticList;
  typedef  ClassStatisticList*  ClassStatisticListPtr;
  #endif

  /**
   *@brief  Confusion matrix used to compute adjustment vectors used to make adjustments to classification results.
   *@details
   *@code
   *  From Paper: "Accurate automatic quantification of taxa-specific plankton abundance using dual classification with correction"
   *          by: Qiao Hu, Cabell Davis
   *@endcode
   *    
   */
  class  AbundanceCorrectionMatrix
  {
  public:
    /**
     *@brief  Constructor.
     *@details  The matrix will contain one row and one column for each entry in '_classes'  and one additional row 
     * and column for '_otherClass'.
     *@param[in] _classes    Classes that are to be part of matrix but should not include '_otherClass'; will create a 
     *                       local instance.
     *@param[in] _otherClass This is the class that predictions to any class that is not in '_otherClass' will go to.
     */
    AbundanceCorrectionMatrix (MLClassList&  _classes,
                               MLClassPtr    _otherClass,
                               RunLog&       _log
                              );
    
    AbundanceCorrectionMatrix (const KKStr&  configFileName,
                               RunLog&       _log
                              );
    
    ~AbundanceCorrectionMatrix ();


    const KKB::DateTime&  DateTimeComputed () const {return dateTimeComputed;}
    const VectorFloat&    ProbOfDetection  () const {return probOfDetection;}
    const VectorFloat&    ProbOfFalseAlarm () const {return probOfFalseAlarm;} 
    bool                  Valid            () const {return valid;}


    void  AddIn (const AbundanceCorrectionMatrix&  matrix,
                 RunLog&                           _log
                );

    /**
     *@brief Compute and return abundance adjusted counts.
     *@details Will treat all classes from 'inputCounts' that are not in the internal 'classes' or 'otherClass' as a 'otherClass'.
     *  The adjusted counts will be computed using the stats earlier computed and saved in 'probOfDetection' and 'probOfFalseAlarm'.
     *  AdjustedCount = InputCount * probOfFalseAlarm / probOfDetection;
     *@param[in] inputCounts List of counts returned from the classifier; each entry will consist of the classification and its
     *                       respective count.
     */
    ClassStatisticListPtr   AdjustClassificationCounts (const ClassStatisticList&  inputCounts,
                                                        RunLog&                    log
                                                       );

    void  ComputeStatistics (); /**< computing  probOfDetection  and  probOfFalseAlarm(Specificity) */


    /**@brief Will lump all counts in 'inputCounts' into classes that match this instance 'classList' and 'otherClass' grouping. */
    ClassStatisticListPtr   LumpCounts (const ClassStatisticList&  inputCounts);

    void  Prediction (MLClassPtr  knownClass,
                      MLClassPtr  predClass,
                      RunLog&     log
                     );

    void  PrintConfusionMatrixTabDelimited (std::ostream&  outFile);

    void  ReadXml (std::istream&  i,
                   RunLog&        log
                  );

    void  WriteXml (std::ostream&  o);

    void  SaveForConfigFileName (const KKStr&  configFileName);



  private:
    void  AllocateMemory ();

    void  AllocatePredictionsTable ();

    template<typename T>
    void   AllocateVector (std::vector<T>&  v,  kkuint32 len);

    kkuint32*   AllocateUint32Array (kkuint32 len);

    void  CleanUp ();
    
    kkint32  LookUpClassIdx (MLClassPtr c);

    void  ReadForConfigFileName (const KKStr&  configFileName,
                                 RunLog&       log
                                );

    template<typename T>
    std::istream&  ReadVector (std::vector<T>&  v,
                               std::istream&    i
                         );

    template<typename T>
    KKStr  VectorToStr (std::vector<T>&  v);

    template<typename T>
    KKStr  ArrayToStr (T* a,  kkuint32 len);


    template<typename T>
    void  ReadArray (kkuint32  len,
                     T*&       a,
                     std::istream&  i
                    );


    MLClassList    classes;
    KKB::DateTime  dateTimeComputed;
    kkuint32*      knownByClass;
    kkint32        numClasses;    /**< Number of classes in 'classes' plus one for 'otherClass'. */
    kkuint32       numCorrect;
    kkuint32       numPredictions;
    MLClassPtr     otherClass;
    kkint32        otherClassIdx; /**< Index into 'classes' for 'otherClass'.                    */

    /**
     * A two dimensional matrix where each column will represent known class.  The predictions of the known class
     * will be distributed across rows.  The last row and last column will represent the other class.  The first 
     * N columns and rows will represent the classes listed in "classes".  The concept is that a Dual classifier
     * will be trained on the classes specified in "classes"  but the test data will come from all classes; 
     * that is the classes listed in "classes" and "otherClass".
     */
    kkuint32**        predictions;
    kkuint32*         predByClass;

    VectorFloat       probOfDetection;
    VectorFloat       probOfFalseAlarm;

    bool              valid;
  };  /* AbundanceCorrectionMatrix */

  typedef  AbundanceCorrectionMatrix*  AbundanceCorrectionMatrixPtr;
}  /* MLL */


#endif
