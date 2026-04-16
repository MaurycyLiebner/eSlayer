#ifndef EWEAPONSWITCH_H
#define EWEAPONSWITCH_H

#include "../ewidget.h"

class eWeaponSwitchButton;
struct eEquipment;

class eWeaponSwitch : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eEquipment* const eq,
                    const eAction& update);
    void updateChecked();
private:
    eEquipment* mEq = nullptr;
    eWeaponSwitchButton* mISwitch = nullptr;
    eWeaponSwitchButton* mIISwitch = nullptr;
};

#endif // EWEAPONSWITCH_H
