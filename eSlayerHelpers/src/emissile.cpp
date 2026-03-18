#include "eSlayerHelpers/emissile.h"

#include "eSlayerHelpers/epacket.h"

int obsticlesFromChance(const float p) {
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

void eMissile::read(ePacket& p) {
    p >> fType;
    p >> fFrom;
    p >> fPos;
    p >> fTo;
}

void eMissile::write(ePacket& p) const {
    p << fType;
    p << fFrom;
    p << fPos;
    p << fTo;
}
