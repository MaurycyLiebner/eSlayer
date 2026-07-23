#include "emercwidget.h"

#include "../../names/emercenarynames.h"
#include "../etabwidget.h"
#include "../../etext.h"

#include <eSlayerHelpers/emercenaries.h>
#include <eSlayerHelpers/eunitsinfo.h>

void eMercWidget::initialize(
    eMercenary& merc,
    eEquipment& eq,
    const eStats& stats) {
    mMerc = &merc;
    mAttributes = merc.attributes();

    const auto innerW = new eTabWidget(window());
    innerW->initialize();

    const auto& m = eMercenariesInfo::sMercs.get(
        merc.fMercType);
    const auto& eqO = m.fEq;
    const auto& places = eqO.fEquipment;

    const auto uid = m.fUnitType;
    const auto& uinfo = eUnitsInfo::sUnits.get(uid);
    mStats.fDifficultyPenalties = uinfo.fDifficultyPenalties;
    mStats.fSkills.emplace_back();
    mStats.fSkills.emplace_back();
    mStats.calculate(mAttributes, merc.fEq);
    mStat = new eStatsWidgetBase(window());
    const auto& names = eMercenaryNames::sNames.get(merc.fMercType);
    const auto& name = names[merc.fNameId % names.size()];
    mStat->initialize(uid, name, mStats, merc.fEq, mAttributes, true);
    innerW->addTab(eText::text(17, 6), mStat);

    mInv = new eInventoryWidgetBase(window());
    mInv->initialize(merc.fUnitId, merc.fEq, stats,
                     eHoverItemType::regular, places,
                     &eq.fDragged, eqO);
    innerW->addTab(eText::text(17, 5), mInv);

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
