#ifndef __mdkParallelForLoop_hpp
#define __mdkParallelForLoop_hpp

namespace mdk
{

template<typename FunctionType>
inline
void ParallelForLoop(FunctionType SingleFunction, const std::vector<int_max>& LoopIndexList, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread)
{
    ParallelBlock([&](std::vector<int_max> SubLoopIndexList, int_max ThreadIndex){ParallelForLoop_Block_in_a_thread(SingleFunction, SubLoopIndexList, ThreadIndex); },
                  DataIndexList, MaxNumberOfThreads, MinNumberOfDataPerThread);
}


template<typename FunctionType>
inline
void ParallelForLoop_Block_in_a_thread(FunctionType SingleFunction, std::vector<int_max> SubLoopIndexList, int_max ThreadIndex)
{
    for (int_max i = 0; i < int_max(SubLoopIndexList.size()); ++i)
    {
        SingleFunction(SubLoopIndexList[i], ThreadIndex);
    }
}


template<typename FunctionType>
inline
void ParallelForLoop(FunctionType SingleFunction, int_max LoopIndex_start, int_max LoopIndex_end, 
                     int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread)
{
    ParallelBlock([&](int_max Index_start, int_max Index_end, int_max ThreadIndex){ParallelForLoop_Block_in_a_thread(SingleFunction, Index_start, Index_end, ThreadIndex); },
                  LoopIndex_start, LoopIndex_end, MaxNumberOfThreads, MinNumberOfDataPerThread);
}


template<typename FunctionType>
inline
void ParallelForLoop_Block_in_a_thread(FunctionType SingleFunction, int_max LoopIndex_start, int_max LoopIndex_end, int_max ThreadIndex)
{
    for (int_max i = LoopIndex_start; i <= LoopIndex_end; ++i)
    {
        SingleFunction(i, ThreadIndex);
    }
}


template<typename FunctionType>
inline
void ParallelBlock(FunctionType BlockFunction, const std::vector<int_max>& DataIndexList, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread)
{
    // divide the LoopIndexList into groups

    auto TotalDataNumber = int_max(DataIndexList.size());

    if (TotalDataNumber > 1 && MaxNumberOfThreads > 1)
    {
        std::vector<int_max> IndexList_start;
        std::vector<int_max> IndexList_end;

        DivideData_For_ParallelBlock(IndexList_start, IndexList_end, 0, TotalDataNumber - 1, MaxNumberOfThreads, MinNumberOfDataPerThread);

        auto ThreadNumber = int_max(IndexList_start.size());

        // create and start the threads
        std::vector<std::thread> ThreadList(ThreadNumber);

        for (int_max i = 0; i < ThreadNumber; ++i)
        {
            auto subNumber = IndexList_end[i] - IndexList_start[i] + 1;

            std::vector<int_max> SubDataIndexList(subNumber);

            for (int_max k = 0; k < subNumber; ++k)
            {
                SubDataIndexList[k] = DataIndexList[IndexList_start[i] + k];
            }

            int_max ThreadIndex = i;

            ThreadList[i] = std::thread([&]{BlockFunction(std::move(SubDataIndexList), ThreadIndex); });

            // this will crash, i can be equal to ThreadNumber
            //ThreadList[i] = std::thread([&]{BlockFunction(IndexList_start[i], IndexList_end[i], ThreadIndex); });
        }

        //wait for all the threads
        for (int_max i = 0; i < ThreadNumber; ++i)
        {
            ThreadList[i].join();
        }
    }
    else//single-thread
    {
        BlockFunction(DataIndexList, 0);
    }
}


template<typename FunctionType>
inline
void ParallelBlock(FunctionType BlockFunction, int_max DataIndex_start, int_max DataIndex_end, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread)
{
    // divide the LoopIndexList into groups

    auto TotalDataNumber = DataIndex_end - DataIndex_start +1;

    if (TotalDataNumber > 1 && MaxNumberOfThreads > 1)
    {
        std::vector<int_max> IndexList_start;
        std::vector<int_max> IndexList_end;

        DivideData_For_ParallelBlock(IndexList_start, IndexList_end, 0, TotalDataNumber - 1, MaxNumberOfThreads, MinNumberOfDataPerThread);

        auto ThreadNumber = int_max(IndexList_start.size());

        // create and start the threads
        std::vector<std::thread> ThreadList(ThreadNumber);

        for (int_max i = 0; i < ThreadNumber; ++i)
        {
            auto SubDataIndex_start = IndexList_start[i];
            auto SubDataIndex_end = IndexList_end[i];
            auto ThreadIndex = i;

            ThreadList[i] = std::thread([&]{BlockFunction(SubDataIndex_start, SubDataIndex_end, ThreadIndex); });

            // this will crash, i can be equal to ThreadNumber
            //ThreadList[i] = std::thread([&]{BlockFunction(IndexList_start[i], IndexList_end[i], ThreadIndex); });
        }

        //wait for all the threads
        for (int_max i = 0; i < ThreadNumber; ++i)
        {
            ThreadList[i].join();
        }
    }
    else//single-thread
    {
        BlockFunction(DataIndex_start, DataIndex_end, 0);
    }
}


inline int_max Compute_NecessaryNumberOfThreads_For_ParallelBlock(int_max TotalDataNumber, int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread)
{
    if (MaxNumberOfThreads <= 1)
    {       
        return 1;
    }

    int_max ThreadNumber = 1;

    int_max DataNumberPerThread = 0;

    for (int_max i = MaxNumberOfThreads; i >= 1; --i)
    {
        DataNumberPerThread = TotalDataNumber / i;

        if (DataNumberPerThread >= MinNumberOfDataPerThread)
        {
            ThreadNumber = i;
            break;
        }
    }

    return ThreadNumber;
}


inline void DivideData_For_ParallelBlock(std::vector<int_max>& DataIndexList_start, std::vector<int_max>& DataIndexList_end,
                                         int_max DataIndex_min, int_max DataIndex_max, 
                                         int_max MaxNumberOfThreads, int_max MinNumberOfDataPerThread)
{
    if (MaxNumberOfThreads <= 1)
    {
        DataIndexList_start.push_back(DataIndex_min);
        DataIndexList_end.push_back(DataIndex_max);
        return;
    }

    auto TotalDataNumber = DataIndex_max - DataIndex_min + 1;

    auto ThreadNumber = Compute_NecessaryNumberOfThreads_For_ParallelBlock(TotalDataNumber, MaxNumberOfThreads, MinNumberOfDataPerThread);

    if (ThreadNumber == 1)
    {//one thread is enough

        DataIndexList_start.push_back(DataIndex_min);
        DataIndexList_end.push_back(DataIndex_max);
        return;
    }

    DataIndexList_start.resize(ThreadNumber);
    DataIndexList_end.resize(ThreadNumber);

    int_max DataNumberPerThread = TotalDataNumber / ThreadNumber;

    int_max tempIndex = DataIndex_min;

    for (int_max i = 0; i < ThreadNumber; ++i)
    {
        DataIndexList_start[i] = tempIndex;
        DataIndexList_end[i]   = tempIndex + DataNumberPerThread - 1;

        tempIndex += DataNumberPerThread;
    }

    DataIndexList_end[ThreadNumber - 1] = DataIndex_max;
}


}//namespace mdk

#endif