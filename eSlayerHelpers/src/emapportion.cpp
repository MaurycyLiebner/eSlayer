#include "eSlayerHelpers/emapportion.h"

#include <eSlayerHelpers/epacket.h>

void eMapPortion::write(ePacket& p) const {
    p << fArea;

    for(const auto& row : fTiles) {
        for(const auto& tile : row) {
            p << tile;
        }
    }

    const uint16_t nObjs = fObjects.size();
    p << nObjs;
    for(const auto& o : fObjects) {
        p << o;
    }
}

void eMapPortion::read(ePacket& p) {
    p >> fArea;

    fTiles.resize(fArea.fHeight);
    for(auto& row : fTiles) {
        row.resize(fArea.fWidth);
        for(auto& tile : row) {
            p >> tile;
        }
    }

    uint16_t nObjs;
    p >> nObjs;
    for(uint16_t i = 0; i < nObjs; i++) {
        auto& o = fObjects.emplace_back();
        p >> o;
    }
}

void eMapData::write(ePacket& p) const {
    p << fTotalWidth;
    p << fTotalHeight;

    const uint16_t nTerrTypes = fTerrainTypes.size();
    p << nTerrTypes;
    for(const auto type : fTerrainTypes) {
        p << type;
    }

    const uint16_t nObjTypes = fObjectTypes.size();
    p << nObjTypes;
    for(const auto type : fObjectTypes) {
        p << type;
    }

    const uint16_t nUnitTypes = fUnitTypes.size();
    p << nUnitTypes;
    for(const auto& unitType : fUnitTypes) {
        p << unitType;
    }
}

void eMapData::read(ePacket& p) {
    p >> fTotalWidth;
    p >> fTotalHeight;

    uint16_t nTerrTypes;
    p >> nTerrTypes;
    for(uint16_t i = 0; i < nTerrTypes; i++) {
        uint16_t t;
        p >> t;
        fTerrainTypes.emplace(t);
    }

    uint16_t nObjTypes;
    p >> nObjTypes;
    for(uint16_t i = 0; i < nObjTypes; i++) {
        uint16_t t;
        p >> t;
        fObjectTypes.emplace(t);
    }

    uint16_t nUnitTypes;
    p >> nUnitTypes;
    for(uint16_t i = 0; i < nUnitTypes; i++) {
        uint16_t unitType;
        p >> unitType;
        fUnitTypes.emplace(unitType);
    }
}
