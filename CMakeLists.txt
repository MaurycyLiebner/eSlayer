cmake_minimum_required(VERSION 3.16)

project(eSlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

add_subdirectory(eSlayerHelpers)
add_subdirectory(eSlayerGame)
add_subdirectory(eSlayerServer)
add_subdirectory(eSlayerMapGenerator)
add_subdirectory(eSlayerNet)
add_subdirectory(eSlayerMissiles)
