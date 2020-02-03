cmake_minimum_required(VERSION 3.13)
project(RPi C)

set(CMAKE_C_STANDARD 99)

add_executable(RPi main.c queue/queue.c queue/queue.h tcp/tcp.c tcp/tcp.h hub/hub.h hub/hub.c)

add_subdirectory(hub)
add_subdirectory(queue)
add_subdirectory(tap)
add_subdirectory(tcp)
add_subdirectory(tests)

enable_testing()

add_test(NAME DoTestQueue COMMAND TestQueue)