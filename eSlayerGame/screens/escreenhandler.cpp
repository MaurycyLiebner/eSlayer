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
#include "../widgets/gameScreen/eminimap.h"
#include "../erendersettings.h"

#include "../textures/eterrstextures.h"
#include "../textures/eobjstextures.h"
#include "../textures/emissilestextures.h"
#include "../textures/euitextures.h"
#include "../textures/echarstextures.h"
#include "../textures/emaptextures.h"
#include "../textures/eitemstextures.h"

#include "../names/eitemnames.h"
#include "../names/emonsternames.h"
#include "../names/eskillnames.h"
#include "../names/eskilltreenames.h"
#include "../names/eclassnames.h"
#include "../names/eareanames.h"
#include "../names/eobjectnames.h"
#include "../names/eelitemodifiersnames.h"
#include "../names/eservernames.h"
#include "../names/elanguagenames.h"
#include "../etext.h"

#include <eSlayerHelpers/escreendimensions.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/ethreads.h>

#include <eSlayerNet/etcpnetwork.h>

eScreenHandler::eScreenHandler(eMainWindow * const window) :
    mWindow(window) {}

void eScreenHandler::showMainMenu() {
    const auto w = new eMainMenu(mWindow);
    const int width = mWindow->width();
    const int height = mWindow->height();
    w->resize(width, height);

    const auto showBlockDialog = [w]() {
        const auto okAction = [w]() {
            w->closeDialog();
        };
        w->showDialog(eText::text(1, 4),
                      nullptr, nullptr,
                      okAction);
    };

    const auto singlePlayer = [this, showBlockDialog]() {
        if(mBlockGameStart) {
            showBlockDialog();
        } else {
            const eServerData serverData{"single_player", ""};
            showCreateOrChooseCharacterMenu(serverData);
        }
    };

    const auto tcpIpGame = [this, showBlockDialog]() {
        if(mBlockGameStart) {
            showBlockDialog();
        } else {
            showTcpIpGameMenu();
        }
    };

    const auto settings = [this, w]() {
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

void loadObjectTypes(const eResolution& res,
                     SDL_Renderer* const r,
                     const std::shared_ptr<eMap>& map) {
    const auto& objTypes = map->objectTypes();
    for(const auto& objType : objTypes) {
        const auto& objInfo = eObjectsInfo::sObjects.get(objType);
        const auto objTexId = objInfo.fTexId;
        auto& texs = eObjsTextures::get(objTexId);
        texs.load(res, r);
    }
}

void loadTerrainTypes(const eResolution& res,
                      SDL_Renderer* const r,
                      const std::shared_ptr<eMap>& map) {
    const auto& terrTypes = map->terrainTypes();
    for(const auto& terrType : terrTypes) {
        auto& texs = eTerrsTextures::get(terrType);
        texs.load(res, r);
    }
}

void loadUnitTypes(const eResolution& res,
                   SDL_Renderer* const r,
                   const std::shared_ptr<eMap>& map) {
    const auto& unitTypes = map->unitTypes();
    for(const auto& unitType : unitTypes) {
        auto& u = eCharsTextures::get(unitType);
        u.loadAll(res, r);
    }
}

void eScreenHandler::showGame(eServerData serverData,
                              const eCharacter& c) {
    eMiniMap::clearAll();
    eGameWidget::sClearAll();
    eTeams::clear();

    mGameStarted = true;
    const auto server = std::make_shared<std::shared_ptr<eServer>>();
    const auto mapName = "basement_1"/*"act1_1"*/;
    const uint8_t mapId = eMapsSettings::sMaps.id(mapName);
    const auto map = std::make_shared<eMap>(mapId);
    const auto clientId = std::make_shared<int>();
    const auto teamId = std::make_shared<eTeamId>();
    const auto serverC = std::make_shared<eCharacter>(c);

    const auto& res = mWindow->resolution();
    const int width = mWindow->width();
    const int height = mWindow->height();

    const auto finish = [this, map, server, clientId, serverC, teamId]() {
        finishGameShow(map, *server, *clientId, *serverC, *teamId);
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
    loading.emplace_back([this, server, mapId, map, clientId]() {
        eMapData data;
        const bool r = requestMap(**server, *clientId, mapId, data);
        if(r) map->loadData(data);
    });
    loading.emplace_back([&res, r]() {
        eMissilesTextures::loadTextures(res, r);
    });
    loading.emplace_back([&res, r, map]() {
        loadTerrainTypes(res, r, map);
    });
    loading.emplace_back([&res, r, map]() {
        loadObjectTypes(res, r, map);
    });
    loading.emplace_back([&res, r, map]() {
        loadUnitTypes(res, r, map);
    });
    loading.emplace_back([&res, r]() {
        eUITextures::sLoad(r, res);
    });
    loading.emplace_back([&res, r]() {
        eItemsTextures::loadTextures(r, res);
    });
    loading.emplace_back([&res, width, height,
                          server, clientId,
                          serverC, teamId,
                          map]() {
        const int tileW = res.tileWidth();
        const int tileH = res.tileHeight();
        const eScreenDimensions screenDims{int(std::ceil(1.f*width/tileW)),
                                           int(std::ceil(2.f*height/tileH))};
        ePointF spawnPos;
        (*server)->spawn(*clientId, *serverC, *teamId, spawnPos, screenDims);
        map->setSpawnPos(spawnPos);
    });
    loading.emplace_back([&res, r]() {
        eMapTextures::load(res, r);
    });
    loading.emplace_back([]() {
        eRenderSettings::read();
    });
    showLoadingScreen(loading, finish);
}

void eScreenHandler::moveToMap(
    const int clientId,
    const eTeamId teamId,
    const eCharacter& c,
    const std::shared_ptr<eServer>& server,
    const uint8_t mapId) {
    const auto map = std::make_shared<eMap>(mapId);

    const auto& res = mWindow->resolution();

    const auto finish = [this, map, server, clientId, c, teamId]() {
        finishGameShow(map, server, clientId, c, teamId);
    };

    std::vector<eAction> loading;
    const auto r = mWindow->renderer();
    loading.emplace_back([this, server, map, mapId, clientId]() {
        eMapData data;
        const bool r = requestMap(*server, clientId, mapId, data);
        if(r) map->loadData(data);
    });
    loading.emplace_back([&res, r, map]() {
        loadTerrainTypes(res, r, map);
    });
    loading.emplace_back([&res, r, map]() {
        loadObjectTypes(res, r, map);
    });
    loading.emplace_back([&res, r, map]() {
        loadUnitTypes(res, r, map);
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
        mWindow->setFullscreen(sett.fFullscreen);
        const auto& res = mWindow->resolution();
        if(mGameStarted && res != sett.fRes) {
            mBlockGameStart = true;
        }
        mWindow->setResolution(sett.fRes);
        const auto& oldL = eLanguage::sLanguage.fName;
        const auto& newL = sett.fLanguage.fName;
        const bool languageChanged = oldL != newL;
        if(languageChanged) {
            eLanguage::sLanguage = sett.fLanguage;
        }

        eThreads::sThreads = sett.fThreads;

        sett.write();
        if(languageChanged) {
            std::vector<eAction> loadings;

            loadings.emplace_back([&]() {
                eLanguageNames::reload();
            });

            loadings.emplace_back([&]() {
                eText::reload();
            });

            loadings.emplace_back([&]() {
                eSkillTreeNames::reload();
            });

            loadings.emplace_back([&]() {
                eClassNames::reload();
            });

            loadings.emplace_back([&]() {
                eItemNames::reload();
            });

            loadings.emplace_back([&]() {
                eAreaNames::reload();
            });

            loadings.emplace_back([&]() {
                eMonsterNames::reload();
            });

            loadings.emplace_back([&]() {
                eSkillNames::reload();
            });

            loadings.emplace_back([&]() {
                eObjectNames::reload();
            });

            loadings.emplace_back([&]() {
                eEliteModifiersNames::reload();
            });

            loadings.emplace_back([&]() {
                eServerNames::reload();
            });

            const auto finish = [this]() {
                showMainMenu();
            };

            showLoadingScreen(loadings, finish);
        } else {
            showMainMenu();
        }
    };

    w->initialize(exitAction, applyAction);
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

void eScreenHandler::finishGameShow(
    const std::shared_ptr<eMap>& map,
    const std::shared_ptr<eServer>& server,
    const int clientId,
    const eCharacter& c, const eTeamId teamId) {
    const int width = mWindow->width();
    const int height = mWindow->height();
    const auto w = new eGameScreen(mWindow);
    w->resize(width, height);
    w->setExitAction([this]() {
        showMainMenu();
    });
    const auto moveToMap = [this, server](const uint8_t mapId) {
        const auto gw = eGameWidget::sInstance;
        const int clientId = gw->clientId();
        const auto teamId = gw->team();
        const auto c = gw->character();
        eScreenHandler::moveToMap(clientId, teamId, c, server, mapId);
    };
    w->initialize(clientId, server, map, c, teamId, moveToMap);
    mWindow->setWidget(w);
}

bool eScreenHandler::requestMap(
    eServer& server,
    const int clientId,
    const uint8_t mapId,
    eMapData& data) {
    bool ready = false;
    const auto readyFunc = [&ready, &data](
        const eMapData& dataT) {
        data = dataT;
        ready = true;
    };
    const bool r = server.requestMapCall(clientId, mapId, readyFunc);
    if(!r) showErrorMsg("Disconnected", "Failed to retrieve the map.");
    uint32_t time = 0;
    while(!ready) {
        SDL_Delay(32);
        time += 32;
        server.checkMapsReady();
        if(time > 10000) {
            showErrorMsg("Disconnected", "Map request timed out.");
            return false;
        }
    }
    return true;
}

void eScreenHandler::loadCharacters() {
    mCharacters.load();
}
