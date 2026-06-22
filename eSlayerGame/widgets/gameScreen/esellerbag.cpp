#include "esellerbag.h"

#include "einventorywidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/eequipmentaction.h>

void eSellerBag::initialize(
    const uint32_t sellerId,
    const int w, const int h,
    std::vector<eInventoryItem>& items,
    const eStats& stats, eEquipment& eq) {
    eBagpackBase::initialize(
        w, h, items, eHoverItemType::sell);
    mSellerId = sellerId;
    mStats = &stats;
    mEq = &eq;
}

bool eSellerBag::dropItem() {
    if(eInventoryWidget::sBlocked) return false;
    auto& dragged = mEq->fDragged;
    if(dragged.fType == eItemType::none) return false;
    dragged = eItem();
    return true;
}

bool eSellerBag::mousePressEvent(
    const eMouseEvent& e) {
    if(eInventoryWidget::sBlocked) return true;
    const auto ipos = mousePosToItemPos({e.x(), e.y()});
    const int itemId = itemIdAt(ipos);
    if(itemId == -1) return true;
    auto& inv = *mItems;
    const auto item = inv[itemId].fItem;
    const auto b = e.button();
    if(b == eMouseButton::left) {
        eEquipmentPlace place;
        const uint32_t gold = 100;
        const uint32_t hgold = mEq->totalGold();
        if(gold > hgold) return true;
        const bool met = mStats->itemReqsMet(item);
        const bool r = mEq->add(item, met, &place);
        if(!r) return true;
        mEq->takeGold(gold);
        if(item.fType != eItemType::potion) {
            inv.erase(inv.begin() + itemId);
        }

        eBuyAction a;
        a.fPlace = place;
        a.fItemId = item.fItemId;
        a.fSellerId = mSellerId;
        eGameWidget::sSendBuyAction(a);
    }
    return true;
}
