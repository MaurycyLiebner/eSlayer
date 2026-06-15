#ifndef EPORTALS_H
#define EPORTALS_H

#include <cstdint>
#include <vector>

class ePacket;

struct ePortal {
    uint32_t fCreator;

    uint8_t fOutdoorMapId;
    uint8_t fOutdoorAreaId;
    uint32_t fOutdoorPortalId;

    uint8_t fCampMapId;
    uint8_t fCampAreaId;
    uint32_t fCampPortalId;

    static void addPortal(const ePortal& p);
    static void removePortal(const uint32_t portalId);

    static std::vector<ePortal> sPortals;
    static uint16_t sPortalsVersion;

    static void read(ePacket& p);
    static void write(ePacket& p);
};

#endif // EPORTALS_H
