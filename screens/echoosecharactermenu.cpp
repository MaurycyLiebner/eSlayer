#include "echoosecharactermenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/mainMenu/echoosecharacterwidget.h"
#include "../widgets/escrollwidget.h"

#include "../elanguage.h"

eChooseCharacterMenu::eChooseCharacterMenu(eMainWindow* const window) :
    eLabel(window) {}

void eChooseCharacterMenu::initialize(
    const eAction& exit,
    const eOkAction& ok,
    const eAction& createCharacter,
    const eDeleteAction& deleteCharacter,
    const eCharacters& chars) {
    const auto res = resolution();

    const auto w = new eWidget(window());

    const auto e = new eMainMenuButton(
        eLanguage::text(3, 0), window());
    e->setPressAction(exit);
    w->addWidget(e);

    const auto cw = new eWidget(window());
    cw->setNoPadding();

    const int buttHeight = height()/8;
    const auto sw = new eScrollWidget(window());
    sw->setScrollStep(buttHeight);

    const auto c = new eChooseCharacterWidget(window());
    c->setNoPadding();
    c->resize(width()/2, height()/2);
    sw->resize(width()/2, height()/2);
    c->initialize(width()/2, buttHeight, chars);
    cw->addWidget(sw);
    sw->setScrollArea(c);

    const auto cwb = new eWidget(window());
    cwb->setNoPadding();

    const auto cnc = new eMainMenuButton(
        eLanguage::text(3, 2), window());
    cnc->setPressAction(createCharacter);
    cwb->addWidget(cnc);

    const auto dc = new eMainMenuButton(
        eLanguage::text(3, 3), window());
    dc->setPressAction([deleteCharacter, c]() {
        const auto cc = c->current();
        if(cc.empty()) return;
        deleteCharacter(cc);
    });
    cwb->addWidget(dc);

    const int pp = res.hugePadding();
    cwb->stackHorizontally(pp);
    cwb->setWidth(c->width());
    cnc->align(eAlignment::left);
    dc->align(eAlignment::right);
    cwb->fitContent();

    cw->addWidget(cwb);

    cw->stackVertically(pp);
    cw->fitContent();
    cwb->align(eAlignment::hcenter);
    w->addWidget(cw);

    const auto o = new eMainMenuButton(
        eLanguage::text(3, 1), window());
    o->setPressAction([ok, c]() {
        const auto cc = c->current();
        if(cc.empty()) return;
        ok(cc);
    });
    w->addWidget(o);
    o->setEnabled(false);

    const int p = res.hugePadding();
    w->resize(width() - 2*p, height() - 2*p);
    w->layoutHorizontallyWithoutSpaces();

    e->align(eAlignment::bottom);
    o->align(eAlignment::bottom);

    addWidget(w);
    w->align(eAlignment::center);
}
