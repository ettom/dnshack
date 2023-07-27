cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

project(dnshackbridge LANGUAGES CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined,leak)
    add_link_options(-fsanitize=address,undefined,leak)
endif()

find_path(CEREAL_INCLUDE_DIRS NAMES cereal/cereal.hpp REQUIRED)
set(INCLUDE_DIRS ../common ${CEREAL_INCLUDE_DIRS})

add_library(dnshackbridge SHARED dnshackbridge.cpp)
target_include_directories(dnshackbridge PRIVATE ${INCLUDE_DIRS})
target_link_options(dnshackbridge PRIVATE -static-libstdc++)

install(TARGETS dnshackbridge DESTINATION lib)

if(ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_executable(tests test.cpp)
    target_include_directories(tests PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
