cmake_minimum_required(VERSION 3.10)
project(counterlib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # relevant for lsp

set(INCLUDE_DIR include)
set(SRC_DIR src)

include_directories(${INCLUDE_DIR})

add_library(moski STATIC ${SRC_DIR}/counter.cpp ${SRC_DIR}/logger.cpp)

add_executable(main ${SRC_DIR}/main.cpp)

target_link_libraries(main PRIVATE moski)

target_compile_options(moski PRIVATE -Wall -Wextra -pedantic)
target_compile_options(main PRIVATE -Wall -Wextra -pedantic)
