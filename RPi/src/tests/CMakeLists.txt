macro(make_test target source library)
    add_executable(${target} ${source})
    target_link_libraries(${target} Libtap ${library})
    target_include_directories(${target} PUBLIC .)
endmacro(make_test)

make_test(DoTestRpaQueue test_rpa_queue.c RpaQueue)