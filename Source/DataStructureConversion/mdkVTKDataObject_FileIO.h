#ifndef __mdkVTKDataObject_FileIO_h
#define __mdkVTKDataObject_FileIO_h

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkImageData.h>

#include "mdkType.h"
#include "mdkString.h"
#include "mdkFileIO.h"

#include "mdkVTKDataStructureConversion.h"

namespace mdk
{

bool LoadVTKPolygonMeshFromJsonDataFile(vtkPolyData& OutputVTKMesh, const String& FilePathAndName);
bool LoadVTKPolygonMeshFromJsonDataFile(vtkPolyData* OutputVTKMesh, const String& FilePathAndName);

bool LoadVTK3DScalarImageFromJsonDataFile(vtkImageData& OutputVTKImage, VTKDataTypeEnum OutputPixelType, const String& FilePathAndName);
bool LoadVTK3DScalarImageFromJsonDataFile(vtkImageData* OutputVTKImage, VTKDataTypeEnum OutputPixelType, const String& FilePathAndName);
}// namespace mdk

#endif