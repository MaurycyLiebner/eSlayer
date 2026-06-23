#include "esellerwidget.h"

#include "../../etext.h"
#include "../mainMenu/emainmenubutton.h"

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

void eSellerWidget::initialize(const uint32_t clientId, const eSeller& s, eEquipment& eq,
                               const eStats& stats) {
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
        tabsWidget->addWidget(b);
        b->setPressAction([this, i, &items, &stats, &eq]() {
            mBag->initialize(mSeller.fId,
                             eSeller::sSellerPageWidth,
                             eSeller::sSellerPageHeight,
                             items, stats, eq);
        });
    };
    const int iMax = mSeller.fPages.size();
    for(; i < iMax; i++) {
        addTab(mSeller.fPages[i]);
    }
    auto& lastPage = mSeller.fClientPage[clientId];
    addTab(lastPage);

    const auto& res = resolution();
    const int p = res.largePadding();
    tabsWidget->stackHorizontally(p);
    tabsWidget->fitContent();
    innerW->addWidget(tabsWidget);

    mBag = new eSellerBag(window());
    mBag->initialize(mSeller.fId,
                     eSeller::sSellerPageWidth,
                     eSeller::sSellerPageHeight,
                     lastPage, stats, eq);
    innerW->addWidget(mBag);

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
