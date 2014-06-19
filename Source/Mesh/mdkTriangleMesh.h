#ifndef __mdkTriangleMesh_h
#define __mdkTriangleMesh_h

#include "mdkPolygonMesh.h"
#include "mdkTriangleMeshAttribute.h"

namespace mdk
{

template<typename ScalarType>
struct TriangleMeshAttributeType
{
    typedef ScalarType  ScalarType;
    typedef bool PointAttributeType;
    typedef VertexAttribute_Of_TriangleMesh<ScalarType>         VertexAttributeType;
    typedef EdgeAttribute_Of_TriangleMesh<ScalarType>           EdgeAttributeType;
    typedef DirectedEdgeAttribute_Of_TriangleMesh<ScalarType>   DirectedEdgeAttributeType;
    typedef CellAttribute_Of_TriangleMesh<ScalarType>           CellAttributeType;
};


template<typename MeshAttributeType>
class TriangleMesh : protected PolygonMesh<MeshAttributeType>
{
public:
    typedef TriangleMesh<MeshAttributeType> MeshType;

    typedef MeshAttributeType MeshAttributeType;

    typedef int_max IndexType;

    typedef typename MeshAttributeType::ScalarType ScalarType;

    //typedef typename MeshAttributeType::PointAttributeType          PointAttributeType;
    typedef typename MeshAttributeType::VertexAttributeType         VertexAttributeType;
    typedef typename MeshAttributeType::EdgeAttributeType           EdgeAttributeType;
    typedef typename MeshAttributeType::DirectedEdgeAttributeType   DirectedEdgeAttributeType;
    typedef typename MeshAttributeType::CellAttributeType           CellAttributeType;

    typedef Vertex_Of_SurfaceMesh<MeshAttributeType>          VertexType;
    typedef Edge_Of_SurfaceMesh<MeshAttributeType>            EdgeType;
    typedef DirectedEdge_Of_SurfaceMesh<MeshAttributeType>    DirectedEdgeType;
    typedef Cell_Of_SurfaceMesh<MeshAttributeType>            CellType;

    typedef Handle_Of_Vertex_Of_SurfaceMesh         VertexHandleType;
    typedef Handle_Of_Edge_Of_SurfaceMesh           EdgeHandleType;
    typedef Handle_Of_DirectedEdge_Of_SurfaceMesh   DirectedEdgeHandleType;
    typedef Handle_Of_Cell_Of_SurfaceMesh           CellHandleType;

    typedef Iterator_Of_Vertex_Of_SurfaceMesh<MeshAttributeType>     VertexIteratorType;
    typedef Iterator_Of_Edge_Of_SurfaceMesh<MeshAttributeType>       EdgeIteratorType;
    typedef Iterator_Of_Cell_Of_SurfaceMesh<MeshAttributeType>       CellIteratorType;

public:
    TriangleMesh();

    TriangleMesh(const TriangleMesh& InputMesh);

    TriangleMesh(TriangleMesh&& InputMesh);

    ~TriangleMesh();

    inline void operator=(const TriangleMesh& InputMesh);

    inline void operator=(TriangleMesh&& InputMesh);

    inline void Clear();

    inline void Copy(const TriangleMesh& InputMesh);
    inline bool Copy(const TriangleMesh* InputMesh);

    inline void Share(TriangleMesh& InputMesh);
    inline bool Share(TriangleMesh* InputMesh);

    inline void ForceShare(const TriangleMesh& InputMesh);
    inline bool ForceShare(const TriangleMesh* InputMesh);

    inline void Take(TriangleMesh&& InputMesh);
    inline bool Take(TriangleMesh& InputMesh);
    inline bool Take(TriangleMesh* InputMesh);

    //-------------------------------------------------------------------
    inline bool IsEmpty() const;
    inline int_max GetPointNumber() const;
    inline int_max GetVertexNumber() const;
    inline int_max GetCellNumber() const;
    inline int_max GetEdgeNumber() const;

    // 3D Position -----------------------------------------------------------

    inline void SetVertexPosition(const DenseVector<VertexHandleType>& VertexHandleList, const DenseMatrix<ScalarType>& VertexPositionMatrix);

    inline DenseMatrix<ScalarType> GetVertexPosition(const DenseVector<VertexHandleType>& VertexHandleList) const;
    inline void GetVertexPosition(DenseMatrix<ScalarType>& VertexPositionMatrix, const DenseVector<VertexHandleType>& VertexHandleList) const;

    //----- Get/Set  Mesh Item {Point, Vertex, Edge, DirectedEdge, Cell} ------//

    inline VertexType& Vertex(VertexHandleType VertexHandle);
    inline const VertexType& Vertex(VertexHandleType VertexHandle) const;

    inline EdgeType& Edge(EdgeHandleType EdgeHandle);
    inline const EdgeType& Edge(EdgeHandleType EdgeHandle) const;

    inline DirectedEdgeType& DirectedEdge(DirectedEdgeHandleType DirectedEdgeHandle);
    inline const DirectedEdgeType& DirectedEdge(DirectedEdgeHandleType DirectedEdgeHandle) const;

    inline CellType& Cell(CellHandleType CellHandle);
    inline const CellType& Cell(CellHandleType CellHandle) const;

    //------------- Iterator --------------------------------------------------------------//

    inline VertexIteratorType GetIteratorOfVertex() const;
    inline EdgeIteratorType   GetIteratorOfEdge() const;
    inline CellIteratorType   GetIteratorOfCell() const;

    // use this function and GetIteratorOfEdge as DirectedEdge Iterator
    inline DirectedEdgeHandleType GetDirectedEdgeHandle(EdgeHandleType EdgeHandle, int_max RelativeIndex) const;

    // get handle by ID -----------------------------------------------------------------------//

    inline VertexHandleType GetVertexHandle(int_max VertexID) const;
    inline EdgeHandleType   GetEdgeHandle(int_max EdgeID) const;
    inline DirectedEdgeHandleType GetDirectedEdgeHandle(int_max EdgeID, int_max RelativeIndex) const;
    inline CellHandleType   GetCellHandle(int_max CellID) const;

    // Add Mesh Item -------------------------------------------------------------------------//
    // add an item and return index (-1 if input is invalid)

    VertexHandleType AddVertex(ScalarType Position[3]);
    VertexHandleType AddVertex(ScalarType x, ScalarType y, ScalarType z);
    DenseVector<VertexHandleType> AddVertex(const DenseMatrix<ScalarType>& PointSet);

    EdgeHandleType AddEdge(VertexHandleType VertexHandle0, VertexHandleType VertexHandle1);
    CellHandleType AddCell(const DenseVector<EdgeHandleType>& EdgeHandleList);

    //Delete Mesh Item ----------------------------------------------------------------------//

    bool DeleteCell(CellHandleType CellHandle);
    bool DeleteEdge(EdgeHandleType EdgeHandle);

    bool DeleteVertex(VertexHandleType VertexHandle);
    bool DeleteVertex(const DenseVector<VertexHandleType>& VertexHandleList);

    //---------------------------------------------------------------------------//
    // attention: after this function is called, every index and handle will be changed
    // and there will be no "dead/deleted" item in any object list (e.g., m_MeshData->VertexList)
    void CleanDataStructure();

    //------------ Construct from input data ------------------------------------//

    bool Construct(const DenseMatrix<ScalarType>& InputVertexPositionTable, const DataArray<DenseVector<int_max>>& InputCellTable);
    // index order in each VertexIndexList should be consistent

    bool Construct(SurfaceMesh<MeshAttributeType> InputSurfaceMesh);

    bool Construct(PolygonMesh<MeshAttributeType> InputPolygonMesh);

    bool CheckIfTriangleMesh() const;

    //--------------------- output -------------------------------------------------//

    void GetVertexPositionTableAndCellTable(DenseMatrix<ScalarType>& VertexPositionTable, DataArray<DenseVector<int_max>>& CellTable) const;

    //------------- Mesh Attribute --------------------------------------------------//

    void UpdateNormalAtVertex();
    void UpdateNormalAtCell();
};

}// namespace mdk

#include "mdkTriangleMesh.hpp"

#endif