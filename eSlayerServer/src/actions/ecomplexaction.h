#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/edamage.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eteamid.h>
#include <eSlayerHelpers/eweaponchoice.h>
#include <eSlayerHelpers/estats.h>

#include <memory>

struct eSkill;
struct eSkillLevel;
struct eUnitSkill;

struct eHitData {
    int fAttackerId;
    eTeamId fAttackTeamId;
    eWeaponChoice fWChoice;

    ePointF fFrom;
    bool fKnockback = false;

    float fLifeSteal = 0.f;
    float fManaSteal = 0.f;

    bool fAlwaysHit = false;
    float fAttackRating = 0.f;
    float fALvl = 1.f;

    float fSplashDmg = 0.f;
    eDamage fDamage;

    float fColdLength = 0.f;
    float fFreezeLength = 0.f;

    std::vector<eSkillStats> fOnAttack;
    std::vector<eSkillStats> fOnStriking;
    std::vector<eSkillStats> fOnKill;
};

class eComplexAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const float by) override;

    void setChild(const std::shared_ptr<eUnitAction>& c);

    bool getHit(const eHitData& data, const bool splash = true);
protected:
    virtual void decide() = 0;
    bool attack(const eAttackData& target);
    bool meeleAttack(const eServerUnit& u,
                     const int schoice,
                     const eWeaponChoice wchoice);
    bool hasChild() const { return mChild.get(); }
private:
    bool hitData(const int schoice,
                 const eWeaponChoice wchoice,
                 eHitData& data);

    bool spawnMissile(const ePointF& to,
                      const int schoice,
                      const eWeaponChoice wchoice);
    bool spawnNova(const ePointF& to,
                   const int schoice,
                   const eWeaponChoice wchoice);
    bool summon(const ePointF& to,
                const int schoice);

    std::shared_ptr<eUnitAction> mChild;
};

#endif // ECOMPLEXACTION_H
