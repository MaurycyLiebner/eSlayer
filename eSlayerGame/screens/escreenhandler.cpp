#include "escreenhandler.h"

#include "emainmenu.h"
#include "../emainwindow.h"
#include "ecreatecharactermenu.h"
#include "echoosecharactermenu.h"
#include "esettingsmenu.h"
#include "eloadingscreen.h"
#include "egamescreen.h"
#include "eerrorscreen.h"
#include "etcpipgamemenu.h"
#include "etcpipjoinmenu.h"
#include "../widgets/gameScreen/egamewidget.h"

#include "../textures/eterrstextures.h"
#include "../textures/eobjstextures.h"
#include "../textures/eeffectstextures.h"
#include "../textures/emissilestextures.h"
#include "../textures/euitextures.h"
#include "../textures/echarstextures.h"

#include <eSlayerHelpers/escreendimensions.h>

#include <eSlayerNet/etcpnetwork.h>

eScreenHandler::eScreenHandler(eMainWindow * const window) :
    mWindow(window) {}

void eScreenHandler::showMainMenu() {
    const auto w = new eMainMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto singlePlayer = [this]() {
        const eServerData serverData{"single_player", ""};
        showCreateOrChooseCharacterMenu(serverData);
    };

    const auto tcpIpGame = [this]() {
        showTcpIpGameMenu();
    };

    const auto settings = [this]() {
        showSettings();
    };

    const auto exitGame = [this]() {
        mWindow->quit();
    };
    w->initialize(singlePlayer,
                  tcpIpGame,
                  settings,
                  exitGame);
    mWindow->setWidget(w);
}

void eScreenHandler::showCreateOrChooseCharacterMenu(
    const eServerData& serverData) {
    loadCharacters();
    if(mCharacters.empty()) {
        showCreateCharacterMenu(serverData);
    } else {
        showChooseCharacterMenu(serverData);
    }
}

void eScreenHandler::showCreateCharacterMenu(eServerData serverData) {
    const auto w = new eCreateCharacterMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto exit = [this, serverData]() {
        showChooseCharacterMenu(serverData);
    };

    const auto ok = [this, serverData](
                        const std::string& name,
                        const bool hardcore) {
        const bool c = mCharacters.contains(name);
        if(c) return;
        mCharacters.add(name, hardcore);
        showGame(serverData, mCharacters.get(name));
    };

    w->initialize(exit, ok);
    mWindow->setWidget(w);
}

void eScreenHandler::showChooseCharacterMenu(eServerData serverData) {
    const auto w = new eChooseCharacterMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto exit = [this]() {
        showMainMenu();
    };

    const auto ok = [this, serverData](const std::string& name) {
        const bool c = mCharacters.contains(name);
        if(!c) return;
        showGame(serverData, mCharacters.get(name));
    };

    const auto createCharacter = [this, serverData]() {
        showCreateCharacterMenu(serverData);
    };

    const auto deleteCharacter = [this, serverData](const std::string& name) {
        mCharacters.remove(name);
        showChooseCharacterMenu(serverData);
    };

    w->initialize(exit, ok,
                  createCharacter,
                  deleteCharacter,
                  mCharacters);
    mWindow->setWidget(w);
}

void eScreenHandler::showTcpIpGameMenu() {
    const auto w = new eTcpIpGameMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto host = [this]() {
        const auto ip = eTCPNetwork::sGetActiveLanIP();
        const eServerData serverData{"tcp_ip_host", ip};
        showCreateOrChooseCharacterMenu(serverData);
    };

    const auto join = [this]() {
        showTcpIpJoinMenu();
    };

    const auto exit = [this]() {
        showMainMenu();
    };

    w->initialize(host, join, exit);
    mWindow->setWidget(w);
}

void eScreenHandler::showTcpIpJoinMenu() {
    const auto w = new eTcpIpJoinMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto join = [this](const std::string& ip) {
        const eServerData serverData{"tcp_ip_join", ip};
        showCreateOrChooseCharacterMenu(serverData);
    };

    const auto exit = [this]() {
        showTcpIpGameMenu();
    };

    w->initialize(join, exit);
    mWindow->setWidget(w);
}

void eScreenHandler::showGame(eServerData serverData,
                              const eCharacter& c) {
    const auto server = std::make_shared<std::shared_ptr<eServer>>();
    const auto map = std::make_shared<eMap>();
    const auto clientId = std::make_shared<int>();
    const auto serverC = std::make_shared<eCharacter>(c);

    const auto& res = mWindow->resolution();
    const int width = mWindow->width();
    const int height = mWindow->height();

    const auto finish = [this, width, height,
                         map, server, clientId, serverC]() {
        const auto w = new eGameScreen(mWindow);
        w->resize(width, height);
        w->setExitAction([this]() {
            showMainMenu();
        });
        w->initialize(*clientId, *server, map, *serverC);
        mWindow->setWidget(w);
    };

    std::vector<eAction> loading;
    const auto r = mWindow->renderer();
    loading.emplace_back([this, server, serverData]() {
        *server = eSlayerServer::generate(serverData);
        (*server)->setFailureHandler([this](const std::string& msg,
                                            const std::string& subMsg) {
            const auto gw = eGameWidget::sInstance;
            if(gw) gw->save();
            showErrorMsg(msg, subMsg);
        });
        (*server)->initialize();
    });
    loading.emplace_back([server, clientId]() {
        *clientId = (*server)->connect();
    });
    loading.emplace_back([this, server, map, clientId]() {
        eMapData data;
        const bool r = (*server)->requestMap(*clientId, "town", data);
        if(!r) showErrorMsg("Disconnected", "Failed to retrieve the map.");
        else map->loadData(data);
    });
    loading.emplace_back([&res, r]() {
        const int id = eEffectsTextures::sEffects.id("lighting");
        auto& lighting = eEffectsTextures::sEffects.get(id);
        lighting.load(res, r);
    });
    loading.emplace_back([&res, r]() {
        eMissilesTextures::loadTextures(res, r);
    });
    loading.emplace_back([&res, r, map]() {
        const int w = res.tileWidth();
        const int h = res.tileHeight();
        const auto& terrTypes = map->terrainTypes();
        for(const auto& terrType : terrTypes) {
            auto& texs = eTerrsTextures::get(terrType);
            texs.loadFixedSize(w, h, res, r);
        }
    });
    loading.emplace_back([&res, r, map]() {
        const auto& objTypes = map->objectTypes();
        for(const auto& objType : objTypes) {
            auto& texs = eObjsTextures::get(objType);
            texs.load(res, r);
        }
    });
    loading.emplace_back([&res, r, map]() {
        const auto& unitTypes = map->unitTypes();
        for(const auto& unitType : unitTypes) {
            auto& u = eCharsTextures::get(unitType);
            u.loadAll(res, r);
        }
    });
    loading.emplace_back([&res, r]() {
        eUITextures::sLoad(r, res);
    });
    loading.emplace_back([&res, width, height,
                          server, clientId, serverC]() {
        const int tileW = res.tileWidth();
        const int tileH = res.tileHeight();
        const eScreenDimensions screenDims{int(std::ceil(1.f*width/tileW)),
                                           int(std::ceil(2.f*height/tileH))};
        (*server)->spawn(*clientId, *serverC, screenDims);
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

void eScreenHandler::showErrorMsg(const std::string& msg,
                                  const std::string& subMsg) {
    const auto w = new eErrorScreen(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);
    w->initialize(msg, subMsg, [this]() {
        showMainMenu();
    });
    mWindow->setWidget(w);
}

void eScreenHandler::loadCharacters() {
    mCharacters.load();
}
