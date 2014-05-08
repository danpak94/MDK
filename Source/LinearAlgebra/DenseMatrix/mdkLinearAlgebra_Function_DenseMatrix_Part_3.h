#ifndef __mdkLinearAlgebra_Function_DenseMatrix_Part_3_h
#define __mdkLinearAlgebra_Function_DenseMatrix_Part_3_h

#include <string>
#include <limits>


#include <Eigen/Dense>

#include "mdkType.h"

namespace mdk
{

//forward-declare ----------------//
template<typename ElementType>
class DenseMatrix;
//-------------------------------------//

template<typename ElementType>
DenseMatrix<ElementType> ComputeInnerProductListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                               const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeInnerProductOfTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeInnerProductOfTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


// SingleVector is a column vector
// each column of VectorSet is a vector
// L2DistanceList[j] = distance between SingleVector and ColVectorSet(ALL, j)
template<typename ElementType>
DenseMatrix<ElementType> ComputeL2DistanceListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                             const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeL2DistanceBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeL2DistanceBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


template<typename ElementType>
DenseMatrix<ElementType> ComputeSquaredL2DistanceListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                                   const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeSquaredL2DistanceBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeSquaredL2DistanceBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


template<typename ElementType>
DenseMatrix<ElementType> ComputeL1DistanceListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                             const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeL1DistanceBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeL1DistanceBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


// sum((x-mean(x)).*(y-mean(y))/(sum((x-mean(x)).^2)*sum((y-mean(y)).^2))
template<typename ElementType>
DenseMatrix<ElementType> ComputeCorrelationListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                              const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeCorrelationBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeCorrelationBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


// sum(x.*y)/(sum(x.^2)*sum(y.^2))
// UncenteredCorrelation = angle between two vectors
template<typename ElementType>
DenseMatrix<ElementType> ComputeUncenteredCorrelationListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                                        const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeUncenteredCorrelationBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeUncenteredCorrelationBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length);


// sum((x-mean(x)).*(y-mean(y))
template<typename ElementType>
DenseMatrix<ElementType> ComputeUnnormalizedCorrelationListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                                          const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeUnnormalizedCorrelationBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeUnnormalizedCorrelationBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


template<typename ElementType>
DenseMatrix<ElementType> ComputeKLDivergenceListOfSingleVectorFromColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                               const DenseMatrix<ElementType>& ColVectorSet);

// D(B||A) is divergence of A from B, and  B is considered as "ground truth"
template<typename ElementType>
inline
ElementType ComputeKLDivergenceOfVectorAFromVectorB(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeKLDivergenceOfVectorAFromVectorB(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


// Jensen�Shannon divergence is in the range [0, 1]
template<typename ElementType>
DenseMatrix<ElementType> ComputeJSDivergenceListFromSingleVectorToColVectorSet(const DenseMatrix<ElementType>& SingleVector,
                                                                               const DenseMatrix<ElementType>& ColVectorSet);

template<typename ElementType>
inline
ElementType ComputeJSDivergenceBetweenTwoVectors(const DenseMatrix<ElementType>& VectorA, const DenseMatrix<ElementType>& VectorB);

template<typename ElementType>
inline
ElementType ComputeJSDivergenceBetweenTwoVectors(const ElementType* VectorA, const ElementType* VectorB, int_max Length, bool CheckInput = true);


}// namespace mdk


#include "mdkLinearAlgebra_Function_DenseMatrix_Part_3.hpp"

#endif