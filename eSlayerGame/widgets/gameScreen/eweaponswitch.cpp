#include "eweaponswitch.h"

#include "../echeckbutton.h"

#include <eSlayerHelpers/eequipment.h>

void eWeaponSwitch::initialize(eEquipment* const eq,
                               const eAction& update) {
    setNoPadding();
    mEq = eq;
    const auto switchAction = [this, update](const bool) {
        mEq->fWeapons1 = !mEq->fWeapons1;
        update();
    };
    mISwitch = new eCheckButton(window());
    mISwitch->setCheckAction(switchAction);
    mISwitch->setText("I");
    mISwitch->fitContent();
    addWidget(mISwitch);

    mIISwitch = new eCheckButton(window());
    mIISwitch->setCheckAction(switchAction);
    mIISwitch->setText("II");
    mIISwitch->fitContent();
    addWidget(mIISwitch);

    stackHorizontally();
    fitContent();
}

void eWeaponSwitch::updateChecked() {
    mISwitch->setChecked(mEq->fWeapons1);
    mIISwitch->setChecked(!mEq->fWeapons1);
}
