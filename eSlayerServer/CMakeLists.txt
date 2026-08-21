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
    include/eSlayerServer/eslayerserverexport.h
    include/eSlayerServer/eserver.h src/eserver.cpp

    src/eserverunit.h src/eserverunit.cpp
    src/eserverclienthandler.h src/eserverclienthandler.cpp

    src/eserverarea.h src/eserverarea.cpp
    src/eserverarea_addunits.cpp
    src/eserverarea_addclient.cpp
    src/eserverarea_items.cpp
    src/eserverarea_increment.cpp
    src/eserverarea_initialize.cpp
    src/eserverarea_data.cpp
    src/eserverarea_body.cpp
    src/eserverarea_cast.cpp
    src/eserverarea_portal.cpp
    src/eserverarea_clientactions.cpp

    src/esingleplayerserver.h src/esingleplayerserver.cpp
    src/elocalserver.h src/elocalserver.cpp
    src/etcpiphost.h src/etcpiphost.cpp
    src/etcpipjoin.h src/etcpipjoin.cpp

    src/actions/eunitaction.h src/actions/eunitaction.cpp
    src/actions/eunitbaseaction.h src/actions/eunitbaseaction.cpp
    src/actions/emovetoenemyaction.h src/actions/emovetoenemyaction.cpp
    src/actions/ecomplexaction.h src/actions/ecomplexaction.cpp
    src/actions/eattackaction.h src/actions/eattackaction.cpp
    src/actions/ewaitaction.h src/actions/ewaitaction.cpp
    src/actions/eyieldwaitaction.h src/actions/eyieldwaitaction.cpp
    src/actions/edieaction.h src/actions/edieaction.cpp
    src/actions/ehitrecoveryaction.h src/actions/ehitrecoveryaction.cpp
    src/actions/eunitactionbase.h src/actions/eunitactionbase.cpp
    src/actions/eblockaction.h src/actions/eblockaction.cpp
    src/actions/eclientaction.h src/actions/eclientaction.cpp
    src/actions/efolloweraction.h src/actions/efolloweraction.cpp
    src/actions/emovetotarget.h src/actions/emovetotarget.cpp
    src/actions/ewalkaroundaction.h src/actions/ewalkaroundaction.cpp
    src/actions/eknockbackaction.h src/actions/eknockbackaction.cpp
    src/actions/eexplodeaction.h src/actions/eexplodeaction.cpp
    src/actions/efleeaction.h src/actions/efleeaction.cpp
    src/actions/emovetangentaction.h src/actions/emovetangentaction.cpp
    src/actions/enpcaction.h src/actions/enpcaction.cpp

    src/eservermissile.h src/eservermissile.cpp
    src/eservernova.h src/eservernova.cpp
    src/eserverskillarea.h src/eserverskillarea.cpp

    src/eserverfileloader.h src/eserverfileloader.cpp
    src/epacketdata.h
    src/eitemgenerator.h src/eitemgenerator.cpp
    src/eelitemodifiers.h src/eelitemodifiers.cpp
    src/ethreadsafe.h
    src/eservermovementhandler.h src/eservermovementhandler.cpp
    src/eclientdata.h src/eclientdata.cpp
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
