#ifndef EINVENTORYWIDGET_H
#define EINVENTORYWIDGET_H

#include "ebgwidget.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eequipmentplace.h>
#include <eSlayerHelpers/eeqoptions.h>

class eHoverWidget;
class eInventoryBagpackWidget;
class eItemPlaceWidget;
struct eEquipment;
class eWeaponSwitch;
class eHoverWidget;
struct eStats;
class eCoinsWidget;

class eInventoryWidgetBase : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const uint32_t unitId,
                    eEquipment& eq, const eStats& stats,
                    const eHoverItemType htype,
                    const std::vector<ePlaceType>& places =
                    {ePlaceType::helmet,
                     ePlaceType::armor,
                     ePlaceType::belt,
                     ePlaceType::boots,
                     ePlaceType::gloves,
                     ePlaceType::ringL,
                     ePlaceType::ringR,
                     ePlaceType::amulet,
                     ePlaceType::weapon1L,
                     ePlaceType::weapon1R,
                     ePlaceType::weapon2L,
                     ePlaceType::weapon2R,
                     ePlaceType::inventory},
                    eItem* const dragged = nullptr,
                    const std::optional<eEqOptions>& options = std::nullopt);

    bool dropItem();

    void updateWeapons(const bool setHover = true);

    static bool sBlocked;
protected:
    void paintEvent(ePainter& p) override;
private:
    uint32_t mUnitId = 0;

    std::optional<eEqOptions> mOptions;
    eItem* mDragged = nullptr;

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

class eInventoryWidget : public eBgWidget {
public:
    using eBgWidget::eBgWidget;

    void initialize(const uint32_t unitId,
                    eEquipment& eq, const eStats& stats,
                    const eHoverItemType htype);

    bool dropItem();
    void updateWeapons();

    static bool sBlocked;
private:
    eInventoryWidgetBase* mBase = nullptr;
};

#endif // EINVENTORYWIDGET_H
