﻿#ifndef mdk_PolygonMesh_FileIO_hpp
#define mdk_PolygonMesh_FileIO_hpp

namespace mdk
{

template<typename MeshAttributeType>
bool SavePolygonMeshAsJsonDataFile(const PolygonMesh<MeshAttributeType>& InputMesh, const String& FilePathAndName)
{
	typedef MeshAttributeType::ScalarType  ScalarType;
	//---------------------------------------------
	if (GetByteCountOfScalar(ScalarType(0)) <= 0)
	{
		MDK_Error("Unknown ScalarType @ SavePolygonMeshAsJsonDataFile_Header(...)")
		return false;
	}
	//-------------------------------------------------------------------------------------
	JsonObject JObject;

	JObject["ObjectType"] = "PolygonMesh";
	JObject["ScalarType"] = GetScalarTypeName(ScalarType(0));
	JObject["IndexType"] = GetScalarTypeName(int_max(0));
	JObject["PointCount"] = InputMesh.GetPointCount();
	JObject["FaceCount"] = InputMesh.GetFaceCount();
	
	//--------------------------------------------------------------------
	ObjectArray<DenseVector<int_max>> FaceData;
	DenseMatrix<ScalarType> PointData;
	InputMesh.GetPointPositionMatrixAndFaceTable(PointData, FaceData);

	String FilePath = ExtractFilePath(FilePathAndName);
	String FileName = ExtractFileName(FilePathAndName);
	String FaceDataFileName = FileName + ".Face.json";
	String PointDataFileName = FileName + ".Point.data";
	JObject["FaceData"] = FaceDataFileName;
	JObject["PointData"] = PointDataFileName;
	//--------------------------------------------------------------------
	//write cell to JsonArray
	JsonArray JArray_FaceData;
	JArray_FaceData.Resize(FaceData.GetLength());
	if (FaceData.IsEmpty() == false)
	{
		for (int_max i = 0; i < FaceData.GetLength(); ++i)
		{
			const DenseVector<int_max>& Face_i = FaceData[i];

			JArray_FaceData[i] = Face_i;
		}
	}
	//----------------------------------------------------------------------------------------------
	bool IsOK = true;
	if (JsonFile::Save(JObject, FilePathAndName) == false)
	{
		IsOK = false;
	}
	if (SaveScalarArrayAsDataFile(PointData.GetElementPointer(), PointData.GetElementCount(), FilePath + PointDataFileName) == false)
	{
		IsOK = false;
	}	
	if (JsonFile::Save(JArray_FaceData, FilePath + FaceDataFileName) == false)
	{
		IsOK = false;
	}
	return IsOK;
}


template<typename MeshAttributeType>
bool LoadPolygonMeshFromJsonDataFile(PolygonMesh<MeshAttributeType>& OutputMesh, const String& FilePathAndName)
{
	typedef MeshAttributeType::ScalarType  ScalarType;
	//---------------------------------------------
	JsonObject JObject;
	if (JsonFile::Load(JObject, FilePathAndName) == false)
	{
		MDK_Error("Json file is invalid @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}

	if (JObject.IsEmpty() == true)
	{
		MDK_Warning("Json file is empty @ LoadPolygonMeshFromJsonDataFile(...)")
		return true;
	}
	//---------------------------------------------
	auto it = JObject.find("ObjectType");
	if (it != JObject.end())
	{
		auto ObjectType = it->second.GetString();
		if (ObjectType != "PolygonMesh" && ObjectType != "TriangleMesh")
		{
			MDK_Error("ObjectType is not PolygonMesh or TriangleMesh @ LoadPolygonMeshFromJsonDataFile(...)")
			return false;
		}
	}
	else
	{
		MDK_Error("Couldn't get ObjectType @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//------------------------------------------
	String ScalarTypeInDataFile;
	it = JObject.find("ScalarType");
	if (it != JObject.end())
	{
		ScalarTypeInDataFile = it->second.GetString();
	}
	else
	{
		MDK_Error("Couldn't get ScalarType @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//-------------------------------------------
	String IndexTypeName;
	it = JObject.find("IndexType");
	if (it != JObject.end())
	{
		IndexTypeName = it->second.GetString();
	}
	else
	{
		MDK_Error("Couldn't get IndexType @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//----------------------------------------------
	int_max PointCount = 0;
	it = JObject.find("PointCount");
	if (it != JObject.end())
	{
		PointCount = it->second.ToScalar<int_max>();
	}
	else
	{
		MDK_Error("Couldn't get PointCount @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//----------------------------------------------
	int_max FaceCount = 0;
	it = JObject.find("FaceCount");
	if (it != JObject.end())
	{
		FaceCount = it->second.ToScalar<int_max>();
	}
	else
	{
		MDK_Error("Couldn't get FaceCount @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//----------------------------------------------	
	String PointDataFileName;
	it = JObject.find("PointData");
	if (it != JObject.end())
	{
		PointDataFileName = it->second.GetString();
	}
	else
	{
		MDK_Error("Couldn't get PointData @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//----------------------------------------------	
	String FaceDataFileName;
	it = JObject.find("FaceData");
	if (it != JObject.end())
	{
		FaceDataFileName = it->second.GetString();
	}
	else
	{
		MDK_Error("Couldn't get FaceData @ LoadPolygonMeshFromJsonDataFile(...)")
		return false;
	}
	//----------------------------------------------
	String FilePath = ExtractFilePath(FilePathAndName);
	
	if (PointCount > 0)
	{
		bool IsOK = true;
		DenseMatrix<ScalarType> PointData(3, PointCount);
		if (LoadScalarArrayFromDataFile(PointData.GetElementPointer(), PointData.GetElementCount(), FilePath + PointDataFileName, ScalarTypeInDataFile) == false)
		{
			IsOK = false;
		}		

		//Get FaceData from JsonArray
		ObjectArray<DenseVector<int_max>> FaceData;
		FaceData.Resize(FaceCount);
		if (FaceCount > 0)
		{
			JsonArray JArray_FaceData;
			if (JsonFile::Load(JArray_FaceData, FilePath + FaceDataFileName) == false)
			{
				IsOK = false;
			}

			if (JArray_FaceData.IsEmpty() == false)
			{
				for (int_max i = 0; i < JArray_FaceData.GetLength(); ++i)
				{
					FaceData[i] = JArray_FaceData[i].ToScalarArray<int_max>();
				}
			}
		}

		OutputMesh.Construct(std::move(PointData), FaceData);
		return IsOK;
	}
	else// empty mesh
	{
		OutputMesh.Clear();
		return true;
	}
}


template<typename MeshAttributeType>
bool SavePolygonMeshAsVTKFile(const PolygonMesh<MeshAttributeType>& InputMesh, const String& FilePathAndName)
{
	auto VTKMesh = ConvertMDKPolygonMeshToVTKPolyData(InputMesh);

	auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	writer->SetFileName(FilePathAndName.StdString().c_str());
	writer->SetInputData(VTKMesh);
	writer->SetFileTypeToASCII();
	
	try
	{
		writer->Write();
	}
	catch (...)
	{
		MDK_Error(" Can not write data @ SavePolygonMeshAsVTKFile(...) ")
		return false;
	}

	return true;
}


template<typename MeshAttributeType>
bool LoadPolygonMeshFromVTKFile(PolygonMesh<MeshAttributeType>& OutputMesh, const String& FilePathAndName)
{
	typedef MeshAttributeType::ScalarType ScalarType;

	auto Reader = vtkSmartPointer<vtkPolyDataReader>::New();
	Reader->SetFileName(FilePathAndName.StdString().c_str());

	try
	{
		Reader->Update();
	}
	catch (...)
	{
		MDK_Error(" Can not read data @ LoadPolygonMeshFromVTKFile(...) ")
		return false;
	}

	auto VTKMesh = Reader->GetOutput();
	return ConvertVTKPolyDataToMDKPolygonMesh(VTKMesh, OutputMesh);
}

}//namespace mdk

#endif