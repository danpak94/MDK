﻿#pragma once

#include <itkImportImageFilter.h>
#include <itkImageDuplicator.h>

namespace mdk
{

//------------- forward declare-------------------//
template<typename ElementType>
class DenseMatrix;

template<typename ElementType>
class SparseMatrix;

template<typename PixelType>
class DenseImage2D;

template<typename PixelType>
class DenseImage3D;

template<typename ScalarType>
class TriangleMesh;

template<typename ScalarType>
class PolygonMesh;
//-------------------------------------------------//


//-------------------------------------- convert mdk image to itk image--------------------------------------------------------------------//
//----------3D----------------
//copy or share data
template<typename PixelType>
itk::SmartPointer<itk::ImportImageFilter<PixelType, 3>> ConvertMDK3DScalarImageByITKImportImageFilter(const DenseImage3D<PixelType>& MDKImage, bool SharePixelData);

//copy data
template<typename PixelType>
itk::SmartPointer<itk::Image<PixelType, 3>> ConvertMDK3DScalarImageToITK3DScalarImage(const DenseImage3D<PixelType>& MDKImage);

//----------2D to 3D ----------------
//copy or share data
template<typename PixelType>
itk::SmartPointer<itk::ImportImageFilter<PixelType, 3>> ConvertMDK2DScalarImageByITKImportImageFilter(const DenseImage2D<PixelType>& MDKImage, bool SharePixelData);

//copy data
template<typename PixelType>
itk::SmartPointer<itk::Image<PixelType, 3>> ConvertMDK2DScalarImageToITK2DScalarImage(const DenseImage2D<PixelType>& MDKImage);

//-------------------------------------- convert itk image to mdk image (copy data) ------------------------------------------------------------//
//----------3D----------------
template<typename PixelType>
bool ConvertITK3DScalarImageToMDK3DScalarImage(itk::Image<PixelType, 3>* ITKImage, DenseImage3D<PixelType>& MDKImage, bool SharePixelData);

template<typename PixelType>
bool ConvertITK3DScalarImageToMDK3DScalarImage(const itk::Image<PixelType, 3>* ITKImage, DenseImage3D<PixelType>& MDKImage);

//----------3D to 2D ----------------
// ITKImage should be a Slice
template<typename PixelType>
bool ConvertITK3DScalarImageSliceToMDK2DScalarImage(itk::Image<PixelType, 3>* ITKImage, DenseImage2D<PixelType>& MDKImage, bool SharePixelData);

template<typename PixelType>
bool ConvertITK3DScalarImageSliceToMDK2DScalarImage(const itk::Image<PixelType, 3>* ITKImage, DenseImage2D<PixelType>& MDKImage);

//--------------------------------------- convert mdk PolygonMesh to itk Mesh -----------------------------------------------//
//template<typename ScalarType>
//itk::SmartPointer<itk::Mesh> ConvertMDKPolygonMeshToITKMesh(const TriangleMesh<ScalarType>& InputMesh);

//--------------------------------------- convert itk Mesh to mdk PolygonMesh ------------------------------------------------//
//template<typename ScalarType>
//bool ConvertITKMeshToMDKPolygonMesh(const itk::Mesh* ITKMesh, PolygonMesh<ScalarType>& OutputMesh);


}// namespace mdk

#include "mdkITKDataStructureConversion.hpp"
