﻿#ifndef mdk_ImageFilter3D_h
#define mdk_ImageFilter3D_h

#include "mdkParallelForLoop.h"
#include "mdkObjectArray.h"
#include "mdkDenseMatrix.h"
#include "mdkDenseImage3D.h"
#include "mdkSparseImage3D.h"

namespace mdk
{

// SISO: Single Input Image, Single Output Image
// Each OutputPixel can be stored in Image or PixelArray or both

template<typename InputImage_Type, typename OutputImage_Type, typename Scalar_Type>
class ImageFilter3D : public Object
{
public:
	typedef InputImage_Type  InputImageType;
	typedef OutputImage_Type OutputImageType;
	typedef typename InputImageType::PixelType  	 InputPixelType;
	typedef typename OutputImageType::PixelType  	 OutputPixelType;

	typedef Scalar_Type  ScalarType; // float or double

	typedef Option_Of_Image3DInterpolation<InputPixelType>  ImageInterpolationOptionType;
	typedef MethodEnum_Of_Image3DInterpolation              ImageInterpolationMethodEnum;
	typedef BoundaryOptionEnum_Of_Image3DInterpolation      ImageInterpolationBoundaryOptionEnum;

protected:
	//-------------------------- input --------------------------------------------------//
	const InputImageType* m_InputImage;

	ImageInterpolationOptionType m_ImageInterpolationOption;

	// only one of these is used
	const DenseMatrix<ScalarType>* m_PointList_3DWorldPosition;
	const DenseMatrix<ScalarType>* m_PointList_3DPosition_InputImage;
	const DenseMatrix<int_max>*    m_PointList_3DIndex_InputImage;
	const DenseMatrix<ScalarType>* m_PointList_3DPosition_OutputImage;
	const DenseMatrix<int_max>*    m_PointList_3DIndex_OutputImage;
	
	Image3DInfo m_OutputImageInfo;

	int_max m_MaxThreadCount; // max number of threads

	// -------------------- internal ----------------------------------------------------//
	int_max m_OutputPixelCount;

	bool m_Flag_ScanOutputImageGrid;
	// true: whole m_OutputImage
	// false: only scan each point in m_PointList_XXX

	bool m_Flag_EnableOutputImage;
	bool m_Flag_EnableOutputPixelArray;
	bool m_Flag_EnableOutputToOtherPlace;

	//------------------------- output ----------------------------------------------------//
	OutputImageType m_OutputImage;
	ObjectArray<OutputPixelType> m_OutputPixelArray;
	// or Other Place

protected:
	ImageFilter3D();
	virtual ~ImageFilter3D();

public:
	virtual void Clear();

	void SetInputImage(const InputImageType* InputImage);

	void SetOutputImageInfo(const Image3DInfo& Info);

	void SetOutputImageInfo(const DenseVector<double, 3>& Origin,
						    const DenseVector<double, 3>& Spacing,
						    const DenseVector<int_max, 3>& Size,
							const DenseMatrix<double>& Orientation);

	// Number of Pixel in x/y/z direction
	// Origin of output image = Origin of input image
	// Spacing of output image may not be equal to Spacing of input image
	void SetOutputImageInfoBySize(const DenseVector<int_max, 3>& Size);
	void SetOutputImageInfoBySize(int_max Lx, int_max Ly, int_max Lz);

	// Origin of output image = Origin of input image
	// Size of output image may not be equal to Size of input image
	void SetOutputImageInfoBySpacing(const DenseVector<double, 3>& Spacing);
	void SetOutputImageInfoBySpacing(double Spacing_x, double Spacing_y, double Spacing_z);

	const Image3DInfo& GetOutputImageInfo();

	void EnableOutputImage(bool On_Off = true);
	void EnableOutputPixelArray(bool On_Off = true);

	void InitializeOutputImage(); // call it if m_OutputImage is used before Update() is called

	OutputImageType* GetOutputImage();

	void InitializeOutputPixelArray(); // call it if m_OutputPixelArray is used before Update() is called

	ObjectArray<OutputPixelType>* GetOutputPixelArray();

	void SetPointListOf3DWorldPosition(const DenseMatrix<ScalarType>* ListOf3DWorldPosition);

	void SetPointListOf3DPositionInOutputImage(const DenseMatrix<ScalarType>* ListOf3DPosition);

	void SetPointListOf3DIndexInOutputImage(const DenseMatrix<int_max>* ListOf3DIndex);

	void SetPointListOf3DIndexInInputImage(const DenseMatrix<int_max>* ListOf3DIndex);

	void SetImageInterpolationOption(const ImageInterpolationOptionType& InputOption);

	const ImageInterpolationOptionType& GetImageInterpolationOption();

	void SetMaxThreadCount(int_max MaxNumber);

	int_max GetMaxThreadCount();

	virtual bool Update();

protected:
	virtual bool CheckInput();
	virtual bool Preprocess();
	virtual bool Postprocess() { return true; }

	inline virtual void Evaluate_in_a_thread(int_max PointIndex_start, int_max PointIndex_end, int_max ThreadIndex);

	// Evaluate at Point (x, y, z) with PointIndex
	// PointIndex may be LinearIndex in m_OutputImage, or index in m_PointList_XXX
	// PointIndex <=> (x,y,z) : from one we get the other, good for debug
	inline virtual OutputPixelType EvaluateAt3DPositionInInputImage(int_max PointIndex, ScalarType x0, ScalarType y0, ScalarType z0, int_max ThreadIndex) = 0;

	// If we want to store pixel NOT in m_OutputPixelArray, but some other place (some data object in a derived class)
	inline virtual void StoreOutputPixelToOtherPlace(OutputPixelType& OutputPixel, int_max PointIndex, int_max ThreadIndex) {}

	int_max GetOptimalThreadCount();
	
	//---------- use these three function to convert Index/Position --------------------------------//

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DPositionInInputImageTo3DPositionInOutputImage(const DenseVector<ScalarType, 3>& Position);

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DIndexInInputImageTo3DPositionInOutputImage(const DenseVector<IndexType, 3>& Index3D);

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DPositionInOutputImageTo3DPositionInInputImage(const DenseVector<ScalarType, 3>& Position);

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DIndexInOutputImageTo3DPositionInInputImage(const DenseVector<IndexType, 3>& Index3D);


	//--------------------------------------------------------------------------------------------
	DenseVector<int_max, 3> TransformLinearIndexTo3DIndexInOutputImage(int_max LinearIndex);

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DIndexTo3DPositionInOutputImage(const DenseVector<IndexType, 3>& Index3D);

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DIndexInOutputImageTo3DWorldPosition(const DenseVector<IndexType, 3>& Index3D);

	template<typename IndexType>
	DenseVector<ScalarType, 3> Transform3DIndexInOutputImageTo3DWorldPosition(IndexType x_Index, IndexType y_Index, IndexType z_Index);

	DenseVector<ScalarType, 3> Transform3DPositionInOutputImageTo3DWorldPosition(ScalarType x, ScalarType y, ScalarType z);

	DenseVector<ScalarType, 3> Transform3DWorldPositionTo3DIndexInOutputImage(const DenseVector<ScalarType, 3>& Position);

	DenseVector<ScalarType, 3> Transform3DWorldPositionTo3DIndexInOutputImage(ScalarType x, ScalarType y, ScalarType z);

private:
	ImageFilter3D(const ImageFilter3D&) = delete;
	void operator=(const ImageFilter3D&) = delete;
};

}// namespace mdk

#include "mdkImageFilter3D.hpp"

#endif