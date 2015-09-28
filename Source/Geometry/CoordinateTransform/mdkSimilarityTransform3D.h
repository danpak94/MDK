﻿#ifndef mdk_SimilarityTransform3D_h
#define mdk_SimilarityTransform3D_h

#include "mdkCoordinateTransform3D.h"
#include "mdkDenseMatrix.h"

namespace mdk
{
// TargetPoint = SimilarityTransform(SourcePoint)
//	           = Scale*RotationMatrix*SourcePoint+Translation
//             = Scale*RotationMatrix*(SourcePoint-RotationCenter) + RotationCenter
// Y = sRX+T <=> Y=s*R(X-C)+C, s is scaling, C is rotation center and C = pinv(I-s*R)*(Yc-s*R*Xc), Yc=mean(Y), Xc=mean(X)
//
// use right-hand coordinate system
// Reference: Least-squares Estimation of Transformation Parameters Between Two Point Patterns, 1991

template<typename Scalar_Type>
class SimilarityTransform3D : public CoordinateTransform3D<Scalar_Type>
{
public:
	typedef Scalar_Type ScalarType;

private:
	// m_SourceLandmarkPointSet and m_TargetLandmarkPointSet must be valid when EstimateParameter() is called
	const DenseMatrix<ScalarType>* m_SourceLandmarkPointSet; // 3 x N
	const DenseMatrix<ScalarType>* m_TargetLandmarkPointSet; // 3 x N

	ScalarType m_Scale;
	DenseMatrix<ScalarType> m_Rotation;
	DenseVector<ScalarType, 3> m_Translation_After_Scale_Rotation;

public:
	SimilarityTransform3D();
	~SimilarityTransform3D();

	void Clear();

	void SetSourceLandmarkPointSet(const DenseMatrix<ScalarType>* SourceLandmarkPointSet) { m_SourceLandmarkPointSet = SourceLandmarkPointSet; }
	void SetTargetLandmarkPointSet(const DenseMatrix<ScalarType>* TargetLandmarkPointSet) { m_TargetLandmarkPointSet = TargetLandmarkPointSet; }
	void EstimateParameter();

	void SetScale(ScalarType Scale) { m_Scale = Scale; }
	ScalarType GetScale() const { return m_Scale; }

	void SetRotationMatrix(const DenseMatrix<ScalarType>& Rotation) { m_Rotation = Rotation; }
	DenseMatrix<ScalarType> GetRotationMatrix() const { return m_Rotation; }

	void SetTranslation_After_Scale_Rotation(const DenseVector<ScalarType, 3>& Translation) { m_Translation_After_Scale_Rotation = Translation; }
	DenseVector<ScalarType, 3> GetTranslation_After_Scale_Rotation() const { return m_Translation_After_Scale_Rotation; }

	inline DenseVector<ScalarType, 3> TransformPoint(ScalarType x, ScalarType y, ScalarType z) const;
	using CoordinateTransform3D::TransformPoint;

private:
	bool CheckLandmarkPointSet();

private:
	SimilarityTransform3D(const SimilarityTransform3D&) = delete;
	void operator=(const SimilarityTransform3D&) = delete;
};

}//namespace mdk

#include "mdkSimilarityTransform3D.hpp"

#endif
