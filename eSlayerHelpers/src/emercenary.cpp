#include "eSlayerHelpers/emercenary.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/emercenaries.h"

void eMercenary::read(ePacket& p) {
    p >> fUpdate;

    if(getUpdate(eShift::mercType)) {
        p >> fMercType;
    }
    if(getUpdate(eShift::name)) {
        p >> fNameId;
    }
    if(getUpdate(eShift::level)) {
        p >> fLevel;
    }
    if(getUpdate(eShift::unitId)) {
        p >> fUnitId;
    }
    if(getUpdate(eShift::exp)) {
        p >> fExp;
    }
    if(getUpdate(eShift::eq)) {
        fEq.read(p);
    }
    if(getUpdate(eShift::dead)) {
        p >> fDead;
    }
}

void eMercenary::readOver(ePacket& p) {
    auto updateTmp = fUpdate;

    read(p);

    fUpdate = updateTmp | fUpdate;
}

void eMercenary::write(ePacket& p) const {
    p << fUpdate;

    if(getUpdate(eShift::mercType)) {
        p << fMercType;
    }
    if(getUpdate(eShift::name)) {
        p << fNameId;
    }
    if(getUpdate(eShift::level)) {
        p << fLevel;
    }
    if(getUpdate(eShift::unitId)) {
        p << fUnitId;
    }
    if(getUpdate(eShift::exp)) {
        p << fExp;
    }
    if(getUpdate(eShift::eq)) {
        fEq.write(p);
    }
    if(getUpdate(eShift::dead)) {
        p << fDead;
    }
}

bool eMercenary::setDead(const bool d) {
    if(d == fDead) return false;
    fDead = d;
    setUpdate(eShift::dead, true);
    return true;
}

bool eMercenary::setUnitId(const uint32_t id) {
    if(id == fUnitId) return false;
    fUnitId = id;
    setUpdate(eShift::unitId, true);
    return true;
}

bool eMercenary::setExp(const float exp) {
    if(exp == fExp) return false;
    fExp = exp;
    setUpdate(eShift::exp, true);
    return true;
}

bool eMercenary::setLevel(const uint8_t level) {
    if(level == fLevel) return false;
    fLevel = level;
    setUpdate(eShift::level, true);
    return true;
}

eAttributes eMercenary::attributes() const {
    auto result = eMercenaryBase::attributes();
    result.fExp = fExp;
    return result;
}

eAttributes eMercenaryBase::attributes() const {
    eAttributes result;
    const auto& m = eMercenariesInfo::sMercs.get(fMercType);
    const auto levelMult = fLevel - 1;
    result.fStrength = m.fBaseStrength + levelMult*m.fStrengthPerLevel;
    result.fDexterity = m.fBaseDexterity + levelMult*m.fDexterityPerLevel;
    result.fVitality = m.fBaseVitality + levelMult*m.fVitalityPerLevel;
    result.fEnergy = m.fBaseEnergy + levelMult*m.fEnergyPerLevel;
    result.fLevel = fLevel;
    return result;
}

std::vector<eModifier> eMercenaryBase::mods() const {
    return eMercenariesInfo::mods(fMercType, fLevel);
}

uint32_t eMercenaryBase::cost() const {
    return fLevel*1000;
}

bool eMercenary::getUpdate(
    const eShift shift) const {
    return getUpdate(fUpdate, shift);
}

void eMercenary::setUpdate(
    const eShift shift, const bool value) {
    setUpdate(fUpdate, shift, value);
}

bool eMercenary::getUpdate(
    const uint8_t update, const eShift shift) {
    return (update >> shift) & 1;
}

void eMercenary::setUpdate(
    uint8_t& update, const eShift shift,
    const bool value) {
    if(value) {
        update |= (1u << shift);
    } else {
        update &= ~(1u << shift);
    }
}

void eMercenary::apply(eMercenary& to) const {
    if(getUpdate(eShift::mercType)) {
        to.fMercType = fMercType;
    }

    if(getUpdate(eShift::name)) {
        to.fNameId = fNameId;
    }

    if(getUpdate(eShift::level)) {
        to.fLevel = fLevel;
    }

    if(getUpdate(eShift::unitId)) {
        to.fUnitId = fUnitId;
    }

    if(getUpdate(eShift::exp)) {
        to.fExp = fExp;
    }

    if(getUpdate(eShift::eq)) {
        to.fEq = fEq;
    }

    if(getUpdate(eShift::dead)) {
        to.fDead = fDead;
    }
}