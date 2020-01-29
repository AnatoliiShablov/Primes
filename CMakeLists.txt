cmake_minimum_required(VERSION 3.5)

project(Primes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest)

add_library(primes_lib STATIC lib/include/primes.h
                              lib/src/primes.cpp)

add_executable(test gtest/main.cpp)
target_link_libraries(test PRIVATE primes_lib GTest::GTest)
