﻿#pragma once

#include "mdkImageFilter3D.h"
#include "mdkIntegralImageBasedImageAverageFilter3D.h"

namespace mdk
{

template<typename InputPixel_Type, typename OutputPixel_Type = InputPixel_Type, typename Scalar_Type = double>
class DenseImageResampler3D : public ImageFilter3D<DenseImage3D<InputPixel_Type>, DenseImage3D<OutputPixel_Type>, Scalar_Type>
{
public:
	typedef InputPixel_Type  InputPixelType;
	typedef OutputPixel_Type OutputPixelType;
	typedef Scalar_Type      ScalarType;

private:
	bool m_Flag_SmoothWhenDownsmapling; // user input
	bool m_Flag_SmoothInputImage;
	DenseImage3D<OutputPixelType> m_SmoothedImage; // Smooth InputImage when down sampling 

public:
	DenseImageResampler3D();
	~DenseImageResampler3D();

	void Clear();

	void EnableSmoothingWhenDownsampling(bool On_Off = true);

private:
	void ClearSelf();
	bool Preprocess();
	bool Postprocess();

	inline OutputPixelType EvaluateAt3DPositionInInputImage(int_max PointIndex, ScalarType x, ScalarType y, ScalarType z, int_max ThreadIndex);

private:
	void operator=(const DenseImageResampler3D&) = delete;
	DenseImageResampler3D(const DenseImageResampler3D&) = delete;
};


}

#include "mdkDenseImageResampler3D.hpp"

