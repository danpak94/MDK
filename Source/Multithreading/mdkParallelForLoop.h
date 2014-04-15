#ifndef __mdkParallelForLoop_h
#define __mdkParallelForLoop_h

#include <vector>
#include <thread>


#include "mdkType.h"


namespace mdk
{

template<typename FunctionType>
inline
void ParallelForLoop(FunctionType SingleFunction, const std::vector<int_max>& LoopIndexList, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread = 1);

template<typename FunctionType>
inline
void ParallelForLoop_SubBlock_in_a_thread(FunctionType SingleFunction, std::vector<int_max> SubLoopIndexList);

template<typename FunctionType>
inline
void ParallelForLoop(FunctionType SingleFunction, int_max LoopIndex_start, int_max LoopIndex_end, 
                     int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread = 1);

template<typename FunctionType>
inline
void ParallelForLoop_SubBlock_in_a_thread(FunctionType SingleFunction, int_max SubLoopIndex_start, int_max SubLoopIndex_end);

template<typename FunctionType>
inline
void ParallelBlock(FunctionType BlockFunction, const std::vector<int_max>& DataIndexList, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread = 1);

template<typename FunctionType>
inline
void ParallelBlock(FunctionType BlockFunction, int_max DataIndex_start, int_max DataIndex_end, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread = 1);

inline void DivideData_For_ParallelComputation(std::vector<int_max>& IndexList_start, std::vector<int_max>& IndexList_end,
                                               int_max Index_min, int_max Index_max, 
                                               int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread = 1);

}//namespace mdk

#include "mdkParallelForLoop.hpp"

#endif