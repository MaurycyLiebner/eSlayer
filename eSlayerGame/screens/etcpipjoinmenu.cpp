#include "etcpipjoinmenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../elanguage.h"
#include "../widgets/elineedit.h"

void eTcpIpJoinMenu::initialize(const eJoinAction& joinGameA,
                                const eAction& exitA) {
    const auto res = resolution();

    const auto inner = eScreenBase::addInner();

    const auto w = new eWidget(window());

    const auto ipEdit = new eLineEdit(window());
    ipEdit->setMaxLength(62);
    ipEdit->grabKeyboard();
    ipEdit->allow('.');
    ipEdit->allow(':');
    ipEdit->allow('/');
    ipEdit->setText("127.0.0.1");
    w->addWidget(ipEdit);

    const int p = res.hugePadding();
    w->stackVertically(p);
    w->fitContent();

    inner->addWidget(w);
    w->align(eAlignment::center);

    {
        const auto e = new eMainMenuButton(
            eLanguage::text(8, 6), window());
        inner->addWidget(e);
        e->setPressAction(exitA);
        e->align(eAlignment::bottom | eAlignment::left);

        const auto j = new eMainMenuButton(
            eLanguage::text(8, 5), window());
        inner->addWidget(j);
        j->setPressAction([joinGameA, ipEdit]() {
            joinGameA(ipEdit->text());
        });
        j->align(eAlignment::bottom | eAlignment::right);
    }
}
