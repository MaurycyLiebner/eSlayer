#ifndef EPORTALS_H
#define EPORTALS_H

#include "epoint.h"

#include "eareaids.h"

#include <cstdint>
#include <vector>
#include <optional>

class ePacket;

struct ESLAYERHELPERS_API ePortal {
    uint32_t fCreator;

    eAreaIds fOutdoorArea;
    uint32_t fOutdoorPortalId;
    ePointF fOutdoorPos;

    eAreaIds fCampArea;
    uint32_t fCampPortalId;
    ePointF fCampPos;

    static std::optional<ePortal> portal(const uint32_t portalId);
    static uint32_t creator(const uint32_t portalId);

    static void addPortal(const ePortal& p);
    static void removePortal(const uint32_t portalId);

    static void removeCreatorActPortal(
        const uint32_t creator,
        const uint8_t actId);

    static uint16_t version() { return sPortalsVersion; }
    static std::vector<ePortal> sPortals;

    static void read(ePacket& p);
    static void write(ePacket& p);
private:
    static uint16_t sPortalsVersion;
};

#endif // EPORTALS_H
