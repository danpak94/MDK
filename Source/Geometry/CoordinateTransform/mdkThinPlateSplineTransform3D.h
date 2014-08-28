#ifndef __mdkThinPlateSplineTransform3D_h
#define __mdkThinPlateSplineTransform3D_h

#include "mdkDenseMatrix.h"

namespace mdk
{
// reference MDK_Document
//	Thin-Plate Spline Transformation.doc and TPS3D.m
// "A physics based coordinate transform for 3D image match" at TMI 1997

// ScalarType is float or double
template<typename Scalar_Type>
class ThinPlateSplineTransform3D : public Object
{
public:
	typedef Scalar_Type ScalarType;

private:
	DenseMatrix<ScalarType> m_SourceControlPointSet; // 3 x N
	DenseMatrix<ScalarType> m_TargetControlPointSet; // 3 x N
	DenseMatrix<ScalarType> m_Parameter;  // (N+4) x 3

public:
	ThinPlateSplineTransform3D();
	~ThinPlateSplineTransform3D();

	void Clear();

	void SetSourceControlPointSet(const DenseMatrix<ScalarType>& SourcePointSet);
	const DenseMatrix<ScalarType>& GetSourceControlPointSet() const;

	void SetTargetControlPointSet(const DenseMatrix<ScalarType>& TargetPointSet);
	const DenseMatrix<ScalarType>& GetTargetControlPointSet() const;

	void SetParameter(const DenseMatrix<ScalarType>& Parameter);
	const DenseMatrix<ScalarType>& GetParameter() const;

	void UpdateParameter();

	DenseVector<ScalarType, 3> TransformPoint(ScalarType x, ScalarType y, ScalarType z) const;

private:
	ThinPlateSplineTransform3D(const ThinPlateSplineTransform3D&) = delete;
	void operator=(const ThinPlateSplineTransform3D&) = delete;
};

}//namespace mdk

#include "mdkThinPlateSplineTransform3D.hpp"

#endif