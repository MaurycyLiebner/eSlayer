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

add_executable(eSlayerGame main.cpp


    audio/emusic.cpp audio/emusic.h
    audio/emusicvector.cpp audio/emusicvector.h
    audio/esounds.cpp audio/esounds.h
    audio/esoundvector.cpp audio/esoundvector.h

    emainwindow.cpp emainwindow.h
    widgets/ewidget.cpp widgets/ewidget.h
    widgets/epainter.cpp widgets/epainter.h
    textures/etexture.cpp textures/etexture.h
    widgets/emouseevent.cpp widgets/emouseevent.h
    widgets/efontcolor.cpp widgets/efontcolor.h
    widgets/efonts.cpp widgets/efonts.h
    ewindowsettings.h ewindowsettings.cpp
    eloadtexthelper.cpp eloadtexthelper.h
    eresolution.cpp eresolution.h

    widgets/elabel.cpp widgets/elabel.h
    widgets/elabelbase.cpp widgets/elabelbase.h
    widgets/ebuttonbase.cpp widgets/ebuttonbase.h
    screens/emainmenu.h screens/emainmenu.cpp
    screens/escreenhandler.h screens/escreenhandler.cpp
    elanguage.h elanguage.cpp
    widgets/mainMenu/emainmenubutton.h widgets/mainMenu/emainmenubutton.cpp
    screens/ecreatecharactermenu.h screens/ecreatecharactermenu.cpp
    widgets/elineedit.h widgets/elineedit.cpp
    widgets/echeckablebutton.h widgets/echeckablebutton.cpp
    widgets/enamedcheckbox.h widgets/enamedcheckbox.cpp
    screens/echoosecharactermenu.h screens/echoosecharactermenu.cpp
    widgets/mainMenu/echoosecharacterwidget.h widgets/mainMenu/echoosecharacterwidget.cpp
    echaracters.h echaracters.cpp

    widgets/echeckbox.h widgets/echeckbox.cpp
    widgets/mainMenu/echaracterbutton.h widgets/mainMenu/echaracterbutton.cpp
    widgets/escrollwidget.h widgets/escrollwidget.cpp
    screens/esettingsmenu.h screens/esettingsmenu.cpp
    widgets/echeckbutton.h widgets/echeckbutton.cpp
    screens/escreenbase.h screens/escreenbase.cpp

    textures/etexturecollection.cpp textures/etexturecollection.h
    textures/espriteloader.h textures/espriteloader.cpp
    textures/echartextures.h textures/echartextures.cpp
    textures/echarmodel.h textures/echarmodel.cpp
    textures/echarstextures.h textures/echarstextures.cpp
    textures/echarunitmodel.h textures/echarunitmodel.cpp

    efileloader.h efileloader.cpp
    audio/eaudiovector.h audio/eaudiovector.cpp
    widgets/eprogressbar.h widgets/eprogressbar.cpp
    screens/eloadingscreen.h screens/eloadingscreen.cpp
    textures/eterrstextures.h textures/eterrstextures.cpp

    screens/egamescreen.h screens/egamescreen.cpp
    egameworld.h egameworld.cpp
    egameinput.h egameinput.cpp
    textures/elightingtexture.h textures/elightingtexture.cpp
    textures/eeffectstextures.h textures/eeffectstextures.cpp
    textures/eeffecttextures.h textures/eeffecttextures.cpp
    textures/erendertargetholder.h textures/erendertargetholder.cpp
    textures/etextgenerator.h textures/etextgenerator.cpp
    widgets/gameScreen/eescmenu.h widgets/gameScreen/eescmenu.cpp
    widgets/gameScreen/eescmenubutton.h widgets/gameScreen/eescmenubutton.cpp
    textures/eobjstextures.h textures/eobjstextures.cpp

    textures/etiletextures.h textures/etiletextures.cpp
    textures/etilestextures.h textures/etilestextures.cpp
    textures/etilesiterator.h textures/etilesiterator.cpp
    units/eunit.h units/eunit.cpp
    widgets/gameScreen/egamepainter.h widgets/gameScreen/egamepainter.cpp
    widgets/gameScreen/egamewidget.h widgets/gameScreen/egamewidget.cpp
    widgets/gameScreen/eunitindicator.h widgets/gameScreen/eunitindicator.cpp

    textures/euitextures.h textures/euitextures.cpp
    widgets/ecolors.h widgets/ecolors.cpp
    emaincharaction.h emaincharaction.cpp
    widgets/gameScreen/ehealthindicator.h widgets/gameScreen/ehealthindicator.cpp
    widgets/gameScreen/eplayerhealthindicator.h widgets/gameScreen/eplayerhealthindicator.cpp
    screens/etcpipgamemenu.h screens/etcpipgamemenu.cpp
    screens/etcpipjoinmenu.h screens/etcpipjoinmenu.cpp
    screens/eerrorscreen.h screens/eerrorscreen.cpp
    textures/emissilestextures.h textures/emissilestextures.cpp
    widgets/gameScreen/eskillbutton.h widgets/gameScreen/eskillbutton.cpp
    widgets/gameScreen/eskillselectwidget.h widgets/gameScreen/eskillselectwidget.cpp
    widgets/etexturecheckbutton.h widgets/etexturecheckbutton.cpp
    textures/eitemstextures.h textures/eitemstextures.cpp
    widgets/gameScreen/einventorywidget.h widgets/gameScreen/einventorywidget.cpp
    widgets/gameScreen/einventorybagpackwidget.h widgets/gameScreen/einventorybagpackwidget.cpp
    widgets/gameScreen/eitemdragwidget.h widgets/gameScreen/eitemdragwidget.cpp
    widgets/gameScreen/eitemplacewidget.h widgets/gameScreen/eitemplacewidget.cpp
    widgets/gameScreen/eweaponswitch.h widgets/gameScreen/eweaponswitch.cpp
    widgets/gameScreen/egrounditemnames.h widgets/gameScreen/egrounditemnames.cpp
    names/eitemnames.h names/eitemnames.cpp
    names/emonsternames.h names/emonsternames.cpp
    widgets/gameScreen/estatswidget.h widgets/gameScreen/estatswidget.cpp
    names/eskillnames.h names/eskillnames.cpp
    widgets/gameScreen/eskilltreeswidget.h widgets/gameScreen/eskilltreeswidget.cpp
    widgets/gameScreen/eskilltreewidget.h widgets/gameScreen/eskilltreewidget.cpp
    names/eskilltreenames.h names/eskilltreenames.cpp
    names/eclassnames.h names/eclassnames.cpp

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
