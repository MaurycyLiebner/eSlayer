#ifndef ECLIENTACTION_H
#define ECLIENTACTION_H

#include "ecomplexaction.h"

class eClientAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void decide() override;
};

#endif // ECLIENTACTION_H
