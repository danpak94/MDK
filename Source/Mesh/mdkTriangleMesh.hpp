﻿#pragma once

namespace mdk
{

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh()
: PolygonMesh()
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(const MDK_Symbol_PureEmpty&)
: Mesh(MDK_PureEmpty)
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(const TriangleMesh<ScalarType>& InputMesh)
: PolygonMesh(InputMesh)
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(TriangleMesh<ScalarType>&& InputMesh)
: PolygonMesh(std::move(InputMesh))
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(const PolygonMesh<ScalarType>& InputMesh)
: PolygonMesh(InputMesh)
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(PolygonMesh<ScalarType>&& InputMesh)
: PolygonMesh(std::move(InputMesh))
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(const Mesh<ScalarType>& InputMesh)
: PolygonMesh(InputMesh)
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::TriangleMesh(Mesh<ScalarType>&& InputMesh)
: PolygonMesh(std::move(InputMesh))
{
}

template<typename ScalarType>
TriangleMesh<ScalarType>::~TriangleMesh()
{
}

template<typename ScalarType>
inline
void TriangleMesh<ScalarType>::operator=(const TriangleMesh<ScalarType>& InputMesh)
{
	this->PolygonMesh::operator=(InputMesh);
}

template<typename ScalarType>
inline
void TriangleMesh<ScalarType>::operator=(TriangleMesh<ScalarType>&& InputMesh)
{
	this->PolygonMesh::operator=(std::move(InputMesh));
}

template<typename ScalarType>
inline
void TriangleMesh<ScalarType>::operator=(const PolygonMesh<ScalarType>& InputMesh)
{
	this->PolygonMesh::operator=(InputMesh);
}

template<typename ScalarType>
inline
void TriangleMesh<ScalarType>::operator=(PolygonMesh<ScalarType>&& InputMesh)
{
	this->PolygonMesh::operator=(std::move(InputMesh));
}

template<typename ScalarType>
inline
void TriangleMesh<ScalarType>::operator=(const Mesh<ScalarType>& InputMesh)
{
	this->PolygonMesh::operator=(InputMesh);
}

template<typename ScalarType>
inline
void TriangleMesh<ScalarType>::operator=(Mesh<ScalarType>&& InputMesh)
{
	this->PolygonMesh::operator=(std::move(InputMesh));
}

template<typename ScalarType>
inline 
int_max TriangleMesh<ScalarType>::AddFaceByEdge(int_max EdgeIndex0, int_max EdgeIndex1, int_max EdgeIndex2)
{
    DenseVector<EdgeIndexType> EdgeIndexList = { EdgeIndex0, EdgeIndex1, EdgeIndex2 };
    return this->PolygonMesh::AddFaceByEdge(EdgeIndexList);
}

template<typename ScalarType>
inline
int_max TriangleMesh<ScalarType>::AddFaceByPoint(int_max PointIndex0, int_max PointIndex1, int_max PointIndex2)
{
    DenseVector<int_max> PointIndexList = { PointIndex0, PointIndex1, PointIndex2 };
    return this->PolygonMesh::AddFaceByPoint(PointIndexList);
}

template<typename ScalarType>
TriangleMesh<ScalarType> TriangleMesh<ScalarType>::GetSubMeshByFace(const DenseVector<int_max>& FaceIndexList) const
{
	TriangleMesh<ScalarType> OutputMesh = this->PolygonMesh::GetSubMeshByFace(FaceIndexList);
	return TriangleMesh;
}

//------------- Function optimized for TriangleMesh --------------------------------------------------//

template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateNormalAtFace(const MDK_Symbol_ALL&)
{ 
	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();
    for (int_max k=0; k<= FaceIndex_max; ++k)
    {
		if (this->IsValidFaceIndex(k) == true)
		{
			this->UpdateNormalAtFace(k);
		}
    }
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateNormalAtFace(int_max FaceIndex)
{
    if (this->IsValidFaceIndex(FaceIndex) == false)
    {
        MDK_Warning("FaceIndex is invalid @ TriangleMesh::UpdateNormalAtFace()")
        return;
    }

    auto PointIndexList = this->Face(FaceIndex).GetPointIndexList();
	auto PointPositionA = this->GetPointPosition(PointIndexList[0]);
	auto PointPositionB = this->GetPointPosition(PointIndexList[1]);
	auto PointPositionC = this->GetPointPosition(PointIndexList[2]);
	auto Normal = ComputeTriangleNormalIn3D(PointPositionA, PointPositionB, PointPositionC);
    this->Face(FaceIndex).Attribute().Normal = Normal;
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateAreaOfFace(const MDK_Symbol_ALL&)
{
	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();
	for (int_max k = 0; k <= FaceIndex_max; ++k)
	{
		if (this->IsValidFaceIndex(k) == true)
		{
			this->UpdateAreaOfFace(k);
		}
	}
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateAreaOfFace(int_max FaceIndex)
{
    if (this->IsValidFaceIndex(FaceIndex) == false)
    {
        MDK_Warning("FaceIndex is invalid @ TriangleMesh::UpdateAreaOfFace()")
        return;
    }

    auto PointIndexList = this->Face(FaceIndex).GetPointIndexList();
	auto PointPositionA = this->GetPointPosition(PointIndexList[0]);
	auto PointPositionB = this->GetPointPosition(PointIndexList[1]);
	auto PointPositionC = this->GetPointPosition(PointIndexList[2]);
	auto Area = ComputeTriangleAreaIn3D(PointPositionA, PointPositionB, PointPositionC);
	this->Face(FaceIndex).Attribute().Area = Area;
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateCornerAngleOfFace(const MDK_Symbol_ALL&)
{
	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();
	for (int_max k = 0; k <= FaceIndex_max; ++k)
	{
		if (this->IsValidFaceIndex(k) == true)
		{
			this->UpdateCornerAngleOfFace(k);
		}
	}
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateCornerAngleOfFace(int_max FaceIndex)
{
    if (this->IsValidFaceIndex(FaceIndex) == false)
    {
        MDK_Warning("FaceIndex is invalid @ TriangleMesh::UpdateCornerAngleOfFace()")
        return;
    }

    auto PointIndexList = this->Face(FaceIndex).GetPointIndexList();
	auto PointPositionA = this->GetPointPosition(PointIndexList[0]);
	auto PointPositionB = this->GetPointPosition(PointIndexList[1]);
	auto PointPositionC = this->GetPointPosition(PointIndexList[2]);

    DenseVector<ScalarType, 3> CornerAngle;
    //----------------------------------------------
    auto Vector_AB = PointPositionB - PointPositionA;
    auto Vector_AC = PointPositionC - PointPositionA;
    CornerAngle[0] = ComputeAngleBetweenTwoVectorIn3D(Vector_AB, Vector_AC);
    //----------------------------------------------
    auto Vector_BA = PointPositionA - PointPositionB;
    auto Vector_BC = PointPositionC - PointPositionB;
    CornerAngle[1] = ComputeAngleBetweenTwoVectorIn3D(Vector_BA, Vector_BC);
    //---------------------------------    
    auto Vector_CA = PointPositionA - PointPositionC;
    auto Vector_CB = PointPositionB - PointPositionC;
    CornerAngle[2] = ComputeAngleBetweenTwoVectorIn3D(Vector_CA, Vector_CB);
    //---------------------------------

    this->Face(FaceIndex).Attribute().CornerAngle = CornerAngle;
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateAngleWeightedNormalAtPoint(const MDK_Symbol_ALL&, bool Flag_Update_RelativeInfo)
{//if Flag_Update_RelativeInfo is false, then FaceNormal must be available: call UpdateNormalAtFace() and UpdateCornerAngleOfFace()
	if (Flag_Update_RelativeInfo == true)
	{
		this->UpdateNormalAtFace(ALL);
		this->UpdateCornerAngleOfFace(ALL);
	}
	int_max PointIndex_max = this->GetMaxValueOfPointIndex();
	for (int_max k = 0; k <= PointIndex_max; ++k)
	{
		if (this->IsValidPointIndex(k) == true)
		{
			this->UpdateAngleWeightedNormalAtPoint(k);
		}
	}
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateAngleWeightedNormalAtPoint(int_max PointIndex)
{ // FaceNormal and FaceCornerAngle must be available: call UpdateNormalAtFace() and UpdateCornerAngleOfFace()

    if (this->IsValidPointIndex(PointIndex) == false)
    {
        MDK_Warning("PointIndex is invalid @ TriangleMesh::UpdateAngleWeightedNormalAtPoint(...)")
        return;
    }

    // compute angle weighted Normal

    auto AdjacentFaceIndexList = this->Point(PointIndex).GetAdjacentFaceIndexList();
    if (AdjacentFaceIndexList.IsEmpty() == true)
    {
		MDK_Warning("The point (PointIndex = " << PointIndex << ") has NO adjacent face @ TriangleMesh::UpdateAngleWeightedNormalAtPoint(...)")
        this->Point(PointIndex).Attribute().AngleWeightedNormal.Fill(0);
        return;
    }

	auto AdjacentFaceCount = AdjacentFaceIndexList.GetLength();
	if (AdjacentFaceCount > 1)
	{
		auto PointPosition = this->Point(PointIndex).GetPosition();

		DenseVector<ScalarType> CornerAngleList;
		CornerAngleList.Resize(AdjacentFaceCount);

		ObjectArray<DenseVector<ScalarType>> FaceNormalTable;
		FaceNormalTable.Resize(AdjacentFaceCount);

		for (int_max k = 0; k < AdjacentFaceCount; ++k)
		{
			auto PointRelativeIndex_k = this->Face(AdjacentFaceIndexList[k]).GetRelativeIndexOfPoint(PointIndex);
			CornerAngleList[k] = this->Face(AdjacentFaceIndexList[k]).Attribute().CornerAngle[PointRelativeIndex_k];
			FaceNormalTable[k] = this->Face(AdjacentFaceIndexList[k]).Attribute().Normal;
		}

		// calculate angle weighted normal at point
		auto AngleWeight = CornerAngleList;
		auto AngleSum = AngleWeight.Sum();
		if (AngleSum <= std::numeric_limits<ScalarType>::epsilon())
		{
			MDK_Warning("AngleSum <= eps @ TriangleMesh::UpdateAngleWeightedNormalAtPoint(...)")
			AngleWeight.Fill(ScalarType(1)/ScalarType(AdjacentFaceCount));
		}
		else
		{
			AngleWeight /= AngleSum;
		}

		DenseVector<ScalarType, 3> Normal;
		Normal.Fill(0);
		for (int_max k = 0; k < AdjacentFaceCount; ++k)
		{
			Normal[0] += AngleWeight[k] * FaceNormalTable[k][0];
			Normal[1] += AngleWeight[k] * FaceNormalTable[k][1];
			Normal[2] += AngleWeight[k] * FaceNormalTable[k][2];
		}		

		this->Point(PointIndex).Attribute().AngleWeightedNormal = Normal;
	}
	else// edge point
	{
		this->Point(PointIndex).Attribute().AngleWeightedNormal = this->Face(AdjacentFaceIndexList[0]).Attribute().Normal;
	}    
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateGaussianCurvatureAtPoint(const MDK_Symbol_ALL&, bool Flag_Update_RelativeInfo)
{//if Flag_Update_RelativeInfo is false, run UpdateCornerAngleOfFace and UpdateAreaOfFace first
	if (Flag_Update_RelativeInfo == true)
	{
		this->UpdateCornerAngleOfFace(ALL);
		this->UpdateAreaOfFace(ALL);
	}
	int_max PointIndex_max = this->GetMaxValueOfPointIndex();
	for (int_max k = 0; k <= PointIndex_max; ++k)
	{
		if (this->IsValidPointIndex(k) == true)
		{
			this->UpdateGaussianCurvatureAtPoint(k);
		}
	}
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateGaussianCurvatureAtPoint(int_max PointIndex)
{// run UpdateCornerAngleOfFace and UpdateAreaOfFace first
    if (this->IsValidPointIndex(PointIndex) == false)
    {
        MDK_Warning("PointIndex is invalid @ TriangleMesh::UpdateGaussianCurvatureAtPoint()")
        return;
    }

	//initialize
	this->Point(PointIndex).Attribute().GaussianCurvature = 0;
	this->Point(PointIndex).Attribute().WeightedGaussianCurvature = 0;

    auto AdjacentFaceIndexList = this->Point(PointIndex).GetAdjacentFaceIndexList();
    auto AdjacentFaceCount = AdjacentFaceIndexList.GetLength();
    if (AdjacentFaceCount <= 1)
    {
        //MDK_Warning("This point only has one or zero adjacent face @ TriangleMesh::UpdateGaussianCurvatureAtPoint()")
        return;
    }

	auto AdjacentPointCount = this->Point(PointIndex).GetAdjacentPointCount();

    DenseVector<ScalarType> AreaList;
	AreaList.FastResize(AdjacentPointCount);

    DenseVector<ScalarType> CornerAngleList;
	CornerAngleList.FastResize(AdjacentPointCount);

    for (int_max k = 0; k < AdjacentFaceCount; ++k)
    {
		auto PointRelativeIndex_k = this->Face(AdjacentFaceIndexList[k]).GetRelativeIndexOfPoint(PointIndex);
        CornerAngleList[k] = this->Face(AdjacentFaceIndexList[k]).Attribute().CornerAngle[PointRelativeIndex_k];
        AreaList[k] = this->Face(AdjacentFaceIndexList[k]).Attribute().Area;
    }

    if (AdjacentFaceCount != AdjacentPointCount)
    {// this point (PointIndex~b) is on boundary edge, an angle must be computed  (big angle a_b_c vs small angle a_b_c)
     //  a
	 //  |  /c
	 //  |/
     //  b
	 //-------------
        if (AdjacentFaceCount != AdjacentPointCount - 1)
        {
            MDK_Error("AdjacentFaceCount != AdjacentPointCount -1 @ TriangleMesh::UpdateGaussianCurvatureAtPoint()")
            return;
        }

        // check any edge boundary
        DenseVector<int_max> AdjacentBoundaryPointIndexList;// point a and c

        auto AdjacentEdgeIndexList = this->Point(PointIndex).GetAdjacentEdgeIndexList();
        for (int_max k = 0; k < AdjacentEdgeIndexList.GetLength(); ++k)
        {
            if (this->Edge(AdjacentEdgeIndexList[k]).IsBoundary() == true)
            {
                int_max tempPointIndex_0, tempPointIndex_1;
                this->Edge(AdjacentEdgeIndexList[k]).GetPointIndexList(tempPointIndex_0, tempPointIndex_1);
                if (tempPointIndex_0 == PointIndex)
                {
                    AdjacentBoundaryPointIndexList.Append(tempPointIndex_1);
                }
                else
                {
                    AdjacentBoundaryPointIndexList.Append(tempPointIndex_0);
                }
            }
        }

        if (AdjacentBoundaryPointIndexList.GetLength() != 2) // point is isolated or is a endpoint of a curve
        {
			MDK_Error("AdjacentBoundaryPointIndexList length is not 2 @ TriangleMesh::UpdateGaussianCurvatureAtPoint()")
            return;
        }

		auto PointPosition = this->GetPointPosition(PointIndex);
		auto Position_a = this->GetPointPosition(AdjacentBoundaryPointIndexList[0]);
		auto Position_b = this->GetPointPosition(AdjacentBoundaryPointIndexList[1]);
        auto Vector_a = Position_a - PointPosition;
        auto Vector_b = Position_b - PointPosition;
        CornerAngleList[AdjacentPointCount - 1] = ComputeAngleBetweenTwoVectorIn3D(Vector_a, Vector_b);
        AreaList[AdjacentPointCount - 1] = ComputeTriangleAreaIn3D(PointPosition, Position_a, Position_b);
    }

    // calculate Gaussian curvature
	auto constant_pi = ScalarType(std::acos(-1.0));
	auto GaussianCurvature = 2 * constant_pi - CornerAngleList.Sum();
	auto WeightedGaussianCurvature = GaussianCurvature / AreaList.Sum();
    //------------------------------
	this->Point(PointIndex).Attribute().GaussianCurvature = GaussianCurvature;
	this->Point(PointIndex).Attribute().WeightedGaussianCurvature = WeightedGaussianCurvature;
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateMeanCurvatureAtPoint(const MDK_Symbol_ALL&, bool Flag_Update_RelativeInfo)
{ //if Flag_Update_RelativeInfo is false, run UpdateAreaOfFace() first
	if (Flag_Update_RelativeInfo == true)
	{
		this->UpdateAreaOfFace(ALL);
	}
	int_max PointIndex_max = this->GetMaxValueOfPointIndex();
	for (int_max k = 0; k <= PointIndex_max; ++k)
	{
		if (this->IsValidPointIndex(k) == true)
		{
			this->UpdateMeanCurvatureAtPoint(k);
		}
	}
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateMeanCurvatureAtPoint(int_max PointIndex)
{ // run UpdateAreaOfFace() first

    if (this->IsValidPointIndex(PointIndex) == false)
    {
        MDK_Warning("PointIndex is invalid @ TriangleMesh::UpdateMeanCurvatureAtPoint()")
        return;
    }

    // cotangent Laplace Beltrami Operator "Polygon Mesh Processing: page 46"

    auto AdjacentPointCount = this->Point(PointIndex).GetAdjacentPointCount();
    if (AdjacentPointCount < 3)
    {
        this->Point(PointIndex).Attribute().MeanCurvatureNormal.Fill(0);
        this->Point(PointIndex).Attribute().MeanCurvature = 0;
        return;
    }

	auto PointPosition = this->GetPointPosition(PointIndex);

    DenseVector<ScalarType, 3> MeanCurvatureNormal;
    MeanCurvatureNormal.Fill(0);

    DenseVector<ScalarType> AreaList;
    AreaList.SetCapacity(AdjacentPointCount);

	DenseVector<ScalarType> CotSumList;
	CotSumList.SetCapacity(AdjacentPointCount);

    auto AdjacentEdgeIndexList = this->Point(PointIndex).GetAdjacentEdgeIndexList();
    for (int_max k = 0; k < AdjacentEdgeIndexList.GetLength(); ++k)
    {
        auto AdjacentFaceIndexList_k = this->Edge(AdjacentEdgeIndexList[k]).GetAdjacentFaceIndexList();
        if (AdjacentFaceIndexList_k.GetLength() == 2)
        {
            auto FaceIndex_a = AdjacentFaceIndexList_k[0];
            auto FaceIndex_b = AdjacentFaceIndexList_k[1];

            int_max tempPointIndex_0, tempPointIndex_1, PointIndex_k;
            this->Edge(AdjacentEdgeIndexList[k]).GetPointIndexList(tempPointIndex_0, tempPointIndex_1);
            if (tempPointIndex_0 == PointIndex)
            {
                PointIndex_k = tempPointIndex_1;
            }
            else
            {
                PointIndex_k = tempPointIndex_0;
            }

			auto Position_k = this->GetPointPosition(PointIndex_k);

            // get cotangent of the angle at PointIndex_a in FaceIndex_a
            int_max PointIndex_a;
            int_max PointRelativeIndex_a;
            auto PointIndexList_a = this->Face(FaceIndex_a).GetPointIndexList();
            if (PointIndexList_a[0] != PointIndex && PointIndexList_a[0] != PointIndex_k)
            {
                PointIndex_a = PointIndexList_a[0];
                PointRelativeIndex_a = 0;
            }
            else if (PointIndexList_a[1] != PointIndex && PointIndexList_a[1] != PointIndex_k)
            {
                PointIndex_a = PointIndexList_a[1];
                PointRelativeIndex_a = 1;
            }
            else
            {
                PointIndex_a = PointIndexList_a[2];
                PointRelativeIndex_a = 2;
            }

            auto CornerAngle_a = this->Face(FaceIndex_a).Attribute().CornerAngle[PointRelativeIndex_a];

			const auto pi = ScalarType(std::acos(-1.0));
			auto constant_pi_half = pi / ScalarType(2);
            auto Cot_a = std::tan(constant_pi_half - CornerAngle_a);

            // get cotangent of the angle at PointIndex_b in FaceIndex_b
            int_max PointIndex_b;
            int_max PointRelativeIndex_b;
            auto PointIndexList_b = this->Face(FaceIndex_b).GetPointIndexList();
            if (PointIndexList_b[0] != PointIndex && PointIndexList_b[0] != PointIndex_k)
            {
                PointIndex_b = PointIndexList_b[0];
                PointRelativeIndex_b = 0;
            }
            else if (PointIndexList_b[1] != PointIndex && PointIndexList_b[1] != PointIndex_k)
            {
                PointIndex_b = PointIndexList_b[1];
                PointRelativeIndex_b = 1;
            }
            else
            {
                PointIndex_b = PointIndexList_b[2];
                PointRelativeIndex_b = 2;
            }

            auto CornerAngle_b = this->Face(FaceIndex_b).Attribute().CornerAngle[PointRelativeIndex_b];
            auto Cot_b = std::tan(constant_pi_half - CornerAngle_b);

            auto SumOfCot = Cot_a + Cot_b;// read "Polygon Mesh Processing: page 46"
            if (SumOfCot > 0)
            {
                MeanCurvatureNormal += SumOfCot * (Position_k - PointPosition);
				CotSumList.Append(SumOfCot);
                AreaList.Append(this->Face(FaceIndex_a).Attribute().Area);
                AreaList.Append(this->Face(FaceIndex_b).Attribute().Area);
            }
        }
    }

    if (AreaList.GetLength() > 0)
    {
        // note: theoretically, mixed region should be used instead of the total region
		auto MeanCurvature = MeanCurvatureNormal.L2Norm() / AreaList.Sum();
		MeanCurvatureNormal /= CotSumList.Sum();
        //-------------------
        this->Point(PointIndex).Attribute().MeanCurvatureNormal = MeanCurvatureNormal;
        this->Point(PointIndex).Attribute().MeanCurvature = MeanCurvature;
    }
    else
    {
        this->Point(PointIndex).Attribute().MeanCurvatureNormal.Fill(0);
        this->Point(PointIndex).Attribute().MeanCurvature = 0;
    }
}

template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateNormalBasedCurvatureAtPoint(const MDK_Symbol_ALL&, bool Flag_Update_RelativeInfo)
{ //if Flag_Update_RelativeInfo is false, run UpdateNormalAtFace first
	if (Flag_Update_RelativeInfo == true)
	{
		this->UpdateNormalAtFace(ALL);
	}
	int_max PointIndex_max = this->GetMaxValueOfPointIndex();
	for (int_max k = 0; k <= PointIndex_max; ++k)
	{
		if (this->IsValidPointIndex(k) == true)
		{
			this->UpdateNormalBasedCurvatureAtPoint(k);
		}
	}
}


template<typename ScalarType>
void TriangleMesh<ScalarType>::UpdateNormalBasedCurvatureAtPoint(int_max PointIndex)
{ // run UpdateNormalAtFace first

	if (this->IsValidPointIndex(PointIndex) == false)
	{
		MDK_Warning("PointIndex is invalid @ TriangleMesh::UpdateNormalBasedCurvatureAtPoint()")
		return;
	}

	// measure the diviation of adjacent face normal 
	// if all the adjacent face normal  point to the same direction, then curvature is zero
	// NormalBasedCurvature is from 0 to 1
	// 0: __ __ two adjacent face in a flat plane 
	// 0.5: |__   two adjacent face 90' angle
	// 1:   || two adjacent face fold to be parallel to each other
	//
	// Attention:
	// if FaceA and FaceB is in the same plane and not fold, then Normal_of_FaceA and Normal_of_FaceB must be the same

	auto AdjacentFaceIndexList = this->Point(PointIndex).GetAdjacentFaceIndexList();
	auto AdjacentFaceCount = AdjacentFaceIndexList.GetLength();
	if (AdjacentFaceCount <= 1)
	{
		this->Point(PointIndex).Attribute().NormalBasedCurvature = 0;
		return;
	}

	ObjectArray<DenseVector<ScalarType, 3>> NormalList;
	NormalList.Resize(AdjacentFaceCount);
	for (int_max k = 0; k < AdjacentFaceCount; ++k)
	{
		NormalList[k] = this->Face(AdjacentFaceIndexList(k)).Attribute().Normal;
	}

	ScalarType MinProjection = 1;
	for (int_max k = 0; k < AdjacentFaceCount-1; ++k)
	{
		auto Normal_k = NormalList[k];
		for (int_max n = k+1; n < AdjacentFaceCount; ++n)
		{
			auto Normal_n = NormalList[n];
			auto projection = Normal_k[0] * Normal_n[0] + Normal_k[1] * Normal_n[1] + Normal_k[2] * Normal_n[2];
			if (MinProjection > projection)
			{
				MinProjection = projection;
			}
		}
	}
	
	this->Point(PointIndex).Attribute().NormalBasedCurvature = (ScalarType(1) - MinProjection)/ ScalarType(2);
}

}// namespace mdk
