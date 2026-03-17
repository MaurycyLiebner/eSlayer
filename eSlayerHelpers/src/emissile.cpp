#include "eSlayerHelpers/emissile.h"

#include "eSlayerHelpers/epacket.h"

void eMissile::increment(const float by) {
    const float speed = 0.1f;
    float remDist = by*speed;
    while(remDist > 0.f && !fPath.empty()) {
        const auto& target = fPath.front();
        eVec2f dir = ePointF::vector(target, fPos);
        if(dir.length() >= remDist) {
            dir.normalize(remDist);
            remDist = 0.f;
        } else {
            remDist -= dir.length();
            fPath.erase(fPath.begin());
        }
        fPos = fPos + dir;
    }
}

void eMissile::read(ePacket& p) {
    p >> fType;
    p >> fPos;

    uint16_t nPath;
    p >> nPath;
    for(int i = 0; i < nPath; i++) {
        auto& pos = fPath.emplace_back();
        p >> pos;
    }
}

void eMissile::write(ePacket& p) const {
    p << fType;
    p << fPos;

    const uint16_t nPath = fPath.size();
    p << nPath;
    for(const auto& pos : fPath) {
        p << pos;
    }
}
