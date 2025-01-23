add_executable(main main.cpp logger.cpp temperature_reader.cpp ../simulator/src/serial_port.cpp)

target_include_directories(main PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/simulator/include
)