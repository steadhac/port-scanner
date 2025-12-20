cmake_minimum_required(VERSION 3.10)
project(port_scanner_cpp)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenSSL REQUIRED)

add_executable(port_scanner
    main.cpp
    scanner.cpp
    cert_utils.cpp
)

target_link_libraries(port_scanner OpenSSL::SSL OpenSSL::Crypto)