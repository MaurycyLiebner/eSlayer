#ifndef EWEAPONSWITCH_H
#define EWEAPONSWITCH_H

#include "../ewidget.h"

class eCheckButton;
struct eEquipment;

class eWeaponSwitch : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eEquipment* const eq,
                    const eAction& update);
    void updateChecked();
private:
    eEquipment* mEq = nullptr;
    eCheckButton* mISwitch = nullptr;
    eCheckButton* mIISwitch = nullptr;
};

#endif // EWEAPONSWITCH_H
