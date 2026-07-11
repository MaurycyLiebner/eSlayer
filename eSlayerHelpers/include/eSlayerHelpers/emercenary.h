#ifndef EMERCENARY_H
#define EMERCENARY_H

#include <cstdint>

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eattributes.h"

struct ESLAYERHELPERS_API eMercenaryBase {
    uint8_t fMercType = 0;
    uint8_t fNameId = 0;
    uint8_t fLevel = 1;

    eAttributes attributes() const;
    std::vector<eModifier> mods() const;
    uint32_t cost() const;
};

struct ESLAYERHELPERS_API eMercenary :
    public eMercenaryBase {
    uint32_t fUnitId = 0; // 0 when dead
    float fExp = 0.f;
    eEquipment fEq;
    bool fDead = true;

    uint8_t fUpdate = std::numeric_limits<decltype(fUpdate)>::max();

    enum eShift : uint8_t {
        mercType,
        name,
        level,
        unitId,
        exp,
        eq,
        dead
    };
    bool getUpdate(const eShift shift) const;
    void setUpdate(const eShift shift, const bool value);

    static bool getUpdate(const uint8_t update, const eShift shift);
    static void setUpdate(uint8_t& update, const eShift shift, const bool value);

    void apply(eMercenary& to) const;

    void read(ePacket& p);
    void readOver(ePacket& p);
    void write(ePacket& p) const;

    bool setDead(const bool d);
    bool setExp(const float exp);
    bool setLevel(const uint8_t level);
};

#endif // EMERCENARY_H
