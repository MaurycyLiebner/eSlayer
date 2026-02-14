cmake_minimum_required(VERSION 3.16)

project(eSlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Disable unnecessary SDL components (optional but recommended)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
)

FetchContent_MakeAvailable(SDL3_ttf)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.4.0
)

FetchContent_MakeAvailable(SDL3_image)

FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG prerelease-3.1.2
)

FetchContent_MakeAvailable(SDL3_mixer)

FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 11.0.0
)

FetchContent_MakeAvailable(tinyxml2)


add_executable(eSlayer main.cpp
    erand.cpp erand.h
    evectorhelpers.cpp evectorhelpers.h

    audio/emusic.cpp audio/emusic.h
    audio/emusicvector.cpp audio/emusicvector.h
    audio/esounds.cpp audio/esounds.h
    audio/esoundvector.cpp audio/esoundvector.h

    emainwindow.cpp emainwindow.h
    widgets/ewidget.cpp widgets/ewidget.h
    widgets/epainter.cpp widgets/epainter.h
    widgets/etexture.cpp widgets/etexture.h
    widgets/emouseevent.cpp widgets/emouseevent.h
    widgets/efontcolor.cpp widgets/efontcolor.h
    widgets/efonts.cpp widgets/efonts.h
    ewindowsettings.h ewindowsettings.cpp
    eloadtexthelper.cpp eloadtexthelper.h
    eresolution.cpp eresolution.h
    egamedir.cpp egamedir.h
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
    echaracter.h echaracter.cpp
    widgets/echeckbox.h widgets/echeckbox.cpp
    widgets/mainMenu/echaracterbutton.h widgets/mainMenu/echaracterbutton.cpp
    widgets/escrollwidget.h widgets/escrollwidget.cpp
    screens/esettingsmenu.h screens/esettingsmenu.cpp
    widgets/echeckbutton.h widgets/echeckbutton.cpp)

include(GNUInstallDirs)
install(TARGETS eSlayer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(eSlayer PRIVATE SDL3::SDL3)
target_link_libraries(eSlayer PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(eSlayer PRIVATE SDL3_image::SDL3_image)
target_link_libraries(eSlayer PRIVATE SDL3_mixer::SDL3_mixer)
target_link_libraries(eSlayer PRIVATE tinyxml2::tinyxml2)
