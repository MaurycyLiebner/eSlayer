#ifndef EFOLLOWERPORTRAITS_H
#define EFOLLOWERPORTRAITS_H

#include "../ewidget.h"

#include <set>

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
                    const int w, const int h,
                    const ePressAction& pressA,
                    const eDropAction& dropA);

    void addFollower(const uint32_t follower);

    bool dropItem();
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateFollowers();
    void updatePortrait(const eUnit& u);
    void addPortrait(const eUnit& u);
    void removePortrait(const uint32_t uid);

    std::map<uint32_t, ePortrait*> mPortraits;
    const eGameWorld* mWorld = nullptr;
    const eGameWidget* mGW = nullptr;
    std::set<uint32_t> mFollowers;
    ePressAction mPressAction;
    eDropAction mDropAction;
};

#endif // EFOLLOWERPORTRAITS_H
