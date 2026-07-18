#include "ecreatecharactermenu.h"

#include "../widgets/mainMenu/emainmenubutton.h"
#include "../widgets/elineedit.h"
#include "../widgets/enamedcheckbox.h"
#include "../widgets/echeckbutton.h"
#include "../emainwindow.h"
#include "../names/eclassnames.h"

#include "../etext.h"

#include <eSlayerHelpers/eclasses.h>

eCreateCharacterMenu::eCreateCharacterMenu(eMainWindow * const window) :
    eScreenBase(window) {}

eCreateCharacterMenu::~eCreateCharacterMenu() {
    const auto window = eWidget::window();
    window->stopTextInput();
}

class eClassWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize() {
        setNoPadding();

        for(const auto& it : eClasses::sClasses) {
            const int id = it.fId;
            const auto name = eClassNames::name(id);
            const auto button = new eCheckButton(window());
            button->setText(name);
            button->fitContent();
            button->setCheckAction([this, id, button](const bool) {
                for(const auto b : mButtons) {
                    b->setChecked(false);
                }
                mClassId = id;
                button->setChecked(true);
            });
            mClassId = id;
            mButtons.emplace_back(button);
            addWidget(button);
        }
        if(!mButtons.empty()) {
            mButtons.back()->setChecked(true);
        }

        stackVertically();
        fitContent();
    }

    int classId() const { return mClassId; }
private:
    std::vector<eCheckButton*> mButtons;
    int mClassId = -1;
};

void eCreateCharacterMenu::initialize(
    const eAction& exit,
    const eOkAction& ok) {
    setExit(exit);
    const auto window = eWidget::window();
    const auto& res = resolution();

    const auto inner = eScreenBase::addInner();

    const auto bottomW = new eWidget(window);
    bottomW->setNoPadding();

    const auto classW = new eClassWidget(window);
    classW->initialize();

    const auto e = new eMainMenuButton(
        eText::text(2, 0), window);
    e->setPressAction(exit);
    bottomW->addWidget(e);

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
    bottomW->addWidget(nw);

    const auto o = new eMainMenuButton(
        eText::text(2, 1), window);
    o->setPressAction([ok, n, classW]() {
        const auto& name = n->text();
        const int classId = classW->classId();
        ok(classId, name, false);
    });
    bottomW->addWidget(o);
    o->setEnabled(false);

    n->setChangeAction([n, o]() {
        const auto text = n->text();
        o->setEnabled(!text.empty());
    });

    bottomW->setWidth(inner->width());
    bottomW->layoutHorizontallyWithoutSpaces();
    bottomW->fitHeight();

    inner->addWidget(classW);
    inner->addWidget(bottomW);
    inner->layoutVerticallyWithoutSpaces();
    classW->align(eAlignment::hcenter);
}
