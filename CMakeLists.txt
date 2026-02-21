cmake_minimum_required(VERSION 3.22.1)
project(spsc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(spsc main.cpp)

target_include_directories(spsc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/spsc/include)
target_compile_options(spsc PRIVATE -Wall -Wextra)

add_subdirectory(tests)
