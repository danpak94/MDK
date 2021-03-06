﻿#pragma once


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
void SparseMatrixDataInCSCFormat<ElementType>::Construct(int_max InputRowNumber, int_max InputColNumber)
{
    //----------------------------------------------------------

    m_RowNumber = InputRowNumber;

    m_ColNumber = InputColNumber;

    m_RowIndexList.resize(0);

    m_ElementList.resize(0);

    m_RecordedElementNumberInEachCol.resize(InputColNumber);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_RecordedElementNumberInEachCol[i] = 0;
    }

    m_ColBeginElementIndexInElementList.resize(InputColNumber + 1);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_ColBeginElementIndexInElementList[i] = -1;
    }

    m_ColBeginElementIndexInElementList[InputColNumber] = 0;
}


template<typename ElementType>
inline
void SparseMatrixDataInCSCFormat<ElementType>::Construct(const int_max* InputRowIndexList,
                                                         const int_max* InputColIndexList,
                                                         const ElementType* InputElementList,
                                                         int_max RecordedElementNumber,
                                                         int_max InputRowNumber,
                                                         int_max InputColNumber,
                                                         int_max AdditionalReservedCapacity = 0)
{
    //--------------------------------------------------------------

    m_RowNumber = InputRowNumber;

    m_ColNumber = InputColNumber;

    //--------------------------------------------------------------

    m_RowIndexList.resize(0);

    m_RowIndexList.reserve(RecordedElementNumber + AdditionalReservedCapacity);

    m_ElementList.resize(0);

    m_ElementList.reserve(RecordedElementNumber + AdditionalReservedCapacity);

    //--------------------------------------------------------------

    m_ColBeginElementIndexInElementList.resize(InputColNumber + 1);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_ColBeginElementIndexInElementList[i] = -1;
    }

    m_ColBeginElementIndexInElementList[InputColNumber] = RecordedElementNumber; // end of the record

    //----------------------------------------------------------------

    m_RecordedElementNumberInEachCol.resize(InputColNumber);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_RecordedElementNumberInEachCol[i] = 0;
    }

    //--------------------------------------------------------------

    std::vector<int_max> InputColIndexList_sort(RecordedElementNumber);

    std::vector<int_max> LinearIndex_In_InputColIndexList(RecordedElementNumber);

    //sort tempColIndexList in ascending order

    Sort(InputColIndexList, RecordedElementNumber, InputColIndexList_sort.data(), LinearIndex_In_InputColIndexList.data(), "ascend");

    //--------------------------------------------------------------

    std::vector<int_max> RowIndexSubList;

    RowIndexSubList.reserve(RecordedElementNumber);

    std::vector<int_max> LinearIndex_In_RowIndexSubList;

    LinearIndex_In_RowIndexSubList.reserve(RecordedElementNumber);

    //----------------------------------------------------------------

    m_ColBeginElementIndexInElementList[InputColIndexList_sort[0]] = 0;
    
    m_RecordedElementNumberInEachCol[InputColIndexList_sort[0]] = 1;

    for (int_max i = 0; i < RecordedElementNumber; ++i)
    {
        int_max CurrentColIndex = InputColIndexList_sort[i];

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

            auto RecordedElementNumberInPerviousCol = int_max(RowIndexSubList.size());

            Sort(RowIndexSubList, RowIndexSubList, LinearIndex_In_RowIndexSubList, "ascend");

            int_max tempRowIndex_prev = -1;

            for (int_max j = 0; j < RecordedElementNumberInPerviousCol; ++j)
            {
                if (RowIndexSubList[j] == tempRowIndex_prev)
                {
                    MDK_Error("duplicate values are found, construction abort! @ mdkSparseMatrixDataInCSCFormat::Construct(...)")
                    
                    return;

                    // Matlab does this : not useful
                    // m_ElementList[m_ElementList.size()] += ElementList[tempOffset];
                }
                else
                {
                    m_RowIndexList.push_back(RowIndexSubList[j]);

                    //m_ColIndexList.push_back(CurrentColIndex);

                    auto tempOffset = LinearIndex_In_InputColIndexList[i + 1 - RecordedElementNumberInPerviousCol + LinearIndex_In_RowIndexSubList[j]];

                    m_ElementList.push_back(InputElementList[tempOffset]);

                    tempRowIndex_prev = RowIndexSubList[j];
                }
            }

            m_RecordedElementNumberInEachCol[CurrentColIndex] = RecordedElementNumberInPerviousCol;

            RowIndexSubList.resize(0);
            LinearIndex_In_RowIndexSubList.resize(0);

            // record the LinearIndex of first element in col # CurrentColIndex + 1

            m_ColBeginElementIndexInElementList[CurrentColIndex+1] = i+1;
        }       
    }
}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::ConstructColVector(const int_max* InputRowIndexList,
                                                                  const ElementType* InputElementList,
                                                                  int_max RecordedElementNumber,
                                                                  int_max InputRowNumber)
{
    std::vector<int_max> RowIndexList_sort(RecordedElementNumber);

    std::vector<int_max> ReletiveIndexList_sort(RecordedElementNumber);

    std::vector<ElementType> ElementList_sort(RecordedElementNumber);

    Sort(InputRowIndexList, RecordedElementNumber, RowIndexList_sort.data(), ReletiveIndexList_sort.data(), "ascend");

    for (int_max i = 0; i < RecordedElementNumber; ++i)
    {
        ElementList_sort[i] = InputElementList[ReletiveIndexList_sort[i]];
    }

    this->ConstructColVectorFromSortedData(std::move(RowIndexList_sort), std::move(ElementList_sort), InputRowNumber);
}


template<typename ElementType>
inline
void SparseMatrixDataInCSCFormat<ElementType>::ConstructRowVector(const int_max* InputColIndexList,
                                                                  const ElementType* InputElementList,
                                                                  int_max RecordedElementNumber,
                                                                  int_max InputColNumber)
{
    std::vector<int_max> ColIndexList_sort(RecordedElementNumber);

    std::vector<int_max> ReletiveIndexList_sort(RecordedElementNumber);

    std::vector<ElementType> ElementList_sort(RecordedElementNumber);

    Sort(InputColIndexList, RecordedElementNumber, ColIndexList_sort.data(), ReletiveIndexList_sort.data(), "ascend");

    for (int_max i = 0; i < RecordedElementNumber; ++i)
    {
        ElementList_sort[i] = InputElementList[ReletiveIndexList_sort[i]];
    }

    this->ConstructRowVectorFromSortedData(std::move(ColIndexList_sort), std::move(ElementList_sort), InputColNumber);
}


template<typename ElementType>
inline
void SparseMatrixDataInCSCFormat<ElementType>::ConstructFromSortedData(std::vector<int_max> InputRowIndexList,
                                                                       std::vector<int_max> InputColIndexList,
                                                                       std::vector<ElementType> InputElementList,
                                                                       int_max InputRowNumber,
                                                                       int_max InputColNumber)
{
    //--------------------------------------------------------------

    m_RowNumber = InputRowNumber;

    m_ColNumber = InputColNumber;

    //--------------------------------------------------------------

    m_RowIndexList = std::move(InputRowIndexList);

    m_ElementList = std::move(InputElementList);

    //--------------------------------------------------------------

    m_ColBeginElementIndexInElementList.resize(InputColNumber + 1);

    auto RecordedElementNumber = int_max(m_ElementList.size());
    
    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_ColBeginElementIndexInElementList[i] = -1;
    }

    m_ColBeginElementIndexInElementList[InputColNumber] = RecordedElementNumber; // end of the record

    //----------------------------------------------------------------

    m_RecordedElementNumberInEachCol.resize(InputColNumber);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_RecordedElementNumberInEachCol[i] = 0;
    }

    //--------------------------------------------------------------

    m_ColBeginElementIndexInElementList[InputColIndexList[0]] = 0;

    m_RecordedElementNumberInEachCol[InputColIndexList[0]] = 1;

    for (int_max i = 1; i < RecordedElementNumber; ++i)
    {
        if (InputColIndexList[i] != InputColIndexList[i-1])
        {
            m_ColBeginElementIndexInElementList[InputColIndexList[i]] = i;            
        }
        else
        {
            m_RecordedElementNumberInEachCol[InputColIndexList[i]] += 1;
        }
    }

}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::ConstructColVectorFromSortedData(std::vector<int_max> InputRowIndexList,
                                                                                std::vector<ElementType> InputElementList,
                                                                                int_max InputRowNumber)
{
    //--------------------------------------------------------------

    m_RowNumber = InputRowNumber;

    m_ColNumber = 1;

    //--------------------------------------------------------------

    m_RowIndexList = std::move(InputRowIndexList);

    m_ElementList = std::move(InputElementList);

    //--------------------------------------------------------------

    m_ColBeginElementIndexInElementList.resize(2);

    auto RecordedElementNumber = int_max(m_ElementList.size());
    
    m_ColBeginElementIndexInElementList[0] = 0;

    m_ColBeginElementIndexInElementList[1] = RecordedElementNumber; // end of the record

    //----------------------------------------------------------------

    m_RecordedElementNumberInEachCol.resize(1);

    m_RecordedElementNumberInEachCol[0] = RecordedElementNumber;
}


template<typename ElementType>
inline
void SparseMatrixDataInCSCFormat<ElementType>::ConstructRowVectorFromSortedData(std::vector<int_max> InputColIndexList,
                                                                                std::vector<ElementType> InputElementList,
                                                                                int_max InputColNumber)
{
    //--------------------------------------------------------------

    m_RowNumber = 1;

    m_ColNumber = InputColNumber;

    auto RecordedElementNumber = int_max(InputElementList.size());

    //--------------------------------------------------------------

    m_RowIndexList.resize(RecordedElementNumber);

    m_ElementList = std::move(InputElementList);

    //--------------------------------------------------------------

    m_ColBeginElementIndexInElementList.resize(InputColNumber + 1);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_ColBeginElementIndexInElementList[i] = -1;
    }

    m_ColBeginElementIndexInElementList[InputColNumber] = RecordedElementNumber; // end of the record

    //----------------------------------------------------------------

    m_RecordedElementNumberInEachCol.resize(InputColNumber);

    for (int_max i = 0; i < InputColNumber; ++i)
    {
        m_RecordedElementNumberInEachCol[i] = 0;
    }

    //--------------------------------------------------------------

    for (int_max i = 0; i < RecordedElementNumber; ++i)
    {
        m_RowIndexList[i] = 1;

        m_ColBeginElementIndexInElementList[InputColIndexList[i]] = i;

        m_RecordedElementNumberInEachCol[InputColIndexList[i]] = 1;
    }
}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::Clear()
{
    m_RowNumber = 0;

    m_ColNumber = 0;

    m_RowIndexList.clear();

    m_ColBeginElementIndexInElementList.clear();

    m_RecordedElementNumberInEachCol.clear();

    m_ElementList.clear();

    m_ZeroElement = ElementType(0);

    m_NaNElement = GetNaNElement(m_NaNElement); // zero if int

    m_IsSizeFixed = false;
}


template<typename ElementType>
inline 
void SparseMatrixDataInCSCFormat<ElementType>::Resize(int_max InputRowNumber, int_max InputColNumber)
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

    if (m_ElementList.size() == 0)
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

            for (int_max i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_RecordedElementNumberInEachCol[i] = 0;
            }

            m_ColBeginElementIndexInElementList.resize(InputColNumber + 1);

            for (int_max i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_ColBeginElementIndexInElementList[i] = -1;
            }

            int_max tempRecordedElmenetNumber = int_max(m_ElementList.size());

            m_ColBeginElementIndexInElementList[InputColNumber - 1] = tempRecordedElmenetNumber;
        }
        else if (InputColNumber < m_ColNumber)
        {
            // first to erase elements in m_ElementList, m_RowIndexList, m_ColIndexList 
            // erase in backward order
            for (int_max i = m_ColNumber - 1; i >= InputColNumber; --i)
            {
                if (m_RecordedElementNumberInEachCol[i] > 0)
                {
                    auto Offset_start = m_ColBeginElementIndexInElementList[i];
                    auto Offset_end = Offset_start + m_RecordedElementNumberInEachCol[i] - 1;

                    m_RowIndexList.erase(m_RowIndexList.begin() + Offset_start, m_RowIndexList.begin() + Offset_end);

                    //m_ColIndexList.erase(m_ColIndexList.begin() + Offset_start, m_ColIndexList.begin() + Offset_end);

                    m_ElementList.erase(m_ElementList.begin() + Offset_start, m_ElementList.begin() + Offset_end);
                }

                m_RecordedElementNumberInEachCol.erase(m_RecordedElementNumberInEachCol.begin() + i);

                m_ColBeginElementIndexInElementList.erase(m_ColBeginElementIndexInElementList.begin() + i);
            }

            int_max tempRecordedElmenetNumber = int_max(m_ElementList.size());

            m_ColBeginElementIndexInElementList[m_ColBeginElementIndexInElementList.size() - 1] = tempRecordedElmenetNumber;
        }
        //else if (InputColNumber == m_ColNumber)
        // do nothing

        m_RowNumber = InputRowNumber;
        m_ColNumber = InputColNumber;
        return;
    }

    // InputRowNumber < m_RowNumber -----------------------------------------------------------------------------

    auto ColIndexList = this->GetColIndexList();

    if (InputColNumber >= m_ColNumber)
    {
        // loop through m_RowIndexList, erase the record if its row-index is greater than RowNumber-1

        for (int_max i = int_max(m_RowIndexList.size()) -1; i >= 0; --i)
        {
            if (m_RowIndexList[i] >= InputRowNumber)
            {
                m_ElementList.erase(m_ElementList.begin() + i);

                m_RowIndexList.erase(m_RowIndexList.begin() + i);

                auto tempColIndex_i = ColIndexList[i];

                m_RecordedElementNumberInEachCol[tempColIndex_i] -= 1;

                if (m_RecordedElementNumberInEachCol[tempColIndex_i] == 0)
                {
                    m_ColBeginElementIndexInElementList[tempColIndex_i] = -1;
                }

                ColIndexList.erase(ColIndexList.begin() + i);
            }
        }

        if (InputColNumber > m_ColNumber)
        {
            m_RecordedElementNumberInEachCol.resize(InputColNumber);

            for (int_max i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_RecordedElementNumberInEachCol[i] = 0;
            }

            m_ColBeginElementIndexInElementList.resize(InputColNumber + 1);

            for (int_max i = m_ColNumber; i < InputColNumber; ++i)
            {
                m_ColBeginElementIndexInElementList[i] = -1;
            }

            int_max tempRecordedElmenetNumber = int_max(m_ElementList.size());

            m_ColBeginElementIndexInElementList[InputColNumber - 1] = tempRecordedElmenetNumber;
        }

        m_RowNumber = InputRowNumber;
        m_ColNumber = InputColNumber;
        return;
    }

    // InputRowNumber < m_RowNumber && InputColNumber < m_ColNumber -------------------------------------------------------

    // keep the elements in range, and then construct a new sparse matrix

    std::vector<int_max> newRowIndexList;

    std::vector<int_max> newColIndexList;

    std::vector<ElementType> newElementList;

    for (int_max i = int_max(m_RowIndexList.size()) -1; i >= 0; --i)
    {
        if (m_RowIndexList[i] < InputRowNumber && ColIndexList[i] < InputColNumber)
        {
            newRowIndexList.push_back(m_RowIndexList[i]);

            newColIndexList.push_back(ColIndexList[i]);

            newElementList.push_back(m_ElementList[i]);
        }
    }

    this->Construct(newRowIndexList.data(), newColIndexList.data(), newElementList.data(), int_max(newElementList.size()), InputRowNumber, InputColNumber);
   
}


template<typename ElementType>
inline
void SparseMatrixDataInCSCFormat<ElementType>::FastResize(int_max InputRowNumber, int_max InputColNumber)
{
    this->Construct(InputRowNumber, InputColNumber);
}


template<typename ElementType>
inline
ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator[](int_max LinearIndex)
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator[](int_max LinearIndex) const
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int_max LinearIndex)
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    auto ColIndex = divresult.quot;

    auto RowIndex = divresult.rem;

    return (*this)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int_max LinearIndex) const
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    auto ColIndex = divresult.quot;

    auto RowIndex = divresult.rem;

    return (*this)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int_max RowIndex, int_max ColIndex)
{
    return this->SetElement(RowIndex, ColIndex, m_ZeroElement);
}


template<typename ElementType>
inline
const ElementType&
SparseMatrixDataInCSCFormat<ElementType>::operator()(int_max RowIndex, int_max ColIndex) const
{
    return this->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrixDataInCSCFormat<ElementType>::GetElement(int_max LinearIndex) const
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    auto ColIndex = divresult.quot;

    auto RowIndex = divresult.rem;

    return this->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrixDataInCSCFormat<ElementType>::GetElement(int_max RowIndex, int_max ColIndex) const
{
    auto RelativeIndex = this->GetElementIndexInElementList(RowIndex, ColIndex);

    if (RelativeIndex >= 0)
    {
        return m_ElementList[RelativeIndex];
    }

    return m_ZeroElement;
}


template<typename ElementType>
inline
ElementType& SparseMatrixDataInCSCFormat<ElementType>::SetElement(int_max LinearIndex, const ElementType& InputElement)
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    auto ColIndex = divresult.quot;

    auto RowIndex = divresult.rem;

    return this->SetElement(RowIndex, ColIndex, InputElement);
}


template<typename ElementType>
inline
ElementType& SparseMatrixDataInCSCFormat<ElementType>::SetElement(int_max RowIndex, int_max ColIndex, const ElementType& InputElement)
{
    auto tempRelativeIndex = this->GetElementIndexInElementList(RowIndex, ColIndex);

    if (tempRelativeIndex >= 0)
    {
        m_ElementList[tempRelativeIndex] = InputElement;

        return m_ElementList[tempRelativeIndex];
    }

    //---------------------------------------- Construct a new record --------------------------------------------------//

    if (m_ColBeginElementIndexInElementList[ColIndex] < 0) // insert the first element in col # ColIndex
    {
        int_max Offset = -1;

        // check col with Index > ColIndex
        for (int_max i = ColIndex + 1; i < m_ColNumber; ++i)
        {
            if (m_ColBeginElementIndexInElementList[i] >= 0)
            {
                Offset = m_ColBeginElementIndexInElementList[i];

                break;
            }
        }

        if (Offset < 0) // every col (index > ColIndex) is empty
        {
            // check backward
            for (int_max i = ColIndex - 1; i >= 0; --i)
            {
                if (m_RecordedElementNumberInEachCol[i] > 0)
                {
                    Offset = m_ColBeginElementIndexInElementList[i] + m_RecordedElementNumberInEachCol[i];

                    break;
                }
            }

            if (Offset < 0) // still empty: this is an empty matrix
            {
                Offset = 0;
            }
        }

        m_RowIndexList.insert(m_RowIndexList.begin() + Offset, RowIndex);

        //m_ColIndexList.insert(m_ColIndexList.begin() + Offset, ColIndex);

        m_ElementList.insert(m_ElementList.begin() + Offset, InputElement);

        m_ColBeginElementIndexInElementList[ColIndex] = Offset;

        for (int_max i = ColIndex + 1; i < m_ColNumber; ++i)
        {
            if (m_ColBeginElementIndexInElementList[i] >= 0)
            {
                m_ColBeginElementIndexInElementList[i] += 1;
            }
        }

        m_RecordedElementNumberInEachCol[ColIndex] = 1;

        return m_ElementList[Offset];

    }
    else // insert the input element in a non-empty col # ColIndex
    {
        // get the RowIndexSubList of the elements in Col # ColIndex
        // compare RowIndex to  RowIndexSubList, and get offset

        int_max Index_Begin = m_ColBeginElementIndexInElementList[ColIndex];

        int_max Offset = Index_Begin + m_RecordedElementNumberInEachCol[ColIndex];

        for (int_max i = 0; i < m_RecordedElementNumberInEachCol[ColIndex]; ++i)
        {
            if (m_RowIndexList[Index_Begin + i] > RowIndex)
            {
                Offset = Index_Begin + i;

                break;
            }
        }

        m_RowIndexList.insert(m_RowIndexList.begin() + Offset, RowIndex);

        //m_ColIndexList.insert(m_ColIndexList.begin() + Offset, ColIndex);

        m_ElementList.insert(m_ElementList.begin() + Offset, InputElement);

        for (int_max i = ColIndex + 1; i < m_ColNumber; ++i)
        {
            if (m_ColBeginElementIndexInElementList[i] >= 0)
            {
                m_ColBeginElementIndexInElementList[i] += 1;
            }
        }

        m_RecordedElementNumberInEachCol[ColIndex] += 1;

        return m_ElementList[Offset];
    }

}


template<typename ElementType>
inline
int_max SparseMatrixDataInCSCFormat<ElementType>::GetElementIndexInElementList(int_max LinearIndex) const
{
    std::lldiv_t divresult;

    divresult = div(LinearIndex, m_RowNumber);

    ColIndex = divresult.quot;

    RowIndex = divresult.rem;

    return this->GetElementIndexInElementList(RowIndex, ColIndex);
}


template<typename ElementType>
inline
int_max SparseMatrixDataInCSCFormat<ElementType>::GetElementIndexInElementList(int_max RowIndex, int_max ColIndex) const
{
    auto RelativeIndex_Begin = m_ColBeginElementIndexInElementList[ColIndex];

    if (RelativeIndex_Begin >= 0)
    {
        for (int_max i = 0; i < m_RecordedElementNumberInEachCol[ColIndex]; ++i)
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
void SparseMatrixDataInCSCFormat<ElementType>::Copy(const SparseMatrixDataInCSCFormat<ElementType>& InputMatrixData)
{
    m_RowNumber = InputMatrixData.m_RowNumber;

    m_ColNumber = InputMatrixData.m_ColNumber;

    m_RowIndexList = InputMatrixData.m_RowIndexList;

    m_ElementList = InputMatrixData.m_ElementList;

    m_ColBeginElementIndexInElementList = InputMatrixData.m_ColBeginElementIndexInElementList;

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

    m_ElementList = std::move(InputMatrixData.m_ElementList);

    m_ColBeginElementIndexInElementList = std::move(InputMatrixData.m_ColBeginElementIndexInElementList);

    m_RecordedElementNumberInEachCol = std::move(InputMatrixData.m_RecordedElementNumberInEachCol);

    m_ZeroElement = InputMatrixData.m_ZeroElement;
}


template<typename ElementType>
inline
int_max SparseMatrixDataInCSCFormat<ElementType>::GetRecordedElementNumber() const
{
    return int_max(m_ElementList.size());
}


template<typename ElementType>
inline
std::vector<int_max> SparseMatrixDataInCSCFormat<ElementType>::GetColIndexList() const
{
    auto RecordedElementNumber = int_max(m_ElementList.size());

    std::vector<int_max> ColIndexList(RecordedElementNumber);

    for (int_max j = 0; j < m_ColNumber; ++j)
    {
        if (m_RecordedElementNumberInEachCol[j] > 0)
        {
            auto begin_index = m_ColBeginElementIndexInElementList[j];
            auto end_index = begin_index + m_RecordedElementNumberInEachCol[j] - 1;

            for (int_max k = begin_index; k <= end_index; ++k)
            {
                ColIndexList[k] = j;
            }
        }
    }

    return ColIndexList;
}


template<typename ElementType>
inline
ElementType* SparseMatrixDataInCSCFormat<ElementType>::GetPointerOfBeginElementInCol(int_max ColIndex)
{
    auto tempIndex = m_ColBeginElementIndexInElementList[ColIndex];

    if (tempIndex >= 0)
    {
        return &m_ElementList[tempIndex];
    }
    else
    {
        return nullptr;
    }
}


template<typename ElementType>
inline
const ElementType* SparseMatrixDataInCSCFormat<ElementType>::GetPointerOfBeginElementInCol(int_max ColIndex) const
{
    auto tempIndex = m_ColBeginElementIndexInElementList[ColIndex];

    if (tempIndex >= 0)
    {
        return &m_ElementList[tempIndex];
    }
    else
    {
        return nullptr;
    }
}


template<typename ElementType>
inline
int_max SparseMatrixDataInCSCFormat<ElementType>::GetIndexInElementListOfBeginElementInCol(int_max ColIndex) const
{
    return m_ColBeginElementIndexInElementList[ColIndex];
}


template<typename ElementType>
inline
int_max SparseMatrixDataInCSCFormat<ElementType>::GetRowIndexOfBeginElementInCol(int_max ColIndex) const
{
    auto tempIndex = m_ColBeginElementIndexInElementList[ColIndex];

    if (tempIndex >= 0)
    {
        return m_RowIndexList[tempIndex];
    }
    else
    {
        return -1;
    }
}



template<typename ElementType>
inline
int_max SparseMatrixDataInCSCFormat<ElementType>::GetRecordedElementNumberInCol(int_max ColIndex) const
{
    return m_RecordedElementNumberInEachCol[ColIndex];
}


template<typename ElementType>
inline
std::vector<int_max> SparseMatrixDataInCSCFormat<ElementType>::GetRowIndexListInCol(int_max ColIndex) const
{
    auto RecordedElementNumber = int_max(m_ElementList.size());

    std::vector<int_max> OutputRowIndexList;
    
    OutputRowIndexList.reserve(m_RecordedElementNumberInEachCol[ColIndex]);

    if (m_RecordedElementNumberInEachCol[ColIndex] > 0)
    {
        auto begin_index = m_ColBeginElementIndexInElementList[ColIndex];
        auto end_index = begin_index + m_RecordedElementNumberInEachCol[ColIndex] - 1;

        for (int_max k = begin_index; k <= end_index; ++k)
        {
            OutputRowIndexList.push_back(m_RowIndexList[k]);
        }
     }
    
    return OutputRowIndexList;
}

//===================================================================================================================================//
//                                                     SparseMatrix
//===================================================================================================================================//

template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix()
{
    this->Resize(0, 0);
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(int_max RowNumber, int_max ColNumber)
{
    this->Resize(RowNumber, ColNumber);
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(const ElementType& Element)
{
    this->Resize(1, 1);
    (*this)(0) = Element;
}


template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(const SparseMatrix<ElementType>& InputMatrix)
{
	this->Copy(InputMatrix);
}


// move constructor
template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(SparseMatrix<ElementType>&& InputMatrix)
{
	(*this) = std::move(InputMatrix);
}

/*
template<typename ElementType>
inline
SparseMatrix<ElementType>::SparseMatrix(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    this->Resize(0, 0);

    this->Take(ShadowMatrix.ConstructSparseMatrix());
}
*/

template<typename ElementType>
inline
SparseMatrix<ElementType>::~SparseMatrix()
{
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const DenseMatrix<int_max>& RowIndexList,
                                          const DenseMatrix<int_max>& ColIndexList,
                                          const DenseMatrix<ElementType>& ElementList,
                                          int_max RowNumber,
                                          int_max ColNumber,
                                          int_max AdditionalReservedCapacity = 0)
{
    return this->Construct(RowIndexList.GetElementPointer(),
                           ColIndexList.GetElementPointer(), 
                           ElementList.GetElementPointer(), 
                           RowNumber,
                           ColNumber, 
                           std::max(AdditionalReservedCapacity, int_max(0)));
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const std::vector<int_max>& RowIndexList,
                                          const std::vector<int_max>& ColIndexList,
                                          const std::vector<ElementType>& ElementList,
                                          int_max RowNumber,
                                          int_max ColNumber,
                                          int_max AdditionalReservedCapacity = 0)
{
    return this->Construct(RowIndexList.data(),
                           ColIndexList.data(), 
                           ElementList.data(), 
                           int_max(ElementList.size()), 
                           RowNumber, 
                           ColNumber,
                           std::max(AdditionalReservedCapacity, int_max(0)));

}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const std::initializer_list<int_max>& RowIndexList,
                                          const std::initializer_list<int_max>& ColIndexList,
                                          const std::initializer_list<ElementType>& ElementList,
                                          int_max RowNumber,
                                          int_max ColNumber,
                                          int_max AdditionalReservedCapacity = 0)
{
    return this->Construct(RowIndexList.begin(), 
                           ColIndexList.begin(), 
                           ElementList.begin(), 
                           int_max(ElementList.size()), 
                           RowNumber, 
                           ColNumber, 
                           std::max(AdditionalReservedCapacity, int_max(0)));
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Construct(const int_max* RowIndexList,
                                          const int_max* ColIndexList,
                                          const ElementType* ElementList,
                                          int_max RecordedElementNumber,
                                          int_max RowNumber,
                                          int_max ColNumber,
                                          int_max AdditionalReservedCapacity = 0)
{
    //check input
    if (RowIndexList == nullptr || ColIndexList == nullptr || ElementList == nullptr || RecordedElementNumber < 0 || RowNumber < 0 || ColNumber < 0)
    {
        MDK_Error("empty input @ SparseMatrix::Construct(pointers, ...)")
        return false;
    }

    if (RecordedElementNumber == 0)
    {
        m_MatrixData->Construct(RowNumber, ColNumber);

        return true;
    }

    m_MatrixData->Construct(RowIndexList, ColIndexList, ElementList, RecordedElementNumber, RowNumber, ColNumber, std::max(AdditionalReservedCapacity, int_max(0)));

    return true;
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::ConstructFromSortedData(const std::vector<int_max>& RowIndexList,
                                                        const std::vector<int_max>& ColIndexList,
                                                        const std::vector<ElementType>& ElementList,
                                                        int_max RowNumber,
                                                        int_max ColNumber)
{
    m_MatrixData->ConstructFromSortedData(std::move(RowIndexList), std::move(ColIndexList), std::move(ElementList), RowNumber, ColNumber);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ConstructColVector(const std::initializer_list<int_max>& RowIndexList,
                                                   const std::vector<ElementType>& ElementList,
                                                   int_max RowNumber)
{
    if (RowIndexList.size() != ElementList.size() || RowNumber < int_max(ElementList.size()))
    {
        MDK_Error("Invalid input @ SparseMatrix::ConstructColVector(std::initializer_list ...)")
        return false;
    }

    m_MatrixData->ConstructColVector(RowIndexList.begin(), ElementList.data(), int_max(ElementList.size()), RowNumber);

    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::ConstructColVector(const std::vector<int_max>& RowIndexList,
                                                   const std::vector<ElementType>& ElementList,
                                                   int_max RowNumber)
{
    if (RowIndexList.size() != ElementList.size() || RowNumber < int_max(ElementList.size()))
    {
        MDK_Error("Invalid input @ SparseMatrix::ConstructColVector(std::vector ...)")
        return false;
    }

    m_MatrixData->ConstructColVector(RowIndexList.data(), ElementList.data(), int_max(ElementList.size()), RowNumber);

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ConstructColVector(const DenseMatrix<int_max>& RowIndexList,
                                                   const DenseMatrix<ElementType>& ElementList,
                                                   int_max RowNumber)
{
    if (RowIndexList.GetElementNumber() != ElementList.GetElementNumber() || RowNumber < ElementList.GetElementNumber())
    {
        MDK_Error("Invalid input @ SparseMatrix::ConstructColVector(DenseMatrix ...)")
        return false;
    }

    m_MatrixData->ConstructColVector(RowIndexList.GetElementPointer(), ElementList.GetElementPointer(), ElementList.GetElementNumber(), RowNumber);

    return true;
}


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::ConstructColVectorFromSortedData(std::vector<int_max> RowIndexList,
                                                                 std::vector<ElementType> ElementList,
                                                                 int_max RowNumber)
{
    m_MatrixData->ConstructColVectorFromSortedData(std::move(RowIndexList), std::move(ElementList), RowNumber);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ConstructRowVector(const std::initializer_list<int_max>& ColIndexList,
                                                   const std::vector<ElementType>& ElementList,
                                                   int_max ColNumber)
{
    if (RowIndexList.size() != ElementList.size() || ColNumber < int_max(ElementList.size()))
    {
        MDK_Error("Invalid input @ SparseMatrix::ConstructRowVector(std::initializer_list ...)")
        return false;
    }

    m_MatrixData->ConstructColVector(RowIndexList.begin(), ElementList.data(), int_max(ElementList.size()), ColNumber);

    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::ConstructRowVector(const std::vector<int_max>& ColIndexList,
                                                   const std::vector<ElementType>& ElementList,
                                                   int_max ColNumber)
{
    if (RowIndexList.size() != ElementList.size() || ColNumber < int_max(ElementList.size()))
    {
        MDK_Error("Invalid input @ SparseMatrix::ConstructRowVector(std::vector ...)")
        return false;
    }

    m_MatrixData->ConstructColVector(RowIndexList.data(), ElementList.data(), int_max(ElementList.size()), ColNumber);

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ConstructRowVector(const DenseMatrix<int_max>& ColIndexList,
                                                   const DenseMatrix<ElementType>& ElementList,
                                                   int_max ColNumber)
{
    if (RowIndexList.GetElementNumber() != ElementList.GetElementNumber() || ColNumber < ElementList.GetElementNumber())
    {
        MDK_Error("Invalid input @ SparseMatrix::ConstructRowVector(DenseMatrix ...)")
        return false;
    }

    m_MatrixData->ConstructRowVector(RowIndexList.GetElementPointer(), ElementList.GetElementPointer(), ElementList.GetElementNumber(), ColNumber);

   return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::ConstructFromSparseColVectorSetInOrder(const std::vector<SparseVector<ElementType>>& SparseVectorSet)
{
	auto ColNumber = int_max(SparseVectorSet.size());
    if (ColNumber == 0)
	{
		this->Clear();
		return true;
    }

    std::vector<int_max> RowIndexList;
    std::vector<int_max> ColIndexList;
    std::vector<ElementType> ElementList;
    
	auto RowNumber_prev = SparseVectorSet[0].GetLength();

    for (int_max j = 0; j < ColNumber; ++j)
    {
		auto RowNumber_j = SparseVectorSet[j].GetLength();

		if (j > 0 && RowNumber_j != RowNumber_prev)
        {
            MDK_Error("Invalid input: RowNumber @ SparseMatrix::ConstructFromSparseColVectorSetInOrder(std::vector ...)")
            return false;
        }

        auto IndexList_j = SparseVectorSet[j].IndexList();

        auto ElementList_j = SparseVectorSet[j].ElementList();

		for (int_max i = 0; i < RowNumber_j; ++i)
        {
            RowIndexList.push_back(IndexList_j[i]);
            ColIndexList.push_back(j);
            ElementList.push_back(ElementList_j[i]);
        }
    }

    m_MatrixData->ConstructFromSortedData(std::move(RowIndexList), std::move(ColIndexList), std::move(ElementList), RowNumber, ColNumber);

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ConstructFromSparseColVectorSetInOrder(const ObjectArray<SparseVector<ElementType>>& SparseVectorSet)
{
	auto ColNumber = SparseVectorSet.GetElementNumber();
    if (ColNumber == 0)
    {
		this->Clear();
        return true;
    }

    std::vector<int_max> RowIndexList;
    std::vector<int_max> ColIndexList;
    std::vector<ElementType> ElementList;

	auto RowNumber_prev = SparseVectorSet[0].GetLength();

    for (int_max j = 0; j < ColNumber; ++j)
    {
		auto RowNumber_j = SparseVectorSet[j].GetLength();

		if (j > 0 && RowNumber_j != RowNumber_prev)
        {
            MDK_Error("SparseVector Length is not the same @ SparseMatrix::ConstructFromSparseColVectorSetInOrder(DenseMatrix ...)")			
            return false;
        }

        auto IndexList_j = SparseVectorSet[j].IndexList();

		auto ElementList_j = SparseVectorSet[j].ElementList();

		for (int_max i = 0; i < RowNumber_j; ++i)
        {
            RowIndexList.push_back(IndexList_j[i]);
            ColIndexList.push_back(j);
			ElementList.push_back(ElementList_j[i]);
        }
    }

	m_MatrixData->ConstructFromSortedData(std::move(RowIndexList), std::move(ColIndexList), std::move(ElementList), RowNumber, ColNumber);

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
	m_MatrixData = std::move(InputMatrix.m_MatrixData);
}


template<typename ElementType>
inline
void SparseMatrix<ElementType>::operator=(const ElementType& Element)
{
    auto ElementNumber = this->GetElementNumber();

    if (this->IsSizeFixed() == true)
    {
        if (ElementNumber <= 0)
        {
            MDK_Error("Can not change matrix size @ SparseMatrix::operator=(Element)")
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
        MDK_Warning("The size of the matrix changes to 1x1 @ SparseMatrix::operator=(Element)")

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
        MDK_Error("operator=(list) can not be called if self is empty @ SparseMatrix::operator=(list)")
        return;
    }

    if (ElementNumber != list.size())
    {
        MDK_Error("Size does not match @ SparseMatrix::operator=(list)")
        return;
    }

    auto RawPointer = this->GetElementPointer();

    int_max Counter = 0;

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
    {
        int_max Index = 0;

        for (int_max j = 0; j < SelfSize.ColNumber; ++j)
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
        MDK_Error("operator=(list in list) can not be called if self is empty @ SparseMatrix::operator=(list in list)")
        return;
    }

    if (SelfSize.RowNumber != list.size())
    {
        MDK_Error("Row Size does not match @ SparseMatrix::operator=(list in list)")
        return;
    }

    //check each row-list 
    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
    {
        auto subList = list.begin()[i];

        if (subList.size() != SelfSize.ColNumber)
        {
            MDK_Error("Col Size does not match in row: " << i << " @ SparseMatrix::operator=(list in list)")
            return;
        }
    }

    auto RawPointer = this->GetElementPointer();

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
    {
        auto subList = list.begin()[i];

        int_max Index = 0;

        for (int_max j = 0; j < SelfSize.ColNumber; ++j)
        {
            RawPointer[Index + i] = subList.begin()[j];

            Index += SelfSize.RowNumber;
        }
    }

    return;
}


/*
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
*/

template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::Copy(const SparseMatrix<ElementType_Input>& InputMatrix)
{
    if (this == &InputMatrix)
    {
        MDK_Warning("A Matrix tries to Copy itself @ SparseMatrix::Copy(InputMatrix)")
        return false;
    }

    if (InputMatrix.IsEmpty() == true)
    {
        MDK_Warning("InputMatrix is empty, and this matrix is set to be empty @ SparseMatrix::Copy(InputMatrix)")

        this->Clear();

        return true;
    }

    auto InputSize = InputMatrix.GetSize();

    if (this->IsSizeFixed() == true)
    {
        auto SelfSize = this->GetSize();

        if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
        {
            MDK_Error("Can not change matrix size @ SparseMatrix::Copy(InputMatrix)")
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
        MDK_Warning("A Matrix tries to share itself @ SparseMatrix::Share(InputMatrix)")
        return false;
    }

    auto InputSize = InputMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (this->IsSizeFixed() == true)
    {
        if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
        {
            MDK_Error("Matrix size can not be changed @ SparseMatrix::Share(InputMatrix)")
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
    // Matrix = Matrix
    if (this == &InputMatrix)
    {
        MDK_Warning("A Matrix tries to ForceShare itself @ SparseMatrix::ForceShare(InputMatrix)")
        return;
    }

    m_MatrixData = InputMatrix.m_MatrixData; // std::SharedCopy_ptr
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::ForceShare(const SparseMatrix<ElementType>* InputMatrix)
{
    if (InputMatrix == nullptr)
    {
        MDK_Error("Input is nullptr @ SparseMatrix::ForceShare(const SparseMatrix<ElementType>* InputMatrix)")
        return false;
    }

    this->ForceShare(*InputMatrix);

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(SparseMatrix<ElementType>& InputMatrix)
{
    // MatrixA = MatrixA
    if (this == &InputMatrix)
    {
        MDK_Warning("A Matrix tries to take itself @ SparseMatrix::Take(InputMatrix)")
        return false;
    }

    auto InputSize = InputMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (this->IsSizeFixed() == true)
    {
        if (InputSize.RowNumber != SelfSize.RowNumber || InputSize.ColNumber != SelfSize.ColNumber)
        {
            MDK_Error("Size does not match @ SparseMatrix::Take(InputMatrix)")
            return false;
        }
    }

    if (InputMatrix.IsEmpty() == true)
    {
        if (SelfSize.RowNumber > 0)
        {
            MDK_Warning("InputMatrix is empty, and this matrix is set to be empty @ SparseMatrix::Take(InputMatrix)")
            this->Clear();
        }

        return true;
    }

    // now, InputMatrix is not empty

    // note: m_MatrixData.swap(InputMatrix.m_MatrixData) will invalidate Share()

    m_MatrixData->Take(*InputMatrix.m_MatrixData);

    //clear input matrix
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


/*
template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Take(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    auto InputSize = ShadowMatrix.GetSize();

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber == SelfSize.RowNumber && InputSize.ColNumber == SelfSize.ColNumber)
    {
        ShadowMatrix.ConstructSparseMatrix(*this);
    }
    else
    {
        if (this->IsSizeFixed() == true)
        {
            MDK_Error("Size does not match @ SparseMatrix::Take(ShadowMatrix)")
            return false;
        }

        this->Take(ShadowMatrix.ConstructSparseMatrix());
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
        GlueMatrix.ConstructSparseMatrix(*this);
    }
    else
    {
        if (this->IsSizeFixed() == true)
        {
            MDK_Error("Size does not match @ SparseMatrix::Take(SparseGlueMatrix_ForLinearCombination)")
            return false;
        }

        this->Take(GlueMatrix.ConstructSparseMatrix());
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
        GlueMatrix.ConstructSparseMatrix(*this);
    }
    else
    {
        if (this->IsSizeFixed() == true)
        {
            MDK_Error("Size does not match @ SparseMatrix::Take(SparseGlueMatrix_ForMultiplication)")
            return false;
        }

        this->Take(GlueMatrix.ConstructSparseMatrix());
    }

    return true;
}
*/


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::Clear()
{
    m_MatrixData->Clear();
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::Reshape(int_max InputRowNumber, int_max InputColNumber)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix size can not be changed @ SparseMatrix::Reshape()")
        return false;
    }

    if (this->IsEmpty() == true)
    {
        if (InputRowNumber > 0 || InputColNumber > 0)
        {
            MDK_Error("Self is empty and Size does not match @ SparseMatrix::Reshape()")
            return false;
        }
    }

    if (InputRowNumber*InputColNumber != this->GetElementNumber())
    {
        MDK_Error("Size does not match @ SparseMatrix::Reshape")
        return false;
    }

  //-------------------------------------------

    return false;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Resize(int_max InputRowNumber, int_max InputColNumber)
{
    if (InputRowNumber < 0 || InputColNumber < 0)
    {
        MDK_Error("Invalid Input: negtive @ SparseMatrix::Resize(int_max RowNumber, int_max ColNumber)")
        return false;
    }

try
{
    //--------initialize the matrix ----------------------------------------------------
	if (!m_MatrixData) //if(this->IsPureEmpty() == true)
    {
		m_MatrixData = std::make_shared<SparseMatrixDataInCSCFormat<ElementType>>();
	}
    //-----------------------------------------------------------------------------------

    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not be changed @ SparseMatrix::Resize(int_max RowNumber, int_max ColNumber)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (InputRowNumber == SelfSize.RowNumber && InputColNumber == SelfSize.ColNumber)
    {        
        return true;
    }

    //-----------------------------------------------------------------------------------

    if (InputRowNumber == 0 || InputColNumber == 0)
    {
        m_MatrixData->Construct(InputRowNumber, InputColNumber);

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

}
catch (...)
{
    MDK_Error("Out of Memory @ SparseMatrix::Resize(int_max InputRowNumber, int_max InputColNumber)")
    return false;
}
    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::FastResize(int_max InputRowNumber, int_max InputColNumber)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not be changed @ SparseMatrix::FastResize(int_max RowNumber, int_max ColNumber)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (InputRowNumber == SelfSize.RowNumber && InputColNumber == SelfSize.ColNumber)
    {
        return true;
    }

    if (InputRowNumber < 0 || InputColNumber < 0)
    {
        MDK_Error("Invalid Input: negtive @ SparseMatrix::FastResize(int_max RowNumber, int_max ColNumber)")
        return false;
    }

try
{
    //--------initialize the matrix ----------------------------------------------------
	if (!m_MatrixData) // if(this->IsPureEmpty() == true)
    {
		m_MatrixData = std::make_shared<SparseMatrixDataInCSCFormat<ElementType>>();
    }
    //-----------------------------------------------------------------------------------

    m_MatrixData->FastResize(InputRowNumber, InputColNumber);
}
catch (...)
{
    MDK_Error("Out of Memory @ SparseMatrix::FastResize(int_max InputRowNumber, int_max InputColNumber)")
    return false;
}
    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::Resize(int_max InputElementNumber) // try to keep the old data, can not use this to resize a m x n matrix (m>1 or n>1)
{
    if (InputElementNumber < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::Resize(int_max InputElementNumber)")
        return false;
    }

    auto Size = this->GetSize();

    if (Size.RowNumber >1 || Size.ColNumber > 1)
    {
        MDK_Error("Self is not empty and not a vector @ SparseMatrix::Resize(int_max InputElementNumber)")
        return false;
    }

    if (Size.RowNumber*Size.ColNumber == InputElementNumber)
    {
        return true;
    }

    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Can not change size @ SparseMatrix::Resize(int_max InputElementNumber)")
        return false;
    }

try
{
    if (Size.RowNumber == 0)// empty -> row vector
    {
        m_MatrixData->Resize(1, InputElementNumber);
    }
    else if (Size.RowNumber == 1)
    {
        m_MatrixData->Resize(1, InputElementNumber);
    }
    else if (Size.ColNumber == 1)
    {
        m_MatrixData->Resize(InputElementNumber, 1);
    }
}
catch (...)
{
    MDK_Error("Out of Memory @ SparseMatrix::Resize(int_max InputElementNumber)")
    return false;
}
    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::FastResize(int_max InputElementNumber) // do not care about old data, can not use this to resize a m x n matrix (m>1 or n>1)
{
    if (InputElementNumber < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::FastResize(int_max InputElementNumber)")
        return false;
    }

    auto Size = this->GetSize();

    if (Size.RowNumber >1 || Size.ColNumber > 1)
    {
        MDK_Error("Self is not empty and not a vector @ SparseMatrix::FastResize(int_max InputElementNumber)")
        return false;
    }

    if (Size.RowNumber*Size.ColNumber == InputElementNumber)
    {
        return true;
    }

    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Can not change size @ SparseMatrix::FastResize(int_max InputElementNumber)")
        return false;
    }

try
{
    if (Size.RowNumber == 0)// empty -> row vector
    {
        m_MatrixData->FastResize(1, InputElementNumber);
    }
    else if (Size.RowNumber == 1)
    {
        m_MatrixData->FastResize(1, InputElementNumber);
    }
    else if (Size.ColNumber == 1)
    {
        m_MatrixData->FastResize(InputElementNumber, 1);
    }
}
catch (...)
{
    MDK_Error("Out of Memory @ SparseMatrix::FastResize(int_max InputElementNumber)")
    return false;
}
    return true;
}


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::FixSize()
{
	if (!m_MatrixData)
	{
		m_MatrixData->m_IsSizeFixed = true;
		m_MatrixData->m_ElementList.shrink_to_fit();
	}
	else
	{
		MDK_Error("Self is pure empty @ SparseMatrix::FixSize()")
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsSizeFixed() const
{
	if (this->IsPureEmpty() == false)
	{
		return m_MatrixData->m_IsSizeFixed;
	}
	else
	{
		return false;
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsEmpty() const
{
	if (!m_MatrixData)
	{
		return true;
	}
	else
	{
		return (m_MatrixData->m_RowNumber == 0 || m_MatrixData->m_ColNumber == 0);
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsPureEmpty() const
{
	return !(m_MatrixData);
}


template<typename ElementType>
inline
MatrixSize SparseMatrix<ElementType>::GetSize() const
{
    MatrixSize Size;
	if (this->IsPureEmpty() == false)
	{
		Size.RowNumber = m_MatrixData->m_RowNumber;
		Size.ColNumber = m_MatrixData->m_ColNumber;
	}
	else
	{
		Size.RowNumber = 0;
		Size.ColNumber = 0;
	}
    return Size;
}


template<typename ElementType>
inline
int_max SparseMatrix<ElementType>::GetElementNumber() const
{
	if (this->IsPureEmpty() == false)
	{
		return m_MatrixData->m_RowNumber * m_MatrixData->m_ColNumber;
	}
	else
	{
		return 0;
	}
}


template<typename ElementType>
inline
int_max SparseMatrix<ElementType>::GetRecordedElementNumber() const
{
	if (this->IsPureEmpty() == false)
	{
		return m_MatrixData->GetRecordedElementNumber();
	}
	else
	{
		return 0;
	}
}


template<typename ElementType>
inline 
int_max SparseMatrix<ElementType>::GetColNumber() const
{
	if (this->IsPureEmpty() == false)
	{
		return m_MatrixData->m_ColNumber;
	}
	else
	{
		return 0;
	}
}


template<typename ElementType>
inline 
int_max SparseMatrix<ElementType>::GetRowNumber() const
{
	if (this->IsPureEmpty() == false)
	{
	    return m_MatrixData->m_RowNumber;
	}
	else
	{
		return 0;
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsVector() const
{
	if (this->IsPureEmpty() == false)
	{
		return (m_MatrixData->m_RowNumber == 1 || m_MatrixData->m_ColNumber == 1);
	}
	else
	{
		return false;
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsRowVector() const
{
	if(this->IsPureEmpty() == false)
	{
		return (m_MatrixData->m_RowNumber == 1);
	}
	else
	{
		return false;
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsColVector() const
{
	if (this->IsPureEmpty() == false)
	{
		return (m_MatrixData->m_ColNumber == 1);
	}
	else
	{
		return false;
	}
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::IsSquare() const
{
	if (this->IsPureEmpty() == false)
	{
		return (m_MatrixData->m_RowNumber ==  m_MatrixData->m_ColNumber);
	}
	else
	{
		return false;
	}
}



template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::GetNaNElement()  const
{
	if (this->IsPureEmpty() == false)
	{
		return m_MatrixData->m_NaNElement;
	}
	else
	{
		return GetNaNElement(ElementType(0));
	}
}


template<typename ElementType>
inline
MatrixElementTypeEnum SparseMatrix<ElementType>::GetElementType() const
{
	return FindMatrixElementType(ElementType(0));
}


template<typename ElementType>
inline 
const std::vector<int_max>& SparseMatrix<ElementType>::RowIndexList() const
{
    return m_MatrixData->m_RowIndexList;
}


template<typename ElementType>
inline 
std::vector<int_max> SparseMatrix<ElementType>::GetRowIndexList() const
{
	return m_MatrixData->m_RowIndexList;
}


template<typename ElementType>
inline 
std::vector<int_max> SparseMatrix<ElementType>::GetColIndexList() const
{
    return m_MatrixData->GetColIndexList();
}


template<typename ElementType>
inline
const std::vector<ElementType>& SparseMatrix<ElementType>::GetElmentList() const
{
    return m_MatrixData->m_ElementList;
}


template<typename ElementType>
inline
const std::vector<int_max>& SparseMatrix<ElementType>::GetColBeginElementIndexInElmentList() const
{
    return m_MatrixData->m_ColBeginElementIndexInElementList;
}


template<typename ElementType>
inline
std::vector<int_max> SparseMatrix<ElementType>::GetRowIndexListInCol(int_max ColIndex) const
{
    if (ColIndex >= m_MatrixData->m_ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetRowIndexListInCol(int_max ColIndex)")
        std::vector<int_max> emptyList;
        return emptyList;
    }

    return m_MatrixData->GetRowIndexListInCol(ColIndex);
}


template<typename ElementType>
inline
ElementType* SparseMatrix<ElementType>::GetPointerOfElementList()
{
    return m_MatrixData->m_ElementList.data();
}


template<typename ElementType>
inline
const ElementType* SparseMatrix<ElementType>::GetPointerOfElementList() const
{
    return m_MatrixData->m_ElementList.data();
}


template<typename ElementType>
inline 
int_max* SparseMatrix<ElementType>::GetPointerOfRowIndexList()
{
    return m_MatrixData->m_RowIndexList.data();
}


template<typename ElementType>
inline
const int_max* SparseMatrix<ElementType>::GetPointerOfRowIndexList() const
{
    return m_MatrixData->m_RowIndexList.data();
}


template<typename ElementType>
inline
int_max* SparseMatrix<ElementType>::GetPointerOfColBeginElementIndexInElementList()
{
    return m_MatrixData->m_ColBeginElementIndexInElementList.data();
}


template<typename ElementType>
inline
const int_max* SparseMatrix<ElementType>::GetPointerOfColBeginElementIndexInElementList() const
{
    return m_MatrixData->m_ColBeginElementIndexInElementList.data();
}


template<typename ElementType>
inline
ElementType* SparseMatrix<ElementType>::GetPointerOfBeginElementInCol(int_max ColIndex)
{
    if (ColIndex >= m_MatrixData->m_ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetPointerOfBeginElementInCol(int_max ColIndex)")
        return nullptr;
    }

    return m_MatrixData->GetPointerOfBeginElementInCol(ColIndex);
}


template<typename ElementType>
inline
const ElementType* SparseMatrix<ElementType>::GetPointerOfBeginElementInCol(int_max ColIndex) const
{
    if (ColIndex >= m_MatrixData->m_ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetPointerOfBeginElementInCol(int_max ColIndex)")
        return nullptr;
    }

    return m_MatrixData->GetPointerOfBeginElementInCol(ColIndex);
}


template<typename ElementType>
inline
int_max SparseMatrix<ElementType>::GetIndexInElementListOfBeginElementInCol(int_max ColIndex) const
{
    if (ColIndex >= m_MatrixData->m_ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetIndexInElementListOfBeginElementInCol(int_max ColIndex)")
        return -1;
    }

    return m_MatrixData->GetIndexInElementListOfBeginElementInCol(ColIndex);
}


template<typename ElementType>
inline
int_max SparseMatrix<ElementType>::GetRowIndexOfBeginElementInCol(int_max ColIndex) const
{
    if (ColIndex >= m_MatrixData->m_ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetRowIndexOfBeginElementInCol(int_max ColIndex)")
        return -1;
    }

    return m_MatrixData->GetRowIndexOfBeginElementInCol(ColIndex);
}


template<typename ElementType>
inline
int_max SparseMatrix<ElementType>::GetRecordedElementNumberInCol(int_max ColIndex) const
{
    if (ColIndex >= m_MatrixData->m_ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetRecordedElementNumberInCol(int_max ColIndex)")
        return 0;
    }

    return m_MatrixData->GetRecordedElementNumberInCol(ColIndex);
}


template<typename ElementType>
inline 
const ElementType& SparseMatrix<ElementType>::GetElement(int_max LinearIndex) const
{ 
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetElement(LinearIndex, InputElement)")
        return m_MatrixData->m_NaNElement;
    }

#endif

    return m_MatrixData->GetElement(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::GetElement(int_max RowIndex, int_max ColIndex) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::GetElement(RowIndex, ColIndex)")
        return m_MatrixData->m_NaNElement;
    }

#endif

    return m_MatrixData->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::SetElement(int_max LinearIndex, const ElementType& InputElement)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::SetElement(LinearIndex, InputElement)")
        return false;
    }

#endif

    m_MatrixData->SetElement(LinearIndex, InputElement);

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::SetElement(int_max RowIndex, int_max ColIndex, const ElementType& InputElement)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid input @ SparseMatrix::SetElement(RowIndex, ColIndex, InputElement)")
        return false;
    }

#endif

    m_MatrixData->SetElement(RowIndex, ColIndex, InputElement);

    return true;
}


//----------- Get/Set Matrix(LinearIndex) -----------------------------------//

// operator[] (): no bound check in release mode

template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::operator[](int_max LinearIndex)
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::operator[](int_max LinearIndex) const
{
    return (*this)(LinearIndex);
}


template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::operator()(int_max LinearIndex)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::operator()(i)")

        return m_MatrixData->m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    return (*m_MatrixData)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::operator()(int_max LinearIndex) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::operator()(i) const")

        return m_MatrixData->m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    // this will call none const ()
    //return (*m_MatrixData)(LinearIndex);

    return m_MatrixData->GetElement(LinearIndex);
}

// at(): bound check

template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::at(int_max LinearIndex)
{
    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::at(i)")
        
        return m_MatrixData->m_NaNElement;
	}

    return (*m_MatrixData)(LinearIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::at(int_max LinearIndex) const
{
    if (LinearIndex >= this->GetElementNumber() || LinearIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::at(i) const")
        
        return m_MatrixData->m_NaNElement;
	}

    // this will call none const ()
    //return (*m_MatrixData)(LinearIndex);

    return m_MatrixData->GetElement(LinearIndex);
}

//----------- Get/Set Matrix(i,j)  ---------------------------------------------//

// operator(): no bound check in release mode

template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::operator()(int_max RowIndex, int_max ColIndex)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::operator()(i,j)")

        return m_MatrixData->m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound
    
    return (*m_MatrixData)(RowIndex, ColIndex);

}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::operator()(int_max RowIndex, int_max ColIndex) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::operator()(i,j) const")

        return m_MatrixData->m_NaNElement;
    }

#endif //MDK_DEBUG_SparseMatrix_Operator_CheckBound

    // this will call none const ()
    //return (*m_MatrixData)(RowIndex, ColIndex);

    return m_MatrixData->GetElement(RowIndex, ColIndex);
}


template<typename ElementType>
inline
ElementType& SparseMatrix<ElementType>::at(int_max RowIndex, int_max ColIndex)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::at(i,j)")
        
        return m_MatrixData->m_NaNElement;
    }
    
    return (*m_MatrixData)(RowIndex, ColIndex);
}


template<typename ElementType>
inline
const ElementType& SparseMatrix<ElementType>::at(int_max RowIndex, int_max ColIndex) const
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::at(i,j) const")
        
        return m_MatrixData->m_NaNElement;
    }

    // this will call none const ()
    //return (*m_MatrixData)(RowIndex, ColIndex);

    return m_MatrixData->GetElement(RowIndex, ColIndex);
}

//---------------------- Get/Set a set of elements ------------------------------------------------------//
/*
// operator(): no bound check in release mode
//
// note: operator[] is for single element access only, operator[{}] is not defined

template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::operator()(const std::initializer_list<int_max>& LinearIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::operator()(std::initializer_list {LinearIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::operator()(std::initializer_list {LinearIndexList})")
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
SparseMatrix<ElementType>::operator()(const std::initializer_list<int_max>& LinearIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::operator()(std::initializer_list {LinearIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::operator()(std::initializer_list {LinearIndexList}) const")
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
SparseMatrix<ElementType>::operator()(const std::vector<int_max>& LinearIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::operator()(std::vector {LinearIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::operator()(std::vector {LinearIndexList})")
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
SparseMatrix<ElementType>::operator()(const std::vector<int_max>& LinearIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::operator()(std::vector {LinearIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::operator()(std::vector {LinearIndexList}) const")
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
SparseMatrix<ElementType>::at(const std::initializer_list<int_max>& LinearIndexList)
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::at(std::initializer_list {LinearIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::at(std::initializer_list {LinearIndexList})")
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
SparseMatrix<ElementType>::at(const std::initializer_list<int_max>& LinearIndexList) const
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::at(std::initializer_list {LinearIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::at(std::initializer_list {LinearIndexList}) const")
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
SparseMatrix<ElementType>::at(const std::vector<int_max>& LinearIndexList)
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::at(std::vector {LinearIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::at(std::vector {LinearIndexList})")
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
SparseMatrix<ElementType>::at(const std::vector<int_max>& LinearIndexList) const
{
    if (LinearIndexList.size() <= 0)
    {
        MDK_Error("LinearIndexList is empty @ SparseMatrix::at(std::vector {LinearIndexList} const)")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto Self_ElementNumber = this->GetElementNumber();

    for (auto it = LinearIndexList.begin(); it != LinearIndexList.end(); ++it)
    {
        if (*it >= Self_ElementNumber || *it < 0)
        {
            MDK_Error("Invalid LinearIndexList @ SparseMatrix::at(std::vector {LinearIndexList} const)")
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
SparseMatrix<ElementType>::operator()(const std::initializer_list<int_max>& RowIndexList, const std::initializer_list<int_max>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList})")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList})")
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
SparseMatrix<ElementType>::operator()(const std::initializer_list<int_max>& RowIndexList, const std::initializer_list<int_max>& ColIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList}) const")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, {ColIndexList}) const")
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
SparseMatrix<ElementType>::operator()(const std::initializer_list<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList is empty @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL)")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL)")
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
SparseMatrix<ElementType>::operator()(const std::initializer_list<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList is empty @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::initializer_list {RowIndexList}, ALL) const")
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
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int_max>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::operator()(ALL, std::initializer_list {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(ALL, std::initializer_list {ColIndexList})")
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
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int_max>& ColIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::operator()(ALL, std::initializer_list {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(ALL, std::initializer_list {ColIndexList}) const")
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
SparseMatrix<ElementType>::operator()(const std::vector<int_max>& RowIndexList, const std::vector<int_max>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})")
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
SparseMatrix<ElementType>::operator()(const std::vector<int_max>& RowIndexList, const std::vector<int_max>& ColIndexList) const 
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList}) const")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(std::vector {RowIndexList}, {ColIndexList})")
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
SparseMatrix<ElementType>::operator()(const std::vector<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::operator()(std::vector {RowIndexList}, ALL)")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::vector {RowIndexList}, ALL)")
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
SparseMatrix<ElementType>::operator()(const std::vector<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::operator()(std::vector {RowIndexList}, ALL) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::operator()(std::vector {RowIndexList}, ALL) const")
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
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int_max>& ColIndexList)
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::operator()(ALL, std::vector {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(ALL, std::vector {ColIndexList})")
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
SparseMatrix<ElementType>::operator()(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int_max>& ColIndexList) const
{
#if defined(MDK_DEBUG_SparseMatrix_Operator_CheckBound)

    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::operator()(ALL, std::vector {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::operator()(ALL, std::vector {ColIndexList}) const")
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
SparseMatrix<ElementType>::at(const std::initializer_list<int_max>& RowIndexList, const std::initializer_list<int_max>& ColIndexList)
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList})")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at({RowIndexList}, {ColIndexList})")
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
SparseMatrix<ElementType>::at(const std::initializer_list<int_max>& RowIndexList, const std::initializer_list<int_max>& ColIndexList) const
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::initializer_list {RowIndexList}, {ColIndexList}) const")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at({RowIndexList}, {ColIndexList})")
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
SparseMatrix<ElementType>::at(const std::initializer_list<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList is empty @ SparseMatrix::at(std::initializer_list {RowIndexList}, ALL)")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::initializer_list {RowIndexList}, ALL)")
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
SparseMatrix<ElementType>::at(const std::initializer_list<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList is empty @ SparseMatrix::at(std::initializer_list {RowIndexList}, ALL) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::initializer_list {RowIndexList}, ALL) const")
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
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int_max>& ColIndexList)
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::at(ALL, std::initializer_list {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at(ALL, std::initializer_list {ColIndexList})")
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
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::initializer_list<int_max>& ColIndexList) const
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::at(ALL, std::initializer_list {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at(ALL, std::initializer_list {ColIndexList}) const")
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
SparseMatrix<ElementType>::at(const std::vector<int_max>& RowIndexList, const std::vector<int_max>& ColIndexList)
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::at(std::vector {RowIndexList}, {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::vector {RowIndexList}, {ColIndexList})")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at(std::vector {RowIndexList}, {ColIndexList})")
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
SparseMatrix<ElementType>::at(const std::vector<int_max>& RowIndexList, const std::vector<int_max>& ColIndexList) const
{
    if (RowIndexList.size() <= 0 || ColIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList or  ColIndexList is empty @ SparseMatrix::at(std::vector {RowIndexList}, {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::vector {RowIndexList}, {ColIndexList}) const")
            SparseShadowMatrix<ElementType> tempShadowMatrix;
            return tempShadowMatrix;
        }
    }

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at(std::vector {RowIndexList}, {ColIndexList}) const")
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
SparseMatrix<ElementType>::at(const std::vector<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol)
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList is empty @ SparseMatrix::at(std::vector {RowIndexList}, ALL)")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::vector {RowIndexList}, ALL)")
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
SparseMatrix<ElementType>::at(const std::vector<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    if (RowIndexList.size() <= 0)
    {
        MDK_Error("RowIndexList is empty @ SparseMatrix::at(std::vector {RowIndexList}, ALL) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::at(std::vector {RowIndexList}, ALL) const")
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
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int_max>& ColIndexList)
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::at(ALL, std::vector {ColIndexList})")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at(ALL, std::vector {ColIndexList})")
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
SparseMatrix<ElementType>::at(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int_max>& ColIndexList) const
{
    if (ColIndexList.size() <= 0)
    {
        MDK_Error("ColIndexList is empty @ SparseMatrix::at(ALL, std::vector {ColIndexList}) const")
        SparseShadowMatrix<ElementType> tempShadowMatrix;
        return tempShadowMatrix;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::at(ALL, std::vector {ColIndexList}) const")
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
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetSubMatrix(const std::vector<int_max>& RowIndexList, const std::vector<int_max>& ColIndexList) const
{
    SparseMatrix<ElementType> tempMatrix; // empty matrix

    GetSubMatrix(tempMatrix, RowIndexList, ColIndexList);

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetSubMatrix(SparseMatrix<ElementType>& OutputMatrix,
                                          const std::vector<int_max>& RowIndexList, 
                                          const std::vector<int_max>& ColIndexList) const
{
    if (this == &OutputMatrix)
    {
        MDK_Error("A Matrix tries to get a SubMatrix of itself @ SparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)")
        return false;
    }

    auto OutputRowNumber = int_max(RowIndexList.size());
    
    auto OutputColNumber = int_max(ColIndexList.size());

    if (OutputRowNumber <= 0 || OutputColNumber <= 0)
    {
        MDK_Error("RowIndexList or ColIndexList is empty @ SparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)")
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Invalid ColIndexList @ SparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)")
            return false;
        }
    }

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid RowIndexList @ SparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)")
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
            MDK_Error("Invalid OutputMatrix @ SparseMatrix::GetSubMatrix(OutputMatrix, RowIndexList, ColIndexList)")
            return false;
        }
    }


    //------------------------------

    return true;
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetSubMatrix(const std::vector<int_max>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol) const
{
    auto SelfSize = this->GetSize();

    std::vector<int_max> ColIndexList(SelfSize.ColNumber);

    for (int_max i = 0; i < SelfSize.ColNumber; ++i)
    {
        ColIndexList[i] = i;
    }

    return this->GetSubMatrix(RowIndexList, ColIndexList);
}

template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetSubMatrix(const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int_max>& ColIndexList) const
{
    auto SelfSize = this->GetSize();

    std::vector<int_max> RowIndexList(SelfSize.RowNumber);

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
    {
        RowIndexList[i] = i;
    }

    return this->GetSubMatrix(RowIndexList, ColIndexList);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Col(int_max ColIndex)
{
    return this->at(ALL, { ColIndex });
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Col(std::initializer_list<int_max>& ColIndexList)
{
    return this->at(ALL, ColIndexList);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Col(const std::vector<int_max>& ColIndexList)
{
    return this->at(ALL, ColIndexList);
}


template<typename ElementType>
inline 
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetCol(int_max ColIndex) const
{
	SparseMatrix<ElementType> tempMatrix;

    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::GetCol(ColIndex)")
        
        return tempMatrix;
	}

    tempMatrix.Resize(SelfSize.RowNumber, 1);

//--------------------------------------

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetCol(int_max ColIndex, DenseMatrix<ElementType>& ColData) const
{
    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::GetCol(int_max ColIndex, DenseMatrix<ElementType>& ColData)")
        return false;
    }

    ColData.resize(SelfSize.RowNumber);

    //-----------------------------------------------

    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::GetCol(int_max ColIndex, std::vector<ElementType>& ColData) const
{
    auto SelfSize = this->GetSize();

	if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::GetCol(int_max ColIndex, std::vector<ElementType>& ColData)")
		return false;
	}

	ColData.resize(SelfSize.RowNumber);

//-----------------------------------------------

	return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::GetCol(int_max ColIndex, ElementType* ColData) const
{
    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0 || ColData == nullptr)
	{
		MDK_Error("Invalid Input @ SparseMatrix::GetCol(int_max ColIndex, ElementType* ColData)")
		return false;
	}

	auto RawPointer this->GetElementPointer();

	int_max Index = ColIndex*SelfSize.RowNumber;

	for (int_max i = 0; i < SelfSize.RowNumber; ++i)
	{
		ColData[i] = RawPointer[Index + i];
	}

	return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetCol(int_max ColIndex, const SparseMatrix<ElementType_Input>& ColData)
{
    auto SelfSize = this->GetSize();

	if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error("Invalid Input : ColIndex is out of bound @ SparseMatrix::SetCol(ColIndex, mdkSparseMatrix)")
		return false;
	}

	auto Size = ColData.GetSize();

	if (Size.ColNumber != 1 || Size.RowNumber != 1)
	{
		MDK_Error("Invalid Input : must be a vector @ SparseMatrix::SetCol(ColIndex, mdkSparseMatrix)")
		return false;
	}

	if (Size.RowNumber > 1 && Size.RowNumber != SelfSize.RowNumber)
	{
		MDK_Error("Invalid Input : size does not match @ SparseMatrix::SetCol(ColIndex, mdkSparseMatrix)")
		return false;
	}

	if (Size.ColNumber > 1 && Size.ColNumber != SelfSize.RowNumber)
	{
		MDK_Error("Invalid Input : size does not match @ SparseMatrix::SetCol(ColIndex, mdkSparseMatrix)")
		return false;
	}

    return this->SetCol(ColIndex, ColData.GetElementPointer(), std::max(Size.RowNumber, Size.ColNumber));
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetCol(int_max ColIndex, const std::initializer_list<ElementType_Input>& ColData)
{
    return this->SetCol(ColIndex, ColData.begin(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetCol(int_max ColIndex, const std::vector<ElementType_Input>& ColData)
{
    return this->SetCol(ColIndex, ColData.data(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetCol(int_max ColIndex, const ElementType_Input* ColData, int_max Length)
{
    auto SelfSize = this->GetSize();

    if (ColData == nullptr || Length <= 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::SetCol(ColIndex, const ElementType_Input* ColData, int_max Length)")
		return false;
	}
    
    auto BeginPointer = this->GetElementPointer();

    int_max Index = ColIndex*SelfSize.RowNumber;

    // just for reference:
    //
    //for (int_max i = 0; i < SelfSize.RowNumber; ++i)
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
bool SparseMatrix<ElementType>::FillCol(int_max ColIndex, const ElementType& Element)
{
    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::FillCol(int_max ColIndex, const ElementType& Element)")
        return false;
    }

    auto BeginPointer = this->GetElementPointer();

    int_max Index = ColIndex*SelfSize.RowNumber;

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
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::AppendCol(const mdkSparseMatrix<ElementType_Input>& ColData)")
        return false;
    }

    if (ColData.IsEmpty() == true)
    {
        MDK_Error("Empty Input @ SparseMatrix::AppendCol(const mdkSparseMatrix<ElementType_Input>& ColData)")
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
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::AppendCol(const std::initializer_list<ElementType_Input>& ColData)")
        return false;
    }

    if (ColData.size() <= 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::AppendCol(const std::initializer_list<ElementType_Input>& ColData)")
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
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::AppendCol(const std::vector<ElementType_Input>& ColData)")
        return false;
    }

    if (ColData.size() <= 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::AppendCol(const std::vector<ElementType_Input>& ColData)")
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
bool SparseMatrix<ElementType>::AppendCol(const ElementType_Input* ColData, int_max Length)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::AppendCol(const ElementType_Input* ColData, int_max Length)")
        return false;
    }

    if (ColData == nullptr || Length <= 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::AppendCol(const ElementType_Input* ColData, int_max Length)")
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
bool SparseMatrix<ElementType>::DeleteCol(int_max ColIndex)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::DeleteCol(int_max ColIndex)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::DeleteCol(int_max ColIndex)")
        return false;
    }

    m_MatrixData->ElementList.erase(m_MatrixData->ElementList.begin() + ColIndex*SelfSize.RowNumber, 
                                  m_MatrixData->ElementList.begin() + (ColIndex + 1)*SelfSize.RowNumber);

    m_MatrixData->ColNumber -= 1;

    m_ElementPointer = m_MatrixData->ElementList.data();

    return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteCol(const std::initializer_list<int_max>& ColIndexList)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::DeleteCol(std::initializer_list<int_max>& ColIndexList)")
        return false;
    }

    if (ColIndexList.size() <= 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::DeleteCol(std::initializer_list ColIndexList)")
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Out of bound Input @ SparseMatrix::DeleteCol(std::initializer_list ColIndexList)")
            return false;
        }
    }

    return this->DeleteCol(ColIndexList.begin(), ColIndexList.size());
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteCol(const std::vector<int_max>& ColIndexList)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::DeleteCol(const std::vector<int_max>& ColIndexList)")
        return false;
    }

    if (ColIndexList.size() <= 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::DeleteCol(std::vector ColIndexList)")
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = ColIndexList.begin(); it != ColIndexList.end(); ++it)
    {
        if (*it >= SelfSize.ColNumber || *it < 0)
        {
            MDK_Error("Out of bound Input @ SparseMatrix::DeleteCol(std::vector ColIndexList)")
            return false;
        }
    }

    return this->DeleteCol(ColIndexList.data(), ColIndexList.size());
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteCol(const int_max* ColIndexListPtr, int_max Length)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::DeleteCol(const int_max* ColIndexPtr, int_max Length)")
        return false;
    }

    if (ColIndexListPtr == nullptr || Length <= 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::DeleteCol(const int_max* ColIndexPtr, int_max Length)")
        return false;
    }

    if (Length == 1)
    {
        return this->DeleteCol(ColIndexListPtr[0]);
    }

    // Length > 1 -------------------------------------------

    auto SelfSize = this->GetSize();

    std::vector<int_max> ColIndexList_max_to_min(Length);

    for (int_max i = 0; i < Length; ++i)
    {
        ColIndexList_max_to_min[i] = ColIndexListPtr[i];
    }

    std::sort(ColIndexList_max_to_min.begin(), ColIndexList_max_to_min.end(), [](int_max a, int_max b) { return a > b; });

    int_max Index_prev = ColIndexList_max_to_min[0] + 1;

    for (int_max i = 0; i < int_max(ColIndexList_max_to_min.size()); ++i)
    {
        auto Index_i = ColIndexList_max_to_min[i];

        if (Index_i == Index_prev)
        {
            MDK_Warning("duplicate Input @ SparseMatrix::DeleteCol(const int_max* ColIndexPtr, int_max Length)")
        }
        else
        {
            m_MatrixData->ElementList.erase(m_MatrixData->ElementList.begin() + Index_i * SelfSize.RowNumber,
                                          m_MatrixData->ElementList.begin() + (Index_i + 1)* SelfSize.RowNumber);

            Index_prev = Index_i;

            m_MatrixData->ColNumber -= 1;
        }
    }

    m_ElementPointer = m_MatrixData->ElementList.data();

    return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int_max ColIndex, const SparseMatrix<ElementType_Input>& ColData)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)")
        return false;
    }

    if (ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input : ColIndex is out of bound @ SparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)")
        return false;
    }

    auto InputSize = ColData.GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
    {
        MDK_Error("Invalid Input : must be a vector @ SparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.RowNumber)
    {
        MDK_Error("Invalid Input : size does not match @ SparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)")
        return false;
    }

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.RowNumber)
    {
        MDK_Error("Invalid Input : size does not match @ SparseMatrix::InsertCol(ColIndex, mdkSparseMatrix)")
        return false;
    }

    return this->InsertCol(ColIndex, ColData.GetElementPointer(), std::max(InputSize.RowNumber, InputSize.ColNumber));

}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int_max ColIndex, const std::initializer_list<ElementType_Input>& ColData)
{
    return this->InsertCol(ColIndex, ColData.begin(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int_max ColIndex, const std::vector<ElementType_Input>& ColData)
{
    return this->InsertCol(ColIndex, ColData.data(), ColData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertCol(int_max ColIndex, const ElementType_Input* ColData, int_max Length)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Matrix Size can not change @ SparseMatrix::InsertCol(ColIndex, const ElementType_Input* ColData, int_max Length)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (ColData == nullptr || Length <= 0 || ColIndex >= SelfSize.ColNumber || ColIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::InsertCol(ColIndex, const ElementType_Input* ColData, int_max Length)")
        return false;
    }

    m_MatrixData->ElementList.insert(m_MatrixData->ElementList.begin() + ColIndex*SelfSize.RowNumber, ColData, ColData + Length);

    m_MatrixData->ColNumber += 1;

    m_ElementPointer = m_MatrixData->ElementList.data();

    return true;
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Row(int_max RowIndex)
{
    return this->at({RowIndex}, ALL);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Row(std::initializer_list<int_max>& RowIndexList)
{
    return this->at(RowIndexList, ALL);
}


template<typename ElementType>
inline
SparseShadowMatrix<ElementType>
SparseMatrix<ElementType>::Row(const std::vector<int_max>& RowIndexList)
{
    return this->at(RowIndexList, ALL);
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::GetRow(int_max RowIndex) const
{
    SparseMatrix<ElementType> tempMatrix;	

    auto SelfSize = this->GetSize();

	if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::GetRow(RowIndex)")
        
        return tempMatrix;
	}

    tempMatrix.Resize(1, SelfSize.ColNumber);

    auto tempRawPointer = tempMatrix.GetElementPointer();

    auto RawPointer = this->GetElementPointer();

	int_max Index = 0;

	for (int_max j = 0; j < SelfSize.ColNumber; ++j)
	{
		tempRawPointer[j] = RawPointer[Index + RowIndex];

		Index += SelfSize.RowNumber;
	}

    return tempMatrix;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetRow(int_max RowIndex, std::vector<ElementType>& RowData) const
{
    auto SelfSize = this->GetSize();

	if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
	{
		MDK_Error("Invalid Input @ SparseMatrix::GetRow(RowIndex, std::vector<ElementType>& RowData)")
		return false;
	}

	RowData.resize(SelfSize.ColNumber);

	auto tempRawPointer = RowData.data();

    auto RawPointer = this->GetElementPointer();

	int_max Index = 0;

	for (int_max j = 0; j < SelfSize.ColNumber; ++j)
	{
		tempRawPointer[j] = RawPointer[Index + RowIndex];

		Index += SelfSize.RowNumber;
	}

	return true;
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::GetRow(int_max RowIndex, ElementType* RowData) const
{
    auto SelfSize = this->GetSize();

	if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
	{
		MDK_Error("Invalid Input @ mdkSparseMatrix GetRow(RowIndex, ElementType* RowData)")
		return false;
	}

	auto RawPointer this->GetElementPointer();

	int_max Index = 0;

	for (int_max j = 0; j < SelfSize.ColNumber; ++j)
	{
		RowData[j] = RawPointer[Index + RowIndex];

		Index += SelfSize.RowNumber;
	}

	return true;
}


template<typename ElementType>
template<typename ElementType_Input>
inline 
bool SparseMatrix<ElementType>::SetRow(int_max RowIndex, const SparseMatrix<ElementType_Input>& RowData)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error("Invalid Input @ mdkSparseMatrix SetRow(int_max RowIndex, const mdkSparseMatrix<ElementType_Input>& RowData)")
        return false;
    }

	auto InputSize = RowData.GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
	{
		MDK_Error("Invalid Input @ mdkSparseMatrix SetRow(RowIndex,mdkSparseMatrix)")
		return false;
	}

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.ColNumber)
	{
		MDK_Error("Invalid Input @ mdkSparseMatrix SetRow(RowIndex,mdkSparseMatrix)")
		return false;
	}

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.ColNumber)
	{
		MDK_Error("Invalid Input @ mdkSparseMatrix SetRow(RowIndex,mdkSparseMatrix)")
		return false;
	}

    return this->SetRow(RowIndex, RowData.GetElementPointer(), std::max(InputSize.RowNumber, InputSize.ColNumber));
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetRow(int_max RowIndex, const std::initializer_list<ElementType_Input>& RowData)
{
    return this->SetRow(RowIndex, RowData.begin(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetRow(int_max RowIndex, const std::vector<ElementType_Input>& RowData)
{
    return this->SetRow(RowIndex, RowData.data(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::SetRow(int_max RowIndex, const ElementType_Input* RowData, int_max Length)
{
    auto SelfSize = this->GetSize();

    if (RowData == nullptr || Length <= 0 || RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::SetRow(RowIndex, const ElementType_Input* RowData, int_max Length)")
        return false;
    }

    if (Length != SelfSize.ColNumber)
    {
        MDK_Error("Length does not match @ SparseMatrix::SetRow(RowIndex, const ElementType_Input* RowData, int_max Length)")
        return false;
    }

    auto RawPointer = this->GetElementPointer();

    int_max Index = 0;

    for (int_max j = 0; j < SelfSize.ColNumber; ++j)
    {
        RawPointer[Index + RowIndex] = ElementType(RowData[j]);

        Index += SelfSize.RowNumber;
     }

    return true;
}


template<typename ElementType>
inline 
bool SparseMatrix<ElementType>::FillRow(int_max RowIndex, const ElementType& Element)
{
    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::FillRow(int_max RowIndex, const ElementType& Element)")
        return false;
    }

    auto RawPointer = this->GetElementPointer();

    int_max Index = 0;

    for (int_max j = 0; j < SelfSize.ColNumber; ++j)
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
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::AppendRow(const mdkSparseMatrix<ElementType_Input>& RowData)")
        return false;
    }

    if (RowData.IsEmpty() == true)
    {
        MDK_Error("Input is empty @ SparseMatrix::AppendRow(const mdkSparseMatrix<ElementType_Input>& RowData)")
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
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::AppendRow(const std::initializer_list<ElementType_Input>& RowData)")
        return false;
    }

    if (RowData.size() <= 0)
    {
        MDK_Error("Input is empty @ SparseMatrix::AppendRow(const std::initializer_list<ElementType_Input>& RowData)")
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
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::AppendRow(const std::vector<ElementType_Input>& RowData)")
        return false;
    }

    if (RowData.size() <= 0)
    {
        MDK_Error("Input is empty @ SparseMatrix::AppendRow(const std::vector<ElementType_Input>& RowData)")
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
bool SparseMatrix<ElementType>::AppendRow(const ElementType_Input* RowData, int_max Length)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::AppendRow(const ElementType_Input* RowData, int_max Length)")
        return false;
    }

    if (RowData == nullptr || Length <= 0)
    {
        MDK_Error("Input is empty @ SparseMatrix::AppendRow(const ElementType_Input* RowData, int_max Length)")
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
bool SparseMatrix<ElementType>::DeleteRow(int_max RowIndex)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::DeleteRow(int_max RowIndex)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::DeleteRow(int_max RowIndex)")
        return false;
    }

    return this->DeleteRow(&RowIndex, 1);
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteRow(const std::vector<int_max>& RowIndexList)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::DeleteRow(const std::vector<int_max>& RowIndexList)")
        return false;
    }

    auto SelfSize = this->GetSize();

    for (auto it = RowIndexList.begin(); it != RowIndexList.end(); ++it)
    {
        if (*it >= SelfSize.RowNumber || *it < 0)
        {
            MDK_Error("Invalid Input @ SparseMatrix::DeleteRow(const std::vector<int_max>& RowIndexList)")
            return false;
        }
    }

    return this->DeleteRow(RowIndexList.data(), RowIndexList.size());
}


template<typename ElementType>
inline
bool SparseMatrix<ElementType>::DeleteRow(const int_max* RowIndexListPtr, int_max Length)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::DeleteRow(const int_max* RowIndexPtr, int_max Length)")
        return false;
    }

    if (RowIndexListPtr == nullptr || Length <= 0)
    {
        MDK_Error("Invalid Input @ SparseMatrix::DeleteRow(const int_max* RowIndexPtr, int_max Length)")
        return false;
    }

    auto SelfSize = this->GetSize();

    std::vector<int_max> CounterList(SelfSize.RowNumber);
    
    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
    {
        CounterList[i] = 0;
    }

    for (int_max i = 0; i < Length; ++i)
    {
        CounterList[RowIndexListPtr[i]] = 1;
    }

    std::vector<int_max> RowIndexList_output;

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
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
bool SparseMatrix<ElementType>::InsertRow(int_max RowIndex, const SparseMatrix<ElementType_Input>& RowData)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::InsertRow(int_max RowIndex, const mdkSparseMatrix<ElementType_Input>& RowData)")
        return false;
    }

    if (RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error("Invalid Input @ mdkSparseMatrix InsertRow(int_max RowIndex, const mdkSparseMatrix<ElementType_Input>& RowData)")
        return false;
    }

    auto InputSize = RowData.GetSize();

    auto SelfSize = this->GetSize();

    if (InputSize.ColNumber != 1 || InputSize.RowNumber != 1)
    {
        MDK_Error("Invalid Input @ mdkSparseMatrix InsertRow(RowIndex,mdkSparseMatrix)")
        return false;
    }

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error("Invalid Input @ mdkSparseMatrix InsertRow(RowIndex,mdkSparseMatrix)")
        return false;
    }

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.ColNumber)
    {
        MDK_Error("Invalid Input @ mdkSparseMatrix InsertRow(RowIndex,mdkSparseMatrix)")
        return false;
    }

    return this->InsertRow(RowIndex, RowData.GetElementPointer(), std::max(Size.RowNumber, Size.ColNumber));
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int_max RowIndex, const std::initializer_list<ElementType_Input>& RowData)
{
    return this->InsertRow(RowIndex, RowData.begin(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int_max RowIndex, const std::vector<ElementType_Input>& RowData)
{
    return this->InsertRow(RowIndex, RowData.data(), RowData.size());
}


template<typename ElementType>
template<typename ElementType_Input>
inline
bool SparseMatrix<ElementType>::InsertRow(int_max RowIndex, const ElementType_Input* RowData, int_max Length)
{
    if (this->IsSizeFixed() == true)
    {
        MDK_Error("Size can not change @ SparseMatrix::InsertRow(RowIndex, const ElementType_Input* RowData, int_max Length)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (RowData == nullptr || Length <= 0 || RowIndex >= SelfSize.RowNumber || RowIndex < 0)
    {
        MDK_Error("Empty Input @ SparseMatrix::InsertRow(RowIndex, const ElementType_Input* RowData, int_max Length)")
        return false;
    }

    if (Length != SelfSize.ColNumber)
    {
        MDK_Error("Length does not match @ SparseMatrix::InsertRow(RowIndex, const ElementType_Input* RowData, int_max Length)")
        return false;
    }

    SparseMatrix<ElementType> tempMatrix(SelfSize.RowNumber + 1, SelfSize.ColNumber);

    auto tempRawPointer = tempMatrix.GetElementPointer();

    auto RawPointer = this->GetElementPointer();

    for (int_max i = 0; i < RowIndex; ++i)
    {
        for (int_max j = 0; j < SelfSize.ColNumber; ++j)
        {
            tempRawPointer[j*(SelfSize.RowNumber + 1) + i] = RawPointer[j*SelfSize.RowNumber + i];
        }
    }

    for (int_max j = 0; j < SelfSize.ColNumber; ++j)
    {
        tempRawPointer[j*(SelfSize.RowNumber + 1) + RowIndex] = ElementType(RowData[j]);
    }

    for (int_max i = RowIndex + 1; i < SelfSize.RowNumber + 1; ++i)
    {
        for (int_max j = 0; j < SelfSize.ColNumber; ++j)
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
        MDK_Error(" Self is empty or not square @ SparseMatrix::Diangonal")
        SparseShadowMatrix<ElementType> tempShadowMatrix;     
        return  tempShadowMatrix;
    }

    std::vector<int_max> LinearIndexList(SelfSize.ColNumber);

    int_max Index = 0;

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
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
        MDK_Error(" Self is empty or not square @ SparseMatrix::GetDiangonal()")
        
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
        MDK_Error(" Self is empty or not square @ SparseMatrix::GetDiangonal(std::vector)")
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
        MDK_Error(" Input is nullptr @ SparseMatrix::GetDiangonal(ElementType* DiangonalData)")
        return  false;
    }

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error(" Self is empty or not square @ SparseMatrix::GetDiangonal(ElementType* DiangonalData)")
        return  false;
    }

    auto RawPointer this->GetElementPointer();

    int_max Index = 0;

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
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
		MDK_Error("Invalid Input @ SparseMatrix::SetDiangonal(Matrix)")
		return false;
	}

    auto SelfSize = this->GetSize();

    if (InputSize.RowNumber > 1 && InputSize.RowNumber != SelfSize.ColNumber)
	{
		MDK_Error("Invalid Input @ SparseMatrix::SetDiangonal(Matrix)")
		return false;
	}

    if (InputSize.ColNumber > 1 && InputSize.ColNumber != SelfSize.ColNumber)
	{
		MDK_Error("Invalid Input @ SparseMatrix::SetDiangonal(Matrix)")
		return false;
	}

    if (InputSize.RowNumber == 1 && InputSize.ColNumber == 1)
    {
        MDK_Warning("Input matrix is 1x1 and treated as a scalar @ SparseMatrix::SetDiangonal(Matrix)")
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
        MDK_Error("Self is empty or not square @ SparseMatrix::SetDiangonal(std::initializer_list)")
        return false;
    }

    if (DiangonalData.size() != SelfSize.RowNumber)
    {
        MDK_Error("Invalid Input @ SparseMatrix::SetDiangonal(std::initializer_list)")
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
		MDK_Error(" Self is empty or not square @ SparseMatrix::SetDiangonal(std::vector)")
		return false;
	}

    if (DiangonalData.size() != SelfSize.RowNumber)
    {
        MDK_Error(" Input is invalid @ SparseMatrix::SetDiangonal(std::vector)")
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
        MDK_Error(" Input is nullptr @ SparseMatrix::SetDiangonal(const ElementType_Input* DiangonalData)")
        return false;
    }

    auto SelfSize = this->GetSize();

    if (SelfSize.RowNumber <= 0 || SelfSize.RowNumber != SelfSize.ColNumber)
    {
        MDK_Error(" Self is empty or not square @ SparseMatrix::SetDiangonal(const ElementType_Input* DiangonalData)")
        return false;
    }

    auto RawPointer = this->GetElementPointer();

    int_max Index = 0;

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
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
        MDK_Error(" Self is empty or not square @ SparseMatrix::FillDiangonal")
        return false;
    }

    auto RawPointer this->GetElementPointer();

    int_max Index = 0;

    for (int_max i = 0; i < SelfSize.RowNumber; ++i)
    {
        RawPointer[Index + i] = Element;

        Index += SelfSize.RowNumber;
    }

    return true;
}
*/

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

/*
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
        MDK_Error("ShadowMatrix is empty @ SparseMatrix::operator+=(ShadowMatrix)")
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
        MDK_Error("Size does not match @ SparseMatrix::operator+=(ShadowMatrix)")
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
        MDK_Error("ShadowMatrix is empty @ SparseMatrix::operator-=(ShadowMatrix)")
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
        MDK_Error("Size does not match @ SparseMatrix::operator-=(ShadowMatrix)")
        return;
    }

    //----------------
}


template<typename ElementType>
inline 
void SparseMatrix<ElementType>::operator*=(const SparseShadowMatrix<ElementType>& ShadowMatrix)
{
    this->operator*=(SparseShadowMatrix.ConstructMatrix());
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
    this->operator*=(GlueMatrix.ConstructMatrix());
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
        MDK_Error("Self is empty @ SparseMatrix::operator^(value)")

        return tempMatrix;
    }

    tempMatrix.Copy(*this);

    auto tempRawPointer = tempMatrix.GetNonZeroElementPointer();

    auto RawPointer = this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInElementList();

    for (int_max i = 0; i < RecordedElementNumber; ++i)
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
        MDK_Error("Self is empty @ SparseMatrix::operator^(value)")
        return;
    }

    auto BeginPointer this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInElementList();

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
    return MatrixElementMultiply(*this, ShadowMatrix.ConstructSparseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const SparseGlueMatrixForLinearCombination<ElementType>& GlueMatrix) const
{
    return MatrixElementMultiply(*this, GlueMatrix.ConstructSparseMatrix());
}


template<typename ElementType>
inline
SparseMatrix<ElementType> SparseMatrix<ElementType>::ElementMultiply(const SparseGlueMatrixForMultiplication<ElementType>& GlueMatrix) const
{
    return MatrixElementMultiply(*this, GlueMatrix.ConstructSparseMatrix());
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
        MDK_Error("self is empty Matrix @ SparseMatrix::Mean")
        return m_NaNElement;
    }

    auto RawPointer = this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInElementList();

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
		MDK_Error("self is empty Matrix @ SparseMatrix::MeanToRow")

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
		MDK_Error("self is empty Matrix @ SparseMatrix::MeanToCol")
  
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
		MDK_Error("self is empty Matrix @ SparseMatrix::Sum")
        return m_NaNElement;
	}

    auto RawPointer = this->GetNonZeroElementPointer();

    auto RecordedElementNumber = this->GetElementNumberInElementList();

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
		MDK_Error("self is empty Matrix @ SparseMatrix::SumToRow")
        
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
		MDK_Error("self is empty Matrix @ SparseMatrix::SumToCol")
        
		return tempMatrix;
	}

    //-------------------------------------------
    
	return tempMatrix;
}


template<typename ElementType>
inline ElementType SparseMatrix<ElementType>::Max() const
{
    auto RecordedElementNumber = this->GetElementNumberInElementList();

    if (RecordedElementNumber <= 0)
	{
		MDK_Error("self is empty Matrix @ SparseMatrix::Max")
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
		MDK_Error("self is empty Matrix @ SparseMatrix::MaxToRow")
        
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
		MDK_Error("self is empty Matrix @ SparseMatrix::MaxToCol")
        
		return tempMatrix;
	}

//------------------------------------------------

	return tempMatrix;
}


template<typename ElementType>
inline ElementType SparseMatrix<ElementType>::Min() const
{
    auto RecordedElementNumber = this->GetElementNumberInElementList();

    if (RecordedElementNumber <= 0)
	{
		MDK_Error("self is empty Matrix @ SparseMatrix::Min")
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
		MDK_Error("self is empty Matrix @ SparseMatrix::MinToRow")
        
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
		MDK_Error("self is empty Matrix @ SparseMatrix::MinToCol")
        
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
void SparseMatrix<ElementType>::TransposeInPlace()
{
    return MatrixTransposeInPlace(*this);
}


template<typename ElementType>
inline
int_max SparseMatrix<ElementType>::Rank() const
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
*/

}//end namespace mdk
