#ifndef EMAPPORTION_H
#define EMAPPORTION_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "erect.h"
#include "estringidmapvector.h"
#include "eobject.h"
#include "etile.h"

#include <vector>
#include <memory>
#include <set>

class ePacket;

struct eMapPortionArea {
    int fX = 0;
    int fY = 0;
    int fWidth = 0;
    int fHeight = 0;
};

struct eMapArea {
    eRect fRect;
    uint8_t fMapId;
    uint8_t fAreaId;
};

struct ESLAYERHELPERS_API eMapPortion {
    static const uint16_t sBaseDim = 32;

    eMapPortionArea fArea;

    std::vector<std::vector<eTile>> fTiles;
    std::vector<std::shared_ptr<eObject>> fObjects;

    void write(ePacket& p) const;
    void read(ePacket& p);
};

enum class eWallType : uint8_t;
enum class eConnectionDir : uint8_t;

struct eMapStairs {
    uint16_t fX;
    uint16_t fY;
    uint8_t fDim;
    eWallType fWallType;
    eConnectionDir fStairsDir;
    uint8_t fMapId;
};

struct ESLAYERHELPERS_API eMapData {
    uint16_t fTotalWidth = 0;
    uint16_t fTotalHeight = 0;

    std::set<uint16_t> fTerrainTypes;
    std::set<uint16_t> fObjectTypes;
    std::set<uint16_t> fUnitTypes;

    uint8_t fLight = 180;
    uint8_t fContrast = 140;

    ePointF fSpawnPos;

    eStringIdMapVector<eMapArea> fAreas;

    std::vector<eMapStairs> fStairs;

    void write(ePacket& p) const;
    void read(ePacket& p);
};

#endif // EMAPPORTION_H
