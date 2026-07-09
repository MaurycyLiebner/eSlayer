#include "esellerwidget.h"

#include "../../etext.h"
#include "../mainMenu/emainmenubutton.h"
#include "einventorywidget.h"
#include "egamewidget.h"
#include "ehoverwidget.h"
#include "ecoinswidget.h"

#include <eSlayerHelpers/estringhelpers.h>

eSellerWidget* eSellerWidget::sInstance = nullptr;

eSellerWidget::eSellerWidget(
    eMainWindow* const w) :
    eBgWidget(w) {
    sInstance = this;
}

eSellerWidget::~eSellerWidget() {
    sInstance = nullptr;
}

void eSellerWidget::initialize(
    const uint32_t clientId,
    const eSeller& s,
    eEquipment& eq,
    const eStats& stats) {
    mClientId = clientId;
    mEq = &eq;
    mSeller = s;

    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    const auto tabsWidget = new eWidget(window());
    tabsWidget->setNoPadding();

    const auto textBase = eText::text(6, 14);
    int i = 0;
    const auto addTab = [&](eInventoryItems& items) {
        const auto i1Str = std::to_string(i + 1);
        const auto text = eStringHelpers::replaceAll(
            textBase, "%1", i1Str);
        const auto b = new eMainMenuButton(text, window());
        b->setSmallFontSize();
        b->setSmallPadding();
        b->fitContent();
        tabsWidget->addWidget(b);
        b->setPressAction([this, i, &items, &stats, &eq]() {
            mBag->initialize(mSeller.fId,
                             eSeller::sSellerPageWidth,
                             eSeller::sSellerPageHeight,
                             items, stats, eq);
        });
        return b;
    };

    auto& ps = mSeller.fPages;
    const int iMax = ps.size();
    for(; i < iMax; i++) {
        addTab(ps[i]);
    }
    auto& lastPage = mSeller.fClientPage[clientId];
    const auto lastB = addTab(lastPage);

    const auto& res = resolution();
    const int p = res.largePadding();
    tabsWidget->stackHorizontally(p);
    tabsWidget->fitContent();
    innerW->addWidget(tabsWidget);

    mBag = new eSellerBag(window());
    auto& firstPage = ps.empty() ? lastPage : ps[0];
    auto& page = lastPage.empty() ? firstPage : lastPage;
    mBag->initialize(mSeller.fId,
                     eSeller::sSellerPageWidth,
                     eSeller::sSellerPageHeight,
                     page, stats, eq);
    const int w = mBag->width();
    tabsWidget->setWidth(w);
    lastB->align(eAlignment::right);
    innerW->addWidget(mBag);

    mStashCoins = new eCoinsWidget(window());
    mStashCoins->initialize(eq.fStashGold, nullptr, 12);
    innerW->addWidget(mStashCoins);

    innerW->stackVertically(p);
    innerW->fitContent();

    setup(innerW);
}

bool eSellerWidget::sReplaceItemId(
    const uint32_t clientId,
    const eReplaceItemId& r) {
    if(!sInstance) return false;
    return sInstance->replaceItemId(clientId, r);
}

bool eSellerWidget::replaceItemId(
    const uint32_t clientId,
    const eReplaceItemId& r) {
    if(mSeller.fId != r.fSellerId) return false;
    return mSeller.setItemId(
        clientId, r.fOldItemId, r.fNewItemId);
}


bool eSellerWidget::dropItem() {
    if(eInventoryWidget::sBlocked) return false;
    auto& dragged = mEq->fDragged;
    if(dragged.fType == eItemType::none) return false;
    const uint32_t gold = dragged.calculateSellCost();
    mEq->fInventoryGold += gold;
    eSellAction a;
    a.fSellerId = mSeller.fId;
    a.fItemId = dragged.fItemId;
    eGameWidget::sSendSellAction(a);
    auto& p = mSeller.fClientPage[mClientId];
    p.tryAdd(dragged);
    dragged = eItem();
    eHoverWidget::sUpdateDragItem(dragged);
    return true;
}

void eSellerWidget::paintEvent(ePainter& p) {
    eBgWidget::paintEvent(p);
    mStashCoins->setCount(mEq->fStashGold);
}