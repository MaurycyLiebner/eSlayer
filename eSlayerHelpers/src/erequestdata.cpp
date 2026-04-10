#include "eSlayerHelpers/erequestdata.h"

#include "eSlayerHelpers/epacket.h"

void eRequestData::read(ePacket& p) {
    p >> fRequestId;

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

    uint16_t nNewItems;
    p >> nNewItems;
    fNewItems.reserve(fNewItems.size() + nNewItems);
    for(int i = 0; i < nNewItems; i++) {
        auto& it = fNewItems.emplace_back();
        it.read(p);
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
    p >> fLevel;
    p >> fExperience;

    uint8_t nMapPoritons;
    p >> nMapPoritons;
    fMapPortions.reserve(fMapPortions.size() + nMapPoritons);
    for(int i = 0; i < nMapPoritons; i++) {
        fMapPortions.emplace_back().read(p);
    }
}

void eRequestData::write(ePacket& p) const {
    p << fRequestId;

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

    const uint16_t nNewItems = fNewItems.size();
    p << nNewItems;
    for(const auto& i : fNewItems) {
        i.write(p);
    }

    const uint16_t nRemovedItems = fRemovedItemIds.size();
    p << nRemovedItems;
    for(const auto id : fRemovedItemIds) {
        p << id;
    }

    p << fMana;
    p << fLevel;
    p << fExperience;

    const uint8_t nMapPoritons = fMapPortions.size();
    p << nMapPoritons;
    for(const auto& mp : fMapPortions) {
        mp.write(p);
    }
}
