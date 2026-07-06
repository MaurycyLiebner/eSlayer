#include "eSlayerHelpers/emapportion.h"

#include <eSlayerHelpers/epacket.h>

void eMapPortion::write(ePacket& p) const {
    p << fArea;

    for(const auto& row : fTiles) {
        for(const auto& tile : row) {
            tile.write(p);
        }
    }

    const uint16_t nObjs = fObjects.size();
    p << nObjs;
    for(const auto& o : fObjects) {
        p << *o;
    }
}

void eMapPortion::read(ePacket& p) {
    p >> fArea;

    fTiles.resize(fArea.fHeight);
    for(auto& row : fTiles) {
        row.resize(fArea.fWidth);
        for(auto& tile : row) {
            tile.read(p);
        }
    }

    uint16_t nObjs;
    p >> nObjs;
    for(uint16_t i = 0; i < nObjs; i++) {
        auto& o = fObjects.emplace_back(std::make_shared<eObject>());
        p >> *o;
    }
}

void eMapData::write(ePacket& p) const {
    p << fId;

    p << fTotalWidth;
    p << fTotalHeight;

    p.write16(fTerrainTypes);
    p.write16(fObjectTypes);
    p.write16(fUnitTypes);

    p << fLight;
    p << fContrast;

    p << fSpawnPos;

    const uint8_t nAreas = fAreas.size();
    p << nAreas;
    for(const auto& it : fAreas) {
        p << it.fName;
        p << it.fValue;
    }

    p.write8(fStairs);
}

void eMapData::read(ePacket& p) {
    p >> fId;

    p >> fTotalWidth;
    p >> fTotalHeight;

    p.read16(fTerrainTypes);
    p.read16(fObjectTypes);
    p.read16(fUnitTypes);

    p >> fLight;
    p >> fContrast;

    p >> fSpawnPos;

    uint8_t nAreas;
    p >> nAreas;
    for(int i = 0; i < nAreas; i++) {
        std::string name;
        p >> name;
        eMapArea area;
        p >> area;
        fAreas.add(name, area);
    }

    p.read8(fStairs);
}
