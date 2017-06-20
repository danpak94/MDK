#include "mdkFileIO.h"
#include "mdkTriangleMesh.h"

using namespace mdk;

void Test_MeshConstruction()
{
	std::string FilePathAndName = "C:/Research/Test_VTK/Subdivision/Build/Leaflet_A_Triangle.vtk";

	TriangleMesh<double> LeafletMesh;
	LoadPolygonMeshFromVTKFile(LeafletMesh, FilePathAndName);

    //SavePolygonMeshAsJsonDataFile(LeafletMesh, "C:/Research/Test_VTK/Subdivision/Build/Leaflet_A_Triangle.json");
    SavePolygonMeshAsVTKFile(LeafletMesh, "C:/Research/Test_VTK/Subdivision/Build/Leaflet_A_Triangle_test.vtk");
}

void Test_PointDataSet()
{
	std::string FilePathAndName = "C:/Research/MDK/MDK_Build/Test/Test_Mesh/Test_TriangleMesh/TestData/Leaflet.vtk";

	TriangleMesh<double> LeafletMesh;
	LoadPolygonMeshFromVTKFile(LeafletMesh, FilePathAndName);

	LeafletMesh.SetPointDataSet("Stress", 6);	
	LeafletMesh.SetFaceDataSet("Stress", 6);

	//SavePolygonMeshAsJsonDataFile(LeafletMesh, "C:/Research/Test_VTK/Subdivision/Build/Leaflet_A_Triangle.json");
	SavePolygonMeshAsVTKFile(LeafletMesh, "C:/Research/MDK/MDK_Build/Test/Test_Mesh/Test_TriangleMesh/TestData/Leaflet_Test_PointDataSet.vtk");
}

void Test_MeshAttribute()
{
	std::string FilePathAndName = "C:/Research/Test_VTK/Subdivision/Build/Leaflet_A_Triangle.vtk";

	TriangleMesh<double> LeafletMesh;
	LoadPolygonMeshFromVTKFile(LeafletMesh, FilePathAndName);

	auto Normal = LeafletMesh.Point(0).Attribute().AngleWeightedNormal;
}

void Test_ShrinkEdge()
{
	std::string FilePathAndName = "C:/Research/MDK/MDK_Build/Test/Test_Mesh/Test_TriangleMesh/TestData/Leaflet";

	TriangleMesh<double> LeafletMesh;
	LoadPolygonMeshFromVTKFile(LeafletMesh, FilePathAndName + ".vtk");

	auto EdgeHandleList = LeafletMesh.GetEdgeIndexList();

	LeafletMesh.ShrinkEdgeToPoint(EdgeHandleList[0], 0);
	SavePolygonMeshAsVTKFile(LeafletMesh, FilePathAndName + "_ShrinkEdge.vtk");
}


void Test_ShrinkFace()
{
	std::string FilePathAndName = "C:/Research/MDK/MDK_Build/Test/Test_Mesh/Test_TriangleMesh/TestData/Leaflet";

	TriangleMesh<double> LeafletMesh;
	LoadPolygonMeshFromVTKFile(LeafletMesh, FilePathAndName + ".vtk");

	auto FaceHandleList = LeafletMesh.GetFaceIndexList();

	LeafletMesh.ShrinkFaceToPoint(FaceHandleList[1], 0);
	SavePolygonMeshAsVTKFile(LeafletMesh, FilePathAndName + "_ShrinkFace.vtk");
}

void Test_GlobalAttribute()
{
	std::string FilePathAndName = "C:/Research/MDK/MDK_Build/Test/Test_Mesh/Test_TriangleMesh/TestData/Square";

	TriangleMesh<double> InputMesh, SquareMesh;
	LoadPolygonMeshFromVTKFile(InputMesh, FilePathAndName + ".vtk");

	InputMesh.SetID(1);
	InputMesh.SetName("Square");
	InputMesh.Point(0).SetName("0_name");
	InputMesh.Face(0).SetName("0_name");
	InputMesh.SetPointSet("1", { 0,1 });

	SavePolygonMeshAsJsonDataFile(InputMesh, FilePathAndName + "_att.json");
	LoadPolygonMeshFromJsonDataFile(SquareMesh, FilePathAndName + "_att.json");
	SavePolygonMeshAsJsonDataFile(SquareMesh, FilePathAndName + "_att_re.json");
}