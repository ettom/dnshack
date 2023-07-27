cmake_minimum_required(VERSION 3.11)

project(dnshack LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
add_subdirectory(dnshackbridge)
add_subdirectory(dnshackresolver)
