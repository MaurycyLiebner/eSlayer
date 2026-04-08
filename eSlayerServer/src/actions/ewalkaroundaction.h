#ifndef EWALKAROUNDACTION_H
#define EWALKAROUNDACTION_H

#include "eunitactionbase.h"

#include <eSlayerHelpers/evec2.h>

#include <memory>

class eWalkAroundAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eWalkAroundAction>
    sCreate(eServerUnit& unit, eServerArea& area,
            const int walkId, const int walkReadyId,
            const float time);

    void increment(const float by) override;
private:
    int mCurrentAnim = -1;
    int mWalkId = -1;
    int mWalkReadyId = -1;
    float mDirChangeCounter = 0.f;
    eVec2f mMoveDir;
};

#endif // EWALKAROUNDACTION_H
