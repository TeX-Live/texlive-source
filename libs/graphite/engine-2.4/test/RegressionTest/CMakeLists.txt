CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)
project(gr-regression-test)

include_directories(${PROJECT_SOURCE_DIR}/../../include/graphite)

add_executable(gr-regression-test
	GrJustifier.cpp
	RegressionTest.cpp
	SimpleTextSrc.cpp
	TestCase.cpp
	stdafx.cpp
	)

target_link_libraries(gr-regression-test graphite)
