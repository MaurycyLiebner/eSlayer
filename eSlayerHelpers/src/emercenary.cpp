#include "eSlayerHelpers/emercenary.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/emercenaries.h"

void eMercenary::read(ePacket& p) {
    p >> fUnitId;
    p >> fMercType;
    p >> fNameId;
    p >> fLevel;
    p >> fExp;
    fEq.read(p);
    p >> fDead;
}

void eMercenary::write(ePacket& p) const {
    p << fUnitId;
    p << fMercType;
    p << fNameId;
    p << fLevel;
    p << fExp;
    fEq.write(p);
    p << fDead;
}

eAttributes eMercenaryBase::attributes() const {
    eAttributes result;
    const auto& m = eMercenariesInfo::sMercs.get(fMercType);
    result.fStrength = m.fBaseStrength + fLevel*m.fStrengthPerLevel;
    result.fDexterity = m.fBaseDexterity + fLevel*m.fDexterityPerLevel;
    result.fVitality = m.fBaseVitality + fLevel*m.fVitalityPerLevel;
    result.fEnergy = m.fBaseEnergy + fLevel*m.fEnergyPerLevel;
    result.fLevel = fLevel;
    return result;
}

std::vector<eModifier> eMercenaryBase::mods() const {
    std::vector<eModifier> result;
    const auto& m = eMercenariesInfo::sMercs.get(fMercType);
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = fLevel*m.fLifePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::lifeValue;
        mod.fValue1 = fLevel*m.fLifePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::fireResistance;
        mod.fValue1 = fLevel*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::coldResistance;
        mod.fValue1 = fLevel*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::lightningResistance;
        mod.fValue1 = fLevel*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::poisonResistance;
        mod.fValue1 = fLevel*m.fResistancePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageValue;
        mod.fValue1 = fLevel*m.fDamagePerLevel;
        mod.fValue2 = fLevel*m.fDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageFire;
        mod.fValue1 = fLevel*m.fFireDamagePerLevel;
        mod.fValue2 = fLevel*m.fFireDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageCold;
        mod.fValue1 = fLevel*m.fColdDamagePerLevel;
        mod.fValue2 = fLevel*m.fColdDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::damageLightning;
        mod.fValue1 = fLevel*m.fLightningDamagePerLevel;
        mod.fValue2 = fLevel*m.fLightningDamagePerLevel;
    }
    {
        auto& mod = result.emplace_back();
        mod.fType = eModifierType::defenseValue;
        mod.fValue1 = fLevel*m.fDefensePerLevel;
    }
    return result;
}

uint32_t eMercenaryBase::cost() const {
    return fLevel*1000;
}
