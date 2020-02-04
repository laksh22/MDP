macro(make_library target source)
    add_library(${target} ${source})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#    find_library(BLUEZ_LIBRARIES bluetooth)
#    target_link_libraries(${target} ${BLUEZ_LIBRARIES})
endmacro(make_library)

make_library(Bluetooth bluetooth.c)
