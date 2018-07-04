cmake_minimum_required(VERSION 3.10)
project(simple_chat)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads)

include_directories(.)

add_executable(simple_chat chatclient.cpp chatserver.cpp client.cpp main.cpp server.cpp socket.cpp utils.cpp)
target_link_libraries(simple_chat Threads::Threads)
