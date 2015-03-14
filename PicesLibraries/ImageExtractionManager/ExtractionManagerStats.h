#if  !defined(_IEM_EXTRACTIONMANAGERSTATS_)
#define  _IEM_EXTRACTIONMANAGERSTATS_


namespace  ImageExtractionManager
{ 
  class  ExtractionManagerStats
  {
  public:
    friend class ExtractionManager;
    ExtractionManagerStats ();
    ExtractionManagerStats (const ExtractionManagerStats&  stats);
    ~ExtractionManagerStats ();

    kkuint64  BytesRead          () const  {return bytesRead;}
    kkuint32  FramesRead         () const  {return framesRead;}
    kkuint32  FramesProcessed    () const  {return framesProcessed;}
    kkuint32  ScanLinesProcessed () const  {return scanLinesProcessed;}
    kkuint32  ImagesFound        () const  {return imagesFound;}
    kkuint32  ImagesClassified   () const  {return imagesClassified;}
    kkuint32  ImagesUpdated      () const  {return imagesUpdated;}
    kkuint32  DuplicatesDetected () const  {return duplicatesDetected;}
    kkuint32  UpdateFailures     () const  {return updateFailures;}
    kkuint32  FramesOnQueue      () const  {return framesOnQueue;}
    kkuint32  ImagesOnQueue      () const  {return imagesOnQueue;}

  private:
    kkuint64  bytesRead;
    kkuint32  framesRead;
    kkuint32  framesProcessed;
    kkuint32  scanLinesProcessed;
    kkuint32  imagesFound;
    kkuint32  imagesClassified;
    kkuint32  imagesUpdated;
    kkuint32  duplicatesDetected;
    kkuint32  updateFailures;
    kkuint32  framesOnQueue;
    kkuint32  imagesOnQueue;
  };  /* ExtractionManagerStats */
}

#endif
