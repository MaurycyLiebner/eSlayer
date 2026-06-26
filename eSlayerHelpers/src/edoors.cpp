#include "eSlayerHelpers/edoors.h"

eDoorsStairsBase::eDoorsStairsBase() {}

eDoorsStairsBase::eDoorsStairsBase(
    const eWallType wallType,
    const int type, const int nTypes,
    const int x0, const int y0) :
    fType(wallType) {
    switch(wallType) {
    case eWallType::topLeft: {
        for(int dy = -type; dy < nTypes - type; dy++) {
            const ePoint tile{x0, y0 + dy};
            fTiles.emplace_back(tile);
        }
    } break;
    case eWallType::topRight: {
        for(int dx = -type; dx < nTypes - type; dx++) {
            const ePoint tile{x0 + dx, y0};
            fTiles.emplace_back(tile);
        }
    } break;
    }
}

ePointF eDoorsStairsBase::pos() const {
    ePointF pos{0.f, 0.f};
    if(fTiles.empty()) return pos;
    for(const auto& t : fTiles) {
        switch(fType) {
        case eWallType::topLeft: {
            pos = pos + eVec2f{float(t.fX), t.fY + 0.5f};
        } break;
        case eWallType::topRight: {
            pos = pos + eVec2f{t.fX + 0.5f, float(t.fY)};
        } break;
        }
    }
    pos /= fTiles.size();
    return pos;
}

void eDoorsStairsBase::read(ePacket& p) {
    p >> fType;

    p.read8(fTiles);
}

void eDoorsStairsBase::write(ePacket& p) const {
    p << fType;

    p.write8(fTiles);
}

eStairs::eStairs() {}

eStairs::eStairs(const eWallType wallType,
                 const int type, const int nTypes,
                 const int x0, const int y0,
                 const uint8_t targetMapId) :
    eDoorsStairsBase(wallType, type, nTypes, x0, y0),
    fTargetMapId(targetMapId) {}

eDoors::eDoors() {}

eDoors::eDoors(const eWallType wallType,
               const int type, const int nTypes,
               const int x0, const int y0,
               const bool open) :
    eDoorsStairsBase(wallType, type, nTypes, x0, y0),
    fOpen(open) {}

eServerStairs::eServerStairs() {}

eServerStairs::eServerStairs(
    const uint8_t mapId, const eStairs& stairs) :
    eStairs(stairs),
    eWithMapId(mapId) {}

void eServerStairs::read(ePacket& p) {
    p >> fMapId;
    eDoorsStairsBase::read(p);
    p >> fTargetMapId;
    p >> fTargetAreaId;
}

void eServerStairs::write(ePacket& p) const {
    p << fMapId;
    eDoorsStairsBase::write(p);
    p << fTargetMapId;
    p << fTargetAreaId;
}

eServerDoors::eServerDoors() {}

eServerDoors::eServerDoors(const uint8_t mapId,
                           const eDoors& doors) :
    eDoors(doors),
    eWithMapId(mapId) {}

void eServerDoors::read(ePacket& p) {
    p >> fMapId;
    eDoorsStairsBase::read(p);
    p >> fOpen;
}

void eServerDoors::write(ePacket& p) const {
    p << fMapId;
    eDoorsStairsBase::write(p);
    p << fOpen;
}
