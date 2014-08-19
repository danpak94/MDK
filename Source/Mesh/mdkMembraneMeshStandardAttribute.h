#ifndef __mdkMembraneMeshStandardAttribute_h
#define __mdkMembraneMeshStandardAttribute_h

namespace mdk
{
//============================================== GlobalAttribute_Of_MembraneMesh ===========================================//

template<typename ScalarType>
struct GlobalStandardAttribute_Of_MembraneMesh
{
	GlobalStandardAttribute_Of_MembraneMesh() {}
	GlobalStandardAttribute_Of_MembraneMesh(const GlobalStandardAttribute_Of_MembraneMesh&) {}
	~GlobalStandardAttribute_Of_MembraneMesh() {}
	void operator=(const GlobalStandardAttribute_Of_MembraneMesh&) {}
    void Clear() {}
};

//============================================== PointAttribute_Of_MembraneMesh ===========================================//

template<typename ScalarType>
struct StandardAttribute_Of_Point_Of_MembraneMesh
{
	StandardAttribute_Of_Point_Of_MembraneMesh() {}
	StandardAttribute_Of_Point_Of_MembraneMesh(const StandardAttribute_Of_Point_Of_MembraneMesh&) {}
	~StandardAttribute_Of_Point_Of_MembraneMesh() {}
	void operator=(const StandardAttribute_Of_Point_Of_MembraneMesh&) {}
    void Clear() {}
};

//============================================== EdgeAttribute_Of_MembraneMesh ===========================================//

template<typename ScalarType>
struct StandardAttribute_Of_Edge_Of_MembraneMesh
{
	StandardAttribute_Of_Edge_Of_MembraneMesh() {}
	StandardAttribute_Of_Edge_Of_MembraneMesh(const StandardAttribute_Of_Edge_Of_MembraneMesh&) {}
	~StandardAttribute_Of_Edge_Of_MembraneMesh() {}
	void operator=(const StandardAttribute_Of_Edge_Of_MembraneMesh) {}
    virtual void Clear(){}
};

//============================================== DirectedEdgeAttribute_Of_MembraneMesh ===========================================//
template<typename ScalarType>
struct StandardAttribute_Of_DirectedEdge_Of_MembraneMesh
{
	StandardAttribute_Of_DirectedEdge_Of_MembraneMesh() {}
	StandardAttribute_Of_DirectedEdge_Of_MembraneMesh(const StandardAttribute_Of_DirectedEdge_Of_MembraneMesh&) {}
	~StandardAttribute_Of_DirectedEdge_Of_MembraneMesh() {}
	void operator=(const StandardAttribute_Of_DirectedEdge_Of_MembraneMesh&) {}
    void Clear() {}
};

//============================================== CellAttribute_Of_MembraneMesh ===========================================//

template<typename ScalarType>
struct StandardAttribute_Of_Cell_Of_MembraneMesh
{
	StandardAttribute_Of_Cell_Of_MembraneMesh() {}
	StandardAttribute_Of_Cell_Of_MembraneMesh(const StandardAttribute_Of_Cell_Of_MembraneMesh& InputAttribute) {}
	~StandardAttribute_Of_Cell_Of_MembraneMesh() {}
	void operator=(const StandardAttribute_Of_Cell_Of_MembraneMesh&) {}
    void Clear() {}
};

}// namespace mdk


#endif