#ifndef EINVENTORYWIDGET_H
#define EINVENTORYWIDGET_H

#include "../ewidget.h"

class eItemDragWidget;
class eInventoryBagpackWidget;
class eItemPlaceWidget;
struct eEquipment;
class eWeaponSwitch;
class eItemDragWidget;

class eInventoryWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eEquipment* const eq);

    bool dropItem(const SDL_Point& pos);

    void updateWeapons();

    static bool sBlocked;
    static eItemDragWidget* sDragWidget;
protected:
    void paintEvent(ePainter& p) override;
private:
    eInventoryBagpackWidget* mBagpack = nullptr;
    std::vector<eItemPlaceWidget*> mItemPalces;
    eEquipment* mEq = nullptr;

    eWeaponSwitch* mLWeaponSwitch = nullptr;
    eWeaponSwitch* mRWeaponSwitch = nullptr;
    eItemPlaceWidget* mWeapon1L = nullptr;
    eItemPlaceWidget* mWeapon1R = nullptr;
    eItemPlaceWidget* mWeapon2L = nullptr;
    eItemPlaceWidget* mWeapon2R = nullptr;
};

#endif // EINVENTORYWIDGET_H
