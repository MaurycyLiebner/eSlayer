cmake_minimum_required(VERSION 3.16)

project(eSlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(eSlayerGame)
add_subdirectory(eSlayerServer)
add_subdirectory(eSlayerMapGenerator)