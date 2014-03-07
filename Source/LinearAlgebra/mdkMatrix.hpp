#ifndef __mdkMatrix_hpp
#define __mdkMatrix_hpp

#include <string>
#include <cmath>
#include <algorithm>

#include "armadillo.h"

#include "mdkType.h"
#include "mdkDebugConfig.h"
#include "mdkMatrix.h"
#include "mdkShadowMatrix.h"
#include "mdkGlueMatrixForLinearCombination.h"
#include "mdkLinearAlgebra.h"

namespace mdk
{
    
//mdkObject mkdObject_For_mdkMatrix;

template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix()
{
	this->Reset();
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(uint64 RowNumber, uint64 ColNumber, bool IsSizeFixed = false)
{
    this->Reset();

    this->Resize(RowNumber, ColNumber);

    m_IsSizeFixed = IsSizeFixed;

    if (IsSizeFixed == true)
    {
        m_ElementData->shrink_to_fit();
    }
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(const mdkMatrix<ElementType>& targetMatrix, bool IsSizeFixed = false)
{
    this->Reset();

    //force-copy data
    this->Copy(targetMatrix);     

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline 
mdkMatrix<ElementType>::mdkMatrix(const ElementType& Element, bool IsSizeFixed = false)
{
    this->Reset();

    this->Resize(1, 1);

    (*this)(0) = Element;

    m_IsSizeFixed = IsSizeFixed;
}


// move constructor
template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(mdkMatrix<ElementType>&& targetMatrix, bool IsSizeFixed = false)
{
    // not necessary to use this->Reset()

    //force-eat data
    m_IsSizeFixed = false;
    this->Eat(targetMatrix);

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(const mdkShadowMatrix<ElementType>& ShadowMatrix, bool IsSizeFixed = false)
{
    // not necessary to use this->Reset()

    //force-share data
    m_IsSizeFixed = false;
    this->Eat(ShadowMatrix.CreateMatrix());

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(const mdkGlueMatrixForLinearCombination<ElementType>& GlueMatrix, bool IsSizeFixed = false)
{
    // not necessary to use this->Reset()

    //force-share data
    m_IsSizeFixed = false;
    this->Eat(GlueMatrix.CreateMatrix());

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(const mdkGlueMatrixForMultiplication<ElementType>& GlueMatrix, bool IsSizeFixed = false)
{
    // not necessary to use this->Reset()

    //force-share data
    m_IsSizeFixed = false;
    this->Eat(GlueMatrix.CreateMatrix());

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(const ElementType* ElementPointer, uint64 RowNumber, uint64 ColNumber, bool IsSizeFixed = false)
{
    this->Reset();

    if (ElementPointer == nullptr )
    {
        mdkWarning << "Empty input @ mdkMatrix::mdkMatrix(ElementType*, uint64, uint64, bool)" << '\n';
        return;
    }

    if (RowNumber == 0 || ColNumber == 0)
    {
        if (ElementPointer != nullptr)
        {
            mdkError << "Invalid input @ mdkMatrix::mdkMatrix(ElementType*, uint64, uint64, bool)" << '\n';
            return;
        }
    }

    // force-copy data
    this->Copy(ElementPointer, RowNumber, ColNumber);

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::mdkMatrix(std::vector<ElementType>* ElementDataPointer, uint64 RowNumber, uint64 ColNumber, bool IsSizeFixed = false)
{
    this->Reset();

    if (ElementPointer == nullptr)
    {
        mdkWarning << "Empty input @ mdkMatrix::mdkMatrix(std::vector<ElementType>*, uint64, uint64, bool)" << '\n';
        return;
    }

    if (RowNumber == 0 || ColNumber == 0)
    {
        if (ElementPointer != nullptr)
        {
            mdkError << "Invalid input @ mdkMatrix::mdkMatrix(std::vector<ElementType>*, uint64, uint64, bool)" << '\n';
            return;
        }
    }

    // only use, do not own
    m_ElementData.reset(ElementDataPointer, [](ElementType*){});

    m_RowNumber = RowNumber;

    m_ColNumber = ColNumber;

    m_ElementNumber = m_RowNumber*m_ColNumber;

    m_IsSizeFixed = IsSizeFixed;
}


template<typename ElementType>
inline
mdkMatrix<ElementType>::~mdkMatrix()
{
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::Reset()
{
    m_RowNumber = 0;
    m_ColNumber = 0;
    m_ElementNumber = 0;

    m_ElementData.reset();

    m_IsSizeFixed = false;

    m_NaNElement = GetMatrixNaNElement(m_NaNElement);
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::Clear()
{
    if (m_IsSizeFixed == true)
    {
        if (m_RowNumber > 0)
        {
            mdkError << "Size can not change @ mdkMatrix::Clear" << '\n';
            return false;
        }
    }

    m_RowNumber = 0;
    m_ColNumber = 0;
    m_ElementNumber = 0;

    m_ElementData.reset();

    m_IsSizeFixed = false;

    return true;
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::ForceClear()
{
    m_RowNumber = 0;
    m_ColNumber = 0;
    m_ElementNumber = 0;

    m_ElementData.reset();

    m_IsSizeFixed = false;
}


template<typename ElementType>
inline
mdkMatrixElementTypeEnum mdkMatrix<ElementType>::GetElementType() const
{
    return FindMatrixElementType(m_NaNElement);
}


template<typename ElementType>
inline 
const ElementType& mdkMatrix<ElementType>::GetNaNElement()  const
{
    return m_NaNElement;
}


template<typename ElementType>
inline
const std::shared_ptr<std::vector<ElementType>>& mdkMatrix<ElementType>::GetElementDataSharedPointer() const
{
    return m_ElementData;
}


template<typename ElementType>
inline
ElementType* mdkMatrix<ElementType>::GetElementDataRawPointer()
{
    if (m_ElementData) // if (m_ElementData != nullptr)
    {
        return m_ElementData->data();
    }
    else
    {
        return nullptr;
    }
}


template<typename ElementType>
inline
const ElementType* mdkMatrix<ElementType>::GetElementDataRawPointer() const
{
    if (m_ElementData) // if (m_ElementData != nullptr)
    {
        return m_ElementData->data();
    }
    else
    {
        return nullptr;
    }
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::Reshape(uint64 targetRowNumber, uint64 targetColNumber)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix size can not be changed @ mdkMatrix::Reshape" << '\n';
        return false;
    }

    if (m_ElementData == nullptr)
    {
        if (targetRowNumber == 0 || targetColNumber == 0)
        {
            m_RowNumber = 0;

            m_ColNumber = 0;

            m_ElementNumber = 0;

            return true;
        }
        else
        {
            mdkError << "Self is empty and Size does not match @ mdkMatrix::Reshape" << '\n';
            return false;
        }
    }

    if (targetRowNumber*targetColNumber != this->GetElementNumber())
    {
        mdkError << "Size does not match @ mdkMatrix::Reshape" << '\n';
        return false;
    }

    m_RowNumber = targetRowNumber;

    m_ColNumber = targetColNumber;

    m_ElementNumber = m_RowNumber*m_ColNumber;

    return true;
}


template<typename ElementType>
inline 
bool mdkMatrix<ElementType>::Resize(uint64 targetRowNumber, uint64 targetColNumber, bool IsSizeFixed == false)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not be changed @ mdkMatrix::Resize(uint64 RowNumber, uint64 ColNumber)" << '\n';
        return false;
    }

    if (targetRowNumber == m_RowNumber && targetColNumber == m_ColNumber)
    {
        m_IsSizeFixed = IsSizeFixed;

        return true;
    }

    if (targetRowNumber == 0 || targetColNumber == 0)
    {
        this->Clear();

        m_IsSizeFixed = IsSizeFixed;

        return true;
    }

    // if self is empty
    if (m_ElementNumber == 0)
    {
        m_RowNumber = targetRowNumber;
        m_ColNumber = targetColNumber;
        m_ElementNumber = m_RowNumber*m_ColNumber;

        m_ElementData = std::make_shared<std::vector<ElementType>>(targetRowNumber*targetColNumber);

        m_IsSizeFixed = IsSizeFixed;

        return true;
    }

    if (targetRowNumber == m_RowNumber && targetColNumber != m_ColNumber)
    {
        if (m_ElementData->capacity() - this->GetElementNumber() < (targetColNumber - m_ColNumber)*m_RowNumber)
        {
            m_ElementData->reserve((m_ColNumber + MDK_Matrix_ColExpansionStep)*m_RowNumber);
        }

        m_ColNumber = targetColNumber;

        m_ElementNumber = m_RowNumber*m_ColNumber;

        m_ElementData->resize(m_ColNumber*m_RowNumber);

        m_IsSizeFixed = IsSizeFixed;

        return true;
    }

    // RowNumber != m_RowNumber -----------------------------------------------------------------------------

    auto tempElementData = std::make_shared<std::vector<ElementType>>(targetRowNumber*targetColNumber);

    auto tempRawPointer = tempElementData->data();

    auto RawPointer = m_ElementData->data();

    auto ColNumber_min = std::min(m_ColNumber, targetColNumber);

    auto RowNumber_min = std::min(m_RowNumber, targetRowNumber);

    for (uint64 j = 0; j < ColNumber_min; ++j)
    {
        auto tempIndex = j*targetRowNumber;

        auto Index = j*m_RowNumber;

        for (uint64 i = 0; i < RowNumber_min; ++i)
        {
            tempRawPointer[tempIndex + i] = RawPointer[Index + i];
        }
    }

    m_RowNumber = targetRowNumber;
    m_ColNumber = targetColNumber;
    m_ElementNumber = m_RowNumber*m_ColNumber;

    m_ElementData = tempElementData;

    m_IsSizeFixed = IsSizeFixed;

    return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::IsSizeFixed() const
{
    return m_IsSizeFixed;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::IsEmpty() const
{
    return !(m_ElementData); 
}


template<typename ElementType>
inline
mdkMatrixSize mdkMatrix<ElementType>::GetSize() const
{
    mdkMatrixSize Size;

    Size.RowNumber = m_RowNumber;

    Size.ColNumber = m_ColNumber;

    return Size;
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::GetSize(uint64* RowNumber, uint64* ColNumber) const
{
    RowNumber[0] = m_RowNumber;

    ColNumber[0] = m_ColNumber;
}


template<typename ElementType>
inline
uint64 mdkMatrix<ElementType>::GetElementNumber() const
{
    return m_RowNumber * m_ColNumber
}


template<typename ElementType>
inline 
uint64 mdkMatrix<ElementType>::GetColNumber() const
{
	return m_ColNumber;
}


template<typename ElementType>
inline 
uint64 mdkMatrix<ElementType>::GetRowNumber() const
{
	return m_RowNumber;
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(const ElementType& Element)
{
	if (m_IsSizeFixed == true)
	{
        if (m_ElementNumber != 1)
		{
			mdkError << "Can not change matrix size @ mdkMatrix::operator=(Element)" << '\n';
			return;
		}
	}

    if (m_ElementNumber == 1)
    {
        (*m_ElementData)[0] = Element;
    }
    else
    {
        mdkWarning << "The size of the matrix changes to 1x1 @ mdkMatrix::operator=(Element)" << '\n';

        m_ElementData = std::make_shared<std::vector<ElementType>>(1);

        (*m_ElementData)[0] = Element;

        m_RowNumber = 1;
        m_ColNumber = 1;
        m_ElementNumber = 1;
    }

	return;
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(const std::initializer_list<ElementType>& list)
{
    //if Self is empty
    if (m_ElementNumber == 0)
	{
		mdkError << "operator=(list) can only be called after SetSzie() @ mdkMatrix::operator=(list)" << '\n';
		return;
	}

    if (m_ElementNumber != list.size())
	{
		mdkError << "Size does not match @ mdkMatrix::operator=(list)" << '\n';
		return;
	}

	auto RawPointer = m_ElementData->data();

	uint64 Counter = 0;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		uint64 Index = 0;

		for (uint64 j = 0; j < m_ColNumber; ++j)
		{
            RawPointer[Index + i] = list.begin()[Counter];

			Index += m_RowNumber;

			Counter += 1;
		}
	}

	return;
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(const mdkMatrix<ElementType>& targetMatrix)
{
    this->Copy(targetMatrix);
}


// move assignment operator
template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(mdkMatrix<ElementType>&& targetMatrix)
{
    this->Eat(targetMatrix);
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(const mdkShadowMatrix<ElementType>& ShadowMatrix)
{
    this->Eat(ShadowMatrix);
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(const mdkGlueMatrixForLinearCombination<ElementType>& GlueMatrix)
{
    this->Eat(GlueMatrix);
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator=(const mdkGlueMatrixForMultiplication<ElementType>& GlueMatrix)
{
    this->Eat(GlueMatrix);
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::Eat(mdkMatrix<ElementType>& targetMatrix)
{
    // MatrixA = MatrixA
    if (this == &targetMatrix)
    {
        mdkWarning << "A Matrix tries to eat itself @ mdkMatrix::Eat(targetMatrix)" << '\n';
        return false;
    }

    auto RowNumber = targetMatrix.GetRowNumber();

    auto ColNumber = targetMatrix.GetColNumber();

    if (m_IsSizeFixed == true)
    {
        if (RowNumber != m_RowNumber || ColNumber != m_ColNumber)
        {
            mdkError << "Matrix size can not be changed @ mdkMatrix::eat(targetMatrix)" << '\n';
            return false;
        }
    }
    else
    {
        if (targetMatrix.IsEmpty() == true)
        {
            mdkWarning << "Input is empty, and this matrix is set to be empty @ mdkMatrix::eat(targetMatrix)" << '\n';

            this->Clear();

            return true;
        }
    }

    m_RowNumber = RowNumber;

    m_ColNumber = ColNumber;

    m_ElementNumber = m_RowNumber*m_ColNumber;

    m_ElementData = targetMatrix.GetElementDataSharedPointer();

    targetMatrix.ForceClear();

    return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::Eat(const mdkShadowMatrix<ElementType>& ShadowMatrix)
{
    if (m_IsSizeFixed == true)
    {
        if (ShadowMatrix.GetRowNumber() != m_RowNumber || ShadowMatrix.GetColNumber() != m_ColNumber)
        {
            mdkError << "Matrix size can not be changed @ mdkMatrix::Eat(ShadowMatrix)" << '\n';
            return false;
        }

        ShadowMatrix.CreateMatrix(*this);

        return true;
    }
   
    // m_IsSizeFixed == false

    if (ShadowMatrix.GetRowNumber() == m_RowNumber || ShadowMatrix.GetColNumber() == m_ColNumber)
    {
        ShadowMatrix.CreateMatrix(*this);
    }
    else
    {
        this->Eat(ShadowMatrix.CreateMatrix());
    }

    return true;    
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::Eat(const mdkGlueMatrixForLinearCombination<ElementType>& GlueMatrix)
{
    if (m_IsSizeFixed == true)
    {
        if (GlueMatrix.GetRowNumber() != m_RowNumber || GlueMatrix.GetColNumber() != m_ColNumber)
        {
            mdkError << "Matrix size can not be changed @ mdkMatrix::Eat(GlueMatrixForLinearCombination)" << '\n';
            return false;
        }

        ShadowMatrix.CreateMatrix(*this);

        return true;
    }

    // m_IsSizeFixed == false

    if (ShadowMatrix.GetRowNumber() == m_RowNumber || ShadowMatrix.GetColNumber() == m_ColNumber)
    {
        ShadowMatrix.CreateMatrix(*this);
    }
    else
    {
        this->Eat(ShadowMatrix.CreateMatrix());
    }

    return true;
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::Eat(const mdkGlueMatrixForMultiplication<ElementType>& GlueMatrix)
{
    if (m_IsSizeFixed == true)
    {
        if (GlueMatrix.GetRowNumber() != m_RowNumber || GlueMatrix.GetColNumber() != m_ColNumber)
        {
            mdkError << "Matrix size can not be changed @ mdkMatrix::Eat(GlueMatrixForMultiplication)" << '\n';
            return false;
        }

        ShadowMatrix.CreateMatrix(*this);

        return true;
    }

    // m_IsSizeFixed == false

    if (ShadowMatrix.GetRowNumber() == m_RowNumber || ShadowMatrix.GetColNumber() == m_ColNumber)
    {
        ShadowMatrix.CreateMatrix(*this);
    }
    else
    {
        this->Eat(ShadowMatrix.CreateMatrix());
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_target>
inline
bool mdkMatrix<ElementType>::Copy(const mdkMatrix<ElementType_target>& targetMatrix)
{
    if (this == &targetMatrix)
    {
        mdkWarning << "A Matrix tries to Copy itself @ mdkMatrix::Copy(targetMatrix)" << '\n';
        return false;
    }

	if (targetMatrix.IsEmpty() == true)
	{
		mdkWarning << "targetMatrix is empty, and this matrix is set to be empty @ mdkMatrix::Copy(targetMatrix)" << '\n';

        this->Clear();

		return true;
	}

    // copy data
    return this->Copy(targetMatrix.GetElementDataRawPointer(), targetMatrix.GetRowNumber(), targetMatrix.GetColNumber());
}


template<typename ElementType>
template<typename ElementType_target>
inline 
bool mdkMatrix<ElementType>::Copy(const ElementType_target* ElementPointer, uint64 RowNumber, uint64 ColNumber)
{
    if (ElementPointer == nullptr || RowNumber == 0 || ColNumber == 0)
	{
        mdkError << "Input pointer is nullptr @ mdkMatrix::Copy(ElementType_target*, RowNumber, ColNumber)" << '\n';
		return true;
	}

    auto tempElementType = FindMatrixElementType(ElementPointer[0]);

    if (tempElementType == mdkMatrixElementTypeEnum::UNKNOWN)
    {
        mdkError << "input type is unknown @ mdkMatrix::Copy(ElementType_target*, RowNumber, ColNumber)" << '\n';
        return false;
    }

    // if this matrix is not empty, check if this and target share the same data
    if (m_RowNumber > 0)
    {
        if (std::size_t(ElementPointer) == std::size_t(m_ElementData->data()))
        {
            mdkWarning << "A Matrix tries to Copy itself @ mdkMatrix::Copy(ElementType_target*, RowNumber, ColNumber)" << '\n';
            return false;
        }
    }

    if (m_IsSizeFixed == true)
    {
        if (RowNumber != m_RowNumber || ColNumber != m_ColNumber)
        {
            mdkError << "Can not change matrix size @ mdkMatrix::Copy(ElementType_target*, RowNumber, ColNumber)" << '\n';
            return false;
        }
    }
    else
    {
        // check to see if new memory allocation is needed --------------------------------

        bool IsNewMemoryNeeded = false;

        //if self is empty
        if (m_ElementNumber == 0)
        {
            IsNewMemoryNeeded = true;
        }
        else
        {
            if (RowNumber != m_RowNumber || ColNumber != m_ColNumber)
            {
                IsNewMemoryNeeded = true;
            }
        }

        if (IsNewMemoryNeeded == true)
        {
            m_RowNumber = RowNumber;

            m_ColNumber = ColNumber;

            m_ElementNumber = m_RowNumber*m_ColNumber;

            m_ElementData = std::make_shared<std::vector<ElementType>>(RowNumber*ColNumber);
        }
    }

    //copy data ----------------------------------------------------------

	auto RawPointer = m_ElementData->data();

    auto tempPtr = ElementPointer;

    for (auto Ptr = RawPointer; Ptr < RawPointer + m_ElementNumber; ++Ptr)
	{
        Ptr[0] = ElementType(tempPtr[0]);

        ++tempPtr;
	}

	return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::Fill(const ElementType& Element)
{
	if (m_ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::Fill" << '\n';
		return false;
	}

    auto RawPointer = m_ElementData->data();

    for (auto Ptr = RawPointer; Ptr < RawPointer + m_ElementNumber; ++Ptr)
    {
        Ptr[0] = Element;
    }

	return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::SharedCopy(const mdkMatrix<ElementType>& targetMatrix)
{
    // MatrixA = MatrixA
    if (this == &targetMatrix)
    {
        mdkWarning << "A Matrix tries to SharedCopy itself @ mdkMatrix::SharedCopy(targetMatrix)" << '\n';
        return false;
    }

    auto targetSize = targetMatrix.GetSize();

    if (m_IsSizeFixed == true)
    {
        if (targetSize.RowNumber != m_RowNumber || targetSize.ColNumber != m_ColNumber)
        {
            mdkError << "Matrix size can not be changed @ mdkMatrix::SharedCopy(targetMatrix)" << '\n';
            return false;
        }
    }

    m_RowNumber = targetMatrix.GetRowNumber();

    m_ColNumber = targetMatrix.GetColNumber();

    m_ElementNumber = m_RowNumber*m_ColNumber;

    m_ElementData = targetMatrix.GetElementDataSharedPointer();

    return true;
}


template<typename ElementType>
inline
ElementType& mdkMatrix<ElementType>::operator()(uint64 LinearIndex)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    if (LinearIndex >= m_ElementNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::operator(i)" <<'\n';
        
        return m_NaNElement;
	}

#endif

	return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
const ElementType& mdkMatrix<ElementType>::operator()(uint64 LinearIndex) const
{
#if defined(MDK_Matrix_Operator_CheckBound)

	if (LinearIndex >= m_ElementNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::operator(i) const" << '\n';
        
        return m_NaNElement;
	}

#endif

	return (*m_ElementData)[LinearIndex];
}


#if defined MDK_Matrix_Enable_Operator_Bracket //-------------------------------------------------------------------------------

template<typename ElementType>
inline
ElementType& mdkMatrix<ElementType>::operator[](uint64 LinearIndex)
{
#if defined(MDK_Matrix_Operator_CheckBound)

	if (LinearIndex >= m_ElementNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::operator[i]" << '\n';
        
        return m_NaNElement;
	}

#endif

	return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
const ElementType& mdkMatrix<ElementType>::operator[](uint64 LinearIndex) const
{
#if defined(MDK_Matrix_Operator_CheckBound)

	if (LinearIndex >= m_ElementNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::operator[i] const" << '\n';
        
        return m_NaNElement;
	}

#endif

	return (*m_ElementData)[LinearIndex];
}

#endif // #if defined MDK_Matrix_Enable_Operator_Bracket ----------------------------------------------------------------------------------


template<typename ElementType>
inline
ElementType& mdkMatrix<ElementType>::at(uint64 LinearIndex)
{
	if (LinearIndex >= m_ElementNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::at(i)" << '\n';
        
        return m_NaNElement;
	}

	return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
const ElementType& mdkMatrix<ElementType>::at(uint64 LinearIndex) const
{
    if (LinearIndex >= m_ElementNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::at(i) const" << '\n';
        
        return m_NaNElement;
	}

	return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
ElementType& mdkMatrix<ElementType>::operator()(uint64 RowIndex, uint64 ColIndex)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    if (RowIndex >= m_RowNumber || ColIndex >= m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::operator(i,j)" << '\n';
        
        return m_NaNElement;
	}

#endif

    auto LinearIndex = ColIndex*m_RowNumber + RowIndex;

    return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
const ElementType& mdkMatrix<ElementType>::operator()(uint64 RowIndex, uint64 ColIndex) const
{
#if defined(MDK_Matrix_Operator_CheckBound)

    if (RowIndex >= m_RowNumber || ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::operator(i,j) const" << '\n';
        
        return m_NaNElement;
    }

#endif

    auto LinearIndex = ColIndex*m_RowNumber + RowIndex;

    return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
ElementType& mdkMatrix<ElementType>::at(uint64 RowIndex, uint64 ColIndex)
{
    if (RowIndex >= m_RowNumber || ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::at(i,j)" << '\n';
        
        return m_NaNElement;
    }

    auto LinearIndex = ColIndex*m_RowNumber + RowIndex;

    return (*m_ElementData)[LinearIndex];
}


template<typename ElementType>
inline
const ElementType& mdkMatrix<ElementType>::at(uint64 RowIndex, uint64 ColIndex) const
{
    if (RowIndex >= m_RowNumber || ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::at(i,j) const" << '\n';
        
        return m_NaNElement;
    }

    auto LinearIndex = ColIndex*m_RowNumber + RowIndex;

    return (*m_ElementData)[LinearIndex];
}



template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(std::initializer_list<uint64>& LinearIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= m_ElementNumber)
        {
            mdkError << "Invalid LinearIndexList @ mdkMatrix::operator()({LinearIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(const std::vector<uint64>& LinearIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= m_ElementNumber)
        {
            mdkError << "Invalid LinearIndexList @ mdkMatrix::operator()(std::vector LinearIndexList)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }
#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol)
{
    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol);

    return tempShadowMatrix;
}


#if defined MDK_Matrix_Enable_Operator_Bracket //-------------------------------------------------------------------------------

template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator[](std::initializer_list<uint64>& LinearIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= m_ElementNumber)
        {
            mdkError << "Invalid LinearIndexList @ mdkMatrix::operator()({LinearIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator[](const std::vector<uint64>& LinearIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= m_ElementNumber)
        {
            mdkError << "Invalid LinearIndexList @ mdkMatrix::SubMatrix(std::vector LinearIndexList)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }
#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator[](const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol)
{
    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol);

    return tempShadowMatrix;
}

#endif // #if defined MDK_Matrix_Enable_Operator_Bracket -------------------------------------------------------------------------------


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(std::initializer_list<uint64>& RowIndexList, std::initializer_list<uint64>& ColIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::operator()({RowIndexList}, {ColIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::operator()({RowIndexList}, {ColIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    if (RowIndexList.size() == 0 || ColIndexList.size() == 0)
    {
        mdkError << "Empty Input @ mdkMatrix::operator()({RowIndexList}, {ColIndexList})" << '\n';
        mdkShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::operator()(const std::initializer_list<uint64>& RowIndexList, const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::operator()({RowIndexList}, ALL)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::operator()(const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol, const std::initializer_list<uint64>& ColIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::operator()(ALL, {ColIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(const std::vector<uint64>& RowIndexList, const std::vector<uint64>& ColIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::operator()(std::vector RowIndexList, ColIndexList)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::operator()(std::vector RowIndexList, ColIndexList)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    if (RowIndexList.size() == 0 || ColIndexList.size() == 0)
    {
        mdkError << "Empty Input @ mdkMatrix::operator()({RowIndexList}, {ColIndexList})" << '\n';
        mdkShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(const std::vector<uint64>& RowIndexList, const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::operator()({RowIndexList}, ALL)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::operator()(const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol, const std::vector<uint64>& ColIndexList)
{
#if defined(MDK_Matrix_Operator_CheckBound)

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::operator()(ALL, {ColIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline 
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::SubMatrix(uint64 RowIndex_start, uint64 RowIndex_end, uint64 ColIndex_start, uint64 ColIndex_end)
{
    if (RowIndex_start >= m_RowNumber || RowIndex_end >= m_RowNumber
        || ColIndex_start >= m_ColNumber || ColIndex_end >= m_ColNumber
        || RowIndex_start > RowIndex_end || ColIndex_start > ColIndex_end)
    {
        mdkError << "Invalid input @ mdkMatrix::SubMatrix(rs, re, cs, ce)" << '\n';
        mdkShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    std::vector<uint64> RowIndexList(RowIndex_end - RowIndex_start + 1);

    for (uint64 i = RowIndex_start; i <= RowIndex_end; ++i)
    {
        RowIndexList[i - RowIndex_start] = i;
    }

    std::vector<uint64> ColIndexList(ColIndex_end - ColIndex_start + 1);

    for (uint64 j = ColIndex_start; j <= ColIndex_end; ++j)
    {
        ColIndexList[j - ColIndex_start] = j;
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline ElementType& 
mdkMatrix<ElementType>::SubMatrix(uint64 RowIndex, uint64 ColIndex)
{
    return (*this)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::SubMatrix(std::initializer_list<uint64>& LinearIndexList)
{
    return (*this)(LinearIndexList);
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::SubMatrix(std::initializer_list<uint64>& RowIndexList,
                                  std::initializer_list<uint64>& ColIndexList)
{
    return (*this)(RowIndexList, ColIndexList);
}


template<typename ElementType>
inline 
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::SubMatrix(const std::initializer_list<uint64>& RowIndexList,
                                  const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol)
{
    return (*this)(RowIndexList, ALL_Symbol);
}


template<typename ElementType>
inline mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::SubMatrix(const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol,
                                  const std::initializer_list<uint64>& ColIndexList)
{
    return (*this)(ALL_Symbol, ColIndexList);
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::SubMatrix(const std::vector<uint64>& LinearIndexList)
{
    return (*this)(LinearIndexList);
}


template<typename ElementType>
inline 
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::SubMatrix(const std::vector<uint64>& RowIndexList,
                                  const std::vector<uint64>& ColIndexList)
{
    return (*this)(RowIndexList, ColIndexList);
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::SubMatrix(const std::vector<uint64>& RowIndexList,
                                  const ALL_Symbol_For_mdkMatrix_Operator& All_Symbol)
{
    return (*this)(RowIndexList, All_Symbol);
}


template<typename ElementType>
inline 
mdkShadowMatrix<ElementType> 
mdkMatrix<ElementType>::SubMatrix(const ALL_Symbol_For_mdkMatrix_Operator&,
                                  const std::vector<uint64>& ColIndexList)
{
    return (*this)(All_Symbol, ColIndexList);
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetSubMatrix(uint64 RowIndex_start, uint64 RowIndex_end,
                                                            uint64 ColIndex_start, uint64 ColIndex_end) const
{
    mdkMatrix<ElementType> tempMatrix; // empty matrix

    if (RowIndex_start >= m_RowNumber || RowIndex_end >= m_RowNumber
        || ColIndex_start >= m_ColNumber || ColIndex_end >= m_ColNumber
        || RowIndex_start > RowIndex_end || ColIndex_start > ColIndex_end)
    {
        mdkError << "Invalid input @ mdkMatrix::GetSubMatrix(rs,re,cs,ce)" << '\n';
        
        return tempMatrix;
    }

    auto RowNumber = RowIndex_end - RowIndex_start + 1;
    auto ColNumber = ColIndex_end - ColIndex_start + 1;

    tempMatrix.Resize(RowNumber, ColNumber);

    auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

    auto RawPointer = m_ElementData->data();

    for (uint64 j = ColIndex_start; j <= ColIndex_end; ++j)
    {
        for (uint64 i = RowIndex_start; i <= RowIndex_end; ++i)
        {
            tempRawPointer[0] = RawPointer[j*m_RowNumber + i];

            ++tempRawPointer;
        }
    }

    return tempMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetSubMatrix(const std::vector<uint64>& RowIndexList, const std::vector<uint64>& ColIndexList) const
{
    mdkMatrix<ElementType> tempMatrix; // empty matrix

    auto RowNumber = uint64(RowIndexList.size());
    auto ColNumber = uint64(ColIndexList.size());

    if (RowNumber == 0 || ColNumber == 0)
    {
        mdkWarning << "RowIndexList or ColIndexList is empty @ mdkMatrix::GetSubMatrix(std::vector RowIndexList, ColIndexList)" << '\n';
        
        return tempMatrix;
    }

    for (uint64 j = 0; j < ColNumber; ++j)
    {
        if (ColIndexList[j] >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::GetSubMatrix(std::vector RowIndexList, ColIndexList)" << '\n';
            
            return tempMatrix;
        }
    }

    for (uint64 i = 0; i < RowNumber; ++i)
    {
        if (RowIndexList[i] >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::GetSubMatrix(std::vector RowIndexList, ColIndexList)" << '\n';
            
            return tempMatrix;
        }
    }

    tempMatrix.Resize(RowNumber, ColNumber);

    auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

    auto RawPointer = m_ElementData->data();

    for (uint64 j = 0; j < ColNumber; ++j)
    {
        auto Index = ColIndexList[j] * m_RowNumber;

        for (uint64 i = 0; i < RowNumber; ++i)
        {
            uint64 LinearIndex = Index + RowIndexList[i];

            tempRawPointer[0] = RawPointer[LinearIndex];

            ++tempRawPointer;
        }
    }

    return tempMatrix;
}


// Attention:
// this function is used for mdkShadowMatrix.CreateMatrix(OutputMatrix)
// Try not to use it for other purpose because it does not check OutputMatrix
//
template<typename ElementType>
inline
void mdkMatrix<ElementType>::GetSubMatrix(mdkMatrix<ElementType>& OutputMatrix,
                                          const std::vector<uint64>& RowIndexList, 
                                          const std::vector<uint64>& ColIndexList) const
{
    if (this == &OutputMatrix)
    {
        mdkError << "A Matrix tries to get a SubMatrix of itself @ mdkMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
        return;
    }

    auto RowNumber = uint64(RowIndexList.size());
    
    auto ColNumber = uint64(ColIndexList.size());

    if (RowNumber == 0 || ColNumber == 0)
    {
        mdkWarning << "RowIndexList or ColIndexList is empty @ mdkMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
        return;
    }

    for (uint64 j = 0; j < ColNumber; ++j)
    {
        if (ColIndexList[j] >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
            return;
        }
    }

    for (uint64 i = 0; i < RowNumber; ++i)
    {
        if (RowIndexList[i] >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::GetSubMatrix(OutputMatrix RowIndexList, ColIndexList)" << '\n';
            return;
        }
    }

    if (OutputMatrix.IsEmpty() == true)
    {
        OutputMatrix.Resize(RowNumber, ColNumber);
    }

    auto tempRawPointer = OutputMatrix.GetElementDataRawPointer();

    auto RawPointer = m_ElementData->data();

    for (uint64 j = 0; j < ColNumber; ++j)
    {
        auto Index = ColIndexList[j] * m_RowNumber;

        for (uint64 i = 0; i < RowNumber; ++i)
        {
            uint64 LinearIndex = Index + RowIndexList[i];

            tempRawPointer[0] = RawPointer[LinearIndex];

            ++tempRawPointer;
        }
    }
}


template<typename ElementType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetSubMatrix(const std::vector<uint64>& RowIndexList, const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol) const
{
    std::vector<uint64> ColIndexList(m_ColNumber);

    for (uint64 i = 0; i < m_ColNumber; ++i)
    {
        ColIndexList[i] = i;
    }

    return this->GetSubMatrix(RowIndexList, ColIndexList);
}

template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetSubMatrix(const ALL_Symbol_For_mdkMatrix_Operator& ALL_Symbol, const std::vector<uint64>& ColIndexList) const
{
    std::vector<uint64> RowIndexList(m_RowNumber);

    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        RowIndexList[i] = i;
    }

    return this->GetSubMatrix(RowIndexList, ColIndexList);
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetSubMatrix(const std::vector<uint64>& LinearIndexList) const
{
    mdkMatrix<ElementType> tempMatrix; // empty matrix

    auto ElementNumber_sub = uint64(LinearIndexList.size());

    if (ElementNumber_sub == 0)
    {
        mdkWarning << "Empty LinearIndexList @ mdkMatrix::GetSubMatrix(std::vector LinearIndexList)" << '\n';
        
        return tempMatrix;
    }

    auto ElementNumber = this->GetElementNumber();

    for (uint64 j = 0; j < ElementNumber_sub; ++j)
    {
        if (LinearIndexList[j] >= m_ElementNumber)
        {
            mdkError << "Invalid LinearIndexList @ mdkMatrix::GetSubMatrix(std::vector LinearIndexList)" << '\n';
            
            return tempMatrix;
        }
    }

    tempMatrix.Resize(ElementNumber_sub, 1);

    auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

    auto RawPointer = m_ElementData->data();

    for (uint64 j = 0; j < ElementNumber_sub; ++j)
    {
        tempRawPointer[j] = RawPointer[LinearIndexList[j]];
    }
    
    return tempMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Col(uint64 ColIndex)
{
    if (ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::Col(ColIndex)" << '\n';
        mdkShadowMatrix<ElementType> tempShadowMatrix;        
        return tempShadowMatrix;
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL, {ColIndex});

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Col(std::initializer_list<uint64>& ColIndexList)
{
    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::Col({ColIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Col(const std::vector<uint64>& ColIndexList)
{
    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {        
        if (*it >= m_ColNumber)
        {
            mdkError << "Invalid ColIndexList @ mdkMatrix::Col(std::vector ColIndexList)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;            
            return tempShadowMatrix;
        }
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, ALL, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetCol(uint64 ColIndex) const
{
	mdkMatrix<ElementType> tempMatrix; // empty matrix

	if (ColIndex >= m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::GetCol(ColIndex)" << '\n';
        
        return tempMatrix;
	}

    tempMatrix.Resize(m_RowNumber, 1);

    auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = ColIndex*m_RowNumber;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		tempRawPointer[i] = RawPointer[Index + i];
	}

    return tempMatrix;
}


template<typename ElementType>
inline 
bool mdkMatrix<ElementType>::GetCol(uint64 ColIndex, std::vector<ElementType>& ColData) const
{
	if (ColIndex >= m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::GetCol(uint64 ColIndex, std::vector<ElementType>& ColData)" << '\n';
		return false;
	}

	ColData.resize(m_RowNumber);

	auto tempRawPointer = ColData.data();

	auto RawPointer = m_ElementData->data();

	uint64 Index = ColIndex*m_RowNumber;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		tempRawPointer[i] = RawPointer[Index + i];
	}

	return true;
}


template<typename ElementType>
inline 
bool mdkMatrix<ElementType>::GetCol(uint64 ColIndex, ElementType* ColData) const
{
	if (ColIndex >= m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::GetCol(uint64 ColIndex, ElementType* ColData)" << '\n';
		return false;
	}

	auto RawPointer = m_ElementData->data();

	uint64 Index = ColIndex*m_RowNumber;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		ColData[i] = RawPointer[Index + i];
	}

	return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::SetCol(uint64 ColIndex, const mdkMatrix<ElementType_input>& ColData)
{
	if (ColIndex >= m_ColNumber)
	{
		mdkError << "Invalid input : ColIndex is out of bound @ mdkMatrix::SetCol(ColIndex, mdkMatrix)" << '\n';
		return false;
	}

	auto Size = ColData.GetSize();

	if (Size.ColNumber != 1 || Size.RowNumber != 1)
	{
		mdkError << "Invalid input : must be a vector @ mdkMatrix::SetCol(ColIndex, mdkMatrix)" << '\n';
		return false;
	}

	if (Size.RowNumber > 1 && Size.RowNumber != m_RowNumber)
	{
		mdkError << "Invalid input : size does not match @ mdkMatrix::SetCol(ColIndex, mdkMatrix)" << '\n';
		return false;
	}

	if (Size.ColNumber > 1 && Size.ColNumber != m_RowNumber)
	{
		mdkError << "Invalid input : size does not match @ mdkMatrix::SetCol(ColIndex, mdkMatrix)" << '\n';
		return false;
	}

    return this->SetCol(ColIndex, ColData.GetElementDataRawPointer(), std::max(Size.RowNumber, Size.ColNumber));
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetCol(uint64 ColIndex, const std::initializer_list<ElementType_input>& ColData)
{
    return this->SetCol(ColIndex, ColData.begin(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetCol(uint64 ColIndex, const std::vector<ElementType_input>& ColData)
{
    return this->SetCol(ColIndex, ColData.data(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::SetCol(uint64 ColIndex, const ElementType_input* ColData, uint64 Length)
{
    if (ColData == nullptr || Length == 0 || ColIndex >= m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::SetCol(ColIndex, const ElementType_input* ColData, uint64 Length)" << '\n';
		return false;
	}
    
    auto RawPointer = m_ElementData->data();

    uint64 Index = ColIndex*m_RowNumber;

    // just for reference:
    //
    //for (uint64 i = 0; i < m_RowNumber; ++i)
    //{
    //	RawPointer[Index + i] = ElementType(ColData[i]);
    //}

    auto tempColData = ColData;

    RawPointer += Index;
    for (auto Ptr = RawPointer; Ptr < RawPointer + m_RowNumber; ++Ptr)
    {
        Ptr[0] = ElementType(tempColData[0]);

        ++tempColData;
    }

	return true;
}


template<typename ElementType>
inline 
bool mdkMatrix<ElementType>::FillCol(uint64 ColIndex, const ElementType& Element)
{
    if (ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::FillCol(uint64 ColIndex, const ElementType& Element)" << '\n';
        return false;
    }

    auto RawPointer = m_ElementData->data();

    uint64 Index = ColIndex*m_RowNumber;

    RawPointer += Index;
    for (auto Ptr = RawPointer; Ptr < RawPointer + m_RowNumber; ++Ptr)
    {
        Ptr[0] = Element;
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::AppendCol(const mdkMatrix<ElementType_input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::AppendCol(const mdkMatrix<ElementType_input>& ColData)" << '\n';
        return false;
    }

    if (ColData.IsEmpty() == true)
    {
        mdkError << "Empty input @ mdkMatrix::AppendCol(const mdkMatrix<ElementType_input>& ColData)" << '\n';
        return false;
    }

    auto  RowNumber = m_RowNumber;

    if (RowNumber == 0)
    {
        RowNumber = std::max(ColData.GetRowNumber(), ColData.GetColNumber());
    }

    this->Resize(RowNumber, m_ColNumber + 1);

    return this->SetCol(m_ColNumber - 1, ColData);
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::AppendCol(const std::initializer_list<ElementType_input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::AppendCol(const std::initializer_list<ElementType_input>& ColData)" << '\n';
        return false;
    }

    if (ColData.size() == 0)
    {
        mdkError << "Empty input @ mdkMatrix::AppendCol(const std::initializer_list<ElementType_input>& ColData)" << '\n';
        return false;
    }

    auto  RowNumber = m_RowNumber;

    if (RowNumber == 0)
    {
        RowNumber = ColData.size();
    }

    this->Resize(RowNumber, m_ColNumber + 1);

    return this->SetCol(m_ColNumber - 1, ColData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool mdkMatrix<ElementType>::AppendCol(const std::vector<ElementType_Input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::AppendCol(const std::vector<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    if (ColData.size() == 0)
    {
        mdkError << "Empty input @ mdkMatrix::AppendCol(const std::vector<ElementType_input>& ColData)" << '\n';
        return false;
    }

    auto  RowNumber = m_RowNumber;

    if (RowNumber == 0)
    {
        RowNumber = ColData.size();
    }

    this->Resize(RowNumber, m_ColNumber + 1);

    return this->SetCol(m_ColNumber - 1, ColData);
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::AppendCol(const ElementType_input* ColData, uint64 Length)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::AppendCol(const ElementType_input* ColData, uint64 Length)" << '\n';
        return false;
    }

    if (ColData == nullptr || Length == 0)
    {
        mdkError << "Empty input @ mdkMatrix::AppendCol(const ElementType_input* ColData, uint64 Length)" << '\n';
        return false;
    }

    auto  RowNumber = m_RowNumber;

    if (RowNumber == 0)
    {
        RowNumber = Length;
    }

    this->Resize(RowNumber, m_ColNumber + 1);

    return this->SetCol(m_ColNumber - 1, ColData, Length);
}


template<typename ElementType>
inline 
bool mdkMatrix<ElementType>::DeleteCol(uint64 ColIndex)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::DeleteCol(uint64 ColIndex)" << '\n';
        return false;
    }

    if (ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::DeleteCol(uint64 ColIndex)" << '\n';
        return false;
    }

    m_ElementData->erase(m_ElementData->begin() + ColIndex*m_RowNumber, m_ElementData->begin() + (ColIndex+1)*m_RowNumber);

    m_ColNumber -= 1;

    m_ElementNumber = m_RowNumber*m_ColNumber;

    return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::DeleteCol(std::initializer_list<uint64>& ColIndexList)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::DeleteCol(std::initializer_list<uint64>& ColIndexList)" << '\n';
        return false;
    }

    if (ColIndexList.size() == 0)
    {
        mdkError << "Empty input @ mdkMatrix::DeleteCol(std::initializer_list ColIndexList)" << '\n';
        return false;
    }

    for (uint64 i = 0; i < ColIndexList.size(); ++i)
    {
        if (ColIndexList[i] >= m_ColNumber)
        {
            mdkError << "Out of bound input @ mdkMatrix::DeleteCol(std::initializer_list ColIndexList)" << '\n';
            return false;
        }
    }

    return this->DeleteCol(ColIndexList.begin(), ColIndexList.size());
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::DeleteCol(const std::vector<uint64>& ColIndexList)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::DeleteCol(const std::vector<uint64>& ColIndexList)" << '\n';
        return false;
    }

    if (ColIndexList.size() == 0)
    {
        mdkError << "Empty input @ mdkMatrix::DeleteCol(std::vector ColIndexList)" << '\n';
        return false;
    }

    for (uint64 i = 0; i < ColIndexList.size(); ++i)
    {
        if (ColIndexList[i] >= m_ColNumber)
        {
            mdkError << "Out of bound input @ mdkMatrix::DeleteCol(std::vector ColIndexList)" << '\n';
            return false;
        }
    }

    return this->DeleteCol(ColIndexList.data(), ColIndexList.size());
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::DeleteCol(const uint64* ColIndexPtr, uint64 Length)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Matrix Size can not change @ mdkMatrix::DeleteCol(const uint64* ColIndexPtr, uint64 Length)" << '\n';
        return false;
    }

    if (ColIndexPtr == nullptr || Length == 0)
    {
        mdkError << "Empty input @ mdkMatrix::DeleteCol(const uint64* ColIndexPtr, uint64 Length)" << '\n';
        return false;
    }

    if (Length == 1)
    {
        return this->DeleteCol(ColIndexPtr[0]);
    }

    // Length > 1 -------------------------------------------

    std::vector<uint64> ColIndexList_max_to_min(Length);

    for (uint64 i = 0; i < Length; ++i)
    {
        ColIndexList_max_to_min[i] = ColIndexPtr[i];
    }

    std::sort(ColIndexList_max_to_min.begin(), ColIndexList_max_to_min.end(), [](uint64 a, uint64 b) { return a > b; });

    uint64 Index_prev = ColIndexList_max_to_min[0] + 1;

    for (uint64 i = 0; i < ColIndexList_max_to_min.size(); ++i)
    {
        auto Index_i = ColIndexList_max_to_min[i];

        if (Index_i == Index_prev)
        {
            mdkWarning << "duplicate input @ mdkMatrix::DeleteCol(const uint64* ColIndexPtr, uint64 Length)" << '\n';
        }
        else
        {
            m_ElementData->erase(m_ElementData->begin() + Index_i * m_RowNumber,
                                 m_ElementData->begin() + (Index_i + 1)* m_RowNumber);

            Index_prev = Index_i;

            m_ColNumber -= 1;
        }
    }

    m_ElementNumber = m_RowNumber*m_ColNumber;

    return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertCol(uint64 ColIndex, const mdkMatrix<ElementType_input>& ColData)
{
    if (ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input : ColIndex is out of bound @ mdkMatrix::InsertCol(ColIndex, mdkMatrix)" << '\n';
        return false;
    }

    auto Size = ColData.GetSize();

    if (Size.ColNumber != 1 || Size.RowNumber != 1)
    {
        mdkError << "Invalid input : must be a vector @ mdkMatrix::InsertCol(ColIndex, mdkMatrix)" << '\n';
        return false;
    }

    if (Size.RowNumber > 1 && Size.RowNumber != m_RowNumber)
    {
        mdkError << "Invalid input : size does not match @ mdkMatrix::InsertCol(ColIndex, mdkMatrix)" << '\n';
        return false;
    }

    if (Size.ColNumber > 1 && Size.ColNumber != m_RowNumber)
    {
        mdkError << "Invalid input : size does not match @ mdkMatrix::InsertCol(ColIndex, mdkMatrix)" << '\n';
        return false;
    }

    return this->InsertCol(ColIndex, ColData.GetElementDataRawPointer(), std::max(Size.RowNumber, Size.ColNumber));

}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertCol(uint64 ColIndex, const std::initializer_list<ElementType_input>& ColData)
{
    return this->InsertCol(ColIndex, ColData.begin(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertCol(uint64 ColIndex, const std::vector<ElementType_input>& ColData)
{
    return this->InsertCol(ColIndex, ColData.data(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertCol(uint64 ColIndex, const ElementType_input* ColData, uint64 Length)
{
    if (ColData == nullptr || Length == 0 || ColIndex >= m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::InsertCol(ColIndex, const ElementType_input* ColData, uint64 Length)" << '\n';
        return false;
    }

    m_ElementData->insert(m_ElementData->begin() + ColIndex*m_RowNumber, ColData, ColData + Length);

    m_ColNumber += 1;

    m_ElementNumber = m_RowNumber*m_ColNumber;

    return true;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Row(uint64 RowIndex)
{
    if (RowIndex >= m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::Row(RowIndex)" << '\n';
        mdkShadowMatrix<ElementType> tempShadowMatrix;        
        return tempShadowMatrix;
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, {RowIndex}, ALL);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Row(std::initializer_list<uint64>& RowIndexList)
{
    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::Row({RowIndexList})" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Row(const std::vector<uint64>& RowIndexList)
{
    auto RowNumber = uint64(RowIndexList.size());

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= m_RowNumber)
        {
            mdkError << "Invalid RowIndexList @ mdkMatrix::Row(std::vector RowIndexList)" << '\n';
            mdkShadowMatrix<ElementType> tempShadowMatrix;            
            return tempShadowMatrix;
        }
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetRow(uint64 RowIndex) const
{
    mdkMatrix<ElementType> tempMatrix;	

	if (RowIndex >= m_RowNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::GetRow(RowIndex)" << '\n';
        
        return tempMatrix;
	}

    tempMatrix.Resize(1, m_ColNumber);

    auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		tempRawPointer[j] = RawPointer[Index + RowIndex];

		Index += m_RowNumber;
	}

    return tempMatrix;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::GetRow(uint64 RowIndex, std::vector<ElementType>& RowData) const
{
	if (RowIndex >= m_RowNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::GetRow(RowIndex, std::vector<ElementType>& RowData)" << '\n';
		return false;
	}

	RowData.resize(m_ColNumber);

	auto tempRawPointer = RowData.data();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		tempRawPointer[j] = RawPointer[Index + RowIndex];

		Index += m_RowNumber;
	}

	return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::GetRow(uint64 RowIndex, ElementType* RowData) const
{
	if (RowIndex >= m_RowNumber)
	{
		mdkError << "Invalid input @ mdkMatrix GetRow(RowIndex, ElementType* RowData)" << '\n';
		return false;
	}

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		RowData[j] = RawPointer[Index + RowIndex];

		Index += m_RowNumber;
	}

	return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::SetRow(uint64 RowIndex, const mdkMatrix<ElementType_input>& RowData)
{
    if (RowIndex >= m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix SetRow(uint64 RowIndex, const mdkMatrix<ElementType_input>& RowData)" << '\n';
        return false;
    }

	auto Size = RowData.GetSize();

	if (Size.ColNumber != 1 && Size.RowNumber != 1)
	{
		mdkError << "Invalid input @ mdkMatrix SetRow(RowIndex,mdkMatrix)" << '\n';
		return false;
	}

	if (Size.RowNumber > 1 && Size.RowNumber != m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix SetRow(RowIndex,mdkMatrix)" << '\n';
		return false;
	}

	if (Size.ColNumber > 1 && Size.ColNumber != m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix SetRow(RowIndex,mdkMatrix)" << '\n';
		return false;
	}

    return this->SetRow(RowIndex, RowData.GetElementDataRawPointer(), std::max(Size.RowNumber, Size.ColNumber));    
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetRow(uint64 RowIndex, const std::initializer_list<ElementType_input>& RowData)
{
    return this->SetRow(RowIndex, RowData.begin(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetRow(uint64 RowIndex, const std::vector<ElementType_input>& RowData)
{
    return this->SetRow(RowIndex, RowData.data(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetRow(uint64 RowIndex, const ElementType_input* RowData, uint64 Length)
{
    if (RowData == nullptr || Length == 0 || RowIndex >= m_RowNumber)
    {
        mdkError << "Empty input @ mdkMatrix::SetRow(RowIndex, const ElementType_input* RowData, uint64 Length)" << '\n';
        return false;
    }

    if (Length != m_ColNumber)
    {
        mdkError << "Length does not match @ mdkMatrix::SetRow(RowIndex, const ElementType_input* RowData, uint64 Length)" << '\n';
        return false;
    }

    auto RawPointer = m_ElementData->data();

    uint64 Index = 0;

    for (uint64 j = 0; j < m_ColNumber; ++j)
    {
        RawPointer[Index + RowIndex] = ElementType(RowData[j]);

        Index += m_RowNumber;
     }

    return true;
}


template<typename ElementType>
inline 
bool mdkMatrix<ElementType>::FillRow(uint64 RowIndex, const ElementType& Element)
{
    if (RowIndex >= m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::FillRow(uint64 RowIndex, const ElementType& Element)" << '\n';
        return false;
    }

    auto RawPointer = m_ElementData->data();

    uint64 Index = 0;

    for (uint64 j = 0; j < m_ColNumber; ++j)
    {
        RawPointer[Index + RowIndex] = Element;

        Index += m_RowNumber;
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::AppendRow(const mdkMatrix<ElementType_input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::AppendRow(const mdkMatrix<ElementType_input>& RowData)" << '\n';
        return false;
    }

    if (RowData.IsEmpty() == true)
    {
        mdkError << "Input is empty @ mdkMatrix::AppendRow(const mdkMatrix<ElementType_input>& RowData)" << '\n';
        return false;
    }

    auto  ColNumber = m_ColNumber;

    if (ColNumber == 0)
    {
        ColNumber = std::max(RowData.GetRowNumber(), RowData.GetColNumber);
    }

    this->Resize(m_RowNumber + 1, ColNumber);

    return this->SetRow(m_RowNumber - 1, RowData);
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool  mdkMatrix<ElementType>::AppendRow(const std::initializer_list<ElementType_input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::AppendRow(const std::initializer_list<ElementType_input>& RowData)" << '\n';
        return false;
    }

    if (RowData.size() == 0)
    {
        mdkError << "Input is empty @ mdkMatrix::AppendRow(const std::initializer_list<ElementType_input>& RowData)" << '\n';
        return false;
    }

    auto  ColNumber = m_ColNumber;

    if (ColNumber == 0)
    {
        ColNumber = RowData.size();
    }

    this->Resize(m_RowNumber + 1, ColNumber);

    return this->SetRow(m_RowNumber - 1, RowData);
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool  mdkMatrix<ElementType>::AppendRow(const std::vector<ElementType_input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::AppendRow(const std::vector<ElementType_input>& RowData)" << '\n';
        return false;
    }

    if (RowData.size() == 0)
    {
        mdkError << "Input is empty @ mdkMatrix::AppendRow(const std::vector<ElementType_input>& RowData)" << '\n';
        return false;
    }

    auto  ColNumber = m_ColNumber;

    if (ColNumber == 0)
    {
        ColNumber = RowData.size();
    }

    this->Resize(m_RowNumber + 1, ColNumber);

    return this->SetRow(m_RowNumber - 1, RowData);
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::AppendRow(const ElementType_input* RowData, uint64 Length)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::AppendRow(const ElementType_input* RowData, uint64 Length)" << '\n';
        return false;
    }

    if (RowData == nullptr || Length == 0)
    {
        mdkError << "Input is empty @ mdkMatrix::AppendRow(const ElementType_input* RowData, uint64 Length)" << '\n';
        return false;
    }

    auto  ColNumber = m_ColNumber;

    if (ColNumber == 0)
    {
        ColNumber = RowData.size();
    }

    this->Resize(m_RowNumber + 1, ColNumber);

    return this->SetRow(m_RowNumber - 1, RowData, Length);
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::DeleteRow(uint64 RowIndex)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::DeleteRow(uint64 RowIndex)" << '\n';
        return false;
    }

    if (RowIndex >= m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::DeleteRow(uint64 RowIndex)" << '\n';
        return false;
    }

    return this->DeleteRow(&RowIndex, 1);
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::DeleteRow(const std::vector<uint64>& RowIndexList)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::DeleteRow(const std::vector<uint64>& RowIndexList)" << '\n';
        return false;
    }

    if (RowIndexList.size() == 0 || RowIndexList.size() > m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::DeleteRow(const std::vector<uint64>& RowIndexList)" << '\n';
        return false;
    }

    return this->DeleteRow(RowIndexList.data(), RowIndexList.size());
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::DeleteRow(const uint64* RowIndexPtr, uint64 Length)
{
    if (m_IsSizeFixed == true)
    {
        mdkError << "Size can not change @ mdkMatrix::DeleteRow(const uint64* RowIndexPtr, uint64 Length)" << '\n';
        return false;
    }

    if (RowIndexPtr == nullptr || Length == 0)
    {
        mdkError << "Invalid input @ mdkMatrix::DeleteRow(const uint64* RowIndexPtr, uint64 Length)" << '\n';
        return false;
    }

    std::vector<uint64> CounterList(m_RowNumber);
    
    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        CounterList[i] = 0;
    }

    for (uint64 i = 0; i < Length; ++i)
    {
        CounterList[RowIndexPtr[i]] += 1;
    }

    std::vector<uint64> RowIndexList_output;

    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        if (CounterList[i] == 0)
        {
            RowIndexList_output.push_back(i);
        }
    }

    if (RowIndexList_output.size() > 0)
    {
        this->Eat(this->GetSubMatrix(RowIndexList_output, ALL));
    }
    else
    {
        this->Reset();
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertRow(uint64 RowIndex, const mdkMatrix<ElementType_input>& RowData)
{
    if (RowIndex >= m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix InsertRow(uint64 RowIndex, const mdkMatrix<ElementType_input>& RowData)" << '\n';
        return false;
    }

    auto Size = RowData.GetSize();

    if (Size.ColNumber != 1 && Size.RowNumber != 1)
    {
        mdkError << "Invalid input @ mdkMatrix InsertRow(RowIndex,mdkMatrix)" << '\n';
        return false;
    }

    if (Size.RowNumber > 1 && Size.RowNumber != m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix InsertRow(RowIndex,mdkMatrix)" << '\n';
        return false;
    }

    if (Size.ColNumber > 1 && Size.ColNumber != m_ColNumber)
    {
        mdkError << "Invalid input @ mdkMatrix InsertRow(RowIndex,mdkMatrix)" << '\n';
        return false;
    }

    return this->InsertRow(RowIndex, RowData.GetElementDataRawPointer(), std::max(Size.RowNumber, Size.ColNumber));
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertRow(uint64 RowIndex, const std::initializer_list<ElementType_input>& RowData)
{
    return this->InsertRow(RowIndex, RowData.begin(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertRow(uint64 RowIndex, const std::vector<ElementType_input>& RowData)
{
    return this->InsertRow(RowIndex, RowData.data(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::InsertRow(uint64 RowIndex, const ElementType_input* RowData, uint64 Length)
{
    if (RowData == nullptr || Length == 0 || RowIndex >= m_RowNumber)
    {
        mdkError << "Empty input @ mdkMatrix::InsertRow(RowIndex, const ElementType_input* RowData, uint64 Length)" << '\n';
        return false;
    }

    if (Length != m_ColNumber)
    {
        mdkError << "Length does not match @ mdkMatrix::InsertRow(RowIndex, const ElementType_input* RowData, uint64 Length)" << '\n';
        return false;
    }

    auto tempElementData = std::make_shared<std::vector<ElementType>>((m_RowNumber+1)*m_ColNumber);

    auto tempRawPointer = tempElementData->data();

    auto RawPointer = m_ElementData->data();

    for (uint64 i = 0; i < RowIndex; ++i)
    {
        for (uint64 j = 0; j < m_ColNumber; ++j)
        {
            tempRawPointer[j*(m_RowNumber + 1) + i] = RawPointer[j*m_RowNumber + i];
        }
    }

    for (uint64 j = 0; j < m_ColNumber; ++j)
    {
        tempRawPointer[j*(m_RowNumber + 1) + RowIndex] = ElementType(RowData[j]);
    }

    for (uint64 i = RowIndex + 1; i < m_RowNumber + 1; ++i)
    {
        for (uint64 j = 0; j < m_ColNumber; ++j)
        {
            tempRawPointer[j*(m_RowNumber + 1) + i] = RawPointer[j*m_RowNumber + i - 1];
        }
    }

    m_RowNumber += 1;

    m_ElementNumber = m_RowNumber*m_ColNumber;

    m_ElementData = tempElementData;

    return true;
}


template<typename ElementType>
inline
mdkShadowMatrix<ElementType>
mdkMatrix<ElementType>::Diangonal()
{
    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << " Self is empty or not square @ mdkMatrix::Diangonal" << '\n';
        mdkShadowMatrix<ElementType> tempShadowMatrix;     
        return  tempShadowMatrix;
    }

    std::vector<uint64> LinearIndexList(m_ColNumber);

    uint64 Index = 0;

    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        LinearIndexList[j] = Index + i;

        Index += m_RowNumber;
    }

    mdkShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::GetDiangonal() const
{
    mdkMatrix<ElementType> tempMatrix; 

    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << " Self is empty or not square @ mdkMatrix::GetDiangonal()" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(m_RowNumber, 1);

    auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

    this->GetDiangonal(tempRawPointer);

    return tempMatrix;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::GetDiangonal(std::vector<ElementType>& DiangonalData) const
{
    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << " Self is empty or not square @ mdkMatrix::GetDiangonal(std::vector)" << '\n';
        return  false;
    }

    DiangonalData.resize(m_RowNumber);

    auto outputRawPointer = DiangonalData.data();

    return this->GetDiangonal(outputRawPointer);
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::GetDiangonal(ElementType* DiangonalData) const
{
    if (DiangonalData == nullptr)
    {
        mdkError << " input is nullptr @ mdkMatrix::GetDiangonal(ElementType* DiangonalData)" << '\n';
        return  false;
    }

    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << " Self is empty or not square @ mdkMatrix::GetDiangonal(ElementType* DiangonalData)" << '\n';
        return  false;
    }

    auto RawPointer = m_ElementData->data();

    uint64 Index = 0;

    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        DiangonalData[j] = RawPointer[Index + i];

        Index += m_RowNumber;
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_input>
inline 
bool mdkMatrix<ElementType>::SetDiangonal(const mdkMatrix<ElementType_input>& DiangonalData)
{
	auto Size = DiangonalData.GetSize();

	if (Size.ColNumber != 1 && Size.RowNumber != 1)
	{
		mdkError << "Invalid input @ mdkMatrix::SetDiangonal(Matrix)" << '\n';
		return false;
	}

	if (Size.RowNumber > 1 && Size.RowNumber != m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::SetDiangonal(Matrix)" << '\n';
		return false;
	}

	if (Size.ColNumber > 1 && Size.ColNumber != m_ColNumber)
	{
		mdkError << "Invalid input @ mdkMatrix::SetDiangonal(Matrix)" << '\n';
		return false;
	}

    if (Size.RowNumber == 1 && Size.ColNumber == 1)
    {
        mdkWarning << "Input matrix is 1x1 and treated as a scalar @ mdkMatrix::SetDiangonal(Matrix)" << '\n';
        return this->FillDiangonal(ElementType(DiangonalData(0)));
    }
    else
    {
        return this->SetDiangonal(DiangonalData.GetElementDataRawPointer());
    }
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetDiangonal(const std::initializer_list<ElementType_input>& DiangonalData)
{
    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << "Self is empty or not square @ mdkMatrix::SetDiangonal({DiangonalData})" << '\n';
        return false;
    }

    if (DiangonalData.size() != m_RowNumber)
    {
        mdkError << "Invalid input @ mdkMatrix::SetDiangonal({DiangonalData})" << '\n';
        return false;
    }

    return this->SetDiangonal(DiangonalData.begin());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetDiangonal(const std::vector<ElementType_input>& DiangonalData)
{
   	if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
	{
		mdkError << " Self is empty or not square @ mdkMatrix::SetDiangonal(std::vector)" << '\n';
		return false;
	}

    if (DiangonalData.size() != m_RowNumber)
    {
        mdkError << " Input is invalid @ mdkMatrix::SetDiangonal(std::vector)" << '\n';
        return false;
    }

	return this->SetDiangonal(DiangonalData.data());
}


template<typename ElementType>
template<typename ElementType_input>
inline
bool mdkMatrix<ElementType>::SetDiangonal(const ElementType_input* DiangonalData)
{
    if (DiangonalData == nullptr)
    {
        mdkError << " Input is nullptr @ mdkMatrix::SetDiangonal(const ElementType_input* DiangonalData)" << '\n';
        return false;
    }

    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << " Self is empty or not square @ mdkMatrix::SetDiangonal(const ElementType_input* DiangonalData)" << '\n';
        return false;
    }

    auto RawPointer = m_ElementData->data();

    uint64 Index = 0;

    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        RawPointer[Index + i] = ElementType(DiangonalData[i]);

        Index += m_RowNumber;
    }

    return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::FillDiangonal(const ElementType& Element)
{
    if (m_ElementNumber == 0 || m_RowNumber != m_ColNumber)
    {
        mdkError << " Self is empty or not square @ mdkMatrix::FillDiangonal" << '\n';
        return false;
    }

    auto RawPointer = m_ElementData->data();

    uint64 Index = 0;

    for (uint64 i = 0; i < m_RowNumber; ++i)
    {
        RawPointer[Index + i] = Element;

        Index += m_RowNumber;
    }

    return true;
}


// ----------------------- Matrix {+ - * % /}  Matrix ------------------------------------------------//

//===========================================================================================================================//
#if !defined MDK_Enable_GlueMatrix  //-----------------------------------------------------------------------

template<typename ElementType>
inline	
mdkMatrix<ElementType> operator+(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkError << "MatrixA or MatrixB is empty @ mdkMatrix operator+(MatrixA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    if (SizeA.RowNumber != SizeB.RowNumber || SizeA.ColNumber != SizeB.ColNumber)
    {
        mdkError << "Size does not match @ mdkMatrix operator+(MatrixA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrA = MatrixA.GetElementDataRawPointer();

    auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeA.RowNumber*SizeA.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ptrA[i] + ptrB[i];
    }

	return  tempMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> operator-(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

	auto SizeA = MatrixA.GetSize();

	auto SizeB = MatrixB.GetSize();

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkError << "MatrixA or MatrixB is empty @ mdkMatrix operator-(MatrixA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

	if (SizeA.ColNumber == 1 && SizeA.RowNumber == 1)
	{
        tempMatrix.share(MatrixA(0) - MatrixB);
        
        return  tempMatrix;
	}

	if (SizeB.ColNumber == 1 && SizeB.RowNumber == 1)
	{
        tempMatrix.share(MatrixA - MatrixB(0));
        
        return  tempMatrix;
	}

	if (SizeA.RowNumber != SizeB.RowNumber || SizeA.ColNumber != SizeB.ColNumber)
	{
		mdkError << "Size does not match @ mdkMatrix operator-(MatrixA, MatrixB)" << '\n';
        
		return  tempMatrix;
	}

	tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

	auto ptrTemp = tempMatrix.GetElementDataRawPointer();

	auto ptrA = MatrixA.GetElementDataRawPointer();

	auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeA.RowNumber*SizeA.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
	{
        ptrTemp[i] = ptrA[i] - ptrB[i];
	}

	return  tempMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> operator*(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;
    
    auto SizeA = MatrixA.GetSize();

    auto SizeB = MatrixB.GetSize();

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkError << "MatrixA or MatrixB is empty @ mdkMatrix operator*(MatrixA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    if (SizeA.ColNumber != SizeB.RowNumber)
    {
        mdkError << "Size does not match @ mdkMatrix operator*(MatrixA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeA.RowNumber, SizeB.ColNumber);

    auto ptrA = MatrixA.GetElementDataSharedPointer()->data();

    auto ptrB = MatrixB.GetElementDataSharedPointer()->data();

    auto ptrTemp = tempMatrix.GetElementDataSharedPointer()->data();

    //--------------------- lapack ------------------------------------------------

    arma::Mat<double> A(ptrA, arma::uword(MatrixA.GetRowNumber()), arma::uword(MatrixA.GetColNumber()), false);
    arma::Mat<double> B(ptrB, arma::uword(MatrixB.GetRowNumber()), arma::uword(MatrixB.GetColNumber()), false);
    arma::Mat<double> C(ptrTemp, arma::uword(tempMatrix.GetRowNumber()), arma::uword(tempMatrix.GetColNumber()), false);

    C = A*B;

    return tempMatrix;

    //--------------------- for-loop : slow ----------------------------------------------------
    uint64 IndexA = 0;

    uint64 IndexB = 0;

    ElementType ZeroElement = MatrixA(0);

    ZeroElement = ZeroElement - ZeroElement;

    for (uint64 j = 0; j < SizeB.ColNumber; ++j)
    {
        for (uint64 i = 0; i < SizeA.RowNumber; ++i)
        {
            auto sum = ZeroElement;

            IndexA = 0;

            for (uint64 k = 0; k < SizeA.ColNumber; ++k)
            {
                //sum += MatrixA(i,k) * MatrixB(k,j);
                //sum += ptrA[k*SizeA.RowNumber + i] * ptrB[j*SizeB.RowNumber + k];
                sum += ptrA[IndexA + i] * ptrB[IndexB + k];

                IndexA += SizeA.RowNumber;
            }

            ptrTemp[0] = sum;

            ++ptrTemp;
        }

        IndexB += SizeB.RowNumber;
    }

    return tempMatrix;   
}


template<typename ElementType>
inline
mdkMatrix<ElementType> operator/(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

	auto SizeA = MatrixA.GetSize();

	auto SizeB = MatrixB.GetSize();

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkWarning << "MatrixA or MatrixB is empty @ mdkMatrix operator/(MatrixA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    if (SizeA.ColNumber == 1 && SizeA.RowNumber == 1)
    {
        tempMatrix.Share(MatrixB.ElementOperation("/", MatrixA(0)));

        
        return tempMatrix;
    }

    if (SizeB.ColNumber == 1 && SizeB.RowNumber == 1)
    {
        tempMatrix.share(MatrixA.ElementOperation("/", MatrixB(0)));

        
        return tempMatrix;
    }

	if (SizeA.RowNumber != SizeB.RowNumber || SizeA.ColNumber != SizeB.ColNumber)
	{
		mdkError << "Size does not match @ mdkMatrix operator/(MatrixA, MatrixB)" << '\n';
        
		return  tempMatrix;
	}

	tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

	auto ptrTemp = tempMatrix.GetElementDataRawPointer();

	auto ptrA = MatrixA.GetElementDataRawPointer();

	auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeA.RowNumber*SizeA.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
	{
        ptrTemp[i] = ptrA[i] / ptrB[i];
	}
    
	return  tempMatrix;
}
#endif // #if !defined MDK_Enable_GlueMatrix //---------------------------------------------------------------
//===========================================================================================================================//


// ----------------------- Matrix {+ - * /}  Element ------------------------------------------------//

//===========================================================================================================================//
#if !defined MDK_Enable_GlueMatrix  //-----------------------------------------------------------------------

template<typename ElementType>
inline mdkMatrix<ElementType> operator+(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeA = MatrixA.GetSize();

    if (SizeA.RowNumber == 0)
    {
        mdkError << "MatrixA is empty @ mdkMatrix operator+(MatrixA, ElementB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrA = MatrixA.GetElementDataRawPointer();

    auto ElementNumber = Size.RowNumber*Size.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ptrA[i] + ElementB;
    }

    return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> operator-(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
	mdkMatrix<ElementType> tempMatrix;

	auto SizeA = MatrixA.GetSize();

	if (SizeA.RowNumber == 0)
	{
		mdkError << "MatrixA is empty @ mdkMatrix operator-(MatrixA, ElementB)" << '\n';
        
		return  tempMatrix;
	}

	tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

	auto ptrTemp = tempMatrix.GetElementDataRawPointer();

	auto ptrA = MatrixA.GetElementDataRawPointer();

    auto ElementNumber = Size.RowNumber*Size.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
	{
        ptrTemp[i] = ptrA[i] - ElementB;
	}

	return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> operator*(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeA = MatrixA.GetSize();

    if (SizeA.RowNumber == 0)
    {
        mdkError << "MatrixA is empty @ mdkMatrix operator*(MatrixA, ElementB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrA = MatrixA.GetElementDataRawPointer();

    auto ElementNumber = Size.RowNumber*Size.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ptrA[i] * ElementB;
    }
    
    return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> operator/(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeA = MatrixA.GetSize();

    if (SizeA.RowNumber == 0)
    {
        mdkError << "MatrixA is empty @ mdkMatrix operator/(MatrixA, ElementB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeA.RowNumber, SizeA.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrA = MatrixA.GetElementDataRawPointer();

    auto ElementNumber = Size.RowNumber*Size.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ptrA[i] / ElementB;
    }
    
    return tempMatrix;
}

#endif //defined MDK_Enable_GlueMatrix -----------------------------------------------------------------------
//===========================================================================================================================//


// ----------------------- Element {+ - * /} Matrix ------------------------------------------------//

//===========================================================================================================================//
#if !defined MDK_Enable_GlueMatrix  //-----------------------------------------------------------------------

template<typename ElementType>
inline mdkMatrix<ElementType> operator+(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeB = MatrixB.GetSize();

    if (SizeB.RowNumber == 0)
    {
        mdkError << "MatrixB is empty @ mdkMatrix operator+(ElementA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeB.RowNumber, SizeB.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeB.RowNumber*SizeB.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ElementA + ptrB[i];
    }

    return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> operator-(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeB = MatrixB.GetSize();

    if (SizeB.RowNumber == 0)
    {
        mdkError << "MatrixB is empty @ mdkMatrix operator-(ElementA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeB.RowNumber, SizeB.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeB.RowNumber*SizeB.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ElementA - ptrB[i];
    }

    return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> operator*(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeB = MatrixB.GetSize();

    if (SizeB.RowNumber == 0)
    {
        mdkError << "MatrixB is empty @ mdkMatrix operator*(ElementA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SizeB.RowNumber, SizeB.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeB.RowNumber*SizeB.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ElementA * ptrB[i];
    }

    return tempMatrix;
}

#endif //defined MDK_Enable_GlueMatrix -----------------------------------------------------------------------
//===========================================================================================================================//


template<typename ElementType>
inline mdkMatrix<ElementType> operator/(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeB = MatrixB.GetSize();

    if (SizeB.RowNumber == 0)
    {
        mdkError << "MatrixB is empty @ mdkMatrix operator/(ElementA, MatrixB)" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(Size.RowNumber, Size.ColNumber);

    auto ptrTemp = tempMatrix.GetElementDataRawPointer();

    auto ptrB = MatrixB.GetElementDataRawPointer();

    auto ElementNumber = SizeB.RowNumber*SizeB.ColNumber;

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        ptrTemp[i] = ElementA / ptrB[i];
    }

    return tempMatrix;
}


//---------------------- Matrix {+= -= *= /= %=} Matrix ----------------------------------------//

template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator+=(const mdkMatrix<ElementType>& targetMatrix)
{
    auto Size = targetMatrix.GetSize();

    if (Size.RowNumber == 0)
    {
        mdkError << "targetMatrix is empty @ mdkMatrix::operator+=(Matrix)" << '\n';
        return;
    }

    if (Size.RowNumber == 1 && Size.ColNumber == 1)
	{
		(*this) += targetMatrix(0);

		return;
	}

    if (Size.RowNumber != m_RowNumber || Size.ColNumber != m_ColNumber)
	{
		mdkError << "Size does not match @ mdkMatrix::operator+=(Matrix)" << '\n';
		return;
	}

	auto RawPointer = m_ElementData->data();

	auto targetRawPointer = targetMatrix.GetElementDataRawPointer();

	for (uint64 i = 0; i < m_ElementNumber; ++i)
	{
		RawPointer[i] += targetRawPointer[i];
	}
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator-=(const mdkMatrix<ElementType>& targetMatrix)
{
    auto Size = targetMatrix.GetSize();

    if (Size.RowNumber == 0)
    {
        mdkWarning << "targetMatrix is empty @ mdkMatrix::operator-=(Matrix)" << '\n';
        return;
    }

    if (Size.RowNumber == 1 && Size.ColNumber == 1)
	{
		(*this) -= targetMatrix(0);

		return;
	}

    if (Size.RowNumber != m_RowNumber || Size.ColNumber != m_ColNumber)
	{
		mdkError << "Size does not match @ mdkMatrix::operator-=(Matrix)" << '\n';
		return;
	}

	auto RawPointer = m_ElementData->data();

	auto targetRawPointer = targetMatrix.GetElementDataRawPointer();

	for (uint64 i = 0; i < m_ElementNumber; ++i)
	{
		RawPointer[i] -= targetRawPointer[i];
	}
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator*=(const mdkMatrix<ElementType>& targetMatrix)
{
	auto Size = targetMatrix.GetSize();

    if (Size.RowNumber == 0)
	{
        mdkError << "targetMatrix is empty @ mdkMatrix::operator*=(Matrix)" << '\n';
		return;
	}

    if (Size.RowNumber == 1 && Size.ColNumber == 1)
    {
        (*this) *= targetMatrix(0);

        return;
    }

    if (m_IsSizeFixed == true)
    {
        if (Size.RowNumber != m_ColNumber || Size.ColNumber != m_ColNumber)
        {
            mdkError << "Matrix Size can not change @ mdkMatrix::operator*=(Matrix)" << '\n';
            return;
        }
    }

    this->Eat((*this) * targetMatrix);
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator/=(const mdkMatrix<ElementType>& targetMatrix)
{
    auto Size = targetMatrix.GetSize();

    if (Size.RowNumber == 0)
    {
        mdkError << "targetMatrix is empty @ mdkMatrix::operator/=(Matrix)" << '\n';
        return;
    }

    if (Size.RowNumber == 1 && Size.ColNumber == 1)
	{
		(*this) /= targetMatrix(0);

		return;
	}

    if (Size.RowNumber != m_RowNumber || Size.ColNumber != m_ColNumber)
	{
		mdkError << "Size does not match @ mdkMatrix::operator/=(Matrix)" << '\n';
		return;
	}

	auto RawPointer = m_ElementData->data();

	auto targetRawPointer = targetMatrix.GetElementDataRawPointer();

	for (uint64 i = 0; i < m_ElementNumber; ++i)
	{
		RawPointer[i] /= targetRawPointer[i];
	}
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::operator+=(mdkLinearCombineGlueMatrix<ElementType>& GlueMatrix)
{
    (*this) = (*this) + GlueMatrix;
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::operator-=(mdkLinearCombineGlueMatrix<ElementType>& GlueMatrix)
{
    (*this) = (*this) - GlueMatrix;
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::operator*=(mdkLinearCombineGlueMatrix<ElementType>& GlueMatrix)
{
    this->operator*=(GlueMatrix.CreateMatrix());
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::operator/=(mdkLinearCombineGlueMatrix<ElementType>& GlueMatrix)
{
    this->operator/=(GlueMatrix.CreateMatrix());
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::operator+=(const mdkShadowMatrix<ElementType>& ShadowMatrix)
{
    this->operator+=(mdkShadowMatrix.CreateMatrix());
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator-=(const mdkShadowMatrix<ElementType>& ShadowMatrix)
{
    this->operator-=(mdkShadowMatrix.CreateMatrix());
}


template<typename ElementType>
inline 
void mdkMatrix<ElementType>::operator*=(const mdkShadowMatrix<ElementType>& ShadowMatrix)
{
    this->operator*=(mdkShadowMatrix.CreateMatrix());
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator/=(const mdkShadowMatrix<ElementType>& ShadowMatrix)
{
    this->operator/=(mdkShadowMatrix.CreateMatrix());
}

//---------------------- Matrix {+= -= *= /=} Element ----------------------------------------//

template<typename ElementType>
inline	
void mdkMatrix<ElementType>::operator+=(ElementType Element)
{
	if (m_ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::operator+=(Element)" << '\n';
		return;
	}

	auto BeginPointer = m_ElementData->data();

	for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementNumber; ++Ptr)
	{
		Ptr[0] += Element;
	}
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator-=(ElementType Element)
{
    auto ElementNumber = m_RowNumber*m_ColNumber;

	if (ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::operator-=(Element)" << '\n';
		return;
	}

	auto BeginPointer = m_ElementData->data();

	for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementNumber; ++Ptr)
	{
		Ptr[0] -= Element;
	}
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator*=(ElementType Element)
{
	if (m_ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::operator*=(Element)" << '\n';
		return;
	}

	auto BeginPointer = m_ElementData->data();

	for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementNumber; ++Ptr)
	{
		Ptr[0] *= Element;
	}
}


template<typename ElementType>
inline
void mdkMatrix<ElementType>::operator/=(ElementType Element)
{
	if (m_ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::operator/=(Element)" << '\n';
		return;
	}

	if (double(abs(Element)) < 0.0000000001)
	{
		mdkWarning << " abs(Element) < 0.0000000001 @ mdkMatrix::operator/=(Element)" << '\n';
	}

	auto BeginPointer = m_ElementData->data();

	for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementNumber; ++Ptr)
	{
		Ptr[0] /= Element;
	}
}


//-------------------- element operation {^} -----------------------------------------------------------//

template<typename ElementType>
inline void mdkMatrix<ElementType>::operator^(double value)
{
    if (m_ElementNumber == 0)
    {
        mdkError << "Self is empty @ mdkMatrix::operator^(value)" << '\n';
        return;
    }

    auto BeginPointer = m_ElementData->data();

    for (auto Ptr = BeginPointer; Ptr < BeginPointer + ElementNumber; ++Ptr)
    {
        Ptr[0] = std::pow(Ptr[0], value);
    }
}


//-------------------- element operation : output a new matrix ------------------------------------------//


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(const char* FunctionName) const
{
    return this->ElementOperation(std::string(FunctionName));
}


template<typename ElementType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(const std::string& FunctionName) const
{
	if (FunctionName == "abs")
	{
        return this->ElementOperation([](ElementType a){return std::abs(a); });
	}
	else if (FunctionName == "sin")
	{
        return this->ElementOperation([](ElementType a){return std::sin(a); });
	}
	else if (FunctionName == "cos")
	{
        return this->ElementOperation([](ElementType a){return std::cos(a); });
	}
	else if (FunctionName == "tan")
	{
        return this->ElementOperation([](ElementType a){return std::tan(a); });
	}
	else if (FunctionName == "sqrt")
	{
        return  this->ElementOperation([](ElementType a){return std::sqrt(a); });
	}
	else
	{
		mdkError << " unknown operator @ mdkMatrix::ElementOperation(std::string FunctionName)" << '\n';

        mdkMatrix<ElementType> tempMatrix;

        return tempMatrix;
	}
       
}


template<typename ElementType>
template<typename FunctionType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(FunctionType Function) const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::ElementOperation(Function)" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_RowNumber, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

    for (uint64 i = 0; i < ElementNumber; ++i)
	{
		tempRawPointer[i] = Function(RawPointer[i]);
	}

	return tempMatrix;
}



template<typename ElementType>
template<typename ElementType_target>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(const char* FunctionName, const mdkMatrix<ElementType_target>& targetMatrix) const
{
    return this->ElementOperation(std::string(FunctionName), targetMatrix);
}


template<typename ElementType>
template<typename ElementType_target>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(const std::string& FunctionName, const mdkMatrix<ElementType_target>& targetMatrix) const
{
	if (FunctionName == "+")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a + b; }, targetMatrix);
	}
	else if (FunctionName == "-")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a - b; }, targetMatrix);
	}
	else if (FunctionName == "*")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a * b; }, targetMatrix);
	}
	else if (FunctionName == "/")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a / b; }, targetMatrix);
	}
	else if (FunctionName == "^")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return std::pow(a, b); }, targetMatrix);
	}
	else
	{
		mdkError << " unknown operator @ mdkMatrix::ElementOperation(std::string FunctionName, targetMatrix)" << '\n';   

        mdkMatrix<ElementType> tempMatrix;

        return tempMatrix;
	}
}


template<typename ElementType>
template<typename FunctionType, typename ElementType_target>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(FunctionType Function, const mdkMatrix<ElementType_target>& targetMatrix) const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::ElementOperation(Function, targetMatrix)" << '\n';
        
		return tempMatrix;
	}

	auto targetMatrixSize = targetMatrix.GetSize();

	if	(targetMatrixSize.ColNumber == 0 || targetMatrixSize.RowNumber == 0)
	{
		mdkError << "empty targetMatrix @ mdkMatrix ElementOperation(Function, targetMatrix)" << '\n';
        
		return tempMatrix;
	}

	if (targetMatrixSize.RowNumber == 1 && targetMatrixSize.ColNumber == 1)
	{
        tempMatrix = this->ElementOperation(Function, targetMatrix(0));
        
        return tempMatrix;
	}

	auto Flag_row = 0;

	auto Flag_col = 0;

	auto Flag_full = 0;

	if (targetMatrixSize.ColNumber == m_ColNumber && targetMatrixSize.RowNumber == m_RowNumber)
	{
		Flag_full = 1;
	}
	else if (targetMatrixSize.ColNumber == 1 && targetMatrixSize.RowNumber == m_RowNumber)
	{
		Flag_col = 1;
	}
	else if (targetMatrixSize.ColNumber == m_ColNumber && targetMatrixSize.RowNumber == 1)
	{
		Flag_row = 1;
	}
	else
	{
		mdkError << "Size does not match @ mdkMatrix::ElementOperation(Functor, targetMatrix)" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_RowNumber, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	auto targetRawPointer = targetMatrix.GetElementDataRawPointer();

	if (Flag_full == 1)
	{
        auto ElementNumber = this->GetElementNumber();

		for (uint64 i = 0; i < ElementNumber; ++i)
		{
			tempRawPointer[i] = Function(RawPointer[i], ElementType(targetRawPointer[i]));
		}
	}
	else if (Flag_col == 1)
	{
		for (uint64 j = 0; j < m_ColNumber; ++j)
		{
			for (uint64 i = 0; i < m_RowNumber; ++i)
			{
				tempRawPointer[0] = Function(RawPointer[0], ElementType(targetRawPointer[i]));

				++tempRawPointer;
				++RawPointer;
			}
		}
	}
	else if (Flag_row == 1)
	{
		uint64 tempIndex = 0;

		for (uint64 i = 0; i < m_RowNumber; ++i)
		{
			tempIndex = 0;

			for (uint64 j = 0; j < m_ColNumber; ++j)
			{
				tempRawPointer[tempIndex + i] = Function(RawPointer[tempIndex + i], ElementType(targetRawPointer[i]));

				tempIndex += m_RowNumber;
			}
		}
	}

	return tempMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(const char* FunctionName, ElementType Element) const
{
    return this->ElementOperation(std::string(FunctionName), Element);
}


template<typename ElementType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(const std::string& FunctionName, ElementType Element) const
{
	if (FunctionName == "+")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a + b; }, Element);
	}
	else if (FunctionName == "-")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a - b; }, Element);
	}
	else if (FunctionName == "*")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a * b; }, Element);
	}
	else if (FunctionName == "/")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return a / b; }, Element);
	}
	else if (FunctionName == "^")
	{
        return this->ElementOperation([](ElementType a, ElementType b){return std::pow(a, b); }, Element);
	}
	else
	{
		mdkError << " unknown operator @ mdkMatrix::ElementOperation(std::string Operator, Element)" << '\n';   

        mdkMatrix<ElementType> tempMatrix;

        return tempMatrix;
	}
}


template<typename ElementType>
template<typename FunctionType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::ElementOperation(FunctionType Function, ElementType Element) const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "Self is empty @ mdkMatrix::ElementOperation(Functor, Element)" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_RowNumber, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

    auto ElementNumber = this->GetElementNumber();

	for (uint64 i = 0; i < ElementNumber; ++i)
	{
		tempRawPointer[i] = Function(RawPointer[i], ElementType(Element));
	}
	
	return tempMatrix;
}


//-------------------- element operation : in place : replace the current data of the matrix ------------------------------------------//


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(const char* FunctionName)
{
    return this->ElementOperationInPlace(std::string(FunctionName));
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(const std::string& FunctionName)
{
    if (FunctionName == "abs")
    {
        return this->ElementOperationInPlace([](ElementType a){return std::abs(a); });
    }
    else if (FunctionName == "sin")
    {
        return this->ElementOperationInPlace([](ElementType a){return std::sin(a); });
    }
    else if (FunctionName == "cos")
    {
        return this->ElementOperationInPlace([](ElementType a){return std::cos(a); });
    }
    else if (FunctionName == "tan")
    {
        return this->ElementOperationInPlace([](ElementType a){return std::tan(a); });
    }
    else if (FunctionName == "sqrt")
    {
        return  this->ElementOperationInPlace([](ElementType a){return std::sqrt(a); });
    }
    else
    {
        mdkError << " unknown operator @ mdkMatrix::ElementOperationInPlace(std::string FunctionName)" << '\n';

        return false;
    }

}


template<typename ElementType>
template<typename FunctionType>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(FunctionType Function)
{
    auto ElementNumber = this->GetElementNumber();

    if (ElementNumber == 0)
    {
        mdkError << "Self is empty @ mdkMatrix::ElementOperationInPlace(Function)" << '\n';

        return false;
    }

    auto RawPointer = m_ElementData->data();

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        RawPointer[i] = Function(RawPointer[i]);
    }

    return true;
}



template<typename ElementType>
template<typename ElementType_target>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(const char* FunctionName, const mdkMatrix<ElementType_target>& targetMatrix)
{
    return this->ElementOperationInPlace(std::string(FunctionName), targetMatrix);
}


template<typename ElementType>
template<typename ElementType_target>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(const std::string& FunctionName, const mdkMatrix<ElementType_target>& targetMatrix)
{
    if (FunctionName == "+")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a + b; }, targetMatrix);
    }
    else if (FunctionName == "-")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a - b; }, targetMatrix);
    }
    else if (FunctionName == "*")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a * b; }, targetMatrix);
    }
    else if (FunctionName == "/")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a / b; }, targetMatrix);
    }
    else if (FunctionName == "^")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return std::pow(a, b); }, targetMatrix);
    }
    else
    {
        mdkError << " unknown operator @ mdkMatrix::ElementOperationInPlace(std::string FunctionName, targetMatrix)" << '\n';

        return false;
    }
}


template<typename ElementType>
template<typename FunctionType, typename ElementType_target>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(FunctionType Function, const mdkMatrix<ElementType_target>& targetMatrix)
{
    if (m_ElementNumber == 0)
    {
        mdkError << "self is empty Matrix @ mdkMatrix::ElementOperationInPlace(Function, targetMatrix)" << '\n';

        return false;
    }

    auto targetMatrixSize = targetMatrix.GetSize();

    if (targetMatrixSize.ColNumber == 0 || targetMatrixSize.RowNumber == 0)
    {
        mdkError << "empty targetMatrix @ mdkMatrix ElementOperationInPlace(Function, targetMatrix)" << '\n';

        return false;
    }

    if (targetMatrixSize.RowNumber == 1 && targetMatrixSize.ColNumber == 1)
    {
        return this->ElementOperation(Function, targetMatrix(0));
    }

    auto Flag_row = 0;

    auto Flag_col = 0;

    auto Flag_full = 0;

    if (targetMatrixSize.ColNumber == m_ColNumber && targetMatrixSize.RowNumber == m_RowNumber)
    {
        Flag_full = 1;
    }
    else if (targetMatrixSize.ColNumber == 1 && targetMatrixSize.RowNumber == m_RowNumber)
    {
        Flag_col = 1;
    }
    else if (targetMatrixSize.ColNumber == m_ColNumber && targetMatrixSize.RowNumber == 1)
    {
        Flag_row = 1;
    }
    else
    {
        mdkError << "Size does not match @ mdkMatrix::ElementOperationInPlace(Function, targetMatrix)" << '\n';

        return false;
    }

    auto RawPointer = m_ElementData->data();

    auto targetRawPointer = targetMatrix.GetElementDataRawPointer();

    if (Flag_full == 1)
    {
        auto ElementNumber = this->GetElementNumber();

        for (uint64 i = 0; i < ElementNumber; ++i)
        {
            RawPointer[i] = Function(RawPointer[i], ElementType(targetRawPointer[i]));
        }
    }
    else if (Flag_col == 1)
    {
        for (uint64 j = 0; j < m_ColNumber; ++j)
        {
            for (uint64 i = 0; i < m_RowNumber; ++i)
            {
                RawPointer[0] = Function(RawPointer[0], ElementType(targetRawPointer[i]));

                ++RawPointer;
            }
        }
    }
    else if (Flag_row == 1)
    {
        uint64 tempIndex = 0;

        for (uint64 i = 0; i < m_RowNumber; ++i)
        {
            tempIndex = 0;

            for (uint64 j = 0; j < m_ColNumber; ++j)
            {
                RawPointer[tempIndex + i] = Function(RawPointer[tempIndex + i], ElementType(targetRawPointer[i]));

                tempIndex += m_RowNumber;
            }
        }
    }

    return true;
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(const char* FunctionName, ElementType Element)
{
    return this->ElementOperationInPlace(std::string(FunctionName), Element);
}


template<typename ElementType>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(const std::string& FunctionName, ElementType Element)
{
    if (FunctionName == "+")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a + b; }, Element);
    }
    else if (FunctionName == "-")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a - b; }, Element);
    }
    else if (FunctionName == "*")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a * b; }, Element);
    }
    else if (FunctionName == "/")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return a / b; }, Element);
    }
    else if (FunctionName == "^")
    {
        return this->ElementOperationInPlace([](ElementType a, ElementType b){return std::pow(a, b); }, Element);
    }
    else
    {
        mdkError << " unknown operator @ mdkMatrix::ElementOperationInPlace(std::string Operator, Element)" << '\n';

        return false;
    }
}


template<typename ElementType>
template<typename FunctionType>
inline
bool mdkMatrix<ElementType>::ElementOperationInPlace(FunctionType Function, ElementType Element)
{
    if (m_ElementNumber == 0)
    {
        mdkError << "Self is empty @ mdkMatrix::ElementOperationInPlace(Function, Element)" << '\n';

        return false;
    }

    auto RawPointer = m_ElementData->data();

    auto ElementNumber = this->GetElementNumber();

    for (uint64 i = 0; i < ElementNumber; ++i)
    {
        RawPointer[i] = Function(RawPointer[i], ElementType(Element));
    }

    return true;
}

//-----------------------------------------------------------------------------------------------------------//


template<typename ElementType>
inline ElementType mdkMatrix<ElementType>::Mean() const
{
    auto ElementNumber = this->GetElementNumber();

    if (ElementNumber == 0)
    {
        mdkError << "self is empty Matrix @ mdkMatrix::Mean" << '\n';
        return this->GetNaNElement();
    }

    auto RawPointer = m_ElementData->data();

    ElementType value = RawPointer[0];

    for (uint64 i = 1; i < ElementNumber; ++i)
    {
        value += RawPointer[i];
    }

    value /= m_ColNumber*m_RowNumber;

    return value;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::MeanToRow() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::MeanToRow" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(1, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

    ElementType value;

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		value = 0;

		for (uint64 i = 0; i < m_RowNumber; ++i)
		{
			value += RawPointer[Index];
			Index += 1;
		}

		value /= m_RowNumber;

		tempRawPointer[j] = value;
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::MeanToCol() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::MeanToCol" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_ColNumber, 1);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

    ElementType value;

	uint64 Index = 0;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		value = 0;

		Index = 0;

		for (uint64 j = 0; j < m_ColNumber; ++j)
		{
			value += RawPointer[Index + i];

			Index += m_RowNumber;
		}

		value /= m_ColNumber;

		tempRawPointer[i] = value;
	}

	return tempMatrix;
}


template<typename ElementType>
inline ElementType mdkMatrix<ElementType>::Sum() const
{
    auto ElementNumber = this->GetElementNumber();

	if (ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::Sum" << '\n';
		return 0;
	}

	auto RawPointer = m_ElementData->data();

	ElementType value = RawPointer[0];

	for (uint64 i = 1; i < ElementNumber; ++i)
	{
		value += RawPointer[i];
	}

	return value;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::SumToRow() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::SumToRow" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(1, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

    ElementType value;

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		value = 0;

		for (uint64 i = 0; i < m_RowNumber; ++i)
		{
			value += RawPointer[Index];
			Index += 1;
		}

		tempRawPointer[j] = value;
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::SumToCol() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::SumToCol" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_RowNumber, 1);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

    ElementType value;

	uint64 Index = 0;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		value = 0;

		Index = 0;

		for (uint64 j = 0; j < m_ColNumber; ++j)
		{
			value += RawPointer[Index + i];

			Index += m_RowNumber;
		}

		tempRawPointer[i] = value;
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline ElementType mdkMatrix<ElementType>::Max() const
{
    auto ElementNumber = this->GetElementNumber();

	if (ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::Max" << '\n';
		return 0;
	}

	auto RawPointer = m_ElementData->data();

	ElementType value = RawPointer[0];

	for (uint64 i = 1; i < ElementNumber; ++i)
	{
		value = std::max(value, RawPointer[i]);
	}

	return value;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::MaxToRow() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::MaxToRow" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(1, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		auto value = RawPointer[Index];

		for (uint64 i = 0; i < m_RowNumber; ++i)
		{
			value = std::max(value, RawPointer[Index]);
			Index += 1;
		}

		tempRawPointer[j] = value;
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::MaxToCol() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::MaxToCol" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_ColNumber, 1);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		Index = 0;

		auto value = RawPointer[Index + i];

		for (uint64 j = 0; j < m_ColNumber; ++j)
		{
			value = std::max(value, RawPointer[Index + i]);

			Index += m_RowNumber;
		}

		tempRawPointer[i] = value;
	}

	return tempMatrix;
}


template<typename ElementType>
inline ElementType mdkMatrix<ElementType>::Min() const
{
    auto ElementNumber = this->GetElementNumber();

	if (ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::Min" << '\n';
		return 0;
	}

	auto RawPointer = m_ElementData->data();

	ElementType value = RawPointer[0];

	for (uint64 i = 1; i < ElementNumber; ++i)
	{
		value = std::min(value, RawPointer[i]);
	}

	return value;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::MinToRow() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::MinToRow" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(1, m_ColNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 j = 0; j < m_ColNumber; ++j)
	{
		auto value = RawPointer[Index];

		for (uint64 i = 0; i < m_RowNumber; ++i)
		{
			value = std::min(value, RawPointer[Index]);
			Index += 1;
		}

		tempRawPointer[j] = value;
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline mdkMatrix<ElementType> mdkMatrix<ElementType>::MinToCol() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::MinToCol" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_ColNumber, 1);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		Index = 0;

		auto value = RawPointer[Index+i];

		for (uint64 j = 0; j < m_ColNumber; ++j)
		{
			value = std::min(value, RawPointer[Index + i]);

			Index += m_RowNumber;
		}

		tempRawPointer[i] = value;
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::Transpose() const
{
	mdkMatrix<ElementType> tempMatrix;

	if (m_ElementNumber == 0)
	{
		mdkError << "self is empty Matrix @ mdkMatrix::GetTranspose" << '\n';
        
		return tempMatrix;
	}

	tempMatrix.Resize(m_ColNumber, m_RowNumber);

	auto tempRawPointer = tempMatrix.GetElementDataRawPointer();

	auto RawPointer = m_ElementData->data();

	uint64 Index = 0;

	for (uint64 i = 0; i < m_RowNumber; ++i)
	{
		Index = 0;

		for (uint64 j = 0; j < m_ColNumber; ++j)
		{	
		    tempRawPointer[0] = RawPointer[Index + i];

			Index += m_RowNumber;

			++tempRawPointer;
		}
	}
    
	return tempMatrix;
}


template<typename ElementType>
inline
uint64 mdkMatrix<ElementType>::Rank() const
{
    return MatrixRank(*this);
}


template<typename ElementType>
inline 
mdkMatrix<ElementType> mdkMatrix<ElementType>::Inv() const
{
    return MatrixInv(*this);
}


template<typename ElementType>
inline
mdkMatrix<ElementType> mdkMatrix<ElementType>::PseudoInv() const
{
	// call Armadillo 


}


template<typename ElementType>
inline
mdkMatrixSVDResult<ElementType> mdkMatrix<ElementType>::SVD() const
{
    return MatrixSVD(*this);
}

}//end namespace mdk

#endif