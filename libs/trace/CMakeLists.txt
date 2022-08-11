set(NAME _orbcode_liborbtrace)

add_library(${NAME} INTERFACE)

target_include_directories(${NAME} INTERFACE include)

add_library(Orbcode::Trace ALIAS _orbcode_liborbtrace)
