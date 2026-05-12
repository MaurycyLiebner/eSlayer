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
