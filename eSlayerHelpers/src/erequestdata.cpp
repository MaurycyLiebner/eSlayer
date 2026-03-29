#include "eSlayerHelpers/erequestdata.h"

#include "eSlayerHelpers/epacket.h"

void eRequestData::read(ePacket& p) {
    p >> fRequestId;

    uint16_t nNewUnits;
    p >> nNewUnits;
    for(int i = 0; i < nNewUnits; i++) {
        auto& u = fNewUnits.emplace_back();
        u.read(p);
    }

    uint16_t nUpdatedUnits;
    p >> nUpdatedUnits;
    for(int i = 0; i < nUpdatedUnits; i++) {
        auto& u = fUpdatedUnits.emplace_back();
        u.read(p);
    }

    uint16_t nMissiles;
    p >> nMissiles;
    for(int i = 0; i < nMissiles; i++) {
        auto& m = fMissiles.emplace_back();
        m.read(p);
    }

    uint16_t nNewItems;
    p >> nNewItems;
    for(int i = 0; i < nNewItems; i++) {
        auto& it = fNewItems.emplace_back();
        it.read(p);
    }

    uint16_t nRemovedItems;
    p >> nRemovedItems;
    for(int i = 0; i < nRemovedItems; i++) {
        uint32_t id;
        p >> id;
        fRemovedItemIds.emplace_back(id);
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
}

void eWeaponData::read(ePacket& p) {
    p >> fWeaponTypeL;
    p << fWeaponTypeR;
    p >> fMeeleRange;
    p >> fRangedRange;
}

void eWeaponData::write(ePacket& p) const {
    p << fWeaponTypeL;
    p << fWeaponTypeR;
    p << fMeeleRange;
    p << fRangedRange;
}
