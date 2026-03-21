#include "ecomplexaction.h"

#include "eserverunit.h"
#include "eserverarea.h"
#include "eattackaction.h"
#include "ehitrecoveryaction.h"
#include "eblockaction.h"
#include "edieaction.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerMissiles/emissileincrement.h>

void eComplexAction::increment(const float by) {
    if(mChild) {
        mChild->increment(by);
    } else {
        decide();
    }
}

void eComplexAction::setChild(const std::shared_ptr<eUnitAction>& c) {
    mChild = c;
    if(c) c->setParent(this);
}

bool eComplexAction::getHit(const eHitData& data) {
    if(eRand::randF() > data.fHitChance) {
        return false;
    }
    const float blockChance = data.fBlockMultiplier*mUnit.blockChance();
    if(eRand::randF() < blockChance) {
        const auto a = eBlockAction::sCreate(mUnit, mArea);
        if(a) mUnit.setChildAction(a);
    } else {
        const int dmg = 10;
        mUnit.fHealth = std::max(0, mUnit.fHealth - dmg);
        if(mUnit.fHealth <= 0) {
            const auto die = std::make_shared<eDieAction>(mUnit, mArea);
            mUnit.setChildAction(die);
        } else if(dmg >= mUnit.fMaxHealth/12.f) {
            const auto a = eHitRecoveryAction::sCreate(mUnit, mArea);
            if(a) mUnit.setChildAction(a);
        }
    }
    return true;
}

bool eComplexAction::attack(const eAttackData& target) {
    const auto& skill = eSkills::sSkills.get(target.fSkill);
    switch(target.fType) {
    case eAttackTargetType::character: {
        const auto u = mArea.unit(target.fChar);
        if(!u) return false;
        if(skill.fType == eSkillType::attack) {
            return attack(*u);
        } else if(skill.fType == eSkillType::missile) {
            return spawnMissile(u->fPos, skill);
        }
    } break;
    case eAttackTargetType::position: {
        if(skill.fType == eSkillType::attack) {
            auto dir = ePointF::vector(target.fPos, mUnit.fPos);
            dir.normalize(mUnit.fRadius + 0.2f);
            const auto targetPos = mUnit.fPos + dir;
            const auto a = [this, targetPos]() {
                const auto target = mArea.unit(targetPos);
                if(!target) return;
                if(target.get() == &mUnit) return;
                const float hitChance = eServerUnit::sHitChance(*target, mUnit);
                eHitData data;
                data.fHitChance = hitChance;
                data.fBlockMultiplier = 1.f;
                target->getHit(data);
            };
            const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
            if(attack) setChild(attack);
            return attack.get();
        } else if(skill.fType == eSkillType::missile) {
            return spawnMissile(target.fPos, skill);
        }

    } break;
    case eAttackTargetType::none: {
        return false;
    } break;
    }
    return true;
}

bool eComplexAction::attack(const eServerUnit& u) {
    if(u.fHealth <= 0) return false;
    const float dist = ePointF::distance(mUnit.fPos, u.fPos);
    const float attackDist = 0.5f*(mUnit.fRadius + u.fRadius);
    if(dist > attackDist) return false;
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const int targetId = u.fCharId;
    const auto a = [this, targetId]() {
        const auto target = mArea.unit(targetId);
        if(!target) return;
        const float hitChance = eServerUnit::sHitChance(*target, mUnit);
        eHitData data;
        data.fHitChance = hitChance;
        data.fBlockMultiplier = 1.f;
        target->getHit(data);
    };
    const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
    if(attack) setChild(attack);
    return attack.get();
}

int piercedFromPierceChance(const float p) {
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

bool eComplexAction::spawnMissile(const ePointF& to,
                                  const eSkill& skill) {
    const auto a = [this, to, skill]() {
        const auto m = std::make_shared<eServerMissile>();
        m->fType = skill.fMissileId;
        m->fTeamId = mUnit.fTeamId;
        const int max = std::numeric_limits<uint8_t>::max();
        const int pierced = piercedFromPierceChance(skill.fPierceChance);
        m->fPierced = std::min(max, pierced);
        m->fSpeed = skill.fSpeed;
        m->fRemDist = skill.fRange;
        m->fPathType = skill.fPathId;
        m->fFrom = mUnit.fPos;
        m->fRadius = skill.fRadius;
        auto dir = ePointF::vector(to, mUnit.fPos);
        dir.normalize(0.5*m->fRadius);
        m->fPos = mUnit.fPos + dir;
        m->fTo = to;
        m->fHitAction = [](eServerUnit& u) {
            eHitData data;
            data.fBlockMultiplier = 0.f;
            data.fHitChance = 1.f;
            u.getHit(data);
        };
        mArea.addMissile(m);
    };
    const auto attack = eAttackAction::sCreate(mUnit, mArea, a);
    if(attack) setChild(attack);
    return attack.get();

    return true;
}
