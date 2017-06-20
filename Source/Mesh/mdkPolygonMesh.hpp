﻿#pragma once

namespace mdk
{

template<typename ScalarType>
PolygonMesh<ScalarType>::PolygonMesh()
{
	this->Recreate();
}


template<typename ScalarType>
PolygonMesh<ScalarType>::PolygonMesh(const MDK_Symbol_PureEmpty&)
{
}


template<typename ScalarType>
PolygonMesh<ScalarType>::PolygonMesh(const PolygonMesh<ScalarType>& InputMesh)
{
    this->Copy(InputMesh);
}


template<typename ScalarType>
PolygonMesh<ScalarType>::PolygonMesh(PolygonMesh<ScalarType>&& InputMesh)
{
	m_MeshData = std::move(InputMesh.m_MeshData);
}


template<typename ScalarType>
PolygonMesh<ScalarType>::~PolygonMesh()
{
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::operator=(const PolygonMesh<ScalarType>& InputMesh)
{
    this->Copy(InputMesh);
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::operator=(PolygonMesh<ScalarType>&& InputMesh)
{
	this->Copy(std::move(InputMesh));
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::Clear()
{
    if (!m_MeshData)
    {
        return;
    }

	m_MeshData->ID = -1;
	m_MeshData->Name = "";

    m_MeshData->PointPositionTable.Clear();
    m_MeshData->PointList.Clear();
    m_MeshData->PointValidityFlagList.Clear();

    m_MeshData->EdgeList.Clear();
    m_MeshData->EdgeValidityFlagList.Clear();

    m_MeshData->FaceList.Clear();
    m_MeshData->FaceValidityFlagList.Clear();

    m_MeshData->Map_Point_ID_to_Index.clear();
    m_MeshData->Map_Edge_ID_to_Index.clear();
    m_MeshData->Map_Face_ID_to_Index.clear();
	
	m_MeshData->Map_Point_Name_to_Index.clear();
	m_MeshData->Map_Edge_Name_to_Index.clear();
	m_MeshData->Map_Face_Name_to_Index.clear();

	m_MeshData->PointSetList.Clear();
	m_MeshData->Map_PointSet_Name_to_Index.clear();

	m_MeshData->EdgeSetList.Clear();
	m_MeshData->Map_EdgeSet_Name_to_Index.clear();

	m_MeshData->FaceSetList.Clear();
	m_MeshData->Map_FaceSet_Name_to_Index.clear();

	m_MeshData->PointDataSet.Clear();
	m_MeshData->Map_PointDataSet_Name_to_Index.clear();
	
	m_MeshData->EdgeDataSet.Clear();
	m_MeshData->Map_EdgeDataSet_Name_to_Index.clear();
	
	m_MeshData->FaceDataSet.Clear();
	m_MeshData->Map_FaceDataSet_Name_to_Index.clear();

    m_MeshData->Attribute.Clear();
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::Clear(const MDK_Symbol_PureEmpty&)
{
	if (m_MeshData)
	{
		m_MeshData.reset();
	}
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::Copy(const PolygonMesh<ScalarType>& InputMesh)
{
	//prevent self copy
	if (&m_MeshData == &InputMesh.m_MeshData)
	{
		return;
	}

	if (this->IsPureEmpty() == true)
	{
		this->Recreate();
	}

	if (InputMesh.IsEmpty() == true)
    {
		this->Clear();
        return;
    }

	m_MeshData->ID = InputMesh.m_MeshData->ID;
	m_MeshData->Name = InputMesh.m_MeshData->Name;

    m_MeshData->PointPositionTable = InputMesh.m_MeshData->PointPositionTable;
    m_MeshData->PointValidityFlagList = InputMesh.m_MeshData->PointValidityFlagList;
    m_MeshData->PointList = InputMesh.m_MeshData->PointList;
    for (int_max k = 0; k < m_MeshData->PointList.GetLength(); ++k)
    {
        if (m_MeshData->PointValidityFlagList[k] == 1)
        {
            m_MeshData->PointList[k].SetParentMesh(*this);
        }
    }

    m_MeshData->EdgeValidityFlagList = InputMesh.m_MeshData->EdgeValidityFlagList;
    m_MeshData->EdgeList = InputMesh.m_MeshData->EdgeList;
    for (int_max k = 0; k < m_MeshData->EdgeList.GetLength(); ++k)
    {
        if (m_MeshData->EdgeValidityFlagList[k] == 1)
        {
            m_MeshData->EdgeList[k].SetParentMesh(*this);
        }
    }

    m_MeshData->FaceValidityFlagList = InputMesh.m_MeshData->FaceValidityFlagList;
    m_MeshData->FaceList = InputMesh.m_MeshData->FaceList;
    for (int_max k = 0; k < m_MeshData->FaceList.GetLength(); ++k)
    {
        if (m_MeshData->FaceValidityFlagList[k] == 1)
        {
            m_MeshData->FaceList[k].SetParentMesh(*this);
        }
    }

    m_MeshData->Map_Point_ID_to_Index = InputMesh.m_MeshData->Map_Point_ID_to_Index;
    m_MeshData->Map_Edge_ID_to_Index = InputMesh.m_MeshData->Map_Edge_ID_to_Index;
    m_MeshData->Map_Face_ID_to_Index = InputMesh.m_MeshData->Map_Face_ID_to_Index;

	m_MeshData->Map_Point_Name_to_Index = InputMesh.m_MeshData->Map_Point_Name_to_Index;
	m_MeshData->Map_Edge_Name_to_Index = InputMesh.m_MeshData->Map_Edge_Name_to_Index;
	m_MeshData->Map_Face_Name_to_Index = InputMesh.m_MeshData->Map_Face_Name_to_Index;

	m_MeshData->PointSetList = InputMesh.m_MeshData->PointSetList;
	m_MeshData->Map_PointSet_Name_to_Index = InputMesh.m_MeshData->Map_PointSet_Name_to_Index;

	m_MeshData->EdgeSetList = InputMesh.m_MeshData->EdgeSetList;
	m_MeshData->Map_EdgeSet_Name_to_Index = InputMesh.m_MeshData->Map_EdgeSet_Name_to_Index;

	m_MeshData->FaceSetList = InputMesh.m_MeshData->FaceSetList;
	m_MeshData->Map_FaceSet_Name_to_Index = InputMesh.m_MeshData->Map_FaceSet_Name_to_Index;

	m_MeshData->PointDataSet = InputMesh.m_MeshData->PointDataSet;
	m_MeshData->Map_PointDataSet_Name_to_Index = InputMesh.m_MeshData->Map_PointDataSet_Name_to_Index;

	m_MeshData->EdgeDataSet = InputMesh.m_MeshData->EdgeDataSet;
	m_MeshData->Map_EdgeDataSet_Name_to_Index = InputMesh.m_MeshData->Map_EdgeDataSet_Name_to_Index;

	m_MeshData->FaceDataSet = InputMesh.m_MeshData->FaceDataSet;
	m_MeshData->Map_FaceDataSet_Name_to_Index = InputMesh.m_MeshData->Map_FaceDataSet_Name_to_Index;

    m_MeshData->Attribute = InputMesh.m_MeshData->Attribute;
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::Copy(PolygonMesh<ScalarType>&& InputMesh)
{
	//prevent self copy
	if (&m_MeshData == &InputMesh.m_MeshData)
	{
		return;
	}

	if (this->IsPureEmpty() == true)
	{
		this->Recreate();
	}

	if (InputMesh.IsEmpty() == true)
	{
		this->Clear();
		return;
	}

	m_MeshData->ID = InputMesh.m_MeshData->ID;
	m_MeshData->Name = std::move(InputMesh.m_MeshData->Name);

	m_MeshData->PointPositionTable = std::move(InputMesh.m_MeshData->PointPositionTable);
	m_MeshData->PointValidityFlagList = std::move(InputMesh.m_MeshData->PointValidityFlagList);
	m_MeshData->PointList = std::move(InputMesh.m_MeshData->PointList);
	for (int_max k = 0; k < m_MeshData->PointList.GetLength(); ++k)
	{
		if (m_MeshData->PointValidityFlagList[k] == 1)
		{
			m_MeshData->PointList[k].SetParentMesh(*this);
		}
	}

	m_MeshData->EdgeValidityFlagList = std::move(InputMesh.m_MeshData->EdgeValidityFlagList);
	m_MeshData->EdgeList = std::move(InputMesh.m_MeshData->EdgeList);
	for (int_max k = 0; k < m_MeshData->EdgeList.GetLength(); ++k)
	{
		if (m_MeshData->EdgeValidityFlagList[k] == 1)
		{
			m_MeshData->EdgeList[k].SetParentMesh(*this);
		}
	}

	m_MeshData->FaceValidityFlagList = std::move(InputMesh.m_MeshData->FaceValidityFlagList);
	m_MeshData->FaceList = std::move(InputMesh.m_MeshData->FaceList);
	for (int_max k = 0; k < m_MeshData->FaceList.GetLength(); ++k)
	{
		if (m_MeshData->FaceValidityFlagList[k] == 1)
		{
			m_MeshData->FaceList[k].SetParentMesh(*this);
		}
	}

	m_MeshData->Map_Point_ID_to_Index = std::move(InputMesh.m_MeshData->Map_Point_ID_to_Index);
	m_MeshData->Map_Edge_ID_to_Index = std::move(InputMesh.m_MeshData->Map_Edge_ID_to_Index);
	m_MeshData->Map_Face_ID_to_Index = std::move(InputMesh.m_MeshData->Map_Face_ID_to_Index);

	m_MeshData->Map_Point_Name_to_Index = std::move(InputMesh.m_MeshData->Map_Point_Name_to_Index);
	m_MeshData->Map_Edge_Name_to_Index = std::move(InputMesh.m_MeshData->Map_Edge_Name_to_Index);
	m_MeshData->Map_Face_Name_to_Index = std::move(InputMesh.m_MeshData->Map_Face_Name_to_Index);

	m_MeshData->PointSetList = std::move(InputMesh.m_MeshData->PointSetList);
	m_MeshData->Map_PointSet_Name_to_Index = std::move(InputMesh.m_MeshData->Map_PointSet_Name_to_Index);

	m_MeshData->EdgeSetList = std::move(InputMesh.m_MeshData->EdgeSetList);
	m_MeshData->Map_EdgeSet_Name_to_Index = std::move(InputMesh.m_MeshData->Map_EdgeSet_Name_to_Index);

	m_MeshData->FaceSetList = std::move(InputMesh.m_MeshData->FaceSetList);
	m_MeshData->Map_FaceSet_Name_to_Index = std::move(InputMesh.m_MeshData->Map_FaceSet_Name_to_Index);

	m_MeshData->PointDataSet = std::move(InputMesh.m_MeshData->PointDataSet);
	m_MeshData->Map_PointDataSet_Name_to_Index = std::move(InputMesh.m_MeshData->Map_PointDataSet_Name_to_Index);

	m_MeshData->EdgeDataSet = std::move(InputMesh.m_MeshData->EdgeDataSet);
	m_MeshData->Map_EdgeDataSet_Name_to_Index = std::move(InputMesh.m_MeshData->Map_EdgeDataSet_Name_to_Index);

	m_MeshData->FaceDataSet = std::move(InputMesh.m_MeshData->FaceDataSet);
	m_MeshData->Map_FaceDataSet_Name_to_Index = std::move(InputMesh.m_MeshData->Map_FaceDataSet_Name_to_Index);

	m_MeshData->Attribute = std::move(InputMesh.m_MeshData->Attribute);
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::Share(PolygonMesh& InputMesh)
{
	m_MeshData = InputMesh.m_MeshData;
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::ForceShare(const PolygonMesh<ScalarType>& InputMesh)
{
	m_MeshData = InputMesh.m_MeshData;
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::Recreate()
{
	m_MeshData = std::make_shared<PolygonMeshData<ScalarType>>();
}

//-------------------------------------------------------------------

template<typename ScalarType>
inline 
bool PolygonMesh<ScalarType>::IsEmpty() const
{
	if (!m_MeshData)
	{
		return true;
	}
	else
	{
		return m_MeshData->PointPositionTable.IsEmpty();
	}
}

template<typename ScalarType>
inline
bool PolygonMesh<ScalarType>::IsPureEmpty() const
{
	return (!m_MeshData);
}

template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetPointCount() const
{
	if (!m_MeshData)
	{
		return 0;
	}
	return m_MeshData->PointValidityFlagList.Sum();
}

template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetEdgeCount() const
{
	if (!m_MeshData)
	{
		return 0;
	}
	return m_MeshData->EdgeValidityFlagList.Sum();	
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetFaceCount() const
{
	if (!m_MeshData)
	{
		return 0;
	}
	return m_MeshData->FaceValidityFlagList.Sum();
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetNamedPointCount() const
{
	if (!m_MeshData)
	{
		return 0;
	}
	return m_MeshData->Map_Point_Name_to_Index.size();
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetNamedFaceCount() const
{
	if (!m_MeshData)
	{
		return 0;
	}
	return m_MeshData->Map_Face_Name_to_Index.size();
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetMaxValueOfPointIndex() const
{
	if (!m_MeshData)
	{
		return -1;
	}
	return m_MeshData->PointPositionTable.GetColCount()-1;	
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetMaxValueOfEdgeIndex() const
{
	if (!m_MeshData)
	{
		return -1;
	}
	return m_MeshData->EdgeList.GetLength()-1;
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetMaxValueOfFaceIndex() const
{
	if (!m_MeshData)
	{
		return -1;
	}
	return m_MeshData->FaceList.GetLength()-1;	
}

//------ Get/Set GlobalAttribute -----------------------------------//

template<typename ScalarType>
inline
GlobalStandardAttribute_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Attribute()
{
    return m_MeshData->Attribute;
}

template<typename ScalarType>
inline
const GlobalStandardAttribute_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Attribute() const
{
    return m_MeshData->Attribute;
}

//--------------------------------------------------------------//

template<typename ScalarType>
void PolygonMesh<ScalarType>::SetID(int_max ID)
{
	m_MeshData->ID = ID;
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetID() const
{
	return m_MeshData->ID;
}

template<typename ScalarType>
void PolygonMesh<ScalarType>::SetName(String Name)
{
	m_MeshData->Name = std::move(Name);
}

template<typename ScalarType>
String PolygonMesh<ScalarType>::GetName() const
{
	return m_MeshData->Name;
}

//------------- Get/Set All the position (valid point) --------------------------------//

template<typename ScalarType>
inline 
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetPointPosition(const MDK_Symbol_ALL&) const
{
	DenseMatrix<ScalarType> PointPositionMatrix;
	this->GetPointPosition(ALL, PointPositionMatrix);
	return PointPositionMatrix;
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::GetPointPosition(const MDK_Symbol_ALL&, DenseMatrix<ScalarType>& PositionMatrix) const
{
	if (this->IsEmpty() == true)
	{
		PositionMatrix.Clear();
		return;
	}

	auto MaxCount = m_MeshData->PointPositionTable.GetColCount();
	PositionMatrix.FastResize(3, this->GetPointCount());
	int_max Counter = 0;
	for (int_max k = 0; k < MaxCount; ++k)
	{
		if (m_MeshData->PointValidityFlagList[k] == 1)
		{
			PositionMatrix.SetCol(Counter, m_MeshData->PointPositionTable.GetPointerOfCol(k));
			Counter += 1;
		}
	}
}

template<typename ScalarType>
inline void PolygonMesh<ScalarType>::SetPointPosition(const MDK_Symbol_ALL&, const DenseMatrix<ScalarType>& PositionMatrix) const
{
	auto PointCount = this->GetPointCount();
	if (PositionMatrix.GetColCount() != PointCount)
	{
		MDK_Error(" input size is wrong @ PolygonMesh<ScalarType>::SetPointPosition(ALL, ...)")
	}

	int_max ColIndex = -1;
	for (int_max k = 0; k < m_MeshData->PointPositionTable.GetColCount(); ++k)
	{
		if (m_MeshData->PointValidityFlagList[k] == 1)
		{
			ColIndex += 1;
			m_MeshData->PointPositionTable.SetCol(k, PositionMatrix.GetPointerOfCol(ColIndex));
		}
	}
}


//---- Get/Set 3D Position by PointIndex --------------------------------------------------------------------------//

template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::
SetPointPosition(int_max PointIndex, ScalarType x, ScalarType y, ScalarType z)
{
    m_MeshData->PointPositionTable.SetCol(PointIndex, { x, y, z });
}

template<typename ScalarType>
inline 
void PolygonMesh<ScalarType>::SetPointPosition(int_max PointIndex, const ScalarType Position[3])
{
    m_MeshData->PointPositionTable.SetCol(PointIndex, Position);
}

template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::SetPointPosition(int_max PointIndex, const DenseVector<ScalarType,3>& Position)
{
	m_MeshData->PointPositionTable.SetCol(PointIndex, Position);
}

template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::
SetPointPosition(const DenseVector<int_max>& PointIndexList, const DenseMatrix<ScalarType>& PointPositionMatrix)
{
	if (PointIndexList.IsEmpty() == true && PointPositionMatrix.IsEmpty() == true)
	{
		return;
	}
	else if (PointIndexList.IsEmpty() == true && PointPositionMatrix.IsEmpty() == false)
	{
		MDK_Error("Invalid input @ PolygonMesh::SetPointPosition(...)")
		return;
	}
	else if (PointIndexList.IsEmpty() == false && PointPositionMatrix.IsEmpty() == true)
	{
		MDK_Error("Invalid input @ PolygonMesh::SetPointPosition(...)")
		return;
	}

	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		auto PointIndex = PointIndexList[k];
		if (PointIndex < 0 || PointIndex >= m_MeshData->PointPositionTable.GetColCount())
		{
			MDK_Error("Invalid PointIndex @ PolygonMesh::SetPointPosition(...)")
			return;
		}

		m_MeshData->PointPositionTable.SetCol(PointIndex, PointPositionMatrix.GetPointerOfCol(k));
	}
}

template<typename ScalarType>
inline
DenseVector<ScalarType, 3> PolygonMesh<ScalarType>::GetPointPosition(int_max PointIndex) const
{
    DenseVector<ScalarType, 3> Position;
    m_MeshData->PointPositionTable.GetCol(PointIndex, Position.GetPointer());
    return Position;
}

template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::GetPointPosition(int_max PointIndex, DenseVector<ScalarType, 3>& Position) const
{
	m_MeshData->PointPositionTable.GetCol(PointIndex, Position.GetPointer());
}

template<typename ScalarType>
inline 
void PolygonMesh<ScalarType>::
GetPointPosition(int_max PointIndex, ScalarType& x, ScalarType& y, ScalarType& z) const
{
    ScalarType Position[3];
    m_MeshData->PointPositionTable.GetCol(PointIndex, Position);
    x = Position[0];
    y = Position[1];
    z = Position[2];
}

template<typename ScalarType>
inline 
void PolygonMesh<ScalarType>::GetPointPosition(int_max PointIndex, ScalarType Position[3]) const
{
    m_MeshData->PointPositionTable.GetCol(PointIndex, Position);
}


template<typename ScalarType>
inline
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetPointPosition(const DenseVector<int_max>& PointIndexList) const
{
    DenseMatrix<ScalarType> PointPositionMatrix;
    this->GetPointPosition(PointIndexList, PointPositionMatrix);
    return PointPositionMatrix;
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::GetPointPosition(const DenseVector<int_max>& PointIndexList, DenseMatrix<ScalarType>& PointPositionMatrix) const
{
    if (PointIndexList.IsEmpty() == true)
    {
        PointPositionMatrix.FastResize(0, 0);
        return;
    }

    PointPositionMatrix.FastResize(3, PointIndexList.GetLength());
    for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
    {
        auto PointIndex = PointIndexList[k];
        if (PointIndex < 0 || PointIndex >= m_MeshData->PointPositionTable.GetColCount())
        {
            MDK_Error("Invalid PointIndex @ PolygonMesh::GetPointPosition(...)")
            PointPositionMatrix.FastResize(0, 0);
            return;
        }
        PointPositionMatrix.SetCol(k, m_MeshData->PointPositionTable.GetPointerOfCol(PointIndex));
    }
}

//------------------------- Mesh Element ----------------------------------------------

template<typename ScalarType>
inline
Point_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Point(int_max PointIndex)
{
    return m_MeshData->PointList[PointIndex];
}

template<typename ScalarType>
inline
const Point_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Point(int_max PointIndex) const
{
    return m_MeshData->PointList[PointIndex];
}

template<typename ScalarType>
inline
Edge_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Edge(int_max EdgeIndex)
{
    return m_MeshData->EdgeList[EdgeIndex];
}

template<typename ScalarType>
inline
const Edge_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Edge(int_max EdgeIndex) const
{
    return m_MeshData->EdgeList[EdgeIndex];
}


template<typename ScalarType>
inline
Face_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Face(int_max FaceIndex)
{
    return m_MeshData->FaceList[FaceIndex];
}

template<typename ScalarType>
inline
const Face_Of_PolygonMesh<ScalarType>& PolygonMesh<ScalarType>::Face(int_max FaceIndex) const
{
    return m_MeshData->FaceList[FaceIndex];
}

//-------------- check index -------------------------------------------------------//

template<typename ScalarType>
bool PolygonMesh<ScalarType>::IsValidPointIndex(int_max PointIndex) const
{
    if (PointIndex < 0 || PointIndex >= m_MeshData->PointValidityFlagList.GetLength())
    {
        return false;
    }

	return (m_MeshData->PointValidityFlagList[PointIndex] == 1);    
    
    // slow
    //if (PointIndex < 0 || PointIndex >= m_MeshData->PointList.GetLength())
    //{
    //    return false;
    //}
    //else
    //{
    //    return m_MeshData->PointList[PointIndex].IsValid();
    //}    
}

template<typename ScalarType>
bool PolygonMesh<ScalarType>::IsValidEdgeIndex(int_max EdgeIndex) const
{
    if (EdgeIndex < 0 || EdgeIndex >= m_MeshData->EdgeValidityFlagList.GetLength())
    {
        return false;
    }
	return (m_MeshData->EdgeValidityFlagList[EdgeIndex] == 1);    
}


template<typename ScalarType>
bool PolygonMesh<ScalarType>::IsValidFaceIndex(int_max FaceIndex) const
{
    if (FaceIndex < 0 || FaceIndex >= m_MeshData->FaceValidityFlagList.GetLength())
    {
        return false;
    }    
	return (m_MeshData->FaceValidityFlagList[FaceIndex] == 1);  
}

//--------- get Valid IndexList ------------------------------------------------------------//

template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetPointIndexList() const
{
	DenseVector<int_max> OutputIndexList;
	OutputIndexList.SetCapacity(m_MeshData->PointList.GetLength());	
    for (int_max k = 0; k < m_MeshData->PointList.GetLength(); ++k)
    {
        if (m_MeshData->PointValidityFlagList[k] == 1)
        {
			OutputIndexList.Append(k);
        }
    }
	return OutputIndexList;
}

template<typename ScalarType>
inline 
DenseVector<int_max> PolygonMesh<ScalarType>::GetEdgeIndexList() const
{
	DenseVector<int_max> OutputIndexList;
	OutputIndexList.SetCapacity(m_MeshData->EdgeList.GetLength());	
    for (int_max k = 0; k < m_MeshData->EdgeList.GetLength(); ++k)
    {
        if (m_MeshData->EdgeValidityFlagList[k] == 1)
        {
			OutputIndexList.Append(k);
        }
    }
	return OutputIndexList;
}


template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetFaceIndexList() const
{
	DenseVector<int_max> OutputIndexList;
	OutputIndexList.SetCapacity(m_MeshData->FaceList.GetLength());
    for (int_max k = 0; k < m_MeshData->FaceList.GetLength(); ++k)
    {
        if (m_MeshData->FaceValidityFlagList[k] == 1)
        {
			OutputIndexList.Append(k);
        }
    }
	return OutputIndexList;
}

//----------- get PointIndex by Position, ID, Name ----------------------------------------------//

template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetPointIndexByPosition(const DenseVector<ScalarType, 3>& Position, ScalarType DistanceThreshold) const
{
    return this->GetPointIndexByPosition(Position[0], Position[1], Position[2], DistanceThreshold);
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetPointIndexByPosition(const ScalarType Position[3], ScalarType DistanceThreshold) const
{
    return this->GetPointIndexByPosition(Position[0], Position[1], Position[2], DistanceThreshold);
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetPointIndexByPosition(ScalarType x, ScalarType y, ScalarType z, ScalarType DistanceThreshold) const
{
    if (this->IsEmpty() == true)
    {        
        return -1;
    }

    if (DistanceThreshold < 0)
    {
        MDK_Error("DistanceThreshold < 0 @ PolygonMesh::GetPointIndexByPosition(...)")
        return -1;
    }

    auto eps_value = std::numeric_limits<ScalarType>::epsilon();

    ScalarType Threshold_sq = DistanceThreshold*DistanceThreshold;
    ScalarType LowerLimit_sq = std::min(eps_value, Threshold_sq);

    ScalarType Distance_sq_min = 0;
    int_max PointIndex_min = -1;

    for (int_max k = 0; k < m_MeshData->PointPositionTable.GetColCount(); ++k)
    {
        if (m_MeshData->PointValidityFlagList[k] == 1)
        {
            ScalarType Pos[3];
            m_MeshData->PointPositionTable.GetCol(k, Pos);
            auto Distance_sq = (Pos[0] - x)*(Pos[0] - x) + (Pos[1] - y)*(Pos[1] - y) + (Pos[2] - z)*(Pos[2] - z);
            if (Distance_sq <= LowerLimit_sq)
            {
                return k;
            }

            if (PointIndex_min == -1) // set initial value
            {
                Distance_sq_min = Distance_sq;
                PointIndex_min = k;
            }

            if (Distance_sq < Distance_sq_min)
            {
                Distance_sq_min = Distance_sq;
                PointIndex_min = k;
            }                                
        }
    }

    if (Distance_sq_min <= Threshold_sq)
    {
		return PointIndex_min;
    }
	else
	{
		return -1;
	}
}


template<typename ScalarType>
inline int_max PolygonMesh<ScalarType>::GetPointIndexByID(int_max PointID) const
{
    if (PointID < 0)
    {
        //MDK_Warning("Invalid PointID (< 0) @ PolygonMesh::GetPointIndexByID(...)")
        return -1;
    }

    auto it = m_MeshData->Map_Point_ID_to_Index.find(PointID);
    if (it != m_MeshData->Map_Point_ID_to_Index.end())
    {
		return it->second;
    }
    else
    {
        //MDK_Warning("Invalid PointID @ PolygonMesh::GetPointIndexByID(...)")
		return -1;
    }
}

template<typename ScalarType>
inline int_max PolygonMesh<ScalarType>::GetPointIndexByName(const String& PointName) const
{
	if (PointName.IsEmpty() == true)
	{
		//MDK_Warning("PointName is empty @ PolygonMesh::GetPointIndexByID(...)")
		return -1;
	}

	auto it = m_MeshData->Map_Point_Name_to_Index.find(PointName);
	if (it != m_MeshData->Map_Point_Name_to_Index.end())
	{
		return it->second;
	}
	else
	{
		//MDK_Warning("Invalid PointName @ PolygonMesh::GetPointIndexByID(...)")
		return -1;
	}
}

//----------- get EdgeIndex by Point, ID, Name -------------------------------------//
template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetEdgeIndexByPoint(int_max PointIndex0, int_max PointIndex1) const
{
	if (this->IsValidPointIndex(PointIndex0) == false || this->IsValidPointIndex(PointIndex1) == false)
	{
		//MDK_Warning("PointIndex0 or PointIndex1 is invalid @ PolygonMesh::GetEdgeIndexByPoint(...)")		
		return -1;
	}

	if (m_MeshData->PointList[PointIndex0].IsOnEdge() == true && m_MeshData->PointList[PointIndex1].IsOnEdge() == true)
	{
		const auto& AdjacentEdgeIndexList0 = m_MeshData->PointList[PointIndex0].AdjacentEdgeIndexList();
		const auto& AdjacentEdgeIndexList1 = m_MeshData->PointList[PointIndex1].AdjacentEdgeIndexList();
		auto EdgeIndexList = Intersect(AdjacentEdgeIndexList0, AdjacentEdgeIndexList1);
		if (EdgeIndexList.IsEmpty() == false)
		{
			if (EdgeIndexList.GetLength() > 1)
			{
				MDK_Warning("EdgeIndexList.GetLength() > 1 @ PolygonMesh::GetEdgeIndexByPoint(...)")
			}
			return EdgeIndexList[0];
		}
	}

	//MDK_Warning("No Edge between PointIndex0 and PointIndex1 @ PolygonMesh::GetEdgeIndexByPoint(...)")
	return -1;
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetEdgeIndexByID(int_max EdgeID) const
{
    if (EdgeID < 0)
    {
        //MDK_Warning("Invalid EdgeID (< 0) @ PolygonMesh::GetEdgeIndexByID(...)")        
        return -1;
    }

    auto it = m_MeshData->Map_Edge_ID_to_Index.find(EdgeID);
    if (it != m_MeshData->Map_Edge_ID_to_Index.end())
    {
        return it->second;
    }
    else
    {
        //MDK_Warning("Invalid EdgeID @ PolygonMesh::GetEdgeIndexByID(...)")
		return -1;
    }
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetEdgeIndexByName(const String& EdgeName) const
{
	if (EdgeName.IsEmpty() == true)
	{
		//MDK_Warning("EdgeName is empty @ PolygonMesh::GetEdgeIndexByName(...)")        
		return -1;
	}

	auto it = m_MeshData->Map_Edge_Name_to_Index.find(EdgeName);
	if (it != m_MeshData->Map_Edge_Name_to_Index.end())
	{
		return it->second;
	}
	else
	{
		//MDK_Warning("Invalid EdgeName @ PolygonMesh::GetEdgeIndexByName(...)")
		return -1;
	}
}

//----------- get FaceIndex by Point, Edge, ID, Name ----------//
template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetFaceIndexByPoint(const DenseVector<int_max>& PointIndexList) const
{
	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		if (this->IsValidPointIndex(PointIndexList[k]) == false)
		{
			//MDK_Warning("PointIndexList is invalid @ PolygonMesh::GetFaceIndexByPoint(...)")
			return -1;
		}
	}

	DenseVector<int_max> EdgeIndexList;
	EdgeIndexList.Resize(PointIndexList.GetLength());
	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		if (k < PointIndexList.GetLength() - 1)
		{
			EdgeIndexList[k] = this->GetEdgeIndexByPoint(PointIndexList[k], PointIndexList[k + 1]);
		}
		else
		{
			EdgeIndexList[k] = this->GetEdgeIndexByPoint(PointIndexList[k], PointIndexList[0]);
		}
		if (EdgeIndexList[k].GetIndex() < 0)
		{
			//MDK_Warning("PointIndexList is invalid @ PolygonMesh::GetFaceIndexByPoint(...)")
			return -1;
		}
	}
	return this->GetFaceIndexByEdge(EdgeIndexList);
}


template<typename ScalarType>
inline 
int_max PolygonMesh<ScalarType>::GetFaceIndexByEdge(const DenseVector<int_max>& EdgeIndexList) const
{
    for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
    {
        if (this->IsValidEdgeIndex(EdgeIndexList[k]) == false)
        {
            //MDK_Warning("EdgeIndexList is invalid @ PolygonMesh::GetFaceIndexByEdge(...)")            
            return -1;
        }
    }

	//---------------------------------------------------------------------------------------------------
	auto FaceIndexList = m_MeshData->EdgeList[EdgeIndexList[0]].GetAdjacentFaceIndexList();
	for (int_max k = 1; k < EdgeIndexList.GetLength(); ++k)
	{
		auto AdjacentFaceIndexList_k = m_MeshData->EdgeList[EdgeIndexList[k]].GetAdjacentFaceIndexList();
		FaceIndexList = Intersect(FaceIndexList, AdjacentFaceIndexList_k);
	}
	//----------------------
	if (FaceIndexList.GetLength() != 1)
	{
		//MDK_Warning("EdgeIndexList is invalid @ PolygonMesh::GetFaceIndexByEdge(...)")
		return -1;
	}

	return FaceIndexList[0];
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetFaceIndexByID(int_max FaceID) const
{
	if (FaceID < 0)
	{
		//MDK_Warning("Invalid FaceID (< 0) @ PolygonMesh::GetFaceIndexByID(...)")
		return -1;
	}

	auto it = m_MeshData->Map_Face_ID_to_Index.find(FaceID);
	if (it != m_MeshData->Map_Face_ID_to_Index.end())
	{
		return it->second;
	}
	else
	{
		//MDK_Warning("Invalid FaceID @ PolygonMesh::GetFaceIndexByID(...)")
		return -1;
	}
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetFaceIndexByName(const String& FaceName) const
{
	if (FaceName.IsEmpty() == true)
	{
		//MDK_Warning("FaceName is empty @ PolygonMesh::GetFaceIndexByName(...)")
		return -1;
	}

	auto it = m_MeshData->Map_Face_Name_to_Index.find(FaceName);
	if (it != m_MeshData->Map_Face_Name_to_Index.end())
	{
		return it->second;
	}
	else
	{
		//MDK_Warning("Invalid FaceName @ PolygonMesh::GetFaceIndexByName(...)")
		return -1;
	}
}

//-------------- check ID -------------------------------------------------------//

template<typename ScalarType>
inline
bool PolygonMesh<ScalarType>::IsValidPointID(int_max PointID) const
{
    if (PointID < 0)
    {
        return false;
    }

    auto it = m_MeshData->Map_Point_ID_to_Index.find(PointID);
    return (it != m_MeshData->Map_Point_ID_to_Index.end());
}

template<typename ScalarType>
inline 
bool PolygonMesh<ScalarType>::IsValidEdgeID(int_max EdgeID) const
{
    if (EdgeID < 0)
    {
        return false;
    }

    auto it = m_MeshData->Map_Edge_ID_to_Index.find(EdgeID);
    return (it != m_MeshData->Map_Edge_ID_to_Index.end());
}


template<typename ScalarType>
inline
bool PolygonMesh<ScalarType>::IsValidFaceID(int_max FaceID) const
{
    if (FaceID < 0)
    {
        return false;
    }

    auto it = m_MeshData->Map_Face_ID_to_Index.find(FaceID);
    return (it != m_MeshData->Map_Face_ID_to_Index.end());
}

//--------- get Valid IDList ------------------------------------------------------------//

template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetValidPointIDList() const
{
	DenseVector<int_max> OutputIDList;
	OutputIDList.SetCapacity(int_max(m_MeshData->Map_Point_ID_to_Index.size()));
	for (auto it = m_MeshData->Map_Point_ID_to_Index.begin(); it != m_MeshData->Map_Point_ID_to_Index.end(); ++it)
	{
		OutputIDList.Append(it->first);
	}
	return OutputIDList;
}

template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetValidEdgeIDList() const
{
	DenseVector<int_max> OutputIDList;
	OutputIDList.SetCapacity(int_max(m_MeshData->Map_Edge_ID_to_Index.size()));
	for (auto it = m_MeshData->Map_Edge_ID_to_Index.begin(); it != m_MeshData->Map_Edge_ID_to_Index.end(); ++it)
	{
		OutputIDList.Append(it->first);
	}
	return OutputIDList;
}


template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetValidFaceIDList() const
{
	DenseVector<int_max> OutputIDList;
	OutputIDList.SetCapacity(int_max(m_MeshData->Map_Face_ID_to_Index.size()));
	for (auto it = m_MeshData->Map_Face_ID_to_Index.begin(); it != m_MeshData->Map_Face_ID_to_Index.end(); ++it)
	{
		OutputIDList.Append(it->first);
	}
	return OutputIDList;
}


//----------- get Point/Edge/Face ID by Index -----------------------------------------------------------//

template<typename ScalarType>
inline 
int_max PolygonMesh<ScalarType>::GetPointID(int_max PointIndex) const
{
    if (this->IsValidPointIndex(PointIndex) == true)
    {
        return m_MeshData->PointList[PointIndex].GetID();
    }
    else
    {
        return -1;
    }
}


template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetPointID(const DenseVector<int_max>& PointIndexList) const
{
	DenseVector<int_max> PointIDList;
	PointIDList.Resize(PointIndexList.GetLength());
	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		PointIDList[k] = this->GetPointID(PointIndexList[k]);
	}
	return PointIDList;
}


template<typename ScalarType>
inline
int_max PolygonMesh<ScalarType>::GetEdgeID(int_max EdgeIndex) const
{
    if (this->IsValidEdgeIndex(EdgeIndex) == true)
    {
        return m_MeshData->EdgeList[EdgeIndex].GetID();
    }
    else
    {
        return -1;
    }
}


template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetEdgeID(const DenseVector<int_max>& EdgeIndexList) const
{
	DenseVector<int_max> EdgeIDList;
	EdgeIDList.Resize(EdgeIndexList.GetLength());
	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		EdgeIDList[k] = this->GetEdgeID(EdgeIndexList[k]);
	}
	return EdgeIDList;
}


template<typename ScalarType>
inline 
int_max PolygonMesh<ScalarType>::GetFaceID(int_max FaceIndex) const
{
    if (this->IsValidFaceIndex(FaceIndex) == true)
    {
        return m_MeshData->FaceList[FaceIndex].GetID();
    }
    else
    {
        return -1;
    }
}


template<typename ScalarType>
inline
DenseVector<int_max> PolygonMesh<ScalarType>::GetFaceID(const DenseVector<int_max>& FaceIndexList) const
{
	DenseVector<int_max> FaceIDList;
	FaceIDList.Resize(FaceIndexList.GetLength());
	for (int_max k = 0; k < FaceIndexList.GetLength(); ++k)
	{
		FaceIDList[k] = this->GetFaceID(FaceIndexList[k]);
	}
	return FaceIDList;
}

//-------------- check Name -------------------------------------------------------//

template<typename ScalarType>
inline bool PolygonMesh<ScalarType>::IsValidPointName(const String& PointName) const
{
	if (PointName.IsEmpty() == true)
	{
		return false;
	}
	auto it = m_MeshData->Map_Point_Name_to_Index.find(PointName);
	return (it != m_MeshData->Map_Point_Name_to_Index.end());
}

template<typename ScalarType>
inline bool PolygonMesh<ScalarType>::IsValidEdgeName(const String& EdgeName) const
{
	if (EdgeName.IsEmpty() == true)
	{
		return false;
	}
	auto it = m_MeshData->Map_Edge_Name_to_Index.find(EdgeName);
	return (it != m_MeshData->Map_Edge_Name_to_Index.end());
}


template<typename ScalarType>
inline bool PolygonMesh<ScalarType>::IsValidFaceName(const String& FaceName) const
{
	if (FaceName.IsEmpty() == true)
	{
		return false;
	}
	auto it = m_MeshData->Map_Face_Name_to_Index.find(FaceName);
	return (it != m_MeshData->Map_Face_Name_to_Index.end());
}

//--------- get Valid NameList ------------------------------------------------------------//

template<typename ScalarType>
ObjectArray<String> PolygonMesh<ScalarType>::GetValidPointNameList() const
{
	ObjectArray<String> NameList;
	NameList.SetCapacity(m_MeshData->Map_Point_Name_to_Index.size());
	for (auto it = m_MeshData->Map_Point_Name_to_Index.begin(); it != m_MeshData->Map_Point_Name_to_Index.end(); ++it)
	{
		NameList.Append(it->first);
	}
	return NameList;
}


template<typename ScalarType>
ObjectArray<String> PolygonMesh<ScalarType>::GetValidEdgeNameList() const
{
	ObjectArray<String> NameList;
	NameList.SetCapacity(m_MeshData->Map_Edge_Name_to_Index.size());
	for (auto it = m_MeshData->Map_Edge_Name_to_Index.begin(); it != m_MeshData->Map_Edge_Name_to_Index.end(); ++it)
	{
		NameList.Append(it->first);
	}
	return NameList;
}


template<typename ScalarType>
ObjectArray<String> PolygonMesh<ScalarType>::GetValidFaceNameList() const
{
	ObjectArray<String> NameList;
	NameList.SetCapacity(m_MeshData->Map_Face_Name_to_Index.size());
	for (auto it = m_MeshData->Map_Face_Name_to_Index.begin(); it != m_MeshData->Map_Face_Name_to_Index.end(); ++it)
	{
		NameList.Append(it->first);
	}
	return NameList;
}

//----------- get Point/Edge/Face Name by Index -----------------------------------------------------------//

template<typename ScalarType>
inline String PolygonMesh<ScalarType>::GetPointName(int_max PointIndex) const
{
	if (this->IsValidPointIndex(PointIndex) == true)
	{
		return m_MeshData->PointList[PointIndex].GetName();
	}
	else
	{
		String EmptyName;
		return EmptyName;
	}
}

template<typename ScalarType>
inline ObjectArray<String> PolygonMesh<ScalarType>::GetPointName(const DenseVector<int_max>& PointIndexList) const
{
	ObjectArray<String> NameList;
	NameList.Resize(PointIndexList.GetLength());
	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		NameList[k] = this->GetPointName(PointIndexList[k]);
	}
	return NameList;
}


template<typename ScalarType>
inline String PolygonMesh<ScalarType>::GetEdgeName(int_max EdgeIndex) const
{
	if (this->IsValidEdgeIndex(EdgeIndex) == true)
	{
		return m_MeshData->EdgeList[EdgeIndex].GetName();
	}
	else
	{
		String EmptyName;
		return EmptyName;
	}
}

template<typename ScalarType>
inline ObjectArray<String> PolygonMesh<ScalarType>::GetEdgeName(const DenseVector<int_max>& EdgeIndexList) const
{
	ObjectArray<String> NameList;
	NameList.Resize(EdgeIndexList.GetLength());
	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		NameList[k] = this->GetEdgeName(EdgeIndexList[k]);
	}
	return NameList;
}


template<typename ScalarType>
inline String PolygonMesh<ScalarType>::GetFaceName(int_max FaceIndex) const
{
	if (this->IsValidFaceIndex(FaceIndex) == true)
	{
		return m_MeshData->FaceList[FaceIndex].GetName();
	}
	else
	{
		String EmptyName;
		return EmptyName;
	}
}

template<typename ScalarType>
inline ObjectArray<String> PolygonMesh<ScalarType>::GetFaceName(const DenseVector<int_max>& FaceIndexList) const
{
	ObjectArray<String> NameList;
	NameList.Resize(FaceIndexList.GetLength());
	for (int_max k = 0; k < FaceIndexList.GetLength(); ++k)
	{
		NameList[k] = this->GetFaceName(FaceIndexList[k]);
	}
	return NameList;
}

//------------ PointSet, EdgeSet, FaceSet ------------------------------------------//

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetPointSetCount() const
{
	return m_MeshData->PointSetList.GetLength();
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::SetPointSet(const String& PointSetName, DenseVector<int_max> PointSet)
{
	auto it = m_MeshData->Map_PointSet_Name_to_Index.find(PointSetName);
	if (it != m_MeshData->Map_PointSet_Name_to_Index.end())
	{
		auto PointSetIndex = it->second;
		m_MeshData->PointSetList[PointSetIndex] = std::move(PointSet);
		return PointSetIndex;
	}
	else
	{
		m_MeshData->PointSetList.Append(std::move(PointSet));
		auto PointSetIndex = m_MeshData->PointSetList.GetLength() - 1;
		m_MeshData->Map_PointSet_Name_to_Index[PointSetName] = PointSetIndex;
		return PointSetIndex;
	}
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetPointSetIndex(const String& PointSetName) const
{
	auto it = m_MeshData->Map_PointSet_Name_to_Index.find(PointSetName);
	if (it != m_MeshData->Map_PointSet_Name_to_Index.end())
	{
		return it->second;
	}
	
	MDK_Warning("PointSet " << PointSetName << " NOT exist @ PolygonMesh::GetPointSetIndex(...)")
	return -1;
}


template<typename ScalarType>
String PolygonMesh<ScalarType>::GetPointSetName(int_max PointSetIndex) const
{
	for (auto it = m_MeshData->Map_PointSet_Name_to_Index.begin(); it != m_MeshData->Map_PointSet_Name_to_Index.end(); ++it)
	{
		if (it->second == PointSetIndex)
		{
			return it->first;
		}
	}

	MDK_Warning("PointSet " << PointSetIndex << " NOT exist @  PolygonMesh::GetPointSetName(...)")
	String EmptyName;
	return EmptyName;
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::GetPointSet(int_max PointSetIndex) const
{
	return m_MeshData->PointSetList[PointSetIndex];
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::GetPointSet(const String& PointSetName) const
{
	auto it = m_MeshData->Map_PointSet_Name_to_Index.find(PointSetName);
	if (it != m_MeshData->Map_PointSet_Name_to_Index.end())
	{
		return m_MeshData->PointSetList[it->second];
	}
	
	MDK_Warning("Unknown PointSetName: " << PointSetName << " @ PolygonMesh::GetPointSet()")
	DenseVector<int_max> EmptySet;		
	return EmptySet;
}


template<typename ScalarType>
ObjectArray<String> PolygonMesh<ScalarType>::GetPointSetName(MDK_Symbol_ALL&) const
{
	ObjectArray<String> NameList;
	NameList.SetCapacity(m_MeshData->Map_PointSet_Name_to_Index.size());
	DenseVector<int_max> IndexList;
	IndexList.SetCapacity(m_MeshData->Map_PointSet_Name_to_Index.size());
	for (auto it = m_MeshData->Map_PointSet_Name_to_Index.begin(); it != m_MeshData->Map_PointSet_Name_to_Index.end(); ++it)
	{
		NameList.Append(it->first);
		IndexList.Append(it->second);
	}
	auto IndexList_sort = IndexList.Sort("ascend");
	NameList = NameList.GetSubSet(IndexList_sort);
	return NameList;
}


template<typename ScalarType>
ObjectArray<DenseVector<int_max>> PolygonMesh<ScalarType>::GetPointSet(MDK_Symbol_ALL&) const
{
	ObjectArray<DenseVector<int_max>> PointSetList;
	PointSetList = m_MeshData->PointSetList;
	return PointSetList;
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetEdgeSetCount() const
{
	return m_MeshData->EdgeSetList.GetLength();
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::SetEdgeSet(const String& EdgeSetName, DenseVector<int_max> EdgeSet)
{
	auto it = m_MeshData->Map_EdgeSet_Name_to_Index.find(EdgeSetName);
	if (it != m_MeshData->Map_EdgeSet_Name_to_Index.end())
	{
		auto EdgeSetIndex = it->second;
		m_MeshData->EdgeSetList[EdgeSetIndex] = std::move(EdgeSet);
		return EdgeSetIndex;
	}
	else
	{
		m_MeshData->EdgeSetList.Append(std::move(EdgeSet));
		auto EdgeSetIndex = m_MeshData->EdgeSetList.GetLength() - 1;
		m_MeshData->Map_EdgeSet_Name_to_Index[EdgeSetName] = EdgeSetIndex;
		return EdgeSetIndex;
	}
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetEdgeSetIndex(const String& EdgeSetName) const
{
	auto it = m_MeshData->Map_EdgeSet_Name_to_Index.find(EdgeSetName);
	if (it != m_MeshData->Map_EdgeSet_Name_to_Index.end())
	{
		return it->second;
	}
	
	MDK_Warning("EdgeSet NOT exist @ PolygonMesh::GetEdgeSetIndex(...)")
	return -1;
}


template<typename ScalarType>
String PolygonMesh<ScalarType>::GetEdgeSetName(int_max EdgeSetIndex) const
{
	for (auto it = m_MeshData->Map_EdgeSet_Name_to_Index.begin(); it != m_MeshData->Map_EdgeSet_Name_to_Index.end(); ++it)
	{
		if (it->second == EdgeSetIndex)
		{
			return it->first;
		}
	}

	MDK_Warning("EdgeSet " << EdgeSetIndex << " NOT exist @  PolygonMesh::GetEdgeSetName(...)")
	String EmptyName;
	return EmptyName;
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::GetEdgeSet(int_max EdgeSetIndex) const
{
	return m_MeshData->EdgeSetList[EdgeSetIndex];
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::GetEdgeSet(const String& EdgeSetName) const
{
	auto it = m_MeshData->Map_EdgeSet_Name_to_Index.find(EdgeSetName);
	if (it != m_MeshData->Map_EdgeSet_Name_to_Index.end())
	{
		return m_MeshData->EdgeSetList[it->second];
	}
	
	MDK_Warning("Unknown EdgeSetName: " << EdgeSetName << " @ PolygonMesh::GetEdgeSet()")
	DenseVector<int_max> EmptySet;		
	return EmptySet;
}


template<typename ScalarType>
ObjectArray<String> PolygonMesh<ScalarType>::GetEdgeSetName(MDK_Symbol_ALL&) const
{
	ObjectArray<String> NameList;
	NameList.SetCapacity(m_MeshData->Map_EdgeSet_Name_to_Index.size());
	DenseVector<int_max> IndexList;
	IndexList.SetCapacity(m_MeshData->Map_EdgeSet_Name_to_Index.size());
	for (auto it = m_MeshData->Map_EdgeSet_Name_to_Index.begin(); it != m_MeshData->Map_EdgeSet_Name_to_Index.end(); ++it)
	{
		NameList.Append(it->first);
		IndexList.Append(it->second);
	}
	auto IndexList_sort = IndexList.Sort("ascend");
	NameList = NameList.GetSubSet(IndexList_sort);
	return NameList;
}


template<typename ScalarType>
ObjectArray<DenseVector<int_max>> PolygonMesh<ScalarType>::GetEdgeSet(MDK_Symbol_ALL&) const
{
	ObjectArray<DenseVector<int_max>> EdgeSetList;
	EdgeSetList = m_MeshData->EdgeSetList;
	return EdgeSetList;
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetFaceSetCount() const
{
	return m_MeshData->FaceSetList.GetLength();
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::SetFaceSet(const String& FaceSetName, DenseVector<int_max> FaceSet)
{
	auto it = m_MeshData->Map_FaceSet_Name_to_Index.find(FaceSetName);
	if (it != m_MeshData->Map_FaceSet_Name_to_Index.end())
	{
		auto FaceSetIndex = it->second;
		m_MeshData->FaceSetList[FaceSetIndex] = std::move(FaceSet);
		return FaceSetIndex;
	}
	else
	{
		m_MeshData->FaceSetList.Append(std::move(FaceSet));
		auto FaceSetIndex = m_MeshData->FaceSetList.GetLength() - 1;
		m_MeshData->Map_FaceSet_Name_to_Index[FaceSetName] = FaceSetIndex;
		return FaceSetIndex;
	}
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetFaceSetIndex(const String& FaceSetName) const
{
	auto it = m_MeshData->Map_FaceSet_Name_to_Index.find(FaceSetName);
	if (it != m_MeshData->Map_FaceSet_Name_to_Index.end())
	{
		return it->second;
	}
	MDK_Warning("FaceSet " << FaceSetName << " NOT exist @ PolygonMesh::GetFaceSetIndex(...)")
	return -1;
}


template<typename ScalarType>
String PolygonMesh<ScalarType>::GetFaceSetName(int_max FaceSetIndex) const
{
	for (auto it = m_MeshData->Map_FaceSet_Name_to_Index.begin(); it != m_MeshData->Map_FaceSet_Name_to_Index.end(); ++it)
	{
		if (it->second == FaceSetIndex)
		{
			return it->first;
		}
	}
	MDK_Warning("FaceSet " << FaceSetIndex << "NOT exist @  PolygonMesh::GetFaceSetName(...)")
	String EmptyName;
	return EmptyName;
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::GetFaceSet(int_max FaceSetIndex) const
{
	return m_MeshData->FaceSetList[FaceSetIndex];
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::GetFaceSet(const String& FaceSetName) const
{
	auto it = m_MeshData->Map_FaceSet_Name_to_Index.find(FaceSetName);
	if (it != m_MeshData->Map_FaceSet_Name_to_Index.end())
	{
		return m_MeshData->FaceSetList[it->second];
	}	
	MDK_Warning("Unknown FaceSetName: " << FaceSetName << " @ PolygonMesh::GetFaceSet()")
	DenseVector<int_max> EmptySet;		
	return EmptySet;	
}


template<typename ScalarType>
ObjectArray<String> PolygonMesh<ScalarType>::GetFaceSetName(MDK_Symbol_ALL&) const
{
	ObjectArray<String> NameList;
	NameList.SetCapacity(m_MeshData->Map_FaceSet_Name_to_Index.size());
	DenseVector<int_max> IndexList;
	IndexList.SetCapacity(m_MeshData->Map_FaceSet_Name_to_Index.size());
	for (auto it = m_MeshData->Map_FaceSet_Name_to_Index.begin(); it != m_MeshData->Map_FaceSet_Name_to_Index.end(); ++it)
	{
		NameList.Append(it->first);
		IndexList.Append(it->second);
	}
	auto IndexList_sort = IndexList.Sort("ascend");
	NameList = NameList.GetSubSet(IndexList_sort);
	return NameList;
}


template<typename ScalarType>
ObjectArray<DenseVector<int_max>> PolygonMesh<ScalarType>::GetFaceSet(MDK_Symbol_ALL&) const
{
	ObjectArray<DenseVector<int_max>> FaceSetList;
	FaceSetList = m_MeshData->FaceSetList;
	return FaceSetList;
}

//------------- PointDataSet, EdgeDataSet, FaceDataset ----------------------------------//
template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetPointDataSetCount() const
{
	return m_MeshData->PointDataSet.GetLength();
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::SetPointDataSet(const String& Name, int_max ScalarCountPerPoint)
{
	auto it = m_MeshData->Map_PointDataSet_Name_to_Index.find(Name);
	if (it != m_MeshData->Map_PointDataSet_Name_to_Index.end())
	{
		auto Index = it->second;
		m_MeshData->PointDataSet[Index].Resize(ScalarCountPerPoint, this->GetMaxValueOfPointIndex());
		m_MeshData->PointDataSet[Index].Fill(ScalarType(0));
		return Index;
	}
	else
	{//new
		DenseMatrix<ScalarType> DataSet;
		DataSet.Resize(ScalarCountPerPoint, this->GetMaxValueOfPointIndex());
		DataSet.Fill(ScalarType(0));
		m_MeshData->PointDataSet.Append(std::move(DataSet));
		auto Index = m_MeshData->PointDataSet.GetLength() - 1;
		m_MeshData->Map_PointDataSet_Name_to_Index[Name] = Index;
		return Index;
	}
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetPointDataSetIndex(const String& Name) const
{
	auto it = m_MeshData->Map_PointDataSet_Name_to_Index.find(Name);
	if (it != m_MeshData->Map_PointDataSet_Name_to_Index.end())
	{
		return it->second;
	}
	MDK_Warning("Name " << Name << " NOT exist @ PolygonMesh::GetPointDataSetIndex(...)")
	return -1;
}


template<typename ScalarType>
String PolygonMesh<ScalarType>::GetPointDataSetName(int_max Index) const
{
	for (auto it = m_MeshData->Map_PointDataSet_Name_to_Index.begin(); it != m_MeshData->Map_PointDataSet_Name_to_Index.end(); ++it)
	{
		if (it->second == Index)
		{
			return it->first;
		}
	}
	MDK_Warning("Index " << Index << "NOT exist @ PolygonMesh::GetPointDataSetName(...)")
	String EmptyName;
	return EmptyName;
}

template<typename ScalarType>
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetPointDataSet(int_max Index) const
{
	if (Index < 0 || Index >= m_MeshData->PointDataSet.GetLength())
	{
		MDK_Error("Invalid Index @ PolygonMesh::GetPointDataSet(...)")
		DenseMatrix<ScalarType> EmptySet;
		return EmptySet;
	}
	
	auto ValidPointIndexList = m_MeshData->PointValidityFlagList.Find([](int_max Flag) { return Flag > 0; });
	return m_MeshData->PointDataSet[Index].GetSubMatrix(ALL, ValidPointIndexList);
}

template<typename ScalarType>
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetPointDataSet(const String& Name) const
{
	auto Index = this->GetPointDataSetIndex(Name);
	return this->GetPointDataSet(Index);
}

//
template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetEdgeDataSetCount() const
{
	return m_MeshData->EdgeDataSet.GetLength();
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::SetEdgeDataSet(const String& Name, int_max ScalarCountPerEdge)
{
	auto it = m_MeshData->Map_EdgeDataSet_Name_to_Index.find(Name);
	if (it != m_MeshData->Map_EdgeDataSet_Name_to_Index.end())	
	{
		auto Index= it->second;
		m_MeshData->EdgeDataSet[Index].Resize(ScalarCountPerEdge, this->GetMaxValueOfEdgeIndex());
		m_MeshData->EdgeDataSet[Index].Fill(ScalarType(0));
		return Index;
	}
	else
	{//new
		DenseMatrix<ScalarType> DataSet;
		DataSet.Resize(ScalarCountPerEdge, this->GetMaxValueOfEdgeIndex());
		DataSet.Fill(ScalarType(0));
		m_MeshData->EdgeDataSet.Append(std::move(DataSet));
		auto Index = m_MeshData->EdgeDataSet.GetLength() - 1;
		m_MeshData->Map_EdgeDataSet_Name_to_Index[Name] = Index;
		return Index;
	}
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetEdgeDataSetIndex(const String& Name) const
{
	auto it = m_MeshData->Map_EdgeDataSet_Name_to_Index.find(Name);
	if (it != m_MeshData->Map_EdgeDataSet_Name_to_Index.end())
	{
		return it->second;
	}
	MDK_Warning("Name " << Name << " NOT exist @ PolygonMesh::GetEdgeDataSetIndex(...)")
	return -1;
}


template<typename ScalarType>
String PolygonMesh<ScalarType>::GetEdgeDataSetName(int_max Index) const
{
	for (auto it = m_MeshData->Map_EdgeDataSet_Name_to_Index.begin(); it != m_MeshData->Map_EdgeDataSet_Name_to_Index.end(); ++it)
	{
		if (it->second == Index)
		{
			return it->first;
		}
	}
	MDK_Warning("Index " << Index << "NOT exist @ PolygonMesh::GetEdgeDataSetName(...)")
	String EmptyName;
	return EmptyName;
}

template<typename ScalarType>
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetEdgeDataSet(int_max Index) const
{
	if (Index < 0 || Index >= m_MeshData->EdgeDataSet.GetLength())
	{
		MDK_Error("Invalid Index @ PolygonMesh::GetEdgeDataSet(...)")
		DenseMatrix<ScalarType> EmptySet;
		return EmptySet;
	}

	auto ValidEdgeIndexList = m_MeshData->EdgeValidityFlagList.Find([](int_max Flag) { return Flag > 0; });
	return m_MeshData->EdgeDataSet[Index].GetSubMatrix(ALL, ValidEdgeIndexList);
}

template<typename ScalarType>
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetEdgeDataSet(const String& Name) const
{
	auto Index = this->GetEdgeDataSetIndex(Name);
	return this->GetEdgeDataSet(Index);
}

//
template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetFaceDataSetCount() const
{
	return m_MeshData->FaceDataSet.GetLength();
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::SetFaceDataSet(const String& Name, int_max ScalarCountPerFace)
{
	auto it = m_MeshData->Map_FaceDataSet_Name_to_Index.find(Name);
	if (it != m_MeshData->Map_FaceDataSet_Name_to_Index.end())
	{
		auto Index = it->second;		
		m_MeshData->FaceDataSet[Index].Resize(ScalarCountPerFace, this->GetMaxValueOfFaceIndex());
		m_MeshData->FaceDataSet[Index].Fill(ScalarType(0));
		return Index;
	}
	else
	{//new
		DenseMatrix<ScalarType> DataSet;
		DataSet.Resize(ScalarCountPerFace, this->GetMaxValueOfFaceIndex());
		DataSet.Fill(ScalarType(0));
		m_MeshData->FaceDataSet.Append(std::move(DataSet));
		auto Index = m_MeshData->FaceDataSet.GetLength() - 1;
		m_MeshData->Map_FaceDataSet_Name_to_Index[Name] = Index;
		return Index;
	}
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetFaceDataSetIndex(const String& Name) const
{
	auto it = m_MeshData->Map_FaceDataSet_Name_to_Index.find(Name);
	if (it != m_MeshData->Map_FaceDataSet_Name_to_Index.end())
	{
		return it->second;
	}
	MDK_Warning("Name " << Name << " NOT exist @ PolygonMesh::GetFaceDataSetIndex(...)")
	return -1;
}


template<typename ScalarType>
String PolygonMesh<ScalarType>::GetFaceDataSetName(int_max Index) const
{
	for (auto it = m_MeshData->Map_FaceDataSet_Name_to_Index.begin(); it != m_MeshData->Map_FaceDataSet_Name_to_Index.end(); ++it)
	{
		if (it->second == Index)
		{
			return it->first;
		}
	}
	MDK_Warning("Index " << Index << "NOT exist @ PolygonMesh::GetFaceDataSetName(...)")
	String EmptyName;
	return EmptyName;
}

template<typename ScalarType>
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetFaceDataSet(int_max Index) const
{
	if (Index < 0 || Index >= m_MeshData->FaceDataSet.GetLength())
	{
		MDK_Error("Invalid Index @ PolygonMesh::GetFaceDataSet(...)")
		DenseMatrix<ScalarType> EmptySet;
		return EmptySet;
	}

	auto ValidFaceIndexList = m_MeshData->FaceValidityFlagList.Find([](int_max Flag) { return Flag > 0; });
	return m_MeshData->FaceDataSet[Index].GetSubMatrix(ALL, ValidFaceIndexList);
}

template<typename ScalarType>
DenseMatrix<ScalarType> PolygonMesh<ScalarType>::GetFaceDataSet(const String& Name) const
{
	auto Index = this->GetFaceDataSetIndex(Name);
	return this->GetFaceDataSet(Index);
}
//------------- Iterator --------------------------------------------------------------//

template<typename ScalarType>
inline
Iterator_Of_Point_Of_PolygonMesh<ScalarType> PolygonMesh<ScalarType>::GetIteratorOfPoint()
{
	PointIteratorType it(*this);
	return it;
}

template<typename ScalarType>
inline
const Iterator_Of_Point_Of_PolygonMesh<ScalarType> PolygonMesh<ScalarType>::GetIteratorOfPoint() const
{
	PointIteratorType it(*this);
	return it;
}

template<typename ScalarType>
inline
Iterator_Of_Edge_Of_PolygonMesh<ScalarType> PolygonMesh<ScalarType>::GetIteratorOfEdge()
{
	EdgeIteratorType it(*this);
	return it;
}

template<typename ScalarType>
inline
const Iterator_Of_Edge_Of_PolygonMesh<ScalarType> PolygonMesh<ScalarType>::GetIteratorOfEdge() const
{
	EdgeIteratorType it(*this);
	return it;
}


template<typename ScalarType>
inline
Iterator_Of_Face_Of_PolygonMesh<ScalarType> PolygonMesh<ScalarType>::GetIteratorOfFace()
{
	FaceIteratorType it(*this);
	return it;
}

template<typename ScalarType>
inline
const Iterator_Of_Face_Of_PolygonMesh<ScalarType> PolygonMesh<ScalarType>::GetIteratorOfFace() const
{
	FaceIteratorType it(*this);
	return it;
}


//------------ SetCapacity, ReleaseUnusedCapacity -------------------------------------//
template<typename ScalarType>
void PolygonMesh<ScalarType>::SetCapacity(int_max PointCount, int_max EdgeCount, int_max FaceCount)
{
	if (this->IsPureEmpty() == true)
	{
		this->Recreate();
	}

	m_MeshData->PointPositionTable.SetCapacity(3 * PointCount);
	m_MeshData->PointList.SetCapacity(PointCount);
	m_MeshData->PointValidityFlagList.SetCapacity(PointCount);
	m_MeshData->EdgeList.SetCapacity(EdgeCount);
	m_MeshData->EdgeValidityFlagList.SetCapacity(EdgeCount);
	m_MeshData->FaceList.SetCapacity(FaceCount);
	m_MeshData->FaceValidityFlagList.SetCapacity(FaceCount);
}


template<typename ScalarType>
void PolygonMesh<ScalarType>::ReleaseUnusedCapacity()
{
	if (this->IsPureEmpty() == true)
	{
		return;
	}

	m_MeshData->PointPositionTable->ReleaseUnusedCapacity();
	m_MeshData->PointList->ReleaseUnusedCapacity();
	m_MeshData->PointValidityFlagList->ReleaseUnusedCapacity();
	m_MeshData->EdgeList->ReleaseUnusedCapacity();
	m_MeshData->EdgeValidityFlagList->ReleaseUnusedCapacity();
	m_MeshData->FaceList->ReleaseUnusedCapacity();
	m_MeshData->FaceValidityFlagList->ReleaseUnusedCapacity();
}

//------------------------------ Add Mesh Item -------------------------------------------------------------------------//

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddPoint(const DenseVector<ScalarType, 3>& Position)
{
    return this->AddPoint(Position[0], Position[1], Position[2]);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddPoint(const DenseVector<ScalarType>& Position)
{
    if (PointSet.GetLength() != 3)
    {
        MDK_Error("Position is a vector but length != 3 @ PolygonMesh::AddPoint(...)")
        return -1;
    }

    return this->AddPoint(Position[0], Position[1], Position[2]);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddPoint(const DenseMatrix<ScalarType>& Position)
{
    if (Position.IsVector() == true)
    {
        if (Position.GetElementCount() != 3)
        {
            MDK_Error("Position is a vector but length != 3 @ PolygonMesh::AddPoint(...)")
            return -1;
        }
    }
    else
    {
        MDK_Error("Position is a not a vector @ PolygonMesh::AddPoint(...)")
        return -1;
    }

    return this->AddPoint(Position[0], Position[1], Position[2]);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddPoint(const ScalarType Position[3])
{
    return this->AddPoint(Position[0], Position[1], Position[2]);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddPoint(ScalarType x, ScalarType y, ScalarType z)
{
    m_MeshData->PointPositionTable.AppendCol({x, y, z});
    auto PointIndex = m_MeshData->PointPositionTable.GetColCount() - 1;

    Point_Of_PolygonMesh<ScalarType> Point;
    Point.SetParentMesh(*this);
    Point.SetIndex(PointIndex);
   
    m_MeshData->PointList.Append(std::move(Point));
    m_MeshData->PointValidityFlagList.Append(1);

    return PointIndex;
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::AddPoint_batch(const DenseMatrix<ScalarType>& PointSet)
{
	DenseVector<int_max> PointIndexList;

    if (PointSet.IsVector() == true)
    {
        if (PointSet.GetElementCount() != 3)
        {
            MDK_Error("PointSet is a vector but length != 3 @ PolygonMesh::AddPoint_batch(...)")
            return PointIndexList;
        }

		PointIndexList.Resize(1);
		PointIndexList[0] = this->AddPoint(PointSet.GetPointer());
        return PointIndexList;
    }

    if (PointSet.GetRowCount() != 3)
    {
        MDK_Error("PointSet is a matrix but RowCount != 3 @ PolygonMesh::AddPoint_batch(...)")
        return PointIndexList;
    }

	PointIndexList.Resize(PointSet.GetColCount());
    for (int_max k = 0; k < PointSet.GetColCount(); ++k)
    {
		PointIndexList[k] = this->AddPoint(PointSet.GetPointerOfCol(k));
    }
    return PointIndexList;
}


template<typename ScalarType>
DenseVector<int_max> PolygonMesh<ScalarType>::AddPoint_batch(DenseMatrix<ScalarType>&& PointSet)
{
	DenseVector<int_max> PointIndexList;

	if (PointSet.IsVector() == true)
	{
		if (PointSet.GetElementCount() != 3)
		{
			MDK_Error("PointSet is a vector but length != 3 @ PolygonMesh::AddPoint_batch(...)")
			return PointIndexList;
		}

		PointIndexList.Resize(1);
		PointIndexList[0] = this->AddPoint(PointSet.GetPointer());
		return PointIndexList;
	}

	if (PointSet.GetRowCount() != 3)
	{
		MDK_Error("PointSet is a matrix but RowCount != 3 @ PolygonMesh::AddPoint_batch(...)")
		return PointIndexList;
	}

	if (this->IsEmpty() == true)
	{
		m_MeshData->PointPositionTable = std::move(PointSet);
		PointIndexList.Resize(m_MeshData->PointPositionTable.GetColCount());
		for (int_max k = 0; k < m_MeshData->PointPositionTable.GetColCount(); ++k)
		{
			Point_Of_PolygonMesh<ScalarType> Point;
			Point.SetParentMesh(*this);
			Point.SetIndex(k);

			m_MeshData->PointList.Append(std::move(Point));
			m_MeshData->PointValidityFlagList.Append(1);

			PointIndexList[k]=k;
		}
	}
	else
	{
		PointIndexList.Resize(PointSet.GetColCount());
		for (int_max k = 0; k < PointSet.GetColCount(); ++k)
		{
			PointIndexList[k] = this->AddPoint(PointSet.GetPointerOfCol(k));
		}
	}
	return PointIndexList;
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddEdge(int_max PointIndex0, int_max PointIndex1)
{
	auto EdgeIndex = m_MeshData->EdgeList.GetLength();
	return this->AddEdge(PointIndex0, PointIndex1, EdgeIndex);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddEdge(int_max PointIndex0, int_max PointIndex1, int_max EdgeIndex_input)
{
    // pay attention to MDK_Error or MDK_Warning, they are made on purpose

    if (this->IsValidPointIndex(PointIndex0) == false || this->IsValidPointIndex(PointIndex1) == false)
    {
        MDK_Error("PointIndex0 or PointIndex1 is invalid @ PolygonMesh::AddEdge(...)")
        return -1;
    }

	if (EdgeIndex_input < 0 || EdgeIndex_input > m_MeshData->EdgeList.GetLength())
	{
		MDK_Error("EdgeIndex_input is out of range @ PolygonMesh::AddEdge(...)")
		return -1;
	}

    auto EdgeIndex_temp = this->GetEdgeIndexByPoint(PointIndex0, PointIndex1);
    if (this->IsValidEdgeIndex(EdgeIndex_temp) == true)
    {
        //MDK_Warning("The edge has been added already @ PolygonMesh::AddEdge(...)")
		if (EdgeIndex_input == m_MeshData->EdgeList.GetLength() || EdgeIndex_input == EdgeIndex_temp)
		{
			return EdgeIndex_temp;
		}
		else
		{
			MDK_Error("Edge exist at EdgeIndex_input @ PolygonMesh::AddEdge(...)")
			return -1;
		}
    }

	if (EdgeIndex_input < m_MeshData->EdgeValidityFlagList.GetLength())
	{
		if (m_MeshData->EdgeValidityFlagList[EdgeIndex_input] == 1)
		{
			MDK_Error("Edge exist at EdgeIndex_input @ PolygonMesh::AddEdge(...)")
			return -1;
		}
	}

	//Create Edge ----------------------------------------------------------------

	if (EdgeIndex_input == m_MeshData->EdgeList.GetLength())
	{
		EdgeType EmptyEdge;
		m_MeshData->EdgeList.Append(std::move(EmptyEdge));
		m_MeshData->EdgeValidityFlagList.Append(1);
	}
	else
	{
		m_MeshData->EdgeList[EdgeIndex_input].ReCreate();
		m_MeshData->EdgeValidityFlagList[EdgeIndex_input] = 1;
	}
    // take reference
	auto& Edge = m_MeshData->EdgeList[EdgeIndex_input];
    Edge.SetParentMesh(*this);
    Edge.SetIndex(EdgeIndex_input);
    Edge.SetPointIndexList(PointIndex0, PointIndex1);

    // update AdjacentPoint information in m_MeshData->PointList
    m_MeshData->PointList[PointIndex0].AdjacentEdgeIndexList().Append(EdgeIndex_input);
    m_MeshData->PointList[PointIndex1].AdjacentEdgeIndexList().Append(EdgeIndex_input);
    
    //------------
    return EdgeIndex_input;
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddFaceByEdge(const DenseVector<int_max>& EdgeIndexList)
{
	auto FaceIndex = m_MeshData->FaceList.GetLength();
	return this->AddFaceByEdge(EdgeIndexList, FaceIndex);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddFaceByEdge(const DenseVector<int_max>& EdgeIndexList, int_max FaceInex_input)
{
    // Input: the input EdgeIndexList must have been ordered
	// the first point of the first input edge may NOT be the first point of the face
    // the order determine the direction/sign of the normal vector
    //
    // pay attention to MDK_Error or MDK_Warning, they are made on purpose

    if (EdgeIndexList.GetLength() < 2)
    {
        MDK_Error("length of EdgeIndexList < 2 @ PolygonMesh::AddFaceByEdge(...)")
        return -1;
    }

    for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
    {
        if (this->IsValidEdgeIndex(EdgeIndexList[k]) == false)
        {
            MDK_Error("Invalid EdgeIndexList @ PolygonMesh::AddFaceByEdge(...)")
            return -1;
        }
    }

	if (FaceInex_input < 0 && FaceInex_input > m_MeshData->FaceValidityFlagList.GetLength())
	{
		MDK_Error("FaceInex_input is out of range @ PolygonMesh::AddFaceByEdge(...)")
		return -1;
	}

    auto FaceIndex_temp = this->GetFaceIndexByEdge(EdgeIndexList);
    if (this->IsValidFaceIndex(FaceIndex_temp) == true)
    {
        //MDK_Warning("The face has been added already @ PolygonMesh::AddFaceByEdge(...)")

		if (FaceInex_input == m_MeshData->FaceValidityFlagList.GetLength() || FaceInex_input == FaceIndex_temp)
		{//attempt to add new face, not necessary			
			return FaceIndex_temp;
		}
		else
		{// attempt to replace old&valid face with new face
			MDK_Error("Face arealdy exists at FaceIndex: " << FaceIndex_temp << " @ PolygonMesh::AddFaceByEdge(...)")
			return -1;
		}
    }

	if (FaceInex_input < m_MeshData->FaceValidityFlagList.GetLength())
	{
		if (m_MeshData->FaceValidityFlagList[FaceInex_input] == 1)
		{// attempt to replace old&valid face with new face
			MDK_Error("Face arealdy exists at FaceInex_input @ PolygonMesh::AddFaceByEdge(...)")			
			return -1;
		}
	}

    // check EdgeIndexList
    auto tempList_unique = EdgeIndexList.FindUnique();
    if (tempList_unique.GetLength() != EdgeIndexList.GetLength())
    {
        MDK_Error("EdgeIndexList is invalid : more than one copy of an index @ PolygonMesh::AddFaceByEdge(...)")
        return -1;
    }

	//-------------------- input check is done ---------------------------------------------------//

    // get PointIndexList from EdgeIndexList ---------------------------------------------------------------------------//
       
    DenseVector<int_max> PointIndexList;
    PointIndexList.Resize(EdgeIndexList.GetLength());

    for (int_max k = 0; k < EdgeIndexList.GetLength() - 1; ++k)
    {
        int_max tempPointIndexList_k[2];
        m_MeshData->EdgeList[EdgeIndexList[k]].GetPointIndexList(tempPointIndexList_k);

        int_max tempPointIndexList_next[2];
        m_MeshData->EdgeList[EdgeIndexList[k + 1]].GetPointIndexList(tempPointIndexList_next);

        if (tempPointIndexList_k[0] == tempPointIndexList_next[0])
        {           
            PointIndexList[k] = tempPointIndexList_k[1];
            PointIndexList[k + 1] = tempPointIndexList_k[0];
        }
        else if (tempPointIndexList_k[0] == tempPointIndexList_next[1])
        {
            PointIndexList[k] = tempPointIndexList_k[1];
            PointIndexList[k + 1] = tempPointIndexList_k[0];
        }
        else if (tempPointIndexList_k[1] == tempPointIndexList_next[0])
        {
            PointIndexList[k] = tempPointIndexList_k[0];
            PointIndexList[k + 1] = tempPointIndexList_k[1];
        }
        else if (tempPointIndexList_k[1] == tempPointIndexList_next[1])
        {
            PointIndexList[k] = tempPointIndexList_k[0];
            PointIndexList[k + 1] = tempPointIndexList_k[1];
        }
        else
        {
			MDK_Error("EdgeIndexList is not ordered or invalid @ PolygonMesh::AddFaceByEdge(...)")			
            return -1;
        }
    }

    // create face --------------------------------------------------------------------------------------------------

	if (FaceInex_input == m_MeshData->FaceList.GetLength())
	{
		FaceType EmptyFace;
		m_MeshData->FaceList.Append(std::move(EmptyFace));
		m_MeshData->FaceValidityFlagList.Append(1);
	}
	else
	{
		m_MeshData->FaceList[FaceInex_input].ReCreate();
		m_MeshData->FaceValidityFlagList[FaceInex_input] = 1;
	}
	// take reference
	auto& Face = m_MeshData->FaceList[FaceInex_input];
	Face.SetParentMesh(*this);
	Face.SetIndex(FaceInex_input);
	Face.PointIndexList() = PointIndexList;
	Face.EdgeIndexList() = EdgeIndexList;

	// update information in m_MeshData->EdgeList ------------------------------------------------------------------------
	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		m_MeshData->EdgeList[EdgeIndexList[k]].AdjacentFaceIndexList().Append(FaceInex_input);
	}

    //-----------------------------
    return FaceInex_input;
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddFaceByPoint(const DenseVector<int_max>& PointIndexList)
{
	auto FaceIndex = m_MeshData->FaceList.GetLength();
	return this->AddFaceByPoint(PointIndexList, FaceIndex);
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::AddFaceByPoint(const DenseVector<int_max>& PointIndexList, int_max FaceIndex_input)
{
    if (PointIndexList.GetLength() < 3)
    {        
        MDK_Error("PointIndexList length < 3 @ PolygonMesh::AddFaceByPoint(...)")
        return -1;
    }

	DenseVector<int_max> EdgeIndexList;
	EdgeIndexList.Resize(PointIndexList.GetLength());
    for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
    {
        if (k < PointIndexList.GetLength() - 1)
        {
			EdgeIndexList[k] = this->AddEdge(PointIndexList[k], PointIndexList[k + 1]);
        }
        else
        {
			EdgeIndexList[k] = this->AddEdge(PointIndexList[k], PointIndexList[0]);
        }
    }

	return this->AddFaceByEdge(EdgeIndexList, FaceIndex_input);
}

//------------------- Delete Mesh Item ----------------------------------------------------------------------------//

template<typename ScalarType>
void PolygonMesh<ScalarType>::DeleteFace(int_max FaceIndex)
{
    // this function will modify any information related to the face
    // FaceIndex and FaceID of the face become invalid after the face is deleted

    // check input 
    if (this->IsValidFaceIndex(FaceIndex) == false)
    {
        //MDK_Warning("Invalid FaceIndex @ PolygonMesh::DeleteFace(...)")
        return;
    }

    this->UpdateRecord_DeleteFace(FaceIndex);
}

template<typename ScalarType>
void PolygonMesh<ScalarType>::DeleteFace(const DenseVector<int_max>& FaceIndexList)
{
	if (FaceIndexList.IsEmpty() == true)
	{
		//MDK_Warning("FaceIndexList is empty @ PolygonMesh::DeleteFace(...)")
		return;
	}

	for (int_max k = 0; k < FaceIndexList.GetLength(); ++k)
	{
		this->DeleteFace(FaceIndexList[k]);
	}
}

template<typename ScalarType>
void PolygonMesh<ScalarType>::DeleteEdge(int_max EdgeIndex)
{    
    if (this->IsValidEdgeIndex(EdgeIndex) == false)
    {
        //MDK_Warning("Invalid EdgeIndex @ PolygonMesh::DeleteEdge(...)")
        return;
    }
    
    auto AdjacentFaceCount = m_MeshData->EdgeList[EdgeIndex].GetAdjacentFaceCount();
    if (AdjacentFaceCount > 0)
    {
        MDK_Error("AdjacentFaceIndexList is not empty, so this edge can not be deleted @ PolygonMesh::DeleteEdge(...)")
        return;
    }

    this->UpdateRecord_DeleteEdge(EdgeIndex);
}

template<typename ScalarType>
void PolygonMesh<ScalarType>::DeleteEdge(const DenseVector<int_max>& EdgeIndexList)
{
	if (EdgeIndexList.IsEmpty() == true)
	{
		//MDK_Warning("EdgeIndexList is empty @ PolygonMesh::DeleteEdge(...)")
		return;
	}

	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		this->DeleteEdge(EdgeIndexList[k]);
	}
}

template<typename ScalarType>
void PolygonMesh<ScalarType>::DeletePoint(int_max PointIndex)
{
    if (this->IsValidPointIndex(PointIndex) == false)
    {
        //MDK_Warning("Invalid PointIndex @ PolygonMesh::DeletePoint(...)")
        return;
    }

    if (m_MeshData->PointList[PointIndex].IsOnEdge() == true)
    {
        MDK_Error("The point is on an edge, so it can not be deleted @ PolygonMesh::DeletePoint(...)")
        return;
    }

    this->UpdateRecord_DeletePoint(PointIndex);
}


template<typename ScalarType>
void PolygonMesh<ScalarType>::DeletePoint(const DenseVector<int_max>& PointIndexList)
{
    if (PointIndexList.IsEmpty() == true)
    {
        //MDK_Warning("PointIndexList is empty @ PolygonMesh::DeletePoint(...)")
        return;
    }

    for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
    {
		this->DeletePoint(PointIndexList[k]);
    }
}

//----------------- remove deleted item from Point/Edge/Face list ----------------------------------------------------------//
// attention: 
// after CleanDataStructure() is called, the size of each list will change, index may become invalid,
// but every valid ID will still be valid
// and there will be no "dead/deleted" item in any object list (e.g., m_MeshData->EdgeList)
// Only use CleanDataStructure() if memory is an issue, when DeletedPointIndexCount/ValidPointCount( GetPointCount() ) > 0.5

template<typename ScalarType>
void PolygonMesh<ScalarType>::CleanDataStructure()
{
	DenseVector<int_max> PointIndexMap_Old_To_New;
	DenseVector<int_max> EdgeIndexMap_Old_To_New;
	DenseVector<int_max> FaceIndexMap_Old_To_New;
	this->CleanDataStructure(PointIndexMap_Old_To_New, EdgeIndexMap_Old_To_New, FaceIndexMap_Old_To_New);
}

template<typename ScalarType>
void PolygonMesh<ScalarType>::CleanDataStructure(DenseVector<int_max>& PointIndexMap_Old_To_New, DenseVector<int_max>& EdgeIndexMap_Old_To_New,	DenseVector<int_max>& FaceIndexMap_Old_To_New)
{
	if (this->GetDeletedPointCount() == 0 && this->GetDeletedEdgeCount() == 0 && this->GetDeletedFaceCount() == 0)
	{
		return;
	}

	//---------------- get IndexMap from old to new ---------------------------------------//
	PointIndexMap_Old_To_New.Clear();
	PointIndexMap_Old_To_New.Resize(m_MeshData->PointList.GetLength());

	DenseVector<int_max> PointIndexMap_New_To_Old;
	PointIndexMap_New_To_Old.SetCapacity(m_MeshData->PointList.GetLength());

	for (int_max k = 0; k < m_MeshData->PointList.GetLength(); ++k) // k is PointIndex_old
	{
		PointIndexMap_Old_To_New[k] = -1;
		if (m_MeshData->PointValidityFlagList[k] == 1)
		{
			PointIndexMap_New_To_Old.Append(k);
			auto PointIndex_new = PointIndexMap_New_To_Old.GetLength() - 1;
			PointIndexMap_Old_To_New[k] = PointIndex_new;
		}
	}

	EdgeIndexMap_Old_To_New.Clear();
	EdgeIndexMap_Old_To_New.Resize(m_MeshData->EdgeList.GetLength());

	DenseVector<int_max> EdgeIndexMap_New_To_Old;
	EdgeIndexMap_New_To_Old.SetCapacity(m_MeshData->EdgeList.GetLength());

	for (int_max k = 0; k < m_MeshData->EdgeList.GetLength(); ++k) // k is EdgeIndex_old
	{
		EdgeIndexMap_Old_To_New[k] = -1;
		if (m_MeshData->EdgeValidityFlagList[k] == 1)
		{
			EdgeIndexMap_New_To_Old.Append(k);
			auto EdgeIndex_new = EdgeIndexMap_New_To_Old.GetLength() - 1;
			EdgeIndexMap_Old_To_New[k] = EdgeIndex_new;
		}
	}

	FaceIndexMap_Old_To_New.Clear();
	FaceIndexMap_Old_To_New.Resize(m_MeshData->FaceList.GetLength());

	DenseVector<int_max> FaceIndexMap_New_To_Old;
	FaceIndexMap_New_To_Old.SetCapacity(m_MeshData->FaceList.GetLength());

	for (int_max k = 0; k < m_MeshData->FaceList.GetLength(); ++k) // k is FaceIndex_old
	{
		FaceIndexMap_Old_To_New[k] = -1;
		if (m_MeshData->FaceValidityFlagList[k] == 1)
		{
			FaceIndexMap_New_To_Old.Append(k);
			auto FaceIndex_new = FaceIndexMap_New_To_Old.GetLength() - 1;
			FaceIndexMap_Old_To_New[k] = FaceIndex_new;
		}
	}

	auto PointCount_old = PointIndexMap_Old_To_New.GetLength();// include deleted point
	auto PointCount_new = PointIndexMap_New_To_Old.GetLength();
	auto EdgeCount_old = EdgeIndexMap_Old_To_New.GetLength();// include deleted edge
	auto EdgeCount_new = EdgeIndexMap_New_To_Old.GetLength();
	auto FaceCount_old = FaceIndexMap_Old_To_New.GetLength();// include deleted face
	auto FaceCount_new = FaceIndexMap_New_To_Old.GetLength();

	//------------------------------------------ update -------------------------------------------------------------//

	if (PointCount_old != PointCount_new)
	{
		m_MeshData->PointPositionTable = m_MeshData->PointPositionTable.GetSubMatrix(ALL, PointIndexMap_New_To_Old);

		PointListType PointList_new;
		PointList_new.Resize(PointIndexMap_New_To_Old.GetLength());
		for (int_max k = 0; k < PointIndexMap_New_To_Old.GetLength(); ++k)// k is PointIndex_new
		{
			PointList_new[k] = std::move(m_MeshData->PointList[PointIndexMap_New_To_Old[k]]);
			PointList_new[k].SetIndex(k);
		}
		m_MeshData->PointList = std::move(PointList_new);
		m_MeshData->PointValidityFlagList.Resize(PointCount_new);
		m_MeshData->PointValidityFlagList.Fill(1);

		// update Map_Point_ID_to_Index
		m_MeshData->Map_Point_ID_to_Index.clear();
		for (int_max k = 0; k < m_MeshData->PointList.GetLength(); ++k)
		{
			auto PointID_k = m_MeshData->PointList[k].GetID();
			if (PointID_k >= 0) // ID is invalid if < 0 
			{
				m_MeshData->Map_Point_ID_to_Index[PointID_k] = k;
			}
		}
	}

	if (EdgeCount_old != EdgeCount_new)
	{
		EdgeListType EdgeList_new;
		EdgeList_new.Resize(EdgeIndexMap_New_To_Old.GetLength());
		for (int_max k = 0; k < EdgeIndexMap_New_To_Old.GetLength(); ++k)// k is EdgeIndex_new
		{
			EdgeList_new[k] = std::move(m_MeshData->EdgeList[EdgeIndexMap_New_To_Old[k]]);
			EdgeList_new[k].SetIndex(k);
		}
		m_MeshData->EdgeList = std::move(EdgeList_new);
		m_MeshData->EdgeValidityFlagList.Resize(EdgeCount_new);
		m_MeshData->EdgeValidityFlagList.Fill(1);

		// update Map_Edge_ID_to_Index
		m_MeshData->Map_Edge_ID_to_Index.clear();
		for (int_max k = 0; k < m_MeshData->EdgeList.GetLength(); ++k)
		{
			auto EdgeID_k = m_MeshData->EdgeList[k].GetID();
			if (EdgeID_k >= 0) // ID is invalid if < 0 
			{
				m_MeshData->Map_Edge_ID_to_Index[EdgeID_k] = k;
			}
		}
	}

	if (FaceCount_old != FaceCount_new)
	{
		FaceListType FaceList_new;
		FaceList_new.Resize(FaceIndexMap_New_To_Old.GetLength());
		for (int_max k = 0; k < FaceIndexMap_New_To_Old.GetLength(); ++k)
		{
			FaceList_new[k] = std::move(m_MeshData->FaceList[FaceIndexMap_New_To_Old[k]]);
			FaceList_new[k].SetIndex(k);
		}
		m_MeshData->FaceList = std::move(FaceList_new);
		m_MeshData->FaceValidityFlagList.Resize(FaceCount_new);
		m_MeshData->FaceValidityFlagList.Fill(1);

		// update Map_Face_ID_to_Index
		m_MeshData->Map_Face_ID_to_Index.clear();
		for (int_max k = 0; k < m_MeshData->FaceList.GetLength(); ++k)
		{
			auto FaceID_k = m_MeshData->FaceList[k].GetID();
			if (FaceID_k >= 0) // ID is invalid if < 0 
			{
				m_MeshData->Map_Face_ID_to_Index[FaceID_k] = k;
			}
		}
	}

	//--------------------------- update adjacency information------------------

	for (int_max FaceIndex_new = 0; FaceIndex_new < m_MeshData->FaceList.GetLength(); ++FaceIndex_new)
	{
		//take reference
		auto& PointIndexList_new = m_MeshData->FaceList[FaceIndex_new].PointIndexList();
		auto PointIndexList_old = PointIndexList_new;
		PointIndexList_new.Clear();
		for (int_max n = 0; n < PointIndexList_old.GetLength(); ++n)
		{
			auto PointIndex_new = PointIndexMap_Old_To_New[PointIndexList_old[n]];
			if (PointIndex_new >= 0)
			{
				PointIndexList_new.Append(PointIndex_new);
			}
		}		
		//take reference
		auto& EdgeIndexList_new = m_MeshData->FaceList[FaceIndex_new].EdgeIndexList();
		auto EdgeIndexList_old = EdgeIndexList_new;
		EdgeIndexList_new.Clear();
		for (int_max n = 0; n < EdgeIndexList_old.GetLength(); ++n)
		{
			auto EdgeIndex_new = EdgeIndexMap_Old_To_New[EdgeIndexList_old[n]];
			if (EdgeIndex_new >= 0)
			{
				EdgeIndexList_new.Append(EdgeIndex_new);
			}
		}
	}

	for (auto EdgeIndex_new = 0; EdgeIndex_new < m_MeshData->EdgeList.GetLength(); ++EdgeIndex_new)
	{
		auto PointIndexList = m_MeshData->EdgeList[EdgeIndex_new].GetPointIndexList();
		PointIndexList[0] = PointIndexMap_Old_To_New[PointIndexList[0]];
		PointIndexList[1] = PointIndexMap_Old_To_New[PointIndexList[1]];
		m_MeshData->EdgeList[EdgeIndex_new].SetPointIndexList(PointIndexList);

		auto& AdjacentFaceIndexList_new = m_MeshData->EdgeList[EdgeIndex_new].AdjacentFaceIndexList();		
		auto AdjacentFaceIndexList_old = AdjacentFaceIndexList_new;
		AdjacentFaceIndexList_new.Clear();
		for (int_max n = 0; n < AdjacentFaceIndexList_old.GetLength(); ++n)
		{
			auto Index_new = FaceIndexMap_Old_To_New[AdjacentFaceIndexList_old[n]];
			if (Index_new >= 0)
			{
				AdjacentFaceIndexList_new.Append(Index_new);
			}
		}
	}

	for (int_max PointIndex_new = 0; PointIndex_new < m_MeshData->PointList.GetLength(); ++PointIndex_new)
	{
		auto& AdjacentEdgeIndexList_new = m_MeshData->PointList[PointIndex_new].AdjacentEdgeIndexList();
		auto AdjacentEdgeIndexList_old = AdjacentEdgeIndexList_new;
		AdjacentEdgeIndexList_new.Clear();
		for (int_max n = 0; n < AdjacentEdgeIndexList_old.GetLength(); ++n)
		{
			auto Index_new = EdgeIndexMap_Old_To_New[AdjacentEdgeIndexList_old[n]];
			if (Index_new >= 0)
			{
				AdjacentEdgeIndexList_new.Append(Index_new);
			}
		}		
	}

	// update additional info
	for (auto it = m_MeshData->Map_Point_Name_to_Index.begin(); it != m_MeshData->Map_Point_Name_to_Index.end(); )
	{
		auto Index_old = it->second;
		auto Index_new = PointIndexMap_Old_To_New[Index_old];
		if (Index_new < 0)
		{
			it = m_MeshData->Map_Point_Name_to_Index.erase(it);
		}
		else
		{
			it->second = Index_new;
			++it;
		}
	}

	for (auto it = m_MeshData->Map_Edge_Name_to_Index.begin(); it != m_MeshData->Map_Edge_Name_to_Index.end(); )
	{
		auto Index_old = it->second;
		auto Index_new = EdgeIndexMap_Old_To_New[Index_old];
		if (Index_new < 0)
		{
			it= m_MeshData->Map_Edge_Name_to_Index.erase(it);
		}
		else
		{
			it->second = Index_new;
			++it;
		}
	}

	for (auto it = m_MeshData->Map_Face_Name_to_Index.begin(); it != m_MeshData->Map_Face_Name_to_Index.end();)
	{
		auto Index_old = it->second;
		auto Index_new = FaceIndexMap_Old_To_New[Index_old];
		if (Index_new < 0)
		{
			it= m_MeshData->Map_Face_Name_to_Index.erase(it);
		}
		else
		{
			it->second = Index_new;
			++it;
		}
	}

	for (int_max k = 0; k < m_MeshData->PointSetList.GetLength(); ++k)
	{
		auto& PointSet_new = m_MeshData->PointSetList[k];
		auto PointSet_old = PointSet_new;
		PointSet_new.Clear();
		for (int_max n = 0; n < PointSet_old.GetLength(); ++n)
		{
			auto PointIndex_old = PointSet_old[n];
			auto PointIndex_new = PointIndexMap_Old_To_New[PointIndex_old];
			if (PointIndex_new >= 0)
			{
				PointSet_new.Append(PointIndex_new);
			}
		}
	}

	for (int_max k = 0; k < m_MeshData->EdgeSetList.GetLength(); ++k)
	{
		auto& EdgeSet_new = m_MeshData->EdgeSetList[k];
		auto EdgeSet_old = EdgeSet_new;
		EdgeSet_new.Clear();
		for (int_max n = 0; n < EdgeSet_old.GetLength(); ++n)
		{
			auto EdgeIndex_old = EdgeSet_old[n];
			auto EdgeIndex_new = EdgeIndexMap_Old_To_New[EdgeIndex_old];
			if (EdgeIndex_new >= 0)
			{
				EdgeSet_new.Append(EdgeIndex_new);
			}
		}		
	}

	for (int_max k = 0; k < m_MeshData->FaceSetList.GetLength(); ++k)
	{
		auto& FaceSet_new = m_MeshData->FaceSetList[k];
		auto FaceSet_old = FaceSet_new;
		FaceSet_new.Clear();
		for (int_max n = 0; n < FaceSet_old.GetLength(); ++n)
		{
			auto FaceIndex_old = FaceSet_old[n];
			auto FaceIndex_new = FaceIndexMap_Old_To_New[FaceIndex_old];
			if (FaceIndex_new >= 0)
			{
				FaceSet_new.Append(FaceIndex_new);
			}
		}
	}

	for (int_max Index = 0; Index < m_MeshData->PointDataSet.GetLength(); ++Index)
	{
		auto DataSet_old = m_MeshData->PointDataSet[Index];
		auto ScalarCountPerPoint = DataSet_old.GetRowCount();
		auto& DataSet_new = m_MeshData->PointDataSet[Index];
		DataSet_new.Clear();
		DataSet_new.Resize(ScalarCountPerPoint, m_MeshData->PointList.GetLength());
		for (int_max PointIndex_old = 0; PointIndex_old < PointIndexMap_Old_To_New.GetLength(); ++PointIndex_old)
		{
			auto PointIndex_new = PointIndexMap_Old_To_New[PointIndex_old];
			if (PointIndex_new >= 0)
			{
				DataSet_new.SetCol(PointIndex_new, DataSet_old.GetPointerOfCol(PointIndex_old));
			}
		}
	}

	for (int_max Index = 0; Index < m_MeshData->EdgeDataSet.GetLength(); ++Index)
	{
		auto DataSet_old = m_MeshData->EdgeDataSet[Index];
		auto ScalarCountPerEdge = DataSet_old.GetRowCount();
		auto& DataSet_new = m_MeshData->EdgeDataSet[Index];
		DataSet_new.Clear();
		DataSet_new.Resize(ScalarCountPerEdge, m_MeshData->EdgeList.GetLength());
		for (int_max EdgeIndex_old = 0; EdgeIndex_old < EdgeIndexMap_Old_To_New.GetLength(); ++EdgeIndex_old)
		{
			auto EdgeIndex_new = EdgeIndexMap_Old_To_New[EdgeIndex_old];
			if (EdgeIndex_new >= 0)
			{
				DataSet_new.SetCol(EdgeIndex_new, DataSet_old.GetPointerOfCol(EdgeIndex_old));
			}
		}
	}

	for (int_max Index = 0; Index < m_MeshData->FaceDataSet.GetLength(); ++Index)
	{
		auto DataSet_old = m_MeshData->FaceDataSet[Index];
		auto ScalarCountPerFace = DataSet_old.GetRowCount();
		auto& DataSet_new = m_MeshData->FaceDataSet[Index];
		DataSet_new.Clear();
		DataSet_new.Resize(ScalarCountPerFace, m_MeshData->FaceList.GetLength());
		for (int_max FaceIndex_old = 0; FaceIndex_old < FaceIndexMap_Old_To_New.GetLength(); ++FaceIndex_old)
		{
			auto FaceIndex_new = FaceIndexMap_Old_To_New[FaceIndex_old];
			if (FaceIndex_new >= 0)
			{
				DataSet_new.SetCol(FaceIndex_new, DataSet_old.GetPointerOfCol(FaceIndex_old));
			}
		}
	}
}


template<typename ScalarType>
bool PolygonMesh<ScalarType>::Check_If_DataStructure_is_Clean() const
{
	auto Count1 = this->GetDeletedPointCount();
	auto Count2 = this->GetDeletedEdgeCount();
	auto Count3 = this->GetDeletedFaceCount();
	return (Count1 + Count2 + Count3 == 0);
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetDeletedPointCount() const
{
	auto ValidPointCount = this->GetPointCount();
	auto PointCountOfList = m_MeshData->PointList.GetLength();
	return  PointCountOfList - ValidPointCount;
}


template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetDeletedEdgeCount() const
{
	auto ValidEdgeCount = this->GetEdgeCount();
	auto EdgeCountOfList = m_MeshData->EdgeList.GetLength();
	return  EdgeCountOfList - ValidEdgeCount;
}

template<typename ScalarType>
int_max PolygonMesh<ScalarType>::GetDeletedFaceCount() const
{
	auto ValidFaceCount = this->GetFaceCount();
	auto FaceCountOfList = m_MeshData->FaceList.GetLength();
	return  FaceCountOfList - ValidFaceCount;
}

//------------ Construct from input data ------------------------------------//

template<typename ScalarType>
void PolygonMesh<ScalarType>::Construct(DenseMatrix<ScalarType> InputPointPositionMatrix, const ObjectArray<DenseVector<int_max>>& InputFaceTable)
{
	if (InputPointPositionMatrix.IsEmpty() == true || InputFaceTable.IsEmpty() == true)
	{
		MDK_Error("InputPointPositionMatrix or InputFaceTable is empty @ PolygonMesh::Construct(...)")
		return;
	}

	if (InputPointPositionMatrix.GetRowCount() != 3 || 3 * InputFaceTable.GetElementCount() < InputPointPositionMatrix.GetColCount())
	{
		MDK_Error("InputPointPositionMatrix or InputFaceTable is invalid @ PolygonMesh::Construct(...)")
		return;
	}
	//--------------------------------------------------------------------------------------------------
	this->Clear(); // attribute will get lost

	auto PointIndexList = this->AddPoint_batch(std::move(InputPointPositionMatrix));

	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		if (this->IsValidPointIndex(PointIndexList[k]) == false)
		{
			MDK_Error("Somthing is wrong with PointIndexList @ PolygonMesh::Construct(...)")
			return;
		}
	}

	for (int_max k = 0; k < InputFaceTable.GetLength(); ++k)
	{
		auto PointIndexList_k = PointIndexList.GetSubSet(InputFaceTable[k]);
		if (PointIndexList_k.IsEmpty() == false)
		{
			this->AddFaceByPoint(PointIndexList_k);
		}
		else
		{
			MDK_Warning("PointIndexList is empty at Face " << k << ", the Face is ignored")
		}
	}
}


template<typename ScalarType>
inline
std::pair<DenseMatrix<ScalarType>, ObjectArray<DenseVector<int_max>>>
PolygonMesh<ScalarType>::GetPointPositionMatrixAndFaceTable() const
{
	std::pair<DenseMatrix<ScalarType>, ObjectArray<DenseVector<int_max>>> Output;
	this->GetPointPositionMatrixAndFaceTable(Output.first, Output.second);
	return Output;
}


template<typename ScalarType>
inline
void PolygonMesh<ScalarType>::GetPointPositionMatrixAndFaceTable(DenseMatrix<ScalarType>& PointPositionMatrix, ObjectArray<DenseVector<int_max>>& FaceTable) const
{
	auto PointCount = this->GetPointCount();
	auto FaceCount = this->GetFaceCount();
	int_max PointIndex_max = this->GetMaxValueOfPointIndex();
	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();

	PointPositionMatrix.FastResize(3, PointCount);
	FaceTable.FastResize(FaceCount);

	// DataStructure may not be clean
	// Map PointIndex (PointIndex.GetIndex()) to OutputIndex (col index) in PointPositionMatrix
	std::unordered_map<int_max, int_max> Map_PointIndex_to_OutputIndex;

	int_max PointCounter = 0;
	for (int_max k = 0; k <= PointIndex_max; ++k)
	{
		if (this->IsValidPointIndex(k) == true)
		{
			auto Pos = this->GetPointPosition(k);
			PointPositionMatrix.SetCol(PointCounter, Pos);
			Map_PointIndex_to_OutputIndex[k] = PointCounter;
			PointCounter += 1;
		}
	}

	int_max FaceCounter = 0;
	for (int_max n=0; n<= FaceIndex_max; ++n)
	{
		if (this->IsValidFaceIndex(n) == true)
		{
			auto PointIndexList = this->Face(n).GetPointIndexList();
			FaceTable[FaceCounter].FastResize(PointIndexList.GetLength());
			for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
			{
				auto tempPointIndex = PointIndexList[k];
				auto it_map = Map_PointIndex_to_OutputIndex.find(tempPointIndex);
				if (it_map != Map_PointIndex_to_OutputIndex.end())
				{
					FaceTable[FaceCounter][k] = it_map->second;
				}
				else
				{
					MDK_Error("tempPointIndex is invalid @ PolygonMesh::GetPointPositionMatrixAndFaceTable(...)")
					return;
				}
			}
			FaceCounter += 1;
		}
	}
}

template<typename ScalarType>
bool PolygonMesh<ScalarType>::CheckIfTriangleMesh() const
{
	if (this->IsEmpty() == true)
	{
		return false;
	}

	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();
	for (int_max k=0; k<= FaceIndex_max; ++k)
	{
		if (this->IsValidFaceIndex(k) == true)
		{
			auto PointCount = this->Face(k).GetPointCount();
			if (PointCount != 3)
			{
				return false;
			}
		}
	}

	return true;
}

template<typename ScalarType>
bool PolygonMesh<ScalarType>::CheckIfQuadMesh() const
{
	if (this->IsEmpty() == true)
	{
		return false;
	}

	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();
	for (int_max k = 0; k <= FaceIndex_max; ++k)
	{
		if (this->IsValidFaceIndex(k) == true)
		{
			auto PointCount = this->Face(k).GetPointCount();
			if (PointCount != 4)
			{
				return false;
			}
		}
	}

	return true;
}

template<typename ScalarType>
bool PolygonMesh<ScalarType>::CheckIfMixedTriangleQuadMesh() const
{
	if (this->IsEmpty() == true)
	{
		return false;
	}

	int_max FaceIndex_max = this->GetMaxValueOfFaceIndex();
	for (int_max k = 0; k <= FaceIndex_max; ++k)
	{
		if (this->IsValidFaceIndex(k) == true)
		{
			auto PointCount = this->Face(k).GetPointCount();
			if (PointCount != 3 && PointCount != 4)
			{
				return false;
			}
		}
	}

	return true;
}


template<typename ScalarType>
std::pair<DenseVector<int_max>, DenseVector<int_max>> PolygonMesh<ScalarType>::GetNeighborPointOfPoint(int_max PointIndex_ref, int_max MaxGraphDistance) const
{
	std::pair<DenseVector<int_max>, DenseVector<int_max>> Output;
	//Output.first[k] is PointIndex
	//Output.second[k] is GraphDistance between Point(PointIndex) and Point(PointIndex_ref)

	int_max PointCount = this->GetPointCount();

	//--------------- check input ------------------------//
	if (PointIndex_ref < 0 || PointIndex_ref >= PointCount)
	{
		return Output;
	}

	if (this->IsEmpty() == true)
	{
		return Output;
	}

	if (MaxGraphDistance <= 0)
	{
		return Output;
	}
	else if (MaxGraphDistance == 1)
	{
		auto AdjPointIndexList = this->Point(PointIndex_ref).GetAdjacentPointIndexList();
		Output.first.Resize(AdjPointIndexList.GetLength());
		Output.second.Resize(AdjPointIndexList.GetLength());
		for (int_max k = 0; k < AdjPointIndexList.GetLength(); ++k)
		{
			Output.first[k] = AdjPointIndexList[k];
			Output.second[k] = 1;
			return Output;
		}
	}
	//----------------------------------------------------//

	DenseVector<int8> PointFlagList;
	PointFlagList.Resize(PointCount);
	PointFlagList.Fill(0); // 0 ~ not selected, 1 ~ selected as ouput

	DenseVector<int_max> PointIndexList_boarder;
	PointIndexList_boarder = this->Point(PointIndex_ref).GetAdjacentPointIndexList();

	for (int_max k = 0; k < PointIndexList_boarder.GetLength(); ++k)
	{
		Output.first.Append(PointIndexList_boarder[k]);
		Output.second.Append(1);
		PointFlagList[PointIndexList_boarder[k]] = 1;
	}

	for (int_max GraphDistance = 2; GraphDistance <= MaxGraphDistance; ++GraphDistance)
	{
		DenseVector<int_max> PointIndexList_boarder_next;
		PointIndexList_boarder_next.SetCapacity(PointIndexList_boarder.GetLength() + 100);
		for (int_max k = 0; k < PointIndexList_boarder.GetLength(); ++k)
		{
			auto AdjPointIndexList = this->Point(PointIndexList_boarder[k]).GetAdjacentPointIndexList();
			for (int_max n = 0; n < AdjPointIndexList.GetLength(); ++n)
			{
				if (PointFlagList[AdjPointIndexList[n]] == 0)
				{
					Output.first.Append(AdjPointIndexList[k]);
					Output.second.Append(GraphDistance);					
					PointIndexList_boarder_next.Append(AdjPointIndexList[n]);
					PointFlagList[AdjPointIndexList[n]] = 1;
				}
			}
		}
		PointIndexList_boarder = std::move(PointIndexList_boarder_next);
	}

	return Output;
}

//-------------------- get a sub mesh by FaceIndexList  -----------------------------------------//

template<typename ScalarType>
PolygonMesh<ScalarType> 
PolygonMesh<ScalarType>::GetSubMeshByFace(const DenseVector<int_max>& FaceIndexList) const
{
    PolygonMesh<ScalarType> OutputMesh;
    
    if (FaceIndexList.IsEmpty() == true)
    {
        return OutputMesh;
    }

    if (this->IsEmpty() == true)
    {
        MDK_Error("Self is empty @ PolygonMesh::GetSubMeshByFace(...)")
        return OutputMesh;
    }

    for (int_max k = 0; k < FaceIndexList.GetLength(); ++k)
    {
        if (this->IsValidFaceIndex(FaceIndexList[k]) == false)
        {
            MDK_Error("FaceIndexList is invalid @ PolygonMesh::GetSubMeshByFace(...)")
            return OutputMesh;
        }
    }
    //-----------------------------------------------------------

    // point index on this mesh -> point index on output mesh
	DenseVector<int_max> Map_PointIndex_OuputIndex;
	Map_PointIndex_OuputIndex.Resize(m_MeshData->PointList.GetLength());
	Map_PointIndex_OuputIndex.Fill(-1);
    
	// edge index on this mesh -> edge index on output mesh	
	DenseVector<int_max> Map_EdgeIndex_OuputIndex;
	Map_EdgeIndex_OuputIndex.Resize(m_MeshData->EdgeList.GetLength());
	Map_EdgeIndex_OuputIndex.Fill(-1);

    // add face one by one
    for (int_max k = 0; k < FaceIndexList.GetLength(); ++k)
    {
        auto FaceID_k = this->Face(FaceIndexList[k]).GetID();
		auto FaceName_k= this->Face(FaceIndexList[k]).GetName();
        const auto& FaceAttribute_k = this->Face(FaceIndexList[k]).Attribute();
        auto PointIndexList_k = this->Face(FaceIndexList[k]).GetPointIndexList();
        auto EdgeIndexList_k = this->Face(FaceIndexList[k]).GetEdgeIndexList();

        // add point
        DenseVector<int_max> PointIndexList_OutputFace;
		PointIndexList_OutputFace.Resize(PointIndexList_k.GetLength());
        for (int_max n = 0; n < PointIndexList_k.GetLength(); ++n)
        {
            auto PointIndex_n = PointIndexList_k[n];
            auto PointID_n = this->Point(PointIndex_n).GetID();
			auto PointName_n = this->Point(PointIndex_n).GetName();
            const auto& PointAttribute_n = this->Point(PointIndex_n).Attribute();
			auto Position_n = this->Point(PointIndex_n).GetPosition();

            // check if the point has already been added to OutputMesh			
			if (Map_PointIndex_OuputIndex[PointIndex_n] < 0)
            {        
                // add Point Position
				PointIndexList_OutputFace[n] = OutputMesh.AddPoint(Position_n);
                // copy PointID if it is valid
                if (PointID_n >= 0)
                {
					OutputMesh.Point(PointIndexList_OutputFace[n]).SetID(PointID_n);
                }
				// copy PointName if it is valid
				if (PointName_n.IsEmpty() == false)
				{
					OutputMesh.Point(PointIndexList_OutputFace[n]).SetName(PointName_n);
				}
                // copy Attribute
				OutputMesh.Point(PointIndexList_OutputFace[n]).Attribute() = PointAttribute_n;
                // add point to map
				Map_PointIndex_OuputIndex[PointIndex_n] = PointIndexList_OutputFace[n];
            }
            else
            {
				PointIndexList_OutputFace[n] = Map_PointIndex_OuputIndex[PointIndex_n];
            }
        }

        // add face, FaceIndex_Output is k
		auto FaceIndex_Output = OutputMesh.AddFaceByPoint(PointIndexList_OutputFace);
        // copy face ID if it is valid
        if (FaceID_k >= 0)
        {
			OutputMesh.Face(FaceIndex_Output).SetID(FaceID_k);
        }
		// copy face Name if it is valid
		if (FaceName_k.IsEmpty() == false)
		{
			OutputMesh.Face(FaceIndex_Output).SetName(FaceName_k);
		}
        // copy Attribute
		OutputMesh.Face(FaceIndex_Output).Attribute() = FaceAttribute_k;

        // copy Edge ID, Name and Attribute
		auto EdgeIndexList_Output = OutputMesh.Face(FaceIndex_Output).GetEdgeIndexList();
        for (int_max n = 0; n < EdgeIndexList_k.GetLength(); ++n)
        {
            // check if the edge has already been added to OutputMesh			
			if (Map_EdgeIndex_OuputIndex[EdgeIndexList_k[n]] < 0)
            {
                auto EdgeID_n = this->Edge(EdgeIndexList_k[n]).GetID();             
                if (EdgeID_n >= 0)
                {
					OutputMesh.Edge(EdgeIndexList_Output[n]).SetID(EdgeID_n);
                }

				auto EdgeName_n = this->Edge(EdgeIndexList_k[n]).GetName();
				if (EdgeName_n.IsEmpty() == false)
				{
					OutputMesh.Edge(EdgeIndexList_Output[n]).SetName(EdgeName_n);
				}

				const auto& EdgeAttribute_n = this->Edge(EdgeIndexList_k[n]).Attribute();
				OutputMesh.Edge(EdgeIndexList_Output[n]).Attribute() = EdgeAttribute_n;

				Map_EdgeIndex_OuputIndex[EdgeIndexList_k[n]] = EdgeIndexList_Output[n];
            }
        }
    }

	//add PointDataSet, EdgeDataSet, FaceDataset	
	for (int_max Index = 0; Index < this->GetPointDataSetCount(); ++Index)
	{
		auto Name = this->GetPointDataSetName(Index);
		auto tempData = this->Point(0).GetData(Index);
		auto ScalarCountPerPoint = tempData.GetLength();
		OutputMesh.SetPointDataSet(Name, ScalarCountPerPoint);
		for (int_max k = 0; k < Map_PointIndex_OuputIndex.GetLength(); ++k)
		{
			if (Map_PointIndex_OuputIndex[k] >= 0)
			{
				auto OuputIndex = Map_PointIndex_OuputIndex[k];
				auto Data = this->Point(k).GetData(Index);
				OutputMesh.Point(OuputIndex).SetData(Name, Data);
			}
		}
	}

	for (int_max Index = 0; Index < this->GetEdgeDataSetCount(); ++Index)
	{
		auto Name = this->GetEdgeDataSetName(Index);
		auto tempData = this->Edge(0).GetData(Index);
		auto ScalarCountPerEdge = tempData.GetLength();
		OutputMesh.SetEdgeDataSet(Name, ScalarCountPerEdge);
		for (int_max k = 0; k < Map_EdgeIndex_OuputIndex.GetLength(); ++k)
		{
			if (Map_EdgeIndex_OuputIndex[k] >= 0)
			{
				auto OuputIndex = Map_EdgeIndex_OuputIndex[k];
				auto Data = this->Edge(k).GetData(Index);
				OutputMesh.Edge(OuputIndex).SetData(Name, Data);
			}
		}
	}

	for (int_max Index = 0; Index < this->GetFaceDataSetCount(); ++Index)
	{
		auto Name = this->GetFaceDataSetName(Index);
		auto tempData = this->Face(0).GetData(Index);
		auto ScalarCountPerFace = tempData.GetLength();
		OutputMesh.SetFaceDataSet(Name, ScalarCountPerFace);
		for (int_max k = 0; k < FaceIndexList.GetLength(); ++k)
		{
			auto Data = this->Face(FaceIndexList[k]).GetData(Index);
			OutputMesh.Face(k).SetData(Name, Data);
		}
	}

    return OutputMesh;
}


template<typename ScalarType>
void PolygonMesh<ScalarType>::Append(const PolygonMesh<ScalarType>& InputMesh)
{// append a mesh, duplication of face/edge/point will not be checked
// this is not checked: append non-triangle mesh to triangle mesh
    //prevent self append
	if (&m_MeshData == &InputMesh.m_MeshData)
	{
		return;
	}

	if (this->IsPureEmpty() == true)
	{
		this->Recreate();
	}

	if (this->IsEmpty() == true)
	{
		if (InputMesh.IsEmpty() == false)
		{
			this->Copy(InputMesh);
		}
		return;
	}

	//no change on ID and Name
	//m_MeshData->ID
	//m_MeshData->Name

	auto MaxPointIndex_init = m_MeshData->PointList.GetLength() - 1;
	auto MaxEdgeIndex_init = m_MeshData->EdgeList.GetLength() - 1;
	auto MaxFaceIndex_init = m_MeshData->FaceList.GetLength() - 1;

	m_MeshData->PointPositionTable = { &m_MeshData->PointPositionTable, &InputMesh.m_MeshData->PointPositionTable };
	m_MeshData->PointValidityFlagList.Append(InputMesh.m_MeshData->PointValidityFlagList);
	m_MeshData->PointList.Insert(MaxPointIndex_init+1, InputMesh.m_MeshData->PointList);
	for (int_max k = 0; k < m_MeshData->PointList.GetLength(); ++k)
	{
		if (k > MaxPointIndex_init)
		{
			if (m_MeshData->PointValidityFlagList[k] == 1)
			{
				m_MeshData->PointList[k].SetParentMesh(*this);
				m_MeshData->PointList[k].SetIndex(k);
				m_MeshData->PointList[k].AdjacentEdgeIndexList() += MaxEdgeIndex_init + 1;
			}
		}
	}

	m_MeshData->EdgeValidityFlagList.Append(InputMesh.m_MeshData->EdgeValidityFlagList);
	m_MeshData->EdgeList.Insert(MaxEdgeIndex_init + 1, InputMesh.m_MeshData->EdgeList);
	for (int_max k = 0; k < m_MeshData->EdgeList.GetLength(); ++k)
	{		
		if (k > MaxEdgeIndex_init)
		{
			if (m_MeshData->EdgeValidityFlagList[k] == 1)
			{
				m_MeshData->EdgeList[k].SetParentMesh(*this);
				m_MeshData->EdgeList[k].SetIndex(k);
				auto PIdxList = m_MeshData->EdgeList[k].GetPointIndexList();
				m_MeshData->EdgeList[k].SetPointIndexList(PIdxList[0] + MaxPointIndex_init + 1, PIdxList[1] + MaxPointIndex_init + 1);
				m_MeshData->EdgeList[k].AdjacentFaceIndexList() += MaxFaceIndex_init + 1;
			}
		}
	}

	m_MeshData->FaceValidityFlagList.Append(InputMesh.m_MeshData->FaceValidityFlagList);
	m_MeshData->FaceList.Insert(MaxFaceIndex_init + 1, InputMesh.m_MeshData->FaceList);
	for (int_max k = 0; k < m_MeshData->FaceList.GetLength(); ++k)
	{		
		if (k > MaxFaceIndex_init)
		{
			if (m_MeshData->FaceValidityFlagList[k] == 1)
			{
				m_MeshData->FaceList[k].SetParentMesh(*this);
				m_MeshData->FaceList[k].SetIndex(k);
				m_MeshData->FaceList[k].PointIndexList() += MaxPointIndex_init + 1;
				m_MeshData->FaceList[k].EdgeIndexList() += MaxEdgeIndex_init + 1;
			}
		}
	}

	//no change --------------------------------------
	//m_MeshData->Map_Point_ID_to_Index
	//m_MeshData->Map_Edge_ID_to_Index
	//m_MeshData->Map_Face_ID_to_Index
	//
	//m_MeshData->Map_Point_Name_to_Index
	//m_MeshData->Map_Edge_Name_to_Index
	//m_MeshData->Map_Face_Name_to_Index
	//
	//m_MeshData->PointSetList
	//m_MeshData->Map_PointSet_Name_to_Index
	//
	//m_MeshData->EdgeSetList
	//m_MeshData->Map_EdgeSet_Name_to_Index
	//
	//m_MeshData->FaceSetList
	//m_MeshData->Map_FaceSet_Name_to_Index
	//	
	//m_MeshData->Attribute
}


// other basic operation ----------------------------------------------------------------------------------------

template<typename ScalarType>
bool PolygonMesh<ScalarType>::SwapPoint(int_max PointIndexA, int_max PointIndexB)
{
	if (PointIndexA == PointIndexB)
	{
		return true;
	}

	if (this->IsValidPointIndex(PointIndexA) == false || this->IsValidPointIndex(PointIndexB) == false)
	{
		MDK_Error("Invalid PointIndex @ PolygonMesh::SwapPoint(...)")
		return false;
	}

	//swap Position
	auto PositionA = m_MeshData->PointList[PointIndexA].GetPosition();
	auto PositionB = m_MeshData->PointList[PointIndexB].GetPosition();
	m_MeshData->PointList[PointIndexA].SetPosition(PositionB);
	m_MeshData->PointList[PointIndexB].SetPosition(PositionA);

	return this->SwapConnectivityOfPoint(PointIndexA, PointIndexB);
}


template<typename ScalarType>
bool PolygonMesh<ScalarType>::SwapConnectivityOfPoint(int_max PointIndexA, int_max PointIndexB)
{
	if (PointIndexA == PointIndexB)
	{
		return true;
	}

	if (this->IsValidPointIndex(PointIndexA) == false || this->IsValidPointIndex(PointIndexB) == false)
	{
		MDK_Error("Invalid PointIndex @ PolygonMesh::SwapConnectivityOfPoint(...)")
		return false;
	}

	const auto& AdjacentPointIndexListA = m_MeshData->PointList[PointIndexA].GetAdjacentPointIndexList();
	const auto& AdjacentFaceIndexListA = m_MeshData->PointList[PointIndexA].GetAdjacentFaceIndexList();
	//to be updated
	auto& AdjacentEdgeIndexListA = m_MeshData->PointList[PointIndexA].AdjacentEdgeIndexList();

	const auto& AdjacentPointIndexListB = m_MeshData->PointList[PointIndexB].GetAdjacentPointIndexList();
	const auto& AdjacentFaceIndexListB = m_MeshData->PointList[PointIndexB].GetAdjacentFaceIndexList();
	//to be updated
	auto& AdjacentEdgeIndexListB = m_MeshData->PointList[PointIndexB].AdjacentEdgeIndexList();

	//---------------- update adjacent point, edge ----------------------------------------------------

	auto tempPointIndexA = m_MeshData->PointList.GetLength();

	//change PointIndexA -> tempPointIndexA
	for (int_max k = 0; k < AdjacentEdgeIndexListA.GetLength(); ++k)
	{
		auto PointIndexList_k = m_MeshData->EdgeList[AdjacentEdgeIndexListA[k]].GetPointIndexList();
		if (PointIndexList_k[0] == PointIndexA)
		{
			m_MeshData->EdgeList[AdjacentEdgeIndexListA[k]].SetPointIndexList(tempPointIndexA, PointIndexList_k[1]);
		}
		else
		{
			m_MeshData->EdgeList[AdjacentEdgeIndexListA[k]].SetPointIndexList(PointIndexList_k[0], tempPointIndexA);
		}
	}

	//change  PointIndexB -> PointIndexA
	for (int_max k = 0; k < AdjacentEdgeIndexListB.GetLength(); ++k)
	{
		if (AdjacentPointIndexListB[k] != tempPointIndexA)
		{
			auto PointIndexList_k = m_MeshData->EdgeList[AdjacentEdgeIndexListB[k]].GetPointIndexList();
			if (PointIndexList_k[0] == PointIndexB)
			{
				m_MeshData->EdgeList[AdjacentEdgeIndexListB[k]].SetPointIndexList(PointIndexA, PointIndexList_k[1]);
			}
			else
			{
				m_MeshData->EdgeList[AdjacentEdgeIndexListB[k]].SetPointIndexList(PointIndexList_k[0], PointIndexA);
			}
		}
	}

	//change tempPointIndexA -> PointIndexB
	for (int_max k = 0; k < AdjacentEdgeIndexListA.GetLength(); ++k)
	{
		auto PointIndexList_k = m_MeshData->EdgeList[AdjacentEdgeIndexListA[k]].GetPointIndexList();
		if (PointIndexList_k[0] == tempPointIndexA)
		{
			m_MeshData->EdgeList[AdjacentEdgeIndexListA[k]].SetPointIndexList(PointIndexB, PointIndexList_k[1]);
		}
		else
		{
			m_MeshData->EdgeList[AdjacentEdgeIndexListA[k]].SetPointIndexList(PointIndexList_k[0], PointIndexB);
		}
	}

	//change PointIndexA -> PointIndexB for Face	
	for (int_max k = 0; k < AdjacentFaceIndexListA.GetLength(); ++k)
	{   //take reference
		auto& PointIndexList_k = m_MeshData->FaceList[AdjacentFaceIndexListA[k]].PointIndexList();
		int_max TempIndex = PointIndexList_k.ExactMatch("first", PointIndexA);
		PointIndexList_k[TempIndex] = PointIndexB;
	}

	//change PointIndexB -> PointIndexA for Face	
	for (int_max k = 0; k < AdjacentFaceIndexListB.GetLength(); ++k)
	{   //take reference
		auto& PointIndexList_k = m_MeshData->FaceList[AdjacentFaceIndexListB[k]].PointIndexList();
		int_max TempIndex = PointIndexList_k.ExactMatch("first", PointIndexB);
		PointIndexList_k[TempIndex] = PointIndexA;
	}

	//swap AdjacentEdgeIndexList
	{
		auto tempList = AdjacentEdgeIndexListA;
		AdjacentEdgeIndexListA = AdjacentEdgeIndexListB;
		AdjacentEdgeIndexListB = tempList;
	}

	//----------
	return true;
}


template<typename ScalarType>
bool PolygonMesh<ScalarType>::MergeConnectivityOfPoint(int_max PointIndexA, int_max PointIndexB, bool Flag_CheckTriangle)
{// merge connection of B to connection of A, B become isolated
	//-------------------------
	//   \         /
	// ---A-------B---
	//   /         \
	//-------------------------
	//   \ /    
	// ---A---   (B)
	//   / \     
    //-------------------------

	if (PointIndexA == PointIndexB)
	{
		return true;
	}

	if (this->IsValidPointIndex(PointIndexA) == false || this->IsValidPointIndex(PointIndexB) == false)
	{
		MDK_Error("Invalid PointIndex @ PolygonMesh::MergeConnectivityOfPoint(...)")		
		return false;
	}
	//triangle check
	if (Flag_CheckTriangle == true)
	{// A and B may belong to the same triangle face
		auto AdjacentPointIndexListA = m_MeshData->PointList[PointIndexA].GetAdjacentPointIndexList();
		auto AdjacentPointIndexListB = m_MeshData->PointList[PointIndexB].GetAdjacentPointIndexList();
		auto AdjacentPointIndexListAB = Intersect(AdjacentPointIndexListA, AdjacentPointIndexListB);
		if (AdjacentPointIndexListAB.IsEmpty() == false)
		{
			MDK_Error("Can not merge: triangle detected @ PolygonMesh::MergeConnectivityOfPoint(...)")
			return false;
		}
	}

	//------- delete Edge AB if it exist --------------------------------------------------------------------

	auto EdgeIndex_AB = this->GetEdgeIndexByPoint(PointIndexA, PointIndexB);
	if (EdgeIndex_AB >= 0)
	{
		auto AdjacentFaceIndexList_AB = m_MeshData->EdgeList[EdgeIndex_AB].GetAdjacentFaceIndexList();
		for (int_max k = 0; k < AdjacentFaceIndexList_AB.GetLength(); ++k)
		{
			auto FaceIndex_k = AdjacentFaceIndexList_AB[k];
			auto PointCount_k = m_MeshData->FaceList[FaceIndex_k].GetPointCount();
			// remove EdgeAB and PointB in the Face
			//take reference
			auto& EdgeIndexList_k = m_MeshData->FaceList[FaceIndex_k].EdgeIndexList();
			auto tempIndex_edge = EdgeIndexList_k.ExactMatch("first", EdgeIndex_AB);
			EdgeIndexList_k.Delete(tempIndex_edge);
			//take reference
			auto& PointIndexList_k = m_MeshData->FaceList[FaceIndex_k].PointIndexList();
			auto tempIndex_point = PointIndexList_k.ExactMatch("first", PointIndexB);
			PointIndexList_k.Delete(tempIndex_point);

		}
		// delete edge AB
		this->UpdateRecord_DeleteEdge(EdgeIndex_AB);
	}

	//-------------------------- Merge connection of B to connection of A ------------------------------------

	//update face adjacent to B
	auto AdjacentFaceIndexListB= m_MeshData->PointList[PointIndexB].GetAdjacentFaceIndexList();
	for (int_max k = 0; k < AdjacentFaceIndexListB.GetLength(); ++k)
	{
		auto FaceIndex_k = AdjacentFaceIndexListB[k];
		//take reference
		auto& PointIndexList_k = m_MeshData->FaceList[FaceIndex_k].PointIndexList();
		auto tempIndex = PointIndexList_k.ExactMatch("first", PointIndexB);
		if (tempIndex >= 0)
		{
			PointIndexList_k[tempIndex] = PointIndexA;
		}
	}

	//update edge adjacent to B
	const auto& AdjacentEdgeIndexListB = m_MeshData->PointList[PointIndexB].AdjacentEdgeIndexList();	
	for (int_max k = 0; k < AdjacentEdgeIndexListB.GetLength(); ++k)
	{
		auto PointIndexList_k = m_MeshData->EdgeList[AdjacentEdgeIndexListB[k]].GetPointIndexList();
		if (PointIndexList_k[0] == PointIndexB)
		{
			m_MeshData->EdgeList[AdjacentEdgeIndexListB[k]].SetPointIndexList(PointIndexA, PointIndexList_k[1]);
		}
		else
		{
			m_MeshData->EdgeList[AdjacentEdgeIndexListB[k]].SetPointIndexList(PointIndexList_k[0], PointIndexA);
		}
	}

	//update adjacency info of A
	auto& AdjacentEdgeIndexListA = m_MeshData->PointList[PointIndexA].AdjacentEdgeIndexList();//to be updated
	AdjacentEdgeIndexListA = { &AdjacentEdgeIndexListA, &AdjacentEdgeIndexListB };
	auto tempIndexList_edge_delete = AdjacentEdgeIndexListA.ExactMatch(EdgeIndex_AB);
	AdjacentEdgeIndexListA.Delete(tempIndexList_edge_delete);

	//delete any adjacency info of B
	m_MeshData->PointList[PointIndexB].AdjacentEdgeIndexList().Clear();

	//---------------------------------------------------------------	
	return true;
}


template<typename ScalarType>
bool PolygonMesh<ScalarType>::ShrinkEdgeToPoint(int_max EdgeIndex, int_max PointIndex)
{
	if (this->IsValidEdgeIndex(EdgeIndex) == false)
	{
		MDK_Error("Invalid EdgeIndex @ PolygonMesh::ShrinkEdgeToPoint(...)")		
		return false;
	}

	if (this->IsValidPointIndex(PointIndex) == false)
	{
		MDK_Error("Invalid PointIndex @ PolygonMesh::ShrinkEdgeToPoint(...)")		
		return false;
	}

	auto PointIndexList = m_MeshData->EdgeList[EdgeIndex].GetPointIndexList();
	if (PointIndexList[0] == PointIndex)
	{
		this->MergeConnectivityOfPoint(PointIndexList[0], PointIndexList[1]);
	}
	else if (PointIndexList[1] == PointIndex)
	{
		this->MergeConnectivityOfPoint(PointIndexList[1], PointIndexList[0]);
	}
	else
	{
		this->MergeConnectivityOfPoint(PointIndexList[0], PointIndexList[1]);
		this->SwapConnectivityOfPoint(PointIndexList[0], PointIndex);
	}
	//-------------------
	return true;
}


template<typename ScalarType>
DenseVector<int_max, 2> PolygonMesh<ScalarType>::SplitEdgeByPoint(int_max EdgeIndex, int_max PointIndex)
{
	DenseVector<int_max, 2> EdgeIndexPair;
	EdgeIndexPair[0] = -1;
	EdgeIndexPair[1] = -1;

	if (this->IsValidEdgeIndex(EdgeIndex) == false)
	{
		MDK_Error("Invalid EdgeIndex @ PolygonMesh::SplitEdgeByPoint(...)")
		return EdgeIndexPair;
	}

	if (this->IsValidPointIndex(PointIndex) == false)
	{
		MDK_Error("Invalid PointIndex @ PolygonMesh::SplitEdgeByPoint(...)")
		return EdgeIndexPair;
	}

	auto PointIndexList = m_MeshData->EdgeList[EdgeIndex].GetPointIndexList();
	auto PointIndex0 = PointIndexList[0];
	auto PointIndex1 = PointIndexList[1];
	auto PointIndex2 = PointIndex;
	//-------------------------
	//   \          /
    // ---P0---P2--P1---
	//   /          \
    //-------------------------

	if (PointIndex2 == PointIndex0 || PointIndex2 == PointIndex1)
	{
		MDK_Error("Input Point is on Edge  @ PolygonMesh::SplitEdgeByPoint(...)")
		return EdgeIndexPair;
	}

	//------------------------- input check is done --------------------------------------------//

	auto EdgeIndex01 = EdgeIndex;
	auto EdgeIndex02 = this->AddEdge(PointIndex0, PointIndex2);
	auto EdgeIndex21 = this->AddEdge(PointIndex2, PointIndex1);

	auto AdjacentFaceIndexList = m_MeshData->EdgeList[EdgeIndex].GetAdjacentFaceIndexList();
	for (int_max k = 0; k < AdjacentFaceIndexList.GetLength(); ++k)
	{// delete Edge01, add Edge02 Edge21 and Point2 in Face
		auto FaceIndex_k = AdjacentFaceIndexList[k];
		//   ----------            -------------
		//   \  Face  /       to   \    Face    /
		//    Pa-->--Pb             Pa--->P2-->Pb
		//   ------------------------------------------
		//   [Pa, Pb] may be [P0 P1] or [P1 P0];
		//-----------------------------------------------
		int_max PointIndexA, PointIndexB;	
		int_max EdgeIndexA2, EdgeIndex2B;
		auto Flag_order= m_MeshData->FaceList[FaceIndex_k].CheckPointOrder(PointIndex0, PointIndex1);
		if (Flag_order == true)
		{
			PointIndexA = PointIndex0;
			PointIndexB = PointIndex1;
			EdgeIndexA2 = EdgeIndex02;
			EdgeIndex2B = EdgeIndex21;
		}
		else
		{
			PointIndexB = PointIndex0;
			PointIndexA = PointIndex1;
			EdgeIndexA2 = EdgeIndex21;
			EdgeIndex2B = EdgeIndex02;
		}
		//update edge
		{
			//take reference
			auto& EdgeIndexList_k = m_MeshData->FaceList[FaceIndex_k].EdgeIndexList();
			//delete Edge01
			auto tempIndex = EdgeIndexList_k.ExactMatch("first", EdgeIndex01);
			EdgeIndexList_k.Delete(tempIndex);
			//add Edge
			EdgeIndexList_k.Insert(tempIndex, EdgeIndex2B);
			EdgeIndexList_k.Insert(tempIndex, EdgeIndexA2);
		}
		//update point
		{
			// take reference
			auto& PointIndexList_k = m_MeshData->FaceList[FaceIndex_k].PointIndexList();
			//find PointB
			auto tempIndex = PointIndexList_k.ExactMatch("first", PointIndexB);
			//add Point2
			PointIndexList_k.Insert(tempIndex, PointIndex2);
		}
	}

	// delete edge P0P1
	this->UpdateRecord_DeleteEdge(EdgeIndex01);

	//-------------------------------------------------------------------------
	EdgeIndexPair[0] = EdgeIndex02;
	EdgeIndexPair[1] = EdgeIndex21;
	return EdgeIndexPair;
}


template<typename ScalarType>
bool PolygonMesh<ScalarType>::ShrinkFaceToPoint(int_max FaceIndex, int_max PointIndex)
{
	if (this->IsValidFaceIndex(FaceIndex) == false)
	{
		MDK_Error("Invalid FaceIndex @ PolygonMesh::ShrinkFaceToPoint(...)")		
		return false;
	}

	if (this->IsValidPointIndex(PointIndex) == false)
	{
		MDK_Error("Invalid PointIndex @ PolygonMesh::ShrinkFaceToPoint(...)")
		return false;
	}

	auto PointIndexList = m_MeshData->FaceList[FaceIndex].GetPointIndexList();
	//triangle check
	for(int_max k=0; k<PointIndexList.GetLength(); ++k)
	{
		int_max PointIndexA = PointIndexList[k];
		int_max PointIndexB = 0;
		if (k < PointIndexList.GetLength() - 1)
		{
			PointIndexB = k + 1;
		}
		auto AdjacentPointIndexListA = m_MeshData->PointList[PointIndexA].GetAdjacentPointIndexList();
		auto AdjacentPointIndexListB = m_MeshData->PointList[PointIndexB].GetAdjacentPointIndexList();
		auto AdjacentPointIndexListAB = Intersect(AdjacentPointIndexListA, AdjacentPointIndexListB);
		if (AdjacentPointIndexListAB.IsEmpty() == false)
		{
			MDK_Error("Can not shrink: triangle detected @ PolygonMesh::ShrinkFaceToPoint(...)")
			return false;
		}
	}
	auto EdgeIndexList= m_MeshData->FaceList[FaceIndex].GetEdgeIndexList();
	auto AdjacentFaceIndexList = m_MeshData->FaceList[FaceIndex].GetAdjacentFaceIndexList();
	this->DeleteFace(FaceIndex);

	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		int_max PointIndexA, PointIndexB;
		m_MeshData->EdgeList[EdgeIndexList[k]].GetPointIndexList(PointIndexA, PointIndexB);

		auto tempList = m_MeshData->EdgeList[EdgeIndexList[k]].GetAdjacentFaceIndexList();
		for (int_max n = 0; n < tempList.GetLength(); ++n)
		{
			if (tempList[n] != FaceIndex)
			{
				int_max AdjFaceIndex = tempList[n];				
				//take reference
				auto& EdgeIndexList_adj = m_MeshData->FaceList[AdjFaceIndex].EdgeIndexList();
				auto tempIndex_edge = EdgeIndexList_adj.ExactMatch("first", EdgeIndexList[k]);
				EdgeIndexList_adj.Delete(tempIndex_edge);
				
				//take reference
				auto& PointIndexList_adj = m_MeshData->FaceList[AdjFaceIndex].PointIndexList();
				auto tempIndexA = PointIndexList_adj.ExactMatch("first", PointIndexA);
				PointIndexList_adj.Delete(tempIndexA);
				auto tempIndexB = PointIndexList_adj.ExactMatch("first", PointIndexB);					
				PointIndexList_adj[tempIndexB]= PointIndex;				
			}
		}
	}

	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		this->UpdateRecord_DeleteEdge(EdgeIndexList[k]);
	}

	for (int_max k = 0; k < PointIndexList.GetLength(); ++k)
	{
		auto FaceIndexList_adj = m_MeshData->PointList[PointIndexList[k]].GetAdjacentFaceIndexList();
		auto TempFaceIndexList = SetDiff(FaceIndexList_adj, AdjacentFaceIndexList);
		for (int_max n = 0; n < TempFaceIndexList.GetLength(); ++n)
		{
			if (TempFaceIndexList[n] != FaceIndex)
			{
				auto AdjFaceIndex = TempFaceIndexList[n];
				//take reference
				auto& PointIndexList_adj = m_MeshData->FaceList[AdjFaceIndex].PointIndexList();
				auto tempIndex = PointIndexList_adj.ExactMatch("first", PointIndexList[k]);
				PointIndexList_adj[tempIndex] = PointIndex;
			}
		}

		auto EdgeIndexList_adj = m_MeshData->PointList[PointIndexList[k]].GetAdjacentEdgeIndexList();
		for (int_max n = 0; n < EdgeIndexList_adj.GetLength(); ++n)
		{
			auto tempPointIndexList = m_MeshData->EdgeList[EdgeIndexList_adj[n]].GetPointIndexList();
			if (tempPointIndexList[0] == PointIndexList[k])
			{
				m_MeshData->EdgeList[EdgeIndexList_adj[n]].SetPointIndexList(PointIndexList[k], tempPointIndexList[1]);
			}
			else if (tempPointIndexList[1] == PointIndexList[k])
			{
				m_MeshData->EdgeList[EdgeIndexList_adj[n]].SetPointIndexList(tempPointIndexList[0], PointIndexList[k]);
			}
		}
	}
	//--------------------------------
	return true;
}


/*
template<typename ScalarType>
int_max PolygonMesh<ScalarType>::MergeAdjacentFace(int_max FaceIndexA, int_max FaceIndexB)
{

}
*/
/*
template<typename ScalarType>
DenseVector<int_max, 2> PolygonMesh<ScalarType>::SplitFaceByEdge(int_max EdgeIndex)
{
}
*/

template<typename ScalarType>
DenseVector<int_max, 2> PolygonMesh<ScalarType>::SplitFace(int_max FaceIndex, int_max PointIndexA, int_max PointIndexB)
{
	auto EdgeIndex = this->AddEdge(PointIndexA, PointIndexB);
	return this->SplitFaceByEdge(FaceIndex, EdgeIndex);
}


template<typename ScalarType>
DenseVector<int_max, 2> PolygonMesh<ScalarType>::SplitFaceByEdge(int_max FaceIndex, int_max EdgeABIndex)
{
	//------------------------
	//  ---<--A------   
	//  |     |     |
	//  |     |     |
	//  '-----B-->--'
	// 
	// -> show the point order
	//EdgeAB not adjacent to the face
	//-----------------------

	DenseVector<int_max, 2> NewFaceIndexList;
	NewFaceIndexList[0] = -1;
	NewFaceIndexList[1] = -1 ;

	//--------------------- check input --------------------------------------------------
	if (this->IsValidFaceIndex(FaceIndex) == false)
	{
		MDK_Error("Invalid FaceIndex @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}

	if (this->IsValidEdgeIndex(EdgeABIndex) == false)
	{
		MDK_Error("Invalid EdgeIndex @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}

	auto PointCountOftheFace = m_MeshData->FaceList[FaceIndex].GetPointCount();
	if (PointCountOftheFace <= 3)
	{
		MDK_Error("Can not split face with <= 3 point @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}

	//------------------------------------------------------------------------------------
	int_max PointIndexA, PointIndexB;
	m_MeshData->EdgeList[EdgeABIndex].GetPointIndexList(PointIndexA, PointIndexB);

	auto PointIndexList_A = m_MeshData->FaceList[FaceIndex].GetPointIndexList_LeadBy(PointIndexA);
	if (PointIndexList_A.IsEmpty() == true)
	{
		MDK_Error("Edge and Face do not share point-A @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}
	for (int_max k = 1; k < PointIndexList_A.GetLength(); ++k)
	{
		if (PointIndexList_A[k] == PointIndexB)
		{
			PointIndexList_A = PointIndexList_A.GetSubSet(span(0, k));
			break;
		}
	}

	auto PointIndexList_B = m_MeshData->FaceList[FaceIndex].GetPointIndexList_LeadBy(PointIndexB);
	if (PointIndexList_B.IsEmpty() == true)
	{
		MDK_Error("Edge and Face do not share point-B @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}
	for (int_max k = 1; k < PointIndexList_B.GetLength(); ++k)
	{
		if (PointIndexList_B[k] == PointIndexA)
		{
			PointIndexList_B = PointIndexList_B.GetSubSet(span(0, k));
			break;
		}
	}

	if (PointIndexList_A.GetLength() < 3 || PointIndexList_B.GetLength() < 3)
	{
		MDK_Error("less than 3 point for each split face @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}
	
	if (PointIndexList_A.GetLength() == PointCountOftheFace || PointIndexList_B.GetLength() == PointCountOftheFace)
	{
		MDK_Error("Can not split: PointA and PointB are adjacent @ PolygonMesh::SplitFaceByEdge(...)")
		return NewFaceIndexList;
	}

	this->DeleteFace(FaceIndex);
	
	NewFaceIndexList[0] =this->AddFaceByPoint(PointIndexList_A);
	NewFaceIndexList[1] =this->AddFaceByPoint(PointIndexList_B);
	return NewFaceIndexList;
}


//----------------------------------- protected function ---------------------------------------------------------//

template<typename ScalarType>
void PolygonMesh<ScalarType>::UpdateRecord_DeletePoint(int_max PointIndex)
{
	auto ID = m_MeshData->PointList[PointIndex].GetID();
	if (ID >= 0)
	{
		auto it = m_MeshData->Map_Point_ID_to_Index.find(ID);
		if (it != m_MeshData->Map_Point_ID_to_Index.end())
		{
			m_MeshData->Map_Point_ID_to_Index.erase(it);
		}
	}

	auto Name = m_MeshData->PointList[PointIndex].GetName();
	if (Name.IsEmpty() == false)
	{
		auto it = m_MeshData->Map_Point_Name_to_Index.find(Name);
		if (it != m_MeshData->Map_Point_Name_to_Index.end())
		{
			m_MeshData->Map_Point_Name_to_Index.erase(it);
		}
	}

    m_MeshData->PointPositionTable.FillCol(PointIndex, 0);

	m_MeshData->PointList[PointIndex].Clear(MDK_PURE_EMPTY);
    m_MeshData->PointValidityFlagList[PointIndex] = 0;
}


template<typename ScalarType>
void PolygonMesh<ScalarType>::UpdateRecord_DeleteEdge(int_max EdgeIndex)
{
	// update AdjacentEdge information in m_MeshData->PointList
	int_max PointIndex0, PointIndex1;
	m_MeshData->EdgeList[EdgeIndex].GetPointIndexList(PointIndex0, PointIndex1);
    //
	auto tempIndex0 = m_MeshData->PointList[PointIndex0].AdjacentEdgeIndexList().ExactMatch(EdgeIndex);
	m_MeshData->PointList[PointIndex0].AdjacentEdgeIndexList().Delete(tempIndex0);
	//
	auto tempIndex1 = m_MeshData->PointList[PointIndex1].AdjacentEdgeIndexList().ExactMatch(EdgeIndex);
	m_MeshData->PointList[PointIndex1].AdjacentEdgeIndexList().Delete(tempIndex1);

    // Delete EdgeID record if the map has it 
	auto ID = m_MeshData->EdgeList[EdgeIndex].GetID();
	if (ID >= 0)
	{
		auto it = m_MeshData->Map_Edge_ID_to_Index.find(ID);
		if (it != m_MeshData->Map_Edge_ID_to_Index.end())
		{
			m_MeshData->Map_Edge_ID_to_Index.erase(it);
		}
	}

	auto Name = m_MeshData->EdgeList[EdgeIndex].GetName();
	if (Name.IsEmpty() == false)
	{
		auto it = m_MeshData->Map_Edge_Name_to_Index.find(Name);
		if (it != m_MeshData->Map_Edge_Name_to_Index.end())
		{
			m_MeshData->Map_Edge_Name_to_Index.erase(it);
		}
	}

    // Delete Edge: only release memory, not remove from EdgeList
	m_MeshData->EdgeList[EdgeIndex].Clear(MDK_PURE_EMPTY);
    m_MeshData->EdgeValidityFlagList[EdgeIndex] = 0;
}


template<typename ScalarType>
void PolygonMesh<ScalarType>::UpdateRecord_DeleteFace(int_max FaceIndex)
{
	// auto& will get reference
	const auto& EdgeIndexList = m_MeshData->FaceList[FaceIndex].EdgeIndexList();
	const auto& PointIndexList = m_MeshData->FaceList[FaceIndex].PointIndexList();

	// update information in m_MeshData.EdgeList -------------------------------------------------------------
	for (int_max k = 0; k < EdgeIndexList.GetLength(); ++k)
	{
		auto tempIndex = m_MeshData->EdgeList[EdgeIndexList[k]].AdjacentFaceIndexList().ExactMatch(FaceIndex);
		m_MeshData->EdgeList[EdgeIndexList[k]].AdjacentFaceIndexList().Delete(tempIndex);
	}

    // delete ID, Name if the map has it 
	auto ID = m_MeshData->FaceList[FaceIndex].GetID();
	if (ID >= 0)
	{
		auto it = m_MeshData->Map_Face_ID_to_Index.find(ID);
		if (it != m_MeshData->Map_Face_ID_to_Index.end())
		{
			m_MeshData->Map_Face_ID_to_Index.erase(it);
		}
	}
	auto Name = m_MeshData->FaceList[FaceIndex].GetName();
	if (Name.IsEmpty() == false)
	{
		auto it = m_MeshData->Map_Face_Name_to_Index.find(Name);
		if (it != m_MeshData->Map_Face_Name_to_Index.end())
		{
			m_MeshData->Map_Face_Name_to_Index.erase(it);
		}
	}

    // Delete Face : only clear memory, not change the length of FaceList
	m_MeshData->FaceList[FaceIndex].Clear(MDK_PURE_EMPTY);
    m_MeshData->FaceValidityFlagList[FaceIndex] = 0;
}

}// namespace mdk
