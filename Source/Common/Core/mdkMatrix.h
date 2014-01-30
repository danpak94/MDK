#ifndef __mdkMatrix_h
#define __mdkMatrix_h

#include <vector>
#include <memory>
#include <initializer_list>
#include <functional>

#include "mdkObject.h"

namespace mdk
{

// 2D Matrix Class Template, each entry is a scalar
// column major
//
// If heavy linear algebra is required, then convert mdkMatrix to Armadillo matrix, do something, and convert back
// Armadillo is a linear algebra library, and it uses column major matrix
//

//forward-declare the template class
template<typename ScalarType>
class mdkMatrix;

struct MatrixSize
{
	uint64 RowNumber;  // RowNumber = the Number of Rows 
	uint64 ColNumber;  // ColNumber = the Number of Columns
};


template<typename ScalarType>
struct mdkMatrixSVDResult
{
// Matrix = U*S*V;
	mdkMatrix<ScalarType> U;  // matrix
	mdkMatrix<ScalarType> S;  // matrix  : change to vector?
	mdkMatrix<ScalarType> V;  // matrix
};


template<typename ScalarType>
struct mdkMatrixPCAResult
{
// Matrix = M+U*S*U';

	mdkMatrix<ScalarType> M; // mean
	mdkMatrix<ScalarType> U; // eigenvector
	mdkMatrix<ScalarType> S;  // matrix  : change to vector?
};

// ----------------------- Matrix {+ - * /}  Matrix ------------------------------------------------//

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator+(mdkMatrix<ScalarType>& MatrixA, mdkMatrix<ScalarType>& MatrixB);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator-(mdkMatrix<ScalarType>& MatrixA, mdkMatrix<ScalarType>& MatrixB);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator*(mdkMatrix<ScalarType>& MatrixA, mdkMatrix<ScalarType>& MatrixB);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator/(mdkMatrix<ScalarType>& MatrixA, mdkMatrix<ScalarType>& MatrixB);

// ----------------------- Scalar {+ - * /} Matrix ------------------------------------------------//

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator+(ScalarType Scalar, mdkMatrix<ScalarType>& Matrix);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator-(ScalarType Scalar, mdkMatrix<ScalarType>& Matrix);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator*(ScalarType Scalar, mdkMatrix<ScalarType>& Matrix);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator/(ScalarType Scalar, mdkMatrix<ScalarType>& Matrix);

// ----------------------- Matrix {+ - * /}  Scalar ------------------------------------------------//

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator+(mdkMatrix<ScalarType>& Matrix, ScalarType Scalar);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator-(mdkMatrix<ScalarType>& Matrix, ScalarType Scalar);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator*(mdkMatrix<ScalarType>& Matrix, ScalarType Scalar);

template<typename ScalarType>
inline mdkMatrix<ScalarType> operator/(mdkMatrix<ScalarType>& Matrix, ScalarType Scalar);
//--------------------------------------------------------------------------------------------------//


template<typename ScalarType>
class mdkMatrix : public mdkObject
{

private:

	uint64 m_RowNumber;
	
	uint64 m_ColNumber;

	std::shared_ptr<std::vector<ScalarType>> m_ScalarData;

	bool m_IsSizeFixed;

	mdkScalarTypeEnum m_ScalarType;

	uint64 m_ColExpansionStep;

	uint64 m_RowExpansionStep;

	ScalarType  m_EmptyScalar;

public:		
	
	inline mdkMatrix();

	inline mdkMatrix(mdkMatrix<ScalarType>& sourceMatrix);

	inline mdkMatrix(uint64 RowNumber, uint64 ColNumber, bool IsSizeFixed = false);

	inline ~mdkMatrix();

	inline void Clear();

	inline std::shared_ptr<std::vector<ScalarType>> GetScalarDataSharedPointer();

	inline ScalarType* GetScalarDataRawPointer();

	inline mdkScalarTypeEnum GetScalarType();

	//---------------------- Matrix Size ----------------------------------------//

	inline bool SetSize(uint64 RowNumber, uint64 ColNumber, bool IsSizeFixed = false);

	inline void FixSize(bool Fix);

	inline bool ReSize(uint64 RowNumber, uint64 ColNumber);

	inline void GetSize(uint64* RowNumber, uint64* ColNumber);

	inline MatrixSize GetSize();

	inline bool IsSizeFixed();

	inline bool IsEmpty();

	//---------------------- Initilize Matrix ----------------------------------------//

	template<typename ScalarType_target>
	inline void operator=(mdkMatrix<ScalarType_target>& targetMatrix);

	inline void operator=(ScalarType Scalar);

	inline void operator=(const std::initializer_list<ScalarType>& list);

	template<typename ScalarType_target>
	inline bool Copy(mdkMatrix<ScalarType_target>& targetMatrix);

	template<typename ScalarType_target>
	inline bool Copy(ScalarType_target* ScalarPointer, uint64 RowNumber, uint64 ColNumber);

	inline bool Fill(ScalarType Scalar);

	//---------------------- Get/Set Matrix(LinearIndex) ----------------------------------------//

	inline ScalarType& operator()(uint64 LinearIndex);

	inline const ScalarType& operator()(uint64 LinearIndex) const;

	inline ScalarType& Element(uint64 LinearIndex);

	inline const ScalarType& Element(uint64 LinearIndex) const;

	//---------------------- Get/Set Matrix(i,j) ----------------------------------------//

	inline ScalarType& operator()(uint64 RowIndex, uint64 ColIndex);

	inline const ScalarType& operator()(uint64 RowIndex, uint64 ColIndex) const;

	inline ScalarType& Element(uint64 RowIndex, uint64 ColIndex);

	inline const ScalarType& Element(uint64 RowIndex, uint64 ColIndex) const;

	//---------------------- Get Matrix(i_s to i_e, j_s to j_e) ----------------------------------------//

	inline mdkMatrix operator()(uint64 RowIndex_s, uint64 RowIndex_e, uint64 ColIndex_s, uint64 ColIndex_e);

	inline mdkMatrix SubMatrix(uint64 RowIndex_s, uint64 RowIndex_e, uint64 ColIndex_s, uint64 ColIndex_e);

	//---------------------- Get/Set a column ----------------------------------------//
	
	inline mdkMatrix GetCol(uint64 ColIndex);

	inline bool GetCol(uint64 ColIndex, ScalarType* ColData);

	inline bool GetCol(uint64 ColIndex, std::vector<ScalarType>& ColData);

	template<typename ScalarType_input>
	inline bool SetCol(uint64 ColIndex, mdkMatrix<ScalarType_input>& ColData);

	inline bool SetCol(uint64 ColIndex, const ScalarType* ColData);

	inline bool SetCol(uint64 ColIndex, const std::vector<ScalarType>& ColData);

	template<typename ScalarType_input>
	inline bool AppendCol(const mdkMatrix<ScalarType_input>& ColData);

	inline bool AppendCol(const ScalarType* ColData);

	inline bool AppendCol(const std::vector<ScalarType>& ColData);

	//---------------------- Get/Set a row or column ----------------------------------------//
	
	inline mdkMatrix GetRow(uint64 RowIndex);

	inline bool GetRow(uint64 ColIndex, ScalarType* RowData);

	inline bool GetRow(uint64 RowIndex, std::vector<ScalarType>& RowData);

	template<typename ScalarType_input>
	inline bool SetRow(uint64 ColIndex, mdkMatrix<ScalarType_input>& RowData);

	inline bool SetRow(uint64 RowIndex, const ScalarType* RowData);

	inline bool SetRow(uint64 RowIndex, const std::vector<ScalarType>& RowData);

	template<typename ScalarType_input>
	inline bool AppendRow(mdkMatrix<ScalarType_input>& RowData);

	inline bool AppendRow(const ScalarType* RowData);

	inline bool AppendRow(const std::vector<ScalarType>& RowData);

	//---------------------- Get/Set the diagonal ----------------------------------------//

	inline mdkMatrix GetDiangonal();

	inline bool GetDiangonal(ScalarType* DiangonalData);

	inline bool GetDiangonal(std::vector<ScalarType>& DiangonalData);

	inline bool SetDiangonal(ScalarType Scalar);

	template<typename ScalarType_input>
	inline bool SetDiangonal(mdkMatrix<ScalarType_input>& DiangonalData);

	inline bool SetDiangonal(const ScalarType* DiangonalData);

	inline bool SetDiangonal(const std::vector<ScalarType>& DiangonalData);

	//---------------------- Matrix {+= -= *= /=} Matrix ----------------------------------------//

	inline void operator+=(mdkMatrix<ScalarType>& targetMatrix);

	inline void operator-=(mdkMatrix<ScalarType>& targetMatrix);

	inline void operator*=(mdkMatrix<ScalarType>& targetMatrix);

	inline void operator/=(mdkMatrix<ScalarType>& targetMatrix);

    //---------------------- Matrix {+= -= *= /=} Scalar ----------------------------------------//

	// error:
	//template<typename ScalarType_target>
	//inline void operator+(ScalarType_target Scalar);

	inline void operator+=(ScalarType Scalar);

	inline void operator-=(ScalarType Scalar);

	inline void operator*=(ScalarType Scalar);

	inline void operator/=(ScalarType Scalar);

	//-------------------- element operation  ------------------------------------------//

	inline mdkMatrix ElementOperation(std::string FunctorName);

	inline mdkMatrix ElementOperation(std::function<ScalarType(ScalarType)> Functor);

	template<typename ScalarType_target>
	inline mdkMatrix ElementOperation(std::string FunctorName, mdkMatrix<ScalarType_target>& targetMatrix);

	template<typename ScalarType_target>
	inline mdkMatrix ElementOperation(std::function<ScalarType(ScalarType, ScalarType)> Functor, mdkMatrix<ScalarType_target>& targetMatrix);

	inline mdkMatrix ElementOperation(std::string FunctorName, ScalarType Scalar);

	inline mdkMatrix ElementOperation(std::function<ScalarType(ScalarType, ScalarType)> Functor, ScalarType Scalar);

	//-------------------- calculate sum mean min max ------------------------------------------//

	inline ScalarType Mean();

	inline mdkMatrix MeanAlongRow();

	inline mdkMatrix MeanAlongCol();

	inline ScalarType Sum();

	inline mdkMatrix SumAlongRow();

	inline mdkMatrix SumAlongCol();

	inline ScalarType Max();

	inline mdkMatrix MaxAlongRow();

	inline mdkMatrix MaxAlongCol();

	inline ScalarType Min();

	inline mdkMatrix MinAlongRow();

	inline mdkMatrix MinAlongCol();

	//----------------------------------- transpose -----------------------------------------//

	inline mdkMatrix GetTranspose();

	//----------------------------------- Rank -----------------------------------------//

	inline uint64 Rank();

	//----------------------------------- inverse -----------------------------------------//

	inline mdkMatrix Inv();

	inline mdkMatrix PseudoInv();

	//----------------------------------- SVD -----------------------------------------//

	inline mdkMatrixSVDResult<ScalarType> SVD();

	//----------------------------------- PCA -----------------------------------------//

	inline mdkMatrixPCAResult<ScalarType> PCA();

	//----------------------------------- Convolution -----------------------------------------//

	template<typename ScalarType_target>
	inline mdkMatrix GetConvolution(mdkMatrix<ScalarType_target> MaskMatrix);

	//----------------------------------- LinearCombine -----------------------------------------//

	inline static mdkMatrix LinearCombine(std::vector<double> AlphaList, std::vector<mdkMatrix<ScalarType>*> MatrixList);

	//---------------------------- private functions ---------------------------------------//

private:
	template<typename ScalarType_target>
	inline mdkScalarTypeEnum FindScalarType(ScalarType_target Scalar);

	inline bool CopyOnWrite();

	template<typename ScalarType_target>
	inline uint64 ByteNumberOfScalar(ScalarType_target Scalar);
};

}//end namespace mdk

#include "mdkMatrix.hpp"

#endif