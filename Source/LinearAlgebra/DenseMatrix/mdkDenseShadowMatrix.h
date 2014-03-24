#ifndef __mdkDenseShadowMatrix_h
#define __mdkDenseShadowMatrix_h

#include <vector>
#include <memory>
#include <initializer_list>
#include <string>
#include <cmath>
#include <algorithm>

#include "mdkDebugConfig.h"

#include "mdkDenseMatrix.h"

namespace mdk
{

//------------------------------ forward-declare -----------//

template<typename ElementType>
class DenseMatrix;

template<typename ElementType>
class DenseGlueMatrixForLinearCombination;

template<typename ElementType>
class DenseGlueMatrixForMultiplication;

struct ALL_Symbol_For_Matrix_Operator;

struct MatrixSize;
//--------------------------- end of forward-declare -------//



//--------------------------------------------------------------------------------------------------------------------------------------------------//

template<typename ElementType>
class DenseShadowMatrix : public Object
{
private:

	int64 m_RowNumber;

	int64 m_ColNumber;

	int64 m_ElementNumber;

    bool m_Flag_OutputVector;

    bool m_Flag_All_Row;

    bool m_Flag_All_Col;

    ElementType m_NaNElement;

	// info of the source Matrix -----------------------------------------------//

	std::vector<int64> m_RowIndexList_source;

	std::vector<int64> m_ColIndexList_source;

	std::vector<int64> m_LinearIndexList_source;

    DenseMatrix<ElementType> m_SourceMatrixSharedCopy;

	//------------------- constructor and destructor ------------------------------------//
private:
    inline DenseShadowMatrix(); // empty ShadowMatrix

    // do not need non const versions of the following constructors
    // adding const to the member functions of mdkDenseShadowMatrix is enough

    inline DenseShadowMatrix(const DenseMatrix<ElementType>& sourceMatrix, const std::vector<int64>& LinearIndexList);

    inline DenseShadowMatrix(const DenseMatrix<ElementType>& sourceMatrix, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol);

    inline DenseShadowMatrix(const DenseMatrix<ElementType>& sourceMatrix, const std::vector<int64>& RowIndexList, const std::vector<int64>& ColIndexList);

    inline DenseShadowMatrix(const DenseMatrix<ElementType>& sourceMatrix, const std::vector<int64>& RowIndexList, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol);

    inline DenseShadowMatrix(const DenseMatrix<ElementType>& sourceMatrix, const ALL_Symbol_For_Matrix_Operator& ALL_Symbol, const std::vector<int64>& ColIndexList);

    inline DenseShadowMatrix(const DenseShadowMatrix<ElementType>& ShadowMatrix) = delete;

    inline DenseShadowMatrix(DenseShadowMatrix<ElementType>&& ShadowMatrix);

public:
	inline ~DenseShadowMatrix();

	//---------------------- mdkDenseShadowMatrix = mdkDenseShadowMatrix or Matrix or Element or GlueMatrix ----------------------------------------//
private:
    inline void operator=(DenseShadowMatrix<ElementType>&&) = delete;

public:
    inline void operator=(const DenseMatrix<ElementType>& InputMatrix);

    inline void operator=(const ElementType& Element);

    inline void operator=(const DenseShadowMatrix<ElementType>& ShadowMatrix);

    inline void operator=(const DenseGlueMatrixForLinearCombination<ElementType>& GlueMatrix);

    inline void operator=(const DenseGlueMatrixForMultiplication<ElementType>& GlueMatrix);

private:
	//---------------------- Get/Set mdkDenseShadowMatrix(LinearIndex) ----------------------------------------//

    // operator[]: no bound check

    inline ElementType& operator[](int64 LinearIndex);

    inline const ElementType& operator[](int64 LinearIndex) const;

	inline ElementType& operator()(int64 LinearIndex);

	inline const ElementType& operator()(int64 LinearIndex) const;

	//---------------------- Get/Set mdkDenseShadowMatrix(i,j) ----------------------------------------//

	inline ElementType& operator()(int64 RowIndex, int64 ColIndex);

	inline const ElementType& operator()(int64 RowIndex, int64 ColIndex) const;

	//---------------------- Other ----------------------------------------//

    inline bool IsLinearIndexListOnly() const;

    inline int64 GetRowNumber() const;

    inline int64 GetColNumber() const;

    inline int64 GetElementNumber() const;

    inline bool IsEmpty() const;

    inline MatrixSize GetSize() const;

    inline const std::vector<int64>& GetRowIndexListOfSource() const;

    inline const std::vector<int64>& GetColIndexListOfSource() const;

    inline const std::vector<int64>& GetLinearIndexListOfSource() const;

    inline const DenseMatrix<ElementType>& GetSourceMatrixSharedCopy() const;

    inline DenseMatrix<ElementType> CreateDenseMatrix() const;

    inline bool CreateDenseMatrix(DenseMatrix<ElementType>& OutputMatrix) const;

public:
    //--------------------------------------------------- ShadowMatrix {+= -= *= /=} Matrix ------------------------------------------------//

    inline void operator+=(const DenseMatrix<ElementType>& Matrix);

    inline void operator-=(const DenseMatrix<ElementType>& Matrix);

    inline void operator*=(const DenseMatrix<ElementType>& Matrix);

    inline void operator/=(const DenseMatrix<ElementType>& Matrix);

    //--------------------------------------------------ShadowMatrix {+= -= *= /=} Element ------------------------------------------------------------//

    inline void operator+=(const ElementType& Element);

    inline void operator-=(const ElementType& Element);

    inline void operator*=(const ElementType& Element);

    inline void operator/=(const ElementType& Element);

    //----------------------------------------------- ShadowMatrix {+= -= *= /=} ShadowMatrix ---------------------------------------------//

    inline void operator+=(const DenseShadowMatrix<ElementType>& ShadowMatrix);

    inline void operator-=(const DenseShadowMatrix<ElementType>& ShadowMatrix);

    inline void operator*=(const DenseShadowMatrix<ElementType>& ShadowMatrix);

    inline void operator/=(const DenseShadowMatrix<ElementType>& ShadowMatrix);

    //----------------------------------------------- ShadowMatrix {+= -= *= /=} GlueMatrixForLinearCombination ----------------------------------------//

    inline void operator+=(const DenseGlueMatrixForLinearCombination<ElementType>& GlueMatrix);

    inline void operator-=(const DenseGlueMatrixForLinearCombination<ElementType>& GlueMatrix);

    inline void operator*=(const DenseGlueMatrixForLinearCombination<ElementType>& GlueMatrix);

    inline void operator/=(const DenseGlueMatrixForLinearCombination<ElementType>& GlueMatrix);

    //------------------------------------------------ ShadowMatrix {+= -= *= /=} GlueMatrixForMultiplication --------------------------------------//

    inline void operator+=(const DenseGlueMatrixForMultiplication<ElementType>& GlueMatrix);

    inline void operator-=(const DenseGlueMatrixForMultiplication<ElementType>& GlueMatrix);

    inline void operator*=(const DenseGlueMatrixForMultiplication<ElementType>& GlueMatrix);

    inline void operator/=(const DenseGlueMatrixForMultiplication<ElementType>& GlueMatrix);

    //---------------------- ElementMultiply is .* in matlab -----------------------------------------------------------------------//

    inline DenseMatrix<ElementType> ElementMultiply(const DenseMatrix<ElementType>& Matrix) const;

    inline DenseMatrix<ElementType> ElementMultiply(const ElementType& Element) const;

    inline DenseMatrix<ElementType> ElementMultiply(const DenseShadowMatrix<ElementType>& ShadowMatrix) const;

    inline DenseMatrix<ElementType> ElementMultiply(const DenseGlueMatrixForLinearCombination<ElementType>& GlueMatrix) const;

    inline DenseMatrix<ElementType> ElementMultiply(const DenseGlueMatrixForMultiplication<ElementType>& GlueMatrix) const;

private:
//------------------------------- friend class ----------------------------------------------------------------------------//
    
    template<typename E_TYPE>
    friend class DenseMatrix;
    
// -------------------------------- friend  function ----------------------------------------------------------------------------//
    
    // ------------------------------------------ ShadowMatrix {+ - * /}  Matrix ----------------------------------------------------------//

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator+(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseMatrix<E_TYPE>& MatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator-(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseMatrix<E_TYPE>& MatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseMatrix<E_TYPE>& MatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseMatrix<E_TYPE>& MatrixB);

    // ---------------------------------------------- Matrix {+ - * /}  ShadowMatrix -----------------------------------------------------//

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator+(const DenseMatrix<E_TYPE>& MatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator-(const DenseMatrix<E_TYPE>& MatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(const DenseMatrix<E_TYPE>& MatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseMatrix<E_TYPE>& MatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    // ------------------------------------------ ShadowMatrix {+ - * /}  Element ------------------------------------------------------------//

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator+(const DenseShadowMatrix<E_TYPE>& ShadowMatrix, const E_TYPE& Element);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator-(const DenseShadowMatrix<E_TYPE>& ShadowMatrix, const E_TYPE& Element);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator*(const DenseShadowMatrix<E_TYPE>& ShadowMatrix, const E_TYPE& Element);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseShadowMatrix<E_TYPE>& ShadowMatrix, const E_TYPE& Element);

    // ----------------------------------------- Element {+ - * /} ShadowMatrix --------------------------------------------------------------//

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator+(const E_TYPE& Element, const DenseShadowMatrix<E_TYPE>& ShadowMatrix);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator-(const E_TYPE& Element, const DenseShadowMatrix<E_TYPE>& ShadowMatrix);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator*(const E_TYPE& Element, const DenseShadowMatrix<E_TYPE>& ShadowMatrix);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const E_TYPE& Element, const DenseShadowMatrix<E_TYPE>& ShadowMatrix);

    // ---------------------------------------- ShadowMatrix {+ - * /}  ShadowMatrix -------------------------------------------------------//

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator+(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator-(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    // ------------------------------------------ ShadowMatrix {+ - * /} GlueMatrixForLinearCombination -------------------------------------------------------//

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator+(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, DenseGlueMatrixForLinearCombination<E_TYPE> GlueMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator-(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, DenseGlueMatrixForLinearCombination<E_TYPE> GlueMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseGlueMatrixForLinearCombination<E_TYPE>& GlueMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseGlueMatrixForLinearCombination<E_TYPE>& GlueMatrixB);

    // -------------------------------------------- GlueMatrixForLinearCombination {+ - * /}  ShadowMatrix -----------------------------------------------------//

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator+(DenseGlueMatrixForLinearCombination<E_TYPE> GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator-(DenseGlueMatrixForLinearCombination<E_TYPE> GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(const DenseGlueMatrixForLinearCombination<E_TYPE>& GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseGlueMatrixForLinearCombination<E_TYPE>& GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    // ------------------------------------------ ShadowMatrix {+ - * /} GlueMatrixForMultiplication -------------------------------------------------------//

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator+(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseGlueMatrixForMultiplication<E_TYPE>& GlueMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator-(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseGlueMatrixForMultiplication<E_TYPE>& GlueMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, DenseGlueMatrixForMultiplication<E_TYPE> GlueMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseShadowMatrix<E_TYPE>& ShadowMatrixA, const DenseGlueMatrixForMultiplication<E_TYPE>& GlueMatrixB);

    // -------------------------------------------- GlueMatrixForMultiplication {+ - * /}  ShadowMatrix -----------------------------------------------------//

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator+(const DenseGlueMatrixForMultiplication<E_TYPE>& GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForLinearCombination<E_TYPE> operator-(const DenseGlueMatrixForMultiplication<E_TYPE>& GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseGlueMatrixForMultiplication<E_TYPE> operator*(DenseGlueMatrixForMultiplication<E_TYPE> GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

    template<typename E_TYPE>
    friend DenseMatrix<E_TYPE> operator/(const DenseGlueMatrixForMultiplication<E_TYPE>& GlueMatrixA, const DenseShadowMatrix<E_TYPE>& ShadowMatrixB);

};

}//end namespace mdk

#include "mdkDenseShadowMatrix.hpp"

#endif
