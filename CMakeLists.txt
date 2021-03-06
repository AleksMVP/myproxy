cmake_minimum_required(VERSION 3.13)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include")
include_directories("include")
include_directories("/usr/local/opt/openssl/include")
link_directories("/usr/lib")
link_directories("/usr/local/lib")
link_directories("/usr/local/opt/openssl/lib")


find_package(Boost REQUIRED system)
find_package(OpenSSL REQUIRED)

add_executable(MyProject src/main.cpp src/Client.cpp)
if (APPLE)
    target_link_libraries (MyProject -pthread -lboost_coroutine -lboost_chrono -lboost_thread-mt -lboost_system OpenSSL::SSL OpenSSL::Crypto)
endif ()

if (UNIX AND NOT APPLE)
target_link_libraries (MyProject -pthread -lboost_coroutine -lboost_chrono -lboost_thread -lboost_system OpenSSL::SSL OpenSSL::Crypto)
endif ()
