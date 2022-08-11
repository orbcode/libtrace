set(NAME tests)

add_library(${NAME} STATIC)

target_include_directories(${NAME} PRIVATE
    cmsis
    include
)

target_sources(${NAME} PRIVATE
    src/try_compile.c
    src/try_compile.cpp
)

target_link_libraries(${NAME} PRIVATE
    Orbcode::Trace
)
