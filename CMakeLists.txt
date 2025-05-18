cmake_minimum_required(VERSION 3.15)
project (one_million_net)

set (default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE)
   message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
   set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include_directories(${CMAKE_SOURCE_DIR}/depends/include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using MSVC compiler")
    add_compile_options(/W4 /permissive-)
else()
    message(STATUS "Using non-MSVC compiler, applying OS_CXX_FLAGS: ${OS_CXX_FLAGS}")
    if (OS_CXX_FLAGS)
        add_compile_options(${OS_CXX_FLAGS})
    endif()
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/)

add_subdirectory(server)
add_subdirectory(client)
