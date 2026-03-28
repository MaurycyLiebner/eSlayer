#include "eitemdragwidget.h"

#include "../../textures/eitemstextures.h"

#include <eSlayerHelpers/eitemsdata.h>

void eItemDragWidget::initialize(const eDropAction& dropAction) {
    mDropAction = dropAction;
}

void eItemDragWidget::setItemDataId(const int dataId) {
    if(dataId == -1) {
        mItem = nullptr;
    } else {
        const auto r = renderer();
        const auto name = eItemsData::name(dataId);
        auto& itemTex = eItemsTextures::get(name);
        itemTex.request(r);
        mItem = itemTex.fTex;
    }
}

void eItemDragWidget::paintEvent(ePainter& p) {
    if(!mItem) return;
    p.drawTexture(mMousePos.x, mMousePos.y, mItem, eAlignment::center);
}

bool eItemDragWidget::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos.x = e.x();
    mMousePos.y = e.y();
    return mItem.get();
}

bool eItemDragWidget::mousePressEvent(const eMouseEvent& e) {
    if(!mItem) return false;
    mDropAction(mMousePos);
    return true;
}
