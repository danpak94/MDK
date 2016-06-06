﻿#pragma once

#include "mdkCoordinateTransform.h"
#include "mdkIntegralImageBasedAverageDenseImageFilter2D.h"
#include "mdkParallelForLoop.h"

namespace mdk
{

template<typename InputPixel_Type, typename OutputPixel_Type = InputPixel_Type, typename Scalar_Type = double>
class DenseImageResampler2D : public Object
{
public:
	typedef InputPixel_Type  InputPixelType;
	typedef OutputPixel_Type OutputPixelType;
	typedef Scalar_Type  ScalarType; // float or double

	typedef Option_Of_Image2DInterpolation<InputPixelType>  ImageInterpolationOptionType;
	typedef MethodEnum_Of_Image2DInterpolation              ImageInterpolationMethodEnum;
	typedef BoundaryOptionEnum_Of_Image2DInterpolation      ImageInterpolationBoundaryOptionEnum;

private:
	//-------------------------- input --------------------------------------------------//
	const DenseImage2D<InputPixelType>* m_InputImage;

	ImageInterpolationOptionType m_ImageInterpolationOption;

	int_max m_MaxThreadCount; // max number of threads

	bool m_Flag_SmoothWhenDownsmapling; // user input
	bool m_Flag_SmoothInputImage;

	const CoordinateTransform<ScalarType>* m_2DPositionTransform_from_OutputImage_to_InputImage;

	//-------------------------- internal -----------------------------------------------//
	DenseImage2D<OutputPixelType> m_SmoothedImage; // Smooth InputImage when down sampling 

    // same point with P_in ~ 3D Positon in InputImage, P_out ~ 3D Position in OutputImage, and P_w ~ 3D World Position
    // orientation matrix O_in ~ Orientation matrix of InputImage, O_out ~ Orientaiton Matrix of OutputImage
	// Orign: In ~ origin of Inputimage, Out ~ origin of OutputImage 
	// P_w = O_in*P_in + In
	// P_w = O_out*P_out + Out
	// P_in = inv(O_in)*D_out*P_out + inv(O_in)*(Out-In)
	// P_out = inv(O_out)*D_in*P_in + inv(O_out)*(In-Out)
	//
	// inv(O_in)*O_out is m_3DPositionTransformFromOuputToInput_Matrix
	// inv(O_in)*(Out-In) is m_3DPositionTransformFromOuputToInput_Offset
	//
	// inv(O_out)*O_in is m_3DPositionTransformFromInputToOutput_Matrix
	// inv(O_out)*(In-Out) is m_3DPositionTransformFromInputToOutput_Offset

	bool m_Flag_Input_Output_SameOrigin;
	bool m_Flag_Input_Output_SameSpacing;
	bool m_Flag_Input_Output_SameOrientation;
	bool m_Flag_Input_Output_SameOrigin_SameOrientation;

	DenseMatrix<double> m_3DPositionTransformFromOuputToInput_Matrix;
	DenseVector<double, 3> m_3DPositionTransformFromOuputToInput_Offset;

	DenseMatrix<double> m_3DPositionTransformFromInputToOutput_Matrix;
	DenseVector<double, 3> m_3DPositionTransformFromInputToOutput_Offset;

	//------------------------- output ----------------------------------------------------//
	DenseImage2D<OutputPixelType> m_OutputImage;

public:
	DenseImageResampler2D();
	~DenseImageResampler2D();
	void Clear();

	void SetInputImage(const DenseImage2D<InputPixelType>* InputImage) { m_InputImage = InputImage; }

	void Set2DPositionTransformFromOutputToInput(const CoordinateTransform<Scalar_Type>* Transform) { m_2DPositionTransform_from_OutputImage_to_InputImage = Transform; }

	void SetOutputImageInfo(const ImageInfo2D& Info);

	void SetOutputImageInfo(const DenseVector<double, 3>& Origin,
							const DenseVector<double, 2>& Spacing,
							const DenseVector<int_max, 2>& Size,
							const DenseMatrix<double>& Orientation);

	// Number of Pixel in x/y/z direction
	// Origin of output image = Origin of input image
	// Spacing of output image may not be equal to Spacing of input image
	void SetOutputImageInfoBySize(const DenseVector<int_max, 2>& Size);
	void SetOutputImageInfoBySize(int_max Lx, int_max Ly);

	// Origin of output image = Origin of input image
	// Size of output image may not be equal to Size of input image
	void SetOutputImageInfoBySpacing(const DenseVector<double, 2>& Spacing);
	void SetOutputImageInfoBySpacing(double Spacing_x, double Spacing_y);

	void SetImageInterpolationOption(const ImageInterpolationOptionType& InputOption) { m_ImageInterpolationOption = InputOption; }
	ImageInterpolationOptionType GetImageInterpolationOption() { return m_ImageInterpolationOption; }

	void SetMaxThreadCount(int_max MaxNumber) { m_MaxThreadCount = MaxNumber; }

	void EnableSmoothingWhenDownsampling(bool On_Off = true);

	void Update();
	DenseImage2D<OutputPixelType>& OutputImage() { return m_OutputImage; }

private:	
	bool CheckInput();
	void SmoothInputImageIfNecessary();
	//---------- Coordinate Transform between Input and Output --------------------------------//	
	void Update3DPositionTransform_Input_Output();	
	DenseVector<ScalarType, 2> Transform2DPositionInInputImageTo2DPositionInOutputImage(const DenseVector<ScalarType, 2>& Position_in);
	DenseVector<ScalarType, 2> Transform2DPositionInOutputImageTo2DPositionInInputImage(const DenseVector<ScalarType, 2>& Position_out);

private:
	void operator=(const DenseImageResampler2D&) = delete;
	DenseImageResampler2D(const DenseImageResampler2D&) = delete;
};


}

#include "mdkDenseImageResampler2D.hpp"

