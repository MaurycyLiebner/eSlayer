#ifndef EEXPLODEACTION_H
#define EEXPLODEACTION_H
#include "ecomplexaction.h"

class eExplodeAction : public eComplexAction {
    enum class eExplodeStage {
        none,
        explode,
        body
    };

public:
    using eComplexAction::eComplexAction;

    void decide() override;
private:
    eExplodeStage mStage = eExplodeStage::none;
};

#endif // EEXPLODEACTION_H
