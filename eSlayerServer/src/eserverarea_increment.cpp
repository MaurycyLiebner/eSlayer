#include "eserverarea.h"

#include <eSlayerHelpers/eportals.h>

#include <eSlayerMapGenerator/emap.h>

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
        const uint32_t i = it.first;
        const auto u = unit(i);
        if(!u) continue;
        auto& clientData = it.second;
        auto& oldArea = clientData.fArea;
        const auto newArea = unitArea(*u);
        if(oldArea != newArea) {
            mUnitAreas.erase(oldArea, i);
            mUnitAreas.emplace(newArea, i);
            oldArea = newArea;
            checkEnterArea(i);
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

    std::vector<uint32_t> finalRemove;
    for(auto& it : mRemovedMissilesRemTime) {
        auto& rem = it.second;
        if(rem-- == 0) {
            finalRemove.emplace_back(it.first);
        }
    }
    for(const auto id : finalRemove) {
        mRemovedMissilesRemTime.erase(id);
        mRemovedMissiles.remove(id);
    }

    std::vector<uint32_t> duplicate;
    for(const auto& m : mMissiles) {
        auto& mref = *m;
        if(mref.fConsecutive > 0 && mref.fTime > 0.f) {
            duplicate.emplace_back(mref.fId);
        }
        mMIncrementer.increment(mref, by);
    }

    for(const auto id : duplicate) {
        const auto m = mMissiles.get(id);
        if(!m) continue;
        auto& mref = *m;
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