#ifndef __mdk3DImage_h
#define __mdk3DImage_h

#include <vector>
#include <memory>

#include "mdkObject.h"

namespace mdk
{

typedef VoxelType_Scalar_double double

typedef VoxelType_Scalar_float float

typedef VoxelType_Scalar_int32 int32

typedef VoxelType_Array_double std::vector<double>

typedef VoxelType_Array_float std::vector<float>

typedef VoxelType_Array_int32 std::vector<int32>

struct Image3DSize
{
	uint64 Lx;
	uint64 Ly;
	uint64 Lz;
};

//-------------------------------------------------------------------------------------------------------//
// 3D Image Class
// Voxel = an array of scalars
// Lx: number of voxels in x-direction
// Ly: number of voxels in y-direction
// Lz: number of voxels in z-direction
//
// VoxelPhysicalSize (unit: mm), e.g., 0.5mm x 0.5mm x 0.5mm,  same resolution in x, y, z directions 
// same resolution => image filters do not need to be re-sampled with different voxel shape
// reslice the input image using itk/vtk, then feed it into this class.
//
// ok to store image with nonisotropic voxels
// but such image is not good for analsyis
// if the size is 0.5/0.5/6, then it is realy bad and useless for 3D analysis
//
// itk can register images with nonisotropic voxels
// --------------------------------------------------------------------------------------------------------//

template<typename VoxelType>
class mdk3DImage : public mdkObject
{

private:
	
	double m_VoxelPhysicalSize[3];

	uint64 m_ImageSize[3]; // {Lx, Ly, Lz} number of voxels in each direction

	uint64 m_VoxelNumber;  // total number of voxels

	uint64 m_VoxelNumberPerZSlice; // total number of voxels in each z-slice  = m_ImageSize[2]*m_ImageSize[1]

	double m_PhysicalOrigin[3];    // {x0, y0, z0} in world coordinate system (unit: mm)

	std::shared_ptr<std::vector<VoxelType>> m_VoxelData;

	VoxelType m_EmptyVoxel;

public:		
	
	mdk3DImage();

	~mdk3DImage();

	bool Initialize(uint64 Lx, uint64 Ly, uint64 Lz,
             		double PhysicalOrigin_x,    double PhysicalOrigin_y,    double PhysicalOrigin_z,
					double VoxelPhysicalSize_x, double VoxelPhysicalSize_y, double VoxelPhysicalSize_z);

	void Clear();

	bool IsEmpty();

	void Copy(mdk3DImage<VoxelType> targetImage);

	void Copy(VoxelType* VoxelPointer, uint64 Lx, uint64 Ly, uint64 Lz);

	void SetPhysicalOrigin(double PhysicalOrigin_x, double PhysicalOrigin_y, double PhysicalOrigin_z);

	void SetVoxelPhysicalSize(double VoxelPhysicalSize_x, double VoxelPhysicalSize_y, double VoxelPhysicalSize_z);

	bool Fill(VoxelType Voxel);

	VoxelType* GetVoxelRawPointer();

	Image3DSize GetImageSize();

	void GetImageSize(uint64* Lx, uint64* Ly, uint64* Lz);

	void GetVoxelPhysicalSize(uint64* VoxelPhysicalSize_x, uint64* VoxelPhysicalSize_y, uint64* VoxelPhysicalSize_z);

	void GetPhysicalOrigin(uint64* PhysicalOrigin_x, uint64* PhysicalOrigin_y, uint64* PhysicalOrigin_z);

	//--------------------------- Get/Set Voxel ------------------------------//

	inline VoxelType& operator()(uint64 LinearIndex);

	//const VoxelType& operator()(uint64 LinearIndex) const;

	inline VoxelType& Voxel(uint64 LinearIndex);

	//const VoxelType& Voxel(uint64 LinearIndex) const;

	inline VoxelType& operator()(uint64 xIndex, uint64 yIndex, uint64 zIndex);

	//const VoxelType& operator()(uint64 xIndex, uint64 yIndex, uint64 zIndex) const;

	inline VoxelType& Voxel(uint64 xIndex, uint64 yIndex, uint64 zIndex);

	//const VoxelType& Voxel(uint64 xIndex, uint64 yIndex, uint64 zIndex) const;

	//-------------------------- Get SubImage -------------------------------//

	mdk3DImage SubImage(uint64 xIndex_s, uint64 xIndex_e, uint64 yIndex_s, uint64 yIndex_e, uint64 zIndex_s, uint64 zIndex_e);

	//-------------------------- Interpolation -------------------------------//

	inline VoxelType& operator()(double x, double y, double z);

	inline VoxelType& Voxel(double x, double y, double z);

private:
	mdk3DImage(const mdk3DImage&);        // Not implemented.
	void operator=(const mdk3DImage&);    // Not implemented.
};

}//end namespace mdk

#include "mdk3DImage.hpp"

#endif