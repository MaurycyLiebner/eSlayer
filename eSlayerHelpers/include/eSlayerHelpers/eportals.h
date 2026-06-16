#ifndef EPORTALS_H
#define EPORTALS_H

#include "epoint.h"

#include <cstdint>
#include <vector>
#include <optional>

class ePacket;

struct ePortal {
    uint32_t fCreator;

    uint8_t fOutdoorMapId;
    uint8_t fOutdoorAreaId;
    uint32_t fOutdoorPortalId;
    ePointF fOutdoorPos;

    uint8_t fCampMapId;
    uint8_t fCampAreaId;
    uint32_t fCampPortalId;
    ePointF fCampPos;

    static std::optional<ePortal> portal(const uint32_t portalId);
    static uint32_t creator(const uint32_t portalId);

    static void addPortal(const ePortal& p);
    static void removePortal(const uint32_t portalId);

    static uint16_t version() { return sPortalsVersion; }
    static std::vector<ePortal> sPortals;

    static void read(ePacket& p);
    static void write(ePacket& p);
private:
    static uint16_t sPortalsVersion;
};

#endif // EPORTALS_H
