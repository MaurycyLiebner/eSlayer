#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/eattackdata.h>

#include <memory>

struct eHitData {
    float fHitChance;
    float fBlockMultiplier;
};

class eComplexAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const float by) override;
    virtual void decide() = 0;

    void setChild(const std::shared_ptr<eUnitAction>& c);

    bool getHit(const eHitData& data);

protected:
    bool attack(const eAttackData& target);
    bool attack(const eServerUnit& u);
private:
    bool spawnMissile(const ePointF& to,
                      const int missileId,
                      const int pathId);

    std::shared_ptr<eUnitAction> mChild;
};

#endif // ECOMPLEXACTION_H
