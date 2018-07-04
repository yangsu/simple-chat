cmake_minimum_required(VERSION 3.10)
project(simple_chat)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads)

include_directories(.)

add_executable(simple_chat
        chatclient.cpp
        chatclient.h
        chatserver.cpp
        chatserver.h
        client.cpp
        client.h
        main.cpp
        server.cpp
        server.h
        socket.cpp
        socket.h
        utils.cpp
        utils.h)
target_link_libraries(simple_chat Threads::Threads)
