﻿#include <iostream>

#include "Test_SPAMSSparseEncoder.h"

using namespace mdk;

int main()
{

    Test_DisplayMatrix();

    //Test_FindKNNByDistanceList();

    Test_OMP();


    std::system("pause");

    return 1;
}