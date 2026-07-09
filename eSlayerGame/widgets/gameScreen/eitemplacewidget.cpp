#include "eitemplacewidget.h"

#include "../../textures/etexturecolorholder.h"
#include "../../textures/eiteminstancetexture.h"
#include "../../textures/euitextures.h"

#include "einventorywidget.h"
#include "ehoverwidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/evectorhelpers.h>

void eItemPlaceWidget::intialize(
    const std::shared_ptr<eTexture>& tex,
    eEquipment& eq,
    const eStats* const stats,
    eItem eEquipment::* const item,
    const ePlaceType place,
    const eHoverItemType htype,
    eItem* dragged) {
    setNoPadding();
    mEq = &eq;
    if(!dragged) {
        mDragged = &mEq->fDragged;
    } else {
        mDragged = dragged;
    }
    mStats = stats;
    mDst = item;
    mPlace.fType = place;
    mHoverType = htype;

    setTexture(tex);
    mTex = tex;
    fitContent();
}

bool eItemPlaceWidget::dropItem() {
    if(eInventoryWidget::sBlocked) return true;
    auto& dragged = *mDragged;
    if(dragged.fType == eItemType::none) return true;
    auto& dst = mEq->*mDst;
    if(dragged.fType == eItemType::jewel) {
        if(dst.spaceForJewel()) {
            dst.addJewel(dragged);
            eEquipmentAction a;
            a.fType = eEquipmentActionType::insertJewel;
            a.fItemId1 = dragged.fItemId;
            a.fItemId2 = dst.fItemId;
            a.fPlace = mPlace;
            eGameWidget::sSendEqAction(a);
            dragged = eItem();
            eHoverWidget::sUpdateDragItem(dragged);
            eHoverWidget::sClearHover();
            return true;
        }
    }
    if(!draggedCompatible()) return true;
    std::swap(dragged, dst);
    eHoverWidget::sUpdateDragItem(dragged);

    eEquipmentAction a;
    if(dragged.fType == eItemType::none) {
        a.fType = eEquipmentActionType::drop;
    } else {
        a.fType = eEquipmentActionType::switchDrag;
        a.fItemId1 = dragged.fItemId;
    }
    a.fPlace = mPlace;
    eGameWidget::sSendEqAction(a);

    return true;
}

void eItemPlaceWidget::setHoverItem() {
    const auto& item = mEq->*mDst;
    const auto rect = globalRect();
    eHoverItem hitem;
    hitem.fItem = item;
    hitem.fType = mHoverType;
    eHoverWidget::sSetHoverItem(hitem, rect);
}

bool eItemPlaceWidget::mousePressEvent(const eMouseEvent& e) {
    if(eInventoryWidget::sBlocked) return true;
    auto& dragged = *mDragged;
    if(dragged.fType != eItemType::none) return true;
    auto& dst = mEq->*mDst;
    std::swap(dragged, dst);
    eHoverWidget::sUpdateDragItem(dragged);

    eEquipmentAction a;
    a.fType = eEquipmentActionType::drag;
    a.fItemId1 = dragged.fItemId;
    eGameWidget::sSendEqAction(a);

    return true;
}

bool eItemPlaceWidget::mouseMoveEvent(const eMouseEvent& e) {
    return true;
}

bool eItemPlaceWidget::mouseEnterEvent(const eMouseEvent& e) {
    setHoverItem();
    return true;
}

bool eItemPlaceWidget::mouseLeaveEvent(const eMouseEvent& e) {
    eHoverWidget::sSetHoverItem(eHoverItem());
    return true;
}

bool eItemPlaceWidget::draggedCompatible() {
    auto& dragged = *mDragged;
    const bool met = mStats ? mStats->itemReqsMet(dragged) : true;
    if(!met) return false;
    const auto& dst = mEq->*mDst;
    return mEq->canPlace(dragged, dst);
}

void eItemPlaceWidget::paintEvent(ePainter& p) {
    const auto& item = mEq->*mDst;
    const auto rect = eWidget::rect();
    const bool h = hovered();
    eTextureColorSetting color;
    if(h) {
        const bool dragging = (*mDragged).fType != eItemType::none;
        if(!dragging) {
            if(item.fType != eItemType::none) {
                color.set(0.f, 0.7f, 0.f);
            }
        } else if(draggedCompatible()) {
            color.set(0.f, 0.7f, 0.f);
        } else {
            color.set(0.7f, 0.f, 0.f);
        }
    } else if(item.fType != eItemType::none) {
        const bool met = mStats ? mStats->itemReqsMet(item) : true;
        if(met) {
            color.set(0.4f, 0.4f, 0.4f);
        } else {
            color.set(0.7f, 0.f, 0.f);
        }
    }
    {
        const eTextureColorHolder mod(color, mTex);
        p.drawTexture(0, 0, mTex);
    }
    if(item.fType == eItemType::none) return;
    const auto r = renderer();
    const auto& res = resolution();
    const eItemInstanceTexture tex(r, res, item);
    const auto mod = tex.request();
    p.drawTexture(rect, mod.fTex, eAlignment::center);
    if(h) {
        const uint8_t nj = tex.nJewels();
        const uint8_t ns = tex.nSockets();
        for(uint8_t i = 0; i < nj; i++) {
            const auto mod = tex.requestJewel(i);
            const auto pos = tex.jewelPosition(i, ns);
            p.drawTexture(rect.x + rect.w*pos.fX,
                          rect.y + rect.h*pos.fY,
                          mod.fTex, eAlignment::center);
        }
        for(uint8_t i = nj; i < ns; i++) {
            const auto& tex = eUITextures::sSocket;
            const auto pos = eItemInstanceTexture::jewelPosition(i, ns);
            p.drawTexture(rect.x + rect.w*pos.fX,
                          rect.y + rect.h*pos.fY,
                          tex, eAlignment::center);
        }
    }
}
