cmake_minimum_required(VERSION 3.13)
project(RPi C)

set(CMAKE_C_STANDARD 99)

add_executable(RPi main.c queue/queue.c queue/queue.h)