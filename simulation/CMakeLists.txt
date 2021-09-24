cmake_minimum_required(VERSION 3.16)
project(simulation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS
        -pthread
        )

add_executable(simulation main.cpp node.h SINR.h Const.h statistics.h LocalBroadcast.h BaseRound.h GlobalBroadcast.h)