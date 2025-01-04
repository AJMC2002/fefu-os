cmake_minimum_required(VERSION 3.10)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    set(EXECUTABLE_NAME HelloWorld.exe)
else()
    set(EXECUTABLE_NAME HelloWorld.out)
endif()

add_executable(${EXECUTABLE_NAME} main.cpp)
