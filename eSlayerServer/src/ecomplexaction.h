#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/edamage.h>

#include <memory>

struct eSkill;
struct eSkillLevel;
struct eUnitSkill;

struct eHitData {
    float fHitChance;
    float fBlockMultiplier;
    eDamage fDamage;
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
    bool meeleAttack(const eServerUnit& u,
                     const eUnitSkill& skill);
    bool getHit(eServerUnit& target);
private:
    bool spawnMissile(const ePointF& to,
                      const eUnitSkill& uskill,
                      const int levelId);

    std::shared_ptr<eUnitAction> mChild;
};

#endif // ECOMPLEXACTION_H
