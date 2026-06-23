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
        w, h, items, eHoverItemType::buy);
    mSellerId = sellerId;
    mStats = &stats;
    mEq = &eq;
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
        const uint32_t gold = item.calculateCost();
        const uint32_t hgold = mEq->totalGold();
        if(gold > hgold) return true;
        const bool met = mStats->itemReqsMet(item);
        const bool r = mEq->add(item, met, &place);
        if(!r) return true;
        mEq->takeGold(gold);
        if(item.fType != eItemType::potion) {
            inv.erase(inv.begin() + itemId);
        } else {
            eInventoryWidget::sBlocked = true;
        }

        eBuyAction a;
        a.fPlace = place;
        a.fItemId = item.fItemId;
        a.fSellerId = mSellerId;
        eGameWidget::sSendBuyAction(a);
    }
    return true;
}

void eSellerBag::paintEvent(ePainter& p) {
    SDL_FColor fillColor{0.f, 0.f, 0.f, 1.f};
    SDL_Rect ihoverRect = {0, 0, 0, 0};
    const auto& dragged = mEq->fDragged;
    const auto mpos = mousePos();
    const auto ipos = mousePosToItemPos(mpos);
    if(dragged.fType == eItemType::none) {
        const int itemId = itemIdAt(ipos);
        if(itemId != -1) {
            const auto& item = (*mItems)[itemId];
            ihoverRect = SDL_Rect{item.fX, item.fY,
                                  item.fW, item.fH};
            fillColor = SDL_FColor{0.f, 0.7f, 0.f, 1.f};
        }
    }

    eBagpackBase::paint(p, ihoverRect, fillColor);
}