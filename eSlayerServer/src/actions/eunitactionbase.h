#ifndef EUNITACTIONBASE_H
#define EUNITACTIONBASE_H

#include "eunitaction.h"

#include <functional>

using eAction = std::function<void()>;

class eUnitActionBase : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const float by) override;

    void setDuration(const float d);

    void setup(const int anim,
               int frames,
               bool blocking,
               const eAction& a);
private:
    float mRemTime;
    bool mBlockingAction;
    float mActionTime;

    eAction mAction;
};

#endif // EUNITACTIONBASE_H
