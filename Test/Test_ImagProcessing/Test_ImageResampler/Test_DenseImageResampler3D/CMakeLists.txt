cmake_minimum_required(VERSION 2.8)
 
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
 
PROJECT(Test_DenseImageResampler3D)

set(MDK_DIR "C:/Research/MDK/MDK_Code" CACHE PATH "path")
find_package(MDK REQUIRED)

FILE(GLOB Test_FILES "*.h" "*.hpp" "*.cpp")

add_executable(Test_DenseImageResampler3D ${Test_FILES})

target_link_libraries(Test_DenseImageResampler3D MDK_LIB)