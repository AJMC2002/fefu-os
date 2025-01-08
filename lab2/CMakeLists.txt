cmake_minimum_required(VERSION 3.10)
project(Lab2)

set(CMAKE_CXX_STANDARD 17)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

if(WIN32)
  set(EXECUTABLE_NAME main.exe)
else()
  set(EXECUTABLE_NAME main.out)
endif()

add_executable(${EXECUTABLE_NAME} main.cpp)
