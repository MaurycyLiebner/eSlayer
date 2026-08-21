#include "eserverarea.h"

#include "actions/ewaitaction.h"

#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>

uint32_t eServerArea::findOtherTarget(
    const eServerUnit& u,
    const float range,
    const std::set<uint32_t>& skip) {
    uint32_t result = 0;
    const auto uid = u.fCharId;
    const auto& upos = u.fPos;
    iterateOverUnitsClamped(upos, range, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto id = u->fCharId;
        if(id == uid) return false;
        if(skip.count(id) > 0) return false;
        result = id;
        return true;
    });
    return result;
}

uint32_t eServerArea::findMinOtherTarget(
    const eServerUnit& u,
    const float range,
    const std::map<uint32_t, uint8_t>& skip) {
    uint32_t result = 0;
    uint8_t min = 255;
    const auto uid = u.fCharId;
    const auto& upos = u.fPos;
    iterateOverUnitsClamped(upos, range, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto id = u->fCharId;
        if(id == uid) return false;
        const auto it = skip.find(id);
        if(it == skip.end()) {
            min = 0;
            result = id;
            return true;
        }
        const auto imin = it->second;
        if(imin < min) {
            min = imin;
            result = id;
        }
        return false;
    });
    return result;
}

int piercedFromPierceChance(const float p) {
    if(p <= 0.f) return 0;
    if(p >= 1.f) return std::numeric_limits<int>::max();
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

void eServerArea::spawnMissile(const ePointF& to,
                               const eSkill& skill,
                               const eHitData& data,
                               const int nMissiles,
                               const float pierceChance,
                               const int missileId,
                               const float range,
                               const float radius,
                               const float time,
                               const bool continuousDamage,
                               const int consecutive) {
    const auto skillType = skill.fType;
    const auto twinBehaviour = skill.fTwinBehaviour;
    const bool avoid = twinBehaviour != eTwinBehaviour::none;
    auto baseDir = ePointF::vector(to, data.fFrom);
    if(baseDir.length() < 0.001f) baseDir = eVec2f::random();
    struct eMissileData {
        ePointF fPos;
        ePointF fTo;
        int fToPierce;
        int fMissileId;
        float fRange;
        float fTime;
        eDamage fDamage;
    };
    std::vector<eMissileData> missiles;
    const auto spawnMissiles = [&](const int missileId,
                                   const float range) {
        float maxAngle = skill.fMaxAngle;
        if(nMissiles > 1 && maxAngle == 0.f) {
            maxAngle = 30.f*(nMissiles - 1);
        }
        if(skill.fAngleAdjust) {
            if(range > 0.f) {
                const float len = baseDir.length();
                const float multBase = 1.f - 3.f*len/range;
                const float angleMult = std::clamp(multBase, 0.1f, 1.f);
                maxAngle *= angleMult;
            }
        }
        float angle = nMissiles == 1 ? 0.f : -0.5f*maxAngle;
        for(int i = 0; i < nMissiles; i++) {
            auto dir = baseDir;
            if(angle != 0.f) dir.rotate(angle);
            auto& md = missiles.emplace_back();
            const int max = std::numeric_limits<uint8_t>::max();
            const int pierced = piercedFromPierceChance(pierceChance);
            md.fToPierce = std::min(max, 1 + pierced);
            md.fPos = data.fFrom;
            md.fTo = data.fFrom + dir;
            md.fMissileId = missileId;
            md.fRange = range;
            md.fDamage = data.fDamage;
            if(nMissiles > 1) {
                angle += maxAngle/(nMissiles - 1);
            }
        }
    };
    if(skillType == eSkillType::missile) {
        spawnMissiles(missileId, range);
    } else if(skillType == eSkillType::wall) {
        eVec2f perp(-baseDir.y, baseDir.x);
        perp.normalize(2*radius);
        ePointF pt = to - perp * (nMissiles/2);
        for(int i = 0; i < nMissiles; i++) {
            auto& md = missiles.emplace_back();
            md.fToPierce = 0;
            md.fPos = pt;
            md.fTo = pt;
            md.fMissileId = skill.fMissileId;
            md.fTime = skill.fTime;
            md.fRange = 0.f;
            md.fDamage = data.fDamage;
            pt = pt + perp;
        }
    } else {
        spawnMissiles(missileId, range);
    }
    std::vector<std::shared_ptr<eServerMissile>> twins;
    for(const auto& md : missiles) {
        const auto m = std::make_shared<eServerMissile>();
        if(avoid) twins.emplace_back(m);
        auto& mref = *m;
        mref.fType = md.fMissileId;
        mref.fTeamId = data.fAttackTeamId;
        mref.fToPierce = md.fToPierce;
        mref.fSpeed = skill.fSpeed;

        mref.fTotalDist = md.fRange;
        mref.fTotalTime = md.fTime;
        mref.fRemDist = mref.fTotalDist;
        mref.fRemTime = mref.fTotalTime;

        mref.fPathType = skill.fPathId;
        mref.fFrom = data.fFrom;
        mref.fRadius = radius;
        mref.fPos = md.fPos;
        mref.fTo = md.fTo;
        mref.fContinuousDamage = continuousDamage;
        mref.fConsecutive = consecutive;
        mref.fEnemyFindRange = skill.fMissileEnemyFindRange;
        mref.fTwinBehaviour = twinBehaviour;
        mref.fTime = 0.f;

        struct eCharSkipper {
            float fTimeSkip = 0.f;
            float fTime = 0.f;
            std::set<uint32_t> fChars;
        };

        std::shared_ptr<eCharSkipper> skip;
        if(continuousDamage) {
            skip = std::make_shared<eCharSkipper>();
        } else if(consecutive > 0) {
            skip = std::make_shared<eCharSkipper>();
            skip->fTimeSkip = 1000.f;
        }
        m->fHitAction = [data, skip](
            eServerUnit& u, const eServerMissile& m) {
            if(skip) {
                auto& c = skip->fChars;
                if(skip->fTime < m.fTime) {
                    c.clear();
                    skip->fTime = m.fTime + skip->fTimeSkip;
                } else {
                    if(c.find(u.fCharId) != c.end()) {
                        return;
                    }
                }
                c.emplace(u.fCharId);
            }
            u.getHit(data);
        };
        addMissile(m);
    }

    for(const auto& m : twins) {
        for(const auto& twin : twins) {
            if(m == twin) continue;
            const auto id = twin->fId;
            m->fTwinMissiles.emplace(id);
        }
    }
}

void eServerArea::spawnArea(const ePointF& to,
                            const eSkill& skill,
                            eHitData data,
                            const float radius,
                            const int missileId) {
    const int area = skill.fAreaMissileId;
    if(area <= 0) return;
    const auto a = std::make_shared<eServerSkillArea>();
    data.fDamage = data.fDamage/25.f;
    data.fHeal /= 25.f;
    a->fIncrement = [this, data, to, radius]() {
        const auto team = data.fAttackTeamId;
        iterateOverUnitsClamped(to, radius,
        [&](const std::shared_ptr<eServerUnit>& u) {
            if(u->fHealth <= 0) return false;
            const auto uteam = u->fTeamId;
            const auto& pos = u->fPos;
            if(team == uteam) {
                u->restoreHealth(data.fHeal);
            } else {
                if(!eTeams::areEnemies(team, uteam)) return false;
                u->getHit(data);
            }
            return false;
        });
    };
    a->fMissileId = area;
    a->fRemTime = skill.fTime;
    a->fRadius = radius;
    a->fPos = to;
    addSkillArea(a);
}

void eServerArea::spawnNova(const eSkill& skill,
                            const eHitData& data,
                            const float radius,
                            const bool continuousDamage) {
    const auto n = std::make_shared<eServerNova>();
    n->fTeamId = data.fAttackTeamId;
    n->fMissileType = skill.fMissileId;
    if(skill.fTargetCorpse) {
        const auto corpseId = nearestCorpse(data.fTo);
        if(!corpseId) return;
        const auto c = unit(corpseId);
        n->fCenter = c->fPos;
        c->explodeCorpse();
    } else {
        n->fCenter = data.fFrom;
    }
    n->fRadius = 0.f;
    n->fMaxRadius = radius;
    n->fSpeed = skill.fSpeed;
    n->fNMissiles = skill.fNovaMissiles;

    struct eCharSkipper {
        std::set<int> fChars;
    };

    const std::shared_ptr<eCharSkipper> skip =
        !continuousDamage ?
            std::make_shared<eCharSkipper>() :
            nullptr;
    n->fHitAction = [data, skip](eServerUnit& u) {
        if(skip) {
            auto& c = skip->fChars;
            if(c.find(u.fCharId) != c.end()) {
                return;
            }
            c.emplace(u.fCharId);
        }
        u.getHit(data);
    };
    addNova(n);
}

std::shared_ptr<eServerUnit>
eServerArea::summon(eServerUnit& by,
                    const uint32_t corpseId,
                    const int unitId,
                    const int maxCount,
                    const std::vector<eModifier>& mods) {
    const auto corpse = unit(corpseId);
    if(!corpse) return nullptr;
    const auto& to = corpse->fPos;

    const auto u = summon(by, to, unitId, maxCount, mods);
    if(u) {
        const auto raise = eWaitAction::sCreateRaise(
            *u, *this);
        if(raise) u->setChildAction(raise);
    }

    planRemoveUnit(corpseId);

    return u;
}

void eServerArea::raise(eServerUnit& by,
                        const uint32_t corpseId,
                        const int maxCount,
                        const std::vector<eModifier>& mods,
                        const bool follow) {
    const auto corpse = unit(corpseId);
    if(!corpse) return;
    const bool c = corpse->isCorpse();
    if(!c) return;
    auto& followers = by.followers();
    const auto raised = eServerArea::raised(by);
    if(maxCount > 0 && raised.size() >= maxCount) {
        const uint32_t removeCharId = raised[0];
        planRemoveUnit(removeCharId);
        followers.remove(removeCharId);
    }
    corpse->raise();
    corpse->setTeamId(by.fTeamId);
    corpse->addBoost(mods, eBoostCurseType::summon, true);
    if(follow) {
        const auto byPtr = unit(by.fCharId);
        iniSetupFollowerAction(corpse, byPtr);
    }
    const auto raise = eWaitAction::sCreateRaise(
        *corpse, *this);
    if(raise) corpse->setChildAction(raise);
}

std::shared_ptr<eServerUnit>
eServerArea::summon(eServerUnit& by,
                    ePointF to,
                    const int unitId,
                    const int maxCount,
                    const std::vector<eModifier>& mods) {
    const bool r = findPlaceForUnit(to, to);
    if(!r) return nullptr;
    auto& followers = by.followers();
    const auto summoned = eServerArea::summoned(by, unitId);
    if(maxCount > 0 && summoned.size() >= maxCount) {
        const uint32_t removeCharId = summoned[0];
        planRemoveUnit(removeCharId);
        followers.remove(removeCharId);
    }
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto modelParts = data.randomModelParts();

    const auto u = std::make_shared<eServerUnit>(
        eUnitType::summoned, data, unitId, *this);
    const uint32_t charId = eServerUnit::sNextCharId++;
    followers.add(charId);
    iniSetupUnit(u, charId, by.fTeamId, to,
                 unitId, udata, data, modelParts);
    u->addBoost(mods, eBoostCurseType::permanent, false);
    {
        const int schoice = u->addSkill();
        u->setSkillId(schoice, 0, false);
    }
    u->recalculateStats();
    u->recalculateAuras();

    const auto byPtr = unit(by.fCharId);
    iniSetupFollowerAction(u, byPtr);

    return u;
}

bool eServerArea::castChance(
    eServerUnit& by,
    const eSkillStats& o,
    const eWeaponChoice wchoice,
    const ePointF& to) {
    const bool r = eRand::randChance(o.fCastChance);
    if(!r) return false;
    cast(by, o, wchoice, to);
    return true;
}

void eServerArea::cast(eServerUnit& by,
                       const eSkillStats& o,
                       const eWeaponChoice wchoice,
                       const ePointF& to) {
    const auto& skill = eSkills::sSkills.get(o.fSkillId);
    eHitData data;
    by.hitData(o, wchoice, data);
    switch(skill.fType) {
    case eSkillType::missile:
    case eSkillType::wall: {
        const int nMissiles = by.skillCount(o, wchoice);
        const float pierceChance = by.pierceChance(o, wchoice);
        const int missileId = by.missileId(o, wchoice);
        const float missileRange = by.missileRange(o, wchoice);
        const float missileTime = by.missileTime(o, wchoice);
        const float radius = by.radius(o, wchoice);
        const bool continuousDamage = skill.fType == eSkillType::wall;
        const int consecutive = by.consecutive(o, wchoice);
        spawnMissile(to, skill, data,
                     nMissiles, pierceChance, missileId,
                     missileRange, radius, missileTime,
                     continuousDamage, consecutive);
    } break;
    case eSkillType::nova: {
        const float radius = by.radius(o, wchoice);
        const bool continuousDamage = false;
        spawnNova(skill, data, radius, continuousDamage);
    } break;
    case eSkillType::summon: {
        const int maxCount = by.skillCount(
            o, eWeaponChoice::left);
        const int unitId = skill.fUnitId;
        const auto summoned = eServerArea::summoned(by, unitId);
        const int currCount = summoned.size();
        if(maxCount > currCount) {
            summon(by, to, unitId, maxCount, {});
        }
    } break;
    case eSkillType::raise: {
        const int maxCount = by.skillCount(
            o, eWeaponChoice::left);
        uint32_t corpseId = 0;
        const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
            const bool c = u->isCorpse();
            if(!c) return false;
            corpseId = u->fCharId;
            return true;
        };
        iterateOverUnitsClamped(by.fPos, 5.f, iter);
        raise(by, corpseId, maxCount, {}, true);
    } break;
    case eSkillType::area:
    case eSkillType::boostCurse: {
        const float radius = by.radius(o, wchoice);
        spawnArea(to, skill, data, radius, skill.fMissileId);
    } break;
    case eSkillType::attack:
    case eSkillType::dualAttack:
    case eSkillType::aura:
    case eSkillType::shoot:
    case eSkillType::kick:
    case eSkillType::smite:
    case eSkillType::passive:
    case eSkillType::throw_:
        break;
    }

    switch(o.fExplode) {
    case eExplodeType::none:
        break;
    default:
        by.die(o.fExplode);
        break;
    }
}