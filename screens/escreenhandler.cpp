#include "escreenhandler.h"

#include "emainmenu.h"
#include "../emainwindow.h"
#include "ecreatecharactermenu.h"
#include "echoosecharactermenu.h"
#include "esettingsmenu.h"

eScreenHandler::eScreenHandler(eMainWindow * const window) :
    mWindow(window) {}

void eScreenHandler::showMainMenu() {
    const auto w = new eMainMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto singlePlayer = [this]() {
        loadCharacters();
        if(mCharacters.empty()) {
            showCreateCharacterMenu();
        } else {
            showChooseCharacterMenu();
        }
    };

    const auto settings = [this]() {
        showSettings();
    };

    const auto exitGame = [this]() {
        mWindow->quit();
    };
    w->initialize(singlePlayer,
                  settings,
                  exitGame);
    mWindow->setWidget(w);
}

void eScreenHandler::showCreateCharacterMenu() {
    const auto w = new eCreateCharacterMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto exit = [this]() {
        showChooseCharacterMenu();
    };

    const auto ok = [this](const std::string& name,
                           const bool hardcore) {
        const bool c = mCharacters.contains(name);
        if(c) return;
        mCharacters.add(name, hardcore);
        showGame(mCharacters.get(name));
    };

    w->initialize(exit, ok);
    mWindow->setWidget(w);
}

void eScreenHandler::showChooseCharacterMenu() {
    const auto w = new eChooseCharacterMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto exit = [this]() {
        showMainMenu();
    };

    const auto ok = [this](const std::string& name) {
        const bool c = mCharacters.contains(name);
        if(!c) return;
        showGame(mCharacters.get(name));
    };

    const auto createCharacter = [this]() {
        showCreateCharacterMenu();
    };

    const auto deleteCharacter = [this](const std::string& name) {
        mCharacters.remove(name);
        showChooseCharacterMenu();
    };

    w->initialize(exit, ok,
                  createCharacter,
                  deleteCharacter,
                  mCharacters);
    mWindow->setWidget(w);
}

void eScreenHandler::showGame(const eCharacter& c) {

}

void eScreenHandler::showSettings() {
    const auto ini = mWindow->settings();
    const auto w = new eSettingsMenu(ini, mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto applyAction = [this](const eWindowSettings& sett) {
        mWindow->setResolution(sett.fRes);
        showMainMenu();
    };

    const auto fullscreenA = [this](const bool fullscreen) {
        mWindow->setFullscreen(fullscreen);
        showSettings();
    };

    w->initialize(applyAction, fullscreenA);
    mWindow->setWidget(w);
}

void eScreenHandler::loadCharacters() {
    mCharacters.load();
}
