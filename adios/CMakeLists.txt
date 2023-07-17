cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(adios2Benchmark C CXX)

find_package(ADIOS2 REQUIRED)

set(common_deps adios2::cxx11)

add_executable(writer writer.cpp)
add_executable(reader reader.cpp)

target_link_libraries(writer ${common_deps})
target_link_libraries(reader ${common_deps})

#add_library(my_library src1.cxx src2.cxx)
#target_link_libraries(my_library PRIVATE adios2::cxx11_mpi MPI::MPI_C)