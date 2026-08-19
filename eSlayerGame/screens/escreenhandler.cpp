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
#include "../widgets/mainMenu/edialog.h"
#include "../widgets/mainMenu/emainmenubutton.h"
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
#include "../names/etalktext.h"
#include "../names/equesttext.h"
#include "../names/emercenarynames.h"
#include "../names/edifficultynames.h"
#include "../etext.h"

#include <eSlayerHelpers/escreendimensions.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/ethreads.h>
#include <eSlayerHelpers/etalkheard.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eclasses.h>
#include <eSlayerHelpers/eunitsinfo.h>

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
            const eServerData serverData{
                "single_player", "", "", true};
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
            const int classId,
            const std::string& name,
            const bool hardcore) {
        const bool c = mCharacters.contains(name);
        if(c) return;
        auto s = serverData;
        s.fDifficulty = 0;
        mCharacters.add(classId, name, hardcore);
        showGame(s, mCharacters.get(name));
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

    const auto ok = [this, w, serverData](const std::string& name) {
        const bool r = mCharacters.contains(name);
        if(!r) return;
        const auto& c = mCharacters.get(name);
        const int maxDiff = c.latestDifficulty();
        if(serverData.fChooseDifficulty && maxDiff > 0) {
            const auto d = w->showDialog(
                eText::text(3, 7),
                nullptr, nullptr, nullptr);
            if(!d) return;
            const int width = w->dialogWidth();

            const auto buttonsW = new eWidget(mWindow);
            buttonsW->setNoPadding();

            for(const auto& it : eDifficulties::sDifficulties) {
                const int diff = it.fId;
                if(diff > maxDiff) break;
                const auto name = eDifficultyNames::name(diff);
                const auto b = new eMainMenuButton(name, mWindow);
                b->setWidth(width);
                b->fitHeight();
                buttonsW->addWidget(b);

                b->setPressAction([this, serverData, diff, c]() {
                    auto s = serverData;
                    s.fDifficulty = diff;
                    showGame(s, c);
                });
            }

            buttonsW->stackVertically();
            buttonsW->fitContent();

            d->addWidget(buttonsW);
            d->stackVertically();
            d->fitContent();
            d->align(eAlignment::center);
            buttonsW->align(eAlignment::hcenter);
        } else {
            auto s = serverData;
            s.fDifficulty = 0;
            showGame(s, c);
        }
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
        const eServerData serverData{
            "tcp_ip_host", ip, "", true};
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
        const eServerData serverData{
            "tcp_ip_join", ip, "", false};
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
    const auto loadById = [&](const uint16_t objType) {
        const bool h = eObjectsInfo::sObjects.has(objType);
        if(!h) {
            eRuntimeThrow("Missing object type.");
        }
        const auto& objInfo = eObjectsInfo::sObjects.get(objType);
        const auto objTexId = objInfo.fTexId;
        const bool hh = eObjsTextures::has(objTexId);
        if(hh) {
            auto& texs = eObjsTextures::get(objTexId);
            texs.load(res, r);
        }
    };
    for(const auto objType : objTypes) {
        loadById(objType);
    }

    const auto loadByName = [&](const std::string& name) {
        const auto id = eObjectsInfo::sObjects.id(name);
        loadById(id);
    };
    loadByName("waypoint");
    loadByName("portal");
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
    for(const auto unitType : unitTypes) {
        auto& u = eCharsTextures::get(unitType);
        u.loadAll(res, r);
    }
}

void eScreenHandler::showGame(eServerData serverData,
                              const eCharacter& c) {
    eMiniMap::clearAll();
    eSlayers::clear();
    eTeams::clear();
    eBodies::clear();
    eGameWidget::clearMessageLog();

    mGameStarted = true;
    const auto server = std::make_shared<std::shared_ptr<eServer>>();
    const auto map = std::make_shared<eMap>();
    const auto clientId = std::make_shared<uint32_t>();
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
    loading.emplace_back([this, server, clientId, serverC]() {
        *clientId = (*server)->connect();
        const int latest = serverC->latestDifficulty();
        if(eDifficulties::sDifficulty > latest) {
            (*server)->disconnect(*clientId);
            showErrorMsg("Disconnected", "You cannot enter a game with a difficulty you did not reach.");
        }
    });

    loading.emplace_back([server, serverC]() {
        const auto diff = eDifficulties::sDifficulty;
        const auto& quests = serverC->quests(diff);
        (*server)->setupGame(quests);
    });

    loading.emplace_back([this, server, map, clientId]() {
        eMapData data;

        eMoveToMapData moveData;
        moveData.fType = eMoveToMapType::spawn;
        moveData.fTo = eAreaIds(0, 0);

        const bool r = requestMap(**server, *clientId, moveData, data);
        if(r) map->loadData(data);
    });
    loading.emplace_back([&res, r, serverC]() {
        const int classId = serverC->classId();
        const auto& eq = serverC->equipment();
        const auto partsMap = eq.partsMap();
        const auto& class_ = eClasses::sClasses.get(classId);
        const auto uinfoId = class_.fUnitInfoId;
        const auto& udata = eUnitsInfo::sUnits.get(uinfoId);
        const auto& texs = eCharsTextures::get(udata.fCharData);
        const auto modelParts = texs.mapToModelParts(partsMap);
        texs.requestModel(modelParts, res, r, nullptr);
    });
    loading.emplace_back([&res, r]() {
        eMissilesTextures::load(res, r);
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
        (*server)->spawn(*clientId, *serverC, *teamId, spawnPos,
                         screenDims);
        map->setSpawnPos(spawnPos);
    });
    loading.emplace_back([&res, r]() {
        eMapTextures::load(res, r);
    });
    loading.emplace_back([]() {
        eRenderSettings::read();
    });
    loading.emplace_back([]() {
        eCharTextures::waitUntilAllLoaded();
    });
    showLoadingScreen(loading, finish);
}

void eScreenHandler::moveToMap(
    const uint32_t clientId,
    const eTeamId teamId,
    const eCharacter& c,
    const std::shared_ptr<eServer>& server,
    const eMoveToMapData& moveData) {
    const auto map = std::make_shared<eMap>();

    const auto& res = mWindow->resolution();

    const auto finish = [this, map, server, clientId, c, teamId]() {
        finishGameShow(map, server, clientId, c, teamId);
    };

    std::vector<eAction> loading;
    const auto r = mWindow->renderer();
    loading.emplace_back([this, server, map, moveData, clientId]() {
        eMapData data;
        const bool r = requestMap(*server, clientId, moveData, data);
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
        if(res != sett.fRes) {
            eCharsTextures::clear(true);
            if(mGameStarted) {
                mBlockGameStart = true;
            }
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
                eMercenaryNames::reload();
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

            loadings.emplace_back([&]() {
                eTalkText::reload();
            });

            loadings.emplace_back([&]() {
                eQuestText::reload();
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
    const uint32_t clientId,
    const eCharacter& c, const eTeamId teamId) {
    const int width = mWindow->width();
    const int height = mWindow->height();
    const auto w = new eGameScreen(mWindow);
    w->resize(width, height);
    w->setExitAction([this]() {
        showMainMenu();
    });
    const auto moveToMap = [this, server](const eMoveToMapData& moveData) {
        const auto gw = eGameWidget::sInstance;
        const uint32_t clientId = gw->clientId();
        const auto teamId = gw->team();
        const auto c = gw->character();
        eScreenHandler::moveToMap(clientId, teamId, c, server, moveData);
    };
    w->initialize(clientId, server, map, c, teamId, moveToMap);
    mWindow->setWidget(w);
}

bool eScreenHandler::requestMap(
    eServer& server,
    const uint32_t clientId,
    const eMoveToMapData& moveData,
    eMapData& data) {
    bool ready = false;
    const auto readyFunc = [&ready, &data](
        const eMapData& dataT) {
        data = dataT;
        ready = true;
    };
    const bool r = server.requestMapCall(clientId, moveData, readyFunc);
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
