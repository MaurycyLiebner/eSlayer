#ifndef EBODY_H
#define EBODY_H

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/epositioned.h"
#include "eSlayerHelpers/eequipmentaction.h"

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

struct ESLAYERHELPERS_API eBodyItemsTaken {
    uint32_t fBodyId;
    std::vector<eBodyEqAction> fItems;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eBodies {
    static std::vector<eBody> sBodies;
    static void clear();
    static void remove(const uint32_t id);
    static void add(const eBody& body);
    static eBody* get(const uint32_t id);
};

#endif // EBODY_H
