cmake_minimum_required(VERSION 3.16)
project(210825)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS
        -pthread
        )

add_executable(210825 main.cpp node.h SINR.h Const.h statistics.h LocalBroadcast.h BaseRound.h GlobalBroadcast.h)