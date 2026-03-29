#include "egameworld.h"

#include "emaincharaction.h"
#include "textures/echarstextures.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

eGameWorld::eProcessResult eGameWorld::processServerData(
    const int clientId,
    eServer& server,
    const eUnit& mainChar,
    eMainCharAction& mainAct,
    SDL_Renderer* const r) {
    eProcessResult result;

    eRequestData data;
    float resultTime;
    const bool b = server.requestData(clientId, data, resultTime);
    eWeaponData wdata;
    const bool w = server.receiveWeaponData(clientId, wdata);
    if(w) mainAct.setWeaponData(wdata);
    if(!b) return result;

    result.fReceived = true;
    mUnitAreas.clear();
    const auto& newUnits = data.fNewUnits;
    const auto& updatedUnits = data.fUpdatedUnits;
    const auto& missiles = data.fMissiles;
    const auto& newItems = data.fNewItems;
    const auto& removedItemIds = data.fRemovedItemIds;
    std::set<int> uPresent;

    // Process new units — full initialization with textures/models
    for(const auto& u : newUnits) {
        const int charId = u.fCharId;
        const auto area = mUnitAreas.posArea(u.fPos);
        mUnitAreas.emplace(area, charId);
        uPresent.emplace(charId);
        if(charId == clientId) {
            result.fHasMainCharData = true;
            result.fMainCharData = u;
            continue;
        }
        const auto& texs = eCharsTextures::get(u.fTypeId);
        const auto unitModel = texs.requestModel(u.fModelParts, r);

        const auto unit = std::make_shared<eUnit>();
        static_cast<eUnitData&>(*unit) = u;
        eCharUnitModel model;
        model.setCharModel(unitModel);
        model.setAnimation(u.fAnim, u.fAnimId, u.fAnimSpeed);
        model.setAngle(u.fAngle);
        unit->setModel(model);
        mUnits.add(charId, unit);
        if(!result.fAggressive && mainChar.fTeamId != u.fTeamId && u.fHealth > 0) {
            const float dist = ePointF::distance(mainChar.fPos, u.fPos);
            if(dist < 5.f) result.fAggressive = true;
        }
    }

    // Process updated units — lightweight dynamic data only
    for(const auto& u : updatedUnits) {
        const int charId = u.fCharId;
        const auto area = mUnitAreas.posArea(u.fPos);
        mUnitAreas.emplace(area, charId);
        uPresent.emplace(charId);
        if(charId == clientId) {
            result.fHasMainCharData = true;
            result.fMainCharData.fCharId = u.fCharId;
            result.fMainCharData.fPos = u.fPos;
            result.fMainCharData.fVel = u.fVel;
            result.fMainCharData.fAngle = u.fAngle;
            result.fMainCharData.fAnim = u.fAnim;
            result.fMainCharData.fAnimId = u.fAnimId;
            result.fMainCharData.fAnimSpeed = u.fAnimSpeed;
            result.fMainCharData.fActionTime = u.fActionTime;
            result.fMainCharData.fHealth = u.fHealth;
            result.fMainCharData.fMaxHealth = u.fMaxHealth;
            continue;
        }
        const auto unit = mUnits.get(charId);
        if(!unit) continue;
        unit->fPos = u.fPos;
        unit->fVel = u.fVel;
        unit->fAngle = u.fAngle;
        unit->fAnim = u.fAnim;
        unit->fAnimId = u.fAnimId;
        unit->fAnimSpeed = u.fAnimSpeed;
        unit->fActionTime = u.fActionTime;
        unit->fHealth = u.fHealth;
        unit->fMaxHealth = u.fMaxHealth;
        auto& model = unit->model();
        model.setAngle(u.fAngle);
        model.setAnimation(unit->fAnim, unit->fAnimId, u.fAnimSpeed);
        if(!result.fAggressive && mainChar.fTeamId != unit->fTeamId && u.fHealth > 0) {
            const float dist = ePointF::distance(mainChar.fPos, u.fPos);
            if(dist < 5.f) result.fAggressive = true;
        }
    }

    for(const auto& u : mUnits) {
        const int charId = u->fCharId;
        const auto it = uPresent.find(charId);
        if(it != uPresent.end()) continue;
        mUnits.remove(charId);
    }

    for(const auto& i : newItems) {
        mGroundItems.add(i.fItemId, std::make_shared<eGroundItem>(i));
    }
    for(const auto id : removedItemIds) {
        mGroundItems.remove(id);
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
