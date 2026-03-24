cmake_minimum_required(VERSION 3.16)

project(eSlayerMissiles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(eSlayerMissiles SHARED
    include/eSlayerMissiles/eslayermissilesexport.h
    include/eSlayerMissiles/emissileincrement.h src/emissileincrement.cpp
    include/eSlayerMissiles/emissilecollision.h src/emissilecollision.cpp
)

target_include_directories(eSlayerMissiles
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerMissiles
    PRIVATE ESLAYERMISSILES_EXPORTS
)

target_link_libraries(eSlayerMissiles PUBLIC eSlayerHelpers)
