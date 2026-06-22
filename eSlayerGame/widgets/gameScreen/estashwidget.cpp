#include "estashwidget.h"

#include "einventorybagpackwidget.h"
#include "eitemplacewidget.h"
#include "ecoinswidget.h"
#include "einventorywidget.h"
#include "ecoinsquestionwidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eequipment.h>

eStashWidget* eStashWidget::sInstance = nullptr;

eStashWidget::eStashWidget(eMainWindow* const window) :
    eBgWidget(window) {
    sInstance = this;
}

eStashWidget::~eStashWidget() {
    sInstance = nullptr;
}

void eStashWidget::initialize(eEquipment& eq, const eStats& stats) {
    mEq = &eq;
    const auto& res = resolution();
    const int p = res.largePadding();

    const auto inner = new eWidget(window());
    inner->setNoPadding();

    mStash = new eInventoryBagpackWidget(window());
    mStash->initialize(eEquipment::fStashWidth,
                       eEquipment::fStashHeight,
                       eq.fStash,
                       eq, eBagpackType::inventory);
    inner->addWidget(mStash);

    mCoins = new eCoinsWidget(window());
    const auto action = [this, &eq]() {
        const auto q = new eCoinsQuestionWidget(window());
        const auto goldA = [&eq](const int count) {
            eq.fInventoryGold += count;
            eq.fStashGold -= count;
            eEquipmentAction a;
            a.fType = eEquipmentActionType::gold;
            a.fStashGold = eq.fStashGold;
            a.fInvGold = eq.fInventoryGold;
            eGameWidget::sSendEqAction(a);
        };
        q->initialize(goldA, 11, eq.fStashGold);
        addWidget(q);
        q->align(eAlignment::center);
    };
    mCoins->initialize(eq.fStashGold, action, 8);
    inner->addWidget(mCoins);

    inner->stackVertically(p);
    inner->fitContent();

    mStash->align(eAlignment::hcenter);
    mCoins->align(eAlignment::hcenter);

    setup(inner);
}

void eStashWidget::paintEvent(ePainter& p) {
    eBgWidget::paintEvent(p);
    mCoins->setCount(mEq->fStashGold);
}

bool eStashWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}

bool eStashWidget::dropItem() {
    if(eInventoryWidget::sBlocked) return false;
    const bool b = mStash->dropItem();
    if(b) return true;
    for(const auto w : mItemPlaces) {
        if(!w->visible()) continue;
        if(!w->hovered()) continue;
        const bool r = w->dropItem();
        if(r) return true;
    }
    return false;
}