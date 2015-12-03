﻿#ifndef mdk_PolygonMeshProcessing_Function_Part_0_h
#define mdk_PolygonMeshProcessing_Function_Part_0_h

#include "vtkSmoothPolyDataFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"

#include "mdkDenseMatrix.h"
#include "mdkPolygonMesh.h"

namespace mdk
{

template<typename MeshAttributeType>
DenseVector<Handle_Of_Point_Of_MembraneMesh> TraceMeshBoundaryCurve(const PolygonMesh<MeshAttributeType>& TargetMesh, Handle_Of_Point_Of_MembraneMesh PointHandle_start);

template<typename MeshAttributeType>
ObjectArray<DenseVector<Handle_Of_Point_Of_MembraneMesh>> TraceMeshBoundaryCurve(const PolygonMesh<MeshAttributeType>& TargetMesh);

template<typename MeshAttributeType>
Handle_Of_Point_Of_MembraneMesh FindNearestPointOnMesh(const PolygonMesh<MeshAttributeType>& TargetMesh,  const DenseVector<typename MeshAttributeType::ScalarType, 3>& PointPosition);

template<typename MeshAttributeType>
PolygonMesh<MeshAttributeType> SmoothMeshByVTKSmoothPolyDataFilter(const PolygonMesh<MeshAttributeType>& InputMesh, int_max Iter, bool Flag_FeatureEdgeSmoothing = true, bool Flag_BoundarySmoothing = true);

template<typename MeshAttributeType>
PolygonMesh<MeshAttributeType> SmoothMeshByVTKWindowedSincPolyDataFilter(const PolygonMesh<MeshAttributeType>& InputMesh, double PassBand, int_max Iter, bool Flag_FeatureEdgeSmoothing = true, bool Flag_BoundarySmoothing = true);

}//namespace mdk

#include "mdkPolygonMeshProcessing_Function_Part_0.hpp"

#endif