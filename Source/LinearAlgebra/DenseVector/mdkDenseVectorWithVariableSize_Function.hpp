#ifndef __mdkDenseVectorWithVariableSize_Function_hpp
#define __mdkDenseVectorWithVariableSize_Function_hpp


namespace mdk
{

template<typename ElementType>
inline
DenseVectorWithVariableSize<ElementType> 
VectorLinearCombine(const std::vector<ElementType>& CoefList, const std::vector<const DenseVectorWithVariableSize<ElementType>*>& VectorSet)
{
    typedef DenseVectorWithVariableSize<ElementType> VectorType;

    VectorType EmptyVector;

    if (CoefList.size() != VectorSet.size())
    {
        MDK_Error("CoefList.size() != VectorSet.size() @ DenseVectorWithVariableSize_Function VectorLinearCombine(...)")

        return EmptyVector;
    } 

    int_max VectorNumber = int_max(VectorSet.size());

    auto Length = VectorSet[0]->GetLength();

    for (int_max k = 1; k < VectorNumber; ++k)
    {
        if (VectorSet[k]->GetLength() != Length)
        {
            MDK_Error("Size is not the same in VectorSet @ DenseVectorWithVariableSize_Function VectorLinearCombine(...)")

            return EmptyVector;
        }
    }

    VectorType OutputVector;

    OutputVector.Resize(Length);

    OutputVector.Fill(ElementType(0));

    for (int_max k = 0; k < VectorNumber; ++k)
    {
        auto Coef_k = CoefList[k];

        VectorType& Vector_k = *VectorSet[k];

        for (int_max i = 0; i < Length; ++i)
        {
            OutputVector[i] + = Coef_k * Vector_k[i];
        }
    }

    return OutputVector;
}

}// namespace mdk

#endif