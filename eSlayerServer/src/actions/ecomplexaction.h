#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/edamage.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eteamid.h>
#include <eSlayerHelpers/eweaponchoice.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eunithit.h>

#include <memory>

struct eSkill;
struct eUnitSkill;

enum class eAttackResult {
    notReady, attacked, failed
};

struct eHitData {
    uint32_t fAttackerId;
    eTeamId fAttackTeamId;
    eWeaponChoice fWChoice;

    eSourceType fSource = eSourceType::other;
    uint8_t fWeaponType = 0;

    ePointF fFrom;
    ePointF fTo;
    bool fKnockback = false;

    float fLifeSteal = 0.f;
    float fManaSteal = 0.f;

    bool fAlwaysHit = false;
    float fAttackRating = 0.f;
    float fALvl = 1.f;

    float fSplashRange = 0.f;
    float fSplashDmg = 0.f;
    eDamage fDamage;

    float fHeal = 0.f;

    float fColdLength = 0.f;
    float fFreezeLength = 0.f;

    float fImmobilizeLength = 0.f;

    float fManaBurn = 0.f;

    std::vector<eSkillStats> fOnAttack;
    std::vector<eSkillStats> fOnStriking;
    std::vector<eSkillStats> fOnKill;

    std::vector<eBoostCurse> fBoosts;
};

class eComplexAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const float by) override;

    void setChild(const std::shared_ptr<eUnitAction>& c);

    bool getHit(const eHitData& data, const bool splash = true);
protected:
    virtual void decide() = 0;
    eAttackResult attack(const eAttackData& target);
    bool hasChild() const { return mChild.get(); }
private:
    eAttackResult attackBase(const eAttackData& target);
    bool hitData(const int schoice,
                 const eWeaponChoice wchoice,
                 eHitData& data);

    bool meeleAttack(ePointF targetPos,
                     const int schoice,
                     const eWeaponChoice wchoice);
    bool meeleAttack(const eServerUnit& u,
                     const int schoice,
                     const eWeaponChoice wchoice);
    bool spawnMissile(const ePointF& to,
                      const int schoice,
                      const eWeaponChoice wchoice);
    bool spawnArea(const ePointF& to,
                   const int schoice,
                   const eWeaponChoice wchoice);
    bool spawnNova(const ePointF& to,
                   const int schoice,
                   const eWeaponChoice wchoice);
    bool summon(const ePointF& to,
                const int schoice);
    bool raise(const ePointF& to,
               const int schoice);

    bool isAtCamp() const;

    std::shared_ptr<eUnitAction> mChild;

    int mSchoice = 0;
    int mSkillId = 0;
    int mRemTargets = 0;
    float mAttackRange = 0.f;
    std::map<uint32_t, uint8_t> mTargetMap;
};

#endif // ECOMPLEXACTION_H
