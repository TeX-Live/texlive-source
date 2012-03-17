CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

project(graphite-engine)
enable_language(CXX C)

include_directories(${PROJECT_SOURCE_DIR}/include)

enable_testing()

add_subdirectory(src)
# add_subdirectory(include)

# using a binary directory prevents a name clash between the test target and test directory
add_subdirectory(test/ProfileHarness ${PROJECT_BINARY_DIR}/ProfileHarness)
add_subdirectory(test/RegressionTest ${PROJECT_BINARY_DIR}/RegressionTest)

# This is the release version, there is also the API SOVERSION in engine/src/CMakeLists.txt
set(version 2.3.1)
set(libdir ${CMAKE_INSTALL_PREFIX}/lib)
set(includedir ${CMAKE_INSTALL_PREFIX}/include)
configure_file(silgraphite.pc.in silgraphite.pc)
install(FILES ${PROJECT_BINARY_DIR}/silgraphite.pc DESTINATION lib/pkgconfig)

add_test(NAME gr-regression-test
	COMMAND "$<TARGET_FILE:gr-regression-test>" -p ${PROJECT_SOURCE_DIR}/test/RegressionTest)

