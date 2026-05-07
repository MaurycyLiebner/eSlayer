cmake_minimum_required(VERSION 3.16)

project(eSlayerServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

add_library(eSlayerServer SHARED
    include/eSlayerServer/eserver.h src/eserver.cpp
    include/eSlayerServer/eslayerserverexport.h
    src/eserverunit.h
    src/esingleplayerserver.h src/esingleplayerserver.cpp
    src/eserverclienthandler.h src/eserverclienthandler.cpp
    src/eserverarea.h src/eserverarea.cpp
    src/eserverunit.cpp
    src/actions/eunitaction.h src/actions/eunitaction.cpp
    src/actions/eunitbaseaction.h src/actions/eunitbaseaction.cpp
    src/actions/emovetoenemyaction.h src/actions/emovetoenemyaction.cpp
    src/actions/ecomplexaction.h src/actions/ecomplexaction.cpp
    src/actions/eattackaction.h src/actions/eattackaction.cpp
    src/actions/ewaitaction.h src/actions/ewaitaction.cpp
    src/actions/eyieldwaitaction.h src/actions/eyieldwaitaction.cpp

    src/eserverfileloader.h src/eserverfileloader.cpp
    src/actions/edieaction.h src/actions/edieaction.cpp
    src/actions/ehitrecoveryaction.h src/actions/ehitrecoveryaction.cpp
    src/actions/eunitactionbase.h src/actions/eunitactionbase.cpp
    src/actions/eblockaction.h src/actions/eblockaction.cpp
    src/actions/eclientaction.h src/actions/eclientaction.cpp
    src/elocalserver.h src/elocalserver.cpp
    src/etcpiphost.h src/etcpiphost.cpp
    src/epacketdata.h
    src/etcpipjoin.h src/etcpipjoin.cpp
    src/eservermissile.h src/eservermissile.cpp
    src/actions/efolloweraction.h src/actions/efolloweraction.cpp
    src/emovetotarget.h src/emovetotarget.cpp
    src/actions/ewalkaroundaction.h src/actions/ewalkaroundaction.cpp
    src/actions/eknockbackaction.h src/actions/eknockbackaction.cpp
    src/eitemgenerator.h src/eitemgenerator.cpp
    src/eelitemodifiers.h src/eelitemodifiers.cpp


)

target_include_directories(eSlayerServer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerServer
    PRIVATE ESLAYERSERVER_EXPORTS
)

target_link_libraries(eSlayerServer PUBLIC eSlayerHelpers)
target_link_libraries(eSlayerServer PUBLIC eSlayerMapGenerator)
target_link_libraries(eSlayerServer PUBLIC eSlayerNet)
target_link_libraries(eSlayerServer PUBLIC eSlayerMissiles)
target_link_libraries(eSlayerServer PRIVATE SDL3::SDL3)
