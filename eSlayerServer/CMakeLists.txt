cmake_minimum_required(VERSION 3.16)

project(eSlayerServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(eSlayerServer SHARED
    include/eSlayerServer/eserver.h src/eserver.cpp
    include/eSlayerServer/eslayerserverexport.h
    src/eserverunit.h
    src/esingleplayerserver.h src/esingleplayerserver.cpp
    src/eserverclienthandler.h src/eserverclienthandler.cpp
    src/eserverarea.h src/eserverarea.cpp
    src/eserverunit.cpp
    src/eunitaction.h src/eunitaction.cpp
    src/eunitbaseaction.h src/eunitbaseaction.cpp
)

target_include_directories(eSlayerServer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerServer
    PRIVATE ESLAYERSERVER_EXPORTS
)

target_link_libraries(eSlayerServer PUBLIC eSlayerHelpers)
target_link_libraries(eSlayerServer PUBLIC eSlayerMapGenerator)
