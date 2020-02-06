macro(make_library target source)
    add_library(${target} ${source})
    target_include_directories(${target} PUBLIC .)
endmacro(make_library)

make_library(Serial serial.c)