#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"
#include "echardata.h"
#include "eunitdynamicdata.h"
#include "eteamid.h"

class ePacket;

const uint8_t sExplosionAnim = 255;

enum class eUnitMod : uint8_t {
    minion,

    auraEnchanted,
    coldEnchanted,
    cursed,
    extraFast,
    extraStrong,
    fireEnchanted,
    lightningEnchanted,
    magicResistant,
    manaBurn,
    multiShot,
    spectralHit,
    stoneSkin,
    teleporting
};

struct ESLAYERHELPERS_API eUnitData : public ePositioned {
    uint32_t fCharId;
    eTeamId fTeamId;

    uint8_t fCharDataId;

    float fRadius;

    float fAngle;

    uint8_t fAnim;
    uint32_t fAnimId;
    float fAnimSpeed;

    float fBlockingActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    std::vector<eUnitMod> fMods;

    eModelParts fModelParts;

    uint8_t fState;

    eUnitData toUnitData() const;
    eUnitDynamicData toDynamicData() const;

    void read(ePacket& p);
    void write(ePacket& p) const;

    bool cold() const;
    void setCold(const bool c);

    bool frozen() const;
    void setFrozen(const bool f);

    bool poisoned() const;
    void setPoisoned(const bool p);

    static float sColdSpeed;
};

#endif // EUNITDATA_H
