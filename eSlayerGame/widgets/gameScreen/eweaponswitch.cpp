#include "eweaponswitch.h"

#include "../echeckablebutton.h"
#include "../../textures/euitextures.h"

#include <eSlayerHelpers/eequipment.h>

class eWeaponSwitchButton : public eCheckableButton {
public:
    using eCheckableButton::eCheckableButton;

    void initialize(const std::shared_ptr<eTexture>& tex) {
        setNoPadding();
        mTex = tex;
        setTexture(tex);
        fitContent();
    }
protected:
    void paintEvent(ePainter& p) override {
        const bool darken = !checked() & !hovered();
        if(darken) mTex->setColorMod(100, 100, 100);
        p.drawTexture(0, 0, mTex);
        if(darken) mTex->clearColorMod();
    }
private:
    std::shared_ptr<eTexture> mTex;
};

void eWeaponSwitch::initialize(eEquipment* const eq,
                               const eAction& update) {
    setNoPadding();
    mEq = eq;
    const auto switchAction = [this, update](const bool) {
        mEq->fWeapons1 = !mEq->fWeapons1;
        update();
    };
    mISwitch = new eWeaponSwitchButton(window());
    mISwitch->setCheckAction(switchAction);
    mISwitch->initialize(eUITextures::sWeaponSwitch1);
    addWidget(mISwitch);

    mIISwitch = new eWeaponSwitchButton(window());
    mIISwitch->setCheckAction(switchAction);
    mIISwitch->initialize(eUITextures::sWeaponSwitch2);
    addWidget(mIISwitch);

    stackHorizontally();
    fitContent();
}

void eWeaponSwitch::updateChecked() {
    mISwitch->setChecked(mEq->fWeapons1);
    mIISwitch->setChecked(!mEq->fWeapons1);
}
