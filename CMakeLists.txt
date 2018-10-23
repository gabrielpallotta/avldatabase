cmake_minimum_required(VERSION 3.12)
project(avldatabase)

include_directories(include)

file(GLOB SOURCE_FILES "src/*.cpp")

add_executable(avldatabase ${SOURCE_FILES} main.cpp) 