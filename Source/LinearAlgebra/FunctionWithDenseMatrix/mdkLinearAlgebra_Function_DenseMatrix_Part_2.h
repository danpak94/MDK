﻿#pragma once

#include <algorithm>
#include <complex>

#include <Eigen/Dense>

#include "mdkConstant.h"
#include "mdkMatrix_Common.h"

namespace mdk
{

//forward-declare ----------------//
template<typename ElementType>
class DenseMatrix;
//-------------------------------------//

//-----------------------------------------------------------------------------------------------//

template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, MatchFunctionType MatchFunction);

template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputNumber, MatchFunctionType MatchFunction);

template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputNumber,
                                         int_max LinearIndex_start, int_max LinearIndex_end, MatchFunctionType MatchFunction);

template<typename ElementType, typename MatchFunctionType>
int_max FindElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, MatchFunctionType MatchFunction);

//------------------------------------------------
template<typename ElementType>
DenseMatrix<int_max> ExactMatchElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, const ElementType& InputElement);

template<typename ElementType>
int_max ExactMatchElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, const ElementType& InputElement);
//---------------------------------------------------
template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, MatchFunctionType MatchFunction);

template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputColNumber, MatchFunctionType MatchFunction);

template<typename ElementType, typename MatchFunctionType>
DenseMatrix<int_max> FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, int_max MaxOutputColNumber,
                                     int_max ColIndex_start, int_max ColIndex_end, MatchFunctionType MatchFunction);

template<typename ElementType, typename MatchFunctionType>
int_max FindColInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, MatchFunctionType MatchFunction);
//---------------------------------------------------
template<typename ElementType>
DenseMatrix<int_max> ExactMatchColInMatrix(const DenseMatrix<ElementType>& InputMatrix, const DenseMatrix<ElementType>& InputCol);

template<typename ElementType>
int_max ExactMatchColInMatrix(const DenseMatrix<ElementType>& InputMatrix, const std::string& first_or_last, const DenseMatrix<ElementType>& InputCol);
//---------------------------------------------------
template<typename ElementType, typename CompareFunctionType>
DenseMatrix<int_max> SortColInMatrix(const DenseMatrix<ElementType>& InputMatrix, CompareFunctionType CompareFunction);

template<typename ElementType, typename CompareFunctionType>
DenseMatrix<int_max> SortColInMatrix(const DenseMatrix<ElementType>& InputMatrix,
                                     int_max ColIndex_start, int_max ColIndex_end, CompareFunctionType CompareFunction);
//---------------------------------------------------
template<typename ElementType>
DenseMatrix<int_max> FindUniqueElementInMatrix(const DenseMatrix<ElementType>& InputMatrix);

//SpecialCompareFunction(a, b) return {-1, 0, 1} as {a < b, a = b, a > b}
template<typename ElementType, typename SpecialCompareFunctionType>
DenseMatrix<int_max> FindUniqueElementInMatrix(const DenseMatrix<ElementType>& InputMatrix, SpecialCompareFunctionType SpecialCompareFunction);

template<typename ElementType>
DenseMatrix<int_max> FindUniqueColInMatrix(const DenseMatrix<ElementType>& InputMatrix);

//SpecialCompareFunction(VectorA, VectorB) return {-1, 0, 1} as {VectorA < VectorB, VectorA = VectorB, VectorA > VectorB}
template<typename ElementType, typename SpecialCompareFunctionType>
DenseMatrix<int_max> FindUniqueColInMatrix(const DenseMatrix<ElementType>& InputMatrix, SpecialCompareFunctionType SpecialCompareFunction);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
ElementType MatrixMean(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixMeanOfEachCol(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixMeanOfEachRow(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
int_max FindLinearIndexOfMaxInMatrix(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
ElementType MatrixMax(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixMaxOfEachCol(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixMaxOfEachRow(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
int_max FindLinearIndexOfMinInMatrix(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
ElementType MatrixMin(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixMinOfEachCol(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixMinOfEachRow(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
ElementType MatrixSum(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixSumOfEachCol(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixSumOfEachRow(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
ElementType MatrixNorm_L1(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
ElementType MatrixNorm_L2(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
DenseMatrix<ElementType> MatrixTranspose(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
void MatrixTransposeInPlace(DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
int_max MatrixRank(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
ElementType MatrixDeterminant(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//
template<typename ElementType>
DenseMatrix<ElementType> MatrixInverse(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrix<ElementType> MatrixPseudoInverse(const DenseMatrix<ElementType>& InputMatrix);
//-----------------------------------------------------------------------------------------------//
// solve A*X=B
template<typename ElementType>
DenseMatrix<ElementType> SolveMatrixLinearEquation(const DenseMatrix<ElementType>& MatrixA, const DenseMatrix<ElementType>& MatrixB);
//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
struct DenseMatrixEigenResult
{
    DenseMatrix<ElementType> EigenVector;    // eigenvector : full matrix
    DenseMatrix<ElementType> EigenValue;     // eigenvalue  : col vector

    DenseMatrixEigenResult(){};

	DenseMatrixEigenResult(const DenseMatrixEigenResult& Result)
	{
		*this = Result;
	}

    DenseMatrixEigenResult(DenseMatrixEigenResult&& Result)
    {
		*this = std::move(Result);
    }

    ~DenseMatrixEigenResult(){};

	void operator=(const DenseMatrixEigenResult& Result)
	{
		EigenVector = Result.EigenVector;
		EigenValue = Result.EigenValue;
	}

	void operator=(DenseMatrixEigenResult&& Result)
	{
		EigenVector = std::move(Result.EigenVector);
		EigenValue = std::move(Result.EigenValue);
	}
};

template<typename ElementType>
DenseMatrixEigenResult<std::complex<ElementType>> NonSymmetricRealMatrixEigen(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
DenseMatrixEigenResult<ElementType> SymmetricRealMatrixEigen(const DenseMatrix<ElementType>& InputMatrix, bool CheckIfSymmetric = false);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
struct DenseMatrixPCAResult
{
    DenseMatrix<ElementType> Mean;           // mean
    DenseMatrix<ElementType> EigenVector;    // eigenvector
    DenseMatrix<ElementType> EigenValue;     // col vector

    DenseMatrixPCAResult(){};

	DenseMatrixPCAResult(const DenseMatrixPCAResult& Result)
	{
		*this = Result;
	}

    DenseMatrixPCAResult(DenseMatrixPCAResult&& Result)
    {
		*this = std::move(Result);
    }

    ~DenseMatrixPCAResult(){};

	void operator=(const DenseMatrixPCAResult& Result)
	{
		Mean = Result.Mean;
		EigenVector = Result.EigenVector;
		EigenValue = Result.EigenValue;
	}

    void operator=(DenseMatrixPCAResult&& Result)
    {
        Mean = std::move(Result.Mean);
        EigenVector = std::move(Result.EigenVector);
        EigenValue = std::move(Result.EigenValue);
    }
};

template<typename ElementType>
DenseMatrixPCAResult<ElementType> MatrixPCA(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
struct DenseMatrixSVDResult
{
    // Matrix = U*[S]*V';
    DenseMatrix<ElementType> U;  // matrix
    DenseMatrix<ElementType> S;  // Vector, S(0) >= S(1) >= S(2) >= ...
    DenseMatrix<ElementType> V;  // matrix

    DenseMatrixSVDResult(){};

	DenseMatrixSVDResult(const DenseMatrixSVDResult& Result)
	{
		*this = Result;
	}

    DenseMatrixSVDResult(DenseMatrixSVDResult&& Result)
    {        
		*this = std::move(Result);
    }

    ~DenseMatrixSVDResult(){};

    void operator=(const DenseMatrixSVDResult& Result)
    {
        U = Result.U;        
        S = Result.S;
		V = Result.V;
    }

	void operator=(DenseMatrixSVDResult&& Result)
	{
		U = std::move(Result.U);
		S = std::move(Result.S);
		V = std::move(Result.V);
	}
};


template<typename ElementType>
DenseMatrixSVDResult<ElementType> MatrixSVD(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
DenseMatrix<ElementType> MatrixConvolution(const DenseMatrix<ElementType>& InputMatrix, const DenseMatrix<ElementType>& Mask, const char* Option);

}//end namespace mdk

#include "mdkLinearAlgebra_Function_DenseMatrix_Part_2.hpp"

