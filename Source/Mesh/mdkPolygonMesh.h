﻿#pragma once

#include <unordered_map>

#include "mdkPolygonMeshItem.h"
#include "mdkPolygonMeshItemIterator.h"
#include "mdkPolygonMeshStandardAttribute.h"

namespace mdk
{

//------------------------------- standard/empty MeshAttributeType -----------------------//
template<typename ScalarType>
struct PolygonMeshStandardAttributeType
{
    typedef ScalarType  ScalarType;
	typedef GlobalStandardAttribute_Of_PolygonMesh<ScalarType>             GlobalAttribute;
	typedef StandardAttribute_Of_Point_Of_PolygonMesh<ScalarType>          PointAttributeType;
	typedef StandardAttribute_Of_Edge_Of_PolygonMesh<ScalarType>           EdgeAttributeType;
	typedef StandardAttribute_Of_DirectedEdge_Of_PolygonMesh<ScalarType>   DirectedEdgeAttributeType;
	typedef StandardAttribute_Of_Face_Of_PolygonMesh<ScalarType>           FaceAttributeType;
};

template<typename ScalarType>
struct PolygonMeshEmptyAttributeType
{
	typedef ScalarType  ScalarType;
	typedef GlobalStandardAttribute_Of_PolygonMesh<ScalarType>             GlobalAttribute;
	typedef StandardAttribute_Of_Point_Of_PolygonMesh<ScalarType>          PointAttributeType;
	typedef StandardAttribute_Of_Edge_Of_PolygonMesh<ScalarType>           EdgeAttributeType;
	typedef StandardAttribute_Of_DirectedEdge_Of_PolygonMesh<ScalarType>   DirectedEdgeAttributeType;
	typedef StandardAttribute_Of_Face_Of_PolygonMesh<ScalarType>           FaceAttributeType;
};
//------------------------------------------------------------------------------------------------//

template<typename MeshAttributeType>
struct PolygonMeshData
{
    //-------------------------------------------------------------------------------------------//
    typedef typename MeshAttributeType::ScalarType      ScalarType;
    typedef typename MeshAttributeType::GlobalAttribute GlobalAttribute;
    //-------------------------------------------------------------------------------------------//

    DenseMatrix<ScalarType> PointPositionTable;
    // row_0: x
    // row_1: y
    // row_2: z
    // a point may or may not connect to other point by edge

	StdObjectVector<Point_Of_PolygonMesh<MeshAttributeType>> PointList;

    DenseVector<int_max> PointValidityFlagList; // do not use bool (std::vector<bool> is compressed)
    // 1: point is an element of the mesh 
    // 0: point is deleted

	StdObjectVector<Edge_Of_PolygonMesh<MeshAttributeType>> EdgeList;

    // do not use this, DirectedEdge_Of_PolygonMesh::operator(&&) will not work when append to the list
    //StdObjectVector<DenseVector<DirectedEdge_Of_PolygonMesh<MeshAttributeType>, 2>> DirectedEdgePairList;

    DenseVector<int_max> EdgeValidityFlagList;
    // 1: Edge is an element of the mesh 
    // 0: Edge is deleted

	StdObjectVector<Face_Of_PolygonMesh<MeshAttributeType>> FaceList; // also known as face, facet, element

    DenseVector<int_max>  FaceValidityFlagList;
    // 1: Face is an element of the mesh 
    // 0: Face is deleted

    // Attention: ID must >= 0
    std::unordered_map<int_max, int_max> Map_PointID_to_PointIndex;
    std::unordered_map<int_max, int_max> Map_EdgeID_to_EdgeIndex;
    std::unordered_map<int_max, DirectedEdgeIndex_Of_PolygonMesh> Map_DirectedEdgeID_to_DirectedEdgeIndex;
    std::unordered_map<int_max, int_max> Map_FaceID_to_FaceIndex;

	//----------------------------- addtional info --------------------------------------------------------------//
	int_max ID; // ID of the mesh
	String Name;// name of the mesh

	std::unordered_map<String, Handle_Of_Point_Of_PolygonMesh, StringHash<String>> Map_PointName_to_PointHandle;
	std::unordered_map<String, Handle_Of_Face_Of_PolygonMesh, StringHash<String>>  Map_FaceName_to_FaceHandle;

	StdObjectVector<DenseVector<Handle_Of_Point_Of_PolygonMesh>>  PointSetList;// PointSetList[k] is PointHandleList Of a PointSet indexed by k
	std::unordered_map<String, int_max, StringHash<String>> Map_PointSetName_to_PointSetIndex;

	StdObjectVector<DenseVector<Handle_Of_Face_Of_PolygonMesh>>  FaceSetList; // FaceSetList[k] is FaceHandleList Of a FaceSet indexed by k
	std::unordered_map<String, int_max, StringHash<String>> Map_FaceSetName_to_FaceSetIndex;

    //Mesh Attribute
    GlobalAttribute Attribute;
};


template<typename MeshAttribute_Type>
class PolygonMesh : public Object
{
public:
	typedef MeshAttribute_Type MeshAttributeType;
    //-------------------------------------------------------------------------------------------//
    typedef typename MeshAttributeType::ScalarType                ScalarType;
    typedef typename MeshAttributeType::GlobalAttribute           GlobalAttribute;
    typedef typename MeshAttributeType::PointAttributeType        PointAttributeType;
    typedef typename MeshAttributeType::EdgeAttributeType         EdgeAttributeType;
    typedef typename MeshAttributeType::DirectedEdgeAttributeType DirectedEdgeAttributeType;
    typedef typename MeshAttributeType::FaceAttributeType         FaceAttributeType;
    //--------------------------------------------------------------------------------------------//
    typedef Point_Of_PolygonMesh<MeshAttributeType>           PointType;
    typedef Edge_Of_PolygonMesh<MeshAttributeType>            EdgeType;
    typedef DirectedEdge_Of_PolygonMesh<MeshAttributeType>    DirectedEdgeType;
    typedef Face_Of_PolygonMesh<MeshAttributeType>            FaceType;

    typedef Handle_Of_Point_Of_PolygonMesh          PointHandleType;
    typedef Handle_Of_Edge_Of_PolygonMesh           EdgeHandleType;
    typedef Handle_Of_DirectedEdge_Of_PolygonMesh   DirectedEdgeHandleType;
    typedef Handle_Of_Face_Of_PolygonMesh           FaceHandleType;

    typedef Iterator_Of_Point_Of_PolygonMesh<MeshAttributeType>           PointIteratorType;
    typedef Iterator_Of_Edge_Of_PolygonMesh<MeshAttributeType>            EdgeIteratorType;
    typedef Iterator_Of_DirectedEdge_Of_PolygonMesh<MeshAttributeType>    DirectedEdgeIteratorType;
    typedef Iterator_Of_Face_Of_PolygonMesh<MeshAttributeType>            FaceIteratorType;
    //--------------------------------------------------------------------------------------------------//
	typedef StdObjectVector<Point_Of_PolygonMesh<MeshAttributeType>>          PointListType;
	typedef StdObjectVector<Edge_Of_PolygonMesh<MeshAttributeType>>           EdgeListType;
	typedef StdObjectVector<DirectedEdge_Of_PolygonMesh<MeshAttributeType>>   DirectedEdgeListType;
	typedef StdObjectVector<Face_Of_PolygonMesh<MeshAttributeType>>           FaceListType;
	//--------------------------------------------------------------------------------------------------//

protected:
    std::shared_ptr<PolygonMeshData<MeshAttributeType>> m_MeshData;

protected:
    template<typename T>
    friend class Point_Of_PolygonMesh;

    template<typename T>
    friend struct Iterator_Of_Point_Of_PolygonMesh;

    template<typename T>
    friend class Edge_Of_PolygonMesh;

    template<typename T>
    friend struct Iterator_Of_Edge_Of_PolygonMesh;

    template<typename T>
    friend class DirectedEdge_Of_PolygonMesh;

    template<typename T>
    friend struct Iterator_Of_DirectedEdge_Of_PolygonMesh;

    template<typename T>
    friend class Face_Of_PolygonMesh;    

    template<typename T>
    friend struct Iterator_Of_Face_Of_PolygonMesh;

public:
    PolygonMesh();
    PolygonMesh(const MDK_Symbol_PureEmpty&);
    PolygonMesh(const PolygonMesh& InputMesh);
    PolygonMesh(PolygonMesh&& InputMesh);
    ~PolygonMesh();

    inline void operator=(const PolygonMesh& InputMesh);
    inline void operator=(PolygonMesh&& InputMesh);

    inline void Clear();
	inline void Clear(const MDK_Symbol_PureEmpty&);

    inline void Copy(const PolygonMesh& InputMesh);
	inline void Copy(PolygonMesh&& InputMesh);
	
	inline void Share(PolygonMesh& InputMesh);
    inline void ForceShare(const PolygonMesh& InputMesh);

	void Recreate();

    //-------------------------------------------------------------------
    inline bool IsEmpty() const;
	inline bool IsPureEmpty() const;

    inline int_max GetPointCount() const;
    inline int_max GetEdgeCount() const;
    inline int_max GetDirectedEdgeCount() const;
    inline int_max GetFaceCount() const;
 
	inline int_max GetMaxValueOfPointIndex() const;// the maximum value of PointHanlde.GetIndex()
	inline int_max GetMaxValueOfEdgeIndex() const; // the maximum value of EdgeHanlde.GetIndex()	
	inline int_max GetMaxValueOfFaceIndex() const; // the maximum value of FaceHanlde.GetIndex()

    //------ Get/Set GlobalAttribute -----------------------------------//

    inline GlobalAttribute& Attribute();
    inline const GlobalAttribute& Attribute() const;

	//------------- Get All the position (valid point) --------------------------------//

	inline DenseMatrix<ScalarType> GetPointPosition(const MDK_Symbol_ALL&) const;
	inline void GetPointPosition(const MDK_Symbol_ALL&, DenseMatrix<ScalarType>& PositionMatrix) const;

    // Get/Set 3D Position by PointHandle --------------------------------------------------------------------------//

    inline void SetPointPosition(PointHandleType PointHandle, ScalarType x, ScalarType y, ScalarType z);
    inline void SetPointPosition(PointHandleType PointHandle, const ScalarType Position[3]);

    inline DenseVector<ScalarType, 3> GetPointPosition(PointHandleType PointHandle) const;
    inline void GetPointPosition(PointHandleType PointHandle, ScalarType& x, ScalarType& y, ScalarType& z) const;
    inline void GetPointPosition(PointHandleType PointHandle, ScalarType Position[3]) const;

    inline void SetPointPosition(const DenseVector<PointHandleType>& PointHandleList, const DenseMatrix<ScalarType>& PointPositionMatrix);

    inline DenseMatrix<ScalarType> GetPointPosition(const DenseVector<PointHandleType>& PointHandleList) const;
    inline void GetPointPosition(const DenseVector<PointHandleType>& PointHandleList, DenseMatrix<ScalarType>& PointPositionMatrix) const;
    
    //----- Get/Set Mesh Item {Point, , Edge, DirectedEdge, Face} by using Handle ------//

    inline PointType& Point(PointHandleType PointHandle);
    inline const PointType& Point(PointHandleType PointHandle) const;

    inline EdgeType& Edge(EdgeHandleType EdgeHandle);
    inline const EdgeType& Edge(EdgeHandleType EdgeHandle) const;

    inline DirectedEdgeType& DirectedEdge(DirectedEdgeHandleType DirectedEdgeHandle);
    inline const DirectedEdgeType& DirectedEdge(DirectedEdgeHandleType DirectedEdgeHandle) const;

    inline FaceType& Face(FaceHandleType FaceHandle);
    inline const FaceType& Face(FaceHandleType FaceHandle) const;

    //-------------- check handle -------------------------------------------------------//

    inline bool IsValidHandle(PointHandleType PointHandle) const;
    inline bool IsValidHandle(EdgeHandleType EdgeHandle) const;
    inline bool IsValidHandle(DirectedEdgeHandleType DirectedEdgeHandle) const;
    inline bool IsValidHandle(FaceHandleType FaceHandle) const;

    //--------- get HandleList ------------------------------------------------------------//

    inline DenseVector<PointHandleType> GetPointHandleList() const;
    inline void GetPointHandleList(DenseVector<PointHandleType>& OutputHandleList) const;

    inline DenseVector<EdgeHandleType> GetEdgeHandleList() const;
    inline void GetEdgeHandleList(DenseVector<EdgeHandleType>& OutputHandleList) const;

    inline DenseVector<DirectedEdgeHandleType> GetDirectedEdgeHandleList() const;
    inline void GetDirectedEdgeHandleList(DenseVector<DirectedEdgeHandleType>& OutputHandleList) const;

    inline DenseVector<FaceHandleType> GetFaceHandleList() const;
    inline void GetFaceHandleList(DenseVector<FaceHandleType>& OutputHandleList) const;

    //----------- get PointHandle by Position, ID, ----------------------------------------------//

    inline PointHandleType GetPointHandleByPosition(const DenseVector<ScalarType, 3>& Position,
                                                    ScalarType DistanceThreshold = std::numeric_limits<ScalarType>::epsilon()) const;
    inline PointHandleType GetPointHandleByPosition(const ScalarType Position[3], 
                                                    ScalarType DistanceThreshold = std::numeric_limits<ScalarType>::epsilon()) const;
    inline PointHandleType GetPointHandleByPosition(ScalarType x, ScalarType y, ScalarType z,
                                                    ScalarType DistanceThreshold = std::numeric_limits<ScalarType>::epsilon()) const;
    inline PointHandleType GetPointHandleByID(int_max PointID) const;

    //----------- get EdgeHandle by ID, PointHandleList, PointIDList -------------------------------------//

    inline EdgeHandleType GetEdgeHandleByID(int_max EdgeID) const;
    inline EdgeHandleType GetEdgeHandleByPoint(PointHandleType PointHandle0, PointHandleType PointHandle1) const;
    inline EdgeHandleType GetEdgeHandleByPoint(int_max PointID0, int_max PointID1) const;

    //----------- get DirectedEdgeHandle by ID, PointHandleList, PointIDList -------------------------------//

    inline DirectedEdgeHandleType GetDirectedEdgeHandleByID(int_max DirectedEdgeID) const;

	// non-manifold is allowed: maybe more than one DirectedEdge from PointHandle_start to PointHandle_end
	//                          there are many face containing PointHandle_start and  PointHandle_end
	//                          each face has a DirectedEdge from PointHandle_start to PointHandle_end
    inline DenseVector<DirectedEdgeHandleType> GetDirectedEdgeHandleListByPoint(PointHandleType PointHandle_start, PointHandleType PointHandle_end) const;

    //----------- get FaceHandle by ID, PointHandleList or PointIDList EdgeHandleList or EdgeIDList ----------//

    inline FaceHandleType GetFaceHandleByID(int_max FaceID) const;

	// PointHandleList/PointIDList/EdgeHandleList/EdgeIDList can have random order of point/edge
    inline FaceHandleType GetFaceHandleByPoint(const DenseVector<PointHandleType>& PointHandleList) const;
    inline FaceHandleType GetFaceHandleByEdge(const DenseVector<EdgeHandleType>& EdgeHandleList) const;

    //-------------- check ID -------------------------------------------------------//

    inline bool IsValidPointID(int_max PointID) const;
    inline bool IsValidEdgeID(int_max EdgeID) const;
    inline bool IsValidDirectedEdgeID(int_max DirectedEdgeID) const;
    inline bool IsValidFaceID(int_max FaceID) const;

    //--------- get IDList ------------------------------------------------------------//

    inline DenseVector<int_max> GetPointIDList() const;
    inline void GetPointIDList(DenseVector<int_max>& OutputIDList) const;

    inline DenseVector<int_max> GetEdgeIDList() const;
    inline void GetEdgeIDList(DenseVector<int_max>& OutputIDList) const;

    inline DenseVector<int_max> GetDirectedEdgeIDList() const;
    inline void GetDirectedEdgeIDList(DenseVector<int_max>& OutputIDList) const;

    inline DenseVector<int_max> GetFaceIDList() const;
    inline void GetFaceIDList(DenseVector<int_max>& OutputIDList) const;

    //----------- get Point ID by Handle, Position -----------------------------------------------------------//

    inline int_max GetPointIDByHandle(PointHandleType PointHandle) const;
    inline int_max GetPointIDByPosition(ScalarType Position[3]) const;
    inline int_max GetPointIDByPosition(ScalarType x, ScalarType y, ScalarType z) const;

	inline DenseVector<int_max> GetPointIDListByHandleList(const DenseVector<PointHandleType>& PointHandleList) const;

    //----------- get Edge ID by EdgeHandle, PointHandleList, PointIDList ------------------------------------------------------//

    inline int_max GetEdgeIDByHandle(EdgeHandleType EdgeHandle) const;
    inline int_max GetEdgeIDByPoint(PointHandleType PointHandle0, PointHandleType PointHandle1) const;

	inline DenseVector<int_max> GetEdgeIDListByHandleList(const DenseVector<EdgeHandleType>& EdgeHandleList) const;

    //----------- get DirectedEdge ID by DirectedEdgeHandle, PointHandle, PointID -------------------------------------------//

    inline int_max GetDirectedEdgeIDByHandle(DirectedEdgeHandleType DirectedEdgeHandle) const;
    inline int_max GetDirectedEdgeIDByPoint(PointHandleType PointHandle_start, PointHandleType PointHandle_end) const;

	inline DenseVector<int_max> GetDirectedEdgeIDListByHandleList(const DenseVector<DirectedEdgeHandleType>& DirectedEdgeHandleList) const;

    //----------- get FaceID by FaceHandle, EdgeHandleList, EdgeIDList, PointHandleList, PointIDList --------------------------//

    inline int_max GetFaceIDByHandle(FaceHandleType FaceHandle) const;
    inline int_max GetFaceIDByPoint(const DenseVector<PointHandleType>& PointHandleList) const;
    inline int_max GetFaceIDByEdge(const DenseVector<EdgeHandleType>& EdgeHandleList) const;

	inline DenseVector<int_max> GetFaceIDListByHandleList(const DenseVector<FaceHandleType>& FaceHandleList) const;

    //------------- Iterator --------------------------------------------------------------//

    inline PointIteratorType  GetIteratorOfPoint();
    inline const PointIteratorType  GetIteratorOfPoint() const;

    inline EdgeIteratorType   GetIteratorOfEdge();
    inline const EdgeIteratorType   GetIteratorOfEdge() const;

    inline DirectedEdgeIteratorType   GetIteratorOfDirectedEdge();
    inline const DirectedEdgeIteratorType   GetIteratorOfDirectedEdge() const;

    inline FaceIteratorType   GetIteratorOfFace();
    inline const FaceIteratorType   GetIteratorOfFace() const;

	//------------ SetCapacity, ReleaseUnusedCapacity -------------------------------------//

	void SetCapacity(int_max PointCount, int_max EdgeCount, int_max FaceCount);

	void ReleaseUnusedCapacity();

    // Add Mesh Item -------------------------------------------------------------------------//
    // add an item and return index (-1 if input is invalid)

    // add a Point and return PointHandle -> PointIndex in m_MeshData->PointList
    PointHandleType AddPoint(const DenseVector<ScalarType, 3>& Position);
    PointHandleType AddPoint(const DenseVector<ScalarType>& Position);
    PointHandleType AddPoint(const DenseMatrix<ScalarType>& Position);
    PointHandleType AddPoint(const ScalarType Position[3]);
    PointHandleType AddPoint(ScalarType x, ScalarType y, ScalarType z);
    
    // add a set of points and return PointHandleList
    DenseVector<PointHandleType> AddPoint_batch(const DenseMatrix<ScalarType>& PointSet);
	DenseVector<PointHandleType> AddPoint_batch(DenseMatrix<ScalarType>&& PointSet);

    // add an Edge and return EdgeHandle -> EdgeIndex in m_MeshData->EdgeList
    // also create invalid DirectedEdge to hold place at m_MeshData->DirectedEdgeList[EdgeIndex]
    // Check is performed in the function to ensure that an edge will not be added more than once
    EdgeHandleType AddEdge(PointHandleType PointHandle0, PointHandleType PointHandle1);

	// EdgeHandle_input can be the handle of a deleted edge, so to reuse old EdgeIndex if necessary
	EdgeHandleType AddEdge(PointHandleType PointHandle0, PointHandleType PointHandle1, EdgeHandleType EdgeHandle_input);

    // add a face and return FaceHandle -> FaceIndex in m_MeshData->FaceList
    // add DirectedEdge of the face
    // the order of Edge in EdgeHandleList determine the direction of each DirectedEdge and the direction/sign of the normal vector
    FaceHandleType AddFaceByEdge(const DenseVector<EdgeHandleType>& EdgeHandleList);

	//FaceHandle_input may be the handle of a deleted face, so to reused old FaceIndex if necessary
	FaceHandleType AddFaceByEdge(const DenseVector<EdgeHandleType>& EdgeHandleList, FaceHandleType FaceHandle_input);

    // Add Face with PointHandleList, Point0 <- Edge0 -> Point1 <- Edge1 -> Point2 ... Point_end <- Edge_end -> Point0
    // in this function, AddFaceByEdge() is called if necessary
    FaceHandleType AddFaceByPoint(const DenseVector<PointHandleType>& PointHandleList);

	//FaceHandle_input may be the handle of a deleted face, so to reused old FaceIndex if necessary
	FaceHandleType AddFaceByPoint(const DenseVector<PointHandleType>& PointHandleList, FaceHandleType FaceHandle_input);

	// modify Mesh additional Info ------------------------------------------------------------------------//

	void SetID(int_max ID);
	int_max GetID() const;
	void SetName(String Name);
	String GetName() const;

	int_max GetNamedPointCount() const;
	int_max GetNamedFaceCount() const;
	int_max GetPointSetCount() const;
	int_max GetFaceSetCount() const;

	void SetPointName(PointHandleType PointHandle, const String& PointName);
	String GetPointName(PointHandleType PointHandle) const;
	PointHandleType GetPointHandleByName(const String& PointName) const;	
	ObjectArray<std::pair<String, PointHandleType>> GetPointNameHandlePair(MDK_Symbol_ALL&) const;
	ObjectArray<String> GetAvailablePointName(MDK_Symbol_ALL&) const;

	int_max SetPointSet(const String& PointSetName, DenseVector<PointHandleType> PointSet);
	int_max GetPointSetIndex(const String& PointSetName) const;
	String GetPointSetName(int_max PointSetIndex) const;
	DenseVector<PointHandleType> GetPointSet(int_max PointSetIndex) const;	
	DenseVector<PointHandleType> GetPointSet(const String& PointSetName) const;
	ObjectArray<String> GetPointSetName(MDK_Symbol_ALL&) const;
	ObjectArray<DenseVector<PointHandleType>> GetPointSet(MDK_Symbol_ALL&) const;

	void SetFaceName(FaceHandleType FaceHandle, const String& FaceName);
	String GetFaceName(FaceHandleType FaceHandle) const;
	FaceHandleType GetFaceHandleByName(const String& FaceName) const;
	ObjectArray<std::pair<String, FaceHandleType>> GetFaceNameHandlePair(MDK_Symbol_ALL&) const;
	ObjectArray<String> GetAvailableFaceName(MDK_Symbol_ALL&) const;
	
	int_max SetFaceSet(const String& FaceSetName, DenseVector<FaceHandleType> FaceSet);
	int_max GetFaceSetIndex(const String& FaceSetName) const;
	String GetFaceSetName(int_max PointSetIndex) const;
	DenseVector<FaceHandleType> GetFaceSet(int_max FaceSetIndex) const;
	DenseVector<FaceHandleType> GetFaceSet(const String& FaceSetName) const;
    ObjectArray<String> GetFaceSetName(MDK_Symbol_ALL&) const;
	ObjectArray<DenseVector<FaceHandleType>> GetFaceSet(MDK_Symbol_ALL&) const;

    // Delete Mesh Item ----------------------------------------------------------------------------//

    // m_MeshData->FaceList[FaceIndex].Clear() only release memory
    // this function will delete each DirectedEdge of the Face, and modify any information related to the face
    // FaceHandle and FaceID of the face become invalid after the face is deleted
	void DeleteFace(FaceHandleType FaceHandle);

    // m_MeshData->EdgeList[EdgeIndex].Clear() only release memory
    // this function will modify any information related to the Edge
    // EdgeHandle and EdgeID of the edge become invalid after the edge is deleted
    // Check is performed in the function to make sure an edge can not be deleted if any adjacent face exit
	//
	// note:  Call Edge(EdgeHandle).GetAdjacentFaceCount() to check if the edge can be deleted or not
	void DeleteEdge(EdgeHandleType EdgeHandle);

    // m_MeshData->PointList[PointIndex].Cear() only release memory
    // this function will modify any information related to the point
    // Check is performed in the function to make sure a point can not be deleted if any adjacent edge exit	
    // PointHandle and PointID of the point become invalid after the point is deleted
	//
	// note: Call Point(PointHandle).IsOnEdge() to check if the point can be deleted or not
	void DeletePoint(PointHandleType PointHandle);
	void DeletePoint(const DenseVector<PointHandleType>& PointHandleList);

    // remove deleted item from object list ----------------------------------------------------------//
    // attention: after CleanDataStructure() is called, handle may become invalid, but, ID will not change
    // use this function when InvalidPointHandleCount/ValidPointCount( GetPointCount() ) > 0.5
    void CleanDataStructure();	
	void CleanDataStructure(DenseVector<int_max>& PointIndexMap_Old_To_New, DenseVector<int_max>& EdgeIndexMap_Old_To_New, DenseVector<int_max>& FaceIndexMap_Old_To_New);
	bool Check_If_DataStructure_is_Clean() const;//true: clean, false: invalid handle exit
    int_max GetDeletedPointCount() const; // the number of Deleted points
	int_max GetDeletedEdgeCount() const; // the number of Deleted edges
	int_max GetDeletedFaceCount() const; // the number of Deleted faces

   //------------ Construct from input data ------------------------------------//

	void Construct(DenseMatrix<ScalarType> InputPointPositionMatrix, const ObjectArray<DenseVector<int_max>>& InputFaceTable);
	// index order in each PointIndexList should be consistent

	//--------------------- output -------------------------------------------------//

	std::pair<DenseMatrix<ScalarType>, ObjectArray<DenseVector<int_max>>> GetPointPositionMatrixAndFaceTable() const;

	void GetPointPositionMatrixAndFaceTable(DenseMatrix<ScalarType>& PointPositionTable, ObjectArray<DenseVector<int_max>>& FaceTable) const;

	//--- check --------------------------//
	bool CheckIfTriangleMesh() const;

    //---------------------------------------------------------------------------------------------------

    // get a sub mesh by FaceHandleList
    PolygonMesh<MeshAttributeType> GetSubMeshByFace(const DenseVector<FaceHandleType>& FaceHandleList) const;

    // other basic operation ----------------------------------------------------------------------------------------
	
	// change P0->P1->P2->...->Pn to Pn->...->P2->P1
	// change AddFaceByPoint({P0, P1, P2,...,Pn}) to AddFaceByPoint({Pn,..., P2, P1, P0})
	void ReversePointOrderOfFace(FaceHandleType FaceHandle);

	// swap PointA and PointB: move A to B, move B to A, NOT change the mesh topology
	// A, B, C, D are point ID/name
	//------------------------
	//   C          C
	//   |          |    
	//   A   B  =>  B   A
	//       |          |
	//       D          D
    //------------------------	
	bool SwapPoint(PointHandleType PointHandleA, PointHandleType PointHandleB);

	// swap the connection: NOT move A or B, Change the mesh topology
	// A, B, C, D are point ID/name
	//------------------------
	//   C         C 
	//   |           \ 
	//   A   B  => A   B
	//       |       \
	//       D         D
    //------------------------	
	bool SwapConnectivityOfPoint(PointHandleType PointHandleA, PointHandleType PointHandleB);

	// merge the connection of PointB to the connection of PointA, then PointB become an isolated point
	// if an edge between A and B exist, then it will be deleted
	// return true if success
	// can NOT merge two point of a triangle face: return false
	//--------------------------------
	//    |   |          | /
	// ---A---B =>    ---A   B
	//    |   |          | \
    //-------------------------------
	bool MergeConnectivityOfPoint(PointHandleType PointHandleA, PointHandleType PointHandleB);

	// PointHandle may not be on the edge
	// return true if success
	bool ShrinkEdgeToPoint(EdgeHandleType EdgeHandle, PointHandleType PointHandle);

	DenseVector<EdgeHandleType, 2> SplitEdgeByPoint(EdgeHandleType EdgeHandle, PointHandleType PointHandle);

	// PointHandle may not be on the face
	// return true if success
	// can NOT shrink if it lead to merge of two point of a triangle face: return false
	bool ShrinkFaceToPoint(FaceHandleType FaceHandle, PointHandleType PointHandle);

	/*
	FaceHandleType MergeAdjacentFace(FaceHandleType FaceHandleA, FaceHandleType FaceHandleB);

	DenseVector<FaceHandleType, 2> SplitFaceByEdge(EdgeHandleType EdgeHandle);
	*/

	//-----------------------------------------------------------------------------------------------------

	template<typename ElementType>
	DenseVector<ElementType> Intersect(const DenseVector<ElementType>& SetA, const DenseVector<ElementType>& SetB) const;

	template<typename ElementType>
	DenseVector<ElementType> SetDiff(const DenseVector<ElementType>& SetA, const DenseVector<ElementType>& SetB) const;//A-B

	//-----------------------------------------------------------------------------------------------------

	template<typename HandleType>
	inline HandleType ConvertIndexToHandle(int_max Index) const;

	template<typename HandleType>
	DenseVector<HandleType> ConvertIndexToHandle(const DenseVector<int_max>& IndexList) const;

	inline PointHandleType ConvertPointIndexToPointHandle(int_max PointIndex) const;
	DenseVector<PointHandleType> ConvertPointIndexToPointHandle(const DenseVector<int_max>& PointIndexList) const;

	inline EdgeHandleType ConvertEdgeIndexToEdgeHandle(int_max EdgeIndex) const;
	DenseVector<EdgeHandleType> ConvertEdgeIndexToEdgeHandle(const DenseVector<int_max>& EdgeIndexList) const;

	inline FaceHandleType ConvertFaceIndexToFaceHandle(int_max FaceIndex) const;
	DenseVector<FaceHandleType> ConvertFaceIndexToFaceHandle(const DenseVector<int_max>& FaceIndexList) const;

	inline int_max ConvertHandleToIndex(PointHandleType PointHandle) const;
	DenseVector<int_max> ConvertHandleToIndex(const DenseVector<PointHandleType>& PointHandleList) const;

	inline int_max ConvertHandleToIndex(EdgeHandleType EdgeHandle) const;
	DenseVector<int_max> ConvertHandleToIndex(const DenseVector<EdgeHandleType>& EdgeHandleList) const;

	inline int_max ConvertHandleToIndex(FaceHandleType FaceHandle) const;
	DenseVector<int_max> ConvertHandleToIndex(const DenseVector<FaceHandleType>& FaceHandleList) const;

	//------------- Mesh Attribute --------------------------------------------------//
	/*
	void UpdateNormalAtFace(); // all
	void UpdateNormalAtFace(FaceHandleType FaceHandle);
	void UpdateNormalAtFace(int_max FaceID);

	void UpdateAreaOfFace(); // all
	void UpdateAreaOfFace(FaceHandleType FaceHandle);
	void UpdateAreaOfFace(int_max FaceID);

	void UpdateCornerAngleOfFace(); // all
	void UpdateCornerAngleOfFace(FaceHandleType FaceHandle);
	void UpdateCornerAngleOfFace(int_max FaceID);

	void UpdateAngleWeightedNormalAtPoint(); // all
	void UpdateAngleWeightedNormalAtPoint(PointHandleType PointHandle);
	void UpdateAngleWeightedNormalAtPoint(int_max PointID);

	void UpdateGaussianCurvatureAtPoint(); // all
	void UpdateGaussianCurvatureAtPoint(PointHandleType PointHandle);
	void UpdateGaussianCurvatureAtPoint(int_max PointID);

	void UpdateMeanCurvatureAtPoint(); // all
	void UpdateMeanCurvatureAtPoint(PointHandleType PointHandle);
	void UpdateMeanCurvatureAtPoint(int_max PointID);
	*/

protected:
    void UpdateRecord_DeletePoint(int_max PointIndex);
    void UpdateRecord_DeleteEdge(int_max EdgeIndex);
    void UpdateRecord_DeleteFace(int_max FaceIndex);
	
	// the DirectedEdge is deleted in this function
	void UpdateRecord_DeleteDirectedEdge(DirectedEdgeIndex_Of_PolygonMesh DirectedEdgeIndex_delete);
		
	// DirectedEdge_insert has complete information including index, adjacency, etc
	void UpdateRecord_InsertDirectedEdge(DirectedEdgeType DirectedEdge_insert);

};

}// namespace mdk

#include "mdkPolygonMesh.hpp"
#include "mdkPolygonMesh_FileIO.h"