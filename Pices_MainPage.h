
/**
 *@mainpage  Pices
 * 
 *@section  Introduction
 *
 * The Pices system is a set of applications that were developed as part of my (Kurt Kramer) Graduate work. I
 * worked closely with my major professors Dr. Dmitry Goldgof and Dr. Lawrence Hall and Marine
 * Scientist Dr. Andrew Remsen. This work was originally done to process the imagery and
 * instrumentation data collected by the SIPPER imaging platform developed by University of 
 * South Florida Marine Science Center of Oceans Technology. It manages data from cruises dating 
 * back to 2002 through 2014 and is being updated to work with the latest implementation of SIPPER 
 * for future deployments.<br>
 *
 * The system consists of several applications and libraries. Most of the code is written in c++
 * and is meant to be O/S neutral. The GUI functionality is written in .net c#.  There is extensive
 * use of Image Processing (IP) and Machine Learning (ML) techniques used in the processing of data.
 * 
 *
 *@section  Applications
 * There are more than 20 applications in the system but only a few comprise the bulk of the
 * functionality
 *
 * List of some but not all applications:
 * - PicesCommander  The main GUI application that provides a thumbnail view of collected plankton images; 
 *                  Provides charting functionality, classification processes, reporting facilities allowing 
 *                  user to analyze data by deployments, stations, instrument parameters(O^2, Depth, Salinity,
 *                  etc...).  Other functionality consists of validation of data, updating of training 
 *                  libraries(Active Learning), and reclassification of imagery based off user improved
 *                  training data.  Randomly extract data from deployments for ground-truthing to
 *                  help gauge classification performance.
 *
 * - WindowsImageExtraction Where raw Sipper data is fed into the system; this application extracts imagery,
 *                  Instrumentation data, computes feature vectors m and performs initial classification that 
 *                  is then fed into the Pices database where the user can then browse the results.
 *
 * - SipperFleViewer A simple viewer application that allows the user to view the raw sipper data, both
 *                  imagery and instrumentation data.
 *
 * - GradeTrainingModel GUI Tool to grade the different training libraries against user ground truthed data;
 *                  generates reports where user can select the classification granularity with respect to
 *                  class hierarchy.
 *
 * - ImportGPS_Data Imports GPS data collected by ship into Pices database.
 *
 * - SpatialAnalysis 
 *
 * - @link CrossValidationApp  CrossValidation @endlink
 *
 * - @link FeatureSelectionApp FeatureSelection @endlink  Application that will tune SVM parameters and image extracted features to 
 *                   maximize classification accuracy;  it was decide to run on a cluster; the more
 *                   CPU's the faster it processes. It can work in a mixed Windows/Linux environment
 *                   only requiring a common file share device.
 * 
 * - SipperInterface GUI Application that is used to operate the SIPPER III imaging platform.
 *
 * - SipperDiskManager - GUI Application that manages the downloading of data from the SIPPER III 
 *                      Proprietary disk format. The forthcoming SIPPER IV will be using standard
 *                      Linux volumes eliminating the need for this application.
 *
 *@section  Libraries
 * There are several libraries that are utilized by the various applications.
 *
 *List of Libraries:
 * - @link KKU BaseLibrary(c++) @endlink  Basic functionality that is common to other libraries and applications. Some examples 
 *                include string management(@link KKU::KKStr KKStr @endlink), Matrix operations(@link MLL::Matrix Matrix @endlink), 
 *                Image Processing(@link KKU::Raster Raster@endlink), Token Parsing(@link KKU::Tokenizer Tokenizer @endlink), 
 *                Statistics, Histogramming, and common operating system specific routines. Most O/S specific code is implemented in the 
 *                module "osServices.cpp" moodule. Thread management is implemented in the two classes @link KKU::KKThread KKThread @endlink and 
 *                @link KKU::KKThreadManager KKThreadManager @endlink. This is an earlier version of the of the KKBase library found 
 *                in "KSquareLibraries".
 *
 * - @link SipperHardware  SipperIstruments(c++) @endlink Classes and functions that are specific to the SIPPER platform are maintained in
 *                this library. Examples are a group of classes meant to process the different SIPPER
 *                file formats going back to the original SIPPER I platform.
 *     -# @link SipperHardware::SipperBuff SipperBuff @endlink Base class to the different SIPPER FIle Formats; provides generalized support for processing SIPPER Files.
 *     -# @link SipperHardware::InstrumentData InstrumentData @endlink Maintains the various instrument data fields that are embedded or associated with a SIPPER file such
 *                       as Depth, Pith, Roll, GPS Coordinates, Temperatue, etc ...
 *     -# @link SipperHardware::InstrumentDataReport InstrumentDataReport @endlink Base class to the different devices that will embed data into
 *                       the SIPPER File,such as the Pitch and Roll sensor, CTD, Battery.  Each specialization of this class will have the knowlede 
 *                       on how to interpret/pase the related data stream.
 *     -# @link SipperHardware::InstrumentDataManager InstrumentDataManager @endlink Central reporting facility that all 
 *                      'InstrumentDataReport' derived classes send there data thru to be fused together resulting in instances
 *                      of 'InstrumentData'.
 * 
 * - @link MLL PicesLibary(c++) @endlink  Classes and Code that support Machine-Learning and specific Pices imaging routines. Other
 *                functionality includes MySQL DataBase. 
 *     -# @link MLL::FeatureFileIO FeatureFileIO @endlink Several common feature data file formats are supported. (sparse, arff, 
 *                c45, etc...)
 *     -# @link MLL::TrainingConfiguration2 Training-Model-Configuration @endlink class and routines that maintain classifier parameters; 
 *                such as classifier type.
 *     -# @link MLL::MLClass  Machine Learning Classes @endlink and containers for tracking lists of classes.
 *     -# Hierarchical Class naming is supported.
 *     -# FeatureData containers  @link MLL::FeatureVectorList "FeatureVectorList" @endlink and @link MLL::ImageFeaturesList "ImageFeaturesList" @endlink, 
                              stratifying by class.
 *     -# @link MLL::NormalizationParms Normalization @endlink for FeatureData.
 *     -# @Link MLL::ConfusionMatrix2 Confusion-Matrix @endlnk
 *     -# @link MLL::CrossValidation CrossValidation @endlink Example 10 fold CV; also (N x X) cross Validation; typically used by grading a classifier.
 *     -# @link MLL::Model Training-Model-Implementations @endlink  Pair-Wise Feature selected SVM, Common Features SVM, UsfCasCor, and Dual.
 *
 * - PicesInterface (cli/c++) A mixed Managed/Unmanaged memory model library its purpose is to provide a interface
 *                           from the ".net" world to the unmanaged world of the O/S neutral c++ code.
 *
 * - ImageExtractionManager (c++) A frame work for extracting and classifying SIPPER imagery data; it will process
 *                           the raw SIPPER data using multiple threads to maximize throughput.
 *
 * - @link JobManagment JobManager @endlink Two classes that simplify the implementation of a parallelizable application. It manages
 *                           discrete jobs with theire interdependencies. So far only RandomSplits makes use of this library; but
 *                           FeatureSelection would be an excelent candidate for utlizing this framework.
 *
 * - SipperFile (c#) .net classes used by the different GUI applications.
 * 
 * - SipperDiskUtilities (c#) Code that interfaces with the Sipper III hard disk. Provides routines that make the
 *                            SipperFles contained on the disk to appear as normal binary data files that can be 
 *                            opened by any .net application.
 * 
 *
 *@section  OutsideLibraries 
 *  I make use of several publicly available libraries:
 *  - <a href="http://www.fftw.org/">FFTW</a> The Fastest Fourier Transform in the West.
 *  - <a href="http://www.zlib.net/">zLib</a>  Used to compress images before storing in database.
 *  - <a href="http://www.csie.ntu.edu.tw/~cjlin/libsvm/">LibSVM</a>  Support Vector Machine.
 */

