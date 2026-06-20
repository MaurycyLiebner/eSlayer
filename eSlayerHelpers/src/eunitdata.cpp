#include "eSlayerHelpers/eunitdata.h"

#include "eSlayerHelpers/epacket.h"

const float radiusMax = 2.f;
const float angleMax = 360.f;
const float animSpeedMax = 100.f;
const float blockingActionTimeMax = 25.5f;

float eUnitData::sColdSpeed = 0.5f;

bool eUnitData::setPosition(
    const ePointF& pos) {
    if(fPos == pos) return false;
    fPos = pos;
    setUpdate(eShift::position, true);
    return true;
}

bool eUnitData::setAnim(
    const uint8_t anim) {
    if(fAnim == anim) return false;
    fAnim = anim;
    setUpdate(eShift::anim, true);
    return true;
}

bool eUnitData::setAnimId(
    const eAnimId& animId) {
    if(fAnimId.fValue == animId.fValue) return false;
    fAnimId = animId;
    setUpdate(eShift::animId, true);
    return true;
}

bool eUnitData::incAnimId(
    const int by) {
    if(by == 0) return false;
    fAnimId.increment(by);
    setUpdate(eShift::animId, true);
    return true;
}

bool eUnitData::setAnimSpeed(
    const float animSpeed) {
    if(fAnimSpeed == animSpeed) return false;
    fAnimSpeed = animSpeed;
    setUpdate(eShift::animSpeed, true);
    return true;
}

bool eUnitData::setBlockingActionTime(
    const float time) {
    if(fBlockingActionTime == time) return false;
    fBlockingActionTime = time;
    setUpdate(eShift::blockingActionTime, true);
    return true;
}

bool eUnitData::setAngle(
    const float angle) {
    if(fAngle == angle) return false;
    fAngle = angle;
    setUpdate(eShift::angle, true);
    return true;
}

bool eUnitData::setHealth(
    const uint16_t health) {
    if(fHealth == health) return false;
    fHealth = health;
    setUpdate(eShift::health, true);
    return true;
}

bool eUnitData::setMaxHealth(
    const uint16_t maxHealth) {
    if(fMaxHealth == maxHealth) return false;
    fMaxHealth = maxHealth;
    setUpdate(eShift::maxHealth, true);
    return true;
}

bool eUnitData::setState(
    const uint8_t state) {
    if(fState == state) return false;
    fState = state;
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitData::setBoosts(
    const std::set<uint8_t>& boosts) {
    if(fBoosts == boosts) return false;
    fBoosts = boosts;
    setUpdate(eShift::boosts, true);
    return true;
}

bool eUnitData::setTeamId(const eTeamId teamId) {
    if(fTeamId == teamId) return false;
    fTeamId = teamId;
    setUpdate(eShift::teamId, true);
    return true;
}

bool eUnitData::setCold(
    const bool c) {
    if(getState(eState::cold_) == c) return false;
    setState(eState::cold_, c);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitData::setFrozen(
    const bool f) {
    if(getState(eState::frozen_) == f) return false;
    setState(eState::frozen_, f);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitData::setPoisoned(
    const bool p) {
    if(getState(eState::poisoned_) == p) return false;
    setState(eState::poisoned_, p);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitData::setStaminaPotion(
    const bool p) {
    if(getState(eState::staminaPotion_) == p) return false;
    setState(eState::staminaPotion_, p);
    setUpdate(eShift::state, true);
    return true;
}

bool eUnitData::getUpdate(
    const eShift shift) const {
    return getUpdate(fUpdate, shift);
}

void eUnitData::setUpdate(
    const eShift shift, const bool value) {
    setUpdate(fUpdate, shift, value);
}

bool eUnitData::getState(
    const eState state) const {
    return (fState >> state) & 1;
}

void eUnitData::setState(
    const eState state, const bool value) {
    if(value) {
        fState |= (1u << state);
    } else {
        fState &= ~(1u << state);
    }
}

bool eUnitData::getUpdate(
    const uint16_t update, const eShift shift) {
    return (update >> shift) & 1;
}

void eUnitData::setUpdate(
    uint16_t& update, const eShift shift,
    const bool value) {
    if(value) {
        update |= (1u << shift);
    } else {
        update &= ~(1u << shift);
    }
}

void eUnitData::apply(eUnitData& to) const {
    if(getUpdate(eShift::anim)) {
        to.fAnim = fAnim;
    }

    if(getUpdate(eShift::animId)) {
        to.fAnimId = fAnimId;
    }

    if(getUpdate(eShift::animSpeed)) {
        to.fAnimSpeed = fAnimSpeed;
    }

    if(getUpdate(eShift::blockingActionTime)) {
        to.fBlockingActionTime = fBlockingActionTime;
    }

    if(getUpdate(eShift::position)) {
        to.fPos = fPos;
    }

    if(getUpdate(eShift::angle)) {
        to.fAngle = fAngle;
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

    if(getUpdate(eShift::teamId)) {
        to.fTeamId = fTeamId;
    }

    if(getUpdate(eShift::unitInfoId)) {
        to.fUnitInfoId = fUnitInfoId;
    }

    if(getUpdate(eShift::radius)) {
        to.fRadius = fRadius;
    }

    if(getUpdate(eShift::mods)) {
        to.fMods = fMods;
    }

    if(getUpdate(eShift::modelParts)) {
        to.fModelParts = fModelParts;
    }
}

bool eUnitData::cold() const {
    return getState(eState::cold_);
}

bool eUnitData::frozen() const {
    return getState(eState::frozen_);
}

bool eUnitData::poisoned() const {
    return getState(eState::poisoned_);
}

bool eUnitData::staminaPotion() const {
    return getState(eState::staminaPotion_);
}

void eUnitData::read(ePacket& p) {
    p >> fCharId;

    p >> fUpdate;

    if(getUpdate(eShift::anim)) {
        p >> fAnim;
    }

    if(getUpdate(eShift::animId)) {
        p >> fAnimId;
    }

    if(getUpdate(eShift::animSpeed)) {
        fAnimSpeed = p.readFloatU16(animSpeedMax);
    }

    if(getUpdate(eShift::blockingActionTime)) {
        fBlockingActionTime = p.readFloatU8(blockingActionTimeMax);
    }

    if(getUpdate(eShift::position)) {
        p >> fPos;
    }

    if(getUpdate(eShift::angle)) {
        fAngle = p.readFloatU8(angleMax);
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

    if(getUpdate(eShift::teamId)) {
        p >> fTeamId;
    }

    if(getUpdate(eShift::unitInfoId)) {
        p >> fUnitInfoId;
    }

    if(getUpdate(eShift::radius)) {
        fRadius = p.readFloatU8(radiusMax);
    }

    if(getUpdate(eShift::mods)) {
        p.read8(fMods);
    }

    if(getUpdate(eShift::modelParts)) {
        fModelParts.read(p);
    }
}

void eUnitData::write(ePacket& p) const {
    p << fCharId;

    p << fUpdate;

    if(getUpdate(eShift::anim)) {
        p << fAnim;
    }

    if(getUpdate(eShift::animId)) {
        p << fAnimId;
    }

    if(getUpdate(eShift::animSpeed)) {
        p.writeFloatU16(fAnimSpeed, animSpeedMax);
    }

    if(getUpdate(eShift::blockingActionTime)) {
        p.writeFloatU8(fBlockingActionTime, blockingActionTimeMax);
    }

    if(getUpdate(eShift::position)) {
        p << fPos;
    }

    if(getUpdate(eShift::angle)) {
        p.writeFloatU8(fAngle, angleMax);
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

    if(getUpdate(eShift::teamId)) {
        p << fTeamId;
    }

    if(getUpdate(eShift::unitInfoId)) {
        p << fUnitInfoId;
    }

    if(getUpdate(eShift::radius)) {
        p.writeFloatU8(fRadius, radiusMax);
    }

    if(getUpdate(eShift::mods)) {
        p.write8(fMods);
    }

    if(getUpdate(eShift::modelParts)) {
        fModelParts.write(p);
    }
}

eUnitData eUnitData::toUnitData(const uint16_t update) const {
    eUnitData result = *this;
    result.fUpdate = update;
    return result;
}
