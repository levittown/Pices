
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
 * and is meant to be O/S neutral. The GUI functionality is written in .net c#. There is extensive
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
 *                  etc...). Other functionality consists of validation of data, updating of training 
 *                  libraries(Active Learning), and reclassification of imagery based off user improved
 *                  training data. Randomly extract data from deployments for ground-truthing to
 *                  help gauge classification performance.
 *
 * - WindowsImageExtraction Where raw Sipper data is fed into the system; this application extracts imagery,
 *                  Instrumentation data, computes feature vectors m and performs initial classification that 
 *                  is then fed into the Pices database where the user can then browse the results.
 *
 * - SipperFleViewer A simple viewer application that allows the user to view the raw sipper data, both
 *                  imagery and instrumentation data.
 *
 * - GradeTrainingModel GUI Tool to grade the different training libraries against user ground-truthed data;
 *                  generates reports where user can select the classification granularity with respect to
 *                  class hierarchy.
 *
 * - ImportGPSData Imports GPS data collected by ship into Pices database.
 *
 * - SpatialAnalysis 
 *
 * - CrossValidationApp  Application that will perform various flavors of the tradition n-fold cross validation.
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
 * - @link KKB KKBase(c++) @endlink  Basic functionality that is common to other libraries and applications. Some examples 
 *                include string management(@link KKB::KKStr KKStr @endlink), Matrix operations(@link KKB::Matrix Matrix @endlink), 
 *                Image Processing(@link KKB::Raster Raster @endlink), Token Parsing(@link KKB::Tokenizer Tokenizer @endlink), 
 *                Statistics, Histogramming, and common operating system specific routines. Most O/S specific code is implemented in the 
 *                module "osServices.cpp" module. Thread management is implemented in the two classes @link KKB::KKThread KKThread @endlink and 
 *                @link KKB::KKThreadManager KKThreadManager @endlink. This is an earlier version of the of the KKBase library found 
 *                in "KSquareLibraries".
 *
 * - @link KKMLL KKMachineLearning(c++) @endlink  Classes required to support Machine Learning functionality.
 *     -# @link KKMLL::FeatureFileIO FeatureFileIO @endlink Several common feature data file formats are supported. (sparse, arff, 
 *                c45, etc...)
 *     -# @link KKMLL::TrainingConfiguration2 Training-Model-Configuration @endlink class and routines that maintain classifier parameters; 
 *                such as classifier type.
 *     -# @link KKMLL::MLClass  Machine Learning Classes @endlink and containers for tracking lists of classes.
 *     -# Hierarchical Class naming is supported.
 *     -# @link KKMLL::FeatureVector "FeatureVector" @endlink General purpose container for @link KKMLL::FeatureVector FeatureVector @endlink derived instances.
 *     -# @link KKMLL::FeatureVectorList "FeatureVectorList" @endlink  General purpose container for @link KKMLL::FeatureVector FeatureVector @endlink derived instances.
 *     -# @link KKMLL::NormalizationParms Normalization @endlink for FeatureData.
 *     -# @link KKMLL::ConfusionMatrix2 Confusion-Matrix @endlink Object used to capture prediction information and build a confusion matrix.
 *     -# @link KKMLL::CrossValidation CrossValidation @endlink Example 10 fold CV; also (N x X) cross Validation; typically used by grading a classifier.
 *     -# @link KKMLL::Model Training-Model-Implementations @endlink  Pair-Wise Feature selected SVM, Common Features SVM, UsfCasCor, and Dual.
 *     -# @link KKMLL::FeatureVectorProducer FeatureVectorProducer @endlink  A abstract class used to define the interfaces required for Creating 
 *                     @link KKMLL::FeatureVector FeatureVector @endlink derived instances from a source object. Examples are 
 *                     @link KKMLL::GrayScaleImagesFVProducer GrayScaleImagesFVProducer @endlink computes instances of @link KKMLL::GrayScaleImagesFV GrayScaleImagesFV @endlink
 *                     feature-vectors; or @link MLL::PicesFVProducer PicesFVProducer @endlink produces a Pices specific feature vector.
 *
 * - @link MLL PicesLibary(c++) @endlink  Classes and Code that is specific to the Sipper and Pices World Examples are a group of classes meant 
 *                 to process the different SIPPER file formats going back to the original SIPPER-I platform. A class for MySQL Database interface,
 *                 a specialized Pices version of @link KKMLL::TrainingConfiguration2 TrainingConfiguration2 @endlink called PicesTrainigConfiguration.
 *     -# @link MLL::SipperBuff SipperBuff @endlink Base class to the different SIPPER FIle Formats; provides generalized support for processing SIPPER Files.
 *     -# @link MLL::InstrumentData InstrumentData @endlink Maintains the various instrument data fields that are embedded or associated with a SIPPER file such
 *                       as Depth, Pith, Roll, GPS Coordinates, Temperature, etc ...
 *     -# @link MLL::InstrumentDataReport InstrumentDataReport @endlink Base class to the different devices that will embed data into
 *                       the SIPPER File,such as the Pitch and Roll sensor, CTD, Battery. Each specialization of this class will have the knowledge 
 *                       on how to interpret/parse the related data stream.
 *     -# @link MLL::InstrumentDataManager InstrumentDataManager @endlink Central reporting facility that all 
 *                      'InstrumentDataReport' derived classes send there data thru to be fused together resulting in instances
 *                      of 'InstrumentData'.
 *     -# @link MLL::FeatureFileIOPices FeatureFileIOPices @endlink A Pices specific derived class of @link KKMLL::FeatureFileIO  FeatureFileIO @endlink
 *                      that supports the reading and writing of a Pices specific feature data.
 *     -# @link MLL::PicesTrainingConfiguration PicesTrainingConfiguration @endlink Specialized configuration file for a Pices Training Library.
 *     -# @link MLL::ImageFeatures  ImageFeatures @endlink  A SIPPER specific implementation of @link KKMLL::FeatureVector FeatureVector @endlink class 
 *                      that is aware of SIPPER elements such as CTD Instrumentation data, GPS coordinates, Flow-rates, etc... 
 *
 * - PicesInterface (cli/c++) A mixed Managed/Unmanaged memory model library; its purpose is to provide a interface
 *                           from the ".net" world to the unmanaged world of the O/S neutral c++ code.
 *
 * - ImageExtractionManager (c++) A frame work for extracting and classifying SIPPER imagery data; it will process
 *                           the raw SIPPER data using multiple threads to maximize throughput.
 *
 * - @link JobManagment JobManager @endlink Two classes that simplify the implementation of a parallelizable applications. It manages
 *                           discrete jobs with their interdependencies. So far only RandomSplits makes use of this library; but
 *                           FeatureSelection would be an excellent candidate for utilizing this framework.
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

