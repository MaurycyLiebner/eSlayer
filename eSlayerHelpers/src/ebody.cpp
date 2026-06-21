#include "eSlayerHelpers/ebody.h"

#include "eSlayerHelpers/epacket.h"

std::vector<eBody> eBodies::sBodies;

void eBody::read(ePacket& p) {
    p >> fPos;
    p >> fMapId;
    p >> fBodyId;
    fEq.bodyRead(p);
}

void eBody::write(ePacket& p) const {
    p << fPos;
    p << fMapId;
    p << fBodyId;
    fEq.bodyWrite(p);
}

void eBody::readIds(ePacket& p) {
    p >> fPos;
    p >> fMapId;
    p >> fBodyId;
    fEq.readBodyIds(p);
}

void eBody::writeIds(ePacket& p) const {
    p << fPos;
    p << fMapId;
    p << fBodyId;
    fEq.writeBodyIds(p);
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

void eBodies::readIds(std::vector<eBodyEquipment>& eqs,
                      std::vector<eBody>& bodies,
                      ePacket& p) {
    bodies.clear();
    uint8_t nBodies;
    p >> nBodies;
    for(int i = 0; i < nBodies; i++) {
        auto& body = bodies.emplace_back();
        auto& eq = eqs[i];
        body.fEq = eq;
        body.readIds(p);
        eq = body.fEq;
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

void eBodies::writeIds(const std::vector<eBody>& bodies,
                       ePacket& p) {
    const uint8_t nBodies = std::min(255lu, bodies.size());
    p << nBodies;
    for(int i = 0; i < nBodies; i++) {
        const auto& body = bodies[i];
        body.writeIds(p);
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

eBody* eBodies::get(const uint32_t id) {
    for(auto& b : sBodies) {
        if(b.fBodyId != id) continue;
        return &b;
    }
    return nullptr;
}

void eBodyItemsTaken::read(ePacket& p) {
    p >> fBodyId;
    p.read16(fItems);
}

void eBodyItemsTaken::write(ePacket& p) const {
    p << fBodyId;
    p.write16(fItems);
}
