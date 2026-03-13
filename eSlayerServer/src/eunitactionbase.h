#ifndef EUNITACTIONBASE_H
#define EUNITACTIONBASE_H

#include "eunitaction.h"

#include <functional>

using eAction = std::function<void()>;

class eUnitActionBase : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;

    void setDuration(const double d);
    void setAction(const double time, const eAction& a);
protected:
    void setup(const int anim,
               int frames,
               const eAction& a);

private:
    double mRemTime;
    double mActionTime;

    eAction mAction;
};

#endif // EUNITACTIONBASE_H
