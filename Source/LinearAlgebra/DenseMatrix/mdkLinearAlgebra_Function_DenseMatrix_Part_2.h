#ifndef __mdkLinearAlgebra_Function_DenseMatrix_Part_2_h
#define __mdkLinearAlgebra_Function_DenseMatrix_Part_2_h

#include <vector>
#include <array>
#include <memory>
#include <string>
#include <initializer_list>
#include <complex>


#include "mdkType.h"
//#include "mdkDenseMatrix.h"
#include "mdkMatrix_Common.h"

namespace mdk
{

//forward-declare ----------------//
template<typename ElementType>
class DenseMatrix;
//-------------------------------------//

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
inline
ElementType MatrixNorm_L1(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
inline
ElementType MatrixNorm_L2(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//


template<typename ElementType>
inline DenseMatrix<ElementType> MatrixTranspose(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
inline int64 MatrixRank(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
inline DenseMatrix<ElementType> MatrixInv(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
struct DenseMatrixEigenResult
{
    DenseMatrix<ElementType> EigenVector;    // eigenvector : full matrix
    DenseMatrix<ElementType> EigenValue;     // eigenvalue  : col vector

    DenseMatrixEigenResult(){};

    // move constructor
    DenseMatrixEigenResult(DenseMatrixEigenResult&& Result)
    {
        EigenVector = std::move(Result.EigenVector);
        EigenValue  = std::move(Result.EigenValue);
    }

    ~DenseMatrixEigenResult(){};

    void operator=(const DenseMatrixEigenResult&& Result)
    {
        EigenVector = std::move(Result.EigenVector);
        EigenValue  = std::move(Result.EigenValue);
    }

    //------------------------------------------------------
    void operator=(const DenseMatrixEigenResult&) = delete;
};

template<typename ElementType>
inline DenseMatrixEigenResult<std::complex<ElementType>> NonSymmetricRealMatrixEigen(const DenseMatrix<ElementType>& InputMatrix);

template<typename ElementType>
inline DenseMatrixEigenResult<ElementType> SymmetricRealMatrixEigen(const DenseMatrix<ElementType>& InputMatrix, bool CheckIfSymmetric = false);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
struct DenseMatrixPCAResult
{
    DenseMatrix<ElementType> Mean;           // mean
    DenseMatrix<ElementType> EigenVector;    // eigenvector
    DenseMatrix<ElementType> EigenValue;     // col vector

    DenseMatrixPCAResult(){};

    // move constructor
    DenseMatrixPCAResult(DenseMatrixPCAResult&& Result)
    {
        Mean = std::move(Result.Mean);
        EigenVector = std::move(Result.EigenVector);
        EigenValue = std::move(Result.EigenValue);
    }

    ~DenseMatrixPCAResult(){};

    void operator=(const DenseMatrixPCAResult&& Result)
    {
        Mean = std::move(Result.Mean);
        EigenVector = std::move(Result.EigenVector);
        EigenValue = std::move(Result.EigenValue);
    }

    //------------------------------------------------------
    void operator=(const DenseMatrixPCAResult&) = delete;
};

template<typename ElementType>
inline DenseMatrixPCAResult<ElementType> MatrixPCA(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
struct DenseMatrixSVDResult
{
    // Matrix = U*S*V;
    DenseMatrix<ElementType> U;  // matrix
    DenseMatrix<ElementType> S;  // matrix  : change to vector?
    DenseMatrix<ElementType> V;  // matrix

    DenseMatrixSVDResult(){};

    // move constructor
    DenseMatrixSVDResult(DenseMatrixSVDResult&& Result)
    {
        // this will call copy "=" not move "="
        //U = Result.U;

        U = std::move(Result.U);
        V = std::move(Result.V);
        S = std::move(Result.S);
    }

    ~DenseMatrixSVDResult(){};

    void operator=(const DenseMatrixSVDResult&& Result)
    {
        U = std::move(Result.U);
        V = std::move(Result.V);
        S = std::move(Result.S);
    }

    //------------------------------------------------------
    void operator=(const DenseMatrixSVDResult&) = delete;

};


template<typename ElementType>
inline DenseMatrixSVDResult<ElementType> MatrixSVD(const DenseMatrix<ElementType>& InputMatrix);

//-----------------------------------------------------------------------------------------------//

template<typename ElementType>
inline DenseMatrix<ElementType> MatrixConvolution(const DenseMatrix<ElementType>& InputMatrix, const DenseMatrix<ElementType>& Mask, const char* Option);

}//end namespace mdk

#include "mdkLinearAlgebra_Function_DenseMatrix_Part_2.hpp"

#endif
