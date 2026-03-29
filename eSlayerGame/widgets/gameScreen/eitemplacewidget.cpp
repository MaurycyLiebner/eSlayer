#include "eitemplacewidget.h"

#include "../../textures/eitemstextures.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/evectorhelpers.h>

void eItemPlaceWidget::intialize(
    const int width, const int height,
    const int dimensions,
    eEquipment* const eq,
    eItem eEquipment::* const item,
    const std::vector<eItemType>& allowedTypes,
    const eAction& dragChange) {
    mEq = eq;
    mItem = item;
    mAllowedTypes = allowedTypes;
    mDraggedChanged = dragChange;

    mWidth = width;
    mHeight = height;
    mDimensions = dimensions;
    resize(mWidth*mDimensions, mHeight*mDimensions);
}

bool eItemPlaceWidget::dropItem() {
    auto& dragged = mEq->fDragged;
    if(dragged.fType == eItemType::none) return false;
    if(!draggedCompatible()) return false;
    auto& item = mEq->*mItem;
    std::swap(dragged, item);
    mDraggedChanged();
    return true;
}

bool eItemPlaceWidget::mousePressEvent(const eMouseEvent& e) {
    auto& dragged = mEq->fDragged;
    if(dragged.fType != eItemType::none) return true;
    auto& item = mEq->*mItem;
    std::swap(dragged, item);
    mDraggedChanged();
    return true;
}

bool eItemPlaceWidget::draggedCompatible() {
    auto& dragged = mEq->fDragged;
    return eVectorHelpers::contains(mAllowedTypes, dragged.fType);
}

void eItemPlaceWidget::paintEvent(ePainter& p) {
    const auto& item = mEq->*mItem;
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
    p.fillRect(rect, fillColor);
    const auto res = resolution();
    const int lineWidth = res.lineWidth();
    p.drawRect(rect, SDL_Color{255, 255, 255, 255}, lineWidth);
    if(item.fType == eItemType::none) return;
    const auto r = renderer();
    auto& itemTex = eItemsTextures::getByItemDataId(item.fDataId);
    itemTex.request(r);
    p.drawTexture(rect, itemTex.fTex, eAlignment::center);
}
