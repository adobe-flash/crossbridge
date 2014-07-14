cmake_minimum_required(VERSION 2.8)

include_directories(../include)
include_directories(../src/mesa)
include_directories(../src/glsl)

option (DEBUG "Enable debugging" FALSE)

if(${DEBUG} MATCHES "on")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -g -O0 -DUNIX")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -fno-rtti -g -O0 -DUNIX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -O4 -DNDEBUG -DUNIX")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -fno-rtti -O4 -DNDEBUG -DUNIX")
endif()

file(GLOB agalasm_sources ../agalassembler/*.cpp)
file(GLOB glcpp-library_sources ../src/glsl/glcpp/*.c)
#file(GLOB glcpp-library_sources_remove ../src/glsl/glcpp/glcpp.c)
#list(REMOVE_ITEM glcpp-library_sources ${glcpp-library_sources_remove})
file(GLOB mesa_sources ../src/mesa/program/*.c)
file(GLOB glsl_sources ../src/glsl/*.cpp ../src/glsl/*.c)
file(GLOB glsl_sources_remove ../src/glsl/main.cpp ../src/glsl/builtin_stubs.cpp ../src/glsl/swc.cpp)
list(REMOVE_ITEM glsl_sources ${glsl_sources_remove})

add_library(glsl2agal ${agalasm_sources} ${glcpp-library_sources} ${mesa_sources} ${glsl_sources})