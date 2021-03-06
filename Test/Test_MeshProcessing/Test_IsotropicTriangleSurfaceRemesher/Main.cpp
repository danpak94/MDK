#include "mdkIsotropicTriangleSurfaceRemesher.h"

using namespace mdk;

void test_Wall_tri()
{
	String DataPath = "C:/Research/MDK/MDK_Build/Test/Test_MeshProcessing/Test_IsotropicTriangleSurfaceRemesher/TestData/";
	TriangleMesh<double> InputMesh;
	LoadPolygonMeshFromVTKFile(InputMesh, DataPath + "Wall_tri.vtk");

	IsotropicTriangleSurfaceRemesher<double> Remesher;
	Remesher.Input.SourceMesh = &InputMesh;
	Remesher.Input.TargetEdgeLength = 1;
	Remesher.Input.Flag_ProcessBounary = true;
	Remesher.Update();
	std::cout << "IsoRemesh completed" << '\n';	
	SavePolygonMeshAsVTKFile(Remesher.Output.TargetMesh, DataPath + "Wall_iso_remsh_0.5_pb.vtk");
}

void main()
{
	test_Wall_tri();
}
