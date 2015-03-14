#if  !defined(_IMAGECLASSCONSTLIST_)
#define  _IMAGECLASSCONSTLIST_


#include "GoalKeeper.h"
#include "RunLog.h"
#include "KKStr.h"
#include "KKQueueConst.h"


namespace MLL
{
#if  !defined(_MLClassDefined_)
  class  MLClass;
  typedef  MLClass*  MLClassPtr;
  typedef  MLClass  const  MLClassConst;
  typedef  MLClassConst*  MLClassConstPtr;
#endif

#if  !defined(_MLClassListDefined_)
  class  MLClassList;
  typedef  MLClassList*  MLClassListPtr;
#endif


  /**
   *@class MLClassList
   *@brief  Maintains a list of (MLClass const) instances where the list itself is constant.
   *@details  There will be only one instance of each MLClass by name and these instances will 
   *  be owned by MLClass::existingMLClasses.
   */
  class  MLClassConstList:  public KKQueueConst<MLClass>
  {
  public:
    typedef  KKB::kkint32 kkint32;
    typedef  KKB::kkuint32  kkuint32;

    typedef  MLClassConstList*  MLClassConstListPtr;

    MLClassConstList ();

    /** @brief Copy constructor;  will copy list but not own the contents. */
    MLClassConstList (const MLClassConstList&  _mlClasses);

    MLClassConstList (const MLClassList&  _mlClasses);

    /** @brief Construct a MLClassList object from the contents of a file. */
    MLClassConstList (const KKStr&  fileName,
                         bool&         successfull
                        );

    virtual
      ~MLClassConstList ();


    virtual
    void   AddMLClass (MLClassConstPtr  _mlClass);


    static
    MLClassConstListPtr  BuildListFromDelimtedStr (const KKStr&  s,
                                                      char          delimiter
                                                     );


    /** @brief  Clears the contents of this list and updates nameIndex structure. */
    virtual
      void  Clear ();
      

    MLClassConstListPtr  ExtractMandatoryClasses ()  const;

    /**
     *@brief  Using the class names create two title lines where we split
     *        names by "_" characters between the two lines.
     */
    void  ExtractTwoTitleLines (KKStr&  titleLine1,
                                KKStr&  titleLine2 
                               ) const;


    /** 
     *@brief Using the class names create three title lines where we split names 
     *       by "_" characters between the three lines.
     */
    void  ExtractThreeTitleLines (KKStr&  titleLine1,
                                  KKStr&  titleLine2, 
                                  KKStr&  titleLine3 
                                 ) const;

    void  ExtractThreeTitleLines (KKStr&  titleLine1,
                                  KKStr&  titleLine2, 
                                  KKStr&  titleLine3,
                                  kkint32 fieldWidth
                                 ) const;

    /**
     *@brief Will generate a HTML formated string that can be used in a HTML table.
     *@details Using the class names create one header line for a HTML table. The 
     *         underscore character ("_") will be used to separate levels
     */
    KKStr   ExtractHTMLTableHeader () const;


    MLClassConstListPtr  ExtractListOfClassesForAGivenHierarchialLevel (kkint32 level)  const;


    /** @brief  return pointer to instance with '_name'; if none exists, create one and add to list. */
    virtual
    MLClassConstPtr  GetMLClassPtr (const KKStr& _name);

    MLClassConstPtr  GetNoiseClass ()  const;

    /**
     *@brief Return a pointer to the MLClass object that represents the unknown Class in the list. 
     *@details That is the one with the name 'UNKNOWN'. If none is found then one will be created
     * and added to the list.
     */
    MLClassConstPtr   GetUnKnownClass ();


    /**
     *@brief  Returns a pointer of MLClass object with name (_name); if none
     *        in list will then return NULL.
     *@param[in]  _name  Name of MLClass to search for.
     *@return  Pointer to MLClass or NULL  if not Found.
     */
    virtual
    MLClassConstPtr  LookUpByName (const KKStr&  _name)  const;


    virtual
    MLClassConstPtr  LookUpByClassId (kkint32  _classId)  const;


    void           Load (const KKStr&  _fileName,
                         bool&         _successfull
                        );

    kkint32        MemoryConsumedEstimated () const;
      
    static
      MLClassConstListPtr  MergeClassList (const MLClassConstList&  list1,
                                              const MLClassConstList&  list2
                                             );


    KKB::kkuint16  NumHierarchialLevels ()  const;


    virtual
    MLClassConstPtr  PopFromBack ();

    virtual
    MLClassConstPtr  PopFromFront ();

    virtual
    void           PushOnBack (MLClassConstPtr  mlClass);

    virtual
    void           PushOnFront (MLClassConstPtr  mlClass);


    void           Save (KKStr   _fileName,
                         bool&   _successfull
                        );

    void           SortByName ();

    KKStr          ToString ()  const; 

    KKStr          ToCommaDelimitedStr ()  const;

    KKStr          ToTabDelimitedStr ()  const;

    void           WriteXML (std::ostream&  o)  const;

    bool           operator== (const MLClassConstList&  right)  const;

    bool           operator!= (const MLClassConstList&  right)  const;

                                                                       
    MLClassConstList& operator=  (const MLClassConstList&  right);

    MLClassConstList& operator-= (const MLClassConstList&  right);         /**< remove all classes that in the 'right' parameter  */
    
    MLClassConstList  operator-  (const MLClassConstList&  right)  const;  /**< remove all classes that in the 'right' parameter  */

    MLClassConstList& operator+= (const MLClassConstList&  right);         /**< add all classes that are in the 'right' parameter */


  private:
    void  AddMLClassToNameIndex (MLClassConstPtr  _mlClass);


    typedef  map<KKStr,MLClassConstPtr>   NameIndex;
    NameIndex     nameIndex;

    bool      undefinedLoaded;  /**< Indicates if the class that represents examples that have not beed 
                                 *   classified yet has been loaded.
                                 */

    class  mlClassNameComparison;
  };  /* MLClassConstList */

  typedef  MLClassConstList::MLClassConstListPtr  MLClassConstListPtr;

#define  _MLClassConstListDefined_



  std::ostream&  operator<< (      std::ostream&         os, 
                             const MLClassConstList&  classList
                            );


  KKStr&  operator<< (      KKStr&                str, 
                      const MLClassConstList&  classList
                     );


  typedef  MLClassConstList*  MLClassConstListPtr;

}  /* namespace MLL */


#endif

