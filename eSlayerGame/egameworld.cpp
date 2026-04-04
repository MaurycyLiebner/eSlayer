#include "egameworld.h"

#include "emaincharaction.h"
#include "textures/echarstextures.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

eGameWorld::eGameWorld(const std::shared_ptr<eMap>& map) :
    mMap(map),
    mUnitAreas(1),
    mMIncrementer(mUnitAreas) {
    const auto obsticle = [this](const ePointF& pos) {
        const auto ipos = pos.floor();
        return !mMap->walkable(ipos.fX, ipos.fY);
    };

    const auto removeMissile = [this](const eMissile& m) {
        mMissiles.remove(m.fId);
    };

    const auto getUnit = [this](const int charId) {
        if(charId == mClientId) {
            return static_cast<eUnitData*>(mMainChar.get());
        }
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    mMIncrementer.initialize(obsticle,
                             removeMissile,
                             getUnit,
                             nullptr);
}

void eGameWorld::initialize(const int clientId,
                            const std::shared_ptr<eUnit>& mainChar) {
    mClientId = clientId;
    mMainChar = mainChar;
}

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
    if(!b) return result;
    if(data.fHasMap) {
        mMap->loadPortion(data.fMapPortion);
    }

    result.fMana = data.fMana;
    result.fLevel = data.fLevel;
    result.fExperience = data.fExperience;

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
        model.setAnimation(u.fAnim, u.fAnimId, u.fAnimSpeed);
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

void eGameWorld::simulateMissiles(const float by) {
    for(const auto& m : mMissiles) {
        const auto oldPos = m->fPos;
        const bool r = mMIncrementer.increment(*m, by);
        if(r) continue;
        const auto& newPos = m->fPos;
        const auto dir = ePointF::vector(oldPos, newPos);
        m->fAngle = dir.angle();
    }
}
