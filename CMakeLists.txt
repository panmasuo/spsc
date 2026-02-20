cmake_minimum_required(VERSION 3.22.1)
project(spsc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spsc)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/spsc/include)

file(GLOB SOURCES ${SOURCES_DIR}/*.cpp)
include_directories(${INCLUDE_DIR})

add_compile_options(-Wall)
add_executable(spsc ${SOURCES})
