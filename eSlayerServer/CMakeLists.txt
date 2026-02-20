cmake_minimum_required(VERSION 3.16)

project(eSlayerMapGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(eSlayerServer SHARED
    include/eSlayerServer/eserver.h src/eserver.cpp
    include/eSlayerServer/eslayerserverexport.h
)

target_include_directories(eSlayerServer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerServer
    PRIVATE ESLAYERSERVER_EXPORTS
)

target_link_libraries(eSlayerServer PUBLIC eSlayerMapGenerator)
