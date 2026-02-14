#ifndef EMAINMENU_H
#define EMAINMENU_H

#include "../widgets/elabel.h"

class eMainMenu : public eLabel {
public:
    eMainMenu(eMainWindow* const window);

    void initialize(const eAction& singlePlayer,
                    const eAction& settings,
                    const eAction& exitGame);
};

#endif // EMAINMENU_H
