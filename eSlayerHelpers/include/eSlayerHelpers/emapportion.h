#ifndef EMAPPORTION_H
#define EMAPPORTION_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "erect.h"
#include "estringidmapvector.h"
#include "epositioned.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <set>

class ePacket;

struct eTile {
    uint8_t fTerrainType;
    uint8_t fTileType;

    bool fWallTL;
    bool fWallTR;
};

struct eObject : public ePositioned {
    uint32_t fObjectId;

    uint16_t fObjectType;
    uint8_t fSubtype;

    float fSize;
};

struct eMapPortionArea {
    int fX = 0;
    int fY = 0;
    int fWidth = 0;
    int fHeight = 0;
};

struct eMapArea {
    eRect fRect;
    uint8_t fLightness = 180;
    uint8_t fContrast = 140;
};

struct ESLAYERHELPERS_API eMapPortion {
    static const uint16_t sBaseDim = 32;

    eMapPortionArea fArea;

    std::vector<std::vector<eTile>> fTiles;
    std::vector<std::shared_ptr<eObject>> fObjects;

    void write(ePacket& p) const;
    void read(ePacket& p);
};

struct ESLAYERHELPERS_API eMapData {
    uint16_t fTotalWidth = 0;
    uint16_t fTotalHeight = 0;

    std::set<uint16_t> fTerrainTypes;
    std::set<uint16_t> fObjectTypes;
    std::set<uint16_t> fUnitTypes;

    uint8_t fLight = 180;
    uint8_t fContrast = 140;

    ePoint fSpawnPos;

    eStringIdMapVector<eMapArea> fAreas;

    void write(ePacket& p) const;
    void read(ePacket& p);
};

#endif // EMAPPORTION_H
