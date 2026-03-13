#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "eunitaction.h"

#include <memory>

class eComplexAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;
    virtual void decide() = 0;

    void setChild(const std::shared_ptr<eUnitAction>& c);
protected:
    bool attack(const eServerUnit& u);
private:
    std::shared_ptr<eUnitAction> mChild;
};

#endif // ECOMPLEXACTION_H
