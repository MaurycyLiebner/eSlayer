#ifndef EDIEACTION_H
#define EDIEACTION_H

#include "ecomplexaction.h"

class eDieAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void decide() override;
};

#endif // EDIEACTION_H
