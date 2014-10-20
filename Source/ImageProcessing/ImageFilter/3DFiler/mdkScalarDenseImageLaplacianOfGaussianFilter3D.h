﻿#ifndef __mdkScalarDenseImageLaplacianOfGaussianFilter3D_h
#define __mdkScalarDenseImageLaplacianOfGaussianFilter3D_h

#include <algorithm>
#include <cmath>

#include "mdkDebugConfig.h"
#include "mdkDenseMatrix.h"
#include "mdkScalarDenseImageConvolutionFilter3D.h"

namespace mdk
{

template<typename InputPixel_Type, typename OutputPixel_Type = InputPixel_Type, typename Scalar_Type = OutputPixel_Type>
class ScalarDenseImageLaplacianOfGaussianFilter3D : public ScalarDenseImageConvolutionFilter3D<InputPixel_Type, OutputPixel_Type, Scalar_Type>
{
public:
	typedef InputPixel_Type  InputPixelType;
	typedef OutputPixel_Type OutputPixelType;
	typedef Scalar_Type      ScalarType;

private:
	DenseVector<ScalarType, 3> m_SigmaList; // [sigma_x, sigma_y, sigma_z]
	DenseMatrix<ScalarType> m_RotationMatrix; // 3 x 3
	ScalarType m_CutOffRatio; // 2 ~ 4

public:		
    ScalarDenseImageLaplacianOfGaussianFilter3D();
    ~ScalarDenseImageLaplacianOfGaussianFilter3D();
  
	// Sigma in Physical size (mm)
	void SetGaussianParameter(const DenseVector<ScalarType, 3>& SigmaList, const DenseMatrix<ScalarType>& RotationMatrix, ScalarType CutOffRatio);

    virtual void Clear();

private:
	bool CheckInput();
	bool Preprocess();
    void BuildMask_3DIndex();
	void BuildMask_3DPhysicalPosition();

private:
    ScalarDenseImageLaplacianOfGaussianFilter3D(const ScalarDenseImageLaplacianOfGaussianFilter3D&) = delete;
    void operator=(const ScalarDenseImageLaplacianOfGaussianFilter3D&) = delete;
};

}//end namespace mdk

#include "mdkScalarDenseImageLaplacianOfGaussianFilter3D.hpp"

#endif