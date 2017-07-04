﻿#pragma once

#include "vtkSmoothPolyDataFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkPlane.h"
#include "vtkClipPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkDijkstraGraphGeodesicPath.h"

namespace mdk
{
//no double count
template<typename ScalarType>
DenseVector<int_max> TraceMeshBoundaryCurve(const PolygonMesh<ScalarType>& InputMesh, int_max PointIndex_start);

template<typename ScalarType>
ObjectArray<DenseVector<int_max>> TraceMeshBoundaryCurve(const PolygonMesh<ScalarType>& InputMesh);

template<typename ScalarType>
int_max FindNearestPointOnMesh(const PolygonMesh<ScalarType>& InputMesh,  const DenseVector<ScalarType, 3>& PointPosition);

template<typename ScalarType>
PolygonMesh<ScalarType> SmoothMeshByVTKSmoothPolyDataFilter(const PolygonMesh<ScalarType>& InputMesh, int_max Iter, bool Flag_FeatureEdgeSmoothing = true, bool Flag_BoundarySmoothing = true);

template<typename ScalarType>
PolygonMesh<ScalarType> SmoothMeshByVTKWindowedSincPolyDataFilter(const PolygonMesh<ScalarType>& InputMesh, double PassBand, int_max Iter, bool Flag_FeatureEdgeSmoothing = true, bool Flag_BoundarySmoothing = true);

//output FaceIndexList with seed FaceIndex_seed
//ClosedEdgeCurve_EdgeIndexList can be in random order
//Attention: EdgeIndex, NOT PointIndex
template<typename ScalarType>
DenseVector<int_max> SegmentMeshByEdgeCurve(const PolygonMesh<ScalarType>& InputMesh, const DenseVector<int_max>& ClosedEdgeCurve_EdgeIndexList, const int_max FaceIndex_seed);

template<typename ScalarType>
DenseVector<DenseVector<int_max>> SegmentMeshByEdgeCurve(const PolygonMesh<ScalarType>& InputMesh, const DenseVector<int_max>& ClosedEdgeCurve_EdgeIndexList);

//clip mesh using a plane defined by origin and normal
template<typename ScalarType>
PolygonMesh<ScalarType> ClipMeshByVTKClipPolyData(vtkPolyData* InputMesh_vtk, const DenseVector<ScalarType, 3>& Origin, const DenseVector<ScalarType, 3>& Normal);

//clip mesh using a plane defined by origin and normal
template<typename ScalarType>
PolygonMesh<ScalarType> ClipMeshByVTKClipPolyData(const PolygonMesh<ScalarType>& InputMesh, const DenseVector<ScalarType, 3>& Origin, const DenseVector<ScalarType, 3>& Normal);

DenseVector<int_max> FindShortestPathByVTKDijkstraGraphGeodesicPath(vtkPolyData* InputMesh_vtk, int_max PointIndex_start, int_max PointIndex_end);

template<typename ScalarType>
DenseVector<int_max> FindShortestPathByVTKDijkstraGraphGeodesicPath(const PolygonMesh<ScalarType>& InputMesh, int_max PointIndex_start, int_max PointIndex_end);

template<typename ScalarType>
PolygonMesh<ScalarType> MergeMeshBoundary(const PolygonMesh<ScalarType>& InputMeshA, const PolygonMesh<ScalarType>& InputMeshB, ScalarType Threshold);

}//namespace mdk

#include "mdkPolygonMeshProcessing_Function_Part_0.hpp"
