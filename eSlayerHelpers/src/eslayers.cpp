#include "eSlayerHelpers/eslayers.h"

#include "eSlayerHelpers/epacket.h"

uint32_t eSlayers::sThisSlayer = 0;
std::map<uint32_t, eSlayer> eSlayers::sSlayers;

void eSlayer::write(ePacket& p) const {
    p << fClientId;
    p << fMapId;
    p << fPos;

    p << fName;
}

void eSlayer::read(ePacket& p) {
    p >> fClientId;
    p >> fMapId;
    p >> fPos;

    p >> fName;
}

std::string eSlayers::name(const uint32_t clientId) {
    const auto it = sSlayers.find(clientId);
    if(it == sSlayers.end()) return "";
    const auto& s = it->second;
    return s.fName;
}

bool eSlayers::setLocation(
    const uint32_t clientId,
    const uint8_t mapId,
    const ePointF& pos) {
    const auto it = sSlayers.find(clientId);
    if(it == sSlayers.end()) return false;
    auto& s = it->second;
    s.fMapId = mapId;
    s.fPos = pos;
    return true;
}

bool eSlayers::setPoisition(
    const uint32_t clientId,
    const ePointF& pos) {
    const auto it = sSlayers.find(clientId);
    if(it == sSlayers.end()) return false;
    auto& s = it->second;
    s.fPos = pos;
    return true;
}

void eSlayers::writeLocations(ePacket& p) {
    const uint8_t nClients = sSlayers.size();
    p << nClients;
    for(const auto& it : sSlayers) {
        const auto& slayer = it.second;
        p << static_cast<const eSlayerLocation&>(slayer);
    }
}

void eSlayers::readLocations(ePacket& p) {
    uint8_t nClients;
    p >> nClients;
    for(int i = 0; i < nClients; i++) {
        eSlayerLocation loc;
        p >> loc;
        setLocation(loc.fClientId, loc.fMapId, loc.fPos);
    }
}

void eSlayers::write(ePacket& p) {
    const uint8_t nClients = sSlayers.size();
    p << nClients;
    for(const auto& it : sSlayers) {
        const auto& slayer = it.second;
        slayer.write(p);
    }
}

void eSlayers::read(ePacket& p) {
    sSlayers.clear();
    uint8_t nClients;
    p >> nClients;
    for(uint8_t i = 0; i < nClients; i++) {
        eSlayer slayer;
        slayer.read(p);
        sSlayers[slayer.fClientId] = slayer;
    }
}

void eSlayers::clear() {
    sSlayers.clear();
}
