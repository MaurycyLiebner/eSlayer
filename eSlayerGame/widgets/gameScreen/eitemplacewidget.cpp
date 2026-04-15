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
    if(dragged.fType == eItemType::none) return true;
    if(!draggedCompatible()) return true;
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
    const bool met = mStats->itemReqsMet(dragged);
    if(!met) return false;
    const auto& dst = mEq->*mDst;
    return mEq->canPlace(dragged, dst);
}

void eItemPlaceWidget::paintEvent(ePainter& p) {
    const auto& item = mEq->*mDst;
    const auto rect = eWidget::rect();
    const bool h = hovered();
    if(h) {
        const bool dragging = mEq->fDragged.fType != eItemType::none;
        if(!dragging) {
            if(item.fType != eItemType::none) {
                mTex->setColorMod(0, 175, 0);
            }
        } else if(draggedCompatible()) {
            mTex->setColorMod(0, 175, 0);
        } else {
            mTex->setColorMod(175, 0, 0);
        }
    } else if(item.fType != eItemType::none) {
        const bool met = mStats->itemReqsMet(item);
        if(met) {
            mTex->setColorMod(100, 100, 100);
        } else {
            mTex->setColorMod(175, 0, 0);
        }
    }
    p.drawTexture(0, 0, mTex);
    mTex->clearColorMod();
    if(item.fType == eItemType::none) return;
    const auto r = renderer();
    const auto& res = resolution();
    auto& itemTex = eItemsTextures::getByItemDataId(item.fDataId);
    itemTex.request(r, res);
    const auto& tex = itemTex.fTex;
    p.drawTexture(rect, itemTex.fTex, eAlignment::center);
}
