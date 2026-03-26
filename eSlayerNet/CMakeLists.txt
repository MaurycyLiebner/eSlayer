cmake_minimum_required(VERSION 3.16)

project(eSlayerServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include(FetchContent)

FetchContent_Declare(
    SDL3_net
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
    GIT_TAG 6623e846d93e0e7474877c21380aef852aac7e18
)

FetchContent_MakeAvailable(SDL3_net)

add_library(eSlayerNet SHARED
    include/eSlayerNet/eslayernetexport.h
    include/eSlayerNet/etcpnetwork.h
    src/etcpnetwork.cpp

)

target_include_directories(eSlayerNet
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerNet
    PRIVATE ESLAYERNET_EXPORTS
)

target_link_libraries(eSlayerNet PUBLIC SDL3_net::SDL3_net)
target_link_libraries(eSlayerNet PUBLIC eSlayerHelpers)
