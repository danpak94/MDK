#pragma once

#include "mdkMinimumStretchBasedTriangleMesh3DTo2DMapper.h"
#include "mdkGeometry.h"

namespace mdk
{
template<typename Scalar_Type>
class TemplateBasedSurfaceRemesher : public Object
{
public:
	typedef Scalar_Type ScalarType;
	typedef typename TriangleMeshStandardAttributeType<ScalarType>      InputMeshAttribute;
	typedef typename PolygonMeshStandardAttributeType<ScalarType>       OutputMeshAttribute;
	typedef OutputMeshAttribute                                         TemplateMeshAttribute;
	
	typedef typename TriangleMesh<InputMeshAttribute>    InputMeshType;
	typedef typename PolygonMesh<OutputMeshAttribute>    TemplateMeshType;
	typedef typename PolygonMesh<OutputMeshAttribute>    OutputMeshType;

private:
	//-------------------- input -------------------------------//

	// input mesh must be triangle mesh
	TriangleMesh<InputMeshAttribute> m_InputMesh;

	ObjectArray<DenseVector<int_max>> m_BoundarySegmentListOfInputMesh;
	//m_BoundaryListOfInputMesh[k] is BoundarySegment: a set of boundary point index
	//BoundarySegment = {PointIndex_start, ..., PointIndex_end}
	//BoundarySegment[k] overlap with BoundarySegment[k-1] and BoundarySegment[k+1] at start/end point
	//m_BoundaryListOfInputMesh[k] ~ m_BoundarySegmentListOfOutputMesh[k] 

	//template mesh point position is [u, v, 0]
	PolygonMesh<TemplateMeshAttribute> m_TemplateMesh;

	ObjectArray<DenseVector<int_max>> m_BoundarySegmentListOfTemplateMesh;
	//m_BoundarySegmentListOfOutputMesh[k] is BoundarySegment: a set of boundary point index
	//BoundarySegment = {PointIndex_start, ..., PointIndex_end}
	//BoundarySegment[k] overlap with BoundarySegment[k-1] and BoundarySegment[k+1] at start/end point
	//m_BoundarySegmentListOfOutputMesh[k] ~ m_BoundaryListOfInputMesh[k] 

	//Parameter to MinimumStretchBasedTriangleMesh3DTo2DMapper
	ScalarType m_DiffusionCoefficient;
	int_max m_MaxInteration;
	//-------------------- internal ---------------------------//

	DenseVector<int_max> m_BoundaryPointIndexListOfInputMesh;
	DenseMatrix<ScalarType> m_UVTalbleOfBoundaryOfInputMesh;// each col is [u,v]

	DenseVector<int_max> m_BoundaryPointIndexListOfTemplateMesh;
	DenseMatrix<ScalarType> m_BoundaryPositionOfTemplateMesh;// each col is [u,v,0]
	DenseMatrix<ScalarType> m_BoundaryPositionOfOutputMesh;  // each col is [x,y,z]

	bool m_Flag_Use_TPS_Transform = false;

	//------------------- output -----------------------------//
	TriangleMesh<InputMeshAttribute> m_TransfromedInputMesh;//2D mesh

	// OutputMesh is Transformed TemplateMesh from 2D to 3D
	PolygonMesh<OutputMeshAttribute> m_OutputMesh;

public:
	TemplateBasedSurfaceRemesher();
	~TemplateBasedSurfaceRemesher();

	TriangleMesh<InputMeshAttribute>& InputMesh() { return m_InputMesh; }
	ObjectArray<DenseVector<int_max>>& BoundarySegmentListOfInputMesh() { return m_BoundarySegmentListOfInputMesh; }
	PolygonMesh<TemplateMeshAttribute>& TemplateMesh() { return m_TemplateMesh; }
	ObjectArray<DenseVector<int_max>>& BoundarySegmentListOfTemplateMesh() { return m_BoundarySegmentListOfTemplateMesh; }
	PolygonMesh<OutputMeshAttribute>& OutputMesh() { return m_OutputMesh; }
	TriangleMesh<InputMeshAttribute>& TransfromedInputMesh() { return m_TransfromedInputMesh; }
	void SetDiffusionCoefficientOfMeshParameterization(ScalarType Coef) { m_DiffusionCoefficient = Coef; }
	void SetMaxIterationOfMeshParameterization(int_max MaxIter) { m_MaxInteration = MaxIter; }
	void EnableTPSTransformOfTemplateMesh(bool Flag = true) { m_Flag_Use_TPS_Transform = Flag; }

	void Clear();
	void Update();

private:
	bool CheckInput();
	void ClearInternalData();
	void FindBoundaryConstraint();
	bool CheckBoundaryConstraint();
	void TransformInputMeshFrom3DTo2D();
	void TransfromTemplateMeshFrom2Dto3D();
	void TransfromTemplateMeshFrom2Dto3D_Method0_TPS();
	void TransfromTemplateMeshFrom2Dto3D_Method1_Interpolation();

	DenseVector<ScalarType> ComputeCumulativeCurveLength(const DenseMatrix<ScalarType>& CurvePosition);

	// relative length is from 0 to 1
	DenseVector<ScalarType> ComputeCumulativeCurveLength_Relative(const DenseMatrix<ScalarType>& CurvePosition);

	DenseVector<int_max, 3> Find3PointOfNearestFace(const DenseVector<ScalarType, 3>& Point, const TriangleMesh<InputMeshAttribute>& TargetMesh);
	
private:
	TemplateBasedSurfaceRemesher(const TemplateBasedSurfaceRemesher&) = delete;
	void operator=(const TemplateBasedSurfaceRemesher&) = delete;
};

}//namespace mdk

#include "mdkTemplateBasedSurfaceRemesher.hpp"
