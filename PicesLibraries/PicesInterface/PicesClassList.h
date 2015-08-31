#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;

//using namespace PicesCSharpClasses;

#include "PicesGoalKeeper.h"
#include "MLClass.h"
#include "PicesClass.h"


namespace PicesInterface 
{

  ref class  ManagedClassLocator;

  public  ref  class  PicesClassList:  public  List<PicesClass^>
  {
  private:
    PicesClassList (int x, int y, int z);

  public:
    PicesClassList ();
    PicesClassList (String^  _directory);
    PicesClassList (PicesClassList^  _classes);
    PicesClassList (const MLClassListPtr  _classes);  /**< Does NOT take ownership of '_classes' */

  protected:
    !PicesClassList ();

  private:
    ~PicesClassList ();

  public:
    property  String^       Directory  {String^      get () {return  directory; }}
    property  PicesClass^   RootNode   {PicesClass^  get () {return  rootNode;  }  void  set (PicesClass^  _rootNode) {rootNode = _rootNode;}}


    static
    PicesClassList^   CreateARootedEmptyClassList ();

    static
    PicesClassList^   CreateFromDirectoryStructure (String^  directoryName);

    static
    PicesClassList^   CreateFromCommaDelStr (String^  s);

    PicesClassList^   ExtractMandatoryClasses ();

    PicesClassList^   ExtractListOfClassesForAGivenHierarchialLevel (uint level);

    /**
     * Returns list of classes that are in this container that have the 'Summarize' flag set to true;  these are 
     * classes that need to have a summary column printed in the classification report for themselves and any 
     * descendants they have.  Example if the 'Crustacea_Copepod' class has this field then there will be a 
     * column in the report that sums up all the Crustacea_Copepod's and their children.
     */
    PicesClassList^   ExtractSummarizeClasses ();


    /**
     * Will generate an array of 2 String's.  Each String will represent a separate line to be 
     * uses as a Class name Header.  Classes will be separated by the "tab('\t')" character.
     * The underscore('_') character in the Class name will be used to deliminate what goes 
     * on which line.  Ex:  The class name "Crustacean_Copepod_Oncea"  will have "Crustacean" 
     * on the 1st line and "Copepod-Oncea" on the second line.
     */
    array<String^>^    ExtractTwoTitleLines ();


    /**
     * Will generate an array of 3 String's.  Each String will represent a separate line to be 
     * uses as a Class name Header.  Classes will be separated by the "tab('\t')" character.
     * The underscore('_') character in the Class name will be used to deliminate what goes 
     * on which line.  Ex:  The class name "Crustacean_Copepod_Oncea"  will have "Crustacean" 
     * on the 1st line, "Copepod" on the second line, and "Oncea" on the third line.
     */
    array<String^>^    ExtractThreeTitleLines ();


    static  PicesClassList^  GetAllKnownClasses ();


    
    /**
     * Using the "_name" parameter will search the global ClassList(allClassesInUniverse)
     * for an existing instance of 'PicesClass' that has the same name. If one is found
     * that is what is returned to the caller; otherwise a new instance is created and returned 
     * to the caller. This new instance will also be added to 'allClassesInUniverse'.
     */
    static  PicesClass^  GetUniqueClass (String^  _name,
                                         String^  _parentName
                                        );


    /**
     * Will locate a PicesClass instance that has the same unmanaged mlClass reference.
     * if none exists then will create a new one and add to 'allClassesInUniverse'.
     */
    static  PicesClass^  GetUniqueClass (MLClassPtr  mlClass);  

    static  PicesClass^  GetUnKnownClassStatic ();

    /**
     *@brief Static method that is meant to only work on 'allClassesInUniverse'.
     *@details  Example use would be when deleting or merging classes.  We would
     *          want to redirect any class that has the srcClass as a parent
     *          to the destClass.
     */
    static  void  PicesClassList::UpdateParent (PicesClass^  srcClass,
                                                PicesClass^  destClass
                                               );

    PicesClass^  LookUpByName (String^  className);

    PicesClass^  LookUpByUnmanagedClass (MLClassPtr  unmanagedClass);

    int          LookUpIndex (PicesClass^ c);

    void         Save  (String^  _directory);

    void         SortByName ();

    /**@brief  Constructs a unmanaged instance of 'MLClassList' that caller will own and be responsible for deleting. */
    MLClassListPtr  ToMLClassConstList ();



  private:
    void  CleanUpUnmanagedResources ();

    static  PicesInterface::PicesGoalKeeper^  getUniqueClassBlocker      // Will make sure that two threads don't try to
      = gcnew PicesInterface::PicesGoalKeeper ("UniqueClassBlocker");    // add the same class name at the same time.

    PicesClass^  LoadSingleClass (String^  fileName);

    void  LoadSubTreeFromDir ();

    void  ClearDirectory ();

    void  SaveSubTree (PicesClass^  subTree);
  
    void  AddClassesUsingSubdirectoryStructure (DirectoryInfo^  di,
                                                PicesClass^     subTree
                                               );

    String^       directory;
    PicesClass^   rootNode;


    //*********************************************************************************
    //*  The following structures support the Concept of only one instance of a       *
    //*  'PicesClass' object for the same name will be created.  A global list of     *
    //*  already existing instances will be maintained by 'allClassesInUniverse'.     *
    //*  The ability to locate an existing instance quickly will be accomplished by   *
    //*  'allClassesInUniverseUnManagedIndex' and 'allClassesInUniverseNameIndex'.    *
    //*                                                                               *
    //*********************************************************************************
    ref  class  Node
    {
    public:
      Node (MLClassPtr   _unmanagedClass,
            PicesClass^  _managedClass
           );

      MLClassPtr   unmanagedClass;
      PicesClass^  managedClass;
      Node^        left;
      Node^        right;
    };  /* Node */
  

    ref class  ManagedClassLocator
    {
    public:
      ManagedClassLocator  ();
      Node^  root;

      bool  Add (MLClassPtr   unmanagedClass,
                 PicesClass^  managedClass
                );

      PicesClass^   LookUp (MLClassPtr  unmanagedClass);
    };  /* ManagedClassLocator */


    ref class  ClassNameComparitor;

    typedef  System::Collections::Generic::SortedList<String^, PicesClass^>  NameIndex;

    static  PicesClassList^       allClassesInUniverse;
    static  ManagedClassLocator^  allClassesInUniverseUnManagedIndex;
    static  NameIndex^            allClassesInUniverseNameIndex;

  };  /* PicesClassList */
}
