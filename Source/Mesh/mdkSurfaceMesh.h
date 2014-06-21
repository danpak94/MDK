#ifndef __mdkSurfaceMesh_h
#define __mdkSurfaceMesh_h

#include <unordered_map>

#include "mdkSurfaceMeshItem.h"
#include "mdkSurfaceMeshItemIterator.h"

namespace mdk
{

template<typename MeshAttributeType>
struct SurfaceMeshData
{
    //-------------------------------------------------------------------------------------------//
    typedef typename MeshAttributeType::ScalarType  ScalarType;
    //-------------------------------------------------------------------------------------------//

    DenseMatrix<ScalarType> PointPositionTable;
    // row_0: x
    // row_1: y
    // row_2: z
    // a point is a  of an Edge or a point on Edge Edge

    DataArray<Point_Of_SurfaceMesh<MeshAttributeType>> PointList;

    DenseVector<int_max> PointValidityFlagList;
    // 1: point is an element of the mesh 
    // 0: point is deleted

    DataArray<Edge_Of_SurfaceMesh<MeshAttributeType>> EdgeList;

    DataArray<DenseVector<DirectedEdge_Of_SurfaceMesh<MeshAttributeType>, 2>> DirectedEdgePairList;

    DenseVector<int_max> EdgeValidityFlagList;
    // 1: Edge is an element of the mesh 
    // 0: Edge is deleted

    DataArray<Cell_Of_SurfaceMesh<MeshAttributeType>> CellList; // also known as face, facet, element

    DenseVector<int_max>  CellValidityFlagList;
    // 1: Cell is an element of the mesh 
    // 0: Cell is deleted

    // Attention: ID must >= 0
    std::unordered_map<int_max, int_max> Map_PointID_to_PointIndex;
    std::unordered_map<int_max, int_max> Map_EdgeID_to_EdgeIndex;
    std::unordered_map<int_max, DirectedEdgeIndex_Of_SurfaceMesh> Map_DirectedEdgeID_to_DirectedEdgeIndex;
    std::unordered_map<int_max, int_max> Map_CellID_to_CellIndex;
};


template<typename MeshAttributeType>
class SurfaceMesh : public Object
{
public:
    //-------------------------------------------------------------------------------------------//
    typedef typename MeshAttributeType::ScalarType                ScalarType;
    typedef typename MeshAttributeType::PointAttributeType        PointAttributeType;
    typedef typename MeshAttributeType::EdgeAttributeType         EdgeAttributeType;
    typedef typename MeshAttributeType::DirectedEdgeAttributeType DirectedEdgeAttributeType;
    typedef typename MeshAttributeType::CellAttributeType         CellAttributeType;
    //-------------------------------------------------------------------------------------------//
    typedef int_max IndexType;
    //--------------------------------------------------------------------------------------------//
    typedef Point_Of_SurfaceMesh<MeshAttributeType>           PointType;
    typedef Edge_Of_SurfaceMesh<MeshAttributeType>            EdgeType;
    typedef DirectedEdge_Of_SurfaceMesh<MeshAttributeType>    DirectedEdgeType;
    typedef Cell_Of_SurfaceMesh<MeshAttributeType>            CellType;

    typedef Handle_Of_Point_Of_SurfaceMesh          PointHandleType;
    typedef Handle_Of_Edge_Of_SurfaceMesh           EdgeHandleType;
    typedef Handle_Of_DirectedEdge_Of_SurfaceMesh   DirectedEdgeHandleType;
    typedef Handle_Of_Cell_Of_SurfaceMesh           CellHandleType;

    typedef Iterator_Of_Point_Of_SurfaceMesh<MeshAttributeType>           PointIteratorType;
    typedef Iterator_Of_Edge_Of_SurfaceMesh<MeshAttributeType>            EdgeIteratorType;
    typedef Iterator_Of_DirectedEdge_Of_SurfaceMesh<MeshAttributeType>    DirectedEdgeIteratorType;
    typedef Iterator_Of_Cell_Of_SurfaceMesh<MeshAttributeType>            CellIteratorType;
    //--------------------------------------------------------------------------------------------------//
private:
    std::shared_ptr<SurfaceMeshData<MeshAttributeType>> m_MeshData;

private:
    template<typename T>
    friend class Point_Of_SurfaceMesh;

    template<typename T>
    friend struct Iterator_Of_Point_Of_SurfaceMesh;

    template<typename T>
    friend class Edge_Of_SurfaceMesh;

    template<typename T>
    friend struct Iterator_Of_Edge_Of_SurfaceMesh;

    template<typename T>
    friend class DirectedEdge_Of_SurfaceMesh;

    template<typename T>
    friend struct Iterator_Of_DirectedEdge_Of_SurfaceMesh;

    template<typename T>
    friend class Cell_Of_SurfaceMesh;    

    template<typename T>
    friend struct Iterator_Of_Cell_Of_SurfaceMesh;

public:
    SurfaceMesh();
    SurfaceMesh(const Pure_Empty_SurfaceMesh_Symbol&);
    SurfaceMesh(const SurfaceMesh& InputMesh);
    SurfaceMesh(SurfaceMesh&& InputMesh);
    ~SurfaceMesh();

    inline void operator=(const SurfaceMesh& InputMesh);
    inline void operator=(SurfaceMesh&& InputMesh);

    inline void Clear();

    inline void Copy(const SurfaceMesh& InputMesh);
    inline bool Copy(const SurfaceMesh* InputMesh);

    inline void Share(SurfaceMesh& InputMesh);
    inline bool Share(SurfaceMesh* InputMesh);

    inline void ForceShare(const SurfaceMesh& InputMesh);
    inline bool ForceShare(const SurfaceMesh* InputMesh);

    inline void Take(SurfaceMesh&& InputMesh);
    inline void Take(SurfaceMesh& InputMesh);
    inline bool Take(SurfaceMesh* InputMesh);

    //-------------------------------------------------------------------
    inline bool IsEmpty() const;
    inline int_max GetPointNumber() const;
    inline int_max GetEdgeNumber() const;
    inline int_max GetDirectedEdgeNumber() const;
    inline int_max GetCellNumber() const;
 
    // Get/Set 3D Position by PointHandleList or PointIDList --------------------------------------------------------------------------//

    inline void SetPointPosition(const DenseVector<PointHandleType>& PointHandleList, const DenseMatrix<ScalarType>& PointPositionMatrix);

    inline void SetPointPosition(const DenseVector<int_max>& PointIDList, const DenseMatrix<ScalarType>& PointPositionMatrix);

    inline DenseMatrix<ScalarType> GetPointPosition(const DenseVector<PointHandleType>& PointHandleList) const;
    inline void GetPointPosition(DenseMatrix<ScalarType>& PointPositionMatrix, const DenseVector<PointHandleType>& PointHandleList) const;

    inline DenseMatrix<ScalarType> GetPointPosition(const DenseVector<int_max>& PointIDList) const;
    inline void GetPointPosition(DenseMatrix<ScalarType>& PointPositionMatrix, const DenseVector<int_max>& PointIDList) const;

    //----- Get/Set Mesh Item {Point, , Edge, DirectedEdge, Cell} by using Handle or ID ------//

    inline PointType& Point(PointHandleType PointHandle);
    inline const PointType& Point(PointHandleType PointHandle) const;

    inline PointType& Point(int_max PointID);
    inline const PointType& Point(int_max PointID) const;

    inline EdgeType& Edge(EdgeHandleType EdgeHandle);
    inline const EdgeType& Edge(EdgeHandleType EdgeHandle) const;

    inline EdgeType& Edge(int_max EdgeID);
    inline const EdgeType& Edge(int_max EdgeID) const;

    inline DirectedEdgeType& DirectedEdge(DirectedEdgeHandleType DirectedEdgeHandle);
    inline const DirectedEdgeType& DirectedEdge(DirectedEdgeHandleType DirectedEdgeHandle) const;

    inline DirectedEdgeType& DirectedEdge(int_max DirectedEdgeID);
    inline const DirectedEdgeType& DirectedEdge(int_max DirectedEdgeID) const;

    inline CellType& Cell(CellHandleType CellHandle);
    inline const CellType& Cell(CellHandleType CellHandle) const;

    inline CellType& Cell(int_max CellID);
    inline const CellType& Cell(int_max CellID) const;

    //-------------- check handle -------------------------------------------------------//

    inline bool IsValidHandle(PointHandleType PointHandle) const;
    inline bool IsValidHandle(EdgeHandleType EdgeHandle) const;
    inline bool IsValidHandle(DirectedEdgeHandleType DirectedEdgeHandle) const;
    inline bool IsValidHandle(CellHandleType CellHandle) const;

    //--------- get HandleList ------------------------------------------------------------//

    inline DenseVector<PointHandleType> GetValidPointHandleList() const;
    inline void GetValidPointHandleList(DenseVector<PointHandleType>& OutputHandleList) const;

    inline DenseVector<EdgeHandleType> GetValidEdgeHandleList() const;
    inline void GetValidEdgeHandleList(DenseVector<EdgeHandleType>& OutputHandleList) const;

    inline DenseVector<DirectedEdgeHandleType> GetValidDirectedEdgeHandleList() const;
    inline void GetValidDirectedEdgeHandleList(DenseVector<DirectedEdgeHandleType>& OutputHandleList) const;

    inline DenseVector<CellHandleType> GetValidCellHandleList() const;
    inline void GetValidCellHandleList(DenseVector<CellHandleType>& OutputHandleList) const;

    //----------- get PointHandle by position ----------------------------------------------//

    inline PointHandleType GetPointHandle(ScalarType Position[3]) const;
    inline PointHandleType GetPointHandle(ScalarType x, ScalarType y, ScalarType z) const;

    //----------- get handle by ID -----------------------------------------------------------//

    inline PointHandleType  GetPointHandle(int_max PointID) const;
    inline EdgeHandleType   GetEdgeHandle(int_max EdgeID) const;
    inline DirectedEdgeHandleType GetDirectedEdgeHandle(int_max DirectedEdgeID) const;
    inline CellHandleType   GetCellHandle(int_max CellID) const;

    //----------- get EdgeHandle and DirectedEdgeHandle by PointHandleList or PointIDList ---------------------//

    inline EdgeHandleType GetEdgeHandle(PointHandleType PointHandle0, PointHandleType PointHandle1) const;
    inline EdgeHandleType GetEdgeHandle(int_max PointID0, int_max PointID1) const;

    inline DirectedEdgeHandleType GetDirectedEdgeHandle(PointHandleType PointHandle_start, PointHandleType PointHandle_end) const;
    inline DirectedEdgeHandleType GetDirectedEdgeHandle(int_max PointID_start, int_max PointID_end) const;

    //----------- get CellHandle by EdgeHandleList or EdgeIDList ------------------------------------------//

    inline CellHandleType GetCellHandle(const DenseVector<EdgeHandleType>& EdgeHandleList) const;
    inline CellHandleType GetCellHandle(const DenseVector<int_max>& EdgeIDList) const;

    //-------------- check ID -------------------------------------------------------//

    inline bool IsValidPointID(int_max PointID) const;
    inline bool IsValidEdgeID(int_max EdgeID) const;
    inline bool IsValidDirectedEdgeID(int_max DirectedEdgeID) const;
    inline bool IsValidCellID(int_max CellID) const;

    //--------- get IDList ------------------------------------------------------------//

    inline DenseVector<int_max> GetValidPointIDList() const;
    inline void GetValidPointIDList(DenseVector<int_max>& OutputIDList) const;

    inline DenseVector<int_max> GetValidEdgeIDList() const;
    inline void GetValidEdgeIDList(DenseVector<int_max>& OutputIDList) const;

    inline DenseVector<int_max> GetValidDirectedEdgeIDList() const;
    inline void GetValidDirectedEdgeIDList(DenseVector<int_max>& OutputIDList) const;

    inline DenseVector<int_max> GetValidCellIDList() const;
    inline void GetValidCellIDList(DenseVector<int_max>& OutputIDList) const;

    //----------- get ID by handle -----------------------------------------------------------//

    inline int_max GetPointID(PointHandleType PointHandle) const;
    inline int_max GetEdgeID(EdgeHandleType EdgeHandle) const;
    inline int_max GetDirectedEdgeID(DirectedEdgeHandleType DirectedEdgeHandle) const;
    inline int_max GetCellID(CellHandleType CellHandle) const;

    //----------- get PointID by position ----------------------------------------------//

    inline int_max GetPointID(ScalarType Position[3]) const;
    inline int_max GetPointID(ScalarType x, ScalarType y, ScalarType z) const;

    //----------- get EdgeID and DirectedEdgeID by PointHandleList or PointIDList ---------------------//

    inline int_max GetEdgeID(PointHandleType PointHandle0, PointHandleType PointHandle1) const;
    inline int_max GetEdgeID(int_max PointID0, int_max PointID1) const;

    inline int_max GetDirectedEdgeID(PointHandleType PointHandle_start, PointHandleType PointHandle_end) const;
    inline int_max GetDirectedEdgeID(int_max PointID_start, int_max PointID_end) const;

    //----------- get CellID by EdgeHandleList or EdgeIDList ------------------------------------------//

    inline int_max GetCellID(const DenseVector<EdgeHandleType>& EdgeHandleList) const;
    inline int_max GetCellID(const DenseVector<int_max>& EdgeIDList) const;

    //------------- Iterator --------------------------------------------------------------//

    inline PointIteratorType  GetIteratorOfPoint();
    inline const PointIteratorType  GetIteratorOfPoint() const;

    inline EdgeIteratorType   GetIteratorOfEdge();
    inline const EdgeIteratorType   GetIteratorOfEdge() const;

    inline DirectedEdgeIteratorType   GetIteratorOfDirectedEdge();
    inline const DirectedEdgeIteratorType   GetIteratorOfDirectedEdge() const;

    inline CellIteratorType   GetIteratorOfCell();
    inline const CellIteratorType   GetIteratorOfCell() const;

    // Add Mesh Item -------------------------------------------------------------------------//
    // add an item and return index (-1 if input is invalid)

    // add a Point and return PointHandle -> PointIndex in m_MeshData->PointList
    PointHandleType AddPoint(const ScalarType Position[3]);
    PointHandleType AddPoint(ScalarType x, ScalarType y, ScalarType z);
    
    // add a set of points and return PointHandleList
    DenseVector<PointHandleType> AddPoint(const DenseMatrix<ScalarType>& PointSet);

    // add an Edge and return EdgeIndex in m_MeshData->EdgeList
    // also create invalid DirectedEdge to hold place at  m_MeshData->DirectedEdgeList[EdgeIndex]
    // PointHandleList[0] and PointHandleList[1] are two  of the Edge
    // new  is added if necessary, so Add is called in this function if necessary
    EdgeHandleType AddEdge(PointHandleType PointHandle0, PointHandleType PointHandle1);
    EdgeHandleType AddEdge(int_max PointID0, int_max PointID1);

    // add a cell and return CellIndex in m_MeshData->CellList
    // add DirectedEdge of the cell
    // the order of Edge in EdgeHandleList determine the order of DirectedEdge
    CellHandleType AddCell(const DenseVector<EdgeHandleType>& EdgeHandleList);
    CellHandleType AddCell(const DenseVector<int_max>& EdgeIDList);

    // Delete Mesh Item ----------------------------------------------------------------------------//

    // m_MeshData->CellList(CellIndex).Clear() only clear memory
    // this function will remove/modify any mesh item related to the cell, including DirectedEdge of the Cell
    bool DeleteCell(CellHandleType CellHandle);
    bool DeleteCell(int_max CellID);

    // m_MeshData->EdgeList[EdgeIndex].Clear() only clear memory
    // this function will remove mesh item related to the Edge
    // If the edge is adjacent to a cell, then the Edge can not be deleted before the cell is deleted
    bool DeleteEdge(EdgeHandleType EdgeHandle);
    bool DeleteEdge(int_max EdgeID);

    // m_MeshData->PointList[PointIndex].Cear() only clear memory
    // If the Point is on an edge, then the Point can not be deleted before the Edge is deleted
    bool DeletePoint(PointHandleType PointHandle);
    bool DeletePoint(const DenseVector<PointHandleType>& PointHandleList);

    bool DeletePoint(int_max PointID);
    bool DeletePoint(const DenseVector<int_max>& PointIDList);

    // remove deleted item from object list ----------------------------------------------------------//
    // attention: after this function is called, handle may become invalid, but, ID will not change
    void CleanDataStructure();
    
    //---------------------------------------------------------------------------------------------------

};

}// namespace mdk

#include "mdkSurfaceMesh.hpp"

#endif