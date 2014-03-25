#ifndef __mdkImageFilter_h
#define __mdkImageFilter_h

#include <vector>
#include <functional>
#include <thread>

#include "mdkDebugConfig.h"
#include "mdkDenseMatrix.h"
#include "mdkImage.h"


namespace mdk
{

template<typename VoxelType_Input, typename VoxelType_Output>
class ImageFilter : public Object
{
protected:
	//-------------- input ----------------------

	// input_0: 
    const Image<VoxelType_Input>*  m_InputImage;

	// input_1:
    const ImageBoxRegionOf3DIndex*  m_InputRegion;  // size of m_InputRegion = size of m_OutputImage or m_OutputArray
	                                 
	// input_2:
    const DenseMatrix<int64>*  m_InputVoxel3DIndexList;  // compute values at these center positions
	                
	// input_3:
	const DenseMatrix<float>*  m_Input3DPositionList;  // compute values at these center positions (float precision is enough)

    // input_4:
	std::function<void(double, double, double, const Image<VoxelType_Input>&, VoxelType_Output&)> m_InputFilterFunction_At3DPosition;

	// input_5:
	std::function<void(int64, int64, int64, const Image<VoxelType_Input>&, VoxelType_Output&)> m_InputFilterFunction_At3DIndex;

    // input_6:
    int64 m_MaxThreadNumber;

    // input_7:
	bool m_IsBoundCheckEnabled;

	//--------------------- output ---------------------

	// input_output_0:
	Image<VoxelType_Output>* m_OutputImage;

	// input_output_1:
    DenseMatrix<VoxelType_Output>* m_OutputArray;

    //------------ internal variable --------------

    Image<VoxelType_Output> m_OutputImage_SharedCopy; // keep tracking m_OutputImage

    DenseMatrix<VoxelType_Output> m_OutputArray_SharedCopy; // keep tracking m_OutputArray

    bool m_Flag_OutputImage;

    bool m_Flag_OutputArray;

    bool m_Flag_OutputToOtherPlace;

    VoxelType_Input   m_ZeroVoxelOfInputImage;

    VoxelType_Output  m_ZeroVoxelOfOutputImage;

    bool m_IsInputFilterFunctionAt3DPositionObtained;

    bool m_IsInputFilterFunctionAt3DIndexObtained;

    int64 m_TotalOutputVoxelNumber;

    int64 m_MinVoxelNumberPerThread;

public:		
	ImageFilter();
	~ImageFilter();
  
    void Clear();

    bool SetInputImage(const Image<VoxelType_Input>* InputImage);

    bool SetInputRegion(const DenseMatrix<int64>* InputRegion);

    bool SetInputVoxel3DIndexList(const DenseMatrix<int64>* InputVoxel3DIndexList);

    bool SetInput3DPositionList(const DenseMatrix<float>* Input3DPositionList);

	void SetInputFilterFunctionAt3DIndex(std::function<void(int64, int64, int64, const Image<VoxelType_Input>&, VoxelType_Output&)> Input);

	void SetInputFilterFunctionAt3DPosition(std::function<void(double, double, double, const Image<VoxelType_Input>&, VoxelType_Output&)> Input);

	bool SetOutputImage(Image<VoxelType_Output>* OutputImage);

    bool SetOutputArray(DenseMatrix<VoxelType_Output>* OutputArray);

    void SetMaxThreadNumber(int64 MaxNumber);

	void EnableBoundCheck();

    void DisableBoundCheck();

    inline virtual void FilterFunctionAt3DIndex(int64 x_Index, int64 y_Index, int64 z_Index, VoxelType_Output& OutputVoxel);

    inline virtual void FilterFunctionAt3DPosition(double x, double y, double z, VoxelType_Output& OutputVoxel);

    virtual bool Update();
	
    Image<VoxelType_Output>* GetOutputImage();

    DenseMatrix<VoxelType_Output>* GetOutputArray();

    //----------------------------------------------------------------

    static bool Apply(Image<VoxelType_Output>* OutputImage, 
                      const Image<VoxelType_Input>* InputImage,                                     
                      const std::string& FilterFunctionType, // "At3DIndex" or "At3DPosition"
                      std::function<void(double, double, double, VoxelType_Output&)> FilterFunction,
                      int64 MaxThreadNumber = 1);

protected:
    bool CheckInput();

    virtual bool Preprocess();

    virtual bool Postprocess();

    //output: IndexList_start and IndexList_end
    void DivideData(int64 Index_min, int64 Index_max, int64 MinDataNumberPerThread,
                    std::vector<int64>& IndexList_start, std::vector<int64>& IndexList_end);

    inline virtual void OutputFunction(int64 OutputVoxelIndex, const VoxelType_Output& OutputVoxel);

    void Update_in_a_Thread(int64 OutputVoxelIndex_start, int64 OutputVoxelIndex_end);

private:
	ImageFilter(const ImageFilter&)    = delete;
	void operator=(const ImageFilter&) = delete;
};

}//end namespace mdk

#include "mdkImageFilter.hpp"

#endif