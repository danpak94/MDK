#ifndef __mdkSPAMSOnlineDictionaryBuilder_hpp
#define __mdkSPAMSOnlineDictionaryBuilder_hpp


#include <opencv2/core/core.hpp>


namespace mdk
{

template<typename ElementType>
SPAMSOnlineDictionaryBuilder<ElementType>::SPAMSOnlineDictionaryBuilder()
{
    this->Clear();
}


template<typename ElementType>
SPAMSOnlineDictionaryBuilder<ElementType>::~SPAMSOnlineDictionaryBuilder()
{
}

//---------------------------------------------------------------------------------------------------------------//

template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::Clear()
{
    m_SparseEncoder = nullptr;

    m_Parameter.Clear();

    m_State.Clear();

    this->ClearPipelineOutput();
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::SetInputFeatureData(const DenseMatrix<ElementType>* InputFeatureData)
{
    m_FeatureData = InputFeatureData;
}

//---------------------------------------------------//

template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::SetInitialDictionary(const FeatureDictionaryForSparseCoding<ElementType>* Dictionary)
{
    m_InitialDictionary = Dictionary;
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::SetOutputDictionary(FeatureDictionaryForSparseCoding<ElementType>* OutputDictionary)
{
    if (OutputDictionary == nullptr)
    {
        MDK_Error("Invalid input @ SPAMSOnlineDictionaryBuilder::SetOutputDictionary(...)")
        return;
    }

    m_Dictionary = OutputDictionary;

    m_Dictionary_SharedCopy.ForceShare(OutputDictionary);
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::SetInitialState(State_Of_SPAMSOnlineDictionaryBuilder<ElementType> InitialState)
{
    m_State = std::move(InitialState);
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::SetSparseEncoder(FeatureDictionaryBasedSparseEncoder<ElementType>* Encoder)
{
    m_SparseEncoder = Encoder;
}


template<typename ElementType>
State_Of_SPAMSOnlineDictionaryBuilder<ElementType>*
SPAMSOnlineDictionaryBuilder<ElementType>::GetCurrentState()
{
    return &m_State;
}


template<typename ElementType>
bool SPAMSOnlineDictionaryBuilder<ElementType>::LoadStateAndParameter(const std::string& FilePathAndName)
{
    return true;
}


template<typename ElementType>
bool SPAMSOnlineDictionaryBuilder<ElementType>::SaveStateAndParameter(const std::string& FilePathAndName)
{
    return true;
}


template<typename ElementType>
bool SPAMSOnlineDictionaryBuilder<ElementType>::CheckInput()
{
    if (m_FeatureData == nullptr)
    {
        MDK_Error("m_FeatureData is nullptr @ SPAMSOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    auto DataSize = m_FeatureData->GetSize();

    if (DataSize.RowNumber == 0)
    {
        MDK_Error("InputFeatureData is empty @ SPAMSOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_SparseEncoder == nullptr)
    {
        MDK_Error("m_SparseEncoder is nullptr @ SPAMSOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.mode < 0) // use MDK sparse encoder (e.g., KNNReconstructionSparseEncoder)
    {
        if (m_InitialDictionary == nullptr)
        {
            MDK_Error("m_InitialDictionary is empty (nullptr) and m_Parameter.mode < 0 @ SPAMSOnlineDictionaryBuilder::CheckInput()")
            return false;
        }

        if (m_InitialDictionary->IsEmpty() == true)
        {
            MDK_Error("m_InitialDictionary is empty and m_Parameter.mode < 0 @ SPAMSOnlineDictionaryBuilder::CheckInput()")
            return false;
        }

        if (m_Parameter.modeD == 3 && m_Parameter.gamma1 == ElementType(1))
        {
            if (m_InitialDictionary->GetProperty_BasisNormalizedWithL1Norm() == false)
            {
                MDK_Error("Basis is NOT Normalized With L1 Norm in m_InitialDictionary" << '\n'
                          << "and m_Parameter.mode < 0 and m_Parameter.modeD == 3 and m_Parameter.gamma1 == 1" << '\n'
                          << "@ SPAMSOnlineDictionaryBuilder::CheckInput()")
                return false;
            }
        }

        if (m_Parameter.posD != m_InitialDictionary->GetProperty_BasisPositive())
        {
            MDK_Error("m_Parameter.posD != m_InitialDictionary->GetProperty_BasisPositive()" << '\n'               
                      << "@ SPAMSOnlineDictionaryBuilder::CheckInput()")
            return false;
        }
    }

    return true;
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::GenerateDictionary()
{
    auto X_spt = std::make_unique<spams::Matrix<ElementType>>(const_cast<ElementType*>(m_FeatureData->GetElementPointer()),
                                                              m_FeatureData->GetRowNumber(), m_FeatureData->GetColNumber());
    auto X = X_spt.get();

    std::unique_ptr<spams::Trainer<ElementType>> trainer_spt;
    spams::Trainer<ElementType>* trainer;

    if (m_State.D.IsEmpty() == true)
    {
        trainer_spt.reset(new spams::Trainer<ElementType>(m_Parameter.K, m_Parameter.batchsize, m_Parameter.numThreads));
        trainer = trainer_spt.get();
    }
    else
    {
        spams::Matrix<ElementType> D1(m_State.D.GetElementPointer(), m_State.D.GetRowNumber(), m_State.D.GetColNumber());

        if (m_State.A.IsEmpty() == true)
        {
            trainer_spt.reset(new spams::Trainer<ElementType>(D1, m_Parameter.batchsize, m_Parameter.numThreads));
            trainer = trainer_spt.get();
        }
        else
        {
            spams::Matrix<ElementType> A(m_State.A.GetElementPointer(), m_State.A.GetRowNumber(), m_State.A.GetColNumber());
            spams::Matrix<ElementType> B(m_State.B.GetElementPointer(), m_State.B.GetRowNumber(), m_State.B.GetColNumber());
            trainer_spt.reset(new spams::Trainer<ElementType>(A, B, D1, m_Parameter.iter, m_Parameter.batchsize, m_Parameter.numThreads));
            trainer = trainer_spt.get();
        }
    }

    //---------------------------------------------------------

    spams::ParamDictLearn<ElementType> param;

    param.lambda = m_Parameter.lambda;
    param.lambda2 = m_Parameter.lambda2;
    param.iter = m_Parameter.iter;
    param.t0 = m_Parameter.t0;
    param.mode = static_cast<spams::constraint_type>(m_Parameter.mode);
    param.posAlpha = m_Parameter.posAlpha;
    param.posD = m_Parameter.posD;
    param.expand = false; // what is this ?
    param.modeD = static_cast<spams::constraint_type_D>(m_Parameter.modeD);
    param.whiten = false;
    param.clean = m_Parameter.clean;
    param.verbose = m_Parameter.verbose;
    param.gamma1 = m_Parameter.gamma1;
    param.gamma2 = m_Parameter.gamma2;
    param.rho = m_Parameter.rho;
    param.stochastic = false; // what is this ?
    param.modeParam = static_cast<spams::mode_compute>(m_Parameter.modeParam);
    param.batch = false;
    param.iter_updateD = m_Parameter.iter_updateD;
    param.log = false;

    //---------------------------------------------------------

    if (param.mode >= 0)
    {
        trainer->train(*X, param);
    }
    else
    {
        trainer->train_extended(*X, param, m_SparseEncoder, *m_InitialDictionary);
    }

    //---------------------------------------------------------

    spams::Matrix<ElementType>& D = trainer->getD_ref();
    m_State.D.Copy(D.rawX(), D.m(), D.n());

    spams::Matrix<ElementType>& A = trainer->getA_ref();
    m_State.A.Copy(A.rawX(), A.m(), A.n());

    spams::Matrix<ElementType>& B = trainer->getB_ref();
    m_State.B.Copy(B.rawX(), B.m(), B.n());
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::ClearPipelineOutput()
{
    m_Dictionary_SharedCopy.Clear();
    m_Dictionary = &m_Dictionary_SharedCopy;
}


template<typename ElementType>
void SPAMSOnlineDictionaryBuilder<ElementType>::UpdatePipelineOutput()
{
    m_Dictionary->BasisMatrix().Copy(m_State.D);

    if (m_Dictionary != &m_Dictionary_SharedCopy)
    {
        m_Dictionary_SharedCopy.Share(m_Dictionary);
    }
}


template<typename ElementType>
FeatureDictionaryForSparseCoding<ElementType>* SPAMSOnlineDictionaryBuilder<ElementType>::GetOutputDictionary()
{
    return &m_Dictionary_SharedCopy;
}


}// namespace mdk


#endif