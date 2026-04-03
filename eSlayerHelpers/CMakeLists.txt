cmake_minimum_required(VERSION 3.16)

project(eSlayerMapGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip.git
    GIT_TAG v1.11.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(libzip)

FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 11.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(tinyxml2)

add_library(eSlayerHelpers SHARED
    include/eSlayerHelpers/eslayerhelpersexport.h
    include/eSlayerHelpers/erand.h src/erand.cpp
    include/eSlayerHelpers/epoint.h src/epoint.cpp
    include/eSlayerHelpers/evec2.h
    include/eSlayerHelpers/epathfinder.h
    src/epathfinder.cpp

    include/eSlayerHelpers/epathfinderpath.h


    include/eSlayerHelpers/epathfindermap.h
    src/epathfindermap.cpp
    src/epathfinderpath.cpp
    include/eSlayerHelpers/emovementhandler.h
    src/emovementhandler.cpp
    include/eSlayerHelpers/emovementgoal.h
    src/emovementgoal.cpp

    include/eSlayerHelpers/eexceptions.h src/eexceptions.cpp
    include/eSlayerHelpers/eunitdata.h
    src/eunitdata.cpp
    include/eSlayerHelpers/eunitdynamicdata.h
    src/eunitdynamicdata.cpp
    include/eSlayerHelpers/echardata.h
    include/eSlayerHelpers/eoffset.h
    src/echardata.cpp
    include/eSlayerHelpers/efileloaderbase.h
    src/efileloaderbase.cpp
    src/eziploader.cpp
    include/eSlayerHelpers/eziploader.h
    include/eSlayerHelpers/erunsettings.h
    src/erunsettings.cpp
    include/eSlayerHelpers/egamedir.h
    src/egamedir.cpp
    include/eSlayerHelpers/epacket.h
    include/eSlayerHelpers/erequestdata.h
    src/erequestdata.cpp
    src/evectorhelpers.cpp
    include/eSlayerHelpers/evectorhelpers.h
    include/eSlayerHelpers/eattackdata.h
    include/eSlayerHelpers/eidmapvector.h
    include/eSlayerHelpers/emissile.h
    src/emissile.cpp
    include/eSlayerHelpers/epositioned.h
    include/eSlayerHelpers/estringidmapvector.h
    include/eSlayerHelpers/eunitarea.h

    include/eSlayerHelpers/eskills.h
    src/eskills.cpp

    include/eSlayerHelpers/eunitareas.h
    src/eunitareas.cpp
    include/eSlayerHelpers/edamage.h
    src/edamage.cpp
    src/echardatainfo.cpp
    include/eSlayerHelpers/echardatainfo.h
    include/eSlayerHelpers/eequipment.h
    include/eSlayerHelpers/eitem.h
    include/eSlayerHelpers/emodifier.h
    src/emodifier.cpp
    src/eitem.cpp
    src/eequipment.cpp
    include/eSlayerHelpers/eweapontype.h
    include/eSlayerHelpers/eskillchoice.h
    include/eSlayerHelpers/eweaponchoice.h
    include/eSlayerHelpers/eitemdata.h
    include/eSlayerHelpers/eitemsdata.h
    src/eitemsdata.cpp
    include/eSlayerHelpers/egrounditem.h
    src/egrounditem.cpp
    include/eSlayerHelpers/escreendimensions.h
    src/escreendimensions.cpp
    include/eSlayerHelpers/eattributes.h
    src/eattributes.cpp
    include/eSlayerHelpers/estats.h
    src/estats.cpp
    src/echaracter.cpp
    include/eSlayerHelpers/echaracter.h
    include/eSlayerHelpers/eitemrarity.h
    src/eitemrarity.cpp
    src/eitemdata.cpp
    src/eweapontype.cpp
    include/eSlayerHelpers/estringhelpers.h
    src/estringhelpers.cpp


)

target_include_directories(eSlayerHelpers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(eSlayerHelpers
    PRIVATE ESLAYERHELPERS_EXPORTS
)

target_link_libraries(eSlayerHelpers PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(eSlayerHelpers PUBLIC libzip::zip)
target_link_libraries(eSlayerHelpers PUBLIC tinyxml2::tinyxml2)
