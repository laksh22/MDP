cmake_minimum_required(VERSION 3.6)
project(RPi C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG_ON)

find_package(Threads REQUIRED)

add_executable(RPi main.c settings.h
        camera/camera.h camera/camera.c
        hub/hub.h hub/hub.c
        rpa_queue/rpa_queue.h rpa_queue/rpa_queue.c
        serial/serial.h serial/serial.c
        bluetooth/bluetooth.h bluetooth/bluetooth.c
        tcp/tcp.h tcp/tcp.c)

#find_library(wiringPi_LIB wiringPi)
#find_library(bluez_LIB bluetooth)

target_link_libraries(RPi Threads::Threads)
#target_link_libraries(RPi ${wiringPi_LIB})
#target_link_libraries(RPi ${bluez_LIB})

add_subdirectory(bluetooth)
add_subdirectory(camera)
add_subdirectory(hub)
add_subdirectory(rpa_queue)
add_subdirectory(serial)
add_subdirectory(tcp)
