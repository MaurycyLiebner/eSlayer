#include "eSlayerMissiles/enovaincrementer.h"

#include <eSlayerHelpers/enova.h>
#include <eSlayerHelpers/efixedsizesetareas.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eobject.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/etile.h>

eNovaIncrementer::eNovaIncrementer(
    eFixedSizeSetAreas& unitAreas) :
    mUnitAreas(unitAreas) {}

void eNovaIncrementer::initialize(
    const eTileInside& tileInside,
    const eGetObjects& getObjects,
    const eGetObject& getObject,
    const eGetTile& getTile,
    const eRemoveNova& removeNova,
    const eGetUnit& getUnit,
    const eHitAction& hitAction) {
    mTileInside = tileInside;
    mGetObjects = getObjects;
    mGetObject = getObject;
    mGetTile = getTile;
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
    if(radius <= 0.f) return false;

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
                    const auto& upos = u->fPos;
                    const float dist = ePointF::distance(c, upos);
                    if(dist > newRadius) continue;
                    const auto vec = ePointF::vector(upos, c);
                    const float angle = vec.angle();
                    const bool r = n.angleInRange(angle);
                    if(!r) continue;
                    if(mHitAction) mHitAction(n, *u);
                }
            }

            if(x < c.fX) {
                const int tx = x + 1;
                const int ty = y;
                const bool insideOld = circleIntersectsTile(
                    c.fX, c.fY, oldRadius, tx, ty);
                if(!insideOld) {
                    const auto& tile = mGetTile(tx, ty);
                    if(tile.fWallTL) {
                        n.obsticle2(ePoint{tx, ty}, ePoint{tx, ty + 1});
                    }
                }
            }

            if(y < c.fY) {
                const int tx = x;
                const int ty = y + 1;
                const bool insideOld = circleIntersectsTile(
                    c.fX, c.fY, oldRadius, tx, ty);
                if(!insideOld) {
                    const auto& tile = mGetTile(tx, ty);
                    if(tile.fWallTR) {
                        n.obsticle2(ePoint{tx, ty}, ePoint{tx + 1, ty});
                    }
                }
            }

            const bool insideOld = circleIntersectsTile(
                c.fX, c.fY, oldRadius, x, y);
            if(insideOld) continue;

            if(x >= c.fX) {
                const int tx = x;
                const int ty = y;
                const auto& tile = mGetTile(tx, ty);
                if(tile.fWallTL) {
                    n.obsticle2(ePoint{tx, ty}, ePoint{tx, ty + 1});
                }
            }

            if(y >= c.fY) {
                const int tx = x;
                const int ty = y;
                const auto& tile = mGetTile(tx, ty);
                if(tile.fWallTR) {
                    n.obsticle2(ePoint{tx, ty}, ePoint{tx + 1, ty});
                }
            }

            const bool r = mTileInside(x, y);
            if(!r) continue;
            const auto& objIds = mGetObjects(x, y);
            for(const auto objId : objIds) {
                const auto obj = mGetObject(objId);
                if(!obj) continue;
                const auto type = obj->fObjectType;
                const auto& info = eObjectsInfo::sObjects.get(type);
                if(!info.fObsticle) continue;
                const auto& pos = obj->fPos;
                const float size = info.fSize;
                n.obsticle1(pos, 0.5f*size);
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
