#include "egameworld.h"

#include "textures/echarstextures.h"

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerMissiles/emissileincrement.h>
#include <eSlayerHelpers/evec2.h>

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
        const auto ipos = u.fPos.floor();
        eUnitTile tile;
        reinterpret_cast<ePoint&>(tile) = ipos;
        mUnitAreas[tile].emplace(charId);
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
            const auto texs = eCharsTextures::get(u.fTypeId);
            const auto unitModel = texs->generateModel(u.fModelParts, r);

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

void eGameWorld::simulateMissiles(
    const float by,
    const std::shared_ptr<eMap>& map) {
    for(const auto& m : mMissiles) {
        const auto oldPos = m->fPos;
        eMissileIncrement::increment(*m, by);
        const auto newPos = m->fPos;
        const auto dir = ePointF::vector(oldPos, newPos);
        m->fAngle = dir.angle();
        if(m->fRemDist <= 0.0001f) {
            mMissiles.remove(m->fId);
        } else {
            const auto ipos = m->fPos.floor();
            const bool obsticle = !map->walkable(ipos.fX, ipos.fY);
            if(obsticle) {
                mMissiles.remove(m->fId);
            } else {
                bool found = false;
                const int margin = int(m->fRadius) + 1;
                const int xMin = ipos.fX - margin;
                const int xMax = ipos.fX + margin;
                const int yMin = ipos.fY - margin;
                const int yMax = ipos.fY + margin;
                for(int x = xMin; x <= xMax; x++) {
                    for(int y = yMin; y <= yMax; y++) {
                        const auto& charIds = mUnitAreas[eUnitTile{x, y}];
                        for(const int charId : charIds) {
                            const auto u = mUnits.get(charId);
                            if(!u || u->fHealth <= 0) continue;
                            if(u->fTeamId == m->fTeamId) continue;
                            const float dist = ePointF::distance(u->fPos, m->fPos);
                            if(dist > 0.5f*(u->fRadius + m->fRadius)) continue;
                            found = true;
                            mMissiles.remove(m->fId);
                            break;
                        }
                        if(found) break;
                    }
                    if(found) break;
                }
            }
        }
    }
}
