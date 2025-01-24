cmake_minimum_required(VERSION 3.10)
project(counterlib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # relevant for lsp
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(INCLUDE_DIR include)
set(SRC_DIR src)

include_directories(${INCLUDE_DIR})

add_library(
  moski STATIC
  ${SRC_DIR}/counter.cpp
  ${SRC_DIR}/logger.cpp
  ${SRC_DIR}/process/process.cpp
  ${SRC_DIR}/process/subprocessA.cpp
  ${SRC_DIR}/process/subprocessB.cpp
  ${SRC_DIR}/signal_handler.cpp
  ${SRC_DIR}/utils.cpp)

add_executable(main ${SRC_DIR}/main.cpp)

target_link_libraries(main PUBLIC moski)

target_compile_options(moski PRIVATE -g -Wall -Wextra -pedantic)
target_compile_options(main PRIVATE -g -Wall -Wextra -pedantic)
