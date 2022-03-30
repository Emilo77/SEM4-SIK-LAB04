cmake_minimum_required (VERSION 3.1)

project(MIMUW_SIK_TCP_SOCKETS)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-g -Wall -Wextra")

add_executable(echo-server echo-server.c common.h err.h)
add_executable(echo-client echo-client.c common.h err.h)

add_executable(nums-server nums-server.c common.h err.h)
add_executable(nums-client nums-client.c common.h err.h)

add_executable(struct-in-memory struct-in-memory.c common.h err.h)

install(TARGETS DESTINATION .)
