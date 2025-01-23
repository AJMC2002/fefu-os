add_executable(simulator src/main.cpp src/serial_port.cpp)

target_link_libraries(simulator)

target_include_directories(simulator PUBLIC include)