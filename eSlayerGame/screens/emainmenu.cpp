#include "emainmenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"

#include "../elanguage.h"

eMainMenu::eMainMenu(eMainWindow * const window) :
    eScreenBase(window) {}

void eMainMenu::initialize(const eAction& singlePlayer,
                           const eAction& tcpIpGame,
                           const eAction& settings,
                           const eAction& exitGame) {
    const auto res = resolution();

    const auto inner = eScreenBase::addInner();

    const auto w = new eWidget(window());

    const auto sp = new eMainMenuButton(
        eLanguage::text(1, 0), window());
    sp->setPressAction(singlePlayer);
    w->addWidget(sp);

    const auto tcpIp = new eMainMenuButton(
        eLanguage::text(1, 1), window());
    tcpIp->setPressAction(tcpIpGame);
    w->addWidget(tcpIp);

    const auto s = new eMainMenuButton(
        eLanguage::text(1, 2), window());
    s->setPressAction(settings);
    w->addWidget(s);

    const auto eg = new eMainMenuButton(
        eLanguage::text(1, 3), window());
    eg->setPressAction(exitGame);
    w->addWidget(eg);

    const int p = res.hugePadding();
    w->stackVertically(p);
    w->fitContent();

    sp->align(eAlignment::hcenter);
    tcpIp->align(eAlignment::hcenter);
    s->align(eAlignment::hcenter);
    eg->align(eAlignment::hcenter);

    inner->addWidget(w);
    w->align(eAlignment::center);
}
