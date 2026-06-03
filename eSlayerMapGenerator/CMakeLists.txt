cmake_minimum_required(VERSION 3.16)

project(eSlayerMapGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

add_library(eSlayerMapGenerator SHARED
    include/eSlayerMapGenerator/emapgenerator.h src/emapgenerator.cpp
    include/eSlayerMapGenerator/eslayermapgeneratorexport.h
    include/eSlayerMapGenerator/emapsettings.h src/emapsettings.cpp
    include/eSlayerMapGenerator/emap.h
    src/emap.cpp
    src/edungeongenerator.h src/edungeongenerator.cpp
    src/echamber.h src/echamber.cpp
    src/eopengenerator.h src/eopengenerator.cpp

)

target_include_directories(eSlayerMapGenerator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerMapGenerator
    PRIVATE ESLAYERMAPGENERATOR_EXPORTS
)

target_link_libraries(eSlayerMapGenerator PUBLIC eSlayerHelpers)
