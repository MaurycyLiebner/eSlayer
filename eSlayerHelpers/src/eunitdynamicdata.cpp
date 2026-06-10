#include "eSlayerHelpers/eunitdynamicdata.h"

#include "eSlayerHelpers/epacket.h"

const float angleMax = 360.f;
const float animSpeedMax = 100.f;
const float blockingActionTimeMax = 25.5f;

bool eUnitDynamicData::setPosition(const ePointF& pos) {
    if(fPos == pos) return false;
    fPos = pos;
    setUpdate(eShift::position, true);
    return true;
}

bool eUnitDynamicData::setAnim(const uint8_t anim) {
    if(fAnim == anim) return false;
    fAnim = anim;
    setUpdate(eShift::anim, true);
    return true;
}

bool eUnitDynamicData::setAnimId(const eAnimId& animId) {
    if(fAnimId.fValue == animId.fValue) return false;
    fAnimId = animId;
    setUpdate(eShift::anim, true);
    return true;
}

bool eUnitDynamicData::incAnimId(const int by) {
    if(by == 0) return false;
    fAnimId.increment(by);
    setUpdate(eShift::anim, true);
    return true;
}

bool eUnitDynamicData::setAnimSpeed(const float animSpeed) {
    if(fAnimSpeed == animSpeed) return false;
    fAnimSpeed = animSpeed;
    setUpdate(eShift::anim, true);
    return true;
}

bool eUnitDynamicData::setBlockingActionTime(const float time) {
    if(fBlockingActionTime == time) return false;
    fBlockingActionTime = time;
    setUpdate(eShift::blockingActionTime, true);
    return true;
}

bool eUnitDynamicData::setAngle(const float angle) {
    if(fAngle == angle) return false;
    fAngle = angle;
    setUpdate(eShift::angle, true);
    return true;
}

bool eUnitDynamicData::setHealth(const uint16_t health) {
    if(fHealth == health) return false;
    fHealth = health;
    setUpdate(eShift::health, true);
    return true;
}

bool eUnitDynamicData::setMaxHealth(const uint16_t maxHealth) {
    if(fMaxHealth == maxHealth) return false;
    fMaxHealth = maxHealth;
    setUpdate(eShift::maxHealth, true);
    return true;
}

bool eUnitDynamicData::setState(const uint8_t state) {
    if(fState == state) return false;
    fState = state;
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitDynamicData::setBoosts(const std::set<uint8_t>& boosts) {
    if(fBoosts == boosts) return false;
    fBoosts = boosts;
    setUpdate(eShift::boosts, true);
    return true;
}

bool eUnitDynamicData::setCold(const bool c) {
    if(getState(eState::cold_) == c) return false;
    setState(eState::cold_, c);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitDynamicData::setFrozen(const bool f) {
    if(getState(eState::frozen_) == f) return false;
    setState(eState::frozen_, f);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitDynamicData::setPoisoned(const bool p) {
    if(getState(eState::poisoned_) == p) return false;
    setState(eState::poisoned_, p);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitDynamicData::getUpdate(const eShift shift) const {
    return getUpdate(fUpdate, shift);
}

void eUnitDynamicData::setUpdate(const eShift shift, const bool value) {
    setUpdate(fUpdate, shift, value);
}

bool eUnitDynamicData::getState(const eState state) const {
    return (fState >> state) & 1;
}

void eUnitDynamicData::setState(const eState state, const bool value) {
    if(value) {
        fState |= (1u << state);
    } else {
        fState &= ~(1u << state);
    }
}

bool eUnitDynamicData::getUpdate(const uint8_t update, const eShift shift) {
    return (update >> shift) & 1;
}

void eUnitDynamicData::setUpdate(
    uint8_t& update, const eShift shift,
    const bool value) {
    if(value) {
        update |= (1u << shift);
    } else {
        update &= ~(1u << shift);
    }
}

void eUnitDynamicData::apply(eUnitDynamicData& to) const {
    if(getUpdate(eShift::anim)) {
        to.fAnim = fAnim;
        to.fAnimId = fAnimId;
        to.fAnimSpeed = fAnimSpeed;
    }

    if(getUpdate(eShift::position)) {
        to.fPos = fPos;
    }

    if(getUpdate(eShift::angle)) {
        to.fAngle = fAngle;
    }

    if(getUpdate(eShift::blockingActionTime)) {
        to.fBlockingActionTime = fBlockingActionTime;
    }

    if(getUpdate(eShift::health)) {
        to.fHealth = fHealth;
    }

    if(getUpdate(eShift::maxHealth)) {
        to.fMaxHealth = fMaxHealth;
    }

    if(getUpdate(eShift::state)) {
        to.fState = fState;
    }

    if(getUpdate(eShift::boosts)) {
        to.fBoosts = fBoosts;
    }
}

void eUnitDynamicData::read(ePacket& p) {
    p >> fCharId;

    p >> fUpdate;

    if(getUpdate(eShift::anim)) {
        p >> fAnim;
        p >> fAnimId;
        fAnimSpeed = p.readFloatU16(animSpeedMax);
    }

    if(getUpdate(eShift::position)) {
        p >> fPos;
    }

    if(getUpdate(eShift::angle)) {
        fAngle = p.readFloatU8(angleMax);
    }

    if(getUpdate(eShift::blockingActionTime)) {
        fBlockingActionTime = p.readFloatU8(blockingActionTimeMax);
    }

    if(getUpdate(eShift::health)) {
        p >> fHealth;
    }

    if(getUpdate(eShift::maxHealth)) {
        p >> fMaxHealth;
    }

    if(getUpdate(eShift::state)) {
        p >> fState;
    }

    if(getUpdate(eShift::boosts)) {
        p.read8(fBoosts);
    }
}

void eUnitDynamicData::write(ePacket& p) const {
    p << fCharId;

    p << fUpdate;

    if(getUpdate(eShift::anim)) {
        p << fAnim;
        p << fAnimId;
        p.writeFloatU16(fAnimSpeed, animSpeedMax);
    }

    if(getUpdate(eShift::position)) {
        p << fPos;
    }

    if(getUpdate(eShift::angle)) {
        p.writeFloatU8(fAngle, angleMax);
    }

    if(getUpdate(eShift::blockingActionTime)) {
        p.writeFloatU8(fBlockingActionTime, blockingActionTimeMax);
    }

    if(getUpdate(eShift::health)) {
        p << fHealth;
    }

    if(getUpdate(eShift::maxHealth)) {
        p << fMaxHealth;
    }

    if(getUpdate(eShift::state)) {
        p << fState;
    }

    if(getUpdate(eShift::boosts)) {
        p.write8(fBoosts);
    }
}

bool eUnitDynamicData::cold() const {
    return getState(eState::cold_);
}

bool eUnitDynamicData::frozen() const {
    return getState(eState::frozen_);
}

bool eUnitDynamicData::poisoned() const {
    return getState(eState::poisoned_);
}
