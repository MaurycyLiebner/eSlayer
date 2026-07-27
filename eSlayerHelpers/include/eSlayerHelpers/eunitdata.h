#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "echardata.h"
#include "epositioned.h"
#include "eteamid.h"

#include <eSlayerHelpers/eanimid.h>

class ePacket;

struct ESLAYERHELPERS_API eUnitData :
    public ePositioned {
    uint32_t fCharId;

    uint32_t fUpdate = std::numeric_limits<decltype(fUpdate)>::max();

    uint8_t fMapId;
    uint8_t fAreaId;

    uint8_t fAnim;
    eAnimId fAnimId;
    float fAnimSpeed;
    float fBlockingActionTime;

    float fAngle;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    uint8_t fState;

    std::set<uint8_t> fBoosts;

    eTeamId fTeamId;

    uint8_t fUnitInfoId;

    float fRadius;

    std::set<uint8_t> fMods;

    uint8_t fImmunities;

    eModelParts fModelParts;

    eUnitData toUnitData(const uint32_t update = std::numeric_limits<decltype(update)>::max()) const;

    enum eImmunity : uint8_t {
        ifire, icold, ilightning, ipoison, iphysical
    };

    enum eState : uint8_t {
        cold_,
        frozen_,
        poisoned_,
        staminaPotion_
    };

    enum eShift : uint8_t {
        mapId,
        areaId,
        anim,
        animId,
        animSpeed,
        blockingActionTime,
        position,
        angle,
        health,
        maxHealth,
        state,
        boosts,
        teamId,
        unitInfoId,
        radius,
        mods,
        modelParts,
        immunities
    };

    bool setPosition(const ePointF& pos);

    bool setMapId(const uint8_t mapId);
    bool setAreaId(const uint8_t areaId);

    bool setAnim(const uint8_t anim);
    bool setAnimId(const eAnimId& animId);
    bool incAnimId(const int by);
    bool setAnimSpeed(const float animSpeed);
    bool setBlockingActionTime(const float time);

    bool setAngle(const float angle);

    bool setHealth(const uint16_t health);
    bool setMaxHealth(const uint16_t maxHealth);

    bool setState(const uint8_t state);
    bool setBoosts(const std::set<uint8_t>& boosts);

    bool setTeamId(const eTeamId teamId);

    bool setCold(const bool c);
    bool setFrozen(const bool f);
    bool setPoisoned(const bool p);
    bool setStaminaPotion(const bool p);

    bool cold() const;
    bool frozen() const;
    bool poisoned() const;
    bool staminaPotion() const;

    bool getUpdate(const eShift shift) const;
    void setUpdate(const eShift shift, const bool value);

    bool getState(const eState state) const;
    void setState(const eState state, const bool value);

    bool getImmunity(const eImmunity imm) const;
    bool setImmunity(const eImmunity imm, const bool value);

    static bool getUpdate(const uint32_t update, const eShift shift);
    static void setUpdate(uint32_t& update, const eShift shift, const bool value);

    void apply(eUnitData& to) const;

    void read(ePacket& p);
    void write(ePacket& p) const;

    static float sColdSpeed;
};

#endif // EUNITDATA_H
