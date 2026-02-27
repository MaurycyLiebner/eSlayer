cmake_minimum_required(VERSION 3.16)

project(eSlayerMapGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(eSlayerHelpers SHARED
    include/eSlayerHelpers/eslayerhelpersexport.h
    include/eSlayerHelpers/erand.h src/erand.cpp
    include/eSlayerHelpers/epoint.h src/epoint.cpp
    include/eSlayerHelpers/evec2.h
    include/eSlayerHelpers/epathfinder.h
    src/epathfinder.cpp

    include/eSlayerHelpers/epathfinderpath.h
    include/eSlayerHelpers/epathsmoother.h
    src/epathsmoother.cpp
    include/eSlayerHelpers/epathfindermap.h
    src/epathfindermap.cpp
    src/epathfinderpath.cpp
    include/eSlayerHelpers/emovementhandler.h
    src/emovementhandler.cpp
    include/eSlayerHelpers/emovementgoal.h
    src/emovementgoal.cpp
)

target_include_directories(eSlayerHelpers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerHelpers
    PRIVATE ESLAYERHELPERS_EXPORTS
)
