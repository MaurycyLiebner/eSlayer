#include "ecreatecharactermenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/elineedit.h"
#include "../widgets/enamedcheckbox.h"
#include "../emainwindow.h"

#include "../etext.h"

eCreateCharacterMenu::eCreateCharacterMenu(eMainWindow * const window) :
    eScreenBase(window) {}

eCreateCharacterMenu::~eCreateCharacterMenu() {
    const auto window = eWidget::window();
    window->stopTextInput();
}

void eCreateCharacterMenu::initialize(
    const eAction& exit,
    const eOkAction& ok) {
    setExit(exit);
    const auto window = eWidget::window();
    const auto& res = resolution();

    const auto inner = eScreenBase::addInner();

    const auto e = new eMainMenuButton(
        eText::text(2, 0), window);
    e->setPressAction(exit);
    inner->addWidget(e);

    const auto nw = new eWidget(window);

    const auto nl = new eLabel(window);
    nl->setText(eText::text(2, 2));
    nl->fitContent();
    nw->addWidget(nl);

    const auto n = new eLineEdit(window);
    n->setMaxLengthAndFit(15);
    n->grabKeyboard();
    nw->addWidget(n);
    window->startTextInput();

    const auto h = new eNamedCheckBox(window);
    h->initialize(eText::text(2, 3));
    nw->addWidget(h);

    const int pp = res.smallPadding();
    nw->stackVertically(pp);
    nw->fitContent();
    inner->addWidget(nw);

    const auto o = new eMainMenuButton(
        eText::text(2, 1), window);
    o->setPressAction([ok, n]() {
        ok(n->text(), false);
    });
    inner->addWidget(o);
    o->setEnabled(false);

    n->setChangeAction([n, o]() {
        const auto text = n->text();
        o->setEnabled(!text.empty());
    });

    inner->layoutHorizontallyWithoutSpaces();

    e->align(eAlignment::bottom);
    nw->align(eAlignment::bottom);
    o->align(eAlignment::bottom);
}
