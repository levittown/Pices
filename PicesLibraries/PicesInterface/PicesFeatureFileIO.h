#pragma once


namespace PicesInterface 
{
  public  ref class PicesFeatureFileIO
  {
  public:
    PicesFeatureFileIO (String^  driverName);

  private:
    ~PicesFeatureFileIO ();

  protected:
    !PicesFeatureFileIO ();

  public:
    ///<summary>Returns list of registered feature data file drivers; you will be able to filter for the drivers that 
    /// you want using the two provided parameters.</summary>
    /// <param name='canRead'>Indicates in included drivers must be able to read a feature-data-file.</param>
    /// <param name='canWrite'>Indicates if driver can write  a feature-data-file.</param>
    static  array<String^>^  RegisteredDriverNames (bool  canRead,
                                                    bool  canWrite
                                                   );

    static  bool             ValidDriverName (String^ _driverName);

    property  bool     CancelLoad         {bool     get ();   void set (bool _cancelLoad); }
    property  bool     CanRead            {bool     get ();}
    property  bool     CanWrite           {bool     get ();}
    property  bool     ChangesMade        {bool     get ();}
    property  String^  DriverName         {String^  get ();} 
    property  uint     NumExamplesWritten {uint     get ();}
    property  bool     Successful         {bool     get ();}
    property  bool     Valid              {bool     get ();}


    void  SaveFeatureFile (String^                  _fileName, 
                           PicesFeatureVectorList^  _examples,
                           PicesRunLog^             _runLog
                          );

    void  SaveFeatureFileNormalized (String^                  _fileName, 
                                     PicesFeatureVectorList^  _examples,
                                     PicesRunLog^             _runLog
                                    );

    PicesFeatureVectorList^   LoadFeatureFile (String^          _fileName, 
                                               PicesClassList^  _classes, 
                                               PicesRunLog^     _runLog
                                              );


  private:
    bool*             cancelLoad;
    bool*             changesMade;
    FeatureFileIOPtr  driver;
    String^           lastError;
    uint*             numExamplesWritten;
    bool*             successful;
  };

}  /* PicesInterface */
