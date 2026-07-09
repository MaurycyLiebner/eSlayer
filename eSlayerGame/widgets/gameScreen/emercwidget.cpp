#include "emercwidget.h"

#include "../../names/emercenarynames.h"

#include <eSlayerHelpers/emercenaries.h>

void eMercWidget::initialize(
    eMercenary& merc,
    eEquipment& eq,
    const eStats& stats) {
    mMerc = &merc;
    mAttributes = merc.attributes();

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

    mStats.fSkills.emplace_back();
    mStats.fSkills.emplace_back();
    mStats.calculate(mAttributes, merc.fEq);
    mStat = new eStatsWidgetBase(window());
    const auto& names = eMercenaryNames::sNames.get(merc.fMercType);
    const auto& name = names[merc.fNameId % names.size()];
    mStat->initialize(name, mStats, merc.fEq, mAttributes);
    innerW->addWidget(mStat);

    innerW->stackVertically(p);
    innerW->fitContent();

    setup(innerW);
}

bool eMercWidget::dropItem() {
    const bool h = mInv->hovered();
    if(!h) return false;
    return mInv->dropItem();
}

void eMercWidget::paintEvent(ePainter& p) {
    eBgWidget::paintEvent(p);
    mStats.calculate(mAttributes, mMerc->fEq);
}
