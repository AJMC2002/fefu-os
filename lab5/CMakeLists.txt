cmake_minimum_required(VERSION 3.14)
project(TemperatureServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # relevant for lsp

include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.7.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libpqxx)

add_subdirectory(src)
