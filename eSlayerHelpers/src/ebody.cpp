#include "eSlayerHelpers/ebody.h"

#include "eSlayerHelpers/epacket.h"

std::vector<eBody> eBodies::sBodies;

void eBodyBase::readBase(ePacket& p) {
    p >> fBodyId;
    fEq.bodyRead(p);
}

void eBodyBase::writeBase(ePacket& p) const {
    p << fBodyId;
    fEq.bodyWrite(p);
}

void eBody::read(ePacket& p) {
    eBodyBase::readBase(p);
    p >> fPos;
}

void eBody::write(ePacket& p) const {
    eBodyBase::writeBase(p);
    p << fPos;
}

void eBodies::read(ePacket& p) {
    read(sBodies, p);
}

void eBodies::read(std::vector<eBody>& bodies, ePacket& p) {
    bodies.clear();
    uint8_t nBodies;
    p >> nBodies;
    for(int i = 0; i < nBodies; i++) {
        auto& body = bodies.emplace_back();
        body.read(p);
    }
}

void eBodies::write(ePacket& p) {
    write(sBodies, p);
}

void eBodies::write(const std::vector<eBody>& bodies,
                    ePacket& p) {
    const uint8_t nBodies = std::min(255lu, bodies.size());
    p << nBodies;
    for(int i = 0; i < nBodies; i++) {
        const auto& body = bodies[i];
        body.write(p);
    }
}

void eBodies::clear() {
    sBodies.clear();
}

void eBodies::remove(const uint32_t id) {
    for(int i = 0; i < sBodies.size(); i++) {
        const auto& b = sBodies[i];
        if(b.fBodyId != id) continue;
        sBodies.erase(sBodies.begin() + i);
        break;
    }
}

void eBodies::add(const eBody& body) {
    sBodies.emplace_back(body);
}
