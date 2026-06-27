#include "eSlayerHelpers/eportals.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/emapsettings.h"

std::vector<ePortal> ePortal::sPortals;
uint16_t ePortal::sPortalsVersion = 0;

std::optional<ePortal> ePortal::portal(const uint32_t portalId) {
    for(const auto& p : sPortals) {
        if(p.fCampPortalId == portalId ||
           p.fOutdoorPortalId == portalId) {
            return p;
        }
    }
    return std::nullopt;
}

uint32_t ePortal::creator(const uint32_t portalId) {
    for(const auto& p : sPortals) {
        if(p.fCampPortalId == portalId ||
           p.fOutdoorPortalId == portalId) {
            return p.fCreator;
        }
    }
    return 0;
}

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
            sPortalsVersion++;
            break;
        }
    }
}

void ePortal::removeCreatorActPortal(
    const uint32_t creator, const uint8_t actId) {
    for(int i = 0; i < sPortals.size(); i++) {
        const auto& p = sPortals[i];
        const auto& area = p.fOutdoorArea;
        const auto mapId = area.fMapId;
        const auto& info = eMapsSettings::sMaps.get(mapId);
        const auto pactId = info.fActId;
        if(pactId == actId) {
            sPortals.erase(sPortals.begin() + i);
            sPortalsVersion++;
            break;
        }
    }
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
