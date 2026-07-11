#ifndef EFOLLOWERPORTRAITS_H
#define EFOLLOWERPORTRAITS_H

#include "../ewidget.h"

class eGameWorld;
class eGameWidget;
class eUnit;
class ePortrait;
struct eEquipment;

class eFollowerPortraits : public eWidget {
public:
    using eWidget::eWidget;

    using eDropAction = std::function<bool(const uint32_t)>;
    using ePressAction = std::function<void(const uint32_t)>;
    void initialize(const eGameWidget& gw,
                    const eGameWorld& world,
                    const ePressAction& pressA,
                    const eDropAction& dropA);

    bool dropItem();
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateFollowers();

    std::vector<ePortrait*> mAllPortraits;
    std::map<uint32_t, ePortrait*> mUnitPortraits;
    std::map<uint8_t, ePortrait*> mStackPortraits;
    const eGameWorld* mWorld = nullptr;
    const eGameWidget* mGW = nullptr;
    ePressAction mPressAction;
    eDropAction mDropAction;
};

#endif // EFOLLOWERPORTRAITS_H
