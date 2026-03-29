#include "eitemdragwidget.h"

#include "../../textures/eitemstextures.h"

#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/eequipment.h>

eItemDragWidget* eItemDragWidget::sInstance = nullptr;

eItemDragWidget::eItemDragWidget(eMainWindow* const w) :
    eWidget(w) {
    sInstance = this;
}

eItemDragWidget::~eItemDragWidget() {
    sInstance = nullptr;
}

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

void eItemDragWidget::sUpdateDragItem(const eEquipment& eq) {
    if(!sInstance) return;
    if(eq.fDragged.fType == eItemType::none) {
        sInstance->setItemDataId(-1);
    } else {
        const int dataId = eq.fDragged.fDataId;
        sInstance->setItemDataId(dataId);
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
