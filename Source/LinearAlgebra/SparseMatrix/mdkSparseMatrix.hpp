#ifndef __mdkSparseMatrix_hpp
#define __mdkSparseMatrix_hpp

//#include "mdkSparseMatrix.h"

namespace mdk
{

//===================================================================================================================================//
//                                                    mdkSparseMatrixDataInCSCFormat
//===================================================================================================================================//

template<typename ElementType>
inline
SparseMatrixDataInCSCFormat<ElementType>::SparseMatrixDataInCSCFormat()
{
    this->Clear();
}


template<typename ElementType>
inline
SparseMatrixDataInCSCFormat<ElementType>::~SparseMatrixDataInCSCFormat()
{

}


template<typename ElementType>
inline
void
SparseMatrixDataInCSCFormat<ElementType>::Construct(int64 InputRowNumber, int64 InputColNumber)
{
    this->Clear();

    //----------------------------------------------------------

    m_RowNumber = InputRowNumber;

    m_ColNumber = InputColNumber;

    m_RecordedElementNumberInEachCol.resize(InputColNumber);

    for (int64 i = 0; i < InputColNumber; ++i)
    {
        m_RecordedElementNumberInEachCol[i] = 0;
    }

    m_ColBeginElementLinearIndexInDataArray.resize(InputColNumber + 1);

    for (int64 i = 0; i < InputColNumber; ++i)
    {
        m_ColBeginElementLinearIndexInDataArray[i] = -1;
    }

    m_ColBeginElementLinearIndexInDataArray[InputColNumber] = 0;
}


template<typename ElementType>
inline
void
SparseMatrixDataInCSCFormat<ElementType>::Construct(const int64* InputRowIndexList,
                                                       const int64* InputColIndexList,
                                                       const ElementType* InputDataArray,
                                                       int64 RecordedElementNumber,
                                                       int64 InputRowNumber,
                                                       int64 InputColNumber,
                                                       int64 AdditionalReservedCapacity = 0)
{
    this->Clear();

    //--------------------------------------------------------------

    m_RowNumber = InputRowNumber;

    m_ColNumber = InputColNumber;

    //--------------------------------------------------------------

    m_RowIndexList.reserve(RecordedElementNumber + AdditionalReservedCapacity);

    m_ColIndexList.reserve(RecordedElementNumber + AdditionalReservedCapacity);

    m_DataArray.reserve(RecordedElementNumber + AdditionalReservedCapacity);

    //--------------------------------------------------------------

    std::vector<int64> InputColIndexList_sort(RecordedElementNumber);

    std::vector<int64> LinearIndex_In_InputColIndexList(RecordedElementNumber);

    //sort tempColIndexList in ascending order

    Sort(InputColIndexList, RecordedElementNumber, InputColIndexList_sort.data(), LinearIndex_In_InputColIndexList.data(), "ascend");

    //--------------------------------------------------------------

    std::vector<int64> RowIndexSubList;

    RowIndexSubList.reserve(RecordedElementNumber);

    std::vector<int64> LinearIndex_In_RowIndexSubList;

    LinearIndex_In_RowIndexSubList.reserve(RecordedElementNumber);

    //--------------------------------------------------------------

    m_ColBeginElementLinearIndexInDataArray.resize(InputColNumber + 1);

    for (int64 i = 0; i < InputColNumber; ++i)
    {
        m_ColBeginElementLinearIndexInDataArray[i] = -1;
    }

    m_ColBeginElementLinearIndexInDataArray[InputColNumber] = RecordedElementNumber; // end of the record

    //----------------------------------------------------------------

    m_RecordedElementNumberInEachCol.resize(InputColNumber);

    for (int64 i = 0; i < InputColNumber; ++i)
    {
        m_RecordedElementNumberInEachCol[i] = 0;
    }

    //----------------------------------------------------------------

    m_ColBeginElementLinearIndexInDataArray[InputColIndexList_sort[0]] = 0;
    
    for (int64 i = 0; i < RecordedElementNumber; ++i)
    {
        int64 CurrentColIndex = InputColIndexList_sort[i];

        // add row index of element # i to the buffer RowIndexSubList for the current Col

        RowIndexSubList.push_back(InputRowIndexList[LinearIndex_In_InputColIndexList[i]]);

        bool Flag_save_col_info = false;

        if (i < RecordedElementNumber - 1)
        {
            if (InputColIndexList_sort[i + 1] != CurrentColIndex) // new col at i+1
            {
                Flag_save_col_info = true;
            }
        }
        else
        {
            Flag_save_col_info = true;
        }

        if (Flag_save_col_info == true)
        {
            // new col is detected at i+1, save the info of the current col

            auto RecordedElementNumberInPerviousCol = int64(RowIndexSubList.size());

            Sort(RowIndexSubList, RowIndexSubList, LinearIndex_In_RowIndexSubList, "ascend");

            int64 tempRowIndex_prev = -1;

            for (int64 j = 0; j < RecordedElementNumberInPerviousCol; ++j)
            {
                if (RowIndexSubList[j] == tempRowIndex_prev)
                {
                    MDK_Error << "duplicate values are found, construction abort! @ mdkSparseMatrixDataInCSCFormat::Construct(...)" << '\n';
                    
                    return;

                    // Matlab does this : not useful
                    // m_DataArray[m_DataArray.size()] += DataArray[tempOffset];
                }
                else
                {
                    m_RowIndexList.push_back(RowIndexSubList[j]);

                    m_ColIndexList.push_back(CurrentColIndex);

                    auto tempOffset = LinearIndex_In_InputColIndexList[i + 1 - RecordedElementNumberInPerviousCol + LinearIndex_In_RowIndexSubList[j]];

                    m_DataArray.push_back(InputDataArray[tempOffset]);

                    tempRowIndex_prev = RowIndexSubList[j];
                }
            }

            m_RecordedElementNumberInEachCol[CurrentColIndex] = RecordedElementNumberInPerviousCol;

            RowIndexSubList.resize(0);
            LinearIndex_In_RowIndexSubList.resize(0);

            // record the LinearIndex of first element in col # CurrentColIndex + 1

            m_ColBeginElementLinearIndexInDataArray[CurrentColIndex+1] = i+1;
        }       
    }
}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::Clear()
{
    m_RowNumber = 0;

    m_ColNumber = 0;

    m_RowIndexList.clear();

    m_ColBeginElementLinearIndexInDataArray.clear();

    m_RecordedElementNumberInEachCol.clear();

    m_DataArray.clear();

    m_ZeroElement = ElementType(0);
}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::Resize(int64 InputRowNumber, int64 InputColNumber)
{
    if (InputRowNumber == m_RowNumber && InputColNumber == m_ColNumber)
    {
        return;
    }

    if (InputRowNumber == 0 || InputColNumber == 0)
    {
        this->Clear();
        return;
    }

    if (m_DataArray.size() == 0)
    {
        this->Construct(InputRowNumber, InputColNumber);
        return;
    }

    //------ none empty , not equal to input size ---------------------//

    if (InputRowNumber >= m_RowNumber)
    {
        if (InputColNumber > m_ColNumber)
        {
            m_RecordedElementNumberInEachCol.resize(InputColNumber);

            for (int64 i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_RecordedElementNumberInEachCol[i] = 0;
            }

            m_ColBeginElementLinearIndexInDataArray.resize(InputColNumber + 1);

            for (int64 i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_ColBeginElementLinearIndexInDataArray[i] = -1;
            }

            int64 tempRecordedElmenetNumber = int64(m_DataArray.size());

            m_ColBeginElementLinearIndexInDataArray[InputColNumber - 1] = tempRecordedElmenetNumber;
        }
        else if (InputColNumber < m_ColNumber)
        {
            // first to erase elements in m_DataArray, m_RowIndexList, m_ColIndexList 
            // erase in backward order
            for (int64 i = m_ColNumber - 1; i >= InputColNumber; --i)
            {
                if (m_RecordedElementNumberInEachCol[i] > 0)
                {
                    auto Offset_start = m_ColBeginElementLinearIndexInDataArray[i];
                    auto Offset_end = Offset_start + m_RecordedElementNumberInEachCol[i] - 1;

                    m_RowIndexList.erase(m_RowIndexList.begin() + Offset_start, m_RowIndexList.begin() + Offset_end);

                    m_ColIndexList.erase(m_ColIndexList.begin() + Offset_start, m_ColIndexList.begin() + Offset_end);

                    m_DataArray.erase(m_DataArray.begin() + Offset_start, m_DataArray.begin() + Offset_end);
                }

                m_RecordedElementNumberInEachCol.erase(m_RecordedElementNumberInEachCol.begin() + i);

                m_ColBeginElementLinearIndexInDataArray.erase(m_ColBeginElementLinearIndexInDataArray.begin() + i);
            }

            int64 tempRecordedElmenetNumber = int64(m_DataArray.size());

            m_ColBeginElementLinearIndexInDataArray[m_ColBeginElementLinearIndexInDataArray.size() - 1] = tempRecordedElmenetNumber;
        }
        //else if (InputColNumber == m_ColNumber)
        // do nothing

        m_RowNumber = InputRowNumber;
        m_ColNumber = InputColNumber;
        return;
    }

    // InputRowNumber < m_RowNumbe -----------------------------------------------------------------------------

    if (InputColNumber >= m_ColNumber)
    {
        // loop through m_RowIndexList, erase the record if its row-index is greater than RowNumber-1

        for (int64 i = int64(m_RowIndexList.size()) -1; i >= 0; --i)
        {
            if (m_RowIndexList[i] >= InputRowNumber)
            {
                m_DataArray.erase(m_DataArray.begin() + i);

                m_RowIndexList.erase(m_RowIndexList.begin() + i);

                auto tempColIndex_i = m_ColIndexList[i];

                m_RecordedElementNumberInEachCol[tempColIndex_i] -= 1;

                if (m_RecordedElementNumberInEachCol[tempColIndex_i] == 0)
                {
                    m_ColBeginElementLinearIndexInDataArray[tempColIndex_i] = -1;
                }

                m_ColIndexList.erase(m_ColIndexList.begin() + i);
            }
        }

        if (InputColNumber > m_ColNumber)
        {
            m_RecordedElementNumberInEachCol.resize(InputColNumber);

            for (int64 i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_RecordedElementNumberInEachCol[i] = 0;
            }

            m_ColBeginElementLinearIndexInDataArray.resize(InputColNumber + 1);

            for (int64 i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_ColBeginElementLinearIndexInDataArray[i] = -1;
            }

            int64 tempRecordedElmenetNumber = int64(m_DataArray.size());

            m_ColBeginElementLinearIndexInDataArray[InputColNumber - 1] = tempRecordedElmenetNumber;
        }

        m_RowNumber = InputRowNumber;
        m_ColNumber = InputColNumber;
        return;
    }

    // InputRowNumber < m_RowNumber && InputColNumber < m_ColNumber -------------------------------------------------------

    // keep the elements in range, and then construct a new sparse matrix

    std::vector<int64> newRowIndexList;

    std::vector<int64> newColIndexList;

    std::vector<ElementType> newDataArray;

    for (int64 i = int64(m_RowIndexList.size()) -1; i >= 0; --i)
    {
        if (m_RowIndexList[i] < InputRowNumber && m_ColIndexList[i] < InputColNumber)
        {
            newRowIndexList.push_back(m_RowIndexList[i]);

            newColIndexList.push_back(m_ColIndexList[i]);

            newDataArray.push_back(m_DataArray[i]);
        }
    }

    this->Construct(newRowIndexList.data(), newColIndexList.data(), newDataArray.data(), int64(newDataArray.size()), InputRowNumber, InputColNumber);
   
}


template<typename ElementType>
inline
ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator[](int64 LinearIndex)
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator[](int64 LinearIndex) const
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int64 LinearIndex)
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    ColIndex = divresult.quot;

    RowIndex = divresult.rem;

    return (*this)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int64 LinearIndex) const
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    ColIndex = divresult.quot;

    RowIndex = divresult.rem;

    return (*this)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int64 RowIndex, int64 ColIndex)
{
    return this->SetElement(RowIndex, ColIndex, m_ZeroElement);
}


template<typename ElementType>
inline
const ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int64 RowIndex, int64 ColIndex) const
{
    return this->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrixDataInCSCFormat<ElementType>::GetElement(int64 LinearIndex) const
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    ColIndex = divresult.quot;

    RowIndex = divresult.rem;

    return this->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrixDataInCSCFormat<ElementType>::GetElement(int64 RowIndex, int64 ColIndex) const
{
    auto RelativeIndex = this->GetLinearIndexInDataArray(RowIndex, ColIndex);

    if (RelativeIndex >= 0)
    {
        return m_DataArray[RelativeIndex];
    }

    return m_ZeroElement;
}


template<typename ElementType>
inline
ElementType& SparseMatrixDataInCSCFormat<ElementType>::SetElement(int64 LinearIndex, const ElementType& InputElement)
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    ColIndex = divresult.quot;

    RowIndex = divresult.rem;

    return this->SetElement(RowIndex, ColIndex, InputElement);
}


template<typename ElementType>
inline
ElementType& SparseMatrixDataInCSCFormat<ElementType>::SetElement(int64 RowIndex, int64 ColIndex, const ElementType& InputElement)
{
    auto tempRelativeIndex = this->GetLinearIndexInDataArray(RowIndex, ColIndex);

    if (tempRelativeIndex >= 0)
    {
        m_DataArray[tempRelativeIndex] = InputElement;

        return m_DataArray[tempRelativeIndex];
    }

    //---------------------------------------- Create a new record --------------------------------------------------//

    if (m_ColBeginElementLinearIndexInDataArray[ColIndex] < 0) // insert the first element in col # ColIndex
    {
        int64 Offset = -1;

        // check col with Index > ColIndex
        for (int64 i = ColIndex + 1; i < m_ColNumber; ++i)
        {
            if (m_ColBeginElementLinearIndexInDataArray[i] >= 0)
            {
                Offset = m_ColBeginElementLinearIndexInDataArray[i];

                break;
            }
        }

        if (Offset < 0) // every col (index > ColIndex) is empty
        {
            // check backward
            for (int64 i = ColIndex - 1; i >= 0; --i)
            {
                if (m_RecordedElementNumberInEachCol[i] > 0)
                {
                    Offset = m_ColBeginElementLinearIndexInDataArray[i] + m_RecordedElementNumberInEachCol[i];

                    break;
                }
            }

            if (Offset < 0) // still empty: this is an empty matrix
            {
                Offset = 0;
            }
        }

        m_RowIndexList.insert(m_RowIndexList.begin() + Offset, RowIndex);

        m_ColIndexList.insert(m_ColIndexList.begin() + Offset, ColIndex);

        m_DataArray.insert(m_DataArray.begin() + Offset, InputElement);

        m_ColBeginElementLinearIndexInDataArray[ColIndex] = Offset;

        for (int64 i = ColIndex + 1; i < m_ColNumber; ++i)
        {
            if (m_ColBeginElementLinearIndexInDataArray[i] >= 0)
            {
                m_ColBeginElementLinearIndexInDataArray[i] += 1;
            }
        }

        m_RecordedElementNumberInEachCol[ColIndex] = 1;

        return m_DataArray[Offset];

    }
    else // insert the input element in a non-empty col # ColIndex
    {
        // get the RowIndexSubList of the elements in Col # ColIndex
        // compare RowIndex to  RowIndexSubList, and get offset

        int64 Index_Begin = m_ColBeginElementLinearIndexInDataArray[ColIndex];

        int64 Offset = Index_Begin + m_RecordedElementNumberInEachCol[ColIndex];

        for (int64 i = 0; i < m_RecordedElementNumberInEachCol[ColIndex]; ++i)
        {
            if (m_RowIndexList[Index_Begin + i] > RowIndex)
            {
                Offset = Index_Begin + i;

                break;
            }
        }

        m_RowIndexList.insert(m_RowIndexList.begin() + Offset, RowIndex);

        m_ColIndexList.insert(m_ColIndexList.begin() + Offset, ColIndex);

        m_DataArray.insert(m_DataArray.begin() + Offset, InputElement);

        for (int64 i = ColIndex + 1; i < m_ColNumber; ++i)
        {
            if (m_ColBeginElementLinearIndexInDataArray[i] >= 0)
            {
                m_ColBeginElementLinearIndexInDataArray[i] += 1;
            }
        }

        m_RecordedElementNumberInEachCol[ColIndex] += 1;

        return m_DataArray[Offset];
    }

}


template<typename ElementType>
inline
int64 SparseMatrixDataInCSCFormat<ElementType>::GetLinearIndexInDataArray(int64 LinearIndex) const
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    ColIndex = divresult.quot;

    RowIndex = divresult.rem;

    return this->GetLinearIndexInDataArray(RowIndex, ColIndex);
}


template<typename ElementType>
inline
int64 SparseMatrixDataInCSCFormat<ElementType>::GetLinearIndexInDataArray(int64 RowIndex, int64 ColIndex) const
{
    auto RelativeIndex_Begin = m_ColBeginElementLinearIndexInDataArray[ColIndex];

    if (RelativeIndex_Begin >= 0)
    {
        for (int64 i = 0; i < m_RecordedElementNumberInEachCol[ColIndex]; ++i)
        {
            auto RelativeIndex = RelativeIndex_Begin + i;

            if (m_RowIndexList[RelativeIndex] == RowIndex)
            {
                return RelativeIndex;
            }
        }
    }

    return -1;
}


template<typename ElementType>
inline
int64 SparseMatrixDataInCSCFormat<ElementType>::GetRecordedElementNumber() const
{
    return int64(m_DataArray.size());
}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::Copy(const SparseMatrixDataInCSCFormat<ElementType>& InputMatrixData)
{
    m_RowNumber = InputMatrixData.m_RowNumber;

    m_ColNumber = InputMatrixData.m_ColNumber;

    m_RowIndexList = InputMatrixData.m_RowIndexList;

    m_DataArray = InputMatrixData.m_DataArray;

    m_ColBeginElementLinearIndexInDataArray = InputMatrixData.m_ColBeginElementLinearIndexInDataArray;

    m_RecordedElementNumberInEachCol = InputMatrixData.m_RecordedElementNumberInEachCol;

    m_ZeroElement = InputMatrixData.m_ZeroElement;
}


template<typename ElementType>
inline
void SparseMatrixDataInCSCFormat<ElementType>::Take(SparseMatrixDataInCSCFormat<ElementType>& InputMatrixData)
{
    m_RowNumber = InputMatrixData.m_RowNumber;

    m_ColNumber = InputMatrixData.m_ColNumber;

    m_RowIndexList = std::move(InputMatrixData.m_RowIndexList);

    m_DataArray = std::move(InputMatrixData.m_DataArray);

    m_ColBeginElementLinearIndexInDataArray = std::move(InputMatrixData.m_ColBeginElementLinearIndexInDataArray);

    m_RecordedElementNumberInEachCol = std::move(InputMatrixData.m_RecordedElementNumberInEachCol);

    m_ZeroElement = InputMatrixData.m_ZeroElement;
}

//===================================================================================================================================//
//                                                    mdkSparseMatrix
//===================================================================================================================================//

template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix()
{
    m_IsSizeFixed = false;

    this->Resize(0, 0);
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(int64 RowNumber, int64 ColNumber)
{
    m_IsSizeFixed = false;

    this->Resize(RowNumber, ColNumber);
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(const ElementType& Element)
{
    m_IsSizeFixed = false;

    this->Resize(1, 1);

    (*this)(0) = Element;
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(const SparseMatrix<ElementType>& InputMatrix, 
                                        ObjectConstructionTypeEnum Method = ObjectConstructionTypeEnum::Copy)
{
    m_IsSizeFixed = false;

    if (Method == mdkObjectConstructionTypeEnum::Copy)
    {
        this->Resize(0, 0);

        this->Copy(InputMatrix);
    }
    else
    {
        this->Share(InputMatrix);
    }
}


// move constructor
template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(SparseMatrix<ElementType>&& InputMatrix)
{
    m_MatrixData = std::move(InputMatrix.m_MatrixData);

    m_NaNElement = InputMatrix.m_NaNElement;

    m_IsSizeFixed = InputMatrix.m_IsSizeFixed;
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    this->Resize(0, 0);

    this->Take(ShadowMatrix.CreateSparseMatrix());

    m_NaNElement = ShadowMatrix.m_NaNElement;
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::~SparseMatrix()
{
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const DenseMatrix<int64>& RowIndexList,
                                          const DenseMatrix<int64>& ColIndexList,
                                          const DenseMatrix<ElementType>& DataArray,
                                          int64 RowNumber,
                                          int64 ColNumber,
                                          int64 AdditionalReservedCapacity = 0)
{
    return this->Construct(RowIndexList.GetElementPointer(),
                           ColIndexList.GetElementPointer(), 
                           DataArray.GetElementPointer(), 
                           RowNumber,
                           ColNumber, 
                           std::max(AdditionalReservedCapacity, int64(0)));
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const std::vector<int64>& RowIndexList,
                                          const std::vector<int64>& ColIndexList,
                                          const std::vector<ElementType>& DataArray,
                                          int64 RowNumber,
                                          int64 ColNumber,
                                          int64 AdditionalReservedCapacity = 0)
{
    return this->Construct(RowIndexList.data(),
                           ColIndexList.data(), 
                           DataArray.data(), 
                           int64(DataArray.size()), 
                           RowNumber, 
                           ColNumber,
                           std::max(AdditionalReservedCapacity, int64(0)));

}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const std::initializer_list<int64>& RowIndexList,
                                          const std::initializer_list<int64>& ColIndexList,
                                          const std::initializer_list<ElementType>& DataArray,
                                          int64 RowNumber,
                                          int64 ColNumber,
                                          int64 AdditionalReservedCapacity = 0)
{
    return this->Construct(RowIndexList.begin(), 
                           ColIndexList.begin(), 
                           DataArray.begin(), 
                           int64(DataArray.size()), 
                           RowNumber, 
                           ColNumber, 
                           std::max(AdditionalReservedCapacity, int64(0)));
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const int64* RowIndexList,
                                          const int64* ColIndexList,
                                          const ElementType* DataArray,
                                          int64 RecordedElementNumber,
                                          int64 RowNumber,
                                          int64 ColNumber,
                                          int64 AdditionalReservedCapacity = 0)
{
    //check input
    if (RowIndexList == nullptr || ColIndexList == nullptr || DataArray == nullptr || RecordedElementNumber < 0 || RowNumber < 0 || ColNumber < 0)
    {
        MDK_Error << "empty input @ mdkSparseMatrix::Construct(pointers, ...)" << '\n';
        return false;
    }

    m_MatrixData = std::make_shared<SparseMatrixDataInCSCFormat<ElementType>>();

    if (RecordedElementNumber == 0)
    {
        m_MatrixData->Construct(RowNumber, ColNumber);

        return true;
    }

    m_MatrixData->Construct(RowIndexList, ColIndexList, DataArray, RecordedElementNumber, RowNumber, ColNumber, std::max(AdditionalReservedCapacity, int64(0)));

    return true;
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const SparseMatrix<ElementType>& InputMatrix)
{
    this->Copy(InputMatrix);
}


// move assignment operator
template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(SparseMatrix<ElementType>&& InputMatrix)
{
    this->Take(InputMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const ElementType& Element)
{
    auto ElementNumber = this->GetElementNumber();

    if (m_IsSizeFixed == true)
    {
        if (ElementNumber <= 0)
        {
            MDK_Error << "Can not change matrix size @ mdkSparseMatrix::operator=(Element)" << '\n';
            return;
        }
    }

    if (ElementNumber <= 0)
    {       
        this->Resize(1, 1);

        (*this)[0] = Element;
    }
    else if (ElementNumber == 1)
    {
        (*this)[0] = Element;
    }
    else
    {
        mdkWarning << "The size of the matrix changes to 1x1 @ mdkSparseMatrix::operator=(Element)" << '\n';

        this->Clear();

        this->Resize(1, 1);

        (*this)[0] = Element;
    }

    return;
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const std::initializer_list<ElementType>& list)
{
    auto SelfSize = this->GetSize();

    auto ElementNumber = SelfSize.RowNumber * SelfSize.ColNumber;

    //if Self is empty
    if (ElementNumber <= 0)
    {
        MDK_Error << "operator=(list) can not be called if self is empty @ mdkSparseMatrix::operator=(list)" << '\n';
        return;
    }

    if (ElementNumber != list.size())
    {
        MDK_Error << "Size does not match @ mdkSparseMatrix::operator=(list)" << '\n';
        return;
    }

    auto RawPointer = this->GetElementPointer();

    int64 Counter = 0;

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        int64 Index = 0;

        for (int64 j = 0; j < SelfSize.ColNumber; ++j)
        {
            RawPointer[Index + i] = list.begin()[Counter];

            Index += SelfSize.RowNumber;

            Counter += 1;
        }
    }

    return;
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const std::initializer_list<std::initializer_list<ElementType>>& list)
{
    auto SelfSize = this->GetSize();

    //if Self is empty
    if (SelfSize.RowNumber <= 0)
    {
        MDK_Error << "operator=(list in list) can not be called if self is empty @ mdkSparseMatrix::operator=(list in list)" << '\n';
        return;
    }

    if (SelfSize.RowNumber != list.size())
    {
        MDK_Error << "Row Size does not match @ mdkSparseMatrix::operator=(list in list)" << '\n';
        return;
    }

    //check each row-list 
    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        auto subList = list.begin()[i];

        if (subList.size() != SelfSize.ColNumber)
        {
            MDK_Error << "Col Size does not match in row: " << i << " @ mdkSparseMatrix::operator=(list in list)" << '\n';
            return;
        }
    }

    auto RawPointer = this->GetElementPointer();

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        auto subList = list.begin()[i];

        int64 Index = 0;

        for (int64 j = 0; j < SelfSize.ColNumber; ++j)
        {
            RawPointer[Index + i] = subList.begin()[j];

            Index += SelfSize.RowNumber;
        }
    }

    return;
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    this->Take(ShadowMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const SparseGlueMatrixForLinearCombination<ElementType>& GlueMatrix)
{
    this->Take(GlueMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const SparseGlueMatrixForMultiplication<ElementType>& GlueMatrix)
{
    this->Take(GlueMatrix);
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::Copy(const SparseMatrix<ElementType_Input>& InputMatrix)
{
    if (this == &InputMatrix)
    {
        mdkWarning << "A Matrix tries to Copy itself @ mdkSparseMatrix::Copy(InputMatrix)" << '\n';
        return false;
    }

    if (InputMatrix.IsEmpty() == true)
    {
        mdkWarning << "InputMatrix is empty, and this matrix is set to be empty @ mdkSparseMatrix::Copy(InputMatrix)" << '\n';

        this->Clear();

        return true;
    }

    auto InputSize = InputMatrix.GetSize();

    if (m_IsSizeFixed == true)
    {
        auto SelfSize = this->GetSize();

        if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
        {
            MDK_Error << "Can not change matrix size @ mdkSparseMatrix::Copy(InputMatrix)" << '\n';
            return false;
        }
    }
    else
    {
        this->Clear();
    }

    //----------------------------- copy ----------------------//

    m_MatrixData->Copy(*InputMatrix.m_MatrixData);
  
    return true;
    
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Share(SparseMatrix<ElementType>& InputMatrix)
{
    // MatrixA = MatrixA
    if (this == &InputMatrix)
    {
        mdkWarning << "A Matrix tries to share itself @ mdkSparseMatrix::Share(InputMatrix)" << '\n';
        return false;
    }

    auto InputSize = InputMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (m_IsSizeFixed == true)
    {
        if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
        {
            MDK_Error << "Matrix size can not be changed @ mdkSparseMatrix::Share(InputMatrix)" << '\n';
            return false;
        }
    }

    m_MatrixData = InputMatrix.m_MatrixData; // std::SharedCopy_ptr

    return true;
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::ForceShare(const SparseMatrix<ElementType>& InputMatrix)
{
    m_MatrixData = InputMatrix.m_MatrixData; // std::SharedCopy_ptr
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(SparseMatrix<ElementType>& InputMatrix)
{
    // MatrixA = MatrixA
    if (this == &InputMatrix)
    {
        mdkWarning << "A Matrix tries to take itself @ mdkSparseMatrix::Take(InputMatrix)" << '\n';
        return false;
    }

    auto InputSize = InputMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (m_IsSizeFixed == true)
    {
        if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
        {
            MDK_Error << "Size does not match @ mdkSparseMatrix::Take(InputMatrix)" << '\n';
            return false;
        }
    }

    if (InputMatrix.IsEmpty() == true)
    {
        if (SelfSize.RowNumber > 0)
        {
            mdkWarning << "InputMatrix is empty, and this matrix is set to be empty @ mdkSparseMatrix::Take(InputMatrix)" << '\n';
            this->Clear();
        }

        return true;
    }

    // now, InputMatrix is not empty

    m_MatrixData->Take(*InputMatrix.m_MatrixData);

    // Resize InputMatrix to be empty
    InputMatrix.Clear();

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(SparseMatrix<ElementType>&& InputMatrix)
{
    SparseMatrix<ElementType>& tempMatrix = InputMatrix;

    return this->Take(tempMatrix);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    auto InputSize = ShadowMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber == SelfSize.RowNumber && InputSize.ColNumber == SelfSize.ColNumber)
    {
        ShadowMatrix.CreateSparseMatrix(*this);
    }
    else
    {
        if (m_IsSizeFixed == true)
        {
            MDK_Error << "Size does not match @ mdkSparseMatrix::Take(ShadowMatrix)" << '\n';
            return false;
        }

        this->Take(ShadowMatrix.CreateSparseMatrix());
    }

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(const SparseGlueMatrixForLinearCombination<ElementType>& GlueMatrix)
{
    auto InputSize = GlueMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber == SelfSize.RowNumber && InputSize.ColNumber == SelfSize.ColNumber)
    {
        GlueMatrix.CreateSparseMatrix(*this);
    }
    else
    {
        if (m_IsSizeFixed == true)
        {
            MDK_Error << "Size does not match @ mdkSparseMatrix::Take(SparseGlueMatrix_ForLinearCombination)" << '\n';
            return false;
        }

        this->Take(GlueMatrix.CreateSparseMatrix());
    }

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(const SparseGlueMatrixForMultiplication<ElementType>& GlueMatrix)
{
    auto InputSize = GlueMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber == SelfSize.RowNumber && InputSize.ColNumber == SelfSize.ColNumber)
    {
        GlueMatrix.CreateSparseMatrix(*this);
    }
    else
    {
        if (m_IsSizeFixed == true)
        {
            MDK_Error << "Size does not match @ mdkSparseMatrix::Take(SparseGlueMatrix_ForMultiplication)" << '\n';
            return false;
        }

        this->Take(GlueMatrix.CreateSparseMatrix());
    }

    return true;
}


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::Clear()
{
    m_MatrixData->Clear();
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Reshape(int64 InputRowNumber, int64 InputColNumber)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix size can not be changed @ mdkSparseMatrix::Reshape()" << '\n';
        return false;
    }

    if (this->IsEmpty() == true)
    {
        if (InputRowNumber > 0 || InputColNumber > 0)
        {
            MDK_Error << "Self is empty and Size does not match @ mdkSparseMatrix::Reshape()" << '\n';
            return false;
        }
    }

    if (InputRowNumber*InputColNumber != this->GetElementNumber())
    {
        MDK_Error << "Size does not match @ mdkSparseMatrix::Reshape" << '\n';
        return false;
    }

  //-------------------------------------------

    return false;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Resize(int64 InputRowNumber, int64 InputColNumber)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not be changed @ mdkSparseMatrix::Resize(int64 RowNumber, int64 ColNumber)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (InputRowNumber == SelfSize.RowNumber && InputColNumber == SelfSize.ColNumber)
    {        
        return true;
    }

    if (InputRowNumber < 0 || InputColNumber < 0)
    {
        MDK_Error << "Invalid Input: negtive @ mdkSparseMatrix::Resize(int64 RowNumber, int64 ColNumber)" << '\n';
        return false;
    }

    //--------initialize the matrix ----------------------------------------------------
    if (!m_MatrixData)
    {
        m_MatrixData = std::make_shared<SparseMatrixDataInCSCFormat<ElementType>>();

        m_NaNElement = GetMatrixNaNElement(m_NaNElement);
    }
    //-----------------------------------------------------------------------------------

    if (InputRowNumber == 0 || InputColNumber == 0)
    {
        this->Clear();

        return true;
    }

    // if self is empty
    if (SelfSize.RowNumber <= 0)
    {
        m_MatrixData->Construct(InputRowNumber, InputColNumber);
    }
    else
    {
        m_MatrixData->Resize(InputRowNumber, InputColNumber);
    }

    return true;
}


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::FixSize()
{
    m_IsSizeFixed = true;

    m_MatrixData->m_DataArray.shrink_to_fit();
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsSizeFixed() const
{
    return m_IsSizeFixed;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsEmpty() const
{
    return !(m_MatrixData); 
}


template<typename ElementType>
inline
MatrixSize SparseMatrix<ElementType>::GetSize() const
{
    MatrixSize Size;

    Size.RowNumber = m_MatrixData->m_RowNumber;

    Size.ColNumber = m_MatrixData->m_ColNumber;

    return Size;
}


template<typename ElementType>
inline
int64 SparseMatrix<ElementType>::GetElementNumber() const
{
    return m_MatrixData->m_RowNumber * m_MatrixData->m_ColNumber;
}


template<typename ElementType>
inline
int64 SparseMatrix<ElementType>::GetRecordedElementNumber() const
{
    return m_MatrixData->GetRecordedElementNumber();
}


template<typename ElementType>
inline 
int64 SparseMatrix<ElementType>::GetColNumber() const
{
    return m_MatrixData->m_ColNumber;
}


template<typename ElementType>
inline 
int64 SparseMatrix<ElementType>::GetRowNumber() const
{
    return m_MatrixData->m_RowNumber;
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::GetNaNElement()  const
{
    return m_NaNElement;
}


template<typename ElementType>
inline
MatrixElementTypeEnum SparseMatrix<ElementType>::GetElementType() const
{
    return FindMatrixElementType(m_NaNElement);
}


template<typename ElementType>
inline
ElementType* SparseMatrix<ElementType>::GetRecordedElementPointer()
{
    return m_MatrixData->m_DataArray.data();
}


template<typename ElementType>
inline
const ElementType* SparseMatrix<ElementType>::GetRecordedElementPointer() const
{
    return m_MatrixData->m_DataArray.data();
}


template<typename ElementType>
inline 
const ElementType& SparseMatrix<ElementType>::GetElement(int64 LinearIndex) const
{ 
    if (LinearIndex >= this->GetElementNumber || LinearIndex < 0)
    {
        MDK_Error << "Invalid input @ mdkSparseMatrix::GetElement(LinearIndex, InputElement)" << '\n';
        return m_NaNElement;
    }

    return m_MatrixData->GetElement(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::GetElement(int64 RowIndex, int64 ColIndex) const
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid input @ mdkSparseMatrix::GetElement(RowIndex, ColIndex)" << '\n';
        return m_NaNElement;
    }

    return m_MatrixData->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::SetElement(int64 LinearIndex, const ElementType& InputElement)
{
    if (LinearIndex >= this->GetElementNumber || LinearIndex < 0)
    {
        MDK_Error << "Invalid input @ mdkSparseMatrix::SetElement(LinearIndex, InputElement)" << '\n';
        return false;
    }

    m_MatrixData->SetElement(LinearIndex, InputElement);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::SetElement(int64 RowIndex, int64 ColIndex, const ElementType& InputElement)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid input @ mdkSparseMatrix::SetElement(RowIndex, ColIndex, InputElement)" << '\n';
        return false;
    }

    m_MatrixData->SetElement(RowIndex, ColIndex, InputElement);
}


//----------- Get/Set Matrix(LinearIndex) -----------------------------------//

// operator[] (): no bound check in release mode

template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::operator[](int64 LinearIndex)
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::operator[](int64 LinearIndex) const
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::operator()(int64 LinearIndex)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::operator()(i)" << '\n';

        return m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    return (*m_MatrixData)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::operator()(int64 LinearIndex) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::operator()(i) const" << '\n';

        return m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    return (*m_MatrixData)(LinearIndex);
}

// at(): bound check

template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::at(int64 LinearIndex)
{
    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::at(i)" << '\n';
        
        return m_NaNElement;
	}

    return (*m_MatrixData)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::at(int64 LinearIndex) const
{
    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::at(i) const" << '\n';
        
        return m_NaNElement;
	}

    return (*m_MatrixData)(LinearIndex);
}

//----------- Get/Set Matrix(i,j)  ---------------------------------------------//

// operator(): no bound check in release mode

template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::operator()(int64 RowIndex, int64 ColIndex)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::operator()(i,j)" << '\n';

        return m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound
    
    return (*m_MatrixData)(RowIndex, ColIndex);

}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::operator()(int64 RowIndex, int64 ColIndex) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::operator()(i,j) const" << '\n';

        return m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    return (*m_MatrixData)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::at(int64 RowIndex, int64 ColIndex)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::at(i,j)" << '\n';
        
        return m_NaNElement;
    }
    
    return (*m_MatrixData)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::at(int64 RowIndex, int64 ColIndex) const
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::at(i,j) const" << '\n';
        
        return m_NaNElement;
    }

    return (*m_MatrixData)(RowIndex, ColIndex);
}

//---------------------- Get/Set a set of elements ------------------------------------------------------//

// operator(): no bound check in release mode
//
// note: operator[] is for single element access only, operator[{}] is not defined

template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(std::initializer_list<int64>& LinearIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::operator()(std::initializer_list {LinearIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::operator()(std::initializer_list {LinearIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(std::initializer_list<int64>& LinearIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::operator()(std::initializer_list {LinearIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::operator()(std::initializer_list {LinearIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::vector<int64>& LinearIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::operator()(std::vector {LinearIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::operator()(std::vector {LinearIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::vector<int64>& LinearIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::operator()(std::vector {LinearIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::operator()(std::vector {LinearIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(std::initializer_list<int64>& LinearIndexList)
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::at(std::initializer_list {LinearIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::at(std::initializer_list {LinearIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(std::initializer_list<int64>& LinearIndexList) const
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::at(std::initializer_list {LinearIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::at(std::initializer_list {LinearIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::vector<int64>& LinearIndexList)
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::at(std::vector {LinearIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::at(std::vector {LinearIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::vector<int64>& LinearIndexList) const
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error << "LinearIndexList is empty @ mdkSparseMatrix::at(std::vector {LinearIndexList} const)" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error << "Invalid LinearIndexList @ mdkSparseMatrix::at(std::vector {LinearIndexList} const)" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol);

    return tempShadowMatrix;
}

//---------------------- Get/Set SubMatrix ------------------------------------------------------//

// operator(): no bound check

template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(std::initializer_list<int64>& RowIndexList, std::initializer_list<int64>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(std::initializer_list<int64>& RowIndexList, std::initializer_list<int64>& ColIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::initializer_list<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList is empty @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL)" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL)" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::initializer_list<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList is empty @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int64>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::operator()(ALL, std::initializer_list {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(ALL, std::initializer_list {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int64>& ColIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::operator()(ALL, std::initializer_list {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(ALL, std::initializer_list {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::vector<int64>& RowIndexList, const std::vector<int64>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::vector<int64>& RowIndexList, const std::vector<int64>& ColIndexList) const 
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::vector<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, ALL)" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, ALL)" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::vector<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, ALL) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::operator()(std::vector {RowIndexList}, ALL) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int64>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::operator()(ALL, std::vector {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(ALL, std::vector {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int64>& ColIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::operator()(ALL, std::vector {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::operator()(ALL, std::vector {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}

// at(): bound check

template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(std::initializer_list<int64>& RowIndexList, std::initializer_list<int64>& ColIndexList)
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at({RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(std::initializer_list<int64>& RowIndexList, std::initializer_list<int64>& ColIndexList) const
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at({RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType> 
SparseMatrix<ElementType>::at(const std::initializer_list<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList is empty @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, ALL)" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, ALL)" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::initializer_list<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList is empty @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, ALL) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::initializer_list {RowIndexList}, ALL) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType> 
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int64>& ColIndexList)
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::at(ALL, std::initializer_list {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at(ALL, std::initializer_list {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int64>& ColIndexList) const
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::at(ALL, std::initializer_list {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at(ALL, std::initializer_list {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::vector<int64>& RowIndexList, const std::vector<int64>& ColIndexList)
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::at(std::vector {RowIndexList}, {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::vector {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at(std::vector {RowIndexList}, {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::vector<int64>& RowIndexList, const std::vector<int64>& ColIndexList) const
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList or  ColIndexList is empty @ mdkSparseMatrix::at(std::vector {RowIndexList}, {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::vector {RowIndexList}, {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at(std::vector {RowIndexList}, {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::vector<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList is empty @ mdkSparseMatrix::at(std::vector {RowIndexList}, ALL)" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::vector {RowIndexList}, ALL)" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const std::vector<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error << "RowIndexList is empty @ mdkSparseMatrix::at(std::vector {RowIndexList}, ALL) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::at(std::vector {RowIndexList}, ALL) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, RowIndexList, ALL_Symbol);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int64>& ColIndexList)
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::at(ALL, std::vector {ColIndexList})" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at(ALL, std::vector {ColIndexList})" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
const SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int64>& ColIndexList) const
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "ColIndexList is empty @ mdkSparseMatrix::at(ALL, std::vector {ColIndexList}) const" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::at(ALL, std::vector {ColIndexList}) const" << '\n';
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, ALL_Symbol, ColIndexList);

    return tempShadowMatrix;
}

// return SubMatrix as Matrix

template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetSubMatrix(const std::vector<int64>& RowIndexList, const std::vector<int64>& ColIndexList) const
{
    SparseMatrix<ElementType> tempMatrix; // empty matrix

    GetSubMatrix(tempMatrix, RowIndexList, ColIndexList);

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetSubMatrix(SparseMatrix<ElementType>& OutputMatrix,
                                          const std::vector<int64>& RowIndexList, 
                                          const std::vector<int64>& ColIndexList) const
{
    if (this == &OutputMatrix)
    {
        MDK_Error << "A Matrix tries to get a SubMatrix of itself @ mdkSparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
        return false;
    }

    auto OutputRowNumber = int64(RowIndexList.size());
    
    auto OutputColNumber = int64(ColIndexList.size());

    if (OutputRowNumber <= 0 || OutputColNumber <= 0)
    {
        MDK_Error << "RowIndexList or ColIndexList is empty @ mdkSparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Invalid ColIndexList @ mdkSparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
            return false;
        }
    }

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid RowIndexList @ mdkSparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
            return false;
        }
    }

    if (OutputMatrix.GetColNumber() != OutputColNumber || OutputMatrix.GetRowNumber() != OutputRowNumber)
    {
        if (OutputMatrix.IsSizeFixed() == false)
        {
            OutputMatrix.Clear();
            OutputMatrix.Resize(OutputRowNumber, OutputColNumber);
        }
        else
        {
            MDK_Error << "Invalid OutputMatrix @ mdkSparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)" << '\n';
            return false;
        }
    }


    //------------------------------

    return true;
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetSubMatrix(const std::vector<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    auto SelfSize = this->GetSize();

    std::vector<int64> ColIndexList(SelfSize.ColNumber);

    for (int64 i = 0; i < SelfSize.ColNumber; ++i)
    {
        ColIndexList[i] = i;
    }

    return this->GetSubMatrix(RowIndexList, ColIndexList);
}

template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetSubMatrix(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int64>& ColIndexList) const
{
    auto SelfSize = this->GetSize();

    std::vector<int64> RowIndexList(SelfSize.RowNumber);

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        RowIndexList[i] = i;
    }

    return this->GetSubMatrix(RowIndexList, ColIndexList);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Col(int64 ColIndex)
{
    return this->at(ALL, { ColIndex });
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Col(std::initializer_list<int64>& ColIndexList)
{
    return this->at(ALL, ColIndexList);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Col(const std::vector<int64>& ColIndexList)
{
    return this->at(ALL, ColIndexList);
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetCol(int64 ColIndex) const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::GetCol(ColIndex)" << '\n';
        
        return tempMatrix;
	}

    tempMatrix.Resize(SelfSize.RowNumber, 1);

//--------------------------------------

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetCol(int64 ColIndex, DenseMatrix<ElementType>& ColData) const
{
    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::GetCol(int64 ColIndex, DenseMatrix<ElementType>& ColData)" << '\n';
        return false;
    }

    ColData.resize(SelfSize.RowNumber);

    //-----------------------------------------------

    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::GetCol(int64 ColIndex, std::vector<ElementType>& ColData) const
{
    auto SelfSize = this->GetSize();

	if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::GetCol(int64 ColIndex, std::vector<ElementType>& ColData)" << '\n';
		return false;
	}

	ColData.resize(SelfSize.RowNumber);

//-----------------------------------------------

	return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::GetCol(int64 ColIndex, ElementType* ColData) const
{
    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0 || ColData == nullptr)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::GetCol(int64 ColIndex, ElementType* ColData)" << '\n';
		return false;
	}

	auto RawPointer this->GetElementPointer();

	int64 Index = ColIndex*SelfSize.RowNumber;

	for (int64 i = 0; i < SelfSize.RowNumber; ++i)
	{
		ColData[i] = RawPointer[Index + i];
	}

	return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetCol(int64 ColIndex, const SparseMatrix<ElementType_Input>& ColData)
{
    auto SelfSize = this->GetSize();

	if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error << "Invalid Input : ColIndex is out of bound @ mdkSparseMatrix::SetCol(ColIndex, mdkSparseMatrix)" << '\n';
		return false;
	}

	auto Size = ColData.GetSize();

	if (Size.ColNumber != 1 || Size.RowNumber != 1)
	{
		MDK_Error << "Invalid Input : must be a vector @ mdkSparseMatrix::SetCol(ColIndex, mdkSparseMatrix)" << '\n';
		return false;
	}

	if (Size.RowNumber > 1 && Size.RowNumber != SelfSize.RowNumber)
	{
		MDK_Error << "Invalid Input : size does not match @ mdkSparseMatrix::SetCol(ColIndex, mdkSparseMatrix)" << '\n';
		return false;
	}

	if (Size.ColNumber > 1 && Size.ColNumber != SelfSize.RowNumber)
	{
		MDK_Error << "Invalid Input : size does not match @ mdkSparseMatrix::SetCol(ColIndex, mdkSparseMatrix)" << '\n';
		return false;
	}

    return this->SetCol(ColIndex, ColData.GetElementPointer(), std::max(Size.RowNumber, Size.ColNumber));
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetCol(int64 ColIndex, const std::initializer_list<ElementType_Input>& ColData)
{
    return this->SetCol(ColIndex, ColData.begin(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetCol(int64 ColIndex, const std::vector<ElementType_Input>& ColData)
{
    return this->SetCol(ColIndex, ColData.data(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetCol(int64 ColIndex, const ElementType_Input* ColData, int64 Length)
{
    auto SelfSize = this->GetSize();

    if (ColData == nullptr || Length <= 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::SetCol(ColIndex, const ElementType_Input* ColData, int64 Length)" << '\n';
		return false;
	}
    
    auto BeginPointer = this->GetElementPointer();

    int64 Index = ColIndex*SelfSize.RowNumber;

    // just for reference:
    //
    //for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    //{
    //	RawPointer[Index + i] = ElementType(ColData[i]);
    //}

    auto tempColData = ColData;

    BeginPointer += Index;
    for (auto Ptr = BeginPointer; Ptr < BeginPointer + SelfSize.RowNumber; ++Ptr)
    {
        Ptr[0] = ElementType(tempColData[0]);

        ++tempColData;
    }

	return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::FillCol(int64 ColIndex, const ElementType& Element)
{
    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::FillCol(int64 ColIndex, const ElementType& Element)" << '\n';
        return false;
    }

    auto BeginPointer = this->GetElementPointer();

    int64 Index = ColIndex*SelfSize.RowNumber;

    BeginPointer += Index;
    for (auto Ptr = BeginPointer; Ptr < BeginPointer + SelfSize.RowNumber; ++Ptr)
    {
        Ptr[0] = Element;
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::AppendCol(const SparseMatrix<ElementType_Input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::AppendCol(const mdkSparseMatrix<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    if (ColData.IsEmpty() == true)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::AppendCol(const mdkSparseMatrix<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto RowNumber = SelfSize.RowNumber;

    if (RowNumber <= 0)
    {
        RowNumber = std::max(ColData.GetRowNumber(), ColData.GetColNumber());
    }

    this->Resize(RowNumber, SelfSize.ColNumber + 1);

    return this->SetCol(SelfSize.ColNumber, ColData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::AppendCol(const std::initializer_list<ElementType_Input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::AppendCol(const std::initializer_list<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    if (ColData.size() <= 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::AppendCol(const std::initializer_list<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  RowNumber = SelfSize.RowNumber;

    if (RowNumber <= 0)
    {
        RowNumber = ColData.size();
    }

    this->Resize(RowNumber, SelfSize.ColNumber + 1);

    return this->SetCol(SelfSize.ColNumber, ColData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::AppendCol(const std::vector<ElementType_Input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::AppendCol(const std::vector<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    if (ColData.size() <= 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::AppendCol(const std::vector<ElementType_Input>& ColData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  RowNumber = SelfSize.RowNumber;

    if (RowNumber <= 0)
    {
        RowNumber = ColData.size();
    }

    this->Resize(RowNumber, SelfSize.ColNumber + 1);

    return this->SetCol(SelfSize.ColNumber, ColData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::AppendCol(const ElementType_Input* ColData, int64 Length)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::AppendCol(const ElementType_Input* ColData, int64 Length)" << '\n';
        return false;
    }

    if (ColData == nullptr || Length <= 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::AppendCol(const ElementType_Input* ColData, int64 Length)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  RowNumber = SelfSize.RowNumber;

    if (RowNumber <= 0)
    {
        RowNumber = Length;
    }

    this->Resize(RowNumber, SelfSize.ColNumber + 1);

    return this->SetCol(SelfSize.ColNumber, ColData, Length);
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::DeleteCol(int64 ColIndex)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::DeleteCol(int64 ColIndex)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::DeleteCol(int64 ColIndex)" << '\n';
        return false;
    }

    m_MatrixData->DataArray.erase(m_MatrixData->DataArray.begin() + ColIndex*SelfSize.RowNumber, 
                                  m_MatrixData->DataArray.begin() + (ColIndex + 1)*SelfSize.RowNumber);

    m_MatrixData->ColNumber -= 1;

    m_ElementPointer = m_MatrixData->DataArray.data();

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteCol(std::initializer_list<int64>& ColIndexList)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::DeleteCol(std::initializer_list<int64>& ColIndexList)" << '\n';
        return false;
    }

    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::DeleteCol(std::initializer_list ColIndexList)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Out of bound Input @ mdkSparseMatrix::DeleteCol(std::initializer_list ColIndexList)" << '\n';
            return false;
        }
    }

    return this->DeleteCol(ColIndexList.begin(), ColIndexList.size());
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteCol(const std::vector<int64>& ColIndexList)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::DeleteCol(const std::vector<int64>& ColIndexList)" << '\n';
        return false;
    }

    if (ColIndexList.size() <= 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::DeleteCol(std::vector ColIndexList)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error << "Out of bound Input @ mdkSparseMatrix::DeleteCol(std::vector ColIndexList)" << '\n';
            return false;
        }
    }

    return this->DeleteCol(ColIndexList.data(), ColIndexList.size());
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteCol(const int64* ColIndexListPtr, int64 Length)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::DeleteCol(const int64* ColIndexPtr, int64 Length)" << '\n';
        return false;
    }

    if (ColIndexListPtr == nullptr || Length <= 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::DeleteCol(const int64* ColIndexPtr, int64 Length)" << '\n';
        return false;
    }

    if (Length == 1)
    {
        return this->DeleteCol(ColIndexListPtr[0]);
    }

    // Length > 1 -------------------------------------------

    auto SelfSize = this->GetSize();

    std::vector<int64> ColIndexList_max_to_min(Length);

    for (int64 i = 0; i < Length; ++i)
    {
        ColIndexList_max_to_min[i] = ColIndexListPtr[i];
    }

    std::sort(ColIndexList_max_to_min.begin(), ColIndexList_max_to_min.end(), [](int64 a, int64 b) { return a > b; });

    int64 Index_prev = ColIndexList_max_to_min[0] + 1;

    for (int64 i = 0; i < int64(ColIndexList_max_to_min.size()); ++i)
    {
        auto Index_i = ColIndexList_max_to_min[i];

        if (Index_i == Index_prev)
        {
            mdkWarning << "duplicate Input @ mdkSparseMatrix::DeleteCol(const int64* ColIndexPtr, int64 Length)" << '\n';
        }
        else
        {
            m_MatrixData->DataArray.erase(m_MatrixData->DataArray.begin() + Index_i * SelfSize.RowNumber,
                                          m_MatrixData->DataArray.begin() + (Index_i + 1)* SelfSize.RowNumber);

            Index_prev = Index_i;

            m_MatrixData->ColNumber -= 1;
        }
    }

    m_ElementPointer = m_MatrixData->DataArray.data();

    return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int64 ColIndex, const SparseMatrix<ElementType_Input>& ColData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)" << '\n';
        return false;
    }

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input : ColIndex is out of bound @ mdkSparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)" << '\n';
        return false;
    }

    auto InputSize = ColData.GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
    {
        MDK_Error << "Invalid Input : must be a vector @ mdkSparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.RowNumber)
    {
        MDK_Error << "Invalid Input : size does not match @ mdkSparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)" << '\n';
        return false;
    }

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.RowNumber)
    {
        MDK_Error << "Invalid Input : size does not match @ mdkSparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)" << '\n';
        return false;
    }

    return this->InsertCol(ColIndex, ColData.GetElementPointer(), std::max(InputSize.RowNumber, InputSize.ColNumber));

}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int64 ColIndex, const std::initializer_list<ElementType_Input>& ColData)
{
    return this->InsertCol(ColIndex, ColData.begin(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int64 ColIndex, const std::vector<ElementType_Input>& ColData)
{
    return this->InsertCol(ColIndex, ColData.data(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int64 ColIndex, const ElementType_Input* ColData, int64 Length)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Matrix Size can not change @ mdkSparseMatrix::InsertCol(ColIndex, const ElementType_Input* ColData, int64 Length)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (ColData == nullptr || Length <= 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::InsertCol(ColIndex, const ElementType_Input* ColData, int64 Length)" << '\n';
        return false;
    }

    m_MatrixData->DataArray.insert(m_MatrixData->DataArray.begin() + ColIndex*SelfSize.RowNumber, ColData, ColData + Length);

    m_MatrixData->ColNumber += 1;

    m_ElementPointer = m_MatrixData->DataArray.data();

    return true;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Row(int64 RowIndex)
{
    return this->at({RowIndex}, ALL);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Row(std::initializer_list<int64>& RowIndexList)
{
    return this->at(RowIndexList, ALL);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Row(const std::vector<int64>& RowIndexList)
{
    return this->at(RowIndexList, ALL);
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetRow(int64 RowIndex) const
{
    SparseMatrix<ElementType> tempMatrix;	

    auto SelfSize = this->GetSize();

	if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::GetRow(RowIndex)" << '\n';
        
        return tempMatrix;
	}

    tempMatrix.Resize(1, SelfSize.ColNumber);

    auto tempRawPointer = tempMatrix.GetElementPointer();

    auto RawPointer = this->GetElementPointer();

	int64 Index = 0;

	for (int64 j = 0; j < SelfSize.ColNumber; ++j)
	{
		tempRawPointer[j] = RawPointer[Index + RowIndex];

		Index += SelfSize.RowNumber;
	}

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetRow(int64 RowIndex, std::vector<ElementType>& RowData) const
{
    auto SelfSize = this->GetSize();

	if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::GetRow(RowIndex, std::vector<ElementType>& RowData)" << '\n';
		return false;
	}

	RowData.resize(SelfSize.ColNumber);

	auto tempRawPointer = RowData.data();

    auto RawPointer = this->GetElementPointer();

	int64 Index = 0;

	for (int64 j = 0; j < SelfSize.ColNumber; ++j)
	{
		tempRawPointer[j] = RawPointer[Index + RowIndex];

		Index += SelfSize.RowNumber;
	}

	return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetRow(int64 RowIndex, ElementType* RowData) const
{
    auto SelfSize = this->GetSize();

	if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix GetRow(RowIndex, ElementType* RowData)" << '\n';
		return false;
	}

	auto RawPointer this->GetElementPointer();

	int64 Index = 0;

	for (int64 j = 0; j < SelfSize.ColNumber; ++j)
	{
		RowData[j] = RawPointer[Index + RowIndex];

		Index += SelfSize.RowNumber;
	}

	return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetRow(int64 RowIndex, const SparseMatrix<ElementType_Input>& RowData)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix SetRow(int64 RowIndex, const mdkSparseMatrix<ElementType_Input>& RowData)" << '\n';
        return false;
    }

	auto InputSize = RowData.GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix SetRow(RowIndex,mdkSparseMatrix)" << '\n';
		return false;
	}

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.ColNumber)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix SetRow(RowIndex,mdkSparseMatrix)" << '\n';
		return false;
	}

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.ColNumber)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix SetRow(RowIndex,mdkSparseMatrix)" << '\n';
		return false;
	}

    return this->SetRow(RowIndex, RowData.GetElementPointer(), std::max(InputSize.RowNumber, InputSize.ColNumber));
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetRow(int64 RowIndex, const std::initializer_list<ElementType_Input>& RowData)
{
    return this->SetRow(RowIndex, RowData.begin(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetRow(int64 RowIndex, const std::vector<ElementType_Input>& RowData)
{
    return this->SetRow(RowIndex, RowData.data(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetRow(int64 RowIndex, const ElementType_Input* RowData, int64 Length)
{
    auto SelfSize = this->GetSize();

    if (RowData == nullptr || Length <= 0 || RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::SetRow(RowIndex, const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    if (Length != SelfSize.ColNumber)
    {
        MDK_Error << "Length does not match @ mdkSparseMatrix::SetRow(RowIndex, const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    auto RawPointer = this->GetElementPointer();

    int64 Index = 0;

    for (int64 j = 0; j < SelfSize.ColNumber; ++j)
    {
        RawPointer[Index + RowIndex] = ElementType(RowData[j]);

        Index += SelfSize.RowNumber;
     }

    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::FillRow(int64 RowIndex, const ElementType& Element)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::FillRow(int64 RowIndex, const ElementType& Element)" << '\n';
        return false;
    }

    auto RawPointer = this->GetElementPointer();

    int64 Index = 0;

    for (int64 j = 0; j < SelfSize.ColNumber; ++j)
    {
        RawPointer[Index + RowIndex] = Element;

        Index += SelfSize.RowNumber;
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::AppendRow(const SparseMatrix<ElementType_Input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::AppendRow(const mdkSparseMatrix<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    if (RowData.IsEmpty() == true)
    {
        MDK_Error << "Input is empty @ mdkSparseMatrix::AppendRow(const mdkSparseMatrix<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  ColNumber = SelfSize.ColNumber;

    if (ColNumber <= 0)
    {
        ColNumber = std::max(RowData.GetRowNumber(), RowData.GetColNumber);
    }

    this->Resize(SelfSize.RowNumber + 1, ColNumber);

    return this->SetRow(SelfSize.RowNumber, RowData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool  SparseMatrix<ElementType>::AppendRow(const std::initializer_list<ElementType_Input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::AppendRow(const std::initializer_list<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    if (RowData.size() <= 0)
    {
        MDK_Error << "Input is empty @ mdkSparseMatrix::AppendRow(const std::initializer_list<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  ColNumber = SelfSize.ColNumber;

    if (ColNumber <= 0)
    {
        ColNumber = RowData.size();
    }

    this->Resize(SelfSize.RowNumber + 1, ColNumber);

    return this->SetRow(SelfSize.RowNumber, RowData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool  SparseMatrix<ElementType>::AppendRow(const std::vector<ElementType_Input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::AppendRow(const std::vector<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    if (RowData.size() <= 0)
    {
        MDK_Error << "Input is empty @ mdkSparseMatrix::AppendRow(const std::vector<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  ColNumber = SelfSize.ColNumber;

    if (ColNumber <= 0)
    {
        ColNumber = RowData.size();
    }

    this->Resize(SelfSize.RowNumber + 1, ColNumber);

    return this->SetRow(SelfSize.RowNumber, RowData);
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::AppendRow(const ElementType_Input* RowData, int64 Length)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::AppendRow(const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    if (RowData == nullptr || Length <= 0)
    {
        MDK_Error << "Input is empty @ mdkSparseMatrix::AppendRow(const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    auto  ColNumber = SelfSize.ColNumber;

    if (ColNumber <= 0)
    {
        ColNumber = RowData.size();
    }

    this->Resize(SelfSize.RowNumber + 1, ColNumber);

    return this->SetRow(SelfSize.RowNumber, RowData, Length);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteRow(int64 RowIndex)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::DeleteRow(int64 RowIndex)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::DeleteRow(int64 RowIndex)" << '\n';
        return false;
    }

    return this->DeleteRow(&RowIndex, 1);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteRow(const std::vector<int64>& RowIndexList)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::DeleteRow(const std::vector<int64>& RowIndexList)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error << "Invalid Input @ mdkSparseMatrix::DeleteRow(const std::vector<int64>& RowIndexList)" << '\n';
            return false;
        }
    }

    return this->DeleteRow(RowIndexList.data(), RowIndexList.size());
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteRow(const int64* RowIndexListPtr, int64 Length)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::DeleteRow(const int64* RowIndexPtr, int64 Length)" << '\n';
        return false;
    }

    if (RowIndexListPtr == nullptr || Length <= 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::DeleteRow(const int64* RowIndexPtr, int64 Length)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    std::vector<int64> CounterList(SelfSize.RowNumber);
    
    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        CounterList[i] = 0;
    }

    for (int64 i = 0; i < Length; ++i)
    {
        CounterList[RowIndexListPtr[i]] = 1;
    }

    std::vector<int64> RowIndexList_output;

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        if (CounterList[i] <= 0)
        {
            RowIndexList_output.push_back(i);
        }
    }

    if (RowIndexList_output.size() > 0)
    {
        this->Take(this->GetSubMatrix(RowIndexList_output, ALL));
    }
    else
    {
        this->Reset();
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int64 RowIndex, const SparseMatrix<ElementType_Input>& RowData)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::InsertRow(int64 RowIndex, const mdkSparseMatrix<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix InsertRow(int64 RowIndex, const mdkSparseMatrix<ElementType_Input>& RowData)" << '\n';
        return false;
    }

    auto InputSize = RowData.GetSize();

    auto SelfSize = this->GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix InsertRow(RowIndex,mdkSparseMatrix)" << '\n';
        return false;
    }

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix InsertRow(RowIndex,mdkSparseMatrix)" << '\n';
        return false;
    }

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.ColNumber)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix InsertRow(RowIndex,mdkSparseMatrix)" << '\n';
        return false;
    }

    return this->InsertRow(RowIndex, RowData.GetElementPointer(), std::max(Size.RowNumber, Size.ColNumber));
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int64 RowIndex, const std::initializer_list<ElementType_Input>& RowData)
{
    return this->InsertRow(RowIndex, RowData.begin(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int64 RowIndex, const std::vector<ElementType_Input>& RowData)
{
    return this->InsertRow(RowIndex, RowData.data(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int64 RowIndex, const ElementType_Input* RowData, int64 Length)
{
    if (m_IsSizeFixed == true)
    {
        MDK_Error << "Size can not change @ mdkSparseMatrix::InsertRow(RowIndex, const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (RowData == nullptr || Length <= 0 || RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error << "Empty Input @ mdkSparseMatrix::InsertRow(RowIndex, const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    if (Length != SelfSize.ColNumber)
    {
        MDK_Error << "Length does not match @ mdkSparseMatrix::InsertRow(RowIndex, const ElementType_Input* RowData, int64 Length)" << '\n';
        return false;
    }

    SparseMatrix<ElementType> tempMatrix(SelfSize.RowNumber + 1, SelfSize.ColNumber);

    auto tempRawPointer = tempMatrix.GetElementPointer();

    auto RawPointer = this->GetElementPointer();

    for (int64 i = 0; i < RowIndex; ++i)
    {
        for (int64 j = 0; j < SelfSize.ColNumber; ++j)
        {
            tempRawPointer[j*(SelfSize.RowNumber + 1) + i] = RawPointer[j*SelfSize.RowNumber + i];
        }
    }

    for (int64 j = 0; j < SelfSize.ColNumber; ++j)
    {
        tempRawPointer[j*(SelfSize.RowNumber + 1) + RowIndex] = ElementType(RowData[j]);
    }

    for (int64 i = RowIndex + 1; i < SelfSize.RowNumber + 1; ++i)
    {
        for (int64 j = 0; j < SelfSize.ColNumber; ++j)
        {
            tempRawPointer[j*(SelfSize.RowNumber + 1) + i] = RawPointer[j*SelfSize.RowNumber + i - 1];
        }
    }

    this->Take(tempMatrix);

    return true;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Diangonal()
{
    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << " Self is empty or not square @ mdkSparseMatrix::Diangonal" << '\n';
        SparseShadowMatrix<ElementType> tempShadowMatrix;     
        return  tempShadowMatrix;
    }

    std::vector<int64> LinearIndexList(SelfSize.ColNumber);

    int64 Index = 0;

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        LinearIndexList[j] = Index + i;

        Index += SelfSize.RowNumber;
    }

    SparseShadowMatrix<ElementType> tempShadowMatrix(*this, LinearIndexList);

    return tempShadowMatrix;
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetDiangonal() const
{
    SparseMatrix<ElementType> tempMatrix; 

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << " Self is empty or not square @ mdkSparseMatrix::GetDiangonal()" << '\n';
        
        return  tempMatrix;
    }

    tempMatrix.Resize(SelfSize.RowNumber, 1);

    auto tempRawPointer = tempMatrix.GetElementPointer();

    this->GetDiangonal(tempRawPointer);

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetDiangonal(std::vector<ElementType>& DiangonalData) const
{
    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << " Self is empty or not square @ mdkSparseMatrix::GetDiangonal(std::vector)" << '\n';
        return  false;
    }

    DiangonalData.resize(SelfSize.RowNumber);

    auto outputRawPointer = DiangonalData.data();

    return this->GetDiangonal(outputRawPointer);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetDiangonal(ElementType* DiangonalData) const
{
    if (DiangonalData == nullptr)
    {
        MDK_Error << " Input is nullptr @ mdkSparseMatrix::GetDiangonal(ElementType* DiangonalData)" << '\n';
        return  false;
    }

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << " Self is empty or not square @ mdkSparseMatrix::GetDiangonal(ElementType* DiangonalData)" << '\n';
        return  false;
    }

    auto RawPointer this->GetElementPointer();

    int64 Index = 0;

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        DiangonalData[j] = RawPointer[Index + i];

        Index += SelfSize.RowNumber;
    }

    return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetDiangonal(const SparseMatrix<ElementType_Input>& DiangonalData)
{
	auto InputSize = DiangonalData.GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::SetDiangonal(Matrix)" << '\n';
		return false;
	}

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.ColNumber)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::SetDiangonal(Matrix)" << '\n';
		return false;
	}

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.ColNumber)
	{
		MDK_Error << "Invalid Input @ mdkSparseMatrix::SetDiangonal(Matrix)" << '\n';
		return false;
	}

    if (InputSize.RowNumber == 1 && InputSize.ColNumber == 1)
    {
        mdkWarning << "Input matrix is 1x1 and treated as a scalar @ mdkSparseMatrix::SetDiangonal(Matrix)" << '\n';
        return this->FillDiangonal(ElementType(DiangonalData(0)));
    }
    else
    {
        return this->SetDiangonal(DiangonalData.GetElementPointer());
    }
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetDiangonal(const std::initializer_list<ElementType_Input>& DiangonalData)
{
    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << "Self is empty or not square @ mdkSparseMatrix::SetDiangonal(std::initializer_list)" << '\n';
        return false;
    }

    if (DiangonalData.size() != SelfSize.RowNumber)
    {
        MDK_Error << "Invalid Input @ mdkSparseMatrix::SetDiangonal(std::initializer_list)" << '\n';
        return false;
    }

    return this->SetDiangonal(DiangonalData.begin());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetDiangonal(const std::vector<ElementType_Input>& DiangonalData)
{
    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
	{
		MDK_Error << " Self is empty or not square @ mdkSparseMatrix::SetDiangonal(std::vector)" << '\n';
		return false;
	}

    if (DiangonalData.size() != SelfSize.RowNumber)
    {
        MDK_Error << " Input is invalid @ mdkSparseMatrix::SetDiangonal(std::vector)" << '\n';
        return false;
    }

	return this->SetDiangonal(DiangonalData.data());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetDiangonal(const ElementType_Input* DiangonalData)
{
    if (DiangonalData == nullptr)
    {
        MDK_Error << " Input is nullptr @ mdkSparseMatrix::SetDiangonal(const ElementType_Input* DiangonalData)" << '\n';
        return false;
    }

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << " Self is empty or not square @ mdkSparseMatrix::SetDiangonal(const ElementType_Input* DiangonalData)" << '\n';
        return false;
    }

    auto RawPointer = this->GetElementPointer();

    int64 Index = 0;

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        RawPointer[Index + i] = ElementType(DiangonalData[i]);

        Index += SelfSize.RowNumber;
    }

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::FillDiangonal(const ElementType& Element)
{
    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error << " Self is empty or not square @ mdkSparseMatrix::FillDiangonal" << '\n';
        return false;
    }

    auto RawPointer this->GetElementPointer();

    int64 Index = 0;

    for (int64 i = 0; i < SelfSize.RowNumber; ++i)
    {
        RawPointer[Index + i] = Element;

        Index += SelfSize.RowNumber;
    }

    return true;
}


//=========================================== For Reference ===============================================================//
/*

// --------------------------------------- Matrix {+ - * /}  Matrix ------------------------------------------------//

template<typename ElementType>
inline	
mdkSparseMatrix<ElementType> operator+(const mdkSparseMatrix<ElementType>& MatrixA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixAdd(MatrixA, MatrixB);
}


template<typename ElementType>
inline
mdkSparseMatrix<ElementType> operator-(const mdkSparseMatrix<ElementType>& MatrixA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixSubtract(MatrixA, MatrixB);
}


template<typename ElementType>
inline
mdkSparseMatrix<ElementType> operator*(const mdkSparseMatrix<ElementType>& MatrixA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixMultiply(MatrixA, MatrixB);
}


template<typename ElementType>
inline
mdkSparseMatrix<ElementType> operator/(const mdkSparseMatrix<ElementType>& MatrixA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixElementDivide(MatrixA, MatrixB);
}

// ----------------------- Matrix {+ - * /}  Element ------------------------------------------------//

template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator+(const mdkSparseMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    return MatrixAdd(MatrixA, ElementB);
}


template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator-(const mdkSparseMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    return MatrixSubtract(MatrixA, ElementB);
}


template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator*(const mdkSparseMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    return MatrixMultiply(MatrixA, ElementB);
}


template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator/(const mdkSparseMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    return MatrixElementDivide(MatrixA, ElementB);
}

// ----------------------- Element {+ - * /} Matrix ------------------------------------------------//

template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator+(const ElementType& ElementA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixAdd(ElementA, MatrixB);
}


template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator-(const ElementType& ElementA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixSubtract(ElementA, MatrixB);
}


template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator*(const ElementType& ElementA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixMultiply(ElementA, MatrixB);
}


template<typename ElementType>
inline mdkSparseMatrix<ElementType> operator/(const ElementType& ElementA, const mdkSparseMatrix<ElementType>& MatrixB)
{
    return MatrixElementDivide(ElementA, MatrixB);
}

*/
//===========================================================================================================================//


//---------------------- Matrix {+= -= *= /=} Matrix ----------------------------------------//

template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator+=(const SparseMatrix<ElementType>& InputMatrix)
{
    MatrixAdd(*this, *this, InputMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator-=(const SparseMatrix<ElementType>& InputMatrix)
{
    MatrixSubtract(*this, *this, InputMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator*=(const SparseMatrix<ElementType>& InputMatrix)
{
    MatrixMultiply(*this, *this, InputMatrix);
}


//---------------------- Matrix {+= -= *= /=} mdkSparseShadowMatrix ----------------------------------------//

template<typename ElementType>
inline 
void SparseMatrix<ElementType>::operator+=(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    auto InputSize = ShadowMatrix.GetSize();

    if (InputSize.RowNumber <= 0)
    {
        MDK_Error << "ShadowMatrix is empty @ mdkSparseMatrix::operator+=(ShadowMatrix)" << '\n';
        return;
    }

    if (InputSize.RowNumber == 1 && InputSize.ColNumber == 1)
    {
        (*this) += ShadowMatrix[0];

        return;
    }

    auto SelfSize = this->GetSize();

    auto Self_ElementNumber = SelfSize.RowNumber * SelfSize.ColNumber;

    if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
    {
        MDK_Error << "Size does not match @ mdkSparseMatrix::operator+=(ShadowMatrix)" << '\n';
        return;
    }

//----------------

}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator-=(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    auto InputSize = ShadowMatrix.GetSize();

    if (InputSize.RowNumber <= 0)
    {
        MDK_Error << "ShadowMatrix is empty @ mdkSparseMatrix::operator-=(ShadowMatrix)" << '\n';
        return;
    }

    if (InputSize.RowNumber == 1 && InputSize.ColNumber == 1)
    {
        (*this) -= ShadowMatrix[0];

        return;
    }

    auto SelfSize = this->GetSize();

    auto Self_ElementNumber = SelfSize.RowNumber * SelfSize.ColNumber;

    if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
    {
        MDK_Error << "Size does not match @ mdkSparseMatrix::operator-=(ShadowMatrix)" << '\n';
        return;
    }

    //----------------
}


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::operator*=(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    this->operator*=(SparseShadowMatrix.CreateMatrix());
}

//---------------------- SparseMatrix {+= -= *=} SparseGlueMatrixForLinearCombination ----------------------------------------//

template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator+=(SparseGlueMatrixForLinearCombination<ElementType> GlueMatrix)
{
    (*this) = (*this) + std::move(GlueMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator-=(SparseGlueMatrixForLinearCombination<ElementType> GlueMatrix)
{
    (*this) = (*this) - std::move(GlueMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator*=(const SparseGlueMatrixForLinearCombination<ElementType>& GlueMatrix)
{
    this->operator*=(GlueMatrix.CreateMatrix());
}


//---------------------- SparseMatrix {+= -= *=} SparseGlueMatrixForMultiplication ----------------------------------------//

template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator+=(SparseGlueMatrixForMultiplication<ElementType> GlueMatrix)
{
    (*this) = (*this) + std::move(GlueMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator-=(SparseGlueMatrixForMultiplication<ElementType> GlueMatrix)
{
    (*this) = (*this) - std::move(GlueMatrix);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator*=(SparseGlueMatrixForMultiplication<ElementType> GlueMatrix)
{
    (*this) = (*this) * std::move(GlueMatrix);
}

//---------------------- SparseMatrix {+= -= *= /=} Element ----------------------------------------//

template<typename ElementType>
inline	
void SparseMatrix<ElementType>::operator+=(const ElementType& Element)
{
    MatrixAdd(*this, *this, Element);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator-=(const ElementType& Element)
{
    MatrixSubtract(*this, *this, Element);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator*=(const ElementType& Element)
{
    MatrixElementMultiply(*this, *this, Element);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator/=(const ElementType& Element)
{
    MatrixElementDivide(*this, *this, Element);
}

//-------------------- element operation {^} -----------------------------------------------------------//

template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::operator^(const ElementType& Element)
{
    SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
    {
        MDK_Error << "Self is empty @ mdkSparseMatrix::operator^(value)" << '\n';

        return tempMatrix;
    }

    tempMatrix.Copy(*this);

    auto tempRawPointer = tempMatrix.GetNonZeroElementPointer();

    auto RawPointer = this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInDataArray();

    for (int64 i = 0; i < RecordedElementNumber; ++i)
    {
        tempRawPointer[i] = std::pow(RawPointer[i], Element);
    }

    return tempMatrix;
}

//-------------------- element operation {^=} -----------------------------------------------------------//

template<typename ElementType>
inline void SparseMatrix<ElementType>::operator^=(const ElementType& Element)
{
    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
    {
        MDK_Error << "Self is empty @ mdkSparseMatrix::operator^(value)" << '\n';
        return;
    }

    auto BeginPointer this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInDataArray();

    for (auto Ptr = BeginPointer; Ptr < BeginPointer + RecordedElementNumber; ++Ptr)
    {
        Ptr[0] = std::pow(Ptr[0], Element);
    }
}


//-------------------- special element operation ( .* in matlab ) ------------------------------------------//

template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const ElementType& Element) const
{
    return MatrixElementMultiply(*this, Element);
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const SparseMatrix<ElementType>& InputSparseMatrix) const
{
    return MatrixElementMultiply(*this, InputSparseMatrix);

}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const SparseShadowMatrix<ElementType>& ShadowMatrix) const
{
    return MatrixElementMultiply(*this, ShadowMatrix.CreateSparseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const SparseGlueMatrixForLinearCombination<ElementType>& GlueMatrix) const
{
    return MatrixElementMultiply(*this, GlueMatrix.CreateSparseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const SparseGlueMatrixForMultiplication<ElementType>& GlueMatrix) const
{
    return MatrixElementMultiply(*this, GlueMatrix.CreateSparseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const DenseMatrix<ElementType>& InputDenseMatrix) const
{
    return MatrixElementMultiply(*this, InputDenseMatrix);

}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const DenseShadowMatrix<ElementType>& DenseShadowMatrix) const
{
    return MatrixElementMultiply(*this, DenseShadowMatrix.CreateDenseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const DenseGlueMatrixForLinearCombination<ElementType>& DenseGlueMatrix) const
{
    return MatrixElementMultiply(*this, DenseGlueMatrix.CreateDenseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const DenseGlueMatrixForMultiplication<ElementType>& DenseGlueMatrix) const
{
    return MatrixElementMultiply(*this, DenseGlueMatrix.CreateDenseMatrix());
}


//-------------------- general element operation : output a new matrix ------------------------------------------//

template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(const char* OperationName) const
{
    return MatrixElementOperation(std::string(OperationName), *this);
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(const std::string& OperationName) const
{
    return MatrixElementOperation(OperationName, *this);
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(std::function<ElementType(const ElementType&)> Operation) const
{
    return MatrixElementOperation(Operation, *this);
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(const std::string& OperationName, const SparseMatrix<ElementType>& InputMatrix) const
{
    return MatrixElementOperation(OperationName, *this, InputMatrix);
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(std::function<ElementType(const ElementType&, const ElementType&)> Operation, 
                                                                            const SparseMatrix<ElementType>& InputMatrix) const
{
    return MatrixElementOperation(Operation, *this, InputMatrix);
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(const char* OperationName, const ElementType& Element) const
{
    return MatrixElementOperation(std::string(OperationName), *this, Element);
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(const std::string& OperationName, const ElementType& Element) const
{
    return MatrixElementOperation(OperationName, *this, Element);
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementOperation(std::function<ElementType(const ElementType&, const ElementType&)> Operation, 
                                                                            const ElementType& Element) const
{
    return MatrixElementOperation(Operation, *this, Element);
}


//-------------------- element operation in place : Object.ElementOperationInPlace update the current data of the object ---------------//

template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(const char* OperationName)
{
    return MatrixElementOperation(*this, std::string(OperationName), *this);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(const std::string& OperationName)
{
    return MatrixElementOperation(*this, OperationName, *this);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(std::function<ElementType(const ElementType&)> Operation)
{
    return MatrixElementOperation(*this, Operation, *this);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(const char* OperationName, const SparseMatrix<ElementType>& InputMatrix)
{
    return MatrixElementOperation(*this, std::string(OperationName), *this, InputMatrix);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(const std::string& OperationName, const SparseMatrix<ElementType>& InputMatrix)
{
    return MatrixElementOperation(*this, OperationName, *this, InputMatrix);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(std::function<ElementType(const ElementType&, const ElementType&)> Operation,
                                                           const SparseMatrix<ElementType>& InputMatrix)
{
    return MatrixElementOperation(*this, Operation, *this, InputMatrix);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(const char* OperationName, const ElementType& Element)
{
    return MatrixElementOperation(*this, std::string(OperationName), *this, Element);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(const std::string& OperationName, const ElementType& Element)
{
    return MatrixElementOperation(*this, OperationName, *this, Element);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ElementOperationInPlace(std::function<ElementType(const ElementType&, const ElementType&)> Operation, const ElementType& Element)
{
    return MatrixElementOperation(*this, Operation, *this, Element);
}

//-----------------------------------------------------------------------------------------------------------//


template<typename ElementType>
inline ElementType SparseMatrix<ElementType>::Mean() const
{
    auto Self_ElementNumber = this->GetElementNumber();

    if (Self_ElementNumber <= 0)
    {
        MDK_Error << "self is empty Matrix @ mdkSparseMatrix::Mean" << '\n';
        return m_NaNElement;
    }

    auto RawPointer = this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInDataArray();

    ElementType value = RawPointer[0];

    for (auto Ptr = RawPointer + 1; Ptr < RawPointer + RecordedElementNumber; ++Ptr)
    {
        value += Ptr[0];
    }

    value /= Self_ElementNumber;

    return value;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::MeanToRow() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::MeanToRow" << '\n';

		return tempMatrix;
	}

//-------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::MeanToCol() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::MeanToCol" << '\n';
  
		return tempMatrix;
	}

    //-------------------------------------
	return tempMatrix;
}


template<typename ElementType>
inline ElementType SparseMatrix<ElementType>::Sum() const
{
    auto Self_ElementNumber = this->GetElementNumber();

    if (Self_ElementNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::Sum" << '\n';
        return m_NaNElement;
	}

    auto RawPointer = this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInDataArray();

    ElementType value = RawPointer[0];

    for (auto Ptr = RawPointer + 1; Ptr < RawPointer + RecordedElementNumber; ++Ptr)
    {
        value += Ptr[0];
    }

	return value;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::SumToRow() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::SumToRow" << '\n';
        
		return tempMatrix;
	}

	//-------------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::SumToCol() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::SumToCol" << '\n';
        
		return tempMatrix;
	}

    //-------------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline ElementType SparseMatrix<ElementType>::Max() const
{
    auto RecordedElementNumber = this->GetElementNumberInDataArray();

    if (RecordedElementNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::Max" << '\n';
        return m_NaNElement;
	}

    auto RawPointer = this->GetNonZeroElementPointer();

	ElementType value = RawPointer[0];

    for (auto Ptr = RawPointer + 1; Ptr < RawPointer + RecordedElementNumber; ++Ptr)
	{
        value = std::max(value, Ptr[0]);
	}

	return value;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::MaxToRow() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::MaxToRow" << '\n';
        
		return tempMatrix;
	}

//--------------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::MaxToCol() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::MaxToCol" << '\n';
        
		return tempMatrix;
	}

//------------------------------------------------

	return tempMatrix;
}


template<typename ElementType>
inline ElementType SparseMatrix<ElementType>::Min() const
{
    auto RecordedElementNumber = this->GetElementNumberInDataArray();

    if (RecordedElementNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::Min" << '\n';
        return m_NaNElement;
	}

    auto RawPointer = this->GetNonZeroElementPointer();

    ElementType value = RawPointer[0];

    for (auto Ptr = RawPointer + 1; Ptr < RawPointer + RecordedElementNumber; ++Ptr)
    {
        value = std::max(value, Ptr[0]);
    }

	return value;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::MinToRow() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::MinToRow" << '\n';
        
		return tempMatrix;
	}

	//----------------------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline SparseMatrix<ElementType> SparseMatrix<ElementType>::MinToCol() const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0)
	{
		MDK_Error << "self is empty Matrix @ mdkSparseMatrix::MinToCol" << '\n';
        
		return tempMatrix;
	}

	//--------------------------------------------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::Transpose() const
{
    return MatrixTranspose(*this);
}


template<typename ElementType>
inline
int64 SparseMatrix<ElementType>::Rank() const
{
    return MatrixRank(*this);
}


template<typename ElementType>
inline 
DenseMatrix<ElementType> SparseMatrix<ElementType>::Inv() const
{
    return MatrixInv(*this);
}


template<typename ElementType>
inline
DenseMatrix<ElementType> SparseMatrix<ElementType>::PseudoInv() const
{
	// call Armadillo 


}


template<typename ElementType>
inline
SparseMatrixSVDResult<ElementType> SparseMatrix<ElementType>::SVD() const
{
    return MatrixSVD(*this);
}

}//end namespace mdk

#endif