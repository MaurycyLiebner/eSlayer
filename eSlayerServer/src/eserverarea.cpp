#include "eserverarea.h"

#include "actions/eclientaction.h"
#include "actions/eunitbaseaction.h"
#include "actions/efolloweraction.h"

#include "eelitemodifiers.h"

#include "eitemgenerator.h"

#include <eSlayerMissiles/emissileincrementer.h>
#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/edoors.h>
#include <eSlayerHelpers/eplacementhelper.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/esellers.h>
#include <eSlayerHelpers/equests.h>
#include <eSlayerHelpers/emercenaries.h>
#include <eSlayerHelpers/edifficulties.h>

std::vector<uint32_t> eServerArea::sSlain;
std::map<uint32_t, std::shared_ptr<eServerUnit>>
eServerArea::sSlayers;

eServerArea::eServerArea() :
    mMIncrementer(mUnitAreas),
    mNIncrementer(mUnitAreas) {
    iniMissileInc();
    iniNovaInc();
}

void eServerArea::iniMissileInc() {
    const auto obstacle = [this](const ePointF& pos) {
        return mMap->obstacle(pos);
    };

    const auto removeMissile = [this](const eMissile& m) {
        mMissiles.remove(m.fId);
    };

    const auto getUnit = [this](const uint32_t charId) {
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    const auto getMissile = [this](const uint32_t mid) {
        const auto m = mMissiles.get(mid);
        return static_cast<eMissile*>(m.get());
    };

    const auto hitAction = [this](const eMissile& m, eUnitData& u) {
        const auto& sm = static_cast<const eServerMissile&>(m);
        auto& su = static_cast<eServerUnit&>(u);
        if(sm.fHitAction) sm.fHitAction(su, sm);
    };

    mMIncrementer.initialize(obstacle,
                             removeMissile,
                             getUnit,
                             getMissile,
                             hitAction);
}

void eServerArea::iniNovaInc() {
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
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    const auto hitAction = [this](const eNova& n, eUnitData& u) {
        const auto& sn = static_cast<const eServerNova&>(n);
        auto& su = static_cast<eServerUnit&>(u);
        if(sn.fHitAction) sn.fHitAction(su);
    };

    mNIncrementer.initialize(inside,
                             getObjects,
                             getObject,
                             getTile,
                             removeNova,
                             getUnit,
                             hitAction);
}

void eServerArea::iniSetupUnit(
        const std::shared_ptr<eServerUnit>& u,
        const uint32_t charId,
        const eTeamId teamId,
        const ePointF& pos,
        const uint8_t unitInfoId,
        const eUnitInfo& uinfo,
        const eCharData& data,
        const eModelParts& modelParts) {
    u->fCharId = charId;
    u->fTeamId = teamId;
    u->fState = 0;
    u->fImmunities = 0;
    u->fUnitInfoId = unitInfoId;
    u->fRadius = uinfo.fRadius;
    u->fAnim = data.animId("stand");
    u->fAnimId = eAnimId{0};
    u->fAnimSpeed = 1.f;
    u->fAngle = eRand::randF(0.f, 360.f);
    u->fBlockingActionTime = 0.f;
    u->fModelParts = modelParts;

    auto& eq = u->equipment();
    for(const auto itemId : uinfo.fItems) {
        eItem item;
        const auto& data = eItemsData::get(itemId);
        item.fType = data.fType;
        item.fSubType = data.fSubtype;
        item.fDataId = itemId;
        eq.add(item, true, nullptr);
    }

    iniSetupUnit(u, pos);
}

void eServerArea::iniSetupSlayerAction(
    const std::shared_ptr<eServerUnit>& u) {
    const auto a = std::make_shared<eClientAction>(*u, *this);
    u->setAction(a);
}

void eServerArea::iniSetupFollowerAction(
    const std::shared_ptr<eServerUnit>& u,
    const std::shared_ptr<eServerUnit>& follow) {
    const auto a = std::make_shared<eFollowerAction>(*u, *this, follow);
    u->setAction(a);
}

void eServerArea::iniSetupUnit(
    const std::shared_ptr<eServerUnit>& u,
    const ePointF& pos) {
    const auto charId = u->fCharId;
    const auto teamId = u->fTeamId;
    const auto typeId = u->fUnitInfoId;
    u->setArea(*this);
    u->fPos = pos;
    const auto& uinfo = eUnitsInfo::sUnits.get(typeId);
    auto& m = u->movementHandler();
    m.setSpeed(uinfo.fWalkSpeed);
    const auto wPos = [this](const ePointF& pos) {
        return mMap->walkable(pos);
    };
    const auto wPath = [this](const ePointF& from,
                              const ePointF& to) {
        return mMap->walkable(from, to);
    };
    const auto iter = [this, charId](
        const ePointF& pos,
        const float dist,
        const eOtherHandler& handler) {
        iterateOverUnits(pos, dist, [handler, charId](
            const std::shared_ptr<eServerUnit>& u) {
            if(charId == u->fCharId) return false;
            handler(*u);
            return false;
        });
    };
    auto& pmap = mMap->pathFinderMap();
    m.intialize(wPos, wPath, iter, charId, teamId, pmap);

    mUnits.add(charId, u);
    const auto area = unitArea(*u);
    mUnitAreas.emplace(area, charId);

    const auto mapId = mMap->id();
    u->setMapId(mapId);
    const auto areaId = mMap->areaAt(pos);
    u->setAreaId(areaId);

    auto& stats = u->stats();
    stats.fDifficultyPenalties = uinfo.fDifficultyPenalties;
}

void eServerArea::addGroundItem(
    const ePointF& pos, const eItem& item) {
    if(item.fType == eItemType::none) return;
    const auto itemId = item.fItemId;
    const auto groundItem = std::make_shared<eGroundItem>();
    static_cast<eItemBase&>(*groundItem) = item;
    const auto baseTile = mItemTiles.posArea(pos);
    bool found = false;
    for(int dist = 0; dist < 100; dist++) {
        for(int x = dist; x >= -dist; x--) {
            for(int y = dist; y >= -dist; y--) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const eArea tile{baseTile.fX + x, baseTile.fY + y};

                const auto tilePos = mItemTiles.areaPos(tile);
                const bool w = walkable(tilePos);
                if(!w) continue;

                if(!mItemTiles.hasArea(tile)) continue;
                const auto& items = mItemTiles.at(tile);
                if(!items.empty()) continue;
                groundItem->fPos = tilePos;
                mItemTiles.emplace(tile, itemId);
                found = true;
                break;
            }
            if(found) break;
        }
        if(found) break;
    }
    if(!found) return;
    mGroundItems.add(itemId, groundItem);
    mItemsOnGround.add(itemId, std::make_shared<eItem>(item));
    const auto area = itemArea(itemId);
    mItemAreas.emplace(area, itemId);
}

void eServerArea::generateItems(
    const ePointF& pos, const int level,
    const float worth) {
    float remWorth = worth;
    while(remWorth >= 0.25f) {
        const float worth = eRand::randF(0.25f, remWorth);
        generateItem(pos, level, worth);
        remWorth -= worth;
    }
}

void eServerArea::generateItem(
    const ePointF& pos, const int level,
    const float worth) {
    const auto item = eItemGenerator::generateItem(level, worth);
    addGroundItem(pos, item);
}

void eServerArea::generatePotion(
    const ePointF& pos, const int level,
    const float worth) {
    const auto item = eItemGenerator::generatePotion(level, worth);
    addGroundItem(pos, item);
}

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    const auto mapId = mMap->id();

    for(auto &it : eSellers::sSellers) {
        auto& s = it.second;
        if(s.fMapId != mapId) continue;
        const auto maxLevel = s.fLevel;
        const auto& typeIds = s.fSellItemTypes;
        if(typeIds.empty()) {
            continue;
        }
        int iMax = 1 + typeIds.size()/3;
        iMax = std::clamp(iMax, 1, 3);
        for(int i = 0; i < iMax; i++) {
            auto& p = s.addPage();
            bool added = false;
            do {
                const int typeId = eRand::randomElement(typeIds);
                const auto minLevel = std::max(1, maxLevel - 10);
                const auto level = eRand::rand(minLevel, maxLevel);
                const float worth = eRand::biasedRandF(2.f, 4.f, 1.f);
                const auto item = eItemGenerator::generateItem(
                    typeId, level, worth);
                added = p.tryAdd(item);
            } while(added);
        }
    }

    const int w = map->width();
    const int h = map->height();
    mUnitAreas.initialize(w, h, mUnitAreaDim);
    mItemAreas.initialize(w, h, mItemAreaDim);
    mItemTiles.initialize(w, h, -mItemTileSubdivision);

    const auto& mareas = map->monsterAreas();

    const auto checkMargin = [&](const int x, const int y,
                                 const eChamber& c) {
        const int unitMargin = 1;
        for(int dx = -unitMargin; dx <= unitMargin; dx++) {
            const int xx = x + dx;
            for(int dy = -unitMargin; dy <= unitMargin; dy++) {
                const int yy = y + dy;
                if(!c.contains({xx, yy})) return false;
                const auto& objs = mMap->objects(xx, yy);
                if(!objs.empty()) return false;
                const auto uarea = mUnitAreas.posArea(ePoint{xx, yy});
                if(!mUnitAreas.hasArea(uarea)) continue;
                const auto& us = mUnitAreas.at(uarea);
                if(!us.empty()) return false;
            }
        }
        return true;
    };

    const auto calcArea = [&](const int x, const int y,
                              const eChamber& c) {
        int area = 0;
        int obstacles = 0;
        for(int dim = 0;; dim++) {
            for(int dx = -dim; dx <= dim; dx++) {
                const int xx = x + dx;
                for(int dy = -dim; dy <= dim; dy++) {
                    const int yy = y + dy;
                    if(std::abs(dx) != dim && std::abs(dy) != dim) continue;
                    if(!c.contains({xx, yy})) return area - obstacles;
                    area++;
                    const auto& objs = mMap->objects(xx, yy);
                    if(!objs.empty()) obstacles++;
                    const auto uarea = mUnitAreas.posArea(ePoint{xx, yy});
                    if(!mUnitAreas.hasArea(uarea)) continue;
                    const auto& us = mUnitAreas.at(uarea);
                    for(const auto uid : us) {
                        const auto& u = mUnits.get(uid);
                        const auto& pos = u->fPos;
                        const auto ipos = pos.floor();
                        if(ipos.fX == xx && ipos.fY == yy) {
                            obstacles++;
                        }
                    }
                }
            }
        }
        return area - obstacles;
    };

    const auto calcMaxArea = [&](const eChamber& c,
                                 int& maxA,
                                 int& xMax,
                                 int& yMax) {
        maxA = 0;
        for(const auto& r : c.fRects) {
            for(int x = r.fX; x < r.fX + r.fW; x++) {
                for(int y = r.fY; y < r.fY + r.fH; y++) {
                    const bool r = checkMargin(x, y, c);
                    if(!r) continue;
                    const int a = calcArea(x, y, c);
                    if(a <= maxA) continue;
                    maxA = a;
                    xMax = x;
                    yMax = y;
                }
            }
        }
    };

    for(const auto& ma : mareas) {
        const auto& chambers = ma.fChambers;
        ePlacementHelper helper;
        for(int i = 0; i < chambers.size(); i++) {
            const auto& sc = chambers[i];
            int maxA = 0;
            int xMax;
            int yMax;
            calcMaxArea(sc, maxA, xMax, yMax);
            helper.add(i, maxA);
        }
        helper.randomize();

        const auto& types = ma.fSettings.fTypes;
        for(const auto& us : types) {
            const auto tryAddUnits = [&]() {
                int area;
                const int id = helper.get(area);
                if(id < 0) return false;
                if(area < us.fMinArea) return false;
                const auto& c = chambers[id];
                const auto& rects = c.fRects;
                if(rects.empty()) return false;
                const auto& r0 = rects[0];

                int maxA = 0;
                int xMax = r0.fX;
                int yMax = r0.fY;

                calcMaxArea(c, maxA, xMax, yMax);
                if(maxA == 0) return false;
                const auto& uBaseData = eUnitsInfo::sUnits.get(us.fBaseType);
                const bool elite = us.fElite;
                bool boss = elite;
                eEliteModifiers mods;
                if(elite) {
                    mods.initialize(1, uBaseData.fLevel);
                }

                const auto addUnit = [&]() {
                    ePointF pos;
                    const bool r = findPlaceForUnit({xMax, yMax}, pos);
                    if(!r) return false;
                    auto type = us.fBaseType;
                    if(boss) {
                        type = eRand::randomElement(us.fBossTypes);
                    } else {
                        type = eRand::randomElement(us.fTypes);
                    }
                    const auto& udata = eUnitsInfo::sUnits.get(type);
                    const auto& data = eCharDataInfo::get(udata.fCharData);
                    const auto modelParts = data.randomModelParts();
                    auto& map = mMap->pathFinderMap();
                    const auto u = std::make_shared<eServerUnit>(
                        boss ? eUnitType::uniqueBoss : eUnitType::minion,
                        data, type, *this);
                    const uint32_t charId = eServerUnit::sNextCharId++;
                    iniSetupUnit(u, charId, eTeamId::neutralHostile,
                                 pos, type, udata, data, modelParts);

                    u->addBoost(udata.fModifiers, eBoostCurseType::permanent, false);

                    if(elite) {
                        mods.apply(*u, boss);
                        boss = false;
                    }

                    {
                        const int schoice = u->addSkill();
                        u->setSkillId(schoice, 0, false);
                    }
                    eSkillLevels skillLevels;
                    using sMap = std::map<uint16_t, uint16_t>;
                    reinterpret_cast<sMap&>(skillLevels) = udata.fSkills;
                    u->setSkillLevels(skillLevels, false);
                    for(const auto it : udata.fSkills) {
                        const int skillId = it.first;
                        const int schoice = u->addSkill();
                        u->setSkillId(schoice, skillId, false);
                    }
                    u->recalculateStats();
                    u->recalculateAuras();

                    const auto a = std::make_shared<eUnitBaseAction>(*u, *this);
                    u->setAction(a);
                    return true;
                };

                const int nUnits = us.fGroupSize;
                for(int i = 0; i < nUnits; i++) {
                    const bool r = addUnit();
                    if(!r) break;
                }

                calcMaxArea(c, maxA, xMax, yMax);
                helper.set(id, maxA);

                return true;
            };

            for(int i = 0; i < us.fCount; i++) {
                const bool r = tryAddUnits();
                if(!r) break;
            }
        }
    }
}

void eServerArea::increment(const float by) {
    if(mPortalsVersion < ePortal::version()) {
        mPortalsVersion = ePortal::version();
        const auto mapId = mMap->id();
        std::set<uint32_t> newPortals;
        const auto addPortal = [&](const uint32_t objId,
                                   const eAreaIds& to,
                                   const ePointF& pos) {
            if(to.fMapId != mapId) return;
            const int n = mPortals.count(objId);
            if(n > 0) return;
            newPortals.emplace(objId);
        };

        for(const auto& p : ePortal::sPortals) {
            addPortal(p.fCampPortalId,
                      p.fCampArea,
                      p.fCampPos);
            addPortal(p.fOutdoorPortalId,
                      p.fOutdoorArea,
                      p.fOutdoorPos);
        }

        for(const auto pid : mPortals) {
            const int n = newPortals.count(pid);
            if(n > 0) continue;
            mMap->removeObject(pid);
        }

        std::swap(mPortals, newPortals);
    }

    std::set<eArea> unitAreas;
    for(auto& it : mClientData) {
        const int i = it.first;
        const auto u = unit(i);
        if(!u) continue;
        auto& clientData = it.second;
        auto& oldArea = clientData.fArea;
        const auto newArea = unitArea(*u);
        if(oldArea != newArea) {
            mUnitAreas.erase(oldArea, i);
            mUnitAreas.emplace(newArea, i);
            oldArea = newArea;
        }

        const auto& screenDims = clientData.fScreen;
        const int halfHeight = std::ceil(0.5f*screenDims.fHeight/mUnitAreaDim);
        const int halfWidth = std::ceil(0.5f*screenDims.fWidth/mUnitAreaDim);
        const int dyMin = -halfHeight - 2;
        const int dyMax = halfHeight + 2;
        const int dxMin = -halfWidth - 2;
        const int dxMax = halfWidth + 2;
        for(int dy = dyMin; dy <= dyMax; dy++) {
            for(int dx = dxMin; dx <= dxMax; dx++) {
                const int y = newArea.fY - dx + dy/2;
                const int x = newArea.fX + dx + dy % 2 + dy/2;
                const eArea area{x, y};
                unitAreas.emplace(area);
            }
        }

        auto& followers = u->followers();
        const auto followersTmp = followers;
        for(const auto charId : followersTmp) {
            const auto u = unit(charId);
            if(!u || u->fMaxHealth <= 0) {
                followers.remove(charId);
            } else {
                const auto area = unitArea(*u);
                unitAreas.emplace(area);
            }
        }
    }

    std::set<int> newAuraSources;
    const bool recalcAura = (mAuraRecalcCounter++ % mAuraRecalcSpan) == 0;
    for(const auto& area : unitAreas) {
        if(!mUnitAreas.hasArea(area)) continue;
        const auto units = mUnitAreas.at(area);
        for(const uint32_t charId : units) {
            const auto u = mUnits.get(charId);
            if(!u) continue;
            if(recalcAura) {
                const auto oldAuraIds = u->auraIds();
                u->removeAllAuras(false);
                if(u->fHealth > 0) {
                    if(u->isAuraSource()) {
                        newAuraSources.emplace(charId);
                    }
                    for(const auto id : mAuraSources) {
                        const auto uu = mUnits.get(id);
                        if(!uu) continue;
                        uu->addAurasTo(*u);
                    }
                    const auto& newAuraIds = u->auraIds();
                    const bool recalc = oldAuraIds != newAuraIds;
                    if(recalc) {
                        const auto it = mClientData.find(charId);
                        if(it != mClientData.end()) {
                            auto& client = it->second;
                            client.fUpdateBoostsAuras = true;
                        }
                        u->recalculateStats();
                    }
                }
                u->applyBoostsTmp();
            }
            const auto oldArea = unitArea(*u);
            u->increment(by);
            const auto newArea = unitArea(*u);
            if(oldArea != newArea) {
                mUnitAreas.erase(oldArea, charId);
                mUnitAreas.emplace(newArea, charId);
            }
        }
    }
    if(recalcAura) std::swap(newAuraSources, mAuraSources);

    for(const auto& m : mMissiles) {
        auto& mref = *m;
        if(mref.fConsecutive > 0 && mref.fTime > 0.5f) {
            const auto mmPtr = std::make_shared<eServerMissile>();
            auto& mmRef = *mmPtr;
            const auto idTmp = mmRef.fId;
            mmRef = mref;
            mmRef.fId = idTmp;
            mmRef.fPos = mmRef.fFrom;
            mmRef.fTime = 0.f;
            mmRef.fRemTime = mref.fTotalTime;
            mmRef.fRemDist = mref.fTotalDist;
            mmRef.fConsecutive = mref.fConsecutive - 1;
            mref.fConsecutive = 0;
            addMissile(mmPtr);
        }
        mMIncrementer.increment(mref, by);
    }

    for(const auto& n : mNovas) {
        mNIncrementer.increment(*n, by);
    }

    for(const auto& a : mSkillAreas) {
        float& time = a->fRemTime;
        time -= by;
        a->fIncrement();
        if(time > 0.f) continue;
        mSkillAreas.remove(a->fId);
    }

    removePlannedUnits();

    mTime += by;
}

void eServerArea::unitsData(
    const uint32_t clientId,
    std::vector<eUnitData>& newUnits,
    std::vector<eUnitData>& updatedUnits) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return;
    const auto clientU = unit(clientId);
    if(!clientU) return;
    const auto& clientPos = clientU->fPos;
    auto& clientData = it->second;
    auto& known = clientData.fKnownUnits;
    std::set<uint32_t> visible;
    const auto& clientArea = clientData.fArea;
    const auto& screenDims = clientData.fScreen;
    const int width = screenDims.fWidth;
    const int height = screenDims.fHeight;
    const float halfHeightF = 0.5f*height;
    const float halfWidthF = 0.5f*width;
    const int halfHeight = std::ceil(halfHeightF/mUnitAreaDim);
    const int halfWidth = std::ceil(halfWidthF/mUnitAreaDim);
    const int dyMin = -halfHeight - 2;
    const int dyMax = halfHeight + 2;
    const int dxMin = -halfWidth - 2;
    const int dxMax = halfWidth + 2;

    enum class eHandleType {
        regular,
        slayers,
        followers
    };

    const auto handleUnit = [&](
            eServerUnit& u,
            const eHandleType htype) {
        const bool slayer = htype == eHandleType::slayers;
        if(u.isSlayer() != slayer) return;
        const auto charId = u.fCharId;
        const auto it = visible.emplace(charId);
        if(!it.second) return;
        const auto update = u.requestUpdate(clientId);
        if(known.find(charId) == known.end()) {
            if(htype == eHandleType::regular) {
                const auto& pos = u.fPos;
                const float y = halfHeightF +
                                0.5f*(pos.fY - clientPos.fY + pos.fX - clientPos.fX);
                const float x = halfWidthF +
                                0.5f*(clientPos.fY - pos.fY + pos.fX - clientPos.fX);
                const float margin = 4.f;
                if(y < -margin) return;
                if(x < -margin) return;
                if(y > height + margin) return;
                if(x > width + margin) return;
            }
            const auto d = u.toUnitData();
            newUnits.emplace_back(d);
            known.emplace(charId);
        } else {
            const auto d = u.toUnitData(update);
            updatedUnits.emplace_back(d);
        }
    };

    const auto handleUnitById = [&](
            const uint32_t charId,
            const eHandleType htype) {
        const auto u = unit(charId);
        if(!u) return;
        handleUnit(*u, htype);
    };
    for(int dy = dyMin; dy <= dyMax; dy++) {
        for(int dx = dxMin; dx <= dxMax; dx++) {
            const int y = clientArea.fY - dx + dy/2;
            const int x = clientArea.fX + dx + dy % 2 + dy/2;
            const eArea area{x, y};
            if(!mUnitAreas.hasArea(area)) continue;
            const auto& units = mUnitAreas.at(area);
            for(const uint32_t charId : units) {
                handleUnitById(charId, eHandleType::regular);
            }
        }
    }

    const auto& followers = clientU->followers();
    for(const auto fId : followers) {
        handleUnitById(fId, eHandleType::followers);
    }

    for(const auto& t : sSlayers) {
        const auto& s = t.second;
        handleUnit(*s, eHandleType::slayers);
    }

    // Remove units no longer visible from the known set
    for(auto it = known.begin(); it != known.end(); ) {
        if(visible.find(*it) == visible.end()) {
            it = known.erase(it);
        } else {
            ++it;
        }
    }
}

void eServerArea::itemsData(
    const uint32_t clientId,
    std::vector<eGroundItem>& newItems,
    std::vector<uint32_t>& removedItemIds) {
    const auto client = unit(clientId);
    if(!client) return;
    auto& clientData = mClientData[clientId];
    auto& known = clientData.fKnownItems;
    std::set<uint32_t> visible;
    const auto& clientPos = client->fPos;
    const auto clientArea = mItemAreas.posArea(clientPos);
    const auto& screenDims = clientData.fScreen;
    const int halfHeight = std::ceil(0.5f*screenDims.fHeight/mItemAreaDim);
    const int halfWidth = std::ceil(0.5f*screenDims.fWidth/mItemAreaDim);
    const int dyMin = -halfHeight - 1;
    const int dyMax = halfHeight + 1;
    const int dxMin = -halfWidth - 1;
    const int dxMax = halfWidth + 1;
    for(int dy = dyMin; dy <= dyMax; dy++) {
        for(int dx = dxMin; dx <= dxMax; dx++) {
            const int y = clientArea.fY - dx + dy/2;
            const int x = clientArea.fX + dx + dy % 2 + dy/2;
            const eArea area{x, y};
            if(!mItemAreas.hasArea(area)) continue;
            const auto& items = mItemAreas.at(area);
            for(const uint32_t itemId : items) {
                const auto i = groundItem(itemId);
                if(!i) continue;
                visible.emplace(itemId);
                if(known.find(itemId) == known.end()) {
                    newItems.emplace_back(*i);
                    known.emplace(itemId);
                }
            }
        }
    }
    for(auto it = known.begin(); it != known.end(); ) {
        if(visible.find(*it) == visible.end()) {
            removedItemIds.emplace_back(*it);
            it = known.erase(it);
        } else {
            ++it;
        }
    }
}

void eServerArea::usedSkills(const uint32_t clientId,
                             std::set<int>& usedSkills) {
    const auto client = unit(clientId);
    if(!client) return;
    usedSkills = client->takeUsedSkills();
}

eArea eServerArea::unitArea(const uint32_t charId) const {
    const auto u = unit(charId);
    if(!u) return {0, 0};
    return unitArea(*u);
}

eArea eServerArea::unitArea(const eServerUnit& u) const {
    const auto& pos = u.fPos;
    return mUnitAreas.posArea(pos);
}

eArea eServerArea::itemArea(const uint32_t itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemArea(*i);
}

eArea eServerArea::itemArea(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemAreas.posArea(pos);
}

eArea eServerArea::itemTile(const uint32_t itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemTile(*i);
}

eArea eServerArea::itemTile(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemTiles.posArea(pos);
}

bool eServerArea::mapPortions(
    const uint32_t clientId,
    std::vector<eMapPortion>& result) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    auto& known = clientData.fKnownMap;
    const bool addAll = known.empty();
    const auto area = known.posArea(u->fPos);
    const int m = 1;
    for(int x = area.fX - m; x <= area.fX + m; x++) {
        for(int y = area.fY - m; y <= area.fY + m; y++) {
            const eArea xyArea{x, y};
            const bool r = known.hasArea(xyArea);
            if(!r) {
                const auto pos = known.areaPos(xyArea).round();
                const eMapPortionArea mapArea{pos.fX,
                                              pos.fY,
                                              eMapPortion::sBaseDim,
                                              eMapPortion::sBaseDim};
                eMapPortion p;
                const bool r = mMap->extractPortion(mapArea, p);
                if(r) {
                    result.emplace_back(std::move(p));
                    known.emplace(xyArea);
                    if(!addAll) return true;
                }
            }
        }
    }
    return addAll;
}

bool eServerArea::walkable(
    const ePointF& pos) const {
    return mMap->walkable(pos);
}

bool eServerArea::walkable(
    const ePointF& from, const ePointF& to) const {
    return mMap->walkable(from, to);
}

bool eServerArea::obstacle(
    const ePointF& pos) const {
    return mMap->obstacle(pos);
}

bool eServerArea::obstacle(
    const ePointF& from, const ePointF& to) const {
    return mMap->obstacle(from, to);
}

bool eServerArea::addClient(const uint32_t clientId,
                            eCharacter& c,
                            eTeamId& teamId,
                            ePointF& spawnPos,
                            std::vector<eBody>& bodies,
                            const eScreenDimensions& screenDims) {
    const int typeId = 0;
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const std::map<std::string, std::string> partsMap{{"whole", "light"}};
    const auto modelParts = data.mapToModelParts(partsMap);
    auto& map = mMap->pathFinderMap();
    const auto u = std::make_shared<eServerUnit>(
        eUnitType::slayer, data, typeId, *this);
    u->setClass(c.classId());
    sSlayers[clientId] = u;
    u->addSkill();
    u->addSkill();
    spawnPos = mMap->spawnPos();
    findPlaceForUnit(spawnPos, spawnPos);
    teamId = eTeams::addTeam(clientId);
    iniSetupUnit(u, clientId, teamId, spawnPos,
                 typeId, udata, data, modelParts);
    iniSetupSlayerAction(u);
    auto& eq = c.equipment();
    eq.iterateOverAll([](eItem& item) {
        if(item.fType == eItemType::none) return;
        eItemGenerator::applyItemId(item);
    });
    u->setEquipment(eq, false);
    const auto& attrs = c.attributes();
    u->setAttributes(attrs, false);
    const auto& skillLevels = c.skillLevels();
    u->setSkillLevels(skillLevels, false);
    u->recalculateStats();
    u->recalculateAuras();

    auto& clientData = mClientData[clientId];
    clientData.fLatestMissile = 0;
    clientData.fLatestNova = 0;
    clientData.fLatestSkillArea = 0;
    clientData.fScreen = screenDims;
    const auto area = unitArea(*u);
    clientData.fArea = area;
    const int diff = eDifficulties::sDifficulty;
    const auto& quests = c.quests(diff);
    clientData.fQuests = quests;
    clientData.fMerc = c.merc();

    for(auto& eq : c.bodies()) {
        eq.iterateOverBody([](eItem& item) {
            if(item.fType == eItemType::none) return;
            eItemGenerator::applyItemId(item);
        });
        auto& body = bodies.emplace_back();
        body.fMapId = mMap->id();
        body.fEq = eq;
        spawnBody(clientId, eq, body.fBodyId, body.fPos);
    }

    checkQuestItems(clientId);

    return true;
}

bool eServerArea::addClient(
    const uint32_t clientId,
    const std::shared_ptr<eServerUnit>& u,
    const std::vector<std::shared_ptr<eServerUnit>>& followers,
    const eClientData& srcData,
    const eMoveToMapData& moveData,
    ePointF& spawnPos) {
    switch(moveData.fType) {
    case eMoveToMapType::waypoint: {
        const auto& to = moveData.fTo;
        const auto toArea = to.fAreaId;
        const bool r = mMap->waypointPosition(
            toArea, spawnPos);
        if(!r) return false;
    } break;
    case eMoveToMapType::portal: {
        const auto pid = moveData.fPortalId;
        const auto p = ePortal::portal(pid);
        if(!p) return false;
        const bool camp = p->fCampPortalId == pid;
        spawnPos = camp ? p->fOutdoorPos : p->fCampPos;
    } break;
    case eMoveToMapType::entrance: {
        const bool r = mMap->spawnPos(
            moveData.fFrom, spawnPos);
        if(!r) return false;
    } break;
    case eMoveToMapType::spawn:
    case eMoveToMapType::respawn: {
        spawnPos = mMap->spawnPos();
        u->respawn();
    } break;
    }

    findPlaceForUnit(spawnPos, spawnPos);
    iniSetupUnit(u, spawnPos);
    iniSetupSlayerAction(u);
    u->setBlockingActionTime(0.f);
    u->updateAll();

    for(const auto& f : followers) {
        findPlaceForUnit(spawnPos, spawnPos);
        iniSetupUnit(f, spawnPos);
        iniSetupFollowerAction(f, u);
        f->setBlockingActionTime(0.f);
        f->updateAll();
    }

    auto& clientData = mClientData[clientId];
    clientData.fScreen = srcData.fScreen;
    clientData.fArea = unitArea(*u);
    clientData.fKnownUnits.clear();
    clientData.fKnownItems.clear();
    clientData.fLatestMissile = 0;
    clientData.fLatestNova = 0;
    clientData.fLatestSkillArea = 0;
    clientData.fKnownMap.clear();
    clientData.fUpdateBoostsAuras = srcData.fUpdateBoostsAuras;
    clientData.fQuests = srcData.fQuests;
    clientData.fSendQuests = srcData.fSendQuests;
    clientData.fMerc = srcData.fMerc;
    clientData.fFollowersState = srcData.fFollowersState;
    clientData.fUsedSkills = srcData.fUsedSkills;

    return true;
}

bool eServerArea::findPlaceForUnit(
    const ePointF& pos, ePointF& result) const {
    const float x = pos.fX;
    const float y = pos.fY;
    for(int dist = 0; dist < 5; dist++) {
        const int maxTries = dist == 0 ? 1 : 10;
        for(int i = 0; i <= maxTries; i++) {
            const float dx = eRand::randF(-dist, dist);
            const float dy = eRand::randF(-dist, dist);
            const ePointF tryPos{x + dx, y + dy};
            const auto u = unit(tryPos, [](const eServerUnit& u) {
                return u.fHealth > 0;
            });
            if(u) continue;
            const bool w = walkable(tryPos);
            if(!w) continue;
            result = tryPos;
            return true;
        }
    }
    return false;
}

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

bool eServerArea::requestSeller(
    const uint32_t clientId,
    const uint32_t sellerId,
    eSeller& seller) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& ss = eSellers::sSellers;
    auto it = ss.find(sellerId);
    if(it == ss.end()) return false;
    auto& s = it->second;
    {
        auto& cp = s.fClientPage;
        const auto it = cp.find(clientId);
        if(it == cp.end()) {
            auto& p = s.addClientPage(clientId);
            const auto& potionTypes = s.fSellPotionTypes;
            for(const auto ptype : potionTypes) {
                const auto& itemData = eItemsData::get(ptype);
                if(itemData.fType != eItemType::potion) continue;
                eItem item;
                eItemGenerator::applyItemId(item);
                const auto type = itemData.fType;
                item.fDataId = ptype;
                item.fType = type;
                item.fSubType = itemData.fSubtype;
                p.tryAdd(item);
            }
        }
    }
    seller = s;
    return true;
}

uint32_t eServerArea::nearestCorpse(const ePointF& pos) const {
    uint32_t result = 0;
    const float maxDist = 3.f;
    float minDist = maxDist;
    const auto iter = [&](
        const std::shared_ptr<eServerUnit>& u) {
        const bool r = u->isCorpse();
        if(!r) return false;
        const auto& upos = u->fPos;
        const float dist = ePointF::distance(upos, pos);
        if(dist > minDist) return false;
        minDist = dist;
        result = u->fCharId;
        return false;
    };
    iterateOverUnits(pos, maxDist, iter);
    return result;
}

std::optional<eFollowersBase>
eServerArea::followersUpdate(const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return std::nullopt;
    const auto& eq = u->equipment();
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return std::nullopt;
    auto& c = it->second;
    const auto& src = u->followers();
    if(src.fState <= c.fFollowersState) return std::nullopt;
    c.fFollowersState = src.fState;
    return src;
}

void eServerArea::clear() {
    sSlayers.clear();
    sSlain.clear();
    eServerUnit::sNextCharId = 1;
    eItemGenerator::clear();
}

bool eServerArea::checkQuestItems(
    const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto& eq = u->equipment();
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& c = it->second;
    const auto& iqs = eQuests::sFindItemQuests;
    for(const auto& it : iqs) {
        const auto itemId = it.first;

        uint8_t count = 0;
        eq.iterateOverAll([&](const eItem& item) {
            if(item.fDataId == itemId) count++;
        });

        const auto& qids = it.second;
        for(const auto& qid : qids) {
            const bool r = c.fQuests.setCount(
                qid.fQuestId, qid.fStageId, count);
            if(r) c.fSendQuests = true;
        }
    }
    return true;
}

bool goThroughPortal(
    const uint32_t clientId,
    const uint32_t portalId) {
    const auto p = ePortal::portal(portalId);
    if(!p) return false;
    if(p->fCreator != clientId) return false;
    if(p->fCampPortalId != portalId) return false;
    ePortal::removePortal(portalId);
    return true;
}

bool eServerArea::moveClient(
    const uint32_t clientId,
    eServerArea& from,
    eServerArea& to,
    const eMoveToMapData& moveData,
    ePointF& spawnPos) {
    const auto u = from.unit(clientId);
    if(!u) return false;
    const auto clientData = from.mClientData[clientId];
    std::vector<std::shared_ptr<eServerUnit>> followers;
    for(const auto f : u->followers()) {
        const auto u = from.unit(f);
        if(!u) continue;
        followers.emplace_back(u);
    }
    if(&from != &to) from.clientMoved(clientId);
    const bool r = to.addClient(
        clientId, u, followers,
        clientData, moveData, spawnPos);
    if(!r) return false;
    if(moveData.fType == eMoveToMapType::portal) {
        goThroughPortal(clientId, moveData.fPortalId);
    }
    return true;
}

bool eServerArea::spawnBody(const uint32_t clientId,
                            const eBodyEquipment& beq,
                            uint32_t& bodyId,
                            ePointF& spawnPos) {
    const auto client = unit(clientId);
    if(!client) return false;
    const auto& data = client->data();
    const int typeId = 0;
    const auto u = std::make_shared<eServerUnit>(
        eUnitType::slayerBody, data, typeId, *this);
    eEquipment bodyEq;
    static_cast<eBodyEquipment&>(bodyEq) = beq;
    u->setEquipment(bodyEq, false);
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const uint32_t charId = eServerUnit::sNextCharId++;
    const auto& modelParts = client->fModelParts;
    const auto teamId = client->fTeamId;
    spawnPos = client->fPos;
    iniSetupUnit(u, charId, teamId, spawnPos,
                 typeId, udata, data, modelParts);
    u->fHealth = 0;
    u->fAnim = data.animId("body");
    bodyId = charId;
    mBodies[clientId].emplace_back(charId);
    return true;
}

bool eServerArea::createBody(
    const uint32_t clientId,
    eBody& body) {
    const auto client = unit(clientId);
    if(!client) return false;
    const bool createBody = true;
    if(!createBody) return false;
    auto& eq = client->equipment();
    const auto beq = eq.takeBody();
    body.fMapId = mMap->id();
    body.fEq = beq;
    const bool r = spawnBody(
        clientId, beq,
        body.fBodyId, body.fPos);

    dropGold(clientId, eq.fInventoryGold);

    return r;
}

bool eServerArea::removeClient(const uint32_t clientId) {
    sSlayers.erase(clientId);
    planRemoveUnit(clientId);
    const auto& bodies = mBodies[clientId];
    for(const auto bodyId : bodies) {
        planRemoveUnit(bodyId);
    }
    mBodies.erase(clientId);
    const int r = mClientData.erase(clientId);
    return r > 0;
}

bool eServerArea::clientMoved(const uint32_t clientId) {
    removeUnit(clientId);
    const int r = mClientData.erase(clientId);
    return r > 0;
}

bool eServerArea::planRemoveUnit(const uint32_t charId) {
    mUnitsToRemove.emplace_back(charId);
    return true;
}

bool eServerArea::removeUnit(const uint32_t charId) {
    const auto area = unitArea(charId);
    if(!mUnitAreas.hasArea(area)) return false;
    mUnitAreas.erase(area, charId);
    mUnits.remove(charId);
    return true;
}

bool eServerArea::pickupBody(
    const uint32_t clientId,
    const uint32_t bodyId,
    bool& bodyRemoved,
    eBodyItemsTaken& taken) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& client = it->second;
    auto& bodies = mBodies[clientId];
    const auto bit = std::find(bodies.begin(), bodies.end(), bodyId);
    if(bit == bodies.end()) return false;
    const auto ubody = unit(bodyId);
    if(!ubody) return false;
    const auto u = unit(clientId);
    if(!u || u->fHealth <= 0) return false;
    const float dist = ePointF::distance(ubody->fPos, u->fPos);
    if(dist > 1.f) return false;
    auto& dst = u->equipment();
    auto& src = ubody->equipment();
    dst.moveFromBody(src, &taken.fItems);
    taken.fBodyId = bodyId;
    bodyRemoved = src.bodyEmpty();
    if(bodyRemoved) {
        bodies.erase(bit);
        planRemoveUnit(bodyId);
    }
    u->recalculateStats();
    u->recalculateAuras();
    return true;
}

bool eServerArea::changeTeam(
    const uint32_t clientId, const eTeamId newTeam) {
    const auto u = unit(clientId);
    if(!u) return false;
    u->setTeamId(newTeam);
    const auto& followers = u->followers();
    for(const auto f : followers) {
        const auto u = unit(f);
        if(!u) continue;
        u->setTeamId(newTeam);
    }
    return true;
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

bool eServerArea::triggerObject(
    const uint32_t clientId, eServerObject& obj) {
    const auto u = unit(clientId);
    if(!u) return false;
    if(obj.fMapId != mMap->id()) return false;
    const auto& pos = obj.fPos;
    const int tx = pos.fX;
    const int ty = pos.fY;
    if(!mMap->inside(tx, ty)) return false;
    const int areaMId = mMap->areaAt(pos);
    if(areaMId < 0) return false;
    const auto& area = mMap->area(areaMId);
    const auto mapId = area.fMapId;
    const auto areaId = area.fAreaId;
    const auto& mapSett = eMapsSettings::sMaps.get(mapId);
    const int diff = eDifficulties::sDifficulty;
    const auto& areaSett = mapSett.fAreas.get(areaId).fDiffs.at(diff);
    const auto level = areaSett.fLevel;
    const auto& objIds = mMap->objects(tx, ty);
    for(const auto id : objIds) {
        const auto& sobj = mMap->object(id);
        const auto objId = sobj->fObjectId;
        if(objId != sobj->fObjectId) continue;
        const auto type = sobj->fObjectType;
        const auto& info = eObjectsInfo::sObjects.get(type);
        if(info.fKey >= 0) {
            const auto& eq = u->equipment();
            bool found = false;
            eq.iterateOverAll([&](const eItem& item) {
                if(item.fDataId == info.fKey) {
                    found = true;
                }
            });
            if(!found) return false;
        }
        switch(info.fType) {
        case eObjectType::treasure: {
            auto& state = sobj->fState;
            if(state != 0) return false;
            const float fx = tx + sobj->fWidth + 0.5f;
            const ePointF pos{fx, float(ty)};
            generateItems(pos, level, 7.5f);

            const int diff = eDifficulties::sDifficulty;
            const auto& itemMap = info.fItemTypes;
            const auto it = itemMap.find(diff);
            if(it != itemMap.end()) {
                const auto& items = it->second;
                for(const auto typeId : items) {
                    const auto item = eItemGenerator::generateItem(typeId, level, 7.5f);
                    addGroundItem(pos, item);
                }
            }
            state = 1;
        } break;
        case eObjectType::trapDoor: {
            auto& state = sobj->fState;
            if(state != 0) return false;
            state = 1;
        } break;
        case eObjectType::healer: {
            u->healAll();
        } break;
        default:
            break;
        }
        static_cast<eObject&>(obj) = *sobj;
        return true;
    }
    return false;
}

bool eServerArea::triggerDoors(
    const uint32_t clientId, const eServerDoors& doors) {
    if(doors.fMapId != mMap->id()) return false;
    mMap->triggerDoors(doors);
    return true;
}

bool eServerArea::pickupItem(
    const uint32_t clientId,
    const uint32_t itemId,
    const bool drag,
    eEquipmentAction& action) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto itemPtr = mItemsOnGround.get(itemId);
    if(!itemPtr) return false;
    auto& item = action.fAddItem;
    action.fUnitId = clientId;
    item = *itemPtr;
    const auto gitem = mGroundItems.get(itemId);
    const auto area = itemArea(itemId);
    const auto tile = itemTile(itemId);
    auto& eq = u->equipment();
    if(gitem->fType == eItemType::gold) {
        eq.fInventoryGold += item.fCount;

        action.fType = eEquipmentActionType::gold;
        action.fInvGold = eq.fInventoryGold;
        action.fStashGold = eq.fStashGold;
    } else {
        action.fType = eEquipmentActionType::add;
        auto& aplace = action.fPlace;
        if(drag) {
            if(eq.fDragged.fType != eItemType::none) return false;
            aplace.fType = ePlaceType::dragged;
            eq.fDragged = item;
        } else {
            const auto& stats = u->stats();
            const bool met = stats.itemReqsMet(item);
            const bool r = eq.add(item, met, &aplace);
            if(!r) return false;
            u->recalculateStats();
            u->recalculateAuras();
        }

        checkQuestItems(clientId);
    }
    mGroundItems.remove(itemId);
    mItemsOnGround.remove(itemId);
    mItemAreas.erase(area, itemId);
    mItemTiles.erase(tile, itemId);
    return true;
}

bool eServerArea::dropItem(const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    const auto pos = u->fPos;
    const auto tryDropItem = [&](eItem& item) {
        if(item.fType != eItemType::none) {
            addGroundItem(pos, item);
            item = eItem();
        }
    };
    tryDropItem(eq.fDragged);
    tryDropItem(eq.fTemporary);
    checkQuestItems(clientId);
    return true;
}

bool eServerArea::dropGold(const uint32_t clientId,
                           uint32_t count) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    const auto pos = u->fPos;
    count = std::min(eq.fInventoryGold, count);
    if(count <= 0) return false;
    eq.fInventoryGold -= count;
    const auto item = eItemGenerator::generateGold(count);
    addGroundItem(pos, item);
    return true;
}

void eServerArea::rearrangeItems(
    const uint32_t clientId, const eEquipment& eq) {
    const auto u = unit(clientId);
    if(!u) return;
    u->setEquipment(eq);
}

bool eServerArea::equipmentAction(
    const uint32_t clientId,
    const eEquipmentAction& a) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    if(a.fUnitId == clientId) {
        const bool r = a.apply(eq, eq.fDragged);
        u->recalculateStats();
        return r;
    } else {
        const auto u = unit(a.fUnitId);
        if(!u) return false;
        auto& ueq = u->equipment();
        const bool r = a.apply(ueq, eq.fDragged);
        u->recalculateStats();
        return r;
    }
}

bool eServerArea::buyAction(
    const uint32_t clientId,
    const eBuyAction& a,
    uint32_t& newItemId) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    auto item = eSellers::item(
        clientId, a.fSellerId, a.fItemId);
    const uint32_t gold = item.calculateCost();
    const uint32_t hgold = eq.totalGold();
    if(gold > hgold) return false;
    if(item.fType == eItemType::none) return false;
    const bool r = eEquipmentAction::add(
        eq, item, a.fPlace);
    if(!r) return false;
    u->recalculateStats();
    eq.takeGold(gold);
    if(item.fType == eItemType::potion) {
        eReplaceItemId r;
        r.fSellerId = a.fSellerId;
        r.fOldItemId = item.fItemId;
        eItemGenerator::applyItemId(item);
        newItemId = item.fItemId;
        r.fNewItemId = newItemId;
        eSellers::replaceItemId(
            clientId, r);
    } else {
        newItemId = 0;
        eSellers::takeItem(
            clientId, a.fSellerId, a.fItemId);
    }
    return true;
}

bool eServerArea::sellAction(
    const uint32_t clientId,
    const eSellAction& a) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& ss = eSellers::sSellers;
    const auto it = ss.find(a.fSellerId);
    if(it == ss.end()) return false;
    auto& s = it->second;
    auto& eq = u->equipment();
    const auto item = eq.take(a.fItemId);
    if(item.fType == eItemType::none) return false;
    const uint32_t gold = item.calculateSellCost();
    eq.fInventoryGold += gold;
    auto& p = s.fClientPage[clientId];
    p.tryAdd(item);
    return true;
}

void eServerArea::changeAttributes(
    const uint32_t clientId, const eAttributes& attrs) {
    const auto u = unit(clientId);
    if(!u) return;
    const auto& dst = u->attributes();
    const bool r = eAttributes::samePoints(attrs, dst);
    if(r) u->setAttributes(attrs);
}

void eServerArea::changeSkillLevels(
    const uint32_t clientId, const eSkillLevels& skillLevels) {
    const auto u = unit(clientId);
    if(!u) return;
    const auto& stats = u->stats();
    const bool r = stats.validLevelsChange(skillLevels);
    if(r) u->setSkillLevels(skillLevels);
}

void eServerArea::consumePotion(
    const uint32_t clientId,
    const uint32_t itemId,
    const uint32_t unitId) {
    const auto u = unit(clientId);
    if(!u) return;
    const auto p = u->takePotion(itemId);
    const auto tu = unit(unitId);
    if(!tu) return;
    tu->consumePotion(p);
}

eMissileData
eServerArea::missileData(const uint32_t clientId) {
    eMissileData result;
    const auto u = unit(clientId);
    if(!u) return result;
    const auto& upos = u->fPos;
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return result;
    auto& newMissiles = result.fNewMissiles;
    newMissiles.reserve(mMissiles.actualSize());
    auto& clientData = it->second;
    auto& latestMissile = clientData.fLatestMissile;
    auto newLatestMissile = latestMissile;
    for(const auto& m : mMissiles) {
        const auto& mref = *m;
        if(mref.fId <= latestMissile) {
            if(mref.needsUpdate()) {
                const auto update = mref.extractUpdate();
                result.fUpdates.emplace_back(update);
            }
            continue;
        }
        newLatestMissile = std::max(newLatestMissile, mref.fId);
        const float dist = ePointF::distance(mref.fPos, upos);
        if(dist > 20.f) continue;
        newMissiles.emplace_back(mref);
    }
    latestMissile = newLatestMissile;
    return std::move(result);
}

std::vector<eNova>
eServerArea::novaData(const uint32_t clientId) {
    std::vector<eNova> result;
    const auto u = unit(clientId);
    if(!u) return result;
    result.reserve(mNovas.actualSize());
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return result;
    auto& clientData = it->second;
    auto& latestNova = clientData.fLatestNova;
    auto newLatestNova = latestNova;
    for(const auto& n : mNovas) {
        if(n->fId <= latestNova) continue;
        newLatestNova = std::max(newLatestNova, n->fId);
        const float dist = ePointF::distance(n->fCenter, u->fPos);
        if(dist > 20.f) continue;
        result.emplace_back(*n);
    }
    latestNova = newLatestNova;
    return result;
}

std::vector<eSkillArea>
eServerArea::skillAreaData(const uint32_t clientId) {
    std::vector<eSkillArea> result;
    const auto u = unit(clientId);
    if(!u) return result;
    result.reserve(mSkillAreas.actualSize());
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return result;
    auto& clientData = it->second;
    auto& latestSkillArea = clientData.fLatestSkillArea;
    auto newLatestSkillArea = latestSkillArea;
    for(const auto& a : mSkillAreas) {
        if(a->fId <= latestSkillArea) continue;
        newLatestSkillArea = std::max(newLatestSkillArea, a->fId);
        const float dist = ePointF::distance(a->fPos, u->fPos);
        if(dist > 20.f) continue;
        result.emplace_back(*a);
    }
    latestSkillArea = newLatestSkillArea;
    return result;
}

bool eServerArea::boostsAurasChanged(const uint32_t clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    clientData.fUpdateBoostsAuras = true;
    return true;
}

bool eServerArea::updateBoostsAuras(const uint32_t clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    const bool result = clientData.fUpdateBoostsAuras;
    clientData.fUpdateBoostsAuras = false;
    return result;
}

std::multimap<eBoostCurseType, eModifier>
eServerArea::boosts(const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return {};
    const auto& stats = u->stats();
    return stats.fBoosts;
}

std::multimap<eAuraType, eModifier>
eServerArea::auras(const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return {};
    const auto& stats = u->stats();
    return stats.fAuraBoosts;
}

std::optional<eSlayerQuests>
eServerArea::quests(const uint32_t clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return std::nullopt;
    auto& clientData = it->second;
    if(!clientData.fSendQuests) return std::nullopt;
    clientData.fSendQuests = false;
    return clientData.fQuests;
}

std::optional<eMercenary>
eServerArea::merc(const uint32_t clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return std::nullopt;
    auto& clientData = it->second;
    const auto result = clientData.fMerc;
    clientData.fMerc->fUpdate = 0;
    return result;
}

bool eServerArea::heardTalk(
    const uint32_t clientId,
    const eConvoId& talk) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    const auto u = unit(clientId);
    if(!u) return false;
    auto& qs = clientData.fQuests;

    {
        const auto& c = eTalks::get(talk);
        const auto& qinfo = eQuests::sQuests.get(c.fQuestId);
        const auto stepId = qinfo.stageToStep(c.fStageId);
        const auto& step = qinfo.fSteps[stepId];
        if(step.fType == eQuestType::bringItem) {
            auto& eq = u->equipment();
            std::vector<uint32_t> items;
            eq.iterateOverAll([&](const eItem& item) {
                if(items.size() >= step.fCount) return;
                if(item.fDataId == step.fTargetItem) {
                    items.emplace_back(item.fItemId);
                }
            });
            if(items.size() >= step.fCount) {
                const bool r = qs.nextStage(c.fQuestId);
                if(r) {
                    for(const auto itemId : items) {
                        eq.take(itemId);
                    }
                    checkQuestItems(clientId);
                    clientData.fSendQuests = true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    qs.heardTalk(talk);
    checkQuestItems(clientId);
    clientData.fSendQuests = true;
    return true;
}

bool eServerArea::addedSocket(
    const uint32_t clientId,
    const uint8_t questId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    auto& tmp = eq.fTemporary;
    const bool r = tmp.addSocket();
    if(!r) return false;
    auto& clientData = it->second;
    auto& qs = clientData.fQuests;
    qs.addedSocket(questId);
    checkQuestItems(clientId);
    clientData.fSendQuests = true;
    return true;
}

void eServerArea::addSkillArea(
    const std::shared_ptr<eServerSkillArea>& a) {
    mSkillAreas.add(a->fId, a);
}

void eServerArea::addMissile(const std::shared_ptr<eServerMissile>& m) {
    mMissiles.add(m->fId, m);
}

void eServerArea::addNova(const std::shared_ptr<eServerNova>& n) {
    mNovas.add(n->fId, n);
}

uint32_t eServerArea::findOtherTarget(
    const eServerUnit& u,
    const float range,
    const std::set<uint32_t>& skip) {
    uint32_t result = 0;
    const auto uid = u.fCharId;
    const auto& upos = u.fPos;
    iterateOverUnitsClamped(upos, range, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto id = u->fCharId;
        if(id == uid) return false;
        if(skip.count(id) > 0) return false;
        result = id;
        return true;
    });
    return result;
}

uint32_t eServerArea::findMinOtherTarget(
    const eServerUnit& u,
    const float range,
    const std::map<uint32_t, uint8_t>& skip) {
    uint32_t result = 0;
    uint8_t min = 255;
    const auto uid = u.fCharId;
    const auto& upos = u.fPos;
    iterateOverUnitsClamped(upos, range, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto id = u->fCharId;
        if(id == uid) return false;
        const auto it = skip.find(id);
        if(it == skip.end()) {
            min = 0;
            result = id;
            return true;
        }
        const auto imin = it->second;
        if(imin < min) {
            min = imin;
            result = id;
        }
        return false;
    });
    return result;
}

int piercedFromPierceChance(const float p) {
    if(p <= 0.f) return 0;
    if(p >= 1.f) return std::numeric_limits<int>::max();
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

void eServerArea::spawnMissile(const ePointF& to,
                               const eSkill& skill,
                               const eHitData& data,
                               const int nMissiles,
                               const float pierceChance,
                               const int missileId,
                               const float range,
                               const float radius,
                               const float time,
                               const bool continuousDamage,
                               const int consecutive) {
    const auto skillType = skill.fType;
    const auto twinBehaviour = skill.fTwinBehaviour;
    const bool avoid = twinBehaviour != eTwinBehaviour::none;
    auto baseDir = ePointF::vector(to, data.fFrom);
    if(baseDir.length() < 0.001f) baseDir = eVec2f::random();
    struct eMissileData {
        ePointF fPos;
        ePointF fTo;
        int fToPierce;
        int fMissileId;
        float fRange;
        float fTime;
        eDamage fDamage;
    };
    std::vector<eMissileData> missiles;
    const auto spawnMissiles = [&](const int missileId,
                                   const float range) {
        float maxAngle = skill.fMaxAngle;
        if(nMissiles > 1 && maxAngle == 0.f) {
            maxAngle = 30.f*(nMissiles - 1);
        }
        if(skill.fAngleAdjust) {
            if(range > 0.f) {
                const float len = baseDir.length();
                const float multBase = 1.f - 3.f*len/range;
                const float angleMult = std::clamp(multBase, 0.1f, 1.f);
                maxAngle *= angleMult;
            }
        }
        float angle = nMissiles == 1 ? 0.f : -0.5f*maxAngle;
        for(int i = 0; i < nMissiles; i++) {
            auto dir = baseDir;
            if(angle != 0.f) dir.rotate(angle);
            auto& md = missiles.emplace_back();
            const int max = std::numeric_limits<uint8_t>::max();
            const int pierced = piercedFromPierceChance(pierceChance);
            md.fToPierce = std::min(max, 1 + pierced);
            md.fPos = data.fFrom;
            md.fTo = data.fFrom + dir;
            md.fMissileId = missileId;
            md.fRange = range;
            md.fDamage = data.fDamage;
            if(nMissiles > 1) {
                angle += maxAngle/(nMissiles - 1);
            }
        }
    };
    if(skillType == eSkillType::missile) {
        spawnMissiles(missileId, range);
    } else if(skillType == eSkillType::wall) {
        eVec2f perp(-baseDir.y, baseDir.x);
        perp.normalize(2*radius);
        ePointF pt = to - perp * (nMissiles/2);
        for(int i = 0; i < nMissiles; i++) {
            auto& md = missiles.emplace_back();
            md.fToPierce = 0;
            md.fPos = pt;
            md.fTo = pt;
            md.fMissileId = skill.fMissileId;
            md.fTime = skill.fTime;
            md.fRange = 0.f;
            md.fDamage = data.fDamage;
            pt = pt + perp;
        }
    } else {
        spawnMissiles(missileId, range);
    }
    std::vector<std::shared_ptr<eServerMissile>> twins;
    for(const auto& md : missiles) {
        const auto m = std::make_shared<eServerMissile>();
        if(avoid) twins.emplace_back(m);
        auto& mref = *m;
        mref.fType = md.fMissileId;
        mref.fTeamId = data.fAttackTeamId;
        mref.fToPierce = md.fToPierce;
        mref.fSpeed = skill.fSpeed;

        mref.fTotalDist = md.fRange;
        mref.fTotalTime = md.fTime;
        mref.fRemDist = mref.fTotalDist;
        mref.fRemTime = mref.fTotalTime;

        mref.fPathType = skill.fPathId;
        mref.fFrom = data.fFrom;
        mref.fRadius = radius;
        mref.fPos = md.fPos;
        mref.fTo = md.fTo;
        mref.fContinuousDamage = continuousDamage;
        mref.fConsecutive = consecutive;
        mref.fEnemyFindRange = skill.fMissileEnemyFindRange;
        mref.fTwinBehaviour = twinBehaviour;
        mref.fTime = 0.f;

        struct eCharSkipper {
            float fTimeSkip = 0.f;
            float fTime = 0.f;
            std::set<uint32_t> fChars;
        };

        std::shared_ptr<eCharSkipper> skip;
        if(continuousDamage) {
            skip = std::make_shared<eCharSkipper>();
        } else if(consecutive > 0) {
            skip = std::make_shared<eCharSkipper>();
            skip->fTimeSkip = 1000.f;
        }
        m->fHitAction = [data, skip](
            eServerUnit& u, const eServerMissile& m) {
            if(skip) {
                auto& c = skip->fChars;
                if(skip->fTime < m.fTime) {
                    c.clear();
                    skip->fTime = m.fTime + skip->fTimeSkip;
                } else {
                    if(c.find(u.fCharId) != c.end()) {
                        return;
                    }
                }
                c.emplace(u.fCharId);
            }
            u.getHit(data);
        };
        addMissile(m);
    }

    for(const auto& m : twins) {
        for(const auto& twin : twins) {
            if(m == twin) continue;
            const auto id = twin->fId;
            m->fTwinMissiles.emplace(id);
        }
    }
}

void eServerArea::spawnArea(const ePointF& to,
                            const eSkill& skill,
                            eHitData data,
                            const float radius,
                            const int missileId) {
    const int area = skill.fAreaMissileId;
    if(area <= 0) return;
    const auto a = std::make_shared<eServerSkillArea>();
    data.fDamage = data.fDamage/25.f;
    data.fHeal /= 25.f;
    a->fIncrement = [this, data, to, radius]() {
        const auto team = data.fAttackTeamId;
        iterateOverUnitsClamped(to, radius,
                [&](const std::shared_ptr<eServerUnit>& u) {
            if(u->fHealth <= 0) return false;
            const auto uteam = u->fTeamId;
            const auto& pos = u->fPos;
            if(team == uteam) {
                u->restoreHealth(data.fHeal);
            } else {
                if(!eTeams::areEnemies(team, uteam)) return false;
                u->getHit(data);
            }
            return false;
        });
    };
    a->fMissileId = area;
    a->fRemTime = skill.fTime;
    a->fRadius = radius;
    a->fPos = to;
    addSkillArea(a);
}

void eServerArea::spawnNova(const eSkill& skill,
                            const eHitData& data,
                            const float radius,
                            const bool continuousDamage) {
    const auto n = std::make_shared<eServerNova>();
    n->fTeamId = data.fAttackTeamId;
    n->fMissileType = skill.fMissileId;
    if(skill.fTargetCorpse) {
        const auto corpseId = nearestCorpse(data.fTo);
        if(!corpseId) return;
        const auto c = unit(corpseId);
        n->fCenter = c->fPos;
        c->explodeCorpse();
    } else {
        n->fCenter = data.fFrom;
    }
    n->fRadius = 0.f;
    n->fMaxRadius = radius;
    n->fSpeed = skill.fSpeed;
    n->fNMissiles = skill.fNovaMissiles;

    struct eCharSkipper {
        std::set<int> fChars;
    };

    const std::shared_ptr<eCharSkipper> skip =
        !continuousDamage ?
            std::make_shared<eCharSkipper>() :
            nullptr;
    n->fHitAction = [data, skip](eServerUnit& u) {
        if(skip) {
            auto& c = skip->fChars;
            if(c.find(u.fCharId) != c.end()) {
                return;
            }
            c.emplace(u.fCharId);
        }
        u.getHit(data);
    };
    addNova(n);
}

void eServerArea::summon(eServerUnit& by,
                         const uint32_t corpseId,
                         const int unitId,
                         const int maxCount,
                         const std::vector<eModifier>& mods) {
    const auto corpse = unit(corpseId);
    if(!corpse) return;
    const auto& to = corpse->fPos;
    summon(by, to, unitId, maxCount, mods);
    planRemoveUnit(corpseId);
}

void eServerArea::summon(eServerUnit& by,
                         ePointF to,
                         const int unitId,
                         const int maxCount,
                         const std::vector<eModifier>& mods) {
    const bool r = findPlaceForUnit(to, to);
    if(!r) return;
    auto& followers = by.followers();
    const auto summoned = eServerArea::summoned(by, unitId);
    if(maxCount > 0 && summoned.size() >= maxCount) {
        const uint32_t removeCharId = summoned[0];
        planRemoveUnit(removeCharId);
        followers.remove(removeCharId);
    }
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto modelParts = data.randomModelParts();

    const auto u = std::make_shared<eServerUnit>(
        eUnitType::summoned, data, unitId, *this);
    const uint32_t charId = eServerUnit::sNextCharId++;
    followers.add(charId);
    iniSetupUnit(u, charId, by.fTeamId, to,
                 unitId, udata, data, modelParts);
    u->addBoost(mods, eBoostCurseType::permanent, false);
    {
        const int schoice = u->addSkill();
        u->setSkillId(schoice, 0, false);
    }
    u->recalculateStats();
    u->recalculateAuras();

    const auto byPtr = unit(by.fCharId);
    iniSetupFollowerAction(u, byPtr);
}

bool eServerArea::summonMerc(
    const uint32_t clientId,
    eMercenary merc) {
    const auto by = unit(clientId);
    if(!by) return false;
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    auto& cMerc = clientData.fMerc;
    if(cMerc) {
        planRemoveUnit(cMerc->fUnitId);
        cMerc = std::nullopt;
    }
    ePointF to = by->fPos;
    const bool r = findPlaceForUnit(to, to);
    if(!r) return false;
    auto& followers = by->followers();
    const auto& mdata = eMercenariesInfo::sMercs.get(merc.fMercType);
    const auto unitId = mdata.fUnitType;
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto modelParts = data.randomModelParts();

    const auto m = std::make_shared<eServerUnit>(
        eUnitType::mercenary, data, unitId, *this);
    m->setMercType(merc.fMercType);
    const auto mercId = eServerUnit::sNextCharId++;
    followers.add(mercId);
    iniSetupUnit(m, mercId, by->fTeamId, to,
                 unitId, udata, data, modelParts);
    {
        const int schoice = m->addSkill();
        m->setSkillId(schoice, 0, false);
    }

    merc.fUnitId = mercId;
    auto& eq = merc.fEq;
    eq.iterateOverAll([](eItem& item) {
        if(item.fType == eItemType::none) return;
        eItemGenerator::applyItemId(item);
    });
    m->setEquipment(eq, false);
    const auto attrs = merc.attributes();
    m->setAttributes(attrs, false);
    const auto mods = merc.mods();
    m->addBoost(mods, eBoostCurseType::merc, false);
    merc.fDead = false;
    cMerc = merc;

    m->recalculateStats();
    m->recalculateAuras();

    iniSetupFollowerAction(m, by);
    return true;
}

bool eServerArea::castChance(
    eServerUnit& by,
    const eSkillStats& o,
    const eWeaponChoice wchoice,
    const ePointF& to) {
    const bool r = eRand::randChance(o.fCastChance);
    if(!r) return false;
    cast(by, o, wchoice, to);
    return true;
}

void eServerArea::cast(eServerUnit& by,
                       const eSkillStats& o,
                       const eWeaponChoice wchoice,
                       const ePointF& to) {
    const auto& skill = eSkills::sSkills.get(o.fSkillId);
    eHitData data;
    by.hitData(o, wchoice, data);
    switch(skill.fType) {
    case eSkillType::missile:
    case eSkillType::wall: {
        const int nMissiles = by.skillCount(o, wchoice);
        const float pierceChance = by.pierceChance(o, wchoice);
        const int missileId = by.missileId(o, wchoice);
        const float missileRange = by.missileRange(o, wchoice);
        const float missileTime = by.missileTime(o, wchoice);
        const float radius = by.radius(o, wchoice);
        const bool continuousDamage = skill.fType == eSkillType::wall;
        const int consecutive = by.consecutive(o, wchoice);
        spawnMissile(to, skill, data,
                     nMissiles, pierceChance, missileId,
                     missileRange, radius, missileTime,
                     continuousDamage, consecutive);
    } break;
    case eSkillType::nova: {
        const float radius = by.radius(o, wchoice);
        const bool continuousDamage = false;
        spawnNova(skill, data, radius, continuousDamage);
    } break;
    case eSkillType::summon: {
        const int maxCount = by.skillCount(
            o, eWeaponChoice::left);
        const int unitId = skill.fUnitId;
        const auto summoned = eServerArea::summoned(by, unitId);
        const int currCount = summoned.size();
        if(maxCount > currCount) {
            summon(by, to, unitId, maxCount, {});
        }
    } break;
    case eSkillType::area:
    case eSkillType::boostCurse: {
        const float radius = by.radius(o, wchoice);
        spawnArea(to, skill, data, radius, skill.fMissileId);
    } break;
    case eSkillType::attack:
    case eSkillType::dualAttack:
    case eSkillType::aura:
    case eSkillType::shoot:
    case eSkillType::kick:
    case eSkillType::smite:
    case eSkillType::passive:
    case eSkillType::throw_:
        break;
    }

    switch(o.fExplode) {
    case eExplodeType::none:
        break;
    default:
        by.die(o.fExplode);
        break;
    }
}

std::vector<uint32_t> eServerArea::summoned(
    const eServerUnit& by, const int unitId) {
    std::vector<uint32_t> result;
    const auto& followers = by.followers();
    for(const auto charId : followers) {
        const auto u = unit(charId);
        const auto unitIdU = u->fUnitInfoId;
        if(unitIdU == unitId) {
            result.emplace_back(charId);
        }
    }
    return result;
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const uint32_t charId) const {
    return mUnits.get(charId);
}

std::shared_ptr<eGroundItem>
eServerArea::groundItem(const uint32_t itemId) const {
    return mGroundItems.get(itemId);
}

std::shared_ptr<eServerUnit> eServerArea::unit(
    const ePointF& pos, const eValidator& validator) const {
    std::shared_ptr<eServerUnit> result;

    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(validator) {
            const bool r = validator(*u);
            if(!r) return false;
        }
        result = u;
        return true;
    };

    const float maxRadius = 1.f;
    iterateOverUnitsClamped(pos, maxRadius, iter);
    return result;
}

bool eServerArea::iterateOverUnits(const eArea& areaMin,
                                   const eArea& areaMax,
                                   const eUnitIter& iter) const {
    for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
        for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
            const eArea area{ax, ay};
            if(!mUnitAreas.hasArea(area)) continue;
            const auto& units = mUnitAreas.at(area);
            for(const uint32_t charId : units) {
                const auto u = unit(charId);
                if(!u) continue;
                const bool r = iter(u);
                if(r) return true;
            }
        }
    }
    return false;
}

bool eServerArea::iterateOverUnitsClamped(
    const ePointF& pos,
    const float maxRadius,
    const eUnitIter& iter) const {
    return iterateOverUnits(pos, maxRadius, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto& upos = u->fPos;
        const double dist = ePointF::distance(pos, upos);
        if(dist > maxRadius) return false;
        return iter(u);
    });
}

bool eServerArea::iterateOverUnits(const ePointF& pos,
                                   const float maxRadius,
                                   const eUnitIter& iter) const {
    const float minX = pos.fX - maxRadius;
    const float maxX = pos.fX + maxRadius;
    const float minY = pos.fY - maxRadius;
    const float maxY = pos.fY + maxRadius;

    const auto areaMin = mUnitAreas.posArea(ePointF{minX, minY});
    const auto areaMax = mUnitAreas.posArea(ePointF{maxX, maxY});
    return iterateOverUnits(areaMin, areaMax, iter);
}

void eServerArea::unitKilled(const eServerUnit& killed) {
    {
        const auto id = killed.fUnitInfoId;
        const auto& mqs = eQuests::sKillMonsterQuests;
        const auto it = mqs.find(id);
        if(it != mqs.end()) {
            const auto& qs = it->second;
            for(auto& cit : mClientData) {
                auto& c = cit.second;

                const uint32_t clientId = cit.first;
                const auto u = unit(clientId);
                if(!u) continue;
                if(u->fHealth <= 0) continue;
                const float dist = ePointF::distance(
                    u->fPos, killed.fPos);
                if(dist > 15.f) continue;

                for(const auto& q : qs) {
                    const bool r = c.fQuests.incCount(
                        q.fQuestId, q.fStageId);
                    if(r) {
                        checkQuestItems(clientId);
                        c.fSendQuests = true;
                    }
                }
            }
        }
    }

    const int level = killed.level();
    const auto type = killed.unitType();
    float worth = 0.f;
    switch(type) {
    case eUnitType::slayer: {
        sSlain.emplace_back(killed.fCharId);
        return;
    } break;
    case eUnitType::slayerBody:
        return;
    case eUnitType::mercenary:
    case eUnitType::summoned:
        break;
    case eUnitType::normal: {
        const bool gen = eRand::randChance(0.2f);
        if(gen) worth = eRand::biasedRandF(0.25f, 10.f, 8.f);
    } break;
    case eUnitType::minion: {
        const bool gen = eRand::randChance(0.2f);
        if(gen) worth = eRand::biasedRandF(0.25f, 10.f, 5.f);
    } break;
    case eUnitType::uniqueBoss: {
        worth = eRand::biasedRandF(2.f, 10.f, 3.f);
        for(int i = 0; i < 4; i++) {
            const float pworth = eRand::randF(0.25f, 1.f);
            generatePotion(killed.fPos, level, pworth);
        }
    } break;
    }

    const float fleeRange = 5.f;
    iterateOverUnitsClamped(killed.fPos, fleeRange,
        [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        if(u->fTeamId != killed.fTeamId) return false;
        if(u->fTeamId != eTeamId::neutralFriendly &&
           u->fTeamId != eTeamId::neutralHostile &&
           u->fTeamId != eTeamId::neutral) return false;
        const auto& info = eUnitsInfo::sUnits.get(u->fUnitInfoId);
        if(eRand::randChance(info.fFleeChance)) {
            const auto& a = u->action();
            if(const auto ua = dynamic_cast<eUnitBaseAction*>(&*a)) {
                const eFlee flee{killed.fPos, info.fFleeDistance};
                ua->planFlee(flee);
            }
        }
        return false;
    });

    if(worth > 0.f) generateItem(killed.fPos, level, worth);
    for(auto& c : mClientData) {
        auto& data = c.second;
        if(data.fMerc) {
            auto& merc = *data.fMerc;
            if(merc.fUnitId == killed.fCharId) {
                merc.setDead(true);
            } else {
                const auto m = unit(merc.fUnitId);
                if(m) {
                    const float dist = ePointF::distance(m->fPos, killed.fPos);
                    if(dist <= 10.f) {
                        const auto& attrs = m->attributes();
                        m->killed(killed);
                        merc.setExp(attrs.fExp);
                        merc.setLevel(attrs.fLevel);
                    }
                }
            }
        }
        const uint32_t clientId = c.first;
        const auto u = unit(clientId);
        if(!u) continue;
        if(u->fHealth <= 0) continue;
        const eTeamId t1 = u->fTeamId;
        const eTeamId t2 = killed.fTeamId;
        if(!eTeams::areEnemies(t1, t2)) continue;
        const float dist = ePointF::distance(u->fPos, killed.fPos);
        if(dist > 10.f) continue;
        u->killed(killed);
    }
}

void eServerArea::removePlannedUnits() {
    for(const uint32_t charId : mUnitsToRemove) {
        removeUnit(charId);
    }
    mUnitsToRemove.clear();
}

eClientData::eClientData() :
    fKnownMap(eMapPortion::sBaseDim) {}
