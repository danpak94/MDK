#ifndef mdk_RotationTransform3D_h
#define mdk_RotationTransform3D_h

#include "mdkCoordinateTransform3D.h"
#include "mdkDenseMatrix.h"

namespace mdk
{
// TargetPoint = RotationTransform(SourcePoint)= RotationMatrix*SourcePoint
// RotationMatrix = Rz*Ry*Rx;
// Rx = [1,      0,  0
//       0, cos(a), -sin(a)
//       0, sin(a), cos(a)]
//
// Ry = [cos(b),   0,  sin(b)
//            0,   1,  0
//       -sin(b),  0,  cos(b)]
//
// Rx = [cos(c),  -sin(c),  0
//       sin(c),   cos(c),  0
//            0,        0,  1]
//
// use right-hand coordinate system
// Reference: Estimating 3-D rigid body transformations: a comparison of four major algorithms

template<typename Scalar_Type>
class RotationTransform3D : public CoordinateTransform3D<Scalar_Type>
{
public:
	typedef Scalar_Type ScalarType;

private:
	// m_SourceControlPointSet and m_TargetControlPointSet must be valid when EstimateParameter() is called
	const DenseMatrix<ScalarType>* m_SourceControlPointSet; // 3 x N
	const DenseMatrix<ScalarType>* m_TargetControlPointSet; // 3 x N

	DenseMatrix<ScalarType> m_Rotation;

	DenseVector<ScalarType, 3> m_RotationCenter;

public:
	RotationTransform3D();
	~RotationTransform3D();

	void Clear();

	void SetSourceControlPointSet(const DenseMatrix<ScalarType>* SourcePointSet);
	void SetTargetControlPointSet(const DenseMatrix<ScalarType>* TargetPointSet);
	void EstimateParameter();

	void SetRotationMatrixByAngle(const DenseVector<ScalarType, 3>& AngleList);
	void SetRotationMatrixByAngle(ScalarType AngleX, ScalarType AngleY, ScalarType AngleZ);

	void SetRotationMatrix(const DenseMatrix<ScalarType>& Rotation);
	DenseMatrix<ScalarType> GetRotationMatrix() const;

	void SetRotationCenter(const DenseVector<ScalarType, 3>& Center);
	DenseVector<ScalarType, 3> GetRotationCenter() const;

	DenseVector<ScalarType, 3> TransformPoint(ScalarType x, ScalarType y, ScalarType z) const;
	using CoordinateTransform3D::TransformPoint;

private:
	bool CheckControlPointSet();

public:
	static DenseMatrix<ScalarType> ComputeRotationMatrix_Rx_ByAngleX(ScalarType AngleX);
	static DenseMatrix<ScalarType> ComputeRotationMatrix_Ry_ByAngleY(ScalarType AngleY);
	static DenseMatrix<ScalarType> ComputeRotationMatrix_Rz_ByAngleZ(ScalarType AngleZ);
	static DenseMatrix<ScalarType> ComputeRotationMatrixByAngleXYZ(const DenseVector<ScalarType, 3>& AngleList);
	static DenseMatrix<ScalarType> ComputeRotationMatrixByAngleXYZ(ScalarType AngleX, ScalarType AngleY, ScalarType AngleZ);
	static DenseMatrix<ScalarType> ComputeRotationMatrixByAngleAlongAxis(ScalarType Angle, const DenseVector<ScalarType, 3>& Axis);

	static DenseVector<ScalarType, 3> RotatePoint(const DenseVector<ScalarType, 3>& PointPosition,											      
												  const DenseMatrix<ScalarType>& RotationMatrix,
												  const DenseVector<ScalarType, 3>& RotationCenter);

	//RotationCenter is [0, 0, 0]
	static DenseVector<ScalarType, 3> RotatePoint(const DenseVector<ScalarType, 3>& PointPosition,
												  const DenseMatrix<ScalarType>& RotationMatrix);
private:
	RotationTransform3D(const RotationTransform3D&) = delete;
	void operator=(const RotationTransform3D&) = delete;
};

}//namespace mdk

#include "mdkRotationTransform3D.hpp"

#endif
