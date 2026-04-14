#include "eitemplacewidget.h"

#include "../../textures/eitemstextures.h"
#include "einventorywidget.h"
#include "eitemdragwidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/evectorhelpers.h>

void eItemPlaceWidget::intialize(
    const std::shared_ptr<eTexture>& tex,
    eEquipment& eq,
    const eStats& stats,
    eItem eEquipment::* const item,
    const std::vector<eItemType>& allowedTypes) {
    setNoPadding();
    mEq = &eq;
    mStats = &stats;
    mDst = item;
    mAllowedTypes = allowedTypes;

    setTexture(tex);
    mTex = tex;
    fitContent();
}

bool eItemPlaceWidget::dropItem() {
    if(eInventoryWidget::sBlocked) return true;
    auto& dragged = mEq->fDragged;
    if(dragged.fType == eItemType::none) return false;
    if(!draggedCompatible()) return false;
    auto& dst = mEq->*mDst;
    std::swap(dragged, dst);
    eItemDragWidget::sUpdateDragItem(*mEq);
    eGameWidget::sSendInventoryRearranged();
    return true;
}

bool eItemPlaceWidget::mousePressEvent(const eMouseEvent& e) {
    if(eInventoryWidget::sBlocked) return true;
    auto& dragged = mEq->fDragged;
    if(dragged.fType != eItemType::none) return true;
    auto& dst = mEq->*mDst;
    std::swap(dragged, dst);
    eItemDragWidget::sUpdateDragItem(*mEq);
    eGameWidget::sSendInventoryRearranged();
    return true;
}

bool eItemPlaceWidget::mouseMoveEvent(const eMouseEvent& e) {
    return true;
}

bool eItemPlaceWidget::mouseEnterEvent(const eMouseEvent& e) {
    const auto& item = mEq->*mDst;
    eItemDragWidget::sSetHoverItem(item);
    return true;
}

bool eItemPlaceWidget::mouseLeaveEvent(const eMouseEvent& e) {
    eItemDragWidget::sSetHoverItem(eItem());
    return true;
}

bool eItemPlaceWidget::draggedCompatible() {
    auto& dragged = mEq->fDragged;
    const auto& dst = mEq->*mDst;
    return mEq->canPlace(dragged, dst);
}

void eItemPlaceWidget::paintEvent(ePainter& p) {
    const auto& item = mEq->*mDst;
    const auto rect = eWidget::rect();
    const bool h = hovered();
    SDL_Color fillColor{0, 0, 0, 255};
    if(h) {
        if(mEq->fDragged.fType == eItemType::none) {
            if(item.fType != eItemType::none) {
                fillColor = SDL_Color{0, 128, 0, 255};
            }
        } else if(draggedCompatible()) {
            fillColor = SDL_Color{0, 128, 0, 255};
        } else {
            fillColor = SDL_Color{128, 0, 0, 255};
        }
    }
    p.drawTexture(0, 0, mTex);
    if(item.fType == eItemType::none) return;
    const auto r = renderer();
    auto& itemTex = eItemsTextures::getByItemDataId(item.fDataId);
    itemTex.request(r);
    const auto& tex = itemTex.fTex;
    const bool mod = !mStats->itemReqsMet(item);
    if(mod) tex->setColorMod(255, 0, 0);
    p.drawTexture(rect, itemTex.fTex, eAlignment::center);
    if(mod) tex->clearColorMod();
}
