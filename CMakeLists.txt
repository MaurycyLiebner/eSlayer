cmake_minimum_required(VERSION 3.16)

project(eSlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)

# RPATH configuration
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_RPATH ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_RPATH FALSE)

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
