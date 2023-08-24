cmake_minimum_required(VERSION 3.0)

# This list is only to guide IDE setups.
# CMake may or may not be able to (only) build the library
#   with these configs depending on your platform setup.

project(Godot.rb C)
set(CMAKE_C_STANDARD 17)

find_package(Ruby 3.2 EXACT REQUIRED)
include_directories(../include ${Ruby_INCLUDE_DIRS})
link_libraries(${Ruby_LIBRARY})

file(GLOB SOURCES *.c)
add_library(Godot_rb SHARED ${SOURCES})
