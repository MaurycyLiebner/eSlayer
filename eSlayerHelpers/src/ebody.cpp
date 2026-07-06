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
