#include "einventorywidget.h"

#include "einventorybagpackwidget.h"

void eInventoryWidget::initialize(
    const eAction& dragChange,
    eEquipment* const eq) {
    mEq = eq;
    const auto& res = resolution();
    int dim;
    switch(res.uiScale()) {
    case eUIScale::small:
        dim = 40;
        break;
    case eUIScale::medium:
        dim = 60;
        break;
    case eUIScale::large:
        dim = 80;
        break;
    }

    mBagpack = new eInventoryBagpackWidget(window());
    mBagpack->initialize(dragChange, mEq, dim, 10, 4);
    addWidget(mBagpack);
    mBagpack->align(eAlignment::bottom | eAlignment::hcenter);
}

void eInventoryWidget::paintEvent(ePainter& p) {

}

bool eInventoryWidget::dropItem(const SDL_Point& pos) {
    {
        const SDL_Point bpos{pos.x - mBagpack->x(),
                             pos.y - mBagpack->y()};
        const bool b = mBagpack->dropItem(bpos);
        if(b) return true;
    }
    return false;
}
