#include "eSlayerHelpers/erequestdata.h"

#include "eSlayerHelpers/epacket.h"

bool eRequestData::read(ePacket& p, const uint32_t currentServerState) {
    uint32_t requestId;
    p >> requestId;

    uint32_t serverState;
    p >> serverState;
    if(serverState != currentServerState) return false;

    fRequestId = requestId;
    fServerState = serverState;

    uint16_t nNewUnits;
    p >> nNewUnits;
    fNewUnits.reserve(fNewUnits.size() + nNewUnits);
    for(int i = 0; i < nNewUnits; i++) {
        auto& u = fNewUnits.emplace_back();
        u.read(p);
    }

    uint16_t nUpdatedUnits;
    p >> nUpdatedUnits;
    fUpdatedUnits.reserve(fUpdatedUnits.size() + nUpdatedUnits);
    for(int i = 0; i < nUpdatedUnits; i++) {
        auto& u = fUpdatedUnits.emplace_back();
        u.read(p);
    }

    uint16_t nMissiles;
    p >> nMissiles;
    fMissiles.reserve(fMissiles.size() + nMissiles);
    for(int i = 0; i < nMissiles; i++) {
        auto& m = fMissiles.emplace_back();
        m.read(p);
    }

    uint16_t nNovas;
    p >> nNovas;
    fNovas.reserve(fNovas.size() + nNovas);
    for(int i = 0; i < nNovas; i++) {
        auto& n = fNovas.emplace_back();
        n.read(p);
    }

    uint16_t nSkillAreas;
    p >> nSkillAreas;
    fSkillAreas.reserve(fSkillAreas.size() + nSkillAreas);
    for(int i = 0; i < nSkillAreas; i++) {
        auto& a = fSkillAreas.emplace_back();
        p >> a;
    }

    p.read8(fUsedSkills);

    uint16_t nNewItems;
    p >> nNewItems;
    fNewItems.reserve(fNewItems.size() + nNewItems);
    for(int i = 0; i < nNewItems; i++) {
        p >> fNewItems.emplace_back();
    }

    uint16_t nRemovedItems;
    p >> nRemovedItems;
    fRemovedItemIds.reserve(fRemovedItemIds.size() + nRemovedItems);
    for(int i = 0; i < nRemovedItems; i++) {
        uint32_t id;
        p >> id;
        fRemovedItemIds.emplace_back(id);
    }

    p >> fMana;
    p >> fStamina;

    bool attrs;
    p >> attrs;
    if(attrs) {
        eAttributes a;
        p >> a;
        fAttributes = a;
    }

    p >> fRemainingSkillPoints;

    uint8_t nMapPoritons;
    p >> nMapPoritons;
    fMapPortions.reserve(fMapPortions.size() + nMapPoritons);
    for(int i = 0; i < nMapPoritons; i++) {
        fMapPortions.emplace_back().read(p);
    }

    p >> fUpdateBoostsAuras;

    if(fUpdateBoostsAuras) {
        fBoosts.clear();
        uint8_t nBoosts;
        p >> nBoosts;
        for(int i = 0; i < nBoosts; i++) {
            eBoostCurseType type;
            p >> type;
            eModifier mod;
            mod.read(p);
            fBoosts.emplace(type, mod);
        }

        fAuras.clear();
        uint8_t nAuras;
        p >> nAuras;
        for(int i = 0; i < nAuras; i++) {
            eAuraType type;
            p >> type;
            eModifier mod;
            mod.read(p);
            fAuras.emplace(type, mod);
        }
    }

    bool hasMerc;
    p >> hasMerc;
    if(hasMerc) {
        if(!fMerc) {
            fMerc = eMercenary();
            fMerc->read(p);
        } else {
            fMerc->readOver(p);
        }
    } else {
        fMerc = std::nullopt;
    }

    return true;
}

void eRequestData::write(ePacket& p) const {
    p << fRequestId;

    p << fServerState;

    const uint16_t nNewUnits = fNewUnits.size();
    p << nNewUnits;
    for(const auto& u : fNewUnits) {
        u.write(p);
    }

    const uint16_t nUpdatedUnits = fUpdatedUnits.size();
    p << nUpdatedUnits;
    for(const auto& u : fUpdatedUnits) {
        u.write(p);
    }

    const uint16_t nMissiles = fMissiles.size();
    p << nMissiles;
    for(const auto& m : fMissiles) {
        m.write(p);
    }

    const uint16_t nNovas = fNovas.size();
    p << nNovas;
    for(const auto& n : fNovas) {
        n.write(p);
    }

    const uint16_t nSkillAreas = fSkillAreas.size();
    p << nSkillAreas;
    for(const auto& a : fSkillAreas) {
        p << a;
    }

    p.write8(fUsedSkills);

    const uint16_t nNewItems = fNewItems.size();
    p << nNewItems;
    for(const auto& i : fNewItems) {
        p << i;
    }

    const uint16_t nRemovedItems = fRemovedItemIds.size();
    p << nRemovedItems;
    for(const auto id : fRemovedItemIds) {
        p << id;
    }

    p << fMana;
    p << fStamina;

    const bool attrs = !!fAttributes;
    p << attrs;
    if(attrs) {
        p << *fAttributes;
    }

    p << fRemainingSkillPoints;

    const uint8_t nMapPoritons = fMapPortions.size();
    p << nMapPoritons;
    for(const auto& mp : fMapPortions) {
        mp.write(p);
    }

    p << fUpdateBoostsAuras;
    if(fUpdateBoostsAuras) {
        const uint8_t nBoosts = fBoosts.size();
        p << nBoosts;
        for(const auto& it : fBoosts) {
            p << it.first;
            auto& mod = it.second;
            mod.write(p);
        }

        const uint8_t nAuras = fAuras.size();
        p << nAuras;
        for(const auto& it : fAuras) {
            p << it.first;
            auto& mod = it.second;
            mod.write(p);
        }
    }

    const bool hasMerc = !!fMerc;
    p << hasMerc;
    if(hasMerc) {
        fMerc->write(p);
    }
}
