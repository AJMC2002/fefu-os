add_executable(server
    main.cpp
    database.cpp
    log_parser.cpp
    server.cpp
)

target_include_directories(server
    PRIVATE
    ../include
    ${httplib_SOURCE_DIR}
    ${libpqxx_SOURCE_DIR}/include
)

target_link_libraries(server
    PRIVATE
    httplib
    pqxx
)