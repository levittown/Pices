#if  !defined(_INSTRUMENTDATALIST_)
#define _INSTRUMENTDATALIST_


/**
 *@class MLL::InstrumentDataList
 *@details
 *  Manages all the instrument data for one single Sipper File.  'InstrumentDataFileManager' is responsible
 *  for managing all the 'InstrumentDataList' objects.  The data comes from the SipperFiles where it is embedded.                                           *
 *@author  Kurt Kramer
 */

#include "KKBaseTypes.h"
#include "KKQueue.h"
#include "RunLog.h"
using namespace KKB;

#include "InstrumentData.h"

namespace  MLL
{
#ifndef  _SIPPERFILE_
  class  SipperFile;
  typedef  SipperFile*  SipperFilePtr;
#endif


  class InstrumentDataList :  public KKQueue<InstrumentData>
  {
  public:
    typedef  KKB::kkint32   kkint32;
    typedef  KKB::kkuint32  kkuint32;

    InstrumentDataList (bool  _owner);
  
  
    InstrumentDataList (const KKStr&   _fileName,
                        bool&          _loadWasSucessfull,
                        RunLog&        _log
                       );
  
  
    ~InstrumentDataList ();
  
    typedef  InstrumentDataList*  InstrumentDataListPtr;
  
    static  InstrumentDataListPtr   CreateFromSipperFile (const KKStr&   fullSipperFileName,
                                                          SipperFilePtr  sipperFile,
                                                          VolConstBool&  cancelFlag,
                                                          RunLog&        log
                                                         );
  
    void  Save (const KKStr&  fileName,
                RunLog&       log
               )  const;
  
  
    /**
     *@brief Calculates the offset, in meters, from the beginning of a SIPPER file for each frame.
     *@details Calculates the offset, in meters, from the beginning of a SIPPER file for each frame.
     *         Data will be assumed to be in scan line order.  Flow-rate and Scan-rate will be used as
     *         part of the calculations.
     */
    VectorDouble  FrameOffsetsInMeters (kkuint32 scanLinesPerFrame,
                                        float  scanRate,             // Scan lines per second.
                                        float  defaultFlowRate       // If a Instrument Data record has a FlowRate <= 0.0 the default rate will be used.
                                       );
  
    
    InstrumentDataPtr  SearchForNearestScanLine (kkuint32  scanLine);
  
    
    void  SipperFileName  (const KKStr&  _sipperFileName)   {sipperFileName = _sipperFileName;}
  
  
    const KKStr&  SipperFileName ()  const  {return  sipperFileName;}
  
  
    VectorFloat  GetVolumePerMeterProfile (SipperFilePtr  sipperFile,
                                           VolConstBool&  cancelFlag,
                                           RunLog&        log
                                          ) const;
  
  
    VectorUlong  GetScanLinesPerMeterProfile ()  const;
  
    
    float ScanRate () const  {return  scanRate;}
    void  ScanRate (float _scanRate)  {scanRate = _scanRate;}
  
    void  ThereIsDepthData    (bool  _thereIsDepthData)     {thereIsDepthData    = _thereIsDepthData;}
    void  ThereIsFlowRateData (bool  _thereIsFlowRateData)  {thereIsFlowRateData = _thereIsFlowRateData;}
  
    
  
  private:
    void  Load (const KKStr&   fileName,
                bool&          _loadWasSucessfull,
                RunLog&        log
               );
  
    void  MakeSureFlowRateAndDepthDataIsThere (bool&  changesMade);
  
    /** @brief  If there was no Depth data on the SipperFile we want to populate the Depth field from the SipperFile::Depth ()  */
    void  MakeSureDepthDataIsThere (SipperFilePtr  sipperFile,  
                                    bool&          changesMade
                                   );
  
  
  
    kkint32  fileVersion;      /**< Comes from the text data file stored in "%PicesHomeDir%\DataFiles\InstrumentData"
                                 * Allows us to determine if we have a current version of the file.
                                 */
         
    float    scanRate;         /**< Camera Scan Rates   Lines/Sec  */
    KKStr    sipperFileName;   /**< Root name only.                */
    bool     thereIsDepthData;
    bool     thereIsFlowRateData;
  };
  
  typedef  InstrumentDataList::InstrumentDataListPtr  InstrumentDataListPtr;













  class InstrumentDataConstList :  public InstrumentDataList
  {
  public:
    typedef  KKB::kkint32   kkint32;
    typedef  KKB::kkuint32  kkuint32;

    InstrumentDataConstList (bool  _owner);
  
  
    InstrumentDataConstList (const InstrumentDataList& list);


    InstrumentDataConstList (const KKStr&  _fileName,
                             bool&         _loadWasSucessfull,
                             RunLog&       _log
                            );
  
  
    ~InstrumentDataConstList ();
  
    typedef  InstrumentDataConstList*  InstrumentDataConstListPtr;
  
    static  InstrumentDataListPtr   CreateFromSipperFile (const KKStr&   fullSipperFileName,
                                                          SipperFilePtr  sipperFile,
                                                          VolConstBool&  cancelFlag,
                                                          RunLog&        log
                                                         );
  
    void  Save (const KKStr&  fileName,
                RunLog&       log
               )  const;
  
  
    /**
     *@brief Calculates the offset, in meters, from the beginning of a SIPPER file for each frame.
     *@details Calculates the offset, in meters, from the beginning of a SIPPER file for each frame.
     *         Data will be assumed to be in scan line order.  Flow-rate and Scan-rate will be used as
     *         part of the calculations.
     */
    VectorDouble  FrameOffsetsInMeters (kkuint32 scanLinesPerFrame,
                                        float    scanRate,             // Scan lines per second.
                                        float    defaultFlowRate       // If a Instrument Data record has a FlowRate <= 0.0 the default rate will be used.
                                       );
  
    
    InstrumentDataConstPtr  SearchForNearestScanLine (kkuint32  scanLine);
  
    
    void  SipperFileName  (const KKStr&  _sipperFileName)   {sipperFileName = _sipperFileName;}
  
  
    const KKStr&  SipperFileName ()  const  {return  sipperFileName;}
  
  
    VectorFloat  GetVolumePerMeterProfile (SipperFilePtr  sipperFile,
                                           VolConstBool&  cancelFlag,
                                           RunLog&        log
                                          ) const;
  
  
    VectorUlong  GetScanLinesPerMeterProfile ()  const;
  
    
    float ScanRate () const  {return  scanRate;}
    void  ScanRate (float _scanRate)  {scanRate = _scanRate;}
  
    void  ThereIsDepthData    (bool  _thereIsDepthData)     {thereIsDepthData    = _thereIsDepthData;}
    void  ThereIsFlowRateData (bool  _thereIsFlowRateData)  {thereIsFlowRateData = _thereIsFlowRateData;}
    
  
  private:
    void  Load (const KKStr&   fileName,
                bool&          _loadWasSucessfull,
                RunLog&        log
               );
  
    void  MakeSureFlowRateAndDepthDataIsThere (bool&  changesMade);
  
    /** @brief  If there was no Depth data on the SipperFile we want to populate the Depth field from the SipperFile::Depth ()  */
    void  MakeSureDepthDataIsThere (SipperFilePtr  sipperFile,  
                                    bool&          changesMade
                                   );
  
  
  
    kkint32  fileVersion;      /**< Comes from the text data file stored in "%PicesHomeDir%\DataFiles\InstrumentData"
                                 * Allows us to determine if we have a current version of the file.
                                 */
         
    float    scanRate;         /**< Camera Scan Rates   Lines/Sec  */
    KKStr    sipperFileName;   /**< Root name only.                */
    bool     thereIsDepthData;
    bool     thereIsFlowRateData;
  };
  
  typedef  InstrumentDataList::InstrumentDataListPtr  InstrumentDataListPtr;

}  /* MLL */

#endif


