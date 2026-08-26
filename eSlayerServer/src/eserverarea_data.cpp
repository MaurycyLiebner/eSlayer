#include "eserverarea.h"

#include <eSlayerMapGenerator/emap.h>

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
        const bool v = u.visible();
        if(!v) return;
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
        // const float dist = ePointF::distance(n->fCenter, u->fPos);
        // if(dist > 20.f) continue;
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
        // const float dist = ePointF::distance(a->fPos, u->fPos);
        // if(dist > 20.f) continue;
        result.emplace_back(*a);
    }
    latestSkillArea = newLatestSkillArea;
    return result;
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
        newMissiles.emplace_back(mref);
    }
    for(const auto& m : mRemovedMissiles) {
        const auto& mref = *m;
        result.fRemoved.emplace_back(mref.fId);
        if(mref.fId <= latestMissile) {
            continue;
        }
        newLatestMissile = std::max(newLatestMissile, mref.fId);
        newMissiles.emplace_back(mref);
    }
    latestMissile = newLatestMissile;
    return std::move(result);
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