#include "escreenhandler.h"

#include "emainmenu.h"
#include "../emainwindow.h"
#include "ecreatecharactermenu.h"
#include "echoosecharactermenu.h"
#include "esettingsmenu.h"
#include "eloadingscreen.h"
#include "egamescreen.h"

#include "../textures/eterrstextures.h"
#include "../textures/eeffectstextures.h"

#include <eSlayerServer/eserver.h>

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
    const auto server = std::make_shared<std::shared_ptr<eServer>>();
    const auto map = std::make_shared<std::shared_ptr<eMap>>();

    const auto finish = [this, map]() {
        const auto w = new eGameScreen(mWindow);
        const int width = mWindow->width();
        const int height = mWindow->height();
        w->resize(width, height);
        w->setExitAction([this]() {
            showMainMenu();
        });
        w->initialize(*map);
        mWindow->setWidget(w);
    };

    std::vector<eAction> loading;
    const auto r = mWindow->renderer();
    loading.emplace_back([server]() {
        *server = eSlayerServer::generate("single_player");
    });
    loading.emplace_back([server, map]() {
        *map = (*server)->requestMap("town");
    });
    loading.emplace_back([r]() {
        const auto lighting = eEffectsTextures::get("lighting");
        lighting->load(r);
    });
    loading.emplace_back([r, map]() {
        const auto& terrTypes = (*map)->terrainTypes();
        for(const auto& terrType : terrTypes) {
            const auto texs = eTerrsTextures::get(terrType.fName);
            texs->load(r);
        }
    });
    showLoadingScreen(loading, finish);
}

void eScreenHandler::showSettings() {
    const auto ini = mWindow->settings();
    const auto w = new eSettingsMenu(ini, mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto exitAction = [this]() {
        showMainMenu();
    };

    const auto applyAction = [this](const eWindowSettings& sett) {
        mWindow->setResolution(sett.fRes);
        sett.write();
        showMainMenu();
    };

    const auto fullscreenA = [this](const bool fullscreen) {
        mWindow->setFullscreen(fullscreen);
        showSettings();
    };

    w->initialize(exitAction, applyAction, fullscreenA);
    mWindow->setWidget(w);
}

void eScreenHandler::showLoadingScreen(
    const std::vector<eAction>& loading,
    const eAction& finish) {
    const auto w = new eLoadingScreen(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);
    w->initialize(loading, finish);
    mWindow->setWidget(w);
}

void eScreenHandler::loadCharacters() {
    mCharacters.load();
}
