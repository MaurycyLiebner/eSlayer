#ifndef EINVENTORYWIDGET_H
#define EINVENTORYWIDGET_H

#include "../ewidget.h"

class eItemDragWidget;
class eInventoryBagpackWidget;
class eItemPlaceWidget;
struct eEquipment;
class eWeaponSwitch;
class eItemDragWidget;
struct eStats;

class eInventoryWidget : public eWidget {
public:
    eInventoryWidget(eMainWindow* const window);
    ~eInventoryWidget();

    void initialize(eEquipment& eq, const eStats& stats);

    bool dropItem(const SDL_Point& pos);

    void updateWeapons();

    static bool sBlocked;
    static eInventoryWidget* sInstance;
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
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
