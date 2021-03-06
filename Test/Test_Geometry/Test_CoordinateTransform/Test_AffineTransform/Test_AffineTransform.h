#ifndef Test_ThinPlateSplineTransform_h
#define Test_ThinPlateSplineTransform_h

//================================================
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <initializer_list>
#include <functional>

#include "mdkAffineTransform2D.h"
#include "mdkAffineTransform3D.h"
#include "mdkDenseMatrix_FileIO.h"

void Test_2D()
{
	using namespace mdk;

	AffineTransform2D<double> Transform;

	String File_Source = "C:/Research/MDK/MDK_Build/Test/Test_Geometry/Test_CoordinateTransform/Test_AffineTransform/TestData/Source2D.json";
	String File_Target = "C:/Research/MDK/MDK_Build/Test/Test_Geometry/Test_CoordinateTransform/Test_AffineTransform/TestData/Target2D.json";

	DenseMatrix<double> SourceControlPointSet, TargetControlPointSet;
	LoadDenseMatrixFromJsonDataFile(SourceControlPointSet, File_Source);
	LoadDenseMatrixFromJsonDataFile(TargetControlPointSet, File_Target);

	Transform.SetSourceLandmarkPointSet(&SourceControlPointSet);
	Transform.SetTargetLandmarkPointSet(&TargetControlPointSet);
	Transform.EstimateParameter();

	auto TransformMatrix = Transform.GetTransformationMatrix();

	DenseMatrix<double> TargetPointSet;
	TargetPointSet.Resize(SourceControlPointSet.GetSize());
	for (int_max k = 0; k < SourceControlPointSet.GetColCount(); ++k)
	{
		DenseVector<double, 2> Pos;
		SourceControlPointSet.GetCol(k, Pos);
		auto Pos_new = Transform.TransformPoint(Pos);
		TargetPointSet.SetCol(k, Pos_new);
	}

	DisplayMatrix("TransformMatrix", TransformMatrix, 4);

	DisplayMatrix("TargetPointSet", TargetPointSet, 4);
}


void Test_3D()
{
	using namespace mdk;

	AffineTransform3D<double> Transform;

	String File_Source = "C:/Research/MDK/MDK_Build/Test/Test_Geometry/Test_CoordinateTransform/Test_AffineTransform/TestData/Source.json";
	String File_Target = "C:/Research/MDK/MDK_Build/Test/Test_Geometry/Test_CoordinateTransform/Test_AffineTransform/TestData/Target.json";

	DenseMatrix<double> SourceControlPointSet, TargetControlPointSet;
	LoadDenseMatrixFromJsonDataFile(SourceControlPointSet, File_Source);
	LoadDenseMatrixFromJsonDataFile(TargetControlPointSet, File_Target);

	Transform.SetSourceLandmarkPointSet(&SourceControlPointSet);
	Transform.SetTargetLandmarkPointSet(&TargetControlPointSet);
	Transform.EstimateParameter();

	auto TransformMatrix = Transform.GetTransformationMatrix();

	DenseMatrix<double> TargetPointSet;
	TargetPointSet.Resize(SourceControlPointSet.GetSize());
	for (int_max k = 0; k < SourceControlPointSet.GetColCount(); ++k)
	{
		DenseVector<double, 3> Pos;
		SourceControlPointSet.GetCol(k, Pos);
		auto Pos_new = Transform.TransformPoint(Pos);
		TargetPointSet.SetCol(k, Pos_new);
	}

	DisplayMatrix("TransformMatrix", TransformMatrix, 4);

	DisplayMatrix("TargetPointSet", TargetPointSet, 4);
}


#endif