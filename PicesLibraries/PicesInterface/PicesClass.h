#ifndef _PICESCLASS_
#define _PICESCLASS_


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;


#include "MLClass.h"
using namespace  KKMLL;

#include "ImageFeatures.h"
using namespace  MLL;


#include "PicesRunLog.h"
using namespace  KKB;


namespace PicesInterface 
{
  ///<summary>Represents a single 'Class' in the machine learning world.</summary>
  ///<remarks>
  /// Machine Learning Class. Keep in mind that when we use the word 'class' in these 
  /// comments we are referring the Machine Learning concept of 'class' and not the c++, 
  /// java, or c# idea. The idea is that for any image or example(more general) a class can 
  /// be assigned to them. This class is to indicate a grouping which will allow us to 
  /// group like images/examples together.
  ///
  /// PicesClass is the Managed Version of the unmanaged class 'MLL::MLClass'.  For 
  /// every instance of this class there will be a matching instance of a 'MLClass' 
  /// object. Also just like 'MLClass' there can only be one instance of a class 
  /// with the same name. This helps simplify other programming because we can compare 
  /// pointers or handles directly to know if we are talking about the same class.  To 
  /// accomplish this, the creation of new Classes is done through a static method in 
  /// the PicesClassList object called PicesClassList::GetUniqueClass.
  /// </remarks>
  public ref  class PicesClass
  {
  public:
    PicesClass (String^  _name,  PicesClass^ _parent);
    PicesClass (String^  _name,  String^     _parentName);

    property  int                 ClassId              {int          get ();}
    property  String^             Description          {String^      get () {return description;}   void set (String^ _description);}
    property  bool                Mandatory            {bool         get () {return mandatory;}     void set (bool    _mandatory);}
    property  String^             Name                 {String^      get () {return name;}}
    property  String^             NameUpper            {String^      get () {return nameUpper;}}
    property  bool                NoiseClass           {bool         get ();}
    property  kkuint16            NumHierarchialLevels {kkuint16     get ();}
    property  PicesClass^         Parent               {PicesClass^  get () {return parent;}        void set (PicesClass^ _parent);}
    property  String^             ParentName           {String^      get ();}
    property  List<PicesClass^>^  Children             {List<PicesClass^>^ get ();}
    property  bool                StoredOnDataBase     {bool         get ();                        void set (bool _StoredOnDataBase);}  
    property  bool                Summarize            {bool         get () {return summarize;}     void set (bool    _summarize);}
    property  bool                UnDefined            {bool         get () {return unDefined;}}                                           /**< Inidicates that class undefined or Noise */

    void    AddAChild  (PicesClass^  child);

    List<PicesClass^>^  BuildListOfDecendents ();


    bool    ChangeNameOfClass (String^ newName);  /**< Will only change the name if no other instance of 'MLClass' is using the new name */

    void    CreateAUnManagedClass ();

    void    DeleteFromDisk (String^  directory);

    PicesClass^  MLClassForGivenHierarchialLevel (uint level);

    bool    IsEqual (PicesClass^ c);

    bool    IsInMySubTree (PicesClass^  candidate);

    
    
    ///<summary>Determines the number of generations between this class and the candidate class.</summary>
    ///<remarks>
    /// We search all our children to determine if Candidate is one our descendent's. If 
    /// it is not we return -1 otherwise we return the number of generations below us.
    /// Our immediate children will be considered 1 generation apart.
    ///</remarks>
    ///<param name='candidate'>Candidate Class that we are searching for amongst out children.</param>
    ///<returns> The number of generations between us and the candidate or -1 if not one of our descendent's.</returns>
    int     NumGenerations (PicesClass^  candidate);

    int     NumOfChildren ();

    bool    ParentLoopCheck (PicesClass^ startingClass);

    void    RemoveAChild (PicesClass^ child);
      
    void    Save (String^  directory);

    PicesClass^  SearchTree (String^  _nameUpper);

    PicesClass^  SearchFamilyTree (String^      _nameUpper,
                                   PicesClass^  _nodeSearchingFrom
                                  );

    ///<summary>Returns a pointer to the unmanaged version of this Class.</summary>
    MLClassPtr  UnmanagedMLClass ();


    static String^  GetClassNameFromDirName (String^  subDir);

    ///<summary>Will change the name of an existing class verifying that a duplicate does not get created; will also 
    ///make sure that the new name is not in use by another class</summary>
    ///<param name='mlClass'> Pointer to existing MLClass instance that you wish to rename. </param>
    ///<param name='newName'> New name that you with to give instance of 'MLClass'</param>
    ///<param name='changeSuccessful'> Will return'True' if change was successful,  only reason it would not work is the name is already used.</param>
    static  void  ChangeNameOfClass (PicesClass^  mlClass, 
                                     String^      newName,
                                     bool%        changeSuccessful
                                    );


  private:
    String^       description;
    bool          mandatory;
    String^       name;
    String^       nameUpper;
    PicesClass^   parent;
    bool          summarize;
    MLClassPtr    unmanagedMLClass;  // Our link to the unmanaged version of this class;
    bool          unDefined;

    List<PicesClass^>^  children;
  
  };  /* PicesClass */

}  /* PicesInterface */

#endif
