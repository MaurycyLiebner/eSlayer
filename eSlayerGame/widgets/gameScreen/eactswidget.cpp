#include "eactswidget.h"

#include "../elabel.h"
#include "../mainMenu/emainmenubutton.h"

#include <eSlayerHelpers/estringhelpers.h>

void eActsWidget::initialize(
    const std::string& title,
    const std::vector<eAct>& acts,
    const eAct& currentAct) {
    const auto& res = resolution();
    const int p = res.largePadding();

    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    const auto titleLabel = new eLabel(window());
    titleLabel->setNoPadding();
    titleLabel->setSmallFontSize();
    titleLabel->setText(title);
    titleLabel->fitContent();
    innerW->addWidget(titleLabel);

    const auto wW = new eWidget(window());
    wW->setNoPadding();

    mActs = acts;

    const auto actsW = new eWidget(window());
    actsW->setNoPadding();

    const auto setCurrent = [this](const eAct& act) {
        for(const auto& a : mActs) {
            const auto b = a.fButton;
            const auto w = a.fWidget;
            if(a == act) {
                w->show();
                b->setFontColor(eFontColor::white);
            } else {
                w->hide();
                b->setFontColor(eFontColor::gray);
            }
        }
    };

    for(auto& act : mActs) {
        const auto r = eStringHelpers::toRoman(act.fActId);
        const auto b = new eMainMenuButton(r, window());
        b->fitContent();
        actsW->addWidget(b);
        b->setPressAction([act, setCurrent]() {
            setCurrent(act);
        });

        wW->addWidget(act.fWidget);

        act.fButton = b;
    }
    wW->fitContent();
    setCurrent(currentAct);

    const int w = wW->width();
    const int nActs = mActs.size();
    const int bw = nActs > 0 ? (w - (nActs - 1)*p)/nActs : 0;
    for(const auto b : actsW->children()) {
        b->setWidth(bw);
    }
    actsW->stackHorizontally(p);
    actsW->fitContent();
    innerW->addWidget(actsW);

    innerW->addWidget(wW);

    innerW->stackVertically(p);
    innerW->fitContent();

    setup(innerW);
    titleLabel->align(eAlignment::hcenter);
}