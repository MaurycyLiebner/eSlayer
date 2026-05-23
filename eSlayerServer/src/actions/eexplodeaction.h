#ifndef EEXPLODEACTION_H
#define EEXPLODEACTION_H

#include "ecomplexaction.h"

class eExplodeAction : public eComplexAction {
    enum class eExplodeStage {
        none,
        explode
    };

public:
    eExplodeAction(const eExplodeType type,
                   eServerUnit& unit,
                   eServerArea& area);

    void decide() override;
private:
    const eExplodeType mType;
    eExplodeStage mStage = eExplodeStage::none;
};

#endif // EEXPLODEACTION_H
