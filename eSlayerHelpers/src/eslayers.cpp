#include "eSlayerHelpers/eslayers.h"

#include "eSlayerHelpers/epacket.h"

uint32_t eSlayers::sThisSlayer = 0;
std::map<uint32_t, eSlayer> eSlayers::sSlayers;

void eSlayer::write(ePacket& p) const {
    p << fClientId;
    p << fName;
}

void eSlayer::read(ePacket& p) {
    p >> fClientId;
    p >> fName;
}

std::string eSlayers::name(const uint32_t clientId) {
    const auto it = sSlayers.find(clientId);
    if(it == sSlayers.end()) return "";
    const auto& s = it->second;
    return s.fName;
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
