enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_compile_definitions("SOURCE_ROOT=${CMAKE_SOURCE_DIR}")

add_library(proc ../lib/run_processes.cpp ../lib/run_processes.h)
add_executable(test main.cpp)

target_link_libraries(test proc GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test)
