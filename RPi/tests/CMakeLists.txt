macro(make_test target source library)
    add_executable(${target} ${source})
    target_link_libraries(${target} Libtap ${library})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(make_test)

make_test(DoTestQueue test_queue.c Queue)