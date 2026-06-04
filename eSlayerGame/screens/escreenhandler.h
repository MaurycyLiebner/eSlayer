#ifndef ESCREENHANDLER_H
#define ESCREENHANDLER_H

#include "../echaracters.h"

#include <eSlayerServer/eserver.h>

#include <functional>

class eMainWindow;

using eAction = std::function<void()>;

class eScreenHandler {
public:
    eScreenHandler(eMainWindow* const window);

    void showMainMenu();
    void showCreateOrChooseCharacterMenu(
        const eServerData& serverData);
    void showCreateCharacterMenu(
        eServerData serverData);
    void showChooseCharacterMenu(
        eServerData serverData);
    void showTcpIpGameMenu();
    void showTcpIpJoinMenu();
    void showGame(eServerData serverData,
                  const eCharacter& c);
    void moveToMap(const int clientId, const eTeamId teamId,
                   const eCharacter& c,
                   const std::shared_ptr<eServer>& server,
                   const std::string& mapName);
    void showSettings();
    void showLoadingScreen(const std::vector<eAction>& loading,
                           const eAction& finish);
    void showErrorMsg(const std::string& msg,
                      const std::string& subMsg);
private:
    void finishGameShow(const std::shared_ptr<eMap>& map,
                        const std::shared_ptr<eServer>& server,
                        const int clientId, const eCharacter& c,
                        const eTeamId teamId);
    bool requestMap(eServer& server,
                    const int clientId,
                    const std::string& mapName,
                    eMapData& data);
    void loadCharacters();

    eMainWindow* const mWindow;
    eCharacters mCharacters;
};

#endif // ESCREENHANDLER_H
