#include "eSlayerHelpers/emodscollection.h"

#include "eSlayerHelpers/eskills.h"
#include "eSlayerHelpers/epacket.h"

void eModsCollection::addBoost(const eModsCollection& other) {
    fCount += other.fCount;
    fCooldown += other.fCooldown;
    fManaCost += other.fManaCost;
    setRadiusU(fRadiusU + other.fRadiusU);

    for(const auto& it : other) {
        const auto& mod = it.second;
        add(mod);
    }
}

void eModsCollection::add(const eModifier& mod) {
    switch(mod.fType) {
    case eModifierType::damagePoison: {
        auto it = find(mod.fType);
        if(it == end()) {
            it = emplace(mod.fType, eModifier{mod.fType});
        }

        auto& totalMod = it->second;

        const float damage1 = totalMod.fValue1;
        const float damage2 = mod.fValue1;

        const float time1 = totalMod.fValue2;
        const float time2 = mod.fValue2;

        const float rate1 = time1 == 0.f ? 0.f : damage1 / time1;
        const float rate2 = time2 == 0.f ? 0.f : damage2 / time2;

        const float combinedDamage = damage1 + damage2;
        const float combinedRate = rate1 + rate2;

        const float combinedTime = combinedRate == 0.f ?
            0.f : combinedDamage / combinedRate;

        totalMod.fValue1 = combinedDamage;
        totalMod.fValue2 = combinedTime;
    } break;
    case eModifierType::onAttack:
    case eModifierType::onStriking:
    case eModifierType::onKill:
    case eModifierType::onStruck:
    case eModifierType::onDeath:
    case eModifierType::aura: {
        emplace(mod.fType, mod);
    } break;
    default: {
        auto it = find(mod.fType);
        if(it == end()) {
            it = emplace(mod.fType, eModifier{mod.fType});
        }

        auto& totalMod = it->second;

        totalMod.fValue1 += mod.fValue1;
        totalMod.fValue2 += mod.fValue2;
    } break;
    }
}

void eModsCollection::applyMod(
    eModifier& mod, const eModifierType multType) const {
    const auto it = find(multType);
    if(it == end()) return;
    const auto& multMod = it->second;
    switch(multType) {
    case eModifierType::defensePercent:
    case eModifierType::attackRatingPercent:
    case eModifierType::lifePercent:
    case eModifierType::manaPercent:
    case eModifierType::poisonSkillDamage: {
        mod.fValue1 *= 1.f + 0.01f*multMod.fValue1;
    } break;
    case eModifierType::damagePercent:
    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage: {
        mod.fValue1 *= 1.f + 0.01f*multMod.fValue1;
        mod.fValue2 *= 1.f + 0.01f*multMod.fValue2;
    } break;
    case eModifierType::skillLevel: {
        mod.fValue2 = std::clamp(mod.fValue2 + multMod.fValue1,
                                 0, eSkills::sMaxSkillLevel - 1);
    } break;
    default:
        break;
    }
};

void eModsCollection::collapse() {
    std::vector<eModifierType> toErase;
    for(auto& it : *this) {
        auto& mod = it.second;
        switch(mod.fType) {
        case eModifierType::defenseValue:
            applyMod(mod, eModifierType::defensePercent);
            toErase.emplace_back(eModifierType::defensePercent);
            break;
        case eModifierType::attackRatingValue:
            applyMod(mod, eModifierType::attackRatingPercent);
            toErase.emplace_back(eModifierType::attackRatingPercent);
            break;
        case eModifierType::lifeValue:
            applyMod(mod, eModifierType::lifePercent);
            toErase.emplace_back(eModifierType::lifePercent);
            break;
        case eModifierType::manaValue:
            applyMod(mod, eModifierType::manaPercent);
            toErase.emplace_back(eModifierType::manaPercent);
            break;
        case eModifierType::damageFire:
            applyMod(mod, eModifierType::fireSkillDamage);
            toErase.emplace_back(eModifierType::fireSkillDamage);
            break;
        case eModifierType::damageCold:
            applyMod(mod, eModifierType::coldSkillDamage);
            toErase.emplace_back(eModifierType::coldSkillDamage);
            break;
        case eModifierType::damageLightning:
            applyMod(mod, eModifierType::lightningSkillDamage);
            toErase.emplace_back(eModifierType::lightningSkillDamage);
            break;
        case eModifierType::damagePoison:
            applyMod(mod, eModifierType::poisonSkillDamage);
            toErase.emplace_back(eModifierType::poisonSkillDamage);
            break;
        case eModifierType::damageValue:
            applyMod(mod, eModifierType::damagePercent);
            toErase.emplace_back(eModifierType::damagePercent);
            break;
        case eModifierType::onAttack:
        case eModifierType::onStriking:
        case eModifierType::onKill:
        case eModifierType::onStruck:
        case eModifierType::onDeath:

        case eModifierType::aura:
            applyMod(mod, eModifierType::skillLevel);
            toErase.emplace_back(eModifierType::skillLevel);
            break;
        default:
            break;
        }
    }

    for(const auto type : toErase) {
        erase(type);
    }
}

void eModsCollection::collapseSkillLevel() {
    std::vector<eModifierType> toErase;
    for(auto& it : *this) {
        auto& mod = it.second;
        switch(mod.fType) {
        case eModifierType::onAttack:
        case eModifierType::onStriking:
        case eModifierType::onKill:
        case eModifierType::onStruck:
        case eModifierType::onDeath:

        case eModifierType::aura:
            applyMod(mod, eModifierType::skillLevel);
            toErase.emplace_back(eModifierType::skillLevel);
            break;
        default:
            break;
        }
    }

    for(const auto type : toErase) {
        erase(type);
    }
}

void eModsCollection::setRadiusU(const uint8_t r) {
    fRadiusU = r;
    fRadius = ePacket::toFloatU8(r, eSkill::sRadiusMax);
}
