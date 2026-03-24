#include "egameworld.h"

#include "textures/echarstextures.h"

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/evec2.h>

#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

eGameWorld::eProcessResult eGameWorld::processServerData(
    const int clientId,
    const std::shared_ptr<eServer>& server,
    const std::shared_ptr<eUnit>& mainChar,
    SDL_Renderer* const r) {
    eProcessResult result;

    server->requestData(clientId);
    eRequestData data;
    float resultTime;
    const bool b = server->receiveData(clientId, data, resultTime);
    if(!b) return result;

    result.fReceived = true;
    mUnitAreas.clear();
    const auto& units = data.fUnits;
    const auto& missiles = data.fMissiles;
    std::set<int> present;
    for(const auto& u : units) {
        const int charId = u.fCharId;
        const auto area = mUnitAreas.posArea(u.fPos);
        mUnitAreas.emplace(area, charId);
        present.emplace(charId);
        if(charId == clientId) {
            result.fHasMainCharData = true;
            result.fMainCharData = u;
            continue;
        }
        const auto unit = mUnits.get(charId);
        if(unit) {
            reinterpret_cast<eUnitData&>(*unit) = u;
            auto& model = unit->model();
            unit->fPos = u.fPos;
            model.setAngle(u.fAngle);
            model.setAnimation(unit->fAnim, unit->fAnimId, u.fAnimSpeed);
        } else {
            const auto& texs = eCharsTextures::get(u.fTypeId);
            const auto unitModel = texs.generateModel(u.fModelParts, r);

            const auto unit = std::make_shared<eUnit>();
            unit->fRadius = u.fRadius;
            reinterpret_cast<eUnitData&>(*unit) = u;
            eCharUnitModel model;
            model.setCharModel(unitModel);
            model.setAnimation(u.fAnim, u.fAnimId, u.fAnimSpeed);
            model.setAngle(u.fAngle);
            unit->setModel(model);
            unit->fPos = u.fPos;
            mUnits.add(charId, unit);
        }
        if(!result.fAggressive && mainChar->fTeamId != u.fTeamId && u.fHealth > 0) {
            const float dist = ePointF::distance(mainChar->fPos, u.fPos);
            if(dist < 5.f) result.fAggressive = true;
        }
    }
    for(const auto& u : mUnits) {
        const int charId = u->fCharId;
        const auto it = present.find(charId);
        if(it != present.end()) continue;
        mUnits.remove(charId);
    }

    for(const auto& m : missiles) {
        const auto mm = std::make_shared<eExtendedMissile>();
        reinterpret_cast<eMissile&>(*mm) = m;
        mMissiles.add(m.fId, mm);
    }

    return result;
}

eGameWorld::eGameWorld() :
    mUnitAreas(1) {}

void eGameWorld::simulateMissiles(const float by, const std::shared_ptr<eMap>& map) {
    for(const auto& m : mMissiles) {
        const auto oldPos = m->fPos;
        eMissileIncrement::increment(*m, by);
        const auto newPos = m->fPos;
        const auto dir = ePointF::vector(oldPos, newPos);
        m->fAngle = dir.angle();
        if(m->fRemDistTime <= 0.0001f) {
            mMissiles.remove(m->fId);
        } else {
            const auto ipos = m->fPos.floor();
            const bool obsticle = !map->walkable(ipos.fX, ipos.fY);
            if(obsticle) {
                mMissiles.remove(m->fId);
            } else {
                // Compute AABB of the travel segment, expanded by max
                // possible collision radius to cover all candidate units
                const float maxRadius = m->fRadius + 1.f;
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
                        const eUnitArea area{ax, ay};
                        const auto& units = mUnitAreas.at(area);
                        for(const int charId : units) {
                            const auto u = mUnits.get(charId);
                            if(!u) continue;
                            eMissileCollision::test(oldPos, newPos,
                                                    *u, *m, collResult);
                        }
                    }
                }

                if(collResult.fHit) {
                    m->fPierced.emplace(collResult.fCharId);
                    if(m->fToPierce == 0) continue;
                    if(--m->fToPierce == 0) {
                        mMissiles.remove(m->fId);
                    }
                }
            }
        }
    }
}
