#include "echoosecharactermenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/mainMenu/echoosecharacterwidget.h"
#include "../widgets/escrollwidget.h"

#include "../elanguage.h"

eChooseCharacterMenu::eChooseCharacterMenu(eMainWindow* const window) :
    eScreenBase(window) {}

void eChooseCharacterMenu::initialize(
    const eAction& exit,
    const eOkAction& ok,
    const eAction& createCharacter,
    const eDeleteAction& deleteCharacter,
    const eCharacters& chars) {
    const auto res = resolution();

    const auto inner = eScreenBase::addInner();

    const auto e = new eMainMenuButton(
        eLanguage::text(3, 0), window());
    e->setPressAction(exit);
    inner->addWidget(e);
    e->align(eAlignment::bottom);

    const auto cw = new eWidget(window());
    cw->setNoPadding();

    const int buttHeight = height()/6;
    const auto sw = new eScrollWidget(window());
    sw->setScrollStep(buttHeight);

    const auto dc = new eMainMenuButton(
        eLanguage::text(3, 3), window());
    dc->setEnabled(false);
    const auto o = new eMainMenuButton(
        eLanguage::text(3, 1), window());
    o->setEnabled(false);

    const auto c = new eChooseCharacterWidget(window());

    const auto changedAction = [dc, o, c]() {
        const auto& current = c->current();
        const bool enabled = !current.empty();
        dc->setEnabled(enabled);
        o->setEnabled(enabled);
    };

    c->setNoPadding();
    c->resize(width()/2, height()/2);
    sw->resize(width()/2, height()/2);
    c->initialize(width()/2, buttHeight, chars, changedAction);
    cw->addWidget(sw);
    sw->setScrollArea(c);

    const auto cwb = new eWidget(window());
    cwb->setNoPadding();

    const auto cnc = new eMainMenuButton(
        eLanguage::text(3, 2), window());
    cnc->setPressAction(createCharacter);
    cwb->addWidget(cnc);

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
    inner->addWidget(cw);

    o->setPressAction([ok, c]() {
        const auto cc = c->current();
        if(cc.empty()) return;
        ok(cc);
    });
    inner->addWidget(o);
    o->align(eAlignment::bottom);

    inner->layoutHorizontallyWithoutSpaces();
}
