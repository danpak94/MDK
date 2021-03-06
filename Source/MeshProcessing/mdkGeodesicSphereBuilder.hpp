#pragma once

namespace mdk
{

template<typename ScalarType>
GeodesicSphereBuilder<ScalarType>::GeodesicSphereBuilder()
{
	this->Clear();
}


template<typename ScalarType>
GeodesicSphereBuilder<ScalarType>::~GeodesicSphereBuilder()
{
}


template<typename ScalarType>
void GeodesicSphereBuilder<ScalarType>::Clear()
{
	Input.MaxDepth = -1;
	Output.SphereList.Clear();	
}

template<typename ScalarType>
void GeodesicSphereBuilder<ScalarType>::Update()
{
	if (Input.MaxDepth < 0)
	{
		MDK_Error("MaxDepth < 0 @ GeodesicSphereBuilder::Update()")
		return;
	}

	Output.SphereList.Clear();
	Output.SphereList.Resize(Input.MaxDepth+1);

	this->BuildInitialSphere();

	for (int_max k = 1; k <= Input.MaxDepth; ++k)
	{
		this->BuildSphereAtDepth(k);
	}
}


template<typename ScalarType>
void GeodesicSphereBuilder<ScalarType>::BuildInitialSphere()
{// icosahedron
	auto& Self = *this;

	const auto X = ScalarType(0.525731112119133606);
	const auto Z = ScalarType(0.850650808352039932);
	ObjectArray<DenseVector<ScalarType, 3>> vdata;//point
	vdata = { { -X, 0.0, Z }, { X, 0.0, Z },  { -X, 0.0, -Z }, { X, 0.0, -Z },
		      { 0.0, Z, X },  { 0.0, Z, -X }, { 0.0, -Z, X },  { 0.0, -Z, -X },
		      { Z, X, 0.0 },  { -Z, X, 0.0 }, { Z, -X, 0.0 },  { -Z, -X, 0.0 }};

	ObjectArray<DenseVector<int, 3>> edata;//cell
	edata = { { 0, 4, 1 },  { 0, 9, 4 },  { 9, 5, 4 },  { 4, 5, 8 },  { 4, 8, 1 },
		      { 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 },  { 5, 2, 3 },  { 2, 7, 3 },
		      { 7, 10, 3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
		      { 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 },  { 7, 2, 11 }};

	DenseVector<int_max> PointIndexList;
	PointIndexList.Resize(12);
	for (int_max k = 0; k < 12; ++k)
	{
		PointIndexList[k] = Output.SphereList[0].AddPoint(vdata[k]);
	}

	Output.SphereList[0].SetCapacity(12, 30, 20);
	for (int_max k = 0; k < 20; ++k)
	{
		auto H0 = PointIndexList[edata[k][0]];
		auto H1 = PointIndexList[edata[k][1]];
		auto H2 = PointIndexList[edata[k][2]];
		Output.SphereList[0].AddFaceByPoint(H0, H1, H2);
	}
}


template<typename ScalarType>
void GeodesicSphereBuilder<ScalarType>::BuildSphereAtDepth(int_max Depth)
{
	auto PointCount_prev = Output.SphereList[Depth - 1].GetPointCount();
	auto EdgeCount_prev = Output.SphereList[Depth - 1].GetEdgeCount();
	auto FaceCount_prev = Output.SphereList[Depth - 1].GetFaceCount();
	auto PointCount = PointCount_prev + EdgeCount_prev;
	auto EdgeCount  = EdgeCount_prev*2;
	auto FaceCount = FaceCount_prev * 4;
	Output.SphereList[Depth].SetCapacity(PointCount, EdgeCount, FaceCount);
	
	//------- add initial point by copying all point of previous Sphere ----------------//
	DenseVector<int_max> PointIndexList_init;
	PointIndexList_init.Resize(PointCount_prev);
	for (int_max k = 0; k <= Output.SphereList[Depth - 1].GetMaxValueOfPointIndex(); ++k)
	{//PointIndex is always valid
		auto Pos = Output.SphereList[Depth - 1].GetPointPosition(k);
		PointIndexList_init[k] = Output.SphereList[Depth].AddPoint(Pos);
	}

	//------- add new point by splitting each edge of previous Sphere -----------------//   
	DenseVector<int_max> PointIndexList_new;
	for (int_max k = 0; k <= Output.SphereList[Depth - 1].GetMaxValueOfEdgeIndex(); ++k)
	{//EdgeIndex is always valid
		auto TempList = Output.SphereList[Depth - 1].Edge(k).GetPointIndexList();
		auto P0 = Output.SphereList[Depth - 1].GetPointPosition(TempList[0]);
		auto P1 = Output.SphereList[Depth - 1].GetPointPosition(TempList[1]);
		auto P3 = P0 + P1;
		P3 /= P3.L2Norm();
		auto H3 = Output.SphereList[Depth].AddPoint(P3);
		PointIndexList_new.Append(H3);
	}

	//------- add cell to current Sphere by splitting each cell of previous Sphere ----//   
	for (int_max k = 0; k <= Output.SphereList[Depth - 1].GetMaxValueOfFaceIndex(); ++k)
	{//FaceIndex is always valid
		auto PointIndexList_prev = Output.SphereList[Depth - 1].Face(k).GetPointIndexList(); // P0, P1, P2
		auto EdgeIndexList_prev = Output.SphereList[Depth - 1].Face(k).GetEdgeIndexList();   // P0-P1, P1-P2, P2-P1
		//-----------------
		//      0
		//    3    5
		// 1    4     2
		//-----------------		
		auto H0 = PointIndexList_init[PointIndexList_prev[0]];
		auto H1 = PointIndexList_init[PointIndexList_prev[1]];
		auto H2 = PointIndexList_init[PointIndexList_prev[2]];
		auto H3 = PointIndexList_new[EdgeIndexList_prev[0]];
		auto H4 = PointIndexList_new[EdgeIndexList_prev[1]];
		auto H5 = PointIndexList_new[EdgeIndexList_prev[2]];
		Output.SphereList[Depth].AddFaceByPoint(H0, H3, H5);
		Output.SphereList[Depth].AddFaceByPoint(H3, H1, H4);
		Output.SphereList[Depth].AddFaceByPoint(H3, H4, H5);
		Output.SphereList[Depth].AddFaceByPoint(H5, H4, H2);
	}
}

}//namespace mdk
