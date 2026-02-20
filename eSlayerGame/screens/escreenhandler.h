#ifndef ESCREENHANDLER_H
#define ESCREENHANDLER_H

#include "../echaracters.h"

#include <functional>

class eMainWindow;

using eAction = std::function<void()>;

class eScreenHandler {
public:
    eScreenHandler(eMainWindow* const window);

    void showMainMenu();
    void showCreateCharacterMenu();
    void showChooseCharacterMenu();
    void showGame(const eCharacter& c);
    void showSettings();
    void showLoadingScreen(const std::vector<eAction>& loading,
                           const eAction& finish);
private:
    void loadCharacters();

    eMainWindow* const mWindow;
    eCharacters mCharacters;
};

#endif // ESCREENHANDLER_H
