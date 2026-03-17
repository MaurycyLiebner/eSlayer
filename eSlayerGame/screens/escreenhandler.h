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
    void showSettings();
    void showLoadingScreen(const std::vector<eAction>& loading,
                           const eAction& finish);
    void showErrorMsg(const std::string& msg,
                      const std::string& subMsg);

private:
    void loadCharacters();

    eMainWindow* const mWindow;
    eCharacters mCharacters;
};

#endif // ESCREENHANDLER_H
