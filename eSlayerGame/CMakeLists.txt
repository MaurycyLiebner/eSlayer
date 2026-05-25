cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

include(FetchContent)

# Disable unnecessary SDL components (optional but recommended)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF)
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
if(MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif()

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3_ttf)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.4.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3_image)

FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG prerelease-3.1.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3_mixer)

FetchContent_Declare(
    rapidcsv
    GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
    GIT_TAG v8.90
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(rapidcsv)

add_executable(eSlayerGame
    main.cpp

    # Audio
    audio/eaudiovector.cpp
    audio/eaudiovector.h
    audio/emusic.cpp
    audio/emusic.h
    audio/emusicvector.cpp
    audio/emusicvector.h
    audio/esounds.cpp
    audio/esounds.h
    audio/esoundvector.cpp
    audio/esoundvector.h

    # Core
    echaracters.cpp
    echaracters.h
    efileloader.cpp
    efileloader.h
    egameinput.cpp
    egameinput.h
    egameworld.cpp
    egameworld.h
    elanguage.cpp
    elanguage.h
    elight.cpp
    elight.h
    eloadtexthelper.cpp
    eloadtexthelper.h
    emaincharaction.cpp
    emaincharaction.h
    emainwindow.cpp
    emainwindow.h
    erendersettings.cpp
    erendersettings.h
    eresolution.cpp
    eresolution.h
    ewindowsettings.cpp
    ewindowsettings.h

    # Names
    names/eareanames.cpp
    names/eareanames.h
    names/eclassnames.cpp
    names/eclassnames.h
    names/eitemnames.cpp
    names/eitemnames.h
    names/emonsternames.cpp
    names/emonsternames.h
    names/eobjectnames.cpp
    names/eobjectnames.h
    names/eskillnames.cpp
    names/eskillnames.h
    names/eskilltreenames.cpp
    names/eskilltreenames.h
    names/eelitemodifiersnames.cpp
    names/eelitemodifiersnames.h

    # Screens
    screens/echoosecharactermenu.cpp
    screens/echoosecharactermenu.h
    screens/ecreatecharactermenu.cpp
    screens/ecreatecharactermenu.h
    screens/eerrorscreen.cpp
    screens/eerrorscreen.h
    screens/egamescreen.cpp
    screens/egamescreen.h
    screens/eloadingscreen.cpp
    screens/eloadingscreen.h
    screens/emainmenu.cpp
    screens/emainmenu.h
    screens/escreenbase.cpp
    screens/escreenbase.h
    screens/escreenhandler.cpp
    screens/escreenhandler.h
    screens/esettingsmenu.cpp
    screens/esettingsmenu.h
    screens/etcpipgamemenu.cpp
    screens/etcpipgamemenu.h
    screens/etcpipjoinmenu.cpp
    screens/etcpipjoinmenu.h

    # Textures
    textures/echarmodel.cpp
    textures/echarmodel.h
    textures/echarstextures.cpp
    textures/echarstextures.h
    textures/echartextures.cpp
    textures/echartextures.h
    textures/echarunitmodel.cpp
    textures/echarunitmodel.h
    textures/eitemstextures.cpp
    textures/eitemstextures.h
    textures/elightinghandler.cpp
    textures/elightinghandler.h
    textures/emaptextures.cpp
    textures/emaptextures.h
    textures/emissilestextures.cpp
    textures/emissilestextures.h
    textures/eobjstextures.cpp
    textures/eobjstextures.h
    textures/epaintcall.cpp
    textures/epaintcall.h
    textures/erendertargetholder.cpp
    textures/erendertargetholder.h
    textures/espriteloader.cpp
    textures/espriteloader.h
    textures/eterrstextures.cpp
    textures/eterrstextures.h
    textures/etextgenerator.cpp
    textures/etextgenerator.h
    textures/etexture.cpp
    textures/etexture.h
    textures/etexturecollection.cpp
    textures/etexturecollection.h
    textures/etiletextures.cpp
    textures/etiletextures.h
    textures/etilesiterator.cpp
    textures/etilesiterator.h
    textures/euitextures.cpp
    textures/euitextures.h

    # Units
    units/eunit.cpp
    units/eunit.h

    # Widgets
    widgets/ebuttonbase.cpp
    widgets/ebuttonbase.h
    widgets/echeckablebutton.cpp
    widgets/echeckablebutton.h
    widgets/echeckbox.cpp
    widgets/echeckbox.h
    widgets/echeckbutton.cpp
    widgets/echeckbutton.h
    widgets/ecolors.cpp
    widgets/ecolors.h
    widgets/ealignment.h
    widgets/efontcolor.cpp
    widgets/efontcolor.h
    widgets/efonts.cpp
    widgets/efonts.h
    widgets/elabel.cpp
    widgets/elabel.h
    widgets/elabelbase.cpp
    widgets/elabelbase.h
    widgets/elineedit.cpp
    widgets/elineedit.h
    widgets/emouseevent.cpp
    widgets/emouseevent.h
    widgets/enamedcheckbox.cpp
    widgets/enamedcheckbox.h
    widgets/epainter.cpp
    widgets/epainter.h
    widgets/eprogressbar.cpp
    widgets/eprogressbar.h
    widgets/escrollwidget.cpp
    widgets/escrollwidget.h
    widgets/etexturecheckbutton.cpp
    widgets/etexturecheckbutton.h
    widgets/ewidget.cpp
    widgets/ewidget.h

    # Widgets - Game Screen
    widgets/gameScreen/ebgwidget.cpp
    widgets/gameScreen/ebgwidget.h
    widgets/gameScreen/ecoinswidget.cpp
    widgets/gameScreen/ecoinswidget.h
    widgets/gameScreen/eescmenu.cpp
    widgets/gameScreen/eescmenu.h
    widgets/gameScreen/eescmenubutton.cpp
    widgets/gameScreen/eescmenubutton.h
    widgets/gameScreen/egamepainter.cpp
    widgets/gameScreen/egamepainter.h
    widgets/gameScreen/egamewidget.cpp
    widgets/gameScreen/egamewidget.h
    widgets/gameScreen/egrounditemnames.cpp
    widgets/gameScreen/egrounditemnames.h
    widgets/gameScreen/ehealthindicator.cpp
    widgets/gameScreen/ehealthindicator.h
    widgets/gameScreen/ehovergenerator.cpp
    widgets/gameScreen/ehovergenerator.h
    widgets/gameScreen/ehoverwidget.cpp
    widgets/gameScreen/ehoverwidget.h
    widgets/gameScreen/einventorybagpackwidget.cpp
    widgets/gameScreen/einventorybagpackwidget.h
    widgets/gameScreen/einventorywidget.cpp
    widgets/gameScreen/einventorywidget.h
    widgets/gameScreen/eitemplacewidget.cpp
    widgets/gameScreen/eitemplacewidget.h
    widgets/gameScreen/eminimap.cpp
    widgets/gameScreen/eminimap.h
    widgets/gameScreen/eplayerhealthindicator.cpp
    widgets/gameScreen/eplayerhealthindicator.h
    widgets/gameScreen/eskillbutton.cpp
    widgets/gameScreen/eskillbutton.h
    widgets/gameScreen/eskillselectwidget.cpp
    widgets/gameScreen/eskillselectwidget.h
    widgets/gameScreen/eskilltreeswidget.cpp
    widgets/gameScreen/eskilltreeswidget.h
    widgets/gameScreen/eskilltreewidget.cpp
    widgets/gameScreen/eskilltreewidget.h
    widgets/gameScreen/estatswidget.cpp
    widgets/gameScreen/estatswidget.h
    widgets/gameScreen/eunitindicator.cpp
    widgets/gameScreen/eunitindicator.h
    widgets/gameScreen/eweaponswitch.cpp
    widgets/gameScreen/eweaponswitch.h

    # Widgets - Main Menu
    widgets/mainMenu/echoosecharacterwidget.cpp
    widgets/mainMenu/echoosecharacterwidget.h
    widgets/mainMenu/echaracterbutton.cpp
    widgets/mainMenu/echaracterbutton.h
    widgets/mainMenu/emainmenubutton.cpp
    widgets/mainMenu/emainmenubutton.h
)

include(GNUInstallDirs)
install(TARGETS eSlayerGame
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(eSlayerGame PRIVATE SDL3::SDL3)
target_link_libraries(eSlayerGame PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(eSlayerGame PRIVATE SDL3_image::SDL3_image)
target_link_libraries(eSlayerGame PRIVATE SDL3_mixer::SDL3_mixer)
target_include_directories(eSlayerGame PRIVATE ${rapidcsv_SOURCE_DIR}/src)

target_link_libraries(eSlayerGame PRIVATE eSlayerServer)
target_link_libraries(eSlayerGame PRIVATE eSlayerMapGenerator)
target_link_libraries(eSlayerGame PRIVATE eSlayerHelpers)
target_link_libraries(eSlayerGame PRIVATE eSlayerMissiles)
