#include "eSlayerMissiles/enovaincrementer.h"

#include <eSlayerHelpers/enova.h>
#include <eSlayerHelpers/efixedsizesetareas.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eobject.h>

eNovaIncrementer::eNovaIncrementer(
    eFixedSizeSetAreas& unitAreas) :
    mUnitAreas(unitAreas) {}

void eNovaIncrementer::initialize(
    const eHasObjects& hasObjects,
    const eGetObjects& getObjects,
    const eGetObject& getObject,
    const eRemoveNova& removeNova,
    const eGetUnit& getUnit,
    const eHitAction& hitAction) {
    mHasObjects = hasObjects;
    mGetObjects = getObjects;
    mGetObject = getObject;
    mRemoveNova = removeNova;
    mGetUnit = getUnit;
    mHitAction = hitAction;
}

bool circleIntersectsTile(
    const float cx,
    const float cy,
    const float radius,
    const int tileX,
    const int tileY) {
    const float nearestX = std::max((float)tileX,
        std::min(cx, (float)tileX + 1.f));

    const float nearestY = std::max((float)tileY,
        std::min(cy, (float)tileY + 1.f));

    const float dx = cx - nearestX;
    const float dy = cy - nearestY;

    return dx*dx + dy*dy <= radius*radius;
}

bool eNovaIncrementer::increment(
    eNova& n, const float by) const {
    const float oldRadius = n.fRadius;
    const float newRadius = oldRadius + n.fSpeed*by;
    n.fRadius = newRadius;

    const auto& c = n.fCenter;
    const int minX = floor(c.fX - newRadius);
    const int maxX = floor(c.fX + newRadius);

    const int minY = floor(c.fY - newRadius);
    const int maxY = floor(c.fY + newRadius);

    for(int y = minY; y <= maxY; y++) {
        for(int x = minX; x <= maxX; x++) {
            const bool insideNew = circleIntersectsTile(
                c.fX, c.fY, newRadius, x, y);
            if(!insideNew) continue;

            const auto area = mUnitAreas.posArea(ePoint{x, y});
            if(mUnitAreas.hasArea(area)) {
                const auto& units = mUnitAreas.at(area);
                for(const int charId : units) {
                    const auto u = mGetUnit(charId);
                    if(!u) continue;
                    if(u->fHealth <= 0) continue;
                    if(!eTeams::areEnemies(u->fTeamId, n.fTeamId)) continue;
                    if(mHitAction) mHitAction(n, *u);
                }
            }

            const bool insideOld = circleIntersectsTile(
                c.fX, c.fY, oldRadius, x, y);
            if(insideOld) continue;

            const bool r = mHasObjects(x, y);
            if(!r) continue;
            const auto& objIds = mGetObjects(x, y);
            for(const auto objId : objIds) {
                const auto obj = mGetObject(x, y, objId);
                if(!obj) continue;
                n.obsticle1(obj->fPos, 0.5f*obj->fSize);
            }
        }
    }

    if(n.fIntervals.empty() ||
       newRadius >= n.fMaxRadius) {
        mRemoveNova(n);
        return true;
    }
    return false;
}
