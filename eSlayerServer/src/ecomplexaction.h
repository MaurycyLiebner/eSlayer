#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/edamage.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eweaponchoice.h>

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
                     const int schoice,
                     const eWeaponChoice wchoice);
    bool getHit(eServerUnit& target,
                const int schoice,
                const eWeaponChoice wchoice);
    bool hasChild() const { return mChild.get(); }
private:
    bool spawnMissile(const ePointF& to,
                      const int schoice,
                      const eWeaponChoice wchoice);
    bool summon(const ePointF& to,
                const int schoice);

    std::shared_ptr<eUnitAction> mChild;
};

#endif // ECOMPLEXACTION_H
