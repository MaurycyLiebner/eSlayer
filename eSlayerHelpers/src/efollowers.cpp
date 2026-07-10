#include "eSlayerHelpers/efollowers.h"

#include "eSlayerHelpers/epacket.h"

std::map<uint32_t, eUnitSpecialData>
eFollowers::sFollowers;

void eFollowersBase::add(const uint32_t id) {
    emplace(id);
    fState++;
}

void eFollowersBase::remove(const uint32_t id) {
    erase(id);
    fState++;
}

void eFollowersBase::read(ePacket& p) {
    p.read8(*this);
    p >> fState;
}

void eFollowersBase::write(ePacket& p) const {
    p.write8(*this);
    p << fState;
}
