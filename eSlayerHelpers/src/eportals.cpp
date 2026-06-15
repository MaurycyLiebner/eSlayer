#include "eSlayerHelpers/eportals.h"

#include "eSlayerHelpers/epacket.h"

std::vector<ePortal> ePortal::sPortals;
uint16_t ePortal::sPortalsVersion = 0;

void ePortal::addPortal(const ePortal& p) {
    sPortals.emplace_back(p);
    sPortalsVersion++;
}

void ePortal::removePortal(const uint32_t portalId) {
    for(int i = 0; i < sPortals.size(); i++) {
        const auto& p = sPortals[i];
        if(p.fCampPortalId == portalId ||
           p.fOutdoorPortalId == portalId) {
            sPortals.erase(sPortals.begin() + i);
            break;
        }
    }
    sPortalsVersion++;
}

void ePortal::read(ePacket& p) {
    p >> sPortalsVersion;
    sPortals.clear();
    p.read16(sPortals);
}

void ePortal::write(ePacket& p) {
    p << sPortalsVersion;
    p.write16(sPortals);
}
