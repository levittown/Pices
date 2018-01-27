#ifndef  _DATASETPARAMETERS_
#define  _DATASETPARAMETERS_

#include "FileDesc.h"
#include "FeatureFileIO.h"
#include "KKStr.h"
#include "SVMparam.h"


typedef  enum  {DataSetNULL, 
                DataSetAdult, 
                DataSetBanana, 
                DataSetBanana2,
                DataSetForest, 
                DataSetLetter, 
                DataSetPage,
                DataSetPlankton,
                DataSetPenDigits,
                DataSetPhoneMe, 
                DataSetSatImage,
                DataSetShuttle,
                DataSetWaveForm,
                DataSetWeb
               }  DataSet;



struct  DataSetParameters
{
public:
  DataSet                    dataSet;
  KKStr                      name;
  KKStr                      dataSetSubDir;
  KKStr                      configFileName;
  KKStr                      trainDataFileName;
  KKStr                      testDataFileName;
  KKMLL::FeatureFileIOPtr    fileFormat;
  KKMLL::SVM_KernalType      kernalTypes     [4];
  KKMLL::SVM_EncodingMethod  encodingMethods [4];
  bool                       unWeighted;
  int                        c_params[20];
  int                        bitReductions[20];
  int                        rsBrBits;
  int                        rsUBR;
};

typedef  DataSetParameters*  DataSetParametersPtr;

#endif
