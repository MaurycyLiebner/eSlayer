#ifndef EINVENTORYWIDGET_H
#define EINVENTORYWIDGET_H

#include "ebgwidget.h"

class eHoverWidget;
class eInventoryBagpackWidget;
class eItemPlaceWidget;
struct eEquipment;
class eWeaponSwitch;
class eHoverWidget;
struct eStats;
class eCoinsWidget;

class eInventoryWidget : public eBgWidget {
public:
    eInventoryWidget(eMainWindow* const window);
    ~eInventoryWidget();

    void initialize(eEquipment& eq, const eStats& stats);

    bool dropItem();

    void updateWeapons();

    static bool sBlocked;
    static eInventoryWidget* sInstance;
protected:
    bool mousePressEvent(const eMouseEvent& e) override;
private:
    eInventoryBagpackWidget* mBagpack = nullptr;
    eCoinsWidget* mCoins = nullptr;
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
