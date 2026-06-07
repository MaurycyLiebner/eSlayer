#include "ecomplexaction.h"

#include "eattackaction.h"
#include "eblockaction.h"
#include "ehitrecoveryaction.h"
#include "eknockbackaction.h"
#include "../eserverarea.h"
#include "../eserverunit.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerMissiles/emissileincrement.h>
#include <eSlayerMissiles/enovaincrementer.h>

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
        const eTeamId t1 = u->fTeamId;
        const eTeamId t2 = mUnit.fTeamId;
        if(!eTeams::areEnemies(t1, t2)) return false;
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
        } else if(skill.fType == eSkillType::boostCurse) {
            return spawnArea(u->fPos, schoice, wchoice);
        } else if(skill.fType == eSkillType::summon) {
            return summon(u->fPos, schoice);
        } else if(skill.fType == eSkillType::nova) {
            return spawnNova(u->fPos, schoice, wchoice);
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

                eHitData data;
                hitData(schoice, wchoice, data);

                auto& area = mArea;
                const auto a = [&area, data, targetPos]() {
                    const auto attacker = area.unit(data.fAttackerId);
                    if(!attacker) return;
                    const auto u = area.unit(targetPos, [&](const eServerUnit& u) {
                        if(u.fHealth <= 0) return false;
                        const eTeamId t1 = u.fTeamId;
                        const eTeamId t2 = attacker->fTeamId;
                        if(!eTeams::areEnemies(t1, t2)) return false;
                        return true;
                    });
                    if(u) u->getHit(data);
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
        } else if(skill.fType == eSkillType::boostCurse) {
            return spawnArea(target.fPos, schoice, wchoice);
        } else if(skill.fType == eSkillType::summon) {
            return summon(target.fPos, schoice);
        } else if(skill.fType == eSkillType::nova) {
            return spawnNova(target.fPos, schoice, wchoice);
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
    const int schoice,
    const eWeaponChoice wchoice) {
    if(u.fHealth <= 0) return false;
    const float dist = ePointF::distance(mUnit.fPos, u.fPos);
    const auto& stats = mUnit.stats();
    const float attackRange = 1.01f*stats.attackRange(
        schoice, mUnit.fRadius, u.fRadius);
    if(dist > attackRange) return false;
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const int targetId = u.fCharId;
    eHitData data;
    hitData(schoice, wchoice, data);
    auto& area = mArea;
    const auto a = [&area, wchoice, targetId, data, attackRange]() {
        const auto attacker = area.unit(data.fAttackerId);
        if(!attacker) return;
        const auto target = area.unit(targetId);
        if(!target) return;

        const auto& onAttack = data.fOnAttack;
        const auto to = target->fPos;
        for(const auto& o : onAttack) {
            area.castChance(*attacker, o, wchoice, to);
        }

        const auto& aPos = attacker->fPos;
        const auto& tPos = target->fPos;
        const float dist = ePointF::distance(aPos, tPos);
        if(dist > attackRange) return;
        target->getHit(data);
    };
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        eAttackType::attack, a,
        schoice, wchoice);
    if(attack) setChild(attack);
    return attack.get();
}

bool eComplexAction::getHit(const eHitData& data,
                            const bool splash) {
    bool hit = false;
    const auto attacker = mArea.unit(data.fAttackerId);
    const float hitChance = eServerUnit::sHitChance(
        mUnit, data.fALvl, data.fAttackRating);
    if(!data.fAlwaysHit && eRand::randF() > hitChance) {
        hit = false;
    } else {
        const float blockChance = mUnit.blockChance();
        if(!data.fAlwaysHit && eRand::randF() < blockChance) {
            hit = false;
            const auto a = eBlockAction::sCreate(mUnit, mArea);
            if(a) mUnit.setChildAction(a);
        } else {
            hit = true;

            auto& stats = mUnit.stats();

            const auto& bs = data.fBoosts;
            if(!bs.empty()) {
                for(const auto& b : bs) {
                    mUnit.addTimedBoost(b.fMods, b.fType, b.fMissileId, b.fTime, false);
                }
                mUnit.recalculateStats();
                mUnit.recalculateAuras();
            }

            {
                const auto& onStruck = stats.fOnStruck;
                const auto to = data.fFrom;
                const auto wchoice = eWeaponChoice::left;
                for(const auto& o : onStruck) {
                    mArea.castChance(mUnit, o, wchoice, to);
                }
            }
            mUnit.coldFor(data.fColdLength);
            mUnit.freezeFor(data.fFreezeLength);
            const float dmg = mUnit.takeDamage(data.fDamage);
            stats.fManaF = std::max(0.f, stats.fManaF - dmg*data.fManaBurn);
            if(mUnit.fHealth <= 0) {
                if(attacker && attacker->fHealth > 0) {
                    const auto& onKill = data.fOnKill;
                    const auto to = mUnit.fPos;
                    const auto wchoice = data.fWChoice;
                    for(const auto& o : onKill) {
                        mArea.castChance(*attacker, o, wchoice, to);
                    }
                }
                mUnit.dieAndCast(data.fFrom);
            } else if(data.fKnockback) {
                const float knockbackDist = 1.f;
                const auto dir = ePointF::vector(mUnit.fPos, data.fFrom);
                const auto a = eKnockbackAction::sCreate(
                    mUnit, mArea, dir, knockbackDist);
                if(a) mUnit.setChildAction(a);
            } else if(dmg >= mUnit.maxHealth()/12.f) {
                const auto a = eHitRecoveryAction::sCreate(mUnit, mArea);
                if(a) mUnit.setChildAction(a);
            }
        }
    }
    if(hit && attacker && attacker->fHealth > 0) {
        const auto& onStriking = data.fOnStriking;
        const auto to = mUnit.fPos;
        const auto wchoice = data.fWChoice;
        for(const auto& o : onStriking) {
            mArea.castChance(*attacker, o, wchoice, to);
        }

        const float physDmg = data.fDamage.fPhysical;
        const float lifeSteal = data.fLifeSteal;
        const float lifeStolen = physDmg*lifeSteal;
        attacker->restoreHealth(lifeStolen);

        const float manaSteal = data.fManaSteal;
        const float manaStolen = physDmg*manaSteal;
        attacker->restoreMana(manaStolen);
    }

    if(splash && data.fSplashDmg > 0.f) {
        const float splashRange = 1.f;
        auto newData = data;
        newData.fDamage = newData.fDamage*data.fSplashDmg;
        const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
            if(u->fHealth <= 0) return false;
            const eTeamId t1 = u->fTeamId;
            const eTeamId t2 = data.fAttackTeamId;
            if(!eTeams::areEnemies(t1, t2)) return false;
            if(&*u == attacker.get()) return false;
            const float dist = ePointF::distance(u->fPos, mUnit.fPos);
            if(dist > splashRange) return false;
            u->getHit(newData, false);
            return false;
        };
        mArea.iterateOverUnits(mUnit.fPos, splashRange, iter);
    }
    return hit;
}

bool eComplexAction::hitData(const int schoice,
                             const eWeaponChoice wchoice,
                             eHitData& data) {
    const auto& stats = mUnit.stats();
    const auto& skill = stats.skill(schoice);
    return mUnit.hitData(skill, wchoice, data);
}

bool eComplexAction::spawnMissile(const ePointF& to,
                                  const int schoice,
                                  const eWeaponChoice wchoice) {
    const auto& from = mUnit.fPos;
    const auto dir = ePointF::vector(to, from);
    mUnit.fAngle = dir.angle();
    const int skillId = mUnit.skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int nMissiles = mUnit.skillCount(schoice, wchoice);
    const float pierceChance = mUnit.pierceChance(schoice, wchoice);
    const int missileId = mUnit.missileId(schoice, wchoice);
    const float missileRange = mUnit.missileRange(schoice, wchoice);
    const float missileTime = mUnit.missileTime(schoice, wchoice);
    const float radius = mUnit.radius(schoice, wchoice);
    const bool continuousDamage = skill.fType == eSkillType::wall;

    eHitData data;
    hitData(schoice, wchoice, data);
    if(continuousDamage) {
        data.fDamage = data.fDamage/25.f;
    }

    auto& area = mArea;
    const auto a = [&area, to, &skill, data,
                    nMissiles, pierceChance, missileId,
                    missileRange, radius, missileTime,
                    continuousDamage]() {
        area.spawnMissile(to, skill, data,
                          nMissiles, pierceChance, missileId,
                          missileRange, radius, missileTime,
                          continuousDamage);
    };
    const eAttackType attackType =
        skill.fType == eSkillType::attack ||
        skill.fType == eSkillType::smite ||
        skill.fType == eSkillType::kick ||
        skill.fType == eSkillType::shoot ||
        skill.fType == eSkillType::throw_ ?
            eAttackType::attack : eAttackType::cast;
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        attackType, a, schoice, wchoice);
    if(attack) setChild(attack);
    return attack.get();

    return true;
}

bool eComplexAction::spawnArea(const ePointF& to,
                               const int schoice,
                               const eWeaponChoice wchoice) {
    const auto& from = mUnit.fPos;
    const auto dir = ePointF::vector(to, from);
    mUnit.fAngle = dir.angle();
    const int skillId = mUnit.skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int missileId = mUnit.missileId(schoice, wchoice);
    const float radius = mUnit.radius(schoice, wchoice);

    eHitData data;
    hitData(schoice, wchoice, data);

    auto& area = mArea;
    const auto a = [&area, to, &skill, data, radius, missileId]() {
        area.spawnArea(to, skill, data, radius, missileId);
    };
    const eAttackType attackType = eAttackType::cast;
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        attackType, a, schoice, wchoice);
    if(attack) setChild(attack);
    return attack.get();

    return true;
}

bool eComplexAction::spawnNova(
    const ePointF& to, const int schoice,
    const eWeaponChoice wchoice) {
    const auto& from = mUnit.fPos;
    const auto dir = ePointF::vector(to, from);
    mUnit.fAngle = dir.angle();
    const int skillId = mUnit.skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const float radius = mUnit.radius(schoice, wchoice);
    const bool continuousDamage = false;
    eHitData data;
    hitData(schoice, wchoice, data);
    if(continuousDamage) {
        data.fDamage = data.fDamage/25.f;
    }

    auto& area = mArea;
    const auto a = [&area, &skill, data, radius, continuousDamage]() {
        area.spawnNova(skill, data, radius, continuousDamage);
    };
    const eAttackType attackType = eAttackType::cast;
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        attackType, a, schoice, wchoice);
    if(attack) setChild(attack);
    return attack.get();

    return true;
}

bool eComplexAction::summon(
    const ePointF& to, const int schoice) {
    const auto& from = mUnit.fPos;
    const auto dir = ePointF::vector(to, from);
    mUnit.fAngle = dir.angle();
    const int skillId = mUnit.skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int maxCount = mUnit.skillCount(
        schoice, eWeaponChoice::left);
    const int charId = mUnit.fCharId;
    auto& area = mArea;
    const auto mods = mUnit.skillModifiers(
        schoice, eWeaponChoice::left);
    const auto a = [&area, charId, &skill, mods,
                    dir, from, maxCount]() {
        const auto unit = area.unit(charId);
        if(!unit) return;
        auto toDir = dir;
        if(toDir.length() > skill.fCastRange) {
            toDir.normalize(skill.fCastRange);
        }
        const auto to = from + toDir;
        area.summon(*unit, to, skill.fUnitId, maxCount, mods);
    };
    const auto attack = eAttackAction::sCreate(
        mUnit, mArea, mUnit.castAnims(schoice),
        eAttackType::cast, a, schoice, eWeaponChoice::left);
    if(attack) setChild(attack);
    return attack.get();
}
