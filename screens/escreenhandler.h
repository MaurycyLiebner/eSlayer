#ifndef ESCREENHANDLER_H
#define ESCREENHANDLER_H

#include "../echaracters.h"

class eMainWindow;

class eScreenHandler {
public:
    eScreenHandler(eMainWindow* const window);

    void showMainMenu();
    void showCreateCharacterMenu();
    void showChooseCharacterMenu();
    void showGame(const eCharacter& c);
    void showSettings();
private:
    void loadCharacters();

    eMainWindow* const mWindow;
    eCharacters mCharacters;
};

#endif // ESCREENHANDLER_H
