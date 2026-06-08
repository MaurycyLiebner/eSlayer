#ifndef EUNITDYNAMICDATA_H
#define EUNITDYNAMICDATA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

#include <eSlayerHelpers/eanimid.h>

#include <set>

class ePacket;

struct ESLAYERHELPERS_API eUnitDynamicData :
    public ePositioned {
    uint32_t fCharId;

    uint8_t fUpdate = std::numeric_limits<decltype(fUpdate)>::max();

    uint8_t fAnim;
    eAnimId fAnimId;
    float fAnimSpeed;
    float fBlockingActionTime;

    float fAngle;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    uint8_t fState;

    std::set<uint8_t> fBoosts;

    enum eState : uint8_t {
        cold_,
        frozen_,
        poisoned_
    };

    enum eShift : uint8_t {
        anim,
        position,
        angle,
        blockingActionTime,
        health,
        maxHealth,
        state,
        boosts
    };

    bool setPosition(const ePointF& pos);

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

    bool setCold(const bool c);
    bool setFrozen(const bool f);
    bool setPoisoned(const bool p);

    bool cold() const;
    bool frozen() const;
    bool poisoned() const;

    bool getUpdate(const eShift shift) const;
    void setUpdate(const eShift shift, const bool value);

    bool getState(const eState state) const;
    void setState(const eState state, const bool value);

    static bool getUpdate(const uint8_t update, const eShift shift);
    static void setUpdate(uint8_t& update, const eShift shift, const bool value);

    void apply(eUnitDynamicData& to) const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDYNAMICDATA_H
