cmake_minimum_required(VERSION 3.9.0)
project(test)

find_package(Tiff CONFIG REQUIRED)

add_executable(test test.c)
target_link_libraries(test PRIVATE TIFF::tiff)
