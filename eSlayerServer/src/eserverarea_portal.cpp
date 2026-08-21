#include "eserverarea.h"

#include <eSlayerHelpers/eobjectsinfo.h>

#include <eSlayerMapGenerator/emap.h>

bool eServerArea::findPlaceForPortal(
    const ePointF& pos, ePointF& result) const {
    const float x = pos.fX;
    const float y = pos.fY;

    const auto portalType = eObjectsInfo::sObjects.id("portal");

    const auto valid = [&](const ePointF& pos) {
        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                const int x = pos.fX + dx;
                const int y = pos.fY + dy;
                const bool r = mMap->inside(x, y);
                if(!r) continue;
                const auto& oIds = mMap->objects(x, y);
                for(const auto oId : oIds) {
                    const auto& o = mMap->object(oId);
                    if(portalType != o->fObjectType) continue;
                    if(o->inside(pos)) return false;
                }
            }
        }
        return true;
    };

    for(int dist = 0; dist < 5; dist++) {
        const int maxTries = dist == 0 ? 1 : 10;
        for(int i = 0; i <= maxTries; i++) {
            const float dx = eRand::randF(-dist, dist);
            const float dy = eRand::randF(-dist, dist);
            const ePointF tryPos{x + dx, y + dy};
            const bool w = walkable(tryPos);
            if(!w) continue;
            const bool r = valid(tryPos);
            if(!r) continue;
            result = tryPos;
            return true;
        }
    }
    return false;
}

bool eServerArea::spawnPortal(const uint32_t clientId,
                              uint32_t& portalId,
                              eAreaIds& area,
                              ePointF& pos) {
    const auto u = unit(clientId);
    if(!u) return false;
    pos = u->fPos;
    return spawnPortal(pos, portalId, area);
}

bool eServerArea::spawnCampPortal(
    const uint32_t clientId,
    uint32_t& portalId,
    eAreaIds& area,
    ePointF& pos) {
    pos = mMap->portalSpawnPos();
    return spawnPortal(pos, portalId, area);
}

bool eServerArea::spawnPortal(
    ePointF& pos,
    uint32_t& portalId,
    eAreaIds& area) {
    const bool r = findPlaceForPortal(pos, pos);
    if(!r) return false;
    const auto typeId = eObjectsInfo::sObjects.id("portal");
    const auto& info = eObjectsInfo::sObjects.get(typeId);
    const auto o = mMap->addObject(pos, info.fWidth, info.fHeight);
    o->fObjectType = typeId;
    o->fSubtype = 0;
    portalId = o->fObjectId;
    area.fMapId = mMap->id();
    area.fAreaId = mMap->areaAt(pos);
    return true;
}