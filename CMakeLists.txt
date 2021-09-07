cmake_minimum_required(VERSION 3.0)

project(syncpp)

add_subdirectory(extern)
set(DL dl)
set(CMAKE_CXX_FLAGS  "-std=c++14 -O3 -pthread -Wall")
set(GTEST_INCLUDE ${CMAKE_SOURCE_DIR}/extern/googletest/googletest/include)

set(GTEST_LIB gtest)
set(GTEST_MAIN_LIB gtest_main)

add_subdirectory(src)