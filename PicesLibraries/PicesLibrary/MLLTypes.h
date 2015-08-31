#ifndef  _MLLTYPES_
#define  _MLLTYPES_

/**
 *@namespace  MLL
 *@brief  Namespace for all Machine Learning and SIPPER specific code.
 *@details
 * A library of routines used for Machine Learning, building Classifiers, etc.  It also includes code that is
 * specific to PICES and SIPPER.  At some point in the future we need split this library into three libraries.
 * MachineLearning, DataBase, and PicesRoutines.
 *
 * This library covers three different areas that at some time in the future should be made into
 * three separate libraries:
 *   -# Generic Machine Learning code.
 *   -# Database Related Code.
 *   -# Analysis.
 *   -# PICES specific code that would be built on top of the Machine Learning and Database code.
 *
 *@section MachineLearninig "Machine Learning Code"
 *
 *@subsection  FeatureData "Feature Data"
 * There are several Feature Data formats supported.  Each one has its own class that is derived from
 * 'FeatureFileIO'.  The description of the data is managed by 'FileDesc'.  For each type of dataset there
 * will exist on one instance of a FileDesc class.
 *@see Attribute
 *@see FeatureNumList
 *@see FeatureFileIO
 *@see FeatureVector
 *@see FeatureVectorList
 *@see MLClass
 *@see BitReduction
 *@see FeatureEncoder
 *@see NormalizationParms
 *@see Orderings
 *
 *@subsection  LearningAlgorithms "Learning Algorithms"
 *There are several learning algorithms implemented.  The Learning algorithms are all sub-classed from 'Model' and their
 *related parameters are all sub-classed from 'ModelParam'.
 *@see Model
 *@see ModelKnn
 *@see ModelOldSVM
 *@see ModelSvmBase
 *@see ModelParam
 *@see TrainingClass
 *@see TrainingProcess2
 *
 *@subsection AnalysisRoutines  "Analysis Routines"
 *@see ConfusionMatrix2
 *@see CrossValidation
 *@see CrossValidationMxN
 *@see CrossValidationVoting
 *@see SizeDistribution
 *
 *@section  DataBase  "Data Base"
 *@see  DataBase
 *@see  DataBaseServer
 *
 *@section  PICESSpecific "PICES Specific"
 * This is code that is built on the SipperInstruments (aka MLL Namespace),  Machine Learning Code, Feature Data Code
 *@see ImageFeatures
 *@see TrainingConfiguration2
 *@see DataBaseImage
 *@see DuplicateImages
 *@see SipperExtractionImageManager
 */

namespace  MLL  
{
}


#endif

