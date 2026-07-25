#include "eSlayerMissiles/emissileincrementer.h"

#include "eSlayerMissiles/emissileincrement.h"
#include "eSlayerMissiles/emissilecollision.h"

#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/efixedsizesetareas.h>
#include <eSlayerHelpers/eunitdata.h>

eMissileIncrementer::eMissileIncrementer(
    eFixedSizeSetAreas& unitAreas) :
    mUnitAreas(unitAreas) {}

void eMissileIncrementer::initialize(
    const eObstacle& obstacle,
    const eRemoveMissile& removeMissile,
    const eGetUnit& getUnit,
    const eGetMissile& getMissile,
    const eHitAction& hitAction) {
    mObstacle = obstacle;
    mRemoveMissile = removeMissile;
    mGetUnit = getUnit;
    mGetMissile = getMissile;
    mHitAction = hitAction;
}

bool eMissileIncrementer::increment(eMissile& m, const float by) const {
    if(m.fEnemyFindRange > 0.f) {
        const float aabbMinX = m.fPos.fX - m.fEnemyFindRange;
        const float aabbMaxX = m.fPos.fX + m.fEnemyFindRange;
        const float aabbMinY = m.fPos.fY - m.fEnemyFindRange;
        const float aabbMaxY = m.fPos.fY + m.fEnemyFindRange;

        // Determine which unit areas overlap this AABB
        const auto areaMin = mUnitAreas.posArea(ePointF{aabbMinX, aabbMinY});
        const auto areaMax = mUnitAreas.posArea(ePointF{aabbMaxX, aabbMaxY});

        float closestUnit = m.fEnemyFindRange + 0.1f;

        std::set<uint32_t> skip;
        const bool alwaysAvoid = m.fTwinBehaviour == eTwinBehaviour::alwaysAvoid;
        const bool tryAvoid = m.fTwinBehaviour == eTwinBehaviour::tryAvoid;

        float optionalClosest = m.fEnemyFindRange + 0.1f;
        ePointF optionalPos;
        uint32_t optionalEnemy = 0;

        const auto tryUnit = [&](const uint32_t charId) {
            const auto u = mGetUnit(charId);
            if(!u) return false;
            const auto& uref = *u;
            if(uref.fHealth <= 0) return false;
            if(!eTeams::areEnemies(uref.fTeamId, m.fTeamId)) return false;
            const bool twinOccupied = skip.count(charId) > 0;
            if(twinOccupied && alwaysAvoid) return false;
            const float dist = ePointF::distance(uref.fPos, m.fPos);
            if(twinOccupied && tryAvoid) {
                if(dist < optionalClosest) {
                    optionalClosest = dist;
                    optionalPos = uref.fPos;
                    optionalEnemy = charId;
                    return true;
                }
            } else {
                if(dist < closestUnit) {
                    closestUnit = dist;
                    m.fEnemyPos = uref.fPos;
                    m.fEnemy = charId;
                    return true;
                }
            }
            return false;
        };

        const bool r = m.fEnemy > 0 && tryUnit(m.fEnemy);
        if(!r) {
            m.fEnemy = 0;

            for(const auto mid : m.fTwinMissiles) {
                const auto mptr = mGetMissile(mid);
                if(!mptr) continue;
                const auto& mref = *mptr;
                if(mref.fEnemy <= 0) continue;
                skip.emplace(mref.fEnemy);
            }

            for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
                for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
                    const eArea area{ax, ay};
                    if(!mUnitAreas.hasArea(area)) continue;
                    const auto& units = mUnitAreas.at(area);
                    for(const uint32_t charId : units) {
                        tryUnit(charId);
                    }
                }
            }
        }

        if(m.fEnemy <= 0) {
            if(optionalEnemy > 0) {
                m.fEnemy = optionalEnemy;
                m.fEnemyPos = optionalPos;
            }
        }
    }

    const auto oldPos = m.fPos;
    const bool r = eMissileIncrement::increment(m, by);
    if(r) {
        mRemoveMissile(m);
        return true;
    }
    const auto& newPos = m.fPos;
    const bool obstacle = mObstacle(newPos);
    if(obstacle) {
        mRemoveMissile(m);
        return true;
    }

    // Compute AABB of the travel segment, expanded by max
    // possible collision radius to cover all candidate units
    const float maxRadius = m.fRadius + 1.f;
    const float aabbMinX = std::min(oldPos.fX, newPos.fX) - maxRadius;
    const float aabbMaxX = std::max(oldPos.fX, newPos.fX) + maxRadius;
    const float aabbMinY = std::min(oldPos.fY, newPos.fY) - maxRadius;
    const float aabbMaxY = std::max(oldPos.fY, newPos.fY) + maxRadius;

    // Determine which unit areas overlap this AABB
    const auto areaMin = mUnitAreas.posArea(ePointF{aabbMinX, aabbMinY});
    const auto areaMax = mUnitAreas.posArea(ePointF{aabbMaxX, aabbMaxY});

    eMissileCollision::eResult collResult;

    for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
        for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
            const eArea area{ax, ay};
            if(!mUnitAreas.hasArea(area)) continue;
            const auto& units = mUnitAreas.at(area);
            for(const uint32_t charId : units) {
                const auto u = mGetUnit(charId);
                if(!u) continue;
                eMissileCollision::test(oldPos, newPos,
                                        *u, m, collResult);
                if(m.fContinuousDamage && collResult.fHit) {
                    const auto hitUnit = mGetUnit(collResult.fCharId);
                    if(hitUnit && mHitAction) mHitAction(m, *hitUnit);
                }
            }
        }
    }

    if(!m.fContinuousDamage && collResult.fHit) {
        const auto hitUnit = mGetUnit(collResult.fCharId);
        if(hitUnit && mHitAction) mHitAction(m, *hitUnit);
        m.fPierced.emplace(collResult.fCharId);
        if(m.fToPierce == 0) return false;
        if(--m.fToPierce == 0) {
            mRemoveMissile(m);
            return true;
        }
    }
    return false;
}
