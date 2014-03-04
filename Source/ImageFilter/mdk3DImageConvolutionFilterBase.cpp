#include <thread>
#include <algorithm>

#include "mdk3DImageConvolutionFilterBase.h"
#include "mdkDebugConfig.h"

namespace mdk
{

mdk3DImageConvolutionFilterBase::mdk3DImageConvolutionFilterBase()
{
}


mdk3DImageConvolutionFilterBase::~mdk3DImageConvolutionFilterBase()
{
	// do nothing
}


bool mdk3DImageConvolutionFilterBase::SetMaskOf3DIndex(const std::vector<mdkMatrix<double>>& MaskList)
{
	auto Length = MaskList.size();

	if (Length == 0)
	{
		return false;
	}

	for (uint64 i = 0; i < Length; ++i)
	{
		if (MaskList[i].IsEmpty() == true)
		{
			return false;
		}
	}

    m_MaskList_3DIndex = MaskList;

	return true;
}


bool mdk3DImageConvolutionFilterBase::SetMaskOf3DPosition(const std::vector<mdkMatrix<double>>& MaskList)
{
    auto Length = MaskList.size();

    if (Length == 0)
    {
        return false;
    }

    for (uint64 i = 0; i < Length; ++i)
    {
        if (MaskList[i].IsEmpty() == true)
        {
            return false;
        }
    }

    m_MaskList_3DPosition = MaskList;

    return true;
}


bool mdk3DImageConvolutionFilterBase::SetMaskOf3DIndex(const mdkMatrix<double>& Mask)
{
    if (Mask.IsEmpty() == true)
    {
        return false;
    }

    m_MaskList_3DIndex.resize(1);

    m_MaskList_3DIndex[0] = Mask;

    return true;
}


bool mdk3DImageConvolutionFilterBase::SetMaskOf3DPosition(const mdkMatrix<double>& Mask)
{
	if (Mask.IsEmpty() == true)
	{
		return false;
	}

    m_MaskList_3DPosition.resize(1);

    m_MaskList_3DPosition[0] = Mask;

	return true;
}


bool mdk3DImageConvolutionFilterBase::LoadMask(const std::string& FilePathAndName)
{

    return true;
}


bool mdk3DImageConvolutionFilterBase::SaveMask(const std::string& FilePathAndName)
{
    return true;
}


void mdk3DImageConvolutionFilterBase::ComputeRegionOfNOBoundCheck_3DIndex()
{
    m_NOBoundCheckRegionList_3DIndex.resize(m_MaskList_3DIndex.size());

    if (m_MaskList_3DIndex.size() == 0)
    {
        return;
    }

    uint64 SafeDistance = 2;

    for (uint64 i = 0; i < m_MaskList_3DIndex.size(); ++i)
    {
        m_NOBoundCheckRegionList_3DIndex[i].IsEmpty = true;

        uint64 MaxDistance_x[2] = { 0, 0 };

        uint64 MaxDistance_y[2] = { 0, 0 };

        uint64 MaxDistance_z[2] = { 0, 0 };

        for (uint64 j = 0; j < m_MaskList_3DIndex[i].GetColNumber(); ++j)
        {
            auto temp = m_MaskList_3DIndex[i](0, j);

            if (temp < 0.0)
            {
                MaxDistance_x[0] = std::max(MaxDistance_x[0], uint64(-temp));
            }
            else
            {
                MaxDistance_x[1] = std::max(MaxDistance_x[1], uint64(temp));
            }

            temp = m_MaskList_3DIndex[i](1, j);

            if (temp < 0.0)
            {
                MaxDistance_y[0] = std::max(MaxDistance_y[0], uint64(-temp));
            }
            else
            {
                MaxDistance_y[1] = std::max(MaxDistance_y[1], uint64(temp));
            }

            temp = m_MaskList_3DIndex[i](2, j);

            if (temp < 0.0)
            {
                MaxDistance_z[0] = std::max(MaxDistance_z[0], uint64(-temp));
            }
            else
            {
                MaxDistance_z[1] = std::max(MaxDistance_z[1], uint64(temp));
            }
        }

        if (MaxDistance_x[0] + MaxDistance_x[1] + 1 + 2*SafeDistance < m_InputImageDimension[0]
            && MaxDistance_y[0] + MaxDistance_y[1] + 1 + 2*SafeDistance < m_InputImageDimension[1]
            && MaxDistance_z[0] + MaxDistance_z[1] + 1 + 2*SafeDistance < m_InputImageDimension[2])
        {
            m_NOBoundCheckRegionList_3DIndex[i].IsEmpty = false;

            m_NOBoundCheckRegionList_3DIndex[i].x0_Index = MaxDistance_x[0] + SafeDistance;

            m_NOBoundCheckRegionList_3DIndex[i].x1_Index = m_InputImageDimension[0] - 1 - MaxDistance_x[1] - SafeDistance;

            m_NOBoundCheckRegionList_3DIndex[i].y0_Index = MaxDistance_y[0] + SafeDistance;

            m_NOBoundCheckRegionList_3DIndex[i].y1_Index = m_InputImageDimension[1] - 1 - MaxDistance_y[1] - SafeDistance;

            m_NOBoundCheckRegionList_3DIndex[i].z0_Index = MaxDistance_z[0] + SafeDistance;

            m_NOBoundCheckRegionList_3DIndex[i].z1_Index = m_InputImageDimension[2] - 1 - MaxDistance_z[1] - SafeDistance;
        }
    }
}


void mdk3DImageConvolutionFilterBase::ComputeRegionOfNOBoundCheck_3DPosition()
{
    m_NOBoundCheckRegionList_3DPosition.resize(m_MaskList_3DPosition.size());

    if (m_MaskList_3DPosition.size() == 0)
    {
        return;
    }
    
    auto SafeDistance_x = 2 * m_InputVoxelPhysicalSize[0];

    auto SafeDistance_y = 2 * m_InputVoxelPhysicalSize[1];

    auto SafeDistance_z = 2 * m_InputVoxelPhysicalSize[2];

    for (uint64 i = 0; i < m_MaskList_3DPosition.size(); ++i)
    {
        m_NOBoundCheckRegionList_3DPosition[i].IsEmpty = true;

        double MaxDistance_x[2] = { 0, 0 };

        double MaxDistance_y[2] = { 0, 0 };

        double MaxDistance_z[2] = { 0, 0 };

        for (uint64 j = 0; j < m_MaskList_3DPosition[i].GetColNumber(); ++j)
        {
            auto temp = m_MaskList_3DPosition[i](0, j);

            if (temp < 0.0)
            {
                MaxDistance_x[0] = std::max(MaxDistance_x[0], -temp);
            }
            else
            {
                MaxDistance_x[1] = std::max(MaxDistance_x[1], temp);
            }

            temp = m_MaskList_3DIndex[i](1, j);

            if (temp < 0.0)
            {
                MaxDistance_y[0] = std::max(MaxDistance_y[0], -temp);
            }
            else
            {
                MaxDistance_y[1] = std::max(MaxDistance_y[1], temp);
            }

            temp = m_MaskList_3DIndex[i](2, j);

            if (temp < 0.0)
            {
                MaxDistance_z[0] = std::max(MaxDistance_z[0], -temp);
            }
            else
            {
                MaxDistance_z[1] = std::max(MaxDistance_z[1], temp);
            }
        }

        if (MaxDistance_x[0] + MaxDistance_x[1] + 2 * SafeDistance_x < m_InputImagePhysicalSize[0]
            && MaxDistance_y[0] + MaxDistance_y[1] + 2 * SafeDistance_y < m_InputImagePhysicalSize[1]
            && MaxDistance_z[0] + MaxDistance_z[1] + 2 * SafeDistance_z < m_InputImagePhysicalSize[2])
        {
            m_NOBoundCheckRegionList_3DPosition[i].IsEmpty = false;

            m_NOBoundCheckRegionList_3DPosition[i].x0 = m_InputImagePhysicalOrigin[0] + MaxDistance_x[0] + SafeDistance_x;

            m_NOBoundCheckRegionList_3DPosition[i].x0 = m_InputImagePhysicalOrigin[0] 
                                                        + m_InputImagePhysicalSize[0] - MaxDistance_x[1] - SafeDistance_x;
          
            m_NOBoundCheckRegionList_3DPosition[i].y0 = m_InputImagePhysicalOrigin[1] + MaxDistance_y[0] + SafeDistance_y;

            m_NOBoundCheckRegionList_3DPosition[i].y1 = m_InputImagePhysicalOrigin[1] 
                                                        + m_InputImageDimension[1] - MaxDistance_y[1] - SafeDistance_y;

            m_NOBoundCheckRegionList_3DPosition[i].z0 = m_InputImagePhysicalOrigin[2] + MaxDistance_z[0] + SafeDistance_z;

            m_NOBoundCheckRegionList_3DPosition[i].z1 = m_InputImagePhysicalOrigin[2]
                                                        + m_InputImageDimension[2] - MaxDistance_z[1] - SafeDistance_z;
        }
    }
}


bool mdk3DImageConvolutionFilterBase::IsMaskOf3DIndexEmpty()
{
    if (m_MaskList_3DIndex.size() > 0)
    {
        return false;
    }

    return true;
}


bool mdk3DImageConvolutionFilterBase::IsMaskOf3DPositionEmpty()
{
    if (m_MaskList_3DPosition.size() > 0)
    {
        return false;
    }

    return true;
}


}//end namespace mdk