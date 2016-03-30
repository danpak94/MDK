﻿#pragma once

#include "mdkCoordinateTransform3D.h"
#include "mdkDenseMatrix.h"

namespace mdk
{
// TargetPoint = RigidTransform(SourcePoint)= RotationMatrix*SourcePoint + TranslationVetor
// Y = RX+T 
//
// RotationMatrix: rotate a-angle around X-axis, Y-axis, Z-axis
// Rx = [1,      0,  0
//       0, cos(a), -sin(a)
//       0, sin(a), cos(a)]
//
// Ry = [cos(b),   0,  sin(b)
//            0,   1,  0
//       -sin(b),  0,  cos(b)]
//
// Rz = [cos(c),  -sin(c),  0
//       sin(c),   cos(c),  0
//            0,        0,  1]
//
// use right-hand coordinate system
// Reference: Estimating 3-D rigid body transformations: a comparison of four major algorithms

template<typename Scalar_Type>
class RigidTransform3D : public CoordinateTransform3D<Scalar_Type>
{
public:
	typedef Scalar_Type ScalarType;

private:
	// m_SourceLandmarkPointSet and m_TargetLandmarkPointSet must be valid when EstimateParameter() is called
	const DenseMatrix<ScalarType>* m_SourceLandmarkPointSet; // 3 x N
	const DenseMatrix<ScalarType>* m_TargetLandmarkPointSet; // 3 x N

	DenseMatrix<ScalarType> m_Rotation;
	DenseVector<ScalarType, 3> m_Translation_AfterRotation;

public:
	RigidTransform3D();
	~RigidTransform3D();

	void Clear();

	void SetSourceLandmarkPointSet(const DenseMatrix<ScalarType>* SourceLandmarkPointSet);
	void SetTargetLandmarkPointSet(const DenseMatrix<ScalarType>* TargetLandmarkPointSet);
	void EstimateParameter();

	void SetRotationMatrix(const DenseMatrix<ScalarType>& Rotation);
	DenseMatrix<ScalarType> GetRotationMatrix() const;

	void SetTranslation_AfterRotation(const DenseVector<ScalarType, 3>& Translation);
	DenseVector<ScalarType, 3> GetTranslation_AfterRotation() const;

	inline DenseVector<ScalarType, 3> TransformPoint(ScalarType x, ScalarType y, ScalarType z) const;
	using CoordinateTransform3D::TransformPoint;

private:
	bool CheckLandmarkPointSet();

private:
	RigidTransform3D(const RigidTransform3D&) = delete;
	void operator=(const RigidTransform3D&) = delete;
};

}//namespace mdk

#include "mdkRigidTransform3D.hpp"

