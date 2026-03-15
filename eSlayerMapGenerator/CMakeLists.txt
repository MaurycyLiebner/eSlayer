cmake_minimum_required(VERSION 3.16)

project(eSlayerMapGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(eSlayerMapGenerator SHARED
    include/eSlayerMapGenerator/emapgenerator.h src/emapgenerator.cpp
    include/eSlayerMapGenerator/eslayermapgeneratorexport.h
)

target_include_directories(eSlayerMapGenerator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerMapGenerator
    PRIVATE ESLAYERMAPGENERATOR_EXPORTS
)

target_link_libraries(eSlayerMapGenerator PUBLIC eSlayerHelpers)
