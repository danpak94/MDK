#ifndef __mdkKNNReconstructionSparseEncoder_hpp
#define __mdkKNNReconstructionSparseEncoder_hpp

//#include "mdkKNNReconstructionSparseEncoder.h"

namespace mdk
{

template<typename ElementType>
KNNReconstructionSparseEncoder<ElementType>::KNNReconstructionSparseEncoder()
{
    this->Clear();
}


template<typename ElementType>
KNNReconstructionSparseEncoder<ElementType>::~KNNReconstructionSparseEncoder()
{

}


template<typename ElementType>
void KNNReconstructionSparseEncoder<ElementType>::Clear()
{
    this->FeatureDictionaryBasedSparseEncoder::Clear();

    m_Parameter.Clear();

    m_GramianMatrix_DtD.Clear();
}


template<typename ElementType>
void KNNReconstructionSparseEncoder<ElementType>::SetupDefaultPipelineOutput()
{
    this->FeatureDictionaryBasedSparseEncoder::SetupDefaultPipelineOutput();

    m_ReconstructionErrorNorm_SharedCopy.Clear();

    m_ReconstructionErrorNorm = &m_ReconstructionErrorNorm_SharedCopy;
}


template<typename ElementType>
void KNNReconstructionSparseEncoder<ElementType>::UpdatePipelineOutput()
{
    this->FeatureDictionaryBasedSparseEncoder::UpdatePipelineOutput();

    if (m_ReconstructionErrorNorm != &m_ReconstructionErrorNorm_SharedCopy)
    {
        m_ReconstructionErrorNorm_SharedCopy.ForceShare(m_ReconstructionErrorNorm);
    }
}


template<typename ElementType>
void KNNReconstructionSparseEncoder<ElementType>::SetOutputReconstructionErrorNorm(DenseMatrix<ElementType>* ErrorNorm)
{
    if (ErrorNorm == nullptr)
    {
        MDK_Error("Invalid input @ KNNReconstructionSparseEncoder::SetOutputReconstructionErrorNorm(...)")
        return;
    }

    m_ReconstructionErrorNorm = ErrorNorm;

    m_ReconstructionErrorNorm_SharedCopy.ForceShare(ErrorNorm);
}


template<typename ElementType>
DenseMatrix<ElementType>* KNNReconstructionSparseEncoder<ElementType>::GetOutputReconstructionErrorNorm()
{
    return &m_ReconstructionErrorNorm_SharedCopy;
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::CheckInput()
{
    if (this->FeatureDictionaryBasedSparseEncoder::CheckInput() == false)
    {
        return false;
    }

    if (m_Parameter.NeighbourNumber <= 0)
    {
        MDK_Error("Invalid input NeighbourNumber (<= 0) @ KNNReconstructionSparseEncoder::CheckInput()")
        return false;
    }

    if (m_Parameter.DistanceTypeForKNNSearch != "L1Distance" 
        && m_Parameter.DistanceTypeForKNNSearch != "L2Distance" 
        && m_Parameter.DistanceTypeForKNNSearch != "Correlation"
        && m_Parameter.DistanceTypeForKNNSearch != "KLDivergence")
    {
        MDK_Error("DistanceTypeForKNNSearch is invalid @ KNNReconstructionSparseEncoder::CheckInput()")
        return false;
    }

    auto Size = m_Dictionary->GetSize();

    m_ReconstructionErrorNorm->FastResize(1, Size.ColNumber);

    return true;
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::ComputeGramianMatrix_DtD()
{
    if (m_Dictionary->IsEmpty() == true)
    {
        MDK_Error("Dictionary is empty @ KNNReconstructionSparseEncoder::ComputeGramianMatrix_DtD()")
        return false;
    }

    const DenseMatrix<ElementType>& D = m_Dictionary->BasisMatrix(); // "auto  D = " will copy

    m_GramianMatrix_DtD = D.Transpose()*D;

    return true;
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::Preprocess()
{
    this->ComputeGramianMatrix_DtD();

    return true;
}


template<typename ElementType>
inline
void KNNReconstructionSparseEncoder<ElementType>::EncodingFunction(int_max DataIndex, int_max ThreadIndex)
{
    const DenseMatrix<ElementType>& BasisMatrix = m_Dictionary->BasisMatrix(); // "auto  = " will copy

    const DenseMatrix<ElementType> DataColVector(const_cast<ElementType*>(m_FeatureData->GetElementPointerOfCol(DataIndex)),
                                                 m_FeatureData->GetRowNumber(), 1);

    SparseVector<ElementType>& CodeInSparseColVector = (*m_CodeInSparseColVectorSet)[DataIndex];

    //-------------------------------------------------------------------------------------------------

    DenseMatrix<ElementType> DistanceList;

    if (m_Parameter.DistanceTypeForKNNSearch == "L1Distance")
    {
        DistanceList = ComputeL1DistanceListFromSingleVectorToColVectorSet(DataColVector, BasisMatrix);
    }
    else if (m_Parameter.DistanceTypeForKNNSearch == "L2Distance")
    {
        DistanceList = ComputeL2DistanceListFromSingleVectorToColVectorSet(DataColVector, BasisMatrix);
    }
    else if (m_Parameter.DistanceTypeForKNNSearch == "Correlation")
    {
        DistanceList = ComputeCorrelationListFromSingleVectorToColVectorSet(DataColVector, BasisMatrix);
    }
    else if (m_Parameter.DistanceTypeForKNNSearch == "KLDivergence")
    {
        DistanceList = ComputeKLDivergenceListOfSingleVectorFromColVectorSet(DataColVector, BasisMatrix);
    }
    else
    {
        MDK_Error("DistanceTypeForKNNSearch is invalid @ KNNReconstructionSparseEncoder::EncodingFunction(...)")
        return;
    }

    auto NeighbourIndexList = FindKNNByDistanceList(DistanceList, m_Parameter.NeighbourNumber);

    auto KNNBasisMatrix = BasisMatrix.GetSubMatrix(ALL, NeighbourIndexList);

    // use LinearLeastSquaresProblemSolver

    auto CodeLength = BasisMatrix.GetColNumber();

    Option_Of_LinearLeastSquaresProblemSolver Option;

    if (m_Parameter.Nonnegative == false && m_Parameter.SumToOne == false)
    {
        Option.MethodName = "Normal";

        DenseMatrix<ElementType> H;

        if (m_GramianMatrix_DtD.IsEmpty() == false)
        {
            H = m_GramianMatrix_DtD.GetSubMatrix(NeighbourIndexList, NeighbourIndexList);
        }

        DenseMatrix<ElementType> A;

        auto Solution = LinearLeastSquaresProblemSolver<ElementType>::Apply(&KNNBasisMatrix, &DataColVector,
                                                                            nullptr, nullptr, &A, nullptr, nullptr, nullptr,
                                                                            &H, &Option);         
    }
    else if (m_Parameter.Nonnegative == true && m_Parameter.SumToOne == false)
    {
        DenseMatrix<ElementType> lb_x(m_Parameter.NeighbourNumber, 1);
        lb_x.Fill(0);

        Option.MethodName = "QuadraticProgramming";

        DenseMatrix<ElementType> H;

        if (m_GramianMatrix_DtD.IsEmpty() == false)
        {
            H = m_GramianMatrix_DtD.GetSubMatrix(NeighbourIndexList, NeighbourIndexList);
        }

        DenseMatrix<ElementType> A;

        auto Solution = LinearLeastSquaresProblemSolver<ElementType>::Apply(&KNNBasisMatrix, &DataColVector,
                                                                            &lb_x, nullptr, &A, nullptr, nullptr, nullptr,
                                                                            &H, &Option);
    }
    else if (m_Parameter.Nonnegative == true && m_Parameter.SumToOne == true)
    {
        DenseMatrix<ElementType> lb_x(m_Parameter.NeighbourNumber, 1);
        lb_x.Fill(ElementType(0));

        DenseMatrix<ElementType> A(1, m_Parameter.NeighbourNumber);
        A.Fill(ElementType(1));

        DenseMatrix<ElementType> lb_A = ElementType(1);
        DenseMatrix<ElementType> ub_A = ElementType(1);

        Option.MethodName = "QuadraticProgramming";

        DenseMatrix<ElementType> H;

        if (m_GramianMatrix_DtD.IsEmpty() == false)
        {
            H = m_GramianMatrix_DtD.GetSubMatrix(NeighbourIndexList, NeighbourIndexList);
        }

        auto Solution = LinearLeastSquaresProblemSolver<ElementType>::Apply(&KNNBasisMatrix, &DataColVector,
                                                                            &lb_x, nullptr, &A, &lb_A, &ub_A, nullptr,
                                                                            &H, &Option);
    }
    else //if(m_Parameter.Nonnegative == false && m_Parameter.SumToOne == true)
    {
        DenseMatrix<ElementType> A(1, m_Parameter.NeighbourNumber);
        A.Fill(ElementType(1));

        DenseMatrix<ElementType> lb_A = ElementType(1);
        DenseMatrix<ElementType> ub_A = ElementType(1);

        Option.MethodName = "QuadraticProgramming";

        DenseMatrix<ElementType> H;

        if (m_GramianMatrix_DtD.IsEmpty() == false)
        {
            H = m_GramianMatrix_DtD.GetSubMatrix(NeighbourIndexList, NeighbourIndexList);
        }

        auto Solution = LinearLeastSquaresProblemSolver<ElementType>::Apply(&KNNBasisMatrix, &DataColVector,
                                                                            nullptr, nullptr, &A, &lb_A, &ub_A, nullptr,
                                                                            &H, &Option);
    }

    // ----- create sparse code -----

    CodeInSparseColVector.Construct(NeighbourIndexList, Solution.X, CodeLength);

    // ----- update m_ReconstructionErrorNorm -------------------

    // compute ReconstructedDataColVector X_hat
    auto ReconstructedDataColVector = MatrixMultiply(KNNBasisMatrix, Solution.X);

    // get reconstruction error ||X-X_hat||
    auto ErrorVector = MatrixSubtract(DataColVector, ReconstructedDataColVector);
    auto ErrorL2Norm = ErrorVector.L2Norm();

    (*m_ReconstructionErrorNorm)[DataIndex] = ErrorL2Norm;
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::Postprocess()
{
    m_GramianMatrix_DtD.Clear();

    return true;
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::Apply(DenseMatrix<ElementType>& OutputCodeInDenseMatrix,
                                                        const DenseMatrix<ElementType>* FeatureData,
                                                        const FeatureDictionary<ElementType>* Dictionary,
                                                        const Parameter_Of_KNNReconstructionSparseEncoder& Parameter,
                                                        int_max MaxNumberOfThreads = 1)
{
    auto Encoder = std::make_unique<KNNReconstructionSparseEncoder<ElementType>>();

    Encoder->SetInputFeatureData(FeatureData);

    Encoder->SetInputDictionary(Dictionary);

    Encoder->SetMaxNumberOfThreads(MaxNumberOfThreads);

    Encoder->SetOutputCodeInDenseMatrix(&OutputCodeInDenseMatrix);

    Encoder->m_Parameter = Parameter;

    Encoder->Update();
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::Apply(SparseMatrix<ElementType>& OutputCodeInSparseMatrix,
                                                        const DenseMatrix<ElementType>* FeatureData,
                                                        const FeatureDictionary<ElementType>* Dictionary,
                                                        const Parameter_Of_KNNReconstructionSparseEncoder& Parameter,
                                                        int_max MaxNumberOfThreads = 1)
{
    auto Encoder = std::make_unique<KNNReconstructionSparseEncoder<ElementType>>();

    Encoder->SetInputFeatureData(FeatureData);

    Encoder->SetInputDictionary(Dictionary);

    Encoder->SetMaxNumberOfThreads(MaxNumberOfThreads);

    Encoder->SetOutputCodeInSparseMatrix(&OutputCodeInSparseMatrix);;

    Encoder->m_Parameter = Parameter;

    Encoder->Update();
}


template<typename ElementType>
bool KNNReconstructionSparseEncoder<ElementType>::Apply(DenseMatrix<SparseMatrix<ElementType>>& OutputCodeInSparseColVectorList,
                                                        const DenseMatrix<ElementType>* FeatureData,
                                                        const FeatureDictionary<ElementType>* Dictionary,
                                                        const Parameter_Of_KNNReconstructionSparseEncoder& Parameter,
                                                        int_max MaxNumberOfThreads = 1)
{
    auto Encoder = std::make_unique<KNNReconstructionSparseEncoder<ElementType>>();

    Encoder->SetInputFeatureData(FeatureData);

    Encoder->SetInputDictionary(Dictionary);

    Encoder->SetMaxNumberOfThreads(MaxNumberOfThreads);

    Encoder->SetOutputCodeInSparseColVectorSet(&OutputCodeInSparseColVectorList);;

    Encoder->m_Parameter = Parameter;

    Encoder->Update();
}

}// namespace mdk


#endif