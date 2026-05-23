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
    const eHitAction& hitAction) {
    mObstacle = obstacle;
    mRemoveMissile = removeMissile;
    mGetUnit = getUnit;
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
        m.fEnemy = false;

        for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
            for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
                const eArea area{ax, ay};
                if(!mUnitAreas.hasArea(area)) continue;
                const auto& units = mUnitAreas.at(area);
                for(const int charId : units) {
                    const auto u = mGetUnit(charId);
                    if(!u) continue;
                    if(u->fHealth <= 0) continue;
                    if(!eTeams::areEnemies(u->fTeamId, m.fTeamId)) continue;
                    const float dist = ePointF::distance(u->fPos, m.fPos);
                    if(dist < closestUnit) {
                        closestUnit = dist;
                        m.fEnemyPos = u->fPos;
                        m.fEnemy = true;
                    }
                }
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
            for(const int charId : units) {
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
