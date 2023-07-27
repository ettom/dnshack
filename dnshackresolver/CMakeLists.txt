cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined,leak)
    add_link_options(-fsanitize=address,undefined,leak)
endif()


project(dnshackresolver LANGUAGES CXX)


add_executable(dnshackresolver dnshackresolver.cpp)
find_path(CEREAL_INCLUDE_DIRS NAMES cereal/cereal.hpp REQUIRED)
target_include_directories(dnshackresolver PRIVATE ../common ${CEREAL_INCLUDE_DIRS})
target_link_options(dnshackresolver PRIVATE -static-libstdc++)

install(TARGETS dnshackresolver DESTINATION bin)
