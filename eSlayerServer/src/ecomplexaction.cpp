#include "ecomplexaction.h"

#include "eserverunit.h"
#include "eserverarea.h"
#include "eattackaction.h"
#include "ehitrecoveryaction.h"
#include "eblockaction.h"
#include "edieaction.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/erunsettings.h>
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
        const float dmg = data.fDamage.fPhysical +
                          data.fDamage.fFire +
                          data.fDamage.fCold +
                          data.fDamage.fLightning;
        mUnit.fHealthF = std::max(0.f, mUnit.fHealthF - dmg);
        mUnit.fHealth = std::ceil(mUnit.fHealthF);
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
    const auto& data = mUnit.data();
    const auto& uskill = data.getSkill(target.fSkill);
    const auto& skill = eSkills::sSkills.get(uskill.fSkillId);
    switch(target.fType) {
    case eAttackTargetType::character: {
        const auto u = mArea.unit(target.fChar);
        if(!u) return false;
        if(skill.fType == eSkillType::attack) {
            return meeleAttack(*u, uskill);
        } else if(skill.fType == eSkillType::missile ||
                  skill.fType == eSkillType::wall) {
            return spawnMissile(u->fPos, uskill, 1);
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
                getHit(*target);
            };
            const auto attack = eAttackAction::sCreate(
                mUnit, mArea, uskill.fCastAnimIds,
                eAttackType::attack, a);
            if(attack) setChild(attack);
            return attack.get();
        } else if(skill.fType == eSkillType::missile ||
                  skill.fType == eSkillType::wall) {
            return spawnMissile(target.fPos, uskill, 1);
        }

    } break;
    case eAttackTargetType::none: {
        return false;
    } break;
    }
    return true;
}

bool eComplexAction::meeleAttack(
    const eServerUnit& u, const eUnitSkill& skill) {
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
        getHit(*target);
    };
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, skill.fCastAnimIds,
        eAttackType::attack, a);
    if(attack) setChild(attack);
    return attack.get();
}

bool eComplexAction::getHit(eServerUnit& target) {
    const float hitChance = eServerUnit::sHitChance(target, mUnit);
    eHitData data;
    data.fHitChance = hitChance;
    data.fBlockMultiplier = 1.f;
    data.fDamage.fPhysical = 10.f;
    return target.getHit(data);
}

int piercedFromPierceChance(const float p) {
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

bool eComplexAction::spawnMissile(const ePointF& to,
                                  const eUnitSkill& uskill,
                                  const int levelId) {
    const auto& skill = eSkills::sSkills.get(uskill.fSkillId);
    const auto& level = skill.fLevels[levelId];
    const auto a = [this, to, skill, level]() {
        struct eMissileData {
            ePointF fPos;
            ePointF fTo;
            int fToPierce;
        };
        std::vector<eMissileData> missiles;
        auto baseDir = ePointF::vector(to, mUnit.fPos);
        if(skill.fType == eSkillType::missile) {
            const float angleMult = std::clamp(1.f - 3.f*baseDir.length()/skill.fRangeTime, 0.1f, 1.f);
            const float maxAngle = skill.fMaxAngle*angleMult;
            float angle = level.fMissiles == 1 ? 0.f : -0.5f*maxAngle;
            for(int i = 0; i < level.fMissiles; i++) {
                auto dir = baseDir;
                if(angle != 0.f) dir.rotate(angle);
                auto& md = missiles.emplace_back();
                const int max = std::numeric_limits<uint8_t>::max();
                const int pierced = piercedFromPierceChance(level.fPierceChance);
                md.fToPierce = 1 + std::min(max, pierced);
                auto castDispl = dir;
                castDispl.normalize(0.5*skill.fRadius);
                md.fPos = mUnit.fPos + castDispl;
                md.fTo = mUnit.fPos + dir;
                if(level.fMissiles > 1) {
                    angle += maxAngle/(level.fMissiles - 1);
                }
            }
        } else if(skill.fType == eSkillType::wall) {
            eVec2f perp(-baseDir.y, baseDir.x);
            perp.normalize(2*skill.fRadius);
            ePointF pt = to - perp * (level.fMissiles/2);
            for(int i = 0; i < level.fMissiles; i++) {
                auto& md = missiles.emplace_back();
                md.fToPierce = 0;
                md.fPos = pt;
                md.fTo = pt;
                pt = pt + perp;
            }
        }
        for(const auto& md : missiles) {
            const auto m = std::make_shared<eServerMissile>();
            m->fType = skill.fMissileId;
            m->fTeamId = mUnit.fTeamId;
            m->fToPierce = md.fToPierce;
            m->fSpeed = skill.fSpeed;
            m->fRemDistTime = skill.fRangeTime;
            m->fPathType = skill.fPathId;
            m->fFrom = mUnit.fPos;
            m->fRadius = skill.fRadius;
            m->fPos = md.fPos;
            m->fTo = md.fTo;
            const bool continuousDamage = skill.fType == eSkillType::wall;
            m->fContinuousDamage = continuousDamage;
            m->fTime = 0.f;
            struct eCharSkipper {
                float fTime = 0.f;
                std::set<int> fChars;
            };

            const std::shared_ptr<eCharSkipper> skip =
                continuousDamage ?
                std::make_shared<eCharSkipper>() :
                nullptr;
            const auto damage = continuousDamage ?
                level.fDamage/eRunSettings::sFPS :
                level.fDamage;
            m->fHitAction = [m, damage, skip](eServerUnit& u) {
                if(skip) {
                    if(skip->fTime < m->fTime) {
                        skip->fChars.clear();
                        skip->fTime = m->fTime;
                    } else {
                        if(skip->fChars.find(u.fCharId) != skip->fChars.end()) {
                            return;
                        }
                    }
                    skip->fChars.emplace(u.fCharId);
                }
                eHitData data;
                data.fBlockMultiplier = 0.f;
                data.fHitChance = 1.f;
                data.fDamage = damage;
                u.getHit(data);
            };
            mArea.addMissile(m);
        }
    };
    const eAttackType attackType =
        skill.fType == eSkillType::attack ?
            eAttackType::attack : eAttackType::cast;
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, uskill.fCastAnimIds,
        attackType, a);
    if(attack) setChild(attack);
    return attack.get();

    return true;
}
