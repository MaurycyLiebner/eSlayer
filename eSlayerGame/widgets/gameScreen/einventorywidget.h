#ifndef EINVENTORYWIDGET_H
#define EINVENTORYWIDGET_H

#include "ebgwidget.h"

#include <eSlayerHelpers/eitem.h>

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

    void initialize(eEquipment& eq, const eStats& stats,
                    const eHoverItemType htype);

    bool dropItem();

    void updateWeapons();

    static bool sBlocked;
    static eInventoryWidget* sInstance;
protected:
    void paintEvent(ePainter& p) override;
private:
    eInventoryBagpackWidget* mBagpack = nullptr;
    eCoinsWidget* mCoins = nullptr;
    std::vector<eItemPlaceWidget*> mItemPlaces;
    eEquipment* mEq = nullptr;

    eWeaponSwitch* mLWeaponSwitch = nullptr;
    eWeaponSwitch* mRWeaponSwitch = nullptr;
    eItemPlaceWidget* mWeapon1L = nullptr;
    eItemPlaceWidget* mWeapon1R = nullptr;
    eItemPlaceWidget* mWeapon2L = nullptr;
    eItemPlaceWidget* mWeapon2R = nullptr;
};

#endif // EINVENTORYWIDGET_H
