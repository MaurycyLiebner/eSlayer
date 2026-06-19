#ifndef EBODY_H
#define EBODY_H

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/epositioned.h"

struct ESLAYERHELPERS_API eBody :
    public ePositioned {
    uint8_t fMapId;
    uint32_t fBodyId;
    eBodyEquipment fEq;

    void read(ePacket& p);
    void write(ePacket& p) const;

    void readIds(ePacket& p);
    void writeIds(ePacket& p) const;
};

struct ESLAYERHELPERS_API eBodies {
    static std::vector<eBody> sBodies;
    static void read(ePacket& p);
    static void read(std::vector<eBody>& bodies, ePacket& p);
    static void readIds(std::vector<eBodyEquipment>& eqs,
                        std::vector<eBody>& bodies, ePacket& p);
    static void write(ePacket& p);
    static void write(const std::vector<eBody>& bodies, ePacket& p);
    static void writeIds(const std::vector<eBody>& bodies, ePacket& p);
    static void clear();
    static void remove(const uint32_t id);
    static void add(const eBody& body);
};

#endif // EBODY_H
