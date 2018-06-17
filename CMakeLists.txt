cmake_minimum_required(VERSION 3.5)

project(huffman)

set(CMAKE_CXX_STANDARD 17)

add_library(huffman_lib
            huffman/huffman.hpp
            huffman/tree.hpp    huffman/tree.cpp)

add_library(gtest
            gtest/gtest.h
            gtest/gtest-all.cc gtest/gtest_main.cc)

add_executable(test huffman_testing.cpp)
target_link_libraries(test gtest huffman_lib -lpthread)

add_executable(huffman cli.cpp)
target_link_libraries(huffman huffman_lib)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()
