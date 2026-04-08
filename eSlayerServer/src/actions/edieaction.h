#ifndef EDIEACTION_H
#define EDIEACTION_H

#include "ecomplexaction.h"

class eDieAction : public eComplexAction {
    enum class eDieStage {
        none,
        collapse,
        body
    };

public:
    using eComplexAction::eComplexAction;

    void decide() override;
private:
    eDieStage mStage = eDieStage::none;
};

#endif // EDIEACTION_H
