#include "eSlayerHelpers/edoors.h"

ePointF eDoors::pos() const {
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

void eDoors::read(ePacket& p) {
    p >> fType;
    p >> fOpen;

    uint8_t nTiles;
    p >> nTiles;
    for(int i = 0; i < nTiles; i++) {
        p >> fTiles.emplace_back();
    }
}

void eDoors::write(ePacket& p) const {
    p << fType;
    p << fOpen;

    const uint8_t nTiles = fTiles.size();
    p << nTiles;
    for(const auto& t : fTiles) {
        p << t;
    }
}
