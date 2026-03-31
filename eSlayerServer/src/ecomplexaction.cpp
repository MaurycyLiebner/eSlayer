#include "ecomplexaction.h"

#include "eSlayerHelpers/eskillchoice.h"
#include "eattackaction.h"
#include "eblockaction.h"
#include "edieaction.h"
#include "ehitrecoveryaction.h"
#include "eserverarea.h"
#include "eserverunit.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erunsettings.h>
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
        const float dmg = mUnit.takeDamage(data.fDamage);
        if(mUnit.fHealth <= 0) {
            mArea.unitKilled(mUnit);
            const auto die = std::make_shared<eDieAction>(mUnit, mArea);
            mUnit.setChildAction(die);
        } else if(dmg >= mUnit.maxHealth()/12.f) {
            const auto a = eHitRecoveryAction::sCreate(mUnit, mArea);
            if(a) mUnit.setChildAction(a);
        }
    }
    return true;
}

bool eComplexAction::attack(const eAttackData& target) {
    if(!mUnit.skillReady(target.fSkill)) return true;
    mUnit.useSkill(target.fSkill);
    const auto& data = mUnit.data();
    const int skillId = mUnit.skillId(target.fSkill);
    const auto& uskill = data.getSkill(skillId);
    const auto& skill = eSkills::sSkills.get(uskill.fSkillId);
    const auto schoice = target.fSkill;
    const auto wchoice = mUnit.useWeapon(schoice);
    switch(target.fType) {
    case eAttackTargetType::character: {
        const auto u = mArea.unit(target.fChar);
        if(!u) return false;
        if(skill.fType == eSkillType::attack ||
           skill.fType == eSkillType::smite ||
           skill.fType == eSkillType::kick) {
            if(skillId == 0 && mUnit.weaponTypeL() == eWeaponType::ranged) {
                return spawnMissile(u->fPos, schoice, wchoice);
            } else {
                return meeleAttack(*u, schoice, wchoice);
            }
        } else if(skill.fType == eSkillType::missile ||
                  skill.fType == eSkillType::wall ||
                  skill.fType == eSkillType::shoot ||
                  skill.fType == eSkillType::throw_) {
            return spawnMissile(u->fPos, schoice, wchoice);
        }
    } break;
    case eAttackTargetType::position: {
        if(skill.fType == eSkillType::attack ||
           skill.fType == eSkillType::smite ||
           skill.fType == eSkillType::kick) {
            const auto wtype = mUnit.weaponType(wchoice);
            if(skillId == 0 && wtype == eWeaponType::ranged) {
                return spawnMissile(target.fPos, schoice, wchoice);
            } else {
                auto dir = ePointF::vector(target.fPos, mUnit.fPos);
                dir.normalize(mUnit.fRadius + 0.2f + mUnit.weaponMeeleRange());
                const auto targetPos = mUnit.fPos + dir;
                const auto a = [this, targetPos, schoice, wchoice]() {
                    const auto u = mArea.unit(targetPos);
                    if(!u) return;
                    if(u.get() == &mUnit) return;
                    getHit(*u, schoice, wchoice);
                };
                const auto attack = eAttackAction::sCreate(
                    mUnit, mArea, uskill.fCastAnimIds,
                    eAttackType::attack, a,
                    schoice, wchoice);
                if(attack) setChild(attack);
                return attack.get();
            }
        } else if(skill.fType == eSkillType::missile ||
                  skill.fType == eSkillType::wall ||
                  skill.fType == eSkillType::shoot ||
                  skill.fType == eSkillType::throw_) {
            return spawnMissile(target.fPos, schoice, wchoice);
        }

    } break;
    case eAttackTargetType::none: {
        return false;
    } break;
    }
    return true;
}

bool eComplexAction::meeleAttack(
    const eServerUnit& u,
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    if(u.fHealth <= 0) return false;
    const float dist = ePointF::distance(mUnit.fPos, u.fPos);
    const float attackDist = 0.5f*(mUnit.fRadius + u.fRadius) +
                             mUnit.weaponMeeleRange();
    if(dist > attackDist) return false;
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const int targetId = u.fCharId;
    const auto a = [this, targetId, schoice, wchoice]() {
        const auto target = mArea.unit(targetId);
        if(!target) return;
        getHit(*target, schoice, wchoice);
    };
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        eAttackType::attack, a,
        schoice, wchoice);
    if(attack) setChild(attack);
    return attack.get();
}

bool eComplexAction::getHit(eServerUnit& target,
                            const eSkillChoice schoice,
                            const eWeaponChoice wchoice) {
    const float hitChance = eServerUnit::sHitChance(
        target, mUnit, schoice, wchoice);
    eHitData data;
    data.fHitChance = hitChance;
    data.fBlockMultiplier = 1.f;
    data.fDamage = mUnit.attackDamage(schoice, wchoice);
    return target.getHit(data);
}

int piercedFromPierceChance(const float p) {
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

bool eComplexAction::spawnMissile(const ePointF& to,
                                  const eSkillChoice& schoice,
                                  const eWeaponChoice& wchoice) {
    const int skillId = mUnit.skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto a = [this, to, skill, schoice, wchoice]() {
        struct eMissileData {
            ePointF fPos;
            ePointF fTo;
            int fToPierce;
            int fMissileId;
            float fRangeTime;
            eDamage fDamage;
        };
        const auto skillType = skill.fType;
        const bool alwaysHit = skillType == eSkillType::missile ||
                               skillType == eSkillType::wall;
        std::vector<eMissileData> missiles;
        const int nMissiles = mUnit.attackMissiles(schoice, wchoice);
        const float pierceChance = mUnit.pierceChance(schoice, wchoice);
        auto baseDir = ePointF::vector(to, mUnit.fPos);
        if(skillType == eSkillType::missile) {
            const float angleMult = std::clamp(1.f - 3.f*baseDir.length()/skill.fRangeTime, 0.1f, 1.f);
            const float maxAngle = skill.fMaxAngle*angleMult;
            float angle = nMissiles == 1 ? 0.f : -0.5f*maxAngle;
            for(int i = 0; i < nMissiles; i++) {
                auto dir = baseDir;
                if(angle != 0.f) dir.rotate(angle);
                auto& md = missiles.emplace_back();
                const int max = std::numeric_limits<uint8_t>::max();
                const int pierced = piercedFromPierceChance(pierceChance);
                md.fToPierce = 1 + std::min(max, pierced);
                auto castDispl = dir;
                castDispl.normalize(0.5*skill.fRadius);
                md.fPos = mUnit.fPos + castDispl;
                md.fTo = mUnit.fPos + dir;
                md.fMissileId = mUnit.missileId(wchoice, schoice);
                md.fRangeTime = mUnit.missileRangeTime(wchoice, schoice);
                md.fDamage = mUnit.attackDamage(schoice, wchoice);
                if(nMissiles > 1) {
                    angle += maxAngle/(nMissiles - 1);
                }
            }
        } else if(skillType == eSkillType::wall) {
            eVec2f perp(-baseDir.y, baseDir.x);
            perp.normalize(2*skill.fRadius);
            ePointF pt = to - perp * (nMissiles/2);
            for(int i = 0; i < nMissiles; i++) {
                auto& md = missiles.emplace_back();
                md.fToPierce = 0;
                md.fPos = pt;
                md.fTo = pt;
                md.fMissileId = skill.fMissileId;
                md.fRangeTime = skill.fRangeTime;
                md.fDamage = mUnit.attackDamage(schoice, wchoice);
                pt = pt + perp;
            }
        } else {
            const float angleMult = std::clamp(1.f - 3.f*baseDir.length()/skill.fRangeTime, 0.1f, 1.f);
            const float maxAngle = skill.fMaxAngle*angleMult;
            float angle = nMissiles == 1 ? 0.f : -0.5f*maxAngle;
            for(int i = 0; i < nMissiles; i++) {
                auto dir = baseDir;
                if(angle != 0.f) dir.rotate(angle);
                auto& md = missiles.emplace_back();
                const int max = std::numeric_limits<uint8_t>::max();
                const int pierced = piercedFromPierceChance(pierceChance);
                md.fToPierce = 1 + std::min(max, pierced);
                auto castDispl = dir;
                castDispl.normalize(0.5*skill.fRadius);
                md.fPos = mUnit.fPos + castDispl;
                md.fTo = mUnit.fPos + dir;
                md.fMissileId = skill.fMissileId == -1 ?
                    mUnit.missileId(wchoice, schoice) : skill.fMissileId;
                md.fRangeTime = mUnit.weaponRangedRange();
                md.fDamage = mUnit.attackDamage(schoice, wchoice);
                if(nMissiles > 1) {
                    angle += maxAngle/(nMissiles - 1);
                }
            }
        }
        for(const auto& md : missiles) {
            const auto m = std::make_shared<eServerMissile>();
            m->fType = md.fMissileId;
            m->fTeamId = mUnit.fTeamId;
            m->fToPierce = md.fToPierce;
            m->fSpeed = skill.fSpeed;
            m->fRemDistTime = md.fRangeTime;
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
                md.fDamage/eRunSettings::sFPS :
                md.fDamage;
            m->fHitAction = [this, m, damage, skip, alwaysHit,
                             schoice, wchoice](eServerUnit& u) {
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
                if(alwaysHit) {
                    data.fHitChance = 1.f;
                } else {
                    data.fHitChance = eServerUnit::sHitChance(
                        mUnit, u, schoice, wchoice);
                }
                data.fDamage = damage;
                u.getHit(data);
            };
            mArea.addMissile(m);
        }
    };
    const eAttackType attackType =
        skill.fType == eSkillType::attack ||
        skill.fType == eSkillType::smite ||
        skill.fType == eSkillType::kick ?
            eAttackType::attack : eAttackType::cast;
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        attackType, a, schoice, wchoice);
    if(attack) setChild(attack);
    return attack.get();

    return true;
}
