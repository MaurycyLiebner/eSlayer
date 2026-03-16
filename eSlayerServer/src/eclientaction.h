#ifndef ECLIENTACTION_H
#define ECLIENTACTION_H

#include "ecomplexaction.h"

class eClientAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void increment(const float by) override;
    void decide() override;

    void attack(const std::shared_ptr<eServerUnit>& target);
private:
    std::shared_ptr<eServerUnit> mAttackTarget;
};

#endif // ECLIENTACTION_H
