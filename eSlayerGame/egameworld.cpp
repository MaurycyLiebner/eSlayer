#include "egameworld.h"

#include "emaincharaction.h"
#include "textures/echarstextures.h"
#include "textures/emissilestextures.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerHelpers/ebody.h>
#include <eSlayerHelpers/eunitsinfo.h>

eGameWorld::eGameWorld(const std::shared_ptr<eMap>& map) :
    mMap(map),
    mMIncrementer(mUnitAreas),
    mNIncrementer(mUnitAreas) {
    iniMissileInc();
    iniNovaInc();
}

void eGameWorld::iniMissileInc() {
    const auto obstacle = [this](const ePointF& pos) {
        return mMap->obstacle(pos);
    };

    const auto removeMissile = [this](eMissile& m) {
        auto& em = static_cast<eExtendedMissile&>(m);
        const auto missileType = em.fType;
        const auto& missileTex = eMissilesInfo::sMissiles.get(missileType);
        const int hitId = missileTex.hitAnimId();
        if(hitId < 0) {
            eGameWorld::removeMissile(m);
        } else {
            em.fAnimId = hitId;
            em.fFrame = 0;
            em.fHit = true;
        }
    };

    const auto getMissile = [this](const uint32_t mid) {
        const auto m = mMissiles.get(mid);
        return static_cast<eMissile*>(m.get());
    };

    const auto getUnit = [this](const uint32_t charId) {
        if(charId == mClientId) {
            return static_cast<eUnitData*>(mMainChar.get());
        }
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    mMIncrementer.initialize(obstacle,
                             removeMissile,
                             getUnit,
                             getMissile,
                             nullptr);
}

void eGameWorld::iniNovaInc() {
    const auto inside = [this](const int x, const int y) {
        return mMap->inside(x, y);
    };

    const auto getObjects = [this](const int x, const int y)
        -> const std::vector<int>& {
        return mMap->objects(x, y);
    };

    const auto getObject = [this](const int id) {
        return mMap->object(id);
    };

    const auto getTile = [this](const int x, const int y)
        -> const eTile& {
        return mMap->tile(x, y);
    };

    const auto removeNova = [this](const eNova& m) {
        mNovas.remove(m.fId);
    };

    const auto getUnit = [this](const uint32_t charId) {
        if(charId == mClientId) {
            return static_cast<eUnitData*>(mMainChar.get());
        }
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    mNIncrementer.initialize(inside,
                             getObjects,
                             getObject,
                             getTile,
                             removeNova,
                             getUnit,
                             nullptr);
}

void eGameWorld::addUnit(const eUnitData& data) {
    const auto mapId = mMap->id();
    if(data.fMapId != mapId) return;
    const uint32_t charId = data.fCharId;
    const auto& pos = data.fPos;
    const auto area = mUnitAreas.posArea(pos);
    if(!mUnitAreas.hasArea(area)) return;
    mUnitAreas.emplace(area, charId);
    mUsedUnitAreas.emplace(area);
}

void eGameWorld::initialize(const uint32_t clientId,
                            const std::shared_ptr<eUnit>& mainChar) {
    const int w = mMap->width();
    const int h = mMap->height();
    mUnitAreas.initialize(w, h, 1);

    mClientId = clientId;
    mMainChar = mainChar;
}

eGameWorld::eProcessResult eGameWorld::processServerData(
    const uint32_t clientId,
    eServer& server,
    const eUnit& mainChar,
    eMainCharAction& mainAct,
    const eResolution& res,
    SDL_Renderer* const r) {
    eRequestData data;
    float resultTime;
    const bool b = server.requestData(clientId, data, resultTime);
    mResult.fReceived = b;
    mResult.fHasMainCharData = false;
    if(!b) return mResult;
    mResult.fAggressive = false;
    for(const auto& mp : data.fMapPortions) {
        mMap->loadPortion(mp);
    }

    mResult.fMerc = data.fMerc;
    mResult.fMana = data.fMana;
    mResult.fStamina = data.fStamina;
    mResult.fAttributes = data.fAttributes;
    mResult.fRemainingSkillPoints = data.fRemainingSkillPoints;
    mResult.fUpdateBoostsAuras = data.fUpdateBoostsAuras;
    if(mResult.fUpdateBoostsAuras) {
        mResult.fBoosts = data.fBoosts;
        mResult.fAuras = data.fAuras;
    }
    for(const auto& a : mUsedUnitAreas) {
        mUnitAreas.clear(a);
    }
    mUsedUnitAreas.clear();
    const auto& newUnits = data.fNewUnits;
    const auto& updatedUnits = data.fUpdatedUnits;
    const auto& missiles = data.fMissiles;
    const auto& missileUpdates = data.fMissileUpdates;
    const auto& novas = data.fNovas;
    const auto& skillAreas = data.fSkillAreas;
    const auto& usedSkills = data.fUsedSkills;
    const auto& newItems = data.fNewItems;
    const auto& removedItemIds = data.fRemovedItemIds;
    std::set<uint32_t> uPresent;

    auto& stats = mainAct.stats();
    for(const int skillId : usedSkills) {
        const float cooldown = stats.cooldownBySkillId(skillId);
        if(cooldown > 0.f) {
            stats.fCooldowns[skillId] = cooldown*25.f;
        }
    }

    for(const auto& u : newUnits) {
        const uint32_t charId = u.fCharId;
        addUnit(u);
        uPresent.emplace(charId);
        if(charId == clientId) {
            mResult.fHasMainCharData = true;
            mResult.fMainCharData = u;
            continue;
        }
        const auto& uinfo = eUnitsInfo::sUnits.get(u.fUnitInfoId);
        const auto& texs = eCharsTextures::get(uinfo.fCharData);
        const auto unitModel = texs.requestModel(
            u.fModelParts, res, r);

        const auto unit = std::make_shared<eUnit>();
        for(const auto& b : eBodies::sBodies) {
            if(b.fBodyId == charId) {
                unit->setSlayerBody(true);
                break;
            }
        }
        static_cast<eUnitData&>(*unit) = u;
        eCharUnitModel model;
        model.setCharModel(unitModel);
        model.setAnimation(u.fAnim, u.fAnimId,
                           u.fAnimSpeed, true);
        model.setAngle(u.fAngle);
        unit->setModel(model);
        mUnits.add(charId, unit);
    }

    for(const auto& u : updatedUnits) {
        const uint32_t charId = u.fCharId;
        uPresent.emplace(charId);
        const bool partsUpdate = u.getUpdate(
            eUnitData::eShift::modelParts);
        const auto tryUpdateParts = [&](const std::shared_ptr<eUnit>& unit) {
            if(!unit) return;
            auto& unitRef = *unit;
            auto& model = unitRef.model();
            if(partsUpdate) {
                const auto& uinfo = eUnitsInfo::sUnits.get(u.fUnitInfoId);
                const auto& texs = eCharsTextures::get(uinfo.fCharData);
                const auto unitModel = texs.requestModel(
                    u.fModelParts, res, r);
                model.setCharModel(unitModel);
            }
        };
        if(charId == clientId) {
            mResult.fHasMainCharData = true;
            auto& d = mResult.fMainCharData;
            u.apply(d);
            addUnit(d);
            tryUpdateParts(mMainChar);
            continue;
        }
        const auto unit = mUnits.get(charId);
        if(!unit) continue;
        auto& unitRef = *unit;
        u.apply(unitRef);
        addUnit(unitRef);
        auto& model = unitRef.model();
        tryUpdateParts(unit);
        model.setAngle(unitRef.fAngle);
        model.setAnimation(unitRef.fAnim, unitRef.fAnimId,
                           unitRef.fAnimSpeed, false);
        if(!mResult.fAggressive && eTeams::areEnemies(mainChar.fTeamId, unitRef.fTeamId) && unitRef.fHealth > 0) {
            const float dist = ePointF::distance(mainChar.fPos, u.fPos);
            if(dist < 5.f) mResult.fAggressive = true;
        }
    }

    for(const auto& u : mUnits) {
        const uint32_t charId = u->fCharId;

        {
            auto& ss = eSlayers::sSlayers;
            const auto it = ss.find(charId);
            if(it != ss.end()) {
                auto& s = it->second;
                s.assign(*u);
                s.fTeamId = u->fTeamId;
            }
        }

        {
            auto& ff = eFollowers::sFollowers;
            const auto it = ff.find(charId);
            if(it != ff.end()) {
                auto& f = it->second;
                f.assign(*u);
            }
        }

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
        const auto missileType = m.fType;
        auto& missileTex = eMissilesInfo::sMissiles.get(missileType);
        const int apearId = missileTex.appearAnimId();
        const int baseId = missileTex.baseAnimId();
        mm->fAnimId = apearId >= 0 ? apearId : baseId;
        reinterpret_cast<eMissile&>(*mm) = m;
        mMissiles.add(m.fId, mm);
    }

    for(const auto& u : missileUpdates) {
        const auto m = mMissiles.get(u.fId);
        if(!m) continue;
        m->applyUpdate(u);
    }

    for(const auto& n : novas) {
        const auto nn = std::make_shared<eExtendedNova>();
        static_cast<eNova&>(*nn) = n;
        mNovas.add(n.fId, nn);
    }

    for(const auto& a : skillAreas) {
        const auto aa = std::make_shared<eExtendedSkillArea>();
        static_cast<eSkillArea&>(*aa) = a;
        mSkillAreas.add(a.fId, aa);
    }

    return mResult;
}

void eGameWorld::simulateMissiles(const float by) {
    for(const auto& m : mMissiles) {
        if(m->fHit) continue;
        const auto oldPos = m->fPos;
        const bool r = mMIncrementer.increment(*m, by);
        if(r) continue;
        const auto& newPos = m->fPos;
        const auto dir = ePointF::vector(oldPos, newPos);
        m->fAngle = dir.angle();
    }
}

void eGameWorld::simulateNovas(const float by) {
    for(const auto& n : mNovas) {
        mNIncrementer.increment(*n, by);
    }
}

void eGameWorld::simulateSkillAreas(const float by) {
    for(const auto& a : mSkillAreas) {
        float& time = a->fRemTime;
        time -= by;
    }
}

void eGameWorld::removeMissile(const eMissile& m) {
    mMissiles.remove(m.fId);
}
