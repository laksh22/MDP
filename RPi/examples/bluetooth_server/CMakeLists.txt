cmake_minimum_required(VERSION 3.6)
project(BServe C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG_ON)

find_package(Threads REQUIRED)

add_executable(BServe bluetooth_server.c)

find_library(bluez_LIB bluetooth)

target_link_libraries(BServe Threads::Threads)
target_link_libraries(BServe ${bluez_LIB})