#ifndef __mdkKNNAverageOnlineDictionaryBuilder_hpp
#define __mdkKNNAverageOnlineDictionaryBuilder_hpp


namespace mdk
{

template<typename ScalarType>
KNNAverageOnlineDictionaryBuilder<ScalarType>::KNNAverageOnlineDictionaryBuilder()
{
    this->Clear();
}


template<typename ScalarType>
KNNAverageOnlineDictionaryBuilder<ScalarType>::~KNNAverageOnlineDictionaryBuilder()
{
}

//---------------------------------------------------------------------------------------------------------------//

template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::Clear()
{
    m_Parameter.Clear();
    m_FeatureData = nullptr;
    m_InitialDictionary = nullptr;
	m_Dictionary.Clear();
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::SetInputFeatureData(const DenseMatrix<ScalarType>* InputFeatureData)
{
    m_FeatureData = InputFeatureData;
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::SetInitialDictionary(const FeatureDictionaryForSparseCoding<ScalarType>* InitialDictionary)
{
    m_InitialDictionary = InitialDictionary;
}


template<typename ScalarType>
FeatureDictionaryForSparseCoding<ScalarType>* KNNAverageOnlineDictionaryBuilder<ScalarType>::GetOutputDictionary()
{
    return &m_Dictionary;
}


template<typename ScalarType>
bool KNNAverageOnlineDictionaryBuilder<ScalarType>::CheckInput()
{
    if (m_FeatureData == nullptr)
    {
        MDK_Error("m_FeatureData is nullptr @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_FeatureData->IsEmpty() == true)
    {
        MDK_Error("InputFeatureData is empty @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_InitialDictionary == nullptr)
    {
        MDK_Error("InitialDictionary is empty (nullptr) @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_InitialDictionary->IsEmpty() == true)
    {
        MDK_Error("InitialDictionary is empty @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_FeatureData->GetRowNumber() != m_InitialDictionary->BasisMatrix().GetRowNumber())
    {
        MDK_Error("RowNumber Of FeatureData != RowNumber Of InitialDictionary @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.ParameterOfKNNSoftAssign.NeighbourNumber <= 0)
    {
        MDK_Error("NeighbourNumber <= 0 @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.MaxNumberOfDataInEachBatch < m_Parameter.ParameterOfKNNSoftAssign.NeighbourNumber)
    {
        MDK_Error("MaxNumberOfDataInEachBatch < MaxNumberOfNeighbours @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.MaxNumberOfInteration <= 0)
    {
        MDK_Error("MaxNumberOfInterations <= 0 @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    auto IsSimilarityTypeSupported = KNNSoftAssignSparseEncoder<ScalarType>::
                                     CheckIfSimilarityTypeSupported(m_Parameter.ParameterOfKNNSoftAssign.SimilarityType);    

    if (IsSimilarityTypeSupported == false)
    {
        MDK_Error("SimilarityType is not supported @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.ParameterOfKNNSoftAssign.SimilarityThreshold <= 0)
    {
        MDK_Error("SimilarityThreshold <= 0 @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.ParameterOfKNNSoftAssign.SimilarityType == VectorSimilarityTypeEnum::L1Distance)
    {
        if (m_InitialDictionary->VarianceOfL1Distance().IsEmpty() == true)
        {
            MDK_Error("VarianceOfL1Distance in InitialDictionary is empty <= 0 @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
            return false;            
        }
    }
    else if (m_Parameter.ParameterOfKNNSoftAssign.SimilarityType == VectorSimilarityTypeEnum::L2Distance)
    {
        if (m_InitialDictionary->VarianceOfL2Distance().IsEmpty() == true)
        {
            MDK_Error("VarianceOfL2Distance in InitialDictionary is empty @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
            return false;
        }
    }
    else if (m_Parameter.ParameterOfKNNSoftAssign.SimilarityType == VectorSimilarityTypeEnum::KLDivergence)
    {
        if (m_InitialDictionary->VarianceOfKLDivergence().IsEmpty() == true)
        {
            MDK_Error("VarianceOfKLDivergence in InitialDictionary is empty 0 @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
            return false;
        }
    }

    if (m_Parameter.ExperienceDiscountFactor < 0 || m_Parameter.ExperienceDiscountFactor > 1)
    {
        MDK_Error("ExperienceDiscountFactor < 0 or > 1 @ KNNAverageOnlineDictionaryBuilder::CheckInput()")
        return false;
    }

    if (m_Parameter.SimilarityThreshold_For_ComputeBasisRedundancy <= 0)
    {
        m_Parameter.SimilarityThreshold_For_ComputeBasisRedundancy = m_Parameter.ParameterOfKNNSoftAssign.SimilarityThreshold;
    }

    return true;
}


template<typename ScalarType>
bool KNNAverageOnlineDictionaryBuilder<ScalarType>::Update()
{
	if (this->CheckInput() == false)
	{
		return false;
	}
	this->GenerateDictionary();
	return true;
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::GenerateDictionary()
{
    this->SetupParameter();

    auto OutputDictionary = this->PreprocessInitialDictionary(*m_InitialDictionary);

    DenseMatrix<ScalarType> BasisExperience_init;
    BasisExperience_init.Copy(OutputDictionary.BasisExperience());

    int_max TotalDataNumber = m_FeatureData->GetColNumber();

    //----------- encode data and update dictionary ----------------------------//

    if (m_Parameter.MaxNumberOfDataInEachBatch >= TotalDataNumber)
    {
        for (int_max k = 0; k < m_Parameter.MaxNumberOfInteration; ++k)
        {
            m_KNNSoftAssignSparseEncoder.SetInputFeatureData(m_FeatureData);
            m_KNNSoftAssignSparseEncoder.SetInputDictionary(&OutputDictionary);
            m_KNNSoftAssignSparseEncoder.Update();
            const auto& CodeTable = *m_KNNSoftAssignSparseEncoder.GetOutputCode();

            this->UpdateDictionary(OutputDictionary, *m_FeatureData, CodeTable);
        }
    }
    else
    {
        //------------------------------------------ run Data batch -------------------------------------------------------//
        
        // random number for sampling
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> UniformRandomNumber(0, TotalDataNumber - 1);

        for (int_max k = 0; k < m_Parameter.MaxNumberOfInteration; ++k)
        {

            DenseMatrix<int_max> DataFlagList(1, TotalDataNumber);
            DataFlagList.Fill(1);
            // 1 : not used yet
            // 0 : used

            DenseMatrix<ScalarType> FeatureData_current;

            int_max NumberOfDataInNextBatch = m_Parameter.MaxNumberOfDataInEachBatch;

            int_max TotalDataNumber_used = 0;

            while (true)
            {
                // sample a subset from m_FeatureData

                FeatureData_current.FastResize(m_FeatureData->GetRowNumber(), NumberOfDataInNextBatch);

                int_max SampleCounter = 0;
                while (true)
                {
                    auto DataIndex = UniformRandomNumber(gen);
                    if (DataFlagList[DataIndex] == 1)
                    {
                        FeatureData_current.SetCol(SampleCounter, m_FeatureData->GetElementPointerOfCol(DataIndex));

                        DataFlagList[DataIndex] = 0;

                        SampleCounter += 1;
                        if (SampleCounter >= NumberOfDataInNextBatch)
                        {
                            break;
                        }
                    }
                }

                m_KNNSoftAssignSparseEncoder.SetInputFeatureData(&FeatureData_current);
                m_KNNSoftAssignSparseEncoder.SetInputDictionary(&OutputDictionary);
                m_KNNSoftAssignSparseEncoder.Update();
                const auto& CodeTable = *m_KNNSoftAssignSparseEncoder.GetOutputCode();

                this->UpdateDictionary(OutputDictionary, FeatureData_current, CodeTable);

                // update NumberOfDataInNextBatch

                TotalDataNumber_used += SampleCounter;

                NumberOfDataInNextBatch = std::min(m_Parameter.MaxNumberOfDataInEachBatch, TotalDataNumber - TotalDataNumber_used);

                if (NumberOfDataInNextBatch <= 0)
                {
                    break;
                }
            }

            // do need to use this->UpdateDictionary_OtherInformation(...)
            this->AdjustBasisExperience(OutputDictionary.BasisExperience(), BasisExperience_init, TotalDataNumber);
        }
    }

    this->UpdateDictionary_OtherInformation(OutputDictionary, BasisExperience_init, TotalDataNumber);

    m_Dictionary = std::move(OutputDictionary);
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::SetupParameter()
{
    m_KNNSoftAssignSparseEncoder.m_Parameter = m_Parameter.ParameterOfKNNSoftAssign;   
    m_KNNSoftAssignSparseEncoder.SetMaxNumberOfThread(m_Parameter.MaxNumberOfThread);
}


template<typename ScalarType>
FeatureDictionaryForSparseCoding<ScalarType>
KNNAverageOnlineDictionaryBuilder<ScalarType>::PreprocessInitialDictionary(const FeatureDictionaryForSparseCoding<ScalarType>& InitialDictionary)
{
    FeatureDictionaryForSparseCoding<ScalarType> OutputDictionary;

    OutputDictionary.Copy(InitialDictionary);

    DenseMatrix<ScalarType>& BasisExperience = OutputDictionary.BasisExperience();

    // discount the previous Experience
    BasisExperience *= m_Parameter.ExperienceDiscountFactor;
    // must >= 1
    for (int_max k = 0; k < BasisExperience.GetElementNumber(); k++)
    {
        if (BasisExperience[k] < 1)
        {
            BasisExperience[k] = 1;
        }
    }

    return OutputDictionary;
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateDictionary(FeatureDictionaryForSparseCoding<ScalarType>& Dictionary,
                 const DenseMatrix<ScalarType>& FeatureData,
                 const DataArray<SparseVector<ScalarType>>& CodeTable)
{
    //------------------------------------------------------------------------------
    // Input:
    // Dictionary is the initial dictionary
    // CodeTable is the soft-assign code of FeatureData using the initial dictionary
    //
    // Output:
    // Dictionary : it will be updated in this function
    //-------------------------------------------------------------------------------

    // copy the initial BasisExperience
    DenseMatrix<ScalarType> BasisExperience_init;
    BasisExperience_init.Copy(Dictionary.BasisExperience());

    // update BasisMatrix and BasisExperience ----------------------------------------

    this->UpdateBasisMatrixAndBasisExperience(Dictionary.BasisMatrix(), Dictionary.BasisExperience(), FeatureData, CodeTable);
    
    // update other information ---------------------------------------------------------------------------------

    // SeedForNewBasisIDGeneration has been copied in this->CopyInitialDictionaryAndDiscountBasisExperience(...)

    // BasisID does not need to be updated : No new basis is added, and not old basis is retired

    // BasisAge updated in this->UpdateDictionary_OtherInformation(...)

    // SimilarityMatrix and BasisRedundancy updated in this->UpdateDictionary_OtherInformation(...)

    //-------- update Variance -----------------------------------------------------------------------------------

    this->UpdateVarianceOfL1Distance(Dictionary.VarianceOfL1Distance(), FeatureData, CodeTable, Dictionary.BasisMatrix(), BasisExperience_init);
    
    this->UpdateVarianceOfL2Distance(Dictionary.VarianceOfL2Distance(), FeatureData, CodeTable, Dictionary.BasisMatrix(), BasisExperience_init);

    if (m_Parameter.Flag_Update_VarianceOfReconstruction_Using_KNNBasisMatrix == true)
    {
        this->UpdateVarianceOfReconstruction(Dictionary.VarianceOfReconstruction(), FeatureData, CodeTable, Dictionary.BasisMatrix(), BasisExperience_init);
    }

    if (m_Parameter.ParameterOfKNNSoftAssign.SimilarityType == VectorSimilarityTypeEnum::KLDivergence)
    {
        this->UpdateVarianceOfKLDivergence(Dictionary.VarianceOfKLDivergence(), FeatureData, CodeTable, Dictionary.BasisMatrix(), BasisExperience_init);
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateDictionary_OtherInformation(FeatureDictionaryForSparseCoding<ScalarType>& Dictionary,
                                  const DenseMatrix<ScalarType>& BasisExperience_init,
                                  int_max TotalDataNumber)
{
    //---------------------- already updated ------------------------------------

    DenseMatrix<ScalarType>& BasisMatrix = Dictionary.BasisMatrix();

    DenseMatrix<ScalarType>& VarianceOfL1Distance = Dictionary.VarianceOfL1Distance();
    DenseMatrix<ScalarType>& VarianceOfL2Distance = Dictionary.VarianceOfL2Distance();
    DenseMatrix<ScalarType>& VarianceOfKLDivergence = Dictionary.VarianceOfKLDivergence();
    DenseMatrix<ScalarType>& VarianceOfReconstruction = Dictionary.VarianceOfReconstruction();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    //-------------------------- update BasisAge -----------------------------

    if (m_Parameter.Flag_Update_BasisAge == true)
    {
        Dictionary.BasisAge() += TotalDataNumber;
    }

    //----------------- adjust BasisExperience if Data is re-used ------------

    this->AdjustBasisExperience(Dictionary.BasisExperience(), BasisExperience_init, TotalDataNumber);

    //---------------------- to be updated ------------------------------------

    DenseMatrix<ScalarType>& SimilarityMatrix = Dictionary.SimilarityMatrix();
    DenseMatrix<ScalarType>& BasisRedundancy = Dictionary.BasisRedundancy();

    //-------------------------------------------------------------------------

    DenseMatrix<ScalarType> VarianceList;

    switch (m_Parameter.ParameterOfKNNSoftAssign.SimilarityType)
    {
    case VectorSimilarityTypeEnum::L1Distance:
        VarianceList.Share(VarianceOfL1Distance);
        break;

    case VectorSimilarityTypeEnum::L2Distance:
        VarianceList.Share(VarianceOfL2Distance);
        break;

    case VectorSimilarityTypeEnum::KLDivergence:
        VarianceList.Share(VarianceOfKLDivergence);
        break;

    default:
        VarianceList.FastResize(1, BasisNumber);
        VarianceList.Fill(0);
        break;
    }

    this->UpdateSimilarityMatrix(SimilarityMatrix, BasisMatrix, VarianceList);

    this->UpdateBasisRedundancy(BasisRedundancy, SimilarityMatrix);    

    Dictionary.SetProperty_SimilarityType(m_Parameter.ParameterOfKNNSoftAssign.SimilarityType);

    Dictionary.SetProperty_SimilarityThresholdForComputeBasisRedundancy(m_Parameter.SimilarityThreshold_For_ComputeBasisRedundancy);
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateBasisMatrixAndBasisExperience(DenseMatrix<ScalarType>&       BasisMatrix,
                                    DenseMatrix<ScalarType>&       BasisExperience,
                                    const DenseMatrix<ScalarType>& FeatureData,
                                    const DataArray<SparseVector<ScalarType>>& CodeTable)
{
    if (m_Parameter.WhetherToUseScaleFactor == true)
    {
        this->UpdateBasisMatrixAndBasisExperience_UseScaleFactor(BasisMatrix, BasisExperience, FeatureData, CodeTable);
    }
    else
    {
        this->UpdateBasisMatrixAndBasisExperience_NoScaleFactor(BasisMatrix, BasisExperience, FeatureData, CodeTable);
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateBasisMatrixAndBasisExperience_UseScaleFactor(DenseMatrix<ScalarType>&       BasisMatrix,
                                                   DenseMatrix<ScalarType>&       BasisExperience,
                                                   const DenseMatrix<ScalarType>& FeatureData,
                                                   const DataArray<SparseVector<ScalarType>>& CodeTable)
{
    // temp function for basis_change update

    auto TempFunction_UpdateBasisChange = [](ScalarType* BasisChange, const ScalarType* BasisVector, const ScalarType* DataVector, int_max VectorLength,
                                             ScalarType CodeElement,  ScalarType SquaredL2NormOfReconstructionCodeVector)
    {
        auto eps_value = std::numeric_limits<ScalarType>::epsilon();

        auto temp = CodeElement / (SquaredL2NormOfReconstructionCodeVector + eps_value);

        if (temp > eps_value)
        {
            for (int_max k = 0; k < VectorLength; ++k)
            {
                auto Error = DataVector[k] - BasisVector[k] * CodeElement;

                BasisChange[k] += temp * Error;
            }
        }
    };

    // temp function for basis update
    //
    //auto TempFunction_UpdateBasis = [](ScalarType* BasisVector, const ScalarType* DataVector, int_max VectorLength,
    //                                   ScalarType CodeElement, ScalarType SquaredL2NormOfReconstructionCodeVector,
    //                                   ScalarType ExperienceOfBasis)
    //{
    //    auto temp = CodeElement / SquaredL2NormOfReconstructionCodeVector;
    //
    //    for (int_max k = 0; k < VectorLength; ++k)
    //    {
    //        auto Error = DataVector[k] - BasisVector[k] * CodeElement;
    //
    //        BasisVector[k] += temp * Error / ExperienceOfBasis;
    //    }
    //};
    

    int_max DataNumber = FeatureData.GetColNumber();

    int_max VectorLength = FeatureData.GetRowNumber();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    DenseMatrix<ScalarType> Basis_Basis_InnerProductList(1, BasisNumber);
    for (int_max k = 0; k < BasisNumber; ++k)
    {
        auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(k);
        Basis_Basis_InnerProductList[k] = ComputeInnerProductOfTwoVector(BasisVectorPtr, BasisVectorPtr, VectorLength, false);
    }

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    DenseMatrix<ScalarType> BasisMatrix_Change(VectorLength, BasisNumber);
    BasisMatrix_Change.Fill(0);

	DenseVector<ScalarType> BasisExperience_Change;
	BasisExperience_Change.Resize(BasisNumber);
	BasisExperience_Change.Fill(0);

    DenseMatrix<ScalarType> ReconstructionCodeVector;

    for (int_max k = 0; k < DataNumber; ++k)//k
    {
        auto DataVectorPtr = FeatureData.GetElementPointerOfCol(k);

		const auto& KNNBasisIndexList = CodeTable[k].IndexList();

        int_max KNNBasisNumber = KNNBasisIndexList.GetLength();

        if (KNNBasisNumber > 0)
        {
            ReconstructionCodeVector.FastResize(KNNBasisNumber, 1);
            // compute reconstruction code Vector
            //
            // use KNN-basis to reconstruct each Data vector (pretend we are doing KNN-reconstruction)
            // reconstruct each Data vector with only one basis
            // 
            // ReconstructionError[n] = ||DataVector - ReconstructionCodeVector[n]*BasisVector||
            //

            for (int_max n = 0; n < KNNBasisNumber; ++n)//n
            {
                auto BasisIndex = KNNBasisIndexList[n];

                auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(BasisIndex);

                auto Basis_Data_InnerProduct = ComputeInnerProductOfTwoVector(BasisVectorPtr, DataVectorPtr, VectorLength, false);

                auto Basis_Basis_InnerProduct = Basis_Basis_InnerProductList[BasisIndex];

                if (Basis_Basis_InnerProduct > eps_value && Basis_Basis_InnerProduct > eps_value*Basis_Data_InnerProduct)
                {
                    ReconstructionCodeVector[n] = Basis_Data_InnerProduct / Basis_Basis_InnerProduct;
                }
                else
                {
                    ReconstructionCodeVector[n] = 0;
                }
            }

            auto SquaredL2NormOfReconstructionCodeVector = ComputeInnerProductOfTwoVector(ReconstructionCodeVector.GetElementPointer(),
                                                                                          ReconstructionCodeVector.GetElementPointer(), 
                                                                                          KNNBasisNumber, false);

            if (SquaredL2NormOfReconstructionCodeVector > eps_value)
            {
                for (int_max n = 0; n < KNNBasisNumber; ++n)//n
                {
                    auto BasisIndex = KNNBasisIndexList[n];

                    if (SquaredL2NormOfReconstructionCodeVector > eps_value*Basis_Basis_InnerProductList[BasisIndex])
                    {
                        auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(BasisIndex);

                        auto BasisVectorChangePtr = BasisMatrix_Change.GetElementPointerOfCol(BasisIndex);

                        auto CodeElement = ReconstructionCodeVector[n];

                        if (CodeElement > eps_value)
                        {
                            TempFunction_UpdateBasisChange(BasisVectorChangePtr, BasisVectorPtr, DataVectorPtr, VectorLength,
                                                           CodeElement, SquaredL2NormOfReconstructionCodeVector);
                            
                            //TempFunction_UpdateBasis(BasisVectorPtr, DataVectorPtr, VectorLength,
                            //                         CodeElement, SquaredL2NormOfReconstructionCodeVector,
                            //                         BasisExperience[BasisIndex]);
                            
                            //BasisExperience[BasisIndex] += CodeElement*CodeElement / SquaredL2NormOfReconstructionCodeVector;
							BasisExperience_Change[BasisIndex] += CodeElement*CodeElement / SquaredL2NormOfReconstructionCodeVector;

                        }// else CodeElement = 0, no update
                    }
                }
            }
        }
    }

	// update BasisExperience
	for (int_max k = 0; k <= BasisNumber - 1; ++k)
	{
		BasisExperience[k] += BasisExperience_Change[k];
	}

    // update Basis
    //for (int_max k = 0; k <= BasisNumber-1; ++k)
    auto TempFunction_UpdateBasisMatrix = [&](int_max k)
    {
        auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(k);

        auto BasisVectorChangePtr = BasisMatrix_Change.GetElementPointerOfCol(k);
        
        auto Experience = BasisExperience[k];

        for (int_max n = 0; n < VectorLength; ++n)
        {
            BasisVectorPtr[n] += BasisVectorChangePtr[n] / Experience;
        }
    };
    ParallelForLoop(TempFunction_UpdateBasisMatrix, 0, BasisNumber - 1, m_Parameter.MaxNumberOfThread);
    
    this->ApplyConstraintOnBasis(BasisMatrix);

	

    // the total Experience is
    // BasisExperience.Sum() <- BasisExperience.Sum() + DataNumber
    //
    // the new "Experience" of the dictionary gained from data is DataNumber
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateBasisMatrixAndBasisExperience_NoScaleFactor(DenseMatrix<ScalarType>&       BasisMatrix,
                                                  DenseMatrix<ScalarType>&       BasisExperience,
                                                  const DenseMatrix<ScalarType>& FeatureData,
                                                  const DataArray<SparseVector<ScalarType>>& CodeTable)
{
    // temp function for basis_change update

    auto TempFunction_UpdateBasisChange = [](ScalarType* BasisChange, const ScalarType* BasisVector, const ScalarType* DataVector, int_max VectorLength,
                                             ScalarType Membership)
    {
        for (int_max k = 0; k < VectorLength; ++k)
        {
            auto Error = DataVector[k] - BasisVector[k];
            BasisChange[k] += Membership * Error;
        }
    };

    //------------------------------------------------------

    int_max DataNumber = FeatureData.GetColNumber();
    int_max VectorLength = FeatureData.GetRowNumber();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    DenseMatrix<ScalarType> BasisMatrix_Change(VectorLength, BasisNumber);
    BasisMatrix_Change.Fill(0);

	DenseVector<ScalarType> BasisExperience_Change;
	BasisExperience_Change.Resize(BasisNumber);
	BasisExperience_Change.Fill(0);

    for (int_max k = 0; k < DataNumber; ++k)//k
    {
        auto DataVectorPtr = FeatureData.GetElementPointerOfCol(k);

        const auto& KNNBasisIndexList = CodeTable[k].IndexList();

		const auto& MembershipList = CodeTable[k].ElementList(); // code from SoftAssignSparseEncorder is membership, not raw Similarity

        int_max KNNBasisNumber = KNNBasisIndexList.GetLength();

        if (KNNBasisNumber > 0)
        {
            for (int_max n = 0; n < KNNBasisNumber; ++n)//n
            {
                auto BasisIndex = KNNBasisIndexList[n];

                auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(BasisIndex);

                auto BasisVectorChangePtr = BasisMatrix_Change.GetElementPointerOfCol(BasisIndex);

                TempFunction_UpdateBasisChange(BasisVectorChangePtr, BasisVectorPtr, DataVectorPtr, VectorLength, MembershipList[n]);

                //BasisExperience[BasisIndex] += MembershipList[n];
				BasisExperience_Change[BasisIndex] += MembershipList[n];
            }
        }
    }

	// update BasisExperience
	for (int_max k = 0; k <= BasisNumber - 1; ++k)
	{
		BasisExperience[k] += BasisExperience_Change[k];
	}

    // update Basis
    //for (int_max k = 0; k <= BasisNumber-1; ++k)
    auto TempFunction_UpdateBasisMatrix = [&](int_max k)
    {
        auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(k);

        auto BasisVectorChangePtr = BasisMatrix_Change.GetElementPointerOfCol(k);

        auto Experience = BasisExperience[k];

        for (int_max n = 0; n < VectorLength; ++n)
        {
            BasisVectorPtr[n] += BasisVectorChangePtr[n] / Experience;
        }
    };
    ParallelForLoop(TempFunction_UpdateBasisMatrix, 0, BasisNumber - 1, m_Parameter.MaxNumberOfThread);

    this->ApplyConstraintOnBasis(BasisMatrix);

    // the total Experience is
    // BasisExperience.Sum() <- BasisExperience.Sum() + DataNumber
    //
    // the new "Experience" of the dictionary gained from data is DataNumber
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::ApplyConstraintOnBasis(DenseMatrix<ScalarType>& BasisMatrix)
{
    if (m_Parameter.BasisPositive == true)
    {
        for (int_max k = 0; k < BasisMatrix.GetElementNumber(); ++k)
        {
            if (BasisMatrix[k] < ScalarType(0))
            {
                BasisMatrix[k] = ScalarType(0);
            }
        }
    }

    if (m_Parameter.BasisNormalizedWithL1Norm == true)
    {
        int_max BasisNumber = BasisMatrix.GetColNumber();
        int_max Length = BasisMatrix.GetRowNumber();

        auto tempValueToFill = ScalarType(1) / ScalarType(Length);

        auto eps_value = std::numeric_limits<ScalarType>::epsilon();

        DenseMatrix<ScalarType> BasisVector;

        for (int_max k = 0; k < BasisNumber; ++k)
        {
            BasisVector.Share(BasisMatrix.GetElementPointerOfCol(k), Length, 1);

            auto tempL1Norm = BasisVector.L1Norm();
            if (tempL1Norm <= eps_value)
            {
                BasisVector.Fill(tempValueToFill);
            }
            else
            {
                BasisVector /= tempL1Norm;
            }
        }
    }
    else if (m_Parameter.BasisNormalizedWithL2Norm == true)
    {
        int_max BasisNumber = BasisMatrix.GetColNumber();
        int_max Length = BasisMatrix.GetRowNumber();

        auto tempValueToFill = std::sqrt(ScalarType(1) / ScalarType(Length));

        auto eps_value = std::numeric_limits<ScalarType>::epsilon();

        DenseMatrix<ScalarType> BasisVector;

        for (int_max k = 0; k < BasisNumber; ++k)
        {
            BasisVector.Share(BasisMatrix.GetElementPointerOfCol(k), Length, 1);

            auto tempL2Norm = BasisVector.L2Norm();
            if (tempL2Norm <= eps_value)
            {
                BasisVector.Fill(tempValueToFill);
            }
            else
            {
                BasisVector /= tempL2Norm;
            }
        }
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
AdjustBasisExperience(DenseMatrix<ScalarType>& BasisExperience, const DenseMatrix<ScalarType>& BasisExperience_init, int_max TotalDataNumber)
{
    int_max BasisNumber = BasisExperience.GetElementNumber();

    DenseMatrix<ScalarType> BasisExperience_Diff = MatrixSubtract(BasisExperience, BasisExperience_init);

    ScalarType TotalGain = BasisExperience_Diff.Sum();

    if (TotalGain > TotalDataNumber)
    {
        BasisExperience_Diff *= ScalarType(TotalDataNumber) / TotalGain;

        MatrixAdd(BasisExperience, BasisExperience_init, BasisExperience_Diff);
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateSimilarityMatrix(DenseMatrix<ScalarType>& SimilarityMatrix, const DenseMatrix<ScalarType>& BasisMatrix, const DenseMatrix<ScalarType>& VarianceList)
{
    int_max BasisNumber = BasisMatrix.GetColNumber();
    int_max VectorLength = BasisMatrix.GetRowNumber();

    auto SimilarityType = m_Parameter.ParameterOfKNNSoftAssign.SimilarityType;

    SimilarityMatrix.FastResize(BasisNumber, BasisNumber);

    //for (int_max k = 0; k <= BasisNumber - 2; ++k)
    auto TempFunction_ComputeSimilarity = [&](int_max k)
    {
        auto BasisVectorPtr_k = BasisMatrix.GetElementPointerOfCol(k);

        for (int_max n = k + 1; n < BasisNumber; ++n) // start from k+1
        {
            auto BasisVectorPtr_n = BasisMatrix.GetElementPointerOfCol(n);

            auto Variance = std::max(VarianceList[k], VarianceList[n]);

            auto Similarity = KNNSoftAssignSparseEncoder<ScalarType>::ComputeSimilarityBetweenTwoVector(BasisVectorPtr_k, BasisVectorPtr_n, VectorLength, 
                                                                                                         SimilarityType, Variance, false);

            SimilarityMatrix(k, n) = Similarity;

            SimilarityMatrix(n, k) = Similarity;
        }
    };

    ParallelForLoop(TempFunction_ComputeSimilarity, 0, BasisNumber - 2, m_Parameter.MaxNumberOfThread);
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateBasisRedundancy(DenseMatrix<ScalarType>& BasisRedundancy, const DenseMatrix<ScalarType>& SimilarityMatrix)
{
    int_max BasisNumber = SimilarityMatrix.GetColNumber();

    BasisRedundancy.FastResize(1, BasisNumber);

    if (BasisNumber == 1)
    {
        BasisRedundancy[0] = 0;
    }

    auto SimilarityThreshold = m_Parameter.SimilarityThreshold_For_ComputeBasisRedundancy;

    //for (int_max k = 0; k <= BasisNumber-1; ++k)
    auto TempFunction_UpdateRedundancy = [&](int_max k)
    {
        BasisRedundancy[k] = 0;

        for (int_max n = 0; n < BasisNumber; ++n)
        {
            if (k != n)
            {
                if (SimilarityMatrix(n, k) >= SimilarityThreshold)
                {
                    BasisRedundancy[k] += SimilarityMatrix(n, k);
                }
            }
        }
    };

    ParallelForLoop(TempFunction_UpdateRedundancy, 0, BasisNumber - 1, m_Parameter.MaxNumberOfThread);
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateVarianceOfL1Distance(DenseMatrix<ScalarType>& Variance,
                           const DenseMatrix<ScalarType>& FeatureData,
                           const DataArray<SparseVector<ScalarType>>& CodeTable,
                           const DenseMatrix<ScalarType>& BasisMatrix,
                           const DenseMatrix<ScalarType>& BasisExperience_init)
{
    int_max DataNumber = FeatureData.GetColNumber();

    int_max VectorLength = FeatureData.GetRowNumber();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    DenseMatrix<ScalarType> Variance_current(1, BasisNumber);
    Variance_current.Fill(0);

    DenseMatrix<ScalarType> WeightList(1, BasisNumber);
    WeightList.Fill(0);

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    for (int_max k = 0; k < DataNumber; ++k)
    {
        const auto& KNN_IndexList = CodeTable[k].IndexList();

		const auto& KNN_Membership = CodeTable[k].ElementList(); // code from SoftAssignSparseEncorder is membership, not raw Similarity 

        auto tempNeighbourNumber = KNN_IndexList.GetLength();

        if (tempNeighbourNumber > 0)
        {
            auto DataVectorPtr = FeatureData.GetElementPointerOfCol(k);

            for (int_max m = 0; m < tempNeighbourNumber; ++m)
            {
                auto BasisIndex = KNN_IndexList[m];

                auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(BasisIndex);

                auto L1Distance = ComputeL1DistanceBetweenTwoVector(DataVectorPtr, BasisVectorPtr, VectorLength, false);

                Variance_current[BasisIndex] += L1Distance*L1Distance*KNN_Membership[m];

                WeightList[BasisIndex] += KNN_Membership[m];
            }
        }
    }

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (WeightList[n] > eps_value)
        {
            Variance_current[n] /= WeightList[n];
        }
    }

    auto MeanVar = Variance.Mean();

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (Variance_current[n] > eps_value*MeanVar)
        {
            Variance[n] = Variance[n] * BasisExperience_init[n] + Variance_current[n];
            Variance[n] /= BasisExperience_init[n] + 1;
        }
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateVarianceOfL2Distance(DenseMatrix<ScalarType>& Variance,
                           const DenseMatrix<ScalarType>& FeatureData,
                           const DataArray<SparseVector<ScalarType>>& CodeTable,
                           const DenseMatrix<ScalarType>& BasisMatrix,
                           const DenseMatrix<ScalarType>& BasisExperience_init)
{
    int_max DataNumber = FeatureData.GetColNumber();

    int_max VectorLength = FeatureData.GetRowNumber();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    DenseMatrix<ScalarType> Variance_current(1, BasisNumber);
    Variance_current.Fill(0);

    DenseMatrix<ScalarType> WeightList(1, BasisNumber);
    WeightList.Fill(0);

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    for (int_max k = 0; k < DataNumber; ++k)
    {
        const auto& KNN_IndexList = CodeTable[k].IndexList();

		const auto& KNN_Membership = CodeTable[k].ElementList(); // code from SoftAssignSparseEncorder is Similarity, not membership 

        auto tempNeighbourNumber = KNN_IndexList.GetLength();

        if (tempNeighbourNumber > 0)
        {
            auto DataVectorPtr = FeatureData.GetElementPointerOfCol(k);

            for (int_max m = 0; m < tempNeighbourNumber; ++m)
            {
                auto BasisIndex = KNN_IndexList[m];

                auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(BasisIndex);

                auto SquaredL2Distance = ComputeSquaredL2DistanceBetweenTwoVector(DataVectorPtr, BasisVectorPtr, VectorLength, false);

                Variance_current[BasisIndex] += SquaredL2Distance*KNN_Membership[m];

                WeightList[BasisIndex] += KNN_Membership[m];
            }
        }
    }

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (WeightList[n] > eps_value)
        {
            Variance_current[n] /= WeightList[n];
        }
    }

    auto MeanVar = Variance.Mean();

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (Variance_current[n] > eps_value*MeanVar)
        {
            Variance[n] = Variance[n] * BasisExperience_init[n] + Variance_current[n];
            Variance[n] /= BasisExperience_init[n] + 1;
        }
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateVarianceOfKLDivergence(DenseMatrix<ScalarType>& Variance,
                             const DenseMatrix<ScalarType>& FeatureData,
                             const DataArray<SparseVector<ScalarType>>& CodeTable,
                             const DenseMatrix<ScalarType>& BasisMatrix,
                             const DenseMatrix<ScalarType>& BasisExperience_init)
{
    int_max DataNumber = FeatureData.GetColNumber();

    int_max VectorLength = FeatureData.GetRowNumber();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    DenseMatrix<ScalarType> Variance_current(1, BasisNumber);
    Variance_current.Fill(0);

    DenseMatrix<ScalarType> WeightList(1, BasisNumber);
    WeightList.Fill(0);

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    for (int_max k = 0; k < DataNumber; ++k)
    {
        const auto& KNN_IndexList = CodeTable[k].IndexList();

		const auto& KNN_Membership = CodeTable[k].ElementList(); // code from SoftAssignSparseEncorder is membership, not raw Similarity 

        auto tempNeighbourNumber = KNN_IndexList.GetLength();

        if (tempNeighbourNumber > 0)
        {
            auto DataVectorPtr = FeatureData.GetElementPointerOfCol(k);

            for (int_max m = 0; m < tempNeighbourNumber; ++m)
            {
                auto BasisIndex = KNN_IndexList[m];

                auto BasisVectorPtr = BasisMatrix.GetElementPointerOfCol(BasisIndex);

                auto KLDivergence = ComputeKLDivergenceOfVectorAFromVectorB(DataVectorPtr, BasisVectorPtr, VectorLength, false);

                Variance_current[BasisIndex] += KLDivergence*KLDivergence*KNN_Membership[m];

                WeightList[BasisIndex] += KNN_Membership[m];
            }
        }
    }

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (WeightList[n] > eps_value)
        {
            Variance_current[n] /= WeightList[n];
        }
    }

    auto MeanVar = Variance.Mean();

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (Variance_current[n] > eps_value*MeanVar)
        {
            Variance[n] = Variance[n] * BasisExperience_init[n] + Variance_current[n];
            Variance[n] /= BasisExperience_init[n] + 1;
        }
    }
}


template<typename ScalarType>
void KNNAverageOnlineDictionaryBuilder<ScalarType>::
UpdateVarianceOfReconstruction(DenseMatrix<ScalarType>& Variance,
                               const DenseMatrix<ScalarType>& FeatureData,
                               const DataArray<SparseVector<ScalarType>>& CodeTable,
                               const DenseMatrix<ScalarType>& BasisMatrix,
                               const DenseMatrix<ScalarType>& BasisExperience_init)
{
    int_max DataNumber = FeatureData.GetColNumber();

    int_max VectorLength = FeatureData.GetRowNumber();

    int_max BasisNumber = BasisMatrix.GetColNumber();

    auto DataReconstructionErrorL2Norm = this->ComputeDataReconstructionErrorL2Norm(FeatureData, CodeTable, BasisMatrix);

    DenseMatrix<ScalarType> Variance_current(1, BasisNumber);
    Variance_current.Fill(0);

    DenseMatrix<ScalarType> WeightList(1, BasisNumber);
    WeightList.Fill(0);

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    for (int_max k = 0; k < DataNumber; ++k)
    {
        const auto& KNN_IndexList = CodeTable[k].IndexList();

		const auto& KNN_Membership = CodeTable[k].ElementList(); // code from SoftAssignSparseEncorder is membership, not raw Similarity 

        auto tempNeighbourNumber = KNN_IndexList.GetLength();

        if (tempNeighbourNumber > 0)
        {
            for (int_max m = 0; m < tempNeighbourNumber; ++m)
            {
                auto BasisIndex = KNN_IndexList[m];

                Variance_current[BasisIndex] += DataReconstructionErrorL2Norm[k] * KNN_Membership[m];

                WeightList[BasisIndex] += KNN_Membership[m];
            }
        }       
    }

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (WeightList[n] > eps_value)
        {
            Variance[n] /= WeightList[n];
        }
    }

    auto MeanVar = Variance.Mean();

    for (int_max n = 0; n < BasisNumber; ++n)
    {
        if (Variance_current[n] > eps_value*MeanVar)
        {
            Variance[n] = Variance[n] * BasisExperience_init[n] + Variance_current[n];
            Variance[n] /= BasisExperience_init[n] + 1;
        }
    }
}


template<typename ScalarType>
DenseMatrix<ScalarType>
KNNAverageOnlineDictionaryBuilder<ScalarType>::
ComputeDataReconstructionErrorL2Norm(const DenseMatrix<ScalarType>&  FeatureData,
                                     const DataArray<SparseVector<ScalarType>>& CodeTable,
                                     const DenseMatrix<ScalarType>&  BasisMatrix)
{
    auto ReconstructionCodeSet = KNNReconstructionSparseEncoder<ScalarType>::
                                 ComputeReconstructionCodeFromSimilarityCode(FeatureData,
                                                                             CodeTable,
                                                                             BasisMatrix,
                                                                             m_Parameter.ConstraintOnKNNReconstructionCode.CodeNonnegative,
                                                                             m_Parameter.ConstraintOnKNNReconstructionCode.CodeSumToOne,
                                                                             m_Parameter.MaxNumberOfThread);

    auto ErrorL2NormList = KNNReconstructionSparseEncoder<ScalarType>::
                           ComputeReconstructionErrorL2Norm(FeatureData, ReconstructionCodeSet, BasisMatrix, m_Parameter.MaxNumberOfThread);

    return ErrorL2NormList;
}



}// namespace mdk


#endif