#include "etcpipgamemenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../elanguage.h"

#include <eSlayerNet/etcpnetwork.h>

void eTcpIpGameMenu::initialize(const eAction& hostGameA,
                                const eAction& joinGameA,
                                const eAction& exitA) {
    setExit(exitA);
    const auto res = resolution();

    const auto inner = eScreenBase::addInner();

    const auto w = new eWidget(window());

    const auto ipTextLabel = new eLabel(window());
    ipTextLabel->setFontColor(eFontColor::gray);
    ipTextLabel->setText(eLanguage::text(8, 3));
    ipTextLabel->fitContent();
    w->addWidget(ipTextLabel);

    const auto ipLabel = new eLabel(window());
    ipLabel->setFontColor(eFontColor::gray);
    const auto ip = eTCPNetwork::sGetActiveLanIP();
    ipLabel->setText(ip);
    ipLabel->fitContent();
    w->addWidget(ipLabel);

    const auto host = new eMainMenuButton(
        eLanguage::text(8, 0), window());
    host->setPressAction(hostGameA);
    w->addWidget(host);

    const auto join = new eMainMenuButton(
        eLanguage::text(8, 1), window());
    join->setPressAction(joinGameA);
    w->addWidget(join);

    const int p = res.hugePadding();
    w->stackVertically(p);
    w->fitContent();

    ipTextLabel->align(eAlignment::hcenter);
    ipLabel->align(eAlignment::hcenter);
    host->align(eAlignment::hcenter);
    join->align(eAlignment::hcenter);

    inner->addWidget(w);
    w->align(eAlignment::center);

    {
        const auto e = new eMainMenuButton(
            eLanguage::text(8, 2), window());
        inner->addWidget(e);
        e->setPressAction(exitA);
        e->align(eAlignment::bottom | eAlignment::left);
    }
}
