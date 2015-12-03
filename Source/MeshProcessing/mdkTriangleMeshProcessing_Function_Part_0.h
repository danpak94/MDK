﻿#ifndef mdk_TriangleMeshProcessing_Function_Part_0_h
#define mdk_TriangleMeshProcessing_Function_Part_0_h

#include <vtkLinearSubdivisionFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkDecimatePro.h>

#include "mdkDenseMatrix.h"
#include "mdkTriangleMesh.h"

namespace mdk
{

template<typename MeshAttributeType>
DenseVector<Handle_Of_Point_Of_MembraneMesh> TraceMeshBoundaryCurve(const TriangleMesh<MeshAttributeType>& TargetMesh,
                                                                    Handle_Of_Point_Of_MembraneMesh PointHandle_start);

template<typename MeshAttributeType>
ObjectArray<DenseVector<Handle_Of_Point_Of_MembraneMesh>> TraceMeshBoundaryCurve(const TriangleMesh<MeshAttributeType>& TargetMesh);

template<typename MeshAttributeType>
Handle_Of_Point_Of_MembraneMesh FindNearestPointOnMesh(const TriangleMesh<MeshAttributeType>& TargetMesh, 
                                                       const DenseVector<typename MeshAttributeType::ScalarType, 3>& PointPosition);

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SubdivideTriangleMesh_Linear(const TriangleMesh<MeshAttributeType>& InputMesh);// divide once only

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SubdivideTriangleMesh_Linear(const TriangleMesh<MeshAttributeType>& InputMesh, int_max SubdivisionNumber);

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SubdivideTriangleMeshByVTKLinearSubdivisionFilter(const TriangleMesh<MeshAttributeType>& TargetMesh, int_max SubdivisionNumber);

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SimplifyTriangleMeshByVTKDecimatePro(const TriangleMesh<MeshAttributeType>& TargetMesh, double TargetReduction);

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SimplifyTriangleMeshByVTKQuadricDecimation(const TriangleMesh<MeshAttributeType>& TargetMesh, double TargetReduction);

// assume GaussianCurvature has been computed at each point of TargetMesh if Flag_UpdateAttribute is false
template<typename MeshAttributeType>
void SmoothTriangleMeshByGaussianCurvature(TriangleMesh<MeshAttributeType>& TargetMesh, double MaxDisplacement, bool Flag_UpdateAttribute = true);

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SmoothMeshByVTKSmoothPolyDataFilter(const TriangleMesh<MeshAttributeType>& InputMesh, int_max Iter, bool Flag_FeatureEdgeSmoothing = true, bool Flag_BoundarySmoothing = true);

template<typename MeshAttributeType>
TriangleMesh<MeshAttributeType> SmoothMeshByVTKWindowedSincPolyDataFilter(const TriangleMesh<MeshAttributeType>& InputMesh, double PassBand, int_max Iter, bool Flag_FeatureEdgeSmoothing = true, bool Flag_BoundarySmoothing = true);

}//namespace mdk

#include "mdkTriangleMeshProcessing_Function_Part_0.hpp"

#endif