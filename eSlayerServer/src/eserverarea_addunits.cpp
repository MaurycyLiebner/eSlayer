#include "eserverarea.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/eplacementhelper.h>
#include <eSlayerHelpers/eunitsinfo.h>

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
            if(mods) {
                if(mods->boss()) {
                    utype = eUnitType::uniqueBoss;
                } else {
                    utype = eUnitType::minion;
                }
            } else {
                utype = eUnitType::normal;
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