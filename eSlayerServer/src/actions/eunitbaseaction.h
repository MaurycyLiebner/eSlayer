#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/epoint.h>

enum class eUnitStrategy {
    attack, move
};

struct eFlee {
    ePointF fFrom;
    float fDist;
};

class eUnitBaseAction : public eComplexAction {
public:
    eUnitBaseAction(eServerUnit& unit,
                    eServerArea& area);

    void increment(const float by) override;

    void planFlee(const eFlee& flee);
protected:
    void decide() override;

    void setStrategy(const eUnitStrategy s)
    { mStrategy = s; }

    bool attacking() const { return mAttacking; }

    bool flee(const eFlee& flee);
    bool checkForAttackIncrement(const float by);
    bool moveToEnemy(const float maxDist);
    void wait(const float time);
    void walkAround(const float time);
    bool lookForAttackTarget();

    void setAttackDist(const float dist) { mAttackDist = dist; }

    int mRunAnimId;
    int mWalkAnimId;
    int mWalkReadyAnimId;
    int mStandAnimId;
    int mStandReadyAnimId;
private:
    float mAttackDist = 10.f;
    eUnitStrategy mStrategy = eUnitStrategy::attack;

    std::optional<eFlee> mFleeFrom;

    constexpr static const float sAttackCounterMax = 25.f;
    float mAttackCounter = eRand::randF(0.f, sAttackCounterMax);
    uint32_t mAttacking = 0;

    float mTanChance = 0.f;
    float mTanDistance = 0.f;
};

#endif // EUNITBASEACTION_H
