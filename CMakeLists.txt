cmake_minimum_required(VERSION 3.9.2)
project(avldatabase)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(deps/googletest)

include_directories(include)

add_executable(avldatabase main.cpp) 

enable_testing()

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

foreach(_test_file ${TEST_SRC_FILES})
  get_filename_component(_test_name ${_test_file} NAME_WE)
  add_executable(${_test_name} ${_test_file})
  target_link_libraries(${_test_name} gtest gtest_main)
  add_test(NAME ${_test_name} COMMAND ${_test_name})
endforeach()