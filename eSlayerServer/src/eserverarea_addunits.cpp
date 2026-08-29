#include "eserverarea.h"

#include "actions/enpcaction.h"
#include "actions/eunitbaseaction.h"

#include "eitemgenerator.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/eplacementhelper.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/esellers.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eitemsdata.h>

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
            const bool visible = u->visible();
            if(!visible) return false;
            handler(*u);
            return false;
        });
    };
    auto& pmap = mMap->pathFinderMap();
    m.intialize(wPos, wPath, iter, charId, teamId, pmap);
    m.setCanEnterCamp(u->fTeamId != eTeamId::neutralHostile);

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

bool eServerArea::findPlaceForUnit(
    const ePointF& pos, ePointF& result,
    const float minDist) const {
    const float x = pos.fX;
    const float y = pos.fY;
    for(int dist = 0; dist < 5; dist++) {
        const int maxTries = dist == 0 ? 1 : 10;
        for(int i = 0; i <= maxTries; i++) {
            const float dx = eRand::randF(-dist, dist);
            const float dy = eRand::randF(-dist, dist);
            const ePointF tryPos{x + dx, y + dy};
            const auto u = unit(tryPos, [tryPos, minDist](
                    const eServerUnit& u) {
                const float dist = ePointF::distance(u.fPos, tryPos);
                if(dist > minDist) return false;
                return u.fHealth > 0 && u.visible();
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

std::shared_ptr<eServerUnit> eServerArea::addUnit(
    const uint16_t type, const eUnitType utype,
    std::optional<eEliteModifiers>& mods, ePointF& pos,
    const uint8_t level) {
    const bool r = findPlaceForUnit(pos, pos, 1.f);
    if(!r) return nullptr;
    const auto& udata = eUnitsInfo::sUnits.get(type);

    bool visible = true;
    bool hideInFuture = false;
    eTeamId teamId;
    switch(udata.fNPCType) {
    case eNPCType::none: {
        teamId = eTeamId::neutralHostile;
    } break;
    default: {
        teamId = eTeamId::neutral;
        visible = sGameQuests.npcVisible(type);
        hideInFuture = sGameQuests.npcHiddenInFuture(type);
    } break;
    }

    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto modelParts = data.randomModelParts();
    auto& map = mMap->pathFinderMap();
    const auto u = std::make_shared<eServerUnit>(
        utype, data, type, *this);

    const uint32_t charId = eServerUnit::sNextCharId++;
    iniSetupUnit(u, charId, teamId, pos, type,
                 udata, data, modelParts);

    u->addBoost(udata.fModifiers, eBoostCurseType::permanent, false);

    if(mods) mods->apply(*u);

    {
        auto& stats = u->stats();
        stats.fDefaultMissileId = udata.fMissile;
        stats.fDefaultRangedRange = udata.fMissileRange;
        stats.fDefaultMeeleRange = udata.fMeeleRange;
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
        if(skillId == 0) continue;
        const int schoice = u->addSkill();
        u->setSkillId(schoice, skillId, false);
    }
    u->recalculateStats();
    u->recalculateAuras();

    switch(udata.fNPCType) {
    case eNPCType::none: {
        const auto a = std::make_shared<eUnitBaseAction>(*u, *this);
        u->setAction(a);
    } break;
    case eNPCType::wounded: {
        const auto a = std::make_shared<eNPCAction>(*u, *this);
        a->layWounded();
        u->setAction(a);
    } break;
    default: {
        const auto a = std::make_shared<eNPCAction>(*u, *this);
        u->setAction(a);
    } break;
    }

    const auto addSeller = [&](const eSellerType type) {
        auto& s = eSellers::sSellers[charId];
        s.fId = charId;
        s.fLevel = level;
        s.fType = type;
        s.fMapId = mMap->id();

        const int diff = eDifficulties::sDifficulty;

        {
            auto& itemMap = udata.fItemTypes;
            const auto it = itemMap.find(diff);
            if(it != itemMap.end()) {
                s.fSellItemTypes = it->second;
            }
        }

        {
            auto& potionMap = udata.fPotionTypes;
            const auto it = potionMap.find(diff);
            if(it != potionMap.end()) {
                s.fSellPotionTypes = it->second;
            }
        }

        const auto maxLevel = s.fLevel;
        const auto& typeIds = s.fSellItemTypes;
        if(typeIds.empty()) return;
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
    };

    switch(udata.fNPCType) {
    case eNPCType::healer:
        addSeller(eSellerType::healer);
        break;
    case eNPCType::trader:
        addSeller(eSellerType::trader);
        break;
    default:
        break;
    }

    if(!visible) {
        hideUnit(*u);
        mHiddenUnits.emplace(charId);
    }
    if(hideInFuture) {
        mFutureHideUnits.emplace(charId);
    }

    return u;
}

void eServerArea::addUnits() {
    const auto& mareas = mMap->monsterAreas();

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

    const auto& bpus = mMap->blueprintUnits();
    for(const auto& bpu : bpus) {
        const auto level = bpu.fLevel;
        std::optional<eEliteModifiers> mods;
        const auto& es = bpu.fElite;
        if(!es.empty()) {
            const auto& uinfo = eUnitsInfo::sUnits.get(bpu.fType);
            mods = eEliteModifiers();
            mods->initialize(es, uinfo.fLevel);
            mods->setBoss(es.count(0) == 0);
        }
        auto pos = bpu.fPos;
        for(int i = 0; i < bpu.fCount; i++) {
            eUnitType utype;
            if(bpu.fSuperUnique) {
                utype = eUnitType::superUniqueBoss;
            } else {
                if(mods) {
                    const bool boss = mods->boss();
                    if(boss) {
                        utype = eUnitType::uniqueBoss;
                    } else {
                        utype = eUnitType::minion;
                    }
                } else {
                    utype = eUnitType::normal;
                }
            }
            const auto u = addUnit(bpu.fType, utype, mods, pos, level);
            u->addItemDrops(bpu.fItemDrops);
        }
    }

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
        const auto level = ma.fLevel;
        const auto& types = ma.fSettings;
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
                std::optional<eEliteModifiers> mods;
                if(elite) {
                    mods = eEliteModifiers();
                    mods->initialize(1, uBaseData.fLevel);
                }

                const auto addUnit = [&]() {
                    const bool boss = mods ? mods->boss() : false;
                    auto type = us.fBaseType;
                    if(boss) {
                        type = eRand::randomElement(us.fBossTypes);
                    } else {
                        type = eRand::randomElement(us.fTypes);
                    }
                    eUnitType utype;
                    if(elite) {
                        if(boss) {
                            utype = eUnitType::uniqueBoss;
                        } else {
                            utype = eUnitType::minion;
                        }
                    } else {
                        utype = eUnitType::normal;
                    }
                    ePointF pos{xMax, yMax};
                    return eServerArea::addUnit(type, utype, mods, pos, level);
                };

                const int nUnits = us.fGroupSize;
                for(int i = 0; i < nUnits; i++) {
                    const auto u = addUnit();
                    if(i == 0) u->addItemDrops(us.fItemDrops);
                    if(!u) break;
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