#ifndef EMAINMENU_H
#define EMAINMENU_H

#include "escreenbase.h"

class eMainMenu : public eScreenBase {
public:
    eMainMenu(eMainWindow* const window);

    void initialize(const eAction& singlePlayer,
                    const eAction& tcpIpGame,
                    const eAction& settings,
                    const eAction& exitGame);
};

#endif // EMAINMENU_H
