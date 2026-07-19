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
    const uint32_t unitId,
    const std::shared_ptr<eTexture>& tex,
    eEquipment& eq,
    const eStats* const stats,
    eItem eEquipment::* const item,
    const ePlaceType place,
    const eHoverItemType htype,
    eItem* const dragged,
    const std::optional<eEqOptions>& options) {
    mUnitId = unitId;
    setNoPadding();
    mEq = &eq;
    if(!dragged) {
        mDragged = &mEq->fDragged;
    } else {
        mDragged = dragged;
    }
    mOptions = options;
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
    if(mOptions) {
        const bool r = mOptions->validateItem(dragged);
        if(!r) return false;
    }
    auto& dst = mEq->*mDst;
    if(dragged.fType == eItemType::jewel) {
        if(dst.spaceForJewel()) {
            dst.addJewel(dragged);
            eEquipmentAction a;
            a.fUnitId = mUnitId;
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
    a.fUnitId = mUnitId;
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
    a.fUnitId = mUnitId;
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
    if(mOptions) {
        const bool r = mOptions->validateItem(dragged);
        if(!r) return false;
    }
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
    const auto r = renderer();
    const auto& res = resolution();
    if(item.fType == eItemType::none) {
        eItem otherW;
        if(mDst == &eEquipment::fWeapon1R) {
            otherW = mEq->fWeapon1L;
        } else if(mDst == &eEquipment::fWeapon2R) {
            otherW = mEq->fWeapon2L;
        }
        if(otherW.fType == eItemType::none) return;

        const auto& itemData = eItemsData::get(otherW.fDataId);
        if(!itemData.fTwoHanded) return;

        {
            const bool met = mStats ? mStats->itemReqsMet(otherW) : true;
            if(met) {
                color.set(0.4f, 0.4f, 0.4f);
            } else {
                color.set(0.7f, 0.f, 0.f);
            }
            const eTextureColorHolder mod(color, mTex);
            p.drawTexture(0, 0, mTex);
        }

        const eItemInstanceTexture itex(r, res, otherW);
        const auto mod = itex.request();
        const auto& tex = mod.fTex;
        const eTextureColorHolder h(true, 1.f, 1.f, 1.f, 0.5f, tex);
        p.drawTexture(rect, tex, eAlignment::center);

        return;
    }
    const eItemInstanceTexture tex(r, res, item);
    const auto mod = tex.request();
    p.drawTexture(rect, mod.fTex, eAlignment::center);
    if(h) {
        const uint8_t nj = tex.nJewels();
        const uint8_t ns = tex.nSockets();
        for(uint8_t i = 0; i < ns; i++) {
            const auto& tex = eUITextures::sSocket;
            const auto pos = eItemInstanceTexture::jewelPosition(i, ns);
            p.drawTexture(rect.x + rect.w*pos.fX,
                          rect.y + rect.h*pos.fY,
                          tex, eAlignment::center);
        }
        for(uint8_t i = 0; i < nj; i++) {
            const auto mod = tex.requestJewel(i);
            const auto pos = tex.jewelPosition(i, ns);
            p.drawTexture(rect.x + rect.w*pos.fX,
                          rect.y + rect.h*pos.fY,
                          mod.fTex, eAlignment::center);
        }
    }
}
