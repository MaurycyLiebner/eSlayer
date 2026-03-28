#include "eSlayerHelpers/erequestdata.h"

#include "eSlayerHelpers/epacket.h"

void eRequestData::read(ePacket& p) {
    p >> fRequestId;

    uint16_t nUnits;
    p >> nUnits;
    for(int i = 0; i < nUnits; i++) {
        auto& u = fUnits.emplace_back();
        u.read(p);
    }

    uint16_t nMissiles;
    p >> nMissiles;
    for(int i = 0; i < nMissiles; i++) {
        auto& m = fMissiles.emplace_back();
        m.read(p);
    }

    uint16_t nItems;
    p >> nItems;
    for(int i = 0; i < nItems; i++) {
        auto& it = fItems.emplace_back();
        it.read(p);
    }
}

void eRequestData::write(ePacket& p) const {
    p << fRequestId;

    const uint16_t nUnits = fUnits.size();
    p << nUnits;
    for(const auto& u : fUnits) {
        u.write(p);
    }

    const uint16_t nMissiles = fMissiles.size();
    p << nMissiles;
    for(const auto& m : fMissiles) {
        m.write(p);
    }

    const uint16_t nItems = fItems.size();
    p << nItems;
    for(const auto& i : fItems) {
        i.write(p);
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
