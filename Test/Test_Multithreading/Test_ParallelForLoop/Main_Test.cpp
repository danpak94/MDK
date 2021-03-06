﻿#include <iostream>
#include <chrono>

#include "mdkDenseMatrix.h"
#include "mdkParallelForLoop.h"


using namespace mdk;

void test_ParallelForLoop()
{
    // time with 1 thread is the same as time with 4 thread
    //auto L = 50000;
    //int_max MaxNumberOfThreads = 1;
    //int_max OuterLoopNumber = 10000;
    //
    // time with 1 thread is greater than time with 4 thread
    auto L = 500000;
    int_max MaxThreadCount = 2;
    int_max OuterLoopNumber = 1000;
    //

    DenseMatrix<double> A(1, L);
    //A.Fill(1);

    DenseMatrix<double> B(1, L);
    //B.Fill(1);

    auto Function = [&](int_max i){ A[i] += B[i]; };
   
    auto t0 = std::chrono::system_clock::now();

    for (int_max k = 0; k < OuterLoopNumber; ++k)
    {
		ParallelForLoop(Function, 0, L - 1, MaxThreadCount);
    }

    auto t1 = std::chrono::system_clock::now();

    std::chrono::duration<double> raw_time = t1 - t0;
    std::cout << "ParallelForLoop  time " << raw_time.count() << '\n';

   // DisplayMatrix("A", A);

    std::system("pause");

}

void test_ParallelForLoop_WithThreadIndex()
{
	// time with 1 thread is the same as time with 4 thread
	//auto L = 50000;
	//int_max MaxNumberOfThreads = 1;
	//int_max OuterLoopNumber = 10000;
	//
	// time with 1 thread is greater than time with 4 thread
	auto L = 500000;
	int_max MaxThreadCount = 2;
	int_max OuterLoopNumber = 1000;
	//

	DenseMatrix<double> A(1, L);
	//A.Fill(1);

	DenseMatrix<double> B(1, L);
	//B.Fill(1);

	auto Function = [&](int_max i, int_max ThreadIndex){ A[i] += B[i] + ThreadIndex; };

	auto t0 = std::chrono::system_clock::now();

	for (int_max k = 0; k < OuterLoopNumber; ++k)
	{
		ParallelForLoop_WithThreadIndex(Function, 0, L - 1, MaxThreadCount);
	}

	auto t1 = std::chrono::system_clock::now();

	std::chrono::duration<double> raw_time = t1 - t0;
	std::cout << "ParallelForLoop  time " << raw_time.count() << '\n';

	// DisplayMatrix("A", A);

	std::system("pause");

}


void main()
{
	test_ParallelForLoop();
	test_ParallelForLoop_WithThreadIndex();
}