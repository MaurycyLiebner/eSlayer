#include "ecreatecharactermenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/elineedit.h"
#include "../widgets/enamedcheckbox.h"

#include "../elanguage.h"

eCreateCharacterMenu::eCreateCharacterMenu(eMainWindow * const window) :
    eLabel(window) {}

void eCreateCharacterMenu::initialize(
    const eAction& exit,
    const eOkAction& ok) {
    const auto res = resolution();

    const auto w = new eWidget(window());

    const auto e = new eMainMenuButton(
        eLanguage::text(2, 0), window());
    e->setPressAction(exit);
    w->addWidget(e);

    const auto nw = new eWidget(window());

    const auto nl = new eLabel(window());
    nl->setText(eLanguage::text(2, 2));
    nl->fitContent();
    nw->addWidget(nl);

    const auto n = new eLineEdit(window());
    n->setMaxLength(15);
    n->grabKeyboard();
    nw->addWidget(n);

    const auto h = new eNamedCheckBox(window());
    h->initialize(eLanguage::text(2, 3));
    nw->addWidget(h);

    const int pp = res.smallPadding();
    nw->stackVertically(pp);
    nw->fitContent();
    w->addWidget(nw);

    const auto o = new eMainMenuButton(
        eLanguage::text(2, 1), window());
    o->setPressAction([ok, n]() {
        ok(n->text(), false);
    });
    w->addWidget(o);
    o->setEnabled(false);

    n->setChangeAction([n, o]() {
        const auto text = n->text();
        o->setEnabled(!text.empty());
    });

    const int p = res.hugePadding();
    w->resize(width() - 2*p, height() - 2*p);
    w->layoutHorizontallyWithoutSpaces();

    e->align(eAlignment::bottom);
    nw->align(eAlignment::bottom);
    o->align(eAlignment::bottom);

    addWidget(w);
    w->align(eAlignment::center);
}
