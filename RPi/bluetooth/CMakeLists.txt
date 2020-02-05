macro(make_library target source)
    add_library(${target} ${source})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(make_library)

make_library(Bluetooth bluetooth.c)
target_link_libraries(Bluetooth "bluetooth")
