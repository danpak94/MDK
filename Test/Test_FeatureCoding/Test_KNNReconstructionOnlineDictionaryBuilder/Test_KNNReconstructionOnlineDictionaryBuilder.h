#include "mdkFileIO.h"
#include "mdkKNNBasisSelectionOnlineDictionaryBuilder.h"
#include "mdkKNNReconstructionOnlineDictionaryBuilder.h"

void Test_SimpleCase()
{
    using namespace mdk;

    CharString FilePath = "C:/Research/MDK_Build/Test/Test_FeatureCoding/KNNReconstructionOnlineDictionaryBuilder/Debug/";

    CharString FeatureDataFilePathAndName = FilePath + "DataSample.json";

    auto FeatureData = LoadDenseMatrixFromJsonDataFile<double>(FeatureDataFilePathAndName);

    int_max FeatureDataNumber = FeatureData.GetColNumber();

    int_max NeighbourNumber = 5;

    //-------------------------------------------------------------------------------------------

    KNNBasisSelectionOnlineDictionaryBuilder<double> InitialDictionaryBuilder;

    typedef KNNBasisSelectionOnlineDictionaryBuilder<double>::SimilarityTypeEnum SimilarityTypeEnum;

    InitialDictionaryBuilder.m_Parameter.BasisNumber = 10;
    InitialDictionaryBuilder.m_Parameter.ParameterOfKNNSoftAssign.NeighbourNumber = NeighbourNumber;

    InitialDictionaryBuilder.m_Parameter.ParameterOfKNNSoftAssign.SimilarityType = SimilarityTypeEnum::L1Distance;
    InitialDictionaryBuilder.m_Parameter.ParameterOfKNNSoftAssign.SimilarityThreshold = 0.1;
    InitialDictionaryBuilder.m_Parameter.ParameterOfKNNSoftAssign.Variance_L1 = 10;

    InitialDictionaryBuilder.m_Parameter.ExperienceDiscountFactor = 0;

    InitialDictionaryBuilder.m_Parameter.MaxNumberOfDataInEachBatch = 100;

    InitialDictionaryBuilder.m_Parameter.DebugInfo.Flag_OutputDebugInfo = true;
    InitialDictionaryBuilder.m_Parameter.DebugInfo.FilePathToSaveDebugInfo = FilePath;
    InitialDictionaryBuilder.m_Parameter.DebugInfo.JsonDataFileName_Of_VectorSimilarityMatrix = "VectorSimilarityMatrix.json";

    InitialDictionaryBuilder.SetInputFeatureData(&FeatureData);

    InitialDictionaryBuilder.Update();

    auto InitialDictionaryPtr = InitialDictionaryBuilder.GetOutputDictionary();

    DisplayMatrix("D_init", InitialDictionaryPtr->BasisMatrix(), 2);

    SaveDenseMatrixAsJsonDataFile(InitialDictionaryPtr->BasisMatrix(), FilePath + "BasisMatrix_init.json");
    SaveDenseMatrixAsJsonDataFile(InitialDictionaryPtr->VarianceOfL1Distance(), FilePath + "VarianceOfL1Distance_init.json");

    //--------------------------------------------------------------------------------------------

    KNNReconstructionOnlineDictionaryBuilder<double> DictionaryBuilder;

    DictionaryBuilder.m_Parameter.BasisNumber = 10;
    DictionaryBuilder.m_Parameter.ParameterOfKNNReconstruction.NeighbourNumber = NeighbourNumber;
    DictionaryBuilder.m_Parameter.ParameterOfKNNReconstruction.SimilarityType = SimilarityTypeEnum::L1Distance;
    DictionaryBuilder.m_Parameter.ParameterOfKNNReconstruction.CodeNonnegative = true;
    DictionaryBuilder.m_Parameter.ParameterOfKNNReconstruction.CodeSumToOne = false;

    DictionaryBuilder.m_Parameter.MaxNumberOfDataInEachBatch = 100;

    DictionaryBuilder.SetInputFeatureData(&FeatureData);

    DictionaryBuilder.SetInitialDictionary(InitialDictionaryPtr);

    DictionaryBuilder.Update();

    auto DictionaryPtr = DictionaryBuilder.GetOutputDictionary();

    DisplayMatrix("D", DictionaryPtr->BasisMatrix(), 2);

    SaveDenseMatrixAsJsonDataFile(DictionaryPtr->BasisMatrix(), FilePath + "BasisMatrix.json");
    SaveDenseMatrixAsJsonDataFile(DictionaryPtr->VarianceOfL1Distance(), FilePath + "VarianceOfL1Distance.json");

}