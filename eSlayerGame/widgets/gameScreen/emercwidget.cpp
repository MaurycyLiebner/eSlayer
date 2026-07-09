#include "emercwidget.h"

#include <eSlayerHelpers/emercenaries.h>

void eMercWidget::initialize(
    eMercenary& merc,
    eEquipment& eq,
    const eStats& stats) {
    const auto& res = resolution();
    const int p = res.largePadding();

    const auto innerW = new eWidget(window());
    innerW->setNoPadding();

    const auto& m = eMercenariesInfo::sMercs.get(
        merc.fMercType);
    const auto& places = m.fEquipment;

    mInv = new eInventoryWidgetBase(window());
    mInv->initialize(merc.fUnitId, merc.fEq, stats,
                     eHoverItemType::regular, places,
                     &eq.fDragged);
    innerW->addWidget(mInv);

    innerW->stackVertically(p);
    innerW->fitContent();

    setup(innerW);
}

bool eMercWidget::dropItem() {
    const bool h = mInv->hovered();
    if(!h) return false;
    return mInv->dropItem();
}
