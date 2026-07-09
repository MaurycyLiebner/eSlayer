#ifndef EFOLLOWERPORTRAITS_H
#define EFOLLOWERPORTRAITS_H

#include "../ewidget.h"

#include <set>

class eGameWorld;
class eUnit;
class ePortrait;

class eFollowerPortraits : public eWidget {
public:
    using eWidget::eWidget;

    using ePressAction = std::function<void(const uint32_t)>;
    void initialize(const eGameWorld& world,
                    const int w, const int h,
                    const ePressAction& pressA);

    void addFollower(const uint32_t follower);
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateFollowers();
    void updatePortrait(const eUnit& u);
    void addPortrait(const eUnit& u);
    void removePortrait(const uint32_t uid);

    std::map<uint32_t, ePortrait*> mPortraits;
    const eGameWorld* mWorld = nullptr;
    std::set<uint32_t> mFollowers;
    ePressAction mPressAction;
};

#endif // EFOLLOWERPORTRAITS_H
