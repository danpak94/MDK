#ifndef __mdkPolygonMeshItem_hpp
#define __mdkPolygonMeshItem_hpp


namespace mdk
{
//=========================================================== Vertex_Of_PolygonMesh ===========================================================//

template<typename ScalarType>
inline
Vertex_Of_PolygonMesh<ScalarType>::Vertex_Of_PolygonMesh(PolygonMesh<ScalarType>* ParentMesh)
{
    this->Create();
    m_Data->Mesh = ParentMesh;
}

template<typename ScalarType>
inline
Vertex_Of_PolygonMesh<ScalarType>::Vertex_Of_PolygonMesh(const Vertex_Of_PolygonMesh<ScalarType>& InputVertex)
{
    this->Create();
    this->Copy(InputVertex);
}

template<typename ScalarType>
inline
Vertex_Of_PolygonMesh<ScalarType>::Vertex_Of_PolygonMesh(Vertex_Of_PolygonMesh<ScalarType>&& InputVertex)
{
    this->Create();
    this->Take(std::forward<Vertex_Of_PolygonMesh<ScalarType>&>(InputVertex));
}

template<typename ScalarType>
inline
Vertex_Of_PolygonMesh<ScalarType>::~Vertex_Of_PolygonMesh() 
{
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::operator=(const Vertex_Of_PolygonMesh<ScalarType>& InputVertex) 
{
    this->Copy(InputVertex); 
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::operator=(Vertex_Of_PolygonMesh<ScalarType>&& InputVertex) 
{
    this->Take(std::forward<Vertex_Of_PolygonMesh<ScalarType>&>(InputVertex));
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::Clear()
{
    m_Data->Mesh = nullptr;
    m_Data->Index = -1;
    m_Data->AdjacentVertexIndexList.Clear();
    m_Data->AdjacentEdgeIndexList.Clear();
    m_Data->AdjacentHalfEdgeIndexList.Clear();
    m_Data->AdjacentPolygonIndexList.Clear();
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::Create()
{
    if (!m_Data)
    {
        m_Data = std::make_unique<Data_Of_Vertex_Of_PolygonMesh<ScalarType>>();
    }
    this->Clear();
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::Delete()
{
    m_Data.reset();
}

template<typename ScalarType>
inline
bool Vertex_Of_PolygonMesh<ScalarType>::IsValid() const
{
    return bool(m_Data);
}

template<typename ScalarType>
inline
bool Vertex_Of_PolygonMesh<ScalarType>::IsDeleted() const
{
    return (!m_Data);
}

template<typename ScalarType>
void Vertex_Of_PolygonMesh<ScalarType>::Copy(const Vertex_Of_PolygonMesh<ScalarType>& InputVertex)
{
    m_Data->Mesh = InputVertex.m_Data->Mesh;
    m_Data->Index = InputVertex.m_Data->Index;
    m_Data->AdjacentVertexIndexList = InputVertex.m_Data->AdjacentVertexIndexList;
    m_Data->AdjacentEdgeIndexList = InputVertex.m_Data->AdjacentEdgeIndexList;
    m_Data->AdjacentHalfEdgeIndexList = InputVertex.m_Data->AdjacentHalfEdgeIndexList;
    m_Data->AdjacentPolygonIndexList = InputVertex.m_Data->AdjacentPolygonIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::Take(Vertex_Of_PolygonMesh<ScalarType>& InputVertex)
{
    m_Data->Mesh = InputVertex.m_Data->Mesh;
    m_Data->Index = InputVertex.m_Data->Index;
    m_Data->AdjacentVertexIndexList   = std::move(InputVertex.m_Data->AdjacentVertexIndexList);
    m_Data->AdjacentEdgeIndexList     = std::move(InputVertex.m_Data->AdjacentEdgeIndexList);
    m_Data->AdjacentHalfEdgeIndexList = std::move(InputVertex.m_Data->AdjacentHalfEdgeIndexList);
    m_Data->AdjacentPolygonIndexList  = std::move(InputVertex.m_Data->AdjacentPolygonIndexList);

    InputVertex.Clear();
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::SetParentMesh(PolygonMesh<ScalarType>* InputMesh)
{
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
PolygonMesh<ScalarType>* Vertex_Of_PolygonMesh<ScalarType>::GetParentMesh()
{
    return m_Data->Mesh;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::SetIndex(int_max VertexIndex)
{
    m_Data->Index = VertexIndex;
}

template<typename ScalarType>
inline
int_max Vertex_Of_PolygonMesh<ScalarType>::GetIndex() const
{
    return m_Data->Index;
}

template<typename ScalarType>
inline 
void Vertex_Of_PolygonMesh<ScalarType>::GetPosition(ScalarType* Pos)
{
    m_Data->Mesh->m_MeshData->VertexPositionTable.GetCol(m_Index, Pos);
}

template<typename ScalarType>
inline 
void Vertex_Of_PolygonMesh<ScalarType>::GetPosition(ScalarType& x, ScalarType& y, ScalarType& z)
{
    ScalarType Pos[3] = { 0, 0, 0 };   
    this->GetPosition(Pos);
    x = Pos[0];
    y = Pos[1];
    z = Pos[2];
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::SetAdjacentHalfEdgeIndexList(DenseVector<int_max> IndexList)
{
    m_Data->AdjacentHalfEdgeIndexList = std::move(IndexList);
}

template<typename ScalarType>
inline
const DenseVector<int_max>& Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentHalfEdgeIndexList() const
{
    return m_Data->AdjacentHalfEdgeIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentHalfEdgeIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList = m_Data->AdjacentHalfEdgeIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::SetAdjacentVertexIndexList(DenseVector<int_max> IndexList)
{
    m_Data->AdjacentVertexIndexList = std::move(IndexList);
}

template<typename ScalarType>
inline
const DenseVector<int_max>& Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentVertexIndexList() const
{
    return m_Data->AdjacentVertexIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentVertexIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList = m_Data->AdjacentVertexIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::SetAdjacentEdgeIndexList(DenseVector<int_max> IndexList)
{
    m_Data->AdjacentEdgeIndexList = std::move(IndexList);
}

template<typename ScalarType>
inline
const DenseVector<int_max>& Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentEdgeIndexList() const
{
    return m_Data->AdjacentEdgeIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentEdgeIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList = m_Data->AdjacentEdgeIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::SetAdjacentPolygonIndexList(DenseVector<int_max> IndexList)
{
    m_Data->AdjacentPolygonIndexList = std::move(IndexList);
}

template<typename ScalarType>
inline
const DenseVector<int_max>& Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList() const
{
    return m_Data->AdjacentPolygonIndexList;
}

template<typename ScalarType>
inline
void Vertex_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList = m_Data->AdjacentPolygonIndexList;
}

//=========================================================== Edge_Of_PolygonMesh ===========================================================//

template<typename ScalarType>
inline
Edge_Of_PolygonMesh<ScalarType>::Edge_Of_PolygonMesh(PolygonMesh<ScalarType>* InputMesh)
{
    this->Create();
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
Edge_Of_PolygonMesh<ScalarType>::Edge_Of_PolygonMesh(const Edge_Of_PolygonMesh<ScalarType>& InputEdge)
{
    this->Create();
    m_Data->Mesh = InputEdge.m_Data->Mesh;
    m_Data->HalfEdgeIndex0 = InputEdge.m_Data->HalfEdgeIndex0;
    m_Data->HalfEdgeIndex1 = InputEdge.m_Data->HalfEdgeIndex1;
}

template<typename ScalarType>
inline
Edge_Of_PolygonMesh<ScalarType>::~Edge_Of_PolygonMesh()
{
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::operator=(const Edge_Of_PolygonMesh<ScalarType>& InputEdge)
{
    m_Data->Mesh = InputEdge.m_Data->Mesh;
    m_Data->HalfEdgeIndex0 = InputEdge.m_Data->HalfEdgeIndex0;
    m_Data->HalfEdgeIndex1 = InputEdge.m_Data->HalfEdgeIndex1;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::Clear()
{
    m_Data->Mesh = nullptr;
    m_Data->HalfEdgeIndex0 = -1;
    m_Data->HalfEdgeIndex1 = -1;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::Create()
{
    if (!m_Data)
    {
        m_Data = std::make_unique<Data_Of_Edge_Of_PolygonMesh<ScalarType>>();
    }
    this->Clear();
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::Delete()
{
    m_Data.reset();
}

template<typename ScalarType>
inline
bool Edge_Of_PolygonMesh<ScalarType>::IsValid() const
{
    return bool(m_Data);
}

template<typename ScalarType>
inline
bool Edge_Of_PolygonMesh<ScalarType>::IsDeleted() const
{
    return (!m_Data);
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::SetParentMesh(PolygonMesh<ScalarType>* InputMesh)
{
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
PolygonMesh<ScalarType>* Edge_Of_PolygonMesh<ScalarType>::GetParentMesh()
{
    return m_Data->Mesh;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::SetIndex(int_max EdgeIndex)
{
    m_Data->Index = EdgeIndex;
}

template<typename ScalarType>
inline
int_max Edge_Of_PolygonMesh<ScalarType>::GetIndex() const
{
    return m_Data->Index;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::SetHalfEdgeIndexList(int_max HalfEdgeIndex0, int_max HalfEdgeIndex1)
{
    m_Data->HalfEdgeIndex0 = HalfEdgeIndex0;
    m_Data->HalfEdgeIndex1 = HalfEdgeIndex1;
}

template<typename ScalarType>
inline
DenseVector<int_max> Edge_Of_PolygonMesh<ScalarType>::GetHalfEdgeIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetHalfEdgeIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetHalfEdgeIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList.FastResize(2);
    OutputIndexList[0] = m_Data->HalfEdgeIndex0;
    OutputIndexList[1] = m_Data->HalfEdgeIndex1;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetHalfEdgeIndexList(int_max& HalfEdgeIndex0, int_max& HalfEdgeIndex1) const
{
    HalfEdgeIndex0 = m_Data->HalfEdgeIndex0;
    HalfEdgeIndex1 = m_Data->HalfEdgeIndex1;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetHalfEdgeIndexList(int_max HalfEdgeIndexList[2]) const
{
    HalfEdgeIndexList[0] = m_Data->HalfEdgeIndex0;
    HalfEdgeIndexList[1] = m_Data->HalfEdgeIndex1;
}

template<typename ScalarType>
inline
DenseVector<int_max> Edge_Of_PolygonMesh<ScalarType>::GetVertexIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetVertexIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetVertexIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList.FastResize(2);
    OutputIndexList[0] = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetStartVertexIndex();
    OutputIndexList[1] = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetEndVertexIndex();
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetVertexIndexList(int_max& VertexIndex0, int_max& VertexIndex1) const
{
    VertexIndex0 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetStartVertexIndex();
    VertexIndex1 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetEndVertexIndex();
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetVertexIndexList(int_max OutputIndexList[2]) const
{
    OutputIndexList[0] = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetStartVertexIndex();
    OutputIndexList[1] = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetEndVertexIndex();
}

template<typename ScalarType>
inline
DenseVector<int_max> Edge_Of_PolygonMesh<ScalarType>::GetAdjacentEdgeIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetAdjacentEdgeIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetAdjacentEdgeIndexList(DenseVector<int_max>& OutputIndexList) const
{
    auto VertexIndex0 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetEndVertexIndex();
    auto VertexIndex1 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex1].GetEndVertexIndex();
    const DenseVector<int_max>& AdjacentEdgeIndexList0 = m_Data->Mesh->m_MeshData->VertexList[VertexIndex0].GetAdjacentEdgeIndexList();
    const DenseVector<int_max>& AdjacentEdgeIndexList1 = m_Data->Mesh->m_MeshData->VertexList[VertexIndex1].GetAdjacentEdgeIndexList();

    OutputIndexList.FastResize(0);
    OutputIndexList.ReserveCapacity(AdjacentEdgeIndexList0.GetLength() + AdjacentEdgeIndexList1.GetLength());

    for (int_max k = 0; k < AdjacentEdgeIndexList0.GetLength(); ++k)
    {
        auto tempIndex = AdjacentEdgeIndexList0[k];
        if (tempIndex != m_Index)
        {
            OutputIndexList.Append(tempIndex);
        }
    }

    for (int_max k = 0; k < AdjacentEdgeIndexList1.GetLength(); ++k)
    {
        auto tempIndex = AdjacentEdgeIndexList1[k];
        if (tempIndex != m_Index)
        {
            OutputIndexList.Append(tempIndex);
        }
    }
}

template<typename ScalarType>
inline
DenseVector<int_max> Edge_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetAdjacentPolygonIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void Edge_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList(DenseVector<int_max>& OutputIndexList) const
{
    auto VertexIndex0 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex0].GetEndVertexIndex();
    auto VertexIndex1 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndex1].GetEndVertexIndex();
    const DenseVector<int_max>& AdjacentPolygonIndexList0 = m_Data->Mesh->m_MeshData->VertexList[VertexIndex0].GetAdjacentPolygonIndexList();
    const DenseVector<int_max>& AdjacentPolygonIndexList1 = m_Data->Mesh->m_MeshData->VertexList[VertexIndex1].GetAdjacentPolygonIndexList();

    OutputIndexList.FastResize(0);
    OutputIndexList.ReserveCapacity(AdjacentPolygonIndexList0.GetLength() + AdjacentPolygonIndexList1.GetLength());

    for (int_max k = 0; k < AdjacentPolygonIndexList0.GetLength(); ++k)
    {
        auto tempIndex = AdjacentPolygonIndexList0[k];
        if (tempIndex != m_Index)
        {
            OutputIndexList.Append(tempIndex);
        }
    }

    for (int_max k = 0; k < AdjacentPolygonIndexList1.GetLength(); ++k)
    {
        auto tempIndex = AdjacentPolygonIndexList1[k];
        if (tempIndex != m_Index)
        {
            OutputIndexList.Append(tempIndex);
        }
    }
}

//=========================================================== HalfEdge_Of_PolygonMesh ===========================================================//

template<typename ScalarType>
inline
HalfEdge_Of_PolygonMesh<ScalarType>::HalfEdge_Of_PolygonMesh(PolygonMesh<ScalarType>* InputMesh)
{
    this->Create();
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
HalfEdge_Of_PolygonMesh<ScalarType>::HalfEdge_Of_PolygonMesh(const HalfEdge_Of_PolygonMesh<ScalarType>& InputHalfEdge)
{
    this->Create();
    (*this) = InputHalfEdge;
}

template<typename ScalarType>
inline
HalfEdge_Of_PolygonMesh<ScalarType>::~HalfEdge_Of_PolygonMesh()
{
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::operator=(const HalfEdge_Of_PolygonMesh<ScalarType>& InputHalfEdge)
{
    m_Data->Mesh = InputHalfEdge.m_Data->Mesh;
    m_Data->Index = InputHalfEdge.m_Data->Index;
    m_Data->VertexIndex_start = InputHalfEdge.m_Data->VertexIndex_start;
    m_Data->VertexIndex_end = InputHalfEdge.m_Data->VertexIndex_end;
    m_Data->EdgeIndex = InputHalfEdge.m_Data->EdgeIndex;
    m_Data->PolygonIndex = InputHalfEdge.m_Data->PolygonIndex;
    m_Data->OppositeHalfEdgeIndex = InputHalfEdge.m_Data->OppositeHalfEdgeIndex;
    m_Data->NextHalfEdgeIndex = InputHalfEdge.m_Data->NextHalfEdgeIndex;
    m_Data->PreviousHalfEdgeIndex = InputHalfEdge.m_Data->PreviousHalfEdgeIndex;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::Clear()
{
    m_Data->Mesh = nullptr;
    m_Data->Index = -1;
    m_Data->VertexIndex_start = -1;
    m_Data->VertexIndex_end = -1;
    m_Data->EdgeIndex = -1;
    m_Data->PolygonIndex = -1;
    m_Data->OppositeHalfEdgeIndex = -1;
    m_Data->NextHalfEdgeIndex = -1;
    m_Data->PreviousHalfEdgeIndex = -1;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::Create()
{
    if (!m_Data)
    {
        m_Data = std::make_unique<Data_Of_HalfEdge_Of_PolygonMesh<ScalarType>>();
    }
    this->Clear();
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::Delete()
{
    m_Data.reset();
}

template<typename ScalarType>
inline
bool HalfEdge_Of_PolygonMesh<ScalarType>::IsValid() const
{
    return bool(m_Data);
}

template<typename ScalarType>
inline
bool HalfEdge_Of_PolygonMesh<ScalarType>::IsDeleted() const
{
    return (!m_Data);
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetParentMesh(PolygonMesh<ScalarType>* InputMesh)
{
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
PolygonMesh<ScalarType>* HalfEdge_Of_PolygonMesh<ScalarType>::GetParentMesh()
{
    return m_Data->Mesh;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetIndex(int_max HalfEdgeIndex)
{
    m_Data->Index = HalfEdgeIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetIndex() const
{
    return m_Data->Index;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetStartVertexIndex(int_max VertexIndex)
{
    m_Data->VertexIndex_start = VertexIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetStartVertexIndex() const
{
    return m_Data->VertexIndex_start;
}


template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetEndVertexIndex(int_max VertexIndex)
{
    m_Data->VertexIndex_end = VertexIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetEndVertexIndex() const
{
    return m_Data->VertexIndex_end;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetEdgeIndex(int_max EdgeIndex)
{
    m_Data->EdgeIndex = EdgeIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetEdgeIndex() const
{
    return m_Data->EdgeIndex;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetPolygonIndex(int_max PolygonIndex)
{
    m_Data->PolygonIndex = PolygonIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetPolygonIndex() const
{
    return m_Data->PolygonIndex;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetOppositeHalfEdgeIndex(int_max HalfEdgeIndex)
{
    m_Data->OppositeHalfEdgeIndex = HalfEdgeIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetOppositeHalfEdgeIndex() const
{
    return m_Data->OppositeHalfEdgeIndex;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetNextHalfEdgeIndex(int_max HalfEdgeIndex)
{
    m_Data->NextHalfEdgeIndex = HalfEdgeIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetNextHalfEdgeIndex() const
{
    return m_Data->NextHalfEdgeIndex;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::SetPreviousHalfEdgeIndex(int_max HalfEdgeIndex)
{
    m_Data->PreviousHalfEdgeIndex = HalfEdgeIndex;
}

template<typename ScalarType>
inline
int_max HalfEdge_Of_PolygonMesh<ScalarType>::GetPreviousHalfEdgeIndex() const
{
    return m_Data->PreviousHalfEdgeIndex;
}

template<typename ScalarType>
inline
DenseVector<int_max> HalfEdge_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetAdjacentPolygonIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList.FastResize(2);
    OutputIndexList[0] = m_Data->PolygonIndex;
    OutputIndexList[1] = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->OppositeHalfEdgeIndex].GetPolygonIndex();
    return OutputIndexList;
}

template<typename ScalarType>
inline
void HalfEdge_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList(int_max& PolygonIndex0, int_max& PolygonIndex1) const
{
    PolygonIndex0 = m_Data->PolygonIndex;
    PolygonIndex1 = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->OppositeHalfEdgeIndex].GetPolygonIndex();
}

//=========================================================== Polygon_Of_PolygonMesh ===========================================================//

template<typename ScalarType>
inline
Polygon_Of_PolygonMesh<ScalarType>::Polygon_Of_PolygonMesh(PolygonMesh<ScalarType>* InputMesh)
{
    this->Create();
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
Polygon_Of_PolygonMesh<ScalarType>::Polygon_Of_PolygonMesh(const Polygon_Of_PolygonMesh<ScalarType>& InputPolygon)
{
    this->Create();
    (*this) = InputPolygon;
}

template<typename ScalarType>
inline
Polygon_Of_PolygonMesh<ScalarType>::Polygon_Of_PolygonMesh(Polygon_Of_PolygonMesh<ScalarType>&& InputPolygon)
{
    this->Create();
    this->operator=(std::forward<Polygon_Of_PolygonMesh<ScalarType>&&>(InputPolygon));
}

template<typename ScalarType>
inline
Polygon_Of_PolygonMesh<ScalarType>::~Polygon_Of_PolygonMesh()
{
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::operator=(const Polygon_Of_PolygonMesh<ScalarType>& InputHalfEdge)
{
    m_Data->Mesh = InputHalfEdge.m_Data->Mesh;
    m_Data->Index = InputHalfEdge.m_Data->Index;
    m_Data->HalfEdgeIndexList = InputHalfEdge.m_Data->HalfEdgeIndexList;
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::operator=(Polygon_Of_PolygonMesh<ScalarType>&& InputHalfEdge)
{
    m_Data->Mesh = InputHalfEdge.m_Data->Mesh;
    m_Data->Index = InputHalfEdge.m_Data->Index;
    m_Data->HalfEdgeIndexList = std::move(InputHalfEdge.m_Data->HalfEdgeIndexList);
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::Clear()
{
    m_Data->Mesh = nullptr;
    m_Data->Index = -1;
    m_Data->HalfEdgeIndexList.Clear();
}


template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::Create()
{
    if (!m_Data)
    {
        m_Data = std::make_unique<Data_Of_Polygon_Of_PolygonMesh<ScalarType>>();
    }
    this->Clear();
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::Delete()
{
    m_Data.reset();
}

template<typename ScalarType>
inline
bool Polygon_Of_PolygonMesh<ScalarType>::IsValid() const
{
    return bool(m_Data);
}

template<typename ScalarType>
inline
bool Polygon_Of_PolygonMesh<ScalarType>::IsDeleted() const
{
    return (!m_Data);
}

template<typename ScalarType>
inline
bool Polygon_Of_PolygonMesh<ScalarType>::IsTriangle() const
{
    return (this->GetVertexNumber() == 3);
}


template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::SetParentMesh(PolygonMesh<ScalarType>* InputMesh)
{
    m_Data->Mesh = InputMesh;
}

template<typename ScalarType>
inline
PolygonMesh<ScalarType>* Polygon_Of_PolygonMesh<ScalarType>::GetParentMesh()
{
    return m_Data->Mesh;
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::SetIndex(int_max PolygonIndex)
{
    m_Data->Index = PolygonIndex;
}

template<typename ScalarType>
inline
int_max Polygon_Of_PolygonMesh<ScalarType>::GetIndex() const
{
    return m_Data->Index;
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::SetHalfEdgeIndexList(DenseVector<int_max> IndexList)
{
    m_Data->HalfEdgeIndexList = std::move(IndexList);
}

template<typename ScalarType>
inline
const DenseVector<int_max>& Polygon_Of_PolygonMesh<ScalarType>::GetHalfEdgeIndexList() const
{
    return m_Data->HalfEdgeIndexList;
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::GetHalfEdgeIndexList(DenseVector<int_max>& OutputIndexList) const
{
    OutputIndexList = m_Data->HalfEdgeIndexList;
}

template<typename ScalarType>
inline int_max Polygon_Of_PolygonMesh<ScalarType>::GetHalfEdgeNumber() const
{
    return m_Data->HalfEdgeIndexList.GetElementNumber();
}

template<typename ScalarType>
inline
const DenseVector<int_max>& Polygon_Of_PolygonMesh<ScalarType>::GetVertexIndexList() const
{
    // DenseVector<int_max> OutputIndexList;
    // this->GetVertexIndexList(OutputIndexList);
    // return OutputIndexList;

    return m_Data->Mesh->m_MeshData->VertexIndexTable[m_Data->Index];
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::GetVertexIndexList(DenseVector<int_max>& OutputIndexList) const
{
    // int_max VertexNumber = m_HalfEdgeIndexList.GetLength();
    // OutputIndexList.FastResize(VertexNumber);
    // for (int_max k = 0; k < VertexNumber; ++k)
    // {
    //     int_max OppositeHalfEdgeIndex = m_Mesh->m_MeshData->HalfEdgeList[m_HalfEdgeIndexList[k]].GetOppositeHalfEdgeIndex();
    //     OutputIndexList[k] = m_Mesh->m_MeshData->HalfEdgeList[OppositeHalfEdgeIndex]].GetVertexIndex;
    // 
    // }
    
    OutputIndexList = m_Data->Mesh->m_MeshData->VertexIndexTable[m_Data->Index];

    // OutputIndexList[k] is the index of the starting vertex of the halfedge (m_HalfEdgeIndexList[k])

}

template<typename ScalarType>
inline 
int_max Polygon_Of_PolygonMesh<ScalarType>::GetVertexNumber() const
{
    return m_Data->Mesh->m_MeshData->VertexIndexTable[m_Data->Index].GetElementNumber();
}

template<typename ScalarType>
inline
DenseVector<int_max> Polygon_Of_PolygonMesh<ScalarType>::GetEdgeIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetEdgeIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::GetEdgeIndexList(DenseVector<int_max>& OutputIndexList) const
{
    int_max VertexNumber = m_Data->HalfEdgeIndexList.GetLength();
    OutputIndexList.FastResize(VertexNumber);
    for (int_max k = 0; k < VertexNumber; ++k)
    {
        OutputIndexList[k] = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndexList[k]].GetEdgeIndex;
    }
}

template<typename ScalarType>
inline int_max Polygon_Of_PolygonMesh<ScalarType>::GetEdgeNumber() const
{
    return m_Data->HalfEdgeIndexList.GetElementNumber();
}


template<typename ScalarType>
inline
DenseVector<int_max> Polygon_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList() const
{
    DenseVector<int_max> OutputIndexList;
    this->GetAdjacentPolygonIndexList(OutputIndexList);
    return OutputIndexList;
}

template<typename ScalarType>
inline
void Polygon_Of_PolygonMesh<ScalarType>::GetAdjacentPolygonIndexList(DenseVector<int_max>& OutputIndexList) const
{
    int_max HalfEdgeNumber = m_Data->HalfEdgeIndexList.GetLength();
    OutputIndexList.FastResize(HalfEdgeNumber);
    for (int_max k = 0; k < HalfEdgeNumber; ++k)
    {
        int_max OppositeHalfEdgeIndex = m_Data->Mesh->m_MeshData->HalfEdgeList[m_Data->HalfEdgeIndexList[k]].GetOppositeHalfEdgeIndex();
        OutputIndexList[k] = m_Data->Mesh->m_MeshData->HalfEdgeList[OppositeHalfEdgeIndex].GetPolygonIndex();
    }
}

}// namespace mdk

#endif