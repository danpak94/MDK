﻿#pragma once


namespace mdk
{

//================================================================================================================================//

template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, MatchFunctionType MatchFunction)
{
    return FindElementInMatrix(InputMatrix, InputMatrix.GetElementCount(), 0, InputMatrix.GetElementCount()-1, MatchFunction);
}


template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputCount, MatchFunctionType MatchFunction)
{
    return FindElementInMatrix(InputMatrix, MaxOutputCount, 0, InputMatrix.GetElementCount()-1, MatchFunction);
}


template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputCount, 
                                         int_max LinearIndex_start, int_max LinearIndex_end, MatchFunctionType MatchFunction)
{
    DenseMatrix<int_max> LinearIndexList;

    if (MaxOutputCount == 0)
    {
        return LinearIndexList;
    }

    auto InputElementCount = InputMatrix.GetElementCount();

    if (MaxOutputCount < 0 || MaxOutputCount > InputElementCount)
    {
        MDK_Error("MaxOutputCount is invalid @ mdkLinearAlgebra_DenseMatrix FindElementInMatrix(...)")
        return LinearIndexList;
    }

    if (LinearIndex_start < 0 || LinearIndex_start >= InputElementCount || LinearIndex_start > LinearIndex_end)
    {
        MDK_Error("LinearIndex_start is invalid @ mdkLinearAlgebra_DenseMatrix FindElementInMatrix(...)")
        return LinearIndexList;
    }

    if (LinearIndex_end < 0 || LinearIndex_end >= InputElementCount)
    {
        MDK_Error("LinearIndex_end is invalid @ mdkLinearAlgebra_DenseMatrix FindElementInMatrix(...)")
        return LinearIndexList;
    }

    if (InputElementCount == 0)
    {
        return LinearIndexList;
    }

    LinearIndexList.SetCapacity(MaxOutputCount);
    for (int_max i = LinearIndex_start; i <= LinearIndex_end; ++i)
    {
        if (MatchFunction(InputMatrix[i]) == true)
        {
            LinearIndexList.AppendCol({ i });
            auto CurrentCount = LinearIndexList.GetElementCount();
            if (CurrentCount == MaxOutputCount)
            {
                break;
            }
        }
    }

    return LinearIndexList;
}


template<typename ElementType, typename MatchFunctionType>
int_max FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, MatchFunctionType MatchFunction)
{
    int_max Index_output = -1;

	if (first_or_last[0] == "f")//first
	{
		for (int_max i = 0; i < InputMatrix.GetElementCount(); ++i)
		{
			if (MatchFunction(InputMatrix[i]) == true)
			{
				Index_output = i;
				break;
			}
		}
	}
	else if (first_or_last[0] == "l")//last
	{
		for (int_max i = InputMatrix.GetElementCount()-1; i >= 0; --i)
		{
			if (MatchFunction(InputMatrix[i]) == true)
			{
				Index_output = i;
				break;
			}
		}
	}
	else
	{
		MDK_Error("Wrong option @ mdkLinearAlgebra_DenseMatrix FindElementInMatrix(...)")
	}

    return Index_output;
}


template<typename ElementType>
DenseMatrix<int_max> ExactMatchElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, const ElementType& InputElement)
{
	return FindElementInMatrix(InputMatrix, [&](const ElementType& Element){return Element == InputElement; });
}


template<typename ElementType>
int_max ExactMatchElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, const ElementType& InputElement)
{
	return FindElementInMatrix(InputMatrix, first_or_last, [&](const ElementType& Element) {return Element == InputElement; });
}


template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, MatchFunctionType MatchFunction)
{
    return FindColInMatrix(InputMatrix, InputMatrix.GetColCount(), 0, InputMatrix.GetColCount()-1, MatchFunction);
}


template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputColCount, MatchFunctionType MatchFunction)
{
    return FindColInMatrix(InputMatrix, MaxOutputColCount, 0, InputMatrix.GetColCount()-1, MatchFunction);
}


template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputColCount, 
                                     int_max ColIndex_start, int_max ColIndex_end, MatchFunctionType MatchFunction)
{
    DenseMatrix<int_max> ColIndexList;

    if (MaxOutputColCount == 0)
    {
        return ColIndexList;
    }

    auto InputSize = InputMatrix.GetSize();

    if (MaxOutputColCount < 0 || MaxOutputColCount > InputSize.ColCount)
    {
        MDK_Error("MaxOutputColCount is invalid @ mdkLinearAlgebra_DenseMatrix FindColInMatrix(...)")
        return ColIndexList;
    }

    if (ColIndex_start < 0 || ColIndex_start >= InputSize.ColCount || ColIndex_start > ColIndex_end)
    {
        MDK_Error("ColIndex_start is invalid @ mdkLinearAlgebra_DenseMatrix FindColInMatrix(...)")
        return ColIndexList;
    }

    if (ColIndex_end < 0 || ColIndex_end >= InputSize.ColCount)
    {
        MDK_Error("ColIndex_end is invalid @ mdkLinearAlgebra_DenseMatrix FindColInMatrix(...)")
        return ColIndexList;
    }

    if (InputSize.ColCount == 0)
    {
        return ColIndexList;
    }
	
    ColIndexList.SetCapacity(MaxOutputColCount);
    DenseMatrix<ElementType> ColVector;
	if (ColIndex_start < ColIndex_end)
	{
		for (int_max i = ColIndex_start; i <= ColIndex_end; ++i)
		{
			DenseMatrix<ElementType> ColVector;
			ColVector.ForceShare(InputMatrix.GetElementPointerOfCol(i), InputSize.RowCount, 1);
			if (MatchFunction(ColVector) == true)
			{
				ColIndexList.Append(i);
				auto CurrentCount = ColIndexList.GetElementCount();
				if (CurrentCount == MaxOutputColCount)
				{
					break;
				}
			}
		}
	}
	else // if (ColIndex_start > ColIndex_end)
	{
		for (int_max i = ColIndex_start; i >= ColIndex_end; --i)
		{
			DenseMatrix<ElementType> ColVector;
			ColVector.ForceShare(InputMatrix.GetElementPointerOfCol(i), InputSize.RowCount, 1);
			if (MatchFunction(ColVector) == true)
			{
				ColIndexList.Append(i);
				auto CurrentCount = ColIndexList.GetElementCount();
				if (CurrentCount == MaxOutputColCount)
				{
					break;
				}
			}
		}
	}

    return ColIndexList;
}


template<typename ElementType, typename MatchFunctionType>
int_max FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, MatchFunctionType MatchFunction)
{
	int_max ColIndex_output = -1;

	auto InputSize = InputMatrix.GetSize();

	if (first_or_last == "first")
	{
		for (int_max i = 0; i <= InputSize.ColCount - 1; ++i)
		{
			DenseMatrix<ElementType> ColVector;
			ColVector.ForceShare(InputMatrix.GetElementPointerOfCol(i), InputSize.RowCount, 1);
			if (MatchFunction(ColVector) == true)
			{
				ColIndex_output = i;
				break;
			}
		}
	}
	else if (first_or_last == "last")
	{
		for (int_max i = InputSize.ColCount - 1; i >= 0;  --i)
		{
			DenseMatrix<ElementType> ColVector;
			ColVector.ForceShare(InputMatrix.GetElementPointerOfCol(i), InputSize.RowCount, 1);
			if (MatchFunction(ColVector) == true)
			{
				ColIndex_output = i;
				break;
			}
		}
	}
	else
	{
		MDK_Error("Wrong option @ mdkLinearAlgebra_DenseMatrix FindColInMatrix(...)")
	}	

	return ColIndex_output;
}


template<typename ElementType>
DenseMatrix<int_max> ExactMatchColInMatrix(const DenseMatrix<ElementType>& InputMatrix, const DenseMatrix<ElementType>& InputCol)
{
	return FindColInMatrix(InputMatrix, [&](const DenseMatrix<ElementType>& Col) {return Col == InputCol; });
}


template<typename ElementType>
int_max ExactMatchColInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, const DenseMatrix<ElementType>& InputCol)
{
	return FindColInMatrix(InputMatrix, first_or_last, [&](const DenseMatrix<ElementType>& Col) {return Col == InputCol; });
}


template<typename ElementType, typename CompareFunctionType>
DenseMatrix<int_max> SortColInMatrix(const DenseMatrix<ElementType>& InputMatrix, CompareFunctionType CompareFunction)
{
    return SortColInMatrix(InputMatrix, 0, InputMatrix.GetColCount()-1, CompareFunction);
}


template<typename ElementType, typename CompareFunctionType>
DenseMatrix<int_max> SortColInMatrix(const DenseMatrix<ElementType>& InputMatrix,
                                     int_max ColIndex_start, int_max ColIndex_end, CompareFunctionType CompareFunction)
{
    DenseMatrix<int_max> ColIndexList;

    auto InputSize = InputMatrix.GetSize();

    if (ColIndex_start < 0 || ColIndex_start >= InputSize.ColCount || ColIndex_start > ColIndex_end)
    {
        MDK_Error("ColIndex_start is invalid @ mdkLinearAlgebra_DenseMatrix SortColInMatrix(...)")
        return ColIndexList;
    }

    if (ColIndex_end < 0 || ColIndex_end >= InputSize.ColCount)
    {
        MDK_Error("ColIndex_end is invalid @ mdkLinearAlgebra_DenseMatrix SortColInMatrix(...)")
        return ColIndexList;
    }

    if (InputSize.ColCount == 0)
    {
        return ColIndexList;
    }

    if (ColIndex_start == ColIndex_end)
    {
        ColIndexList.AppendCol({ ColIndex_start });
        return ColIndexList;
    }

    ColIndexList.FastResize(1, InputSize.ColCount);
    for (int_max i = ColIndex_start; i <= ColIndex_end; ++i)
    {
        ColIndexList[i] = i;
    }

    DenseMatrix<ElementType> ColVector_a, ColVector_b;
    std::sort(ColIndexList.begin(), ColIndexList.end(), [&](int_max a, int_max b)
    {
        ColVector_a.ForceShare(InputMatrix.GetElementPointerOfCol(a), InputSize.RowCount, 1);
        ColVector_b.ForceShare(InputMatrix.GetElementPointerOfCol(b), InputSize.RowCount, 1);
        return CompareFunction(ColVector_a, ColVector_b);
    });

    return ColIndexList;
}


template<typename ElementType>
DenseMatrix<int_max> FindUniqueElementInMatrix(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<int_max> LinearIndexList_unique;

    if (InputMatrix.IsEmpty() == true)
    {
        return LinearIndexList_unique;
    }

    auto LinearIndexList_sort = InputMatrix.Sort([](const ElementType& a, const ElementType& b){ return a < b; });

    LinearIndexList_unique.SetCapacity(LinearIndexList_sort.GetElementCount());
    LinearIndexList_unique.Append(LinearIndexList_sort[0]);

    auto Element_prev = InputMatrix[LinearIndexList_sort[0]];

    for (int_max k = 1; k < LinearIndexList_sort.GetElementCount(); ++k)
    {
        auto Element = InputMatrix[LinearIndexList_sort[k]];
        if (Element != Element_prev)
        {
            LinearIndexList_unique.Append(LinearIndexList_sort[k]);
            Element_prev = Element;
        }
    }

    return LinearIndexList_unique;
}


template<typename ElementType, typename SpecialCompareFunctionType>
DenseMatrix<int_max> FindUniqueElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, SpecialCompareFunctionType SpecialCompareFunction)
{
    // Result = SpecialCompareFunction(const ElementType& a, const ElementType& b)
    // Result: integer type
    // Result: -1: a < b
    // Result: 0:  a = b 
    // Result: 1:  a > b
    //

    DenseMatrix<int_max> LinearIndexList_unique;

    if (InputMatrix.IsEmpty() == true)
    {
        return LinearIndexList_unique;
    }

    auto LinearIndexList_sort = InputMatrix.Sort([&](const ElementType& a, const ElementType& b)
    { 
        auto Result = SpecialCompareFunction(a, b);
        return (Result < 0);
    });

    LinearIndexList_unique.SetCapacity(LinearIndexList_sort.GetElementCount());
    LinearIndexList_unique.Append(LinearIndexList_sort[0]);

    auto Element_prev = InputMatrix[LinearIndexList_sort[0]];

    for (int_max k = 1; k < LinearIndexList_sort.GetElementCount(); ++k)
    {
        auto Element = InputMatrix[LinearIndexList_sort[k]];
        if (SpecialCompareFunction(Element, Element_prev) == 0)
        {
            LinearIndexList_unique.Append(LinearIndexList_sort[k]);
            Element_prev = Element;
        }
    }

    return LinearIndexList_unique;
}


template<typename ElementType>
DenseMatrix<int_max> FindUniqueColInMatrix(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<int_max> ColIndexList_unique;

    if (InputMatrix.IsEmpty() == true)
    {
        return ColIndexList_unique;
    }

    auto TempFunction_CompareCol = [](const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB)
    {
        auto RowCount = VectorA.GetElementCount();

        for (int_max k = 0; k < RowCount; ++k)
        {
            if (VectorA[k] < VectorB[k])
            {
                return true;
            }
            else if (VectorA[k] > VectorB[k])
            {
                return false;
            }
        }

        // default value must be false, true will cause assert failure
        return false;
    };

    auto ColIndexList_sort = SortColInMatrix(InputMatrix, TempFunction_CompareCol);

    ColIndexList_unique.SetCapacity(ColIndexList_sort.GetElementCount());
    ColIndexList_unique.Append(ColIndexList_sort[0]);

    DenseMatrix<ElementType> Vector_prev, Vector_k;
    Vector_prev.ForceShare(InputMatrix.GetElementPointerOfCol(ColIndexList_sort[0]), InputMatrix.GetRowCount(), 1);
    for (int_max k = 1; k < ColIndexList_sort.GetElementCount(); ++k)
    {
        Vector_k.ForceShare(InputMatrix.GetElementPointerOfCol(ColIndexList_sort[k]), InputMatrix.GetRowCount(), 1);
        if (TempFunction_CompareCol(Vector_prev, Vector_k) == true)
        {
            ColIndexList_unique.Append(ColIndexList_sort[k]);
            Vector_prev.ForceShare(InputMatrix.GetElementPointerOfCol(ColIndexList_sort[k]), InputMatrix.GetRowCount(), 1);
        }
    }

    return ColIndexList_unique;
}


template<typename ElementType, typename SpecialCompareFunctionType>
DenseMatrix<int_max> FindUniqueColInMatrix(const DenseMatrix<ElementType>& InputMatrix, SpecialCompareFunctionType SpecialCompareFunction)
{
    // Result = SpecialCompareFunction(const DenseMatrix<ElementType>& a, const const DenseMatrix<ElementType>& b)
    // Result: integer type
    // Result: -1: a < b
    // Result: 0:  a = b 
    // Result: 1:  a > b
    //

    DenseMatrix<int_max> ColIndexList_unique;

    if (InputMatrix.IsEmpty() == true)
    {
        return ColIndexList_unique;
    }

    auto ColIndexList_sort = SortColInMatrix(InputMatrix, [&](const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB)
    {
        auto Result = SpecialCompareFunction(VectorA, VectorB);
        return (Result < 0);
    });

    ColIndexList_unique.SetCapacity(ColIndexList_sort.GetElementCount());
    ColIndexList_unique.Append(ColIndexList_sort[0]);

    DenseMatrix<ElementType> Vector_prev, Vector_k;
    Vector_prev.ForceShare(InputMatrix.GetElementPointerOfCol(ColIndexList_sort[0]), InputMatrix.GetRowCount(), 1);
    for (int_max k = 1; k < ColIndexList_sort.GetElementCount(); ++k)
    {
        Vector_k.ForceShare(InputMatrix.GetElementPointerOfCol(ColIndexList_sort[k]), InputMatrix.GetRowCount(), 1);
        if (SpecialCompareFunction(Vector_prev, Vector_k) != 0)
        {
            ColIndexList_unique.Append(ColIndexList_sort[k]);
            Vector_prev.ForceShare(InputMatrix.GetElementPointerOfCol(ColIndexList_sort[k]), InputMatrix.GetRowCount(), 1);
        }
    }

    return ColIndexList_unique;
}

//================================================================================================================================//

template<typename ElementType>
ElementType MatrixMean(const DenseMatrix<ElementType>& InputMatrix)
{
    auto ElementCount = InputMatrix.GetElementCount();
    if (ElementCount <= 0)
    {
        MDK_Error("Input is an empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMean(InputMatrix)")
        return InputMatrix.GetErrorElement();
    }

    auto RawPointer = InputMatrix.GetElementPointer();
    ElementType value = RawPointer[0];
    for (auto Ptr = RawPointer + 1; Ptr < RawPointer + ElementCount; ++Ptr)
    {
        value += Ptr[0];
    }
    value /= ElementType(ElementCount);
    return value;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixMeanOfEachCol(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMeanOfEachCol(InputMatrix)")
        return tempMatrix;
    }

    tempMatrix.Resize(1, InputSize.ColCount);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max j = 0; j < InputSize.ColCount; ++j)
    {
        auto value = RawPointer[0];
        for (int_max i = 1; i < InputSize.RowCount; ++i)
        {
            value += RawPointer[i];
        }
        tempRawPointer[j] = value / ElementType(InputSize.RowCount);
        RawPointer += InputSize.RowCount;
    }

    return tempMatrix;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixMeanOfEachRow(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMeanOfEachRow(InputMatrix)")
        return tempMatrix;
    }

    tempMatrix.Resize(InputSize.RowCount, 1);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();

    for (int_max i = 0; i < InputSize.RowCount; ++i)
    {
        auto value = RawPointer[i];
        int_max Index = InputSize.RowCount;
        for (int_max j = 1; j < InputSize.ColCount; ++j)
        {
            value += RawPointer[Index + i];
            Index += InputSize.RowCount;
        }
        tempRawPointer[i] = value / ElementType(InputSize.ColCount);
    }

    return tempMatrix;
}


template<typename ElementType>
ElementType MatrixStd(const DenseMatrix<ElementType>& InputMatrix) // standard deviation
{
	auto ElementCount = InputMatrix.GetElementCount();
	if (ElementCount <= 0)
	{
		MDK_Error("Input is an empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixStd(InputMatrix)")
		return InputMatrix.GetErrorElement();
	}

	auto MeanValue = MatrixMean(InputMatrix);
	auto RawPointer = InputMatrix.GetElementPointer();
	auto Value = ElementType(0);
	for (int_max i = 0; i < ElementCount; ++i)
	{
		auto temp = RawPointer[i] - MeanValue;
		Value += temp*temp;
	}
	Value /= ElementType(ElementCount);
	Value = std::sqrt(Value);

	return Value;
}


template<typename ElementType>
int_max FindLinearIndexOfMaxInMatrix(const DenseMatrix<ElementType>& InputMatrix)
{
    auto Input_ElementCount = InputMatrix.GetElementCount();
    if (Input_ElementCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix FindLinearIndexOfMaxInMatrix(InputMatrix)")

        return -1;
    }

    auto InputPtr = InputMatrix.GetElementPointer();
    ElementType MaxValue = InputPtr[0];
    int_max LinearIndex = 0;
    for (int_max k = 1; k < Input_ElementCount; ++k)
    {
        if (InputPtr[k] > MaxValue)
        {
            MaxValue = InputPtr[k];
            LinearIndex = k;
        }
    }

    return LinearIndex;
}


template<typename ElementType>

ElementType MatrixMax(const DenseMatrix<ElementType>& InputMatrix)
{   
    if (InputMatrix.IsEmpty() == true)
    { 
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMax(InputMatrix)")
        return InputMatrix.GetErrorElement();
    }

    int_max LinearIndex = FindLinearIndexOfMaxInMatrix(InputMatrix);
    return InputMatrix[LinearIndex];
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixMaxOfEachCol(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMaxOfEachCol(InputMatrix)")

        return tempMatrix;
    }
	
	tempMatrix.Resize(1, InputSize.ColCount);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max j = 0; j < InputSize.ColCount; ++j)
    {
        auto value = RawPointer[0];
        ++RawPointer;
        for (int_max i = 1; i < InputSize.RowCount; ++i)
        {
            value = std::max(value, RawPointer[0]);
            ++RawPointer;
        }
        tempRawPointer[j] = value;
    }

    return tempMatrix;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixMaxOfEachRow(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMaxOfEachRow(InputMatrix)")
        return tempMatrix;
    }

	tempMatrix.Resize(InputSize.RowCount, 1);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max i = 0; i < InputSize.RowCount; ++i)
    {
        auto value = RawPointer[i];
        auto Index = InputSize.RowCount;
        for (int_max j = 1; j < InputSize.ColCount; ++j)
        {
            value = std::max(value, RawPointer[Index + i]);
            Index += InputSize.RowCount;
        }
        tempRawPointer[i] = value;
    }

    return tempMatrix;
}


template<typename ElementType>
int_max FindLinearIndexOfMinInMatrix(const DenseMatrix<ElementType>& InputMatrix)
{
    auto Input_ElementCount = InputMatrix.GetElementCount();
    if (Input_ElementCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix FindLinearIndexOfMinInMatrix(InputMatrix)")
        return -1;
    }

    auto InputPtr = InputMatrix.GetElementPointer();
    ElementType MinValue = InputPtr[0];
    int_max LinearIndex = 0;
    for (int_max k = 1; k < Input_ElementCount; ++k)
    {
        if (InputPtr[k] < MinValue)
        {
            MinValue = InputPtr[k];
            LinearIndex = k;
        }
    }

    return LinearIndex;
}


template<typename ElementType>
ElementType MatrixMin(const DenseMatrix<ElementType>& InputMatrix)
{
    if (InputMatrix.IsEmpty() == true)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMin(InputMatrix)")
        return InputMatrix.GetErrorElement();
    }

    auto LinearIndex = FindLinearIndexOfMinInMatrix(InputMatrix);
    return InputMatrix[LinearIndex];
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixMinOfEachCol(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMinOfEachCol(InputMatrix)")
        return tempMatrix;
    }

    tempMatrix.Resize(1, InputSize.ColCount);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max j = 0; j < InputSize.ColCount; ++j)
    {
        auto value = RawPointer[0];
        ++RawPointer;
        for (int_max i = 1; i < InputSize.RowCount; ++i)
        {
            value = std::min(value, RawPointer[0]);
            ++RawPointer;
        }
        tempRawPointer[j] = value;
    }

    return tempMatrix;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixMinOfEachRow(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixMinOfEachRow(InputMatrix)")
        return tempMatrix;
    }

	tempMatrix.Resize(InputSize.RowCount, 1);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max i = 0; i < InputSize.RowCount; ++i)
    {
        auto value = RawPointer[i];
        auto Index = InputSize.RowCount;
        for (int_max j = 1; j < InputSize.ColCount; ++j)
        {
            value = std::min(value, RawPointer[Index + i]);
            Index += InputSize.RowCount;
        }
        tempRawPointer[i] = value;
    }

    return tempMatrix;
}


template<typename ElementType>
ElementType MatrixSum(const DenseMatrix<ElementType>& InputMatrix)
{
    auto Input_ElementCount = InputMatrix.GetElementCount();
    if (Input_ElementCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixSum(InputMatrix)")
        return InputMatrix.GetErrorElement();
    }

    auto RawPointer = InputMatrix.GetElementPointer();
    ElementType value = RawPointer[0];
    for (auto Ptr = RawPointer + 1; Ptr < RawPointer + Input_ElementCount; ++Ptr)
    {
        value += Ptr[0];
    }

    return value;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixSumOfEachCol(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("InputMatrix is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixSumOfEachCol(InputMatrix)")
        return tempMatrix;
    }

    tempMatrix.Resize(1, InputSize.ColCount);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max j = 0; j < InputSize.ColCount; ++j)
    {
        auto value = RawPointer[0];
        ++RawPointer;
        for (int_max i = 1; i < InputSize.RowCount; ++i)
        {
            value += RawPointer[0];
            ++RawPointer;
        }
        tempRawPointer[j] = value;
    }

    return tempMatrix;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixSumOfEachRow(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto InputSize = InputMatrix.GetSize();
    if (InputSize.RowCount <= 0)
    {
        MDK_Error("Input is empty Matrix @ mdkLinearAlgebra_DenseMatrix MatrixSumOfEachRow(InputMatrix)")
        return tempMatrix;
    }

    tempMatrix.Resize(InputSize.RowCount, 1);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max i = 0; i < InputSize.RowCount; ++i)
    {
        auto value = RawPointer[i];
        int_max Index = InputSize.RowCount;
        for (int_max j = 1; j < InputSize.ColCount; ++j)
        {
            value += RawPointer[Index + i];
            Index += InputSize.RowCount;
        }
        tempRawPointer[i] = value;
    }

    return tempMatrix;
}


//===================================================================================================================================//

template<typename ElementType>
int_max MatrixNorm_L0(const DenseMatrix<ElementType>& InputMatrix, ElementType Zero)
{
	auto ElementCount = InputMatrix.GetElementCount();
	if (ElementCount == 0)
	{
		MDK_Warning("empty input matrix @ mdkLinearAlgebra_DenseMatrix MatrixNorm_L0(...)")
		return 0;
	}

	int_max Count = 0;
	auto BeginPointer = InputMatrix.GetElementPointer();
	for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementCount; ++Ptr)
	{
		if (Ptr[0] > Zero)
		{
			Count += 1;
		}		
	}
	return Count;
}


template<typename ElementType>
ElementType MatrixNorm_L1(const DenseMatrix<ElementType>& InputMatrix)
{
    auto ElementCount = InputMatrix.GetElementCount();
    if (ElementCount == 0)
    {
        MDK_Error("empty input matrix @ mdkLinearAlgebra_DenseMatrix MatrixNorm_L1(InputMatrix)")
        return InputMatrix.GetErrorElement();
    }

    ElementType Value = ElementType(0);
    auto BeginPointer = InputMatrix.GetElementPointer();
    for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementCount; ++Ptr)
    {
        Value += std::abs(Ptr[0]);
    }
    return Value;
}


template<typename ElementType>
ElementType MatrixNorm_L2(const DenseMatrix<ElementType>& InputMatrix)
{
    auto ElementCount = InputMatrix.GetElementCount();
    if (ElementCount == 0)
    {
        MDK_Error("empty input matrix @ mdkLinearAlgebra_DenseMatrix MatrixNorm_L2(InputMatrix)")
        return InputMatrix.GetErrorElement();
    }

    auto BeginPointer = InputMatrix.GetElementPointer();
    auto Value = BeginPointer[0] * BeginPointer[0];
    for (auto Ptr = BeginPointer + 1; Ptr < BeginPointer + ElementCount; ++Ptr)
    {
        Value += Ptr[0] * Ptr[0];
    }
    Value = std::sqrt(Value);

    return Value;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixTranspose(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> tempMatrix;

    auto Size = InputMatrix.GetSize();
    if (Size.RowCount == 0)
    {
        MDK_Error("InputMatrix is empty @ mdkLinearAlgebra_DenseMatrix MatrixTranspose(InputMatrix)")
        return tempMatrix;
    }
    if (Size.ColCount == 1)
    {
        tempMatrix.Copy(InputMatrix.GetElementPointer(), 1, Size.RowCount);
        return tempMatrix;
    }

    if (Size.RowCount == 1)
    {
        tempMatrix.Copy(InputMatrix.GetElementPointer(), Size.ColCount, 1);
        return tempMatrix;
    }

    tempMatrix.FastResize(Size.ColCount, Size.RowCount);
    auto tempRawPointer = tempMatrix.GetElementPointer();
    auto RawPointer = InputMatrix.GetElementPointer();
    for (int_max i = 0; i < Size.RowCount; ++i)
    {
        int_max Index = 0;
        for (int_max j = 0; j < Size.ColCount; ++j)
        {
            tempRawPointer[0] = RawPointer[Index + i];
            Index += Size.RowCount;
            ++tempRawPointer;
        }
    }

    return tempMatrix;
}


template<typename ElementType>
void MatrixTransposeInPlace(DenseMatrix<ElementType>& InputMatrix)
{
    auto Size = InputMatrix.GetSize();
    if (Size.RowCount == 0)
    {
        return;
	}
    if (Size.ColCount == 1 || Size.RowCount == 1)
    {
        InputMatrix.Reshape(Size.ColCount, Size.RowCount);
        return;
    }

    //call Armadillo
    //arma::Mat<ElementType> A(InputMatrix.GetElementPointer(), arma::uword(InputMatrix.GetRowCount()), arma::uword(InputMatrix.GetColCount()), false);
    //arma::inplace_trans(A);
    //InputMatrix.Reshape(Size.ColCount, Size.RowCount);

	//call Eigen
	Eigen::Map<Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic>> A(InputMatrix.GetElementPointer(), InputMatrix.GetRowCount(), InputMatrix.GetColCount());
	A.transposeInPlace();	
}


template<typename ElementType>
int_max MatrixRank(const DenseMatrix<ElementType>& InputMatrix)
{
	if (InputMatrix.IsEmpty() == true)
    {
        //MDK_Error("InputMatrix is empty  @ mdkLinearAlgebra_DenseMatrix MatrixRank(InputMatrix)")
        return 0;
    }

	if (InputMatrix.GetRowCount() == 1 || InputMatrix.GetColCount() == 1)
	{
		return 0;
	}

    auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

    //call Armadillo 
	//auto Size = InputMatrix.GetSize();
    //arma::Mat<ElementType> tempMat(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);
    //int_max value = arma::rank(tempMat);

	//call Eigen	
	//http://stackoverflow.com/questions/31041921/how-to-get-rank-of-a-matrix-in-eigen-library
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
	Eigen::Map<EigenMatrixType> A(ptrInputMatrix, InputMatrix.GetRowCount(), InputMatrix.GetColCount());
	//FullPivLU<EigenMatrixType> lu_decomp(A);
	//auto rank = int_max(lu_decomp.rank());
	Eigen::ColPivHouseholderQR<EigenMatrixType> qr_decomp(A);
	auto value = int_max(qr_decomp.rank());

    return value;
}


template<typename ElementType>
ElementType MatrixDeterminant(const DenseMatrix<ElementType>& InputMatrix)
{
	if (InputMatrix.IsEmpty() == true)
	{
		return ElementType(0);
	}

	if (InputMatrix.GetRowCount() == 1 && InputMatrix.GetColCount() == 1)
	{
		return InputMatrix[0];
	}

	if (InputMatrix.IsSquare() == false)
	{
		MDK_Error("InputMatrix is not square  @ mdkLinearAlgebra_DenseMatrix MatrixDeterminant(InputMatrix)")
		return ElementType(0);
	}

	auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

	//call Armadillo 
	//auto Size = InputMatrix.GetSize();
	//arma::Mat<ElementType> tempMat(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);
	//auto Det = arma::det(tempMat);

	//call Eigen
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
	Eigen::Map<EigenMatrixType> A(ptrInputMatrix, InputMatrix.GetRowCount(), InputMatrix.GetColCount());
	auto Det = A.determinant();

	return Det;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixInverse(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrix<ElementType> OutputMatrix;

    auto Size = InputMatrix.GetSize();
    if (Size.RowCount == 0)
    {
		MDK_Warning("InputMatrix is empty matrix @ mdkLinearAlgebra_DenseMatrix MatrixInverse(InputMatrix)")
        return OutputMatrix;
    }

    if (Size.RowCount != Size.ColCount)
    {
        MDK_Error("InputMatrix is not square @ mdkLinearAlgebra_DenseMatrix MatrixInverse(InputMatrix)")
        return OutputMatrix;
    }

	OutputMatrix.FastResize(Size.RowCount, Size.ColCount);

    auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

    // call Armadillo 
    //arma::Mat<ElementType> tempMat(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);
	//arma::Mat<ElementType> tempInv(OutputMatrix.GetElementPointer(), arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);
    //tempInv = arma::inv(tempMat);

	//call Eigen
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
	Eigen::Map<EigenMatrixType> A(ptrInputMatrix, InputMatrix.GetRowCount(), InputMatrix.GetColCount());
	Eigen::Map<EigenMatrixType> B(OutputMatrix.GetElementPointer(), OutputMatrix.GetRowCount(), OutputMatrix.GetColCount());
	B = A.inverse();

	return OutputMatrix;
}


template<typename ElementType>
DenseMatrix<ElementType> MatrixPseudoInverse(const DenseMatrix<ElementType>& InputMatrix)
{
	DenseMatrix<ElementType> OutputMatrix;

	auto Size = InputMatrix.GetSize();
	if (Size.RowCount == 0)
	{
		MDK_Warning("InputMatrix is empty matrix @ mdkLinearAlgebra_DenseMatrix MatrixPseudoInverse(InputMatrix)")
	    return OutputMatrix;
	}

	OutputMatrix.FastResize(Size.ColCount, Size.RowCount);// switch size

	auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

	//call Armadillo 
	//http://arma.sourceforge.net/docs.html#pinv
	//auto tolerance = ElementType(std::max(Size.RowCount, Size.ColCount))*InputMatrix.L1Norm()*std::numeric_limits<ElementType>::epsilon();
	//arma::Mat<ElementType> tempMat(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);
	//arma::Mat<ElementType> tempInv(OutputMatrix.GetElementPointer(), arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);	
	//bool Flag = arma::pinv(tempInv, tempMat, tolerance, "std"); // do not use "dc", it has problem
	//if (Flag == false)
	//{
	//	MDK_Error("Armadillo pinv failed @ @ mdkLinearAlgebra_DenseMatrix MatrixPseudoInverse(InputMatrix)")
	//}

	//call Eigen
	//http://eigen.tuxfamily.org/index.php?title=FAQ#Is_there_a_method_to_compute_the_.28Moore-Penrose.29_pseudo_inverse_.3F
	// X=USV*
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
	Eigen::Map<EigenMatrixType> A(OutputMatrix.GetElementPointer(), OutputMatrix.GetRowCount(), OutputMatrix.GetColCount());
	Eigen::Map<EigenMatrixType> X(ptrInputMatrix, InputMatrix.GetRowCount(), InputMatrix.GetColCount());
	Eigen::JacobiSVD<EigenMatrixType> svd(X, Eigen::ComputeFullU | Eigen::ComputeFullV);
	const auto& S = svd.singularValues();
	const auto& U = svd.matrixU();
	const auto& V = svd.matrixV();
	EigenMatrixType invS(V.cols(), U.rows());
	invS.setZero();
	ElementType tolerance = ElementType(std::max(InputMatrix.GetRowCount(), InputMatrix.GetColCount()))*std::numeric_limits<ElementType>::epsilon();
	for (int_max k = 0; k < S.size(); ++k)
	{
		if (S[k] > tolerance)
		{
			invS(k, k) = ElementType(1) / S(k);
		}
	}
	A = V*invS*U.adjoint();

	return OutputMatrix;
}


template<typename ElementType>
DenseMatrix<ElementType> SolveMatrixLinearEquation(const DenseMatrix<ElementType>& MatrixA, const DenseMatrix<ElementType>& MatrixB)
{	// A*X=B
	DenseMatrix<ElementType> MatrixX;

	auto SizeA = MatrixA.GetSize();
	auto SizeB = MatrixB.GetSize();
	if (SizeA.RowCount != SizeB.RowCount)
	{
		MDK_Error("A RowCount != B RowCount @ mdkLinearAlgebra_DenseMatrix SolveMatrixLinearEquation(MatrixA, MatrixB)")
 	    return MatrixX;
	}
	if (SizeA.RowCount == 0)
	{
		MDK_Warning("MatrixA is empty @ mdkLinearAlgebra_DenseMatrix SolveMatrixLinearEquation(MatrixA, MatrixB)")
	    return MatrixX;
	}

	MatrixX.Resize(SizeA.ColCount, SizeB.ColCount);

	auto ptrX = const_cast<ElementType*>(MatrixX.GetElementPointer());
	auto ptrA = const_cast<ElementType*>(MatrixA.GetElementPointer());
	auto ptrB = const_cast<ElementType*>(MatrixB.GetElementPointer());

	// call Armadillo 
	//arma::Mat<ElementType> tempX(ptrX, arma::uword(SizeA.ColCount), arma::uword(SizeB.ColCount), false);
	//arma::Mat<ElementType> tempA(ptrA, arma::uword(SizeA.RowCount), arma::uword(SizeA.ColCount), false);
	//arma::Mat<ElementType> tempB(ptrB, arma::uword(SizeB.RowCount), arma::uword(SizeB.ColCount), false);
	//auto Flag = arma::solve(tempX, tempA, tempB);
	//if (Flag == false)
	//{
	//	MDK_Error("No solution @ mdkLinearAlgebra_DenseMatrix SolveMatrixLinearEquation(MatrixA, MatrixB)")
	//}

	//call eigen: SVD
	Eigen::Map<Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic>> A(ptrA, SizeA.RowCount, SizeA.ColCount);
	Eigen::Map<Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic>> B(ptrB, SizeB.RowCount, SizeB.ColCount);
	Eigen::Map<Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic>> X(ptrX, SizeA.ColCount, SizeB.ColCount);
	try
	{
		X = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(B);
	}
	catch (...)
	{
		MDK_Error("Eigen-Lib error @ mdkLinearAlgebra_DenseMatrix SolveMatrixLinearEquation(MatrixA, MatrixB)")
	}
	
	return MatrixX;
}


template<typename ElementType>
DenseMatrixEigenResult<std::complex<ElementType>> NonSymmetricRealMatrixEigen(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrixEigenResult<std::complex<ElementType>> Result;

    auto Size = InputMatrix.GetSize();
    if (Size.RowCount == 0)
    {
        MDK_Error("InputMatrix is empty matrix @ mdkLinearAlgebra_DenseMatrix NonSymmetricRealMatrixEigen(InputMatrix)")

        return Result;
    }
    if (Size.RowCount != Size.ColCount)
    {
        MDK_Error("InputMatrix is not square @ mdkLinearAlgebra_DenseMatrix NonSymmetricRealMatrixEigen(InputMatrix)")

        return Result;
    }

    Result.EigenVector.FastResize(Size.RowCount, Size.RowCount);
    Result.EigenValue.FastResize(Size.RowCount, 1);
    auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

    //call Armadillo 
    //arma::Mat<ElementType> tempMat(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.RowCount), false);
    //arma::Mat<std::complex<ElementType>> tempEigenVector(Result.EigenVector.GetElementPointer(), arma::uword(Size.RowCount), arma::uword(Size.RowCount), false);
    //arma::Col<std::complex<ElementType>> tempEigenValue(Result.EigenValue.GetElementPointer(), arma::uword(Size.RowCount), false);
    //arma::eig_gen(tempEigenValue, tempEigenVector, tempMat);

	//call Eigen
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenRealMatrixType;
	typedef Eigen::Matrix<std::complex<ElementType>, Eigen::Dynamic, Eigen::Dynamic> EigenComplexMatrixType;
	Eigen::Map<EigenRealMatrixType> A(ptrInputMatrix, Size.RowCount, Size.ColCount);
	Eigen::Map<EigenComplexMatrixType> V(Result.EigenVector.GetElementPointer(), Result.EigenVector.GetRowCount(), Result.EigenVector.GetColCount());
	Eigen::Map<EigenComplexMatrixType> S(Result.EigenValue.GetElementPointer(), Result.EigenValue.GetRowCount(), Result.EigenValue.GetColCount());
	Eigen::EigenSolver<EigenMatrixType> es(A);
	S = es.eigenvalues();
	V = es.eigenvectors();

    return Result;
}


template<typename ElementType>
DenseMatrixEigenResult<ElementType> SymmetricRealMatrixEigen(const DenseMatrix<ElementType>& InputMatrix, bool CheckIfSymmetric)
{
    DenseMatrixEigenResult<ElementType> Result;

    auto Size = InputMatrix.GetSize();
    if (Size.RowCount == 0)
    {
        MDK_Error("Matrix is empty matrix @ mdkLinearAlgebra_DenseMatrix SymmetricRealMatrixEigen(...)")
        return Result;
    }
    if (Size.RowCount != Size.ColCount)
    {
        MDK_Error("Matrix is not square @ mdkLinearAlgebra_DenseMatrix SymmetricRealMatrixEigen(...)")
        return Result;
    }
    if (CheckIfSymmetric == true)
    {
        DenseMatrix<ElementType> tempMatrix_2 = InputMatrix - InputMatrix.Transpose();
		for (auto& i : tempMatrix_2) { i = std::abs(i); }

        double tempsum = double(tempMatrix_2.Sum());
		auto EPS = std::numeric_limits<ElementType>::epsilon();
        if (std::abs(tempsum) > EPS)
        {
			MDK_Error("Matrix is not Symmetric, abort @ mdkLinearAlgebra_DenseMatrix SymmetricRealMatrixEigen(...)")
			return Result;
        }
    }
  
    Result.EigenVector.FastResize(Size.RowCount, Size.RowCount);
    Result.EigenValue.FastResize(Size.RowCount, 1);

    auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

    //call Armadillo
    //arma::Mat<ElementType> tempMat(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.RowCount), false);
    //arma::Mat<ElementType> tempEigenVector(Result.EigenVector.GetElementPointer(), arma::uword(Size.RowCount), arma::uword(Size.RowCount), false);
    //arma::Col<ElementType> tempEigenValue(Result.EigenValue.GetElementPointer(), arma::uword(Size.RowCount), false);
    //arma::eig_sym(tempEigenValue, tempEigenVector, tempMat);

	//call Eigen
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenRealMatrixType;
	typedef Eigen::Matrix<std::complex<ElementType>, Eigen::Dynamic, Eigen::Dynamic> EigenComplexMatrixType;
	Eigen::Map<EigenRealMatrixType> A(ptrInputMatrix, Size.RowCount, Size.ColCount);
	Eigen::Map<EigenRealMatrixType> V(Result.EigenVector.GetElementPointer(), Result.EigenVector.GetRowCount(), Result.EigenVector.GetColCount());
	Eigen::Map<EigenRealMatrixType> S(Result.EigenValue.GetElementPointer(), Result.EigenValue.GetRowCount(), Result.EigenValue.GetColCount());
	Eigen::SelfAdjointEigenSolver<EigenRealMatrixType> es(A);
	S = es.eigenvalues();
	V = es.eigenvectors();

    return Result;
}


template<typename ElementType>
DenseMatrixPCAResult<ElementType> MatrixPCA(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrixPCAResult<ElementType> PCAResult;

    auto Size = InputMatrix.GetSize();
    if (Size.ColCount <= 1)
    {
        MDK_Error("ColCount <= 1, return empty PCAResult @ mdkLinearAlgebra_DenseMatrix MatrixPCA(...)")
        return PCAResult;
    }

    auto MeanCol = InputMatrix.SumToCol();
    MeanCol /= Size.ColCount;

    DenseMatrix<ElementType> CovarianceMatrix(Size.RowCount, Size.RowCount);

    CovarianceMatrix.Fill(0);

    // reference only
    //for (int_max i = 0; i < Size.ColCount; ++i)
    //{
    //    DenseMatrix<ElementType> tempCol = InputMatrix(ALL, { i }) - MeanCol;       
    //    CovarianceMatrix += tempCol * tempCol.Transpose();
    //}

    DenseMatrix<ElementType> tempMatrix(Size.RowCount, Size.RowCount);
    DenseMatrix<ElementType> tempCol(Size.RowCount, 1);
    DenseMatrix<ElementType> tempColTranspose(1, Size.RowCount);
    for (int_max i = 0; i < Size.ColCount; ++i)
    {
        InputMatrix.GetCol(i, tempCol);
        MatrixSubtract(tempCol, tempCol, MeanCol);
        tempColTranspose = tempCol.Transpose();
        MatrixMultiply(tempMatrix, tempCol, tempColTranspose);
        CovarianceMatrix += tempMatrix;
    }
    CovarianceMatrix /= Size.ColCount;
    auto EigenResult = SymmetricRealMatrixEigen(CovarianceMatrix);
    //--------------------------------------------------------------//
    PCAResult.Mean = std::move(MeanCol);
    PCAResult.EigenVector = std::move(EigenResult.EigenVector);
    PCAResult.EigenValue = std::move(EigenResult.EigenValue);

    return PCAResult;
}


template<typename ElementType>
DenseMatrixSVDResult<ElementType> MatrixSVD(const DenseMatrix<ElementType>& InputMatrix)
{
    DenseMatrixSVDResult<ElementType> Result;

    auto Size = InputMatrix.GetSize();
    if (Size.RowCount == 0)
    {
        MDK_Error("Matrix is empty  @ mdkLinearAlgebra_DenseMatrix MatrixSVD(InputMatrix)")
        return Result;
    }

	Result.U.FastResize(Size.RowCount, Size.RowCount);
    Result.S.FastResize(std::min(Size.RowCount, Size.ColCount), 1);
	Result.V.FastResize(Size.ColCount, Size.ColCount);

    auto ptrInputMatrix = const_cast<ElementType*>(InputMatrix.GetElementPointer());

    //call Armadillo : can not handle non-square matrix
	//arma::Mat<ElementType> X(ptrInputMatrix, arma::uword(Size.RowCount), arma::uword(Size.ColCount), false);
	//arma::Mat<ElementType> U(Result.U.GetElementPointer(), arma::uword(Result.U.GetRowCount()), arma::uword(Result.U.GetColCount()), false);
	//arma::Col<ElementType> S(Result.S.GetElementPointer(), arma::uword(Result.S.GetRowCount()), false);
	//arma::Mat<ElementType> V(Result.V.GetElementPointer(), arma::uword(Result.V.GetRowCount()), arma::uword(Result.V.GetColCount()), false);
    //arma::svd(U, S, V, X);

	//call eigen	
	// X=USV*
	typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
	Eigen::Map<const EigenMatrixType> X(ptrInputMatrix, Size.RowCount, Size.ColCount);
	Eigen::JacobiSVD<EigenMatrixType> svd(X, Eigen::ComputeFullU | Eigen::ComputeFullV);	
	const auto& S = svd.singularValues();
	const auto& U = svd.matrixU();
	const auto& V = svd.matrixV();

	for (int_max k = 0; k < Result.S.GetElementCount(); ++k)
	{
		Result.S[k] = S[k];
	}

	if (U.IsRowMajor == false)
	{
		for (int_max k = 0; k < Result.U.GetElementCount(); ++k)
		{
			Result.U[k] = U.data()[k];
		}
	}
	else
	{
		for (int_max j = 0; j < Size.RowCount; ++j)
		{
			for (int_max i = 0; i < Size.RowCount; ++i)
			{
				Result.U(i, j) = U(i, j);
			}
		}
	}

	if (V.IsRowMajor == false)
	{
		for (int_max k = 0; k < Result.V.GetElementCount(); ++k)
		{
			Result.V[k] = V.data()[k];
		}
	}
	else
	{
		for (int_max j = 0; j < Size.ColCount; ++j)
		{
			for (int_max i = 0; i < Size.ColCount; ++i)
			{
				Result.V(i, j) = V(i, j);
			}
		}
	}

    return Result;
}

}//end namespace mdk
