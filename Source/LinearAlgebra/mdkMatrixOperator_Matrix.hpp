#ifndef __mdkMatrixOperator_Matrix_hpp
#define __mdkMatrixOperator_Matrix_hpp

namespace mdk
{

//============================================================================================================================================//
//
//                                                    {Matrix} v.s {Matrix, Element}
//
//============================================================================================================================================//

// ----------------------------------------------------- Matrix  {+ - * /}  Matrix ---------------------------------------------------------//

// note: Matrix {+ -} Matrix return GlueMatrixForLinearCombination
// note: Matrix {*}   Matrix return MultiplyGlueMatrix
// note: Matrix {/}   Matrix return Matrix

template<typename ElementType>
inline 
mdkGlueMatrixForLinearCombination<ElementType> operator+(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    auto SizeA = MatrixA.GetSize();

    auto SizeB = MatrixB.GetSize();

    if (SizeA.ColNumber == 1 && SizeA.RowNumber == 1)
    {
        return MatrixA(0) + MatrixB;
    }

    if (SizeB.ColNumber == 1 && SizeB.RowNumber == 1)
    {
        return MatrixA + MatrixB(0);
    }

    mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;

    if (SizeA.RowNumber != SizeB.RowNumber || SizeA.ColNumber != SizeB.ColNumber)
    {
        mdkError << "Size does not match @ mdkMatrixOperator: +(MatrixA, MatrixB)" << '\n';
        mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;
        return  tempGlueMatrix;
    }

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkWarning << "MatrixA or MatrixB is empty @ mdkMatrixOperator: +(MatrixA, MatrixB)" << '\n';
        mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;
        return  tempGlueMatrix;
    }


    tempGlueMatrix.m_RowNumber = SizeA.RowNumber;

    tempGlueMatrix.m_ColNumber = SizeA.ColNumber;

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixA.GetElementDataSharedPointer());

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixB.GetElementDataSharedPointer());

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(1));

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(1));

    return tempGlueMatrix
}


template<typename ElementType>
inline 
mdkGlueMatrixForLinearCombination<ElementType> operator-(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    auto SizeA = MatrixA.GetSize();

    auto SizeB = MatrixB.GetSize();

    if (SizeA.ColNumber == 1 && SizeA.RowNumber == 1)
    {
        return MatrixA(0) - MatrixB;
    }

    if (SizeB.ColNumber == 1 && SizeB.RowNumber == 1)
    {
        return MatrixA - MatrixB(0);
    }

    mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;

    if (SizeA.RowNumber != SizeB.RowNumber || SizeA.ColNumber != SizeB.ColNumber)
    {
        mdkError << "Size does not match @ mdkMatrixOperator: -(MatrixA, MatrixB)" << '\n';

        return  tempGlueMatrix;
    }

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkWarning << "MatrixA or MatrixB is empty @ mdkMatrixOperator: -(MatrixA, MatrixB)" << '\n';

        return  tempGlueMatrix;
    }
  

    tempGlueMatrix.m_RowNumber = SizeA.RowNumber;

    tempGlueMatrix.m_ColNumber = SizeA.ColNumber;

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixA.GetElementDataSharedPointer());

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixB.GetElementDataSharedPointer());

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(1));

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(-1));

    return tempGlueMatrix;
}


template<typename ElementType>
inline mdkGlueMatrixForMultiplication<ElementType> operator*(const mdkMatrix<ElementType>& MatrixA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkGlueMatrixForMultiplication<ElementType> tempGlueMatrix;

    auto SizeA = MatrixA.GetSize();

    auto SizeB = MatrixB.GetSize();

    if (SizeA.ColNumber != SizeB.RowNumber)
    {
        mdkError << "Size does not match @ mdkMatrixOperator: *(MatrixA, MatrixB)" << '\n';

        return  tempGlueMatrix;
    }

    if (SizeA.RowNumber == 0 || SizeB.RowNumber == 0)
    {
        mdkWarning << "MatrixA or MatrixB is empty @ mdkMatrixOperator: *(MatrixA, MatrixB)" << '\n';

        return  tempGlueMatrix;
    }

    tempGlueMatrix.m_RowNumber = SizeA.RowNumber;

    tempGlueMatrix.m_ColNumber = SizeB.ColNumber;

    tempGlueMatrix.m_SourceMatrixSharedCopyList.resize(2);

    tempGlueMatrix.m_SourceMatrixSharedCopyList[0].SharedCopy(MatrixA);

    tempGlueMatrix.m_SourceMatrixSharedCopyList[1].SharedCopy(MatrixB);

    return tempGlueMatrix;
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
        mdkWarning << "MatrixA or MatrixB is empty @ mdkMatrixOperator: /(MatrixA, MatrixB)" << '\n';

        return  tempMatrix;
    }

    if (SizeA.ColNumber == 1 && SizeA.RowNumber == 1)
    {
        tempMatrix = MatrixB.ElementOperation("/", MatrixA(0));

        return tempMatrix;
    }

    if (SizeB.ColNumber == 1 && SizeB.RowNumber == 1)
    {
        tempMatrix = MatrixA.ElementOperation("/", MatrixB(0));

        return tempMatrix;
    }

    if (SizeA.RowNumber != SizeB.RowNumber || SizeA.ColNumber != SizeB.ColNumber)
    {
        mdkError << "Size does not match @ mdkMatrixOperator: /(MatrixA, MatrixB)" << '\n';

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

// ---------------------------------------------------- Matrix  {+ - * /}  Element ------------------------------------------------------//

// note: Matrix {+ -}  Element return GlueMatrixForLinearCombination
// note: Matrix {* /}  Element return GlueMatrixForMultiplication  (definition in mdkGlueMatrixForMultiplication.h)

template<typename ElementType>
inline 
mdkGlueMatrixForLinearCombination<ElementType> operator+(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixA.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixA.GetColNumber();

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixA.GetElementDataSharedPointer());

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(1));

    tempGlueMatrix.m_IndependentElement = ElementB;

    return tempGlueMatrix;
}


template<typename ElementType>
inline 
mdkGlueMatrixForLinearCombination<ElementType> operator-(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixA.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixA.GetColNumber();

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixA.GetElementDataSharedPointer());

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(1));

    tempGlueMatrix.m_IndependentElement = -ElementB;

    return tempGlueMatrix;
}


template<typename ElementType>
inline 
GlueMatrixForMultiplication<ElementType> operator*(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkGlueMatrixForMultiplication<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixA.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixA.GetColNumber();

    tempGlueMatrix.m_SourceMatrixSharedCopyList.resize(1);

    tempGlueMatrix.m_SourceMatrixSharedCopyList[0].SharedCopy(MatrixA);

    tempGlueMatrix.m_Element_Coef = ElementB;

    return tempGlueMatrix;
}


template<typename ElementType>
inline 
GlueMatrixForMultiplication<ElementType> operator/(const mdkMatrix<ElementType>& MatrixA, const ElementType& ElementB)
{
    mdkGlueMatrixForMultiplication<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixA.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixA.GetColNumber();

    tempGlueMatrix.m_SourceMatrixSharedCopyList.resize(1);

    tempGlueMatrix.m_SourceMatrixSharedCopyList[0].SharedCopy(MatrixA);

    tempGlueMatrix.m_Element_Coef = ElementType(1) / ElementB;

    return tempGlueMatrix;
}

// --------------------------------------------------- Element {+ - * /} Matrix  ---------------------------------------------------------//

// note: Element {+ -} Matrix return GlueMatrixForLinearCombination
// note: Element {*}   Matrix return GlueMatrixForMultiplication
// note: Element {/}   Matrix return Matrix

template<typename ElementType>
inline 
mdkGlueMatrixForLinearCombination<ElementType> operator+(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixB.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixB.GetColNumber();

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixB.GetElementDataSharedPointer());

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(1));

    tempGlueMatrix.m_IndependentElement = ElementA;

    return tempGlueMatrix;
}


template<typename ElementType>
inline
mdkGlueMatrixForLinearCombination<ElementType> operator-(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkGlueMatrixForLinearCombination<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixB.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixB.GetColNumber();

    tempGlueMatrix.m_MatrixElementDataSharedPointerList.push_back(MatrixB.GetElementDataSharedPointer());

    tempGlueMatrix.m_ElementList_Coef.push_back(ElementType(-1));

    tempGlueMatrix.m_IndependentElement = ElementA;

    return tempGlueMatrix;
}


template<typename ElementType>
inline 
GlueMatrixForMultiplication<ElementType> operator*(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkGlueMatrixForMultiplication<ElementType> tempGlueMatrix;

    tempGlueMatrix.m_RowNumber = MatrixB.GetRowNumber();

    tempGlueMatrix.m_ColNumber = MatrixB.GetColNumber();

    tempGlueMatrix.m_SourceMatrixSharedCopyList.resize(1);

    tempGlueMatrix.m_SourceMatrixSharedCopyList[0].SharedCopy(MatrixB);

    tempGlueMatrix.m_Element_Coef = ElementA;

    return tempGlueMatrix;
}


template<typename ElementType>
inline
mdkMatrix<ElementType> operator/(const ElementType& ElementA, const mdkMatrix<ElementType>& MatrixB)
{
    mdkMatrix<ElementType> tempMatrix;

    auto SizeB = MatrixB.GetSize();

    if (SizeB.RowNumber == 0)
    {
        mdkError << "MatrixB is empty @ mdkMatrixOperator: /(ElementA, MatrixB)" << '\n';

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

} // namespace mdk