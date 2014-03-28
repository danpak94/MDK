#ifndef __TestMatrix_h
#define __TestMatrix_h

//================================================
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <initializer_list>
#include <functional>

#include "mdkDenseMatrix.h"

namespace mdk
{

template<typename T>
void DisplayMatrix(const std::string& Name, const DenseMatrix<T>& Matrix, uint32 value_std_setw = 6, uint32 precision = 4)
{
    std::cout << Name << " = " << '\n';

    for (int64 i = 0; i < Matrix.GetRowNumber(); ++i)
    {
        for (int64 j = 0; j < Matrix.GetColNumber(); ++j)
        {
            //std::cout << std::fixed << std::setprecision(precision) << Matrix(i, j) << ' ';

            std::cout << std::setw(value_std_setw + precision) << std::setprecision(precision) << Matrix(i, j) << ' ';
        }
        std::cout << '\n';
    }
}

void print_vec(const std::vector<int>& vec)
{
    for (auto x : vec) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';
}

void test_vec_insert()
{
    std::vector<int> vec;
    print_vec(vec);

    //auto it = vec.begin();
    //it = vec.insert(it, { 1, 2 });
    //print_vec(vec);

    std::vector<int> vec2 = {1, 2};

    auto it = vec.begin();
    it = vec.insert(it, vec2.begin(), vec2.end());
    print_vec(vec);
}


void Test_SubMatrix()
{
    DenseMatrix<double> A;

    A.Resize(5, 6);

    A = { 1,     2,     3,     4,     5,     6,
          10,    20,    30,    40,    50,    60,
          100,   200,   300,   40,    500,   600,
          1000,  2000,  3000,  4000,  5000,  6000,
          10000, 20000, 30000, 40000, 50000, 60000, };

    DisplayMatrix("A", A);

    // DenseMatrix<double> subA1 = A.GetSubMatrix(span(0, 3), span(2, 5));

    DisplayMatrix("A", A);

    // aa is scalar
    auto aa = A({  });

    //this can not be compiled
    //auto bb = A({0});

    DenseMatrix<double> testA = { 0};

    DenseMatrix<double> subA2;
    
    subA2 = A({ 1, 2 });

    DisplayMatrix("subA2", subA2);


    DenseMatrix<double> B(1,2);

    DenseMatrix<double> subA3 = A.at({ 1, 2 });

   // DisplayMatrix("subA3", subA3);

    /*
    // DenseMatrix<double> subA2 = A.at({ 1 }, { 0, 1 });

    

    DenseMatrix<double> subA3;

    // subA3 = A({ 1, 2, 3 }, { 0, 1, 2 });

    // DenseMatrix<double> subA3 = A.at({ 1, 2, 3 }, { 0, 1, 2 });

    // DisplayMatrix("subA3", subA3);

    DenseMatrix<double> subAa = A.at(ALL, { 0, 1, 2, 1, 0 });

    DisplayMatrix("subAa", subAa);

    DenseMatrix<double> subAb = A.at({ 0, 1, 2, 1, 0 }, ALL);

    DisplayMatrix("subAb", subAb);
    */
}





}//namespace mdk

#endif