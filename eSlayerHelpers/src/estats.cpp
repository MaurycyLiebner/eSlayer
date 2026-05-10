#include "eSlayerHelpers/estats.h"

#include "eSlayerHelpers/eattributes.h"
#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/erand.h"
#include "eSlayerHelpers/erunsettings.h"
#include "eSlayerHelpers/eskills.h"
#include "eSlayerHelpers/evectorhelpers.h"
#include "eSlayerHelpers/eweaponchoice.h"

bool eStats::canUseSkill(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto weapon = wchoice == eWeaponChoice::left ?
        fWeaponTypeL : fWeaponTypeR;
    const auto otherWeapon = wchoice == eWeaponChoice::right ?
        fWeaponTypeL : fWeaponTypeR;
    const auto& skillStats = skill(schoice);
    const int skillId = skillStats.fSkillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto skillType = skill.fType;
    switch(skillType) {
    case eSkillType::attack:
        return weapon != eWeaponType::shield &&
               (weapon != eWeaponType::none ||
                otherWeapon == eWeaponType::none);
    case eSkillType::smite:
        return weapon == eWeaponType::shield;
    case eSkillType::kick:
        return true;
    case eSkillType::shoot:
        return weapon == eWeaponType::ranged;
    case eSkillType::throw_:
        return weapon == eWeaponType::throwable;
    case eSkillType::missile:
        return true;
    case eSkillType::wall:
        return true;
    case eSkillType::nova:
        return true;
    case eSkillType::summon:
        return true;
    case eSkillType::passive:
        return false;
    case eSkillType::aura:
        return false;
    case eSkillType::boostCurse:
        return true;
    }
    return false;
}

void gCalculateWeaponDmg(const eItem& weapon,
                         float& min, float& max) {
    float baseMin = weapon.fMinDmg;
    float baseMax = weapon.fMaxDmg;
    float percentIncMin = 0.f;
    float percentIncMax = 0.f;
    for(const auto& mod : weapon.fModifiers) {
        switch(mod.fType) {
        case eModifierType::damagePercent:
            percentIncMin += 0.01f*mod.fValue1;
            percentIncMax += 0.01f*mod.fValue2;
            break;
        case eModifierType::damageValue:
            baseMin += mod.fValue1;
            baseMax += mod.fValue2;
            break;
        default:
            break;
        }
    }

    min = baseMin*(1.f + percentIncMin);
    max = baseMax*(1.f + percentIncMax);
}

bool gWeaponIsRanged(const eWeaponType subtype) {
    switch(subtype) {
    case eWeaponType::none:
    case eWeaponType::meele:
    case eWeaponType::shield:
        return false;
    case eWeaponType::ranged:
    case eWeaponType::throwable:
        return true;
    }
    return false;
}

bool gWeaponIsMeele(const eWeaponType subtype) {
    switch(subtype) {
    case eWeaponType::none:
    case eWeaponType::meele:
    case eWeaponType::shield:
    case eWeaponType::throwable:
        return true;
    case eWeaponType::ranged:
        return false;
    }
    return false;
}

enum class eModifierSource {
    item, skill, boost
};

struct eSkillStatsHelper {
    eSkillStatsHelper(eSkillStats& stats) :
        fStats(stats) {
        const int skillId = fStats.fSkillId;
        if(skillId == -1) {
            fSkillType = eSkillType::attack;
        } else {
            const auto& skill = eSkills::sSkills.get(skillId);
            fSkillType = skill.fType;
        }
    }

    eSkillStats& fStats;
    eSkillType fSkillType;

    float fBaseAR = 0.f;

    float fFlatARLW = 0.f;
    float fBonusARLW = 0.f;

    float fFlatARRW = 0.f;
    float fBonusARRW = 0.f;

    eDamage fDmgMinLWBase;
    eDamage fDmgMaxLWBase;
    eDamage fDmgMinRWBase;
    eDamage fDmgMaxRWBase;

    eDamage fDmgMultMin{1.f, 1.f, 1.f, 1.f, 1.f};
    eDamage fDmgMultMax{1.f, 1.f, 1.f, 1.f, 1.f};

    float fPoisonBitRateLW = 0.f;
    float fPoisonBitRateRW = 0.f;
    float fPoisonFrameLengthLW = 0.f;
    float fPoisonFrameLengthRW = 0.f;

    bool applyMod(const eModifierType type, const eModifierSource src) const {
        switch(fSkillType) {
        case eSkillType::attack:
        case eSkillType::kick:
        case eSkillType::smite:
        case eSkillType::shoot:
        case eSkillType::throw_:
            return true;
        case eSkillType::missile:
        case eSkillType::wall:
        case eSkillType::nova:
        case eSkillType::summon:
            return src == eModifierSource::skill ||
                   type == eModifierType::fireSkillDamage ||
                   type == eModifierType::coldSkillDamage ||
                   type == eModifierType::lightningSkillDamage ||
                   type == eModifierType::poisonSkillDamage;
        case eSkillType::aura:
        case eSkillType::boostCurse:
        case eSkillType::passive:
            return false;
        }
        return false;
    }

    void addMod(const eModifier& mod, const eModifierSource src,
                const bool lw, const bool rw) {
        const bool r = applyMod(mod.fType, src);
        if(!r) return;
        switch(mod.fType) {
        case eModifierType::damagePercent: {
            fDmgMultMin.fPhysical += 0.01f*mod.fValue1;
            fDmgMultMax.fPhysical += 0.01f*mod.fValue2;
        } break;
        case eModifierType::damageValue: {
            fDmgMinLWBase.fPhysical += mod.fValue1;
            fDmgMinRWBase.fPhysical += mod.fValue1;
            fDmgMaxLWBase.fPhysical += mod.fValue2;
            fDmgMaxRWBase.fPhysical += mod.fValue2;
        } break;
        case eModifierType::damageFire: {
            if(lw) {
                fDmgMinLWBase.fFire += mod.fValue1;
                fDmgMaxLWBase.fFire += mod.fValue2;
            }
            if(rw) {
                fDmgMinRWBase.fFire += mod.fValue1;
                fDmgMaxRWBase.fFire += mod.fValue2;
            }
        } break;
        case eModifierType::damageCold: {
            if(lw) {
                fDmgMinLWBase.fCold += mod.fValue1;
                fDmgMaxLWBase.fCold += mod.fValue2;
            }
            if(rw) {
                fDmgMinRWBase.fCold += mod.fValue1;
                fDmgMaxRWBase.fCold += mod.fValue2;
            }
        } break;
        case eModifierType::damageLightning: {
            if(rw) {
                fDmgMinLWBase.fLightning += mod.fValue1;
                fDmgMaxLWBase.fLightning += mod.fValue2;
            }
            if(rw) {
                fDmgMinRWBase.fLightning += mod.fValue1;
                fDmgMaxRWBase.fLightning += mod.fValue2;
            }
        } break;
        case eModifierType::damagePoison: {
            const float framesLength = mod.fValue2*eRunSettings::sFPS;
            const float bitRate = 256.f*mod.fValue1/framesLength;
            if(lw) {
                fPoisonBitRateLW += bitRate;
                fPoisonFrameLengthLW += framesLength;
            }
            if(rw) {
                fPoisonBitRateRW += bitRate;
                fPoisonFrameLengthRW += framesLength;
            }
        } break;
        case eModifierType::pierceChance: {
            if(lw) {
                fStats.fPierceLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fStats.fPierceRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::attackRatingValue: {
            if(lw) {
                fFlatARLW += mod.fValue1;
            }
            if(rw) {
                fFlatARRW += mod.fValue1;
            }
        } break;
        case eModifierType::attackRatingPercent: {
            if(lw) {
                fBonusARLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fBonusARRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::lifeSteal: {
            if(lw) {
                fStats.fLifeStealLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fStats.fLifeStealRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::manaSteal: {
            if(lw) {
                fStats.fManaStealLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fStats.fManaStealRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::meeleSplashDamage: {
            if(lw) {
                fStats.fMeeleSplashDamageLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fStats.fMeeleSplashDamageRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::knockback: {
            if(lw) {
                fStats.fKnockbackLW = true;
            }
            if(rw) {
                fStats.fKnockbackRW = true;
            }
        } break;
        case eModifierType::attackSpeed: {
            if(src == eModifierSource::skill) {
                fStats.fAttackSpeedS += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::fireSkillDamage: {
            fDmgMultMin.fFire += 0.01f*mod.fValue1;
            fDmgMultMax.fFire += 0.01f*mod.fValue1;
        } break;
        case eModifierType::coldSkillDamage: {
            fDmgMultMin.fCold += 0.01f*mod.fValue1;
            fDmgMultMax.fCold += 0.01f*mod.fValue1;
        } break;
        case eModifierType::lightningSkillDamage: {
            fDmgMultMin.fLightning += 0.01f*mod.fValue1;
            fDmgMultMax.fLightning += 0.01f*mod.fValue1;
        } break;
        case eModifierType::poisonSkillDamage: {
            fDmgMultMin.fPoisonPerFrame += 0.01f*mod.fValue1;
            fDmgMultMax.fPoisonPerFrame += 0.01f*mod.fValue1;
        } break;
        case eModifierType::coldLength: {
            const float framesLength = mod.fValue1*eRunSettings::sFPS;
            if(lw) {
                fStats.fColdLengthLW += framesLength;
            }
            if(rw) {
                fStats.fColdLengthRW += framesLength;
            }
        } break;
        case eModifierType::freezeLength: {
            const float framesLength = mod.fValue1*eRunSettings::sFPS;
            if(lw) {
                fStats.fFreezeLengthLW += framesLength;
            }
            if(rw) {
                fStats.fFreezeLengthRW += framesLength;
            }
        } break;
        default:
            break;
        }
    }

    void apply() {
        fDmgMinLWBase.fPoisonPerFrame = fPoisonBitRateLW/256.f;
        fDmgMinLWBase.fPoisonFrameLength = fPoisonFrameLengthLW;
        fDmgMaxLWBase.fPoisonPerFrame = fPoisonBitRateLW/256.f;
        fDmgMaxLWBase.fPoisonFrameLength = fPoisonFrameLengthLW;

        fDmgMinRWBase.fPoisonPerFrame = fPoisonBitRateRW/256.f;
        fDmgMinRWBase.fPoisonFrameLength = fPoisonFrameLengthRW;
        fDmgMaxRWBase.fPoisonPerFrame = fPoisonBitRateRW/256.f;
        fDmgMaxRWBase.fPoisonFrameLength = fPoisonFrameLengthRW;

        fDmgMinLWBase.clamp();
        fDmgMaxLWBase.clamp();
        fDmgMinRWBase.clamp();
        fDmgMaxRWBase.clamp();

        fDmgMultMin.clamp();
        fDmgMultMax.clamp();
        fDmgMultMin.clamp();
        fDmgMultMax.clamp();

        fStats.fDamageMinLW = fDmgMinLWBase*fDmgMultMin;
        fStats.fDamageMaxLW = fDmgMaxLWBase*fDmgMultMax;
        fStats.fDamageMinRW = fDmgMinRWBase*fDmgMultMin;
        fStats.fDamageMaxRW = fDmgMaxRWBase*fDmgMultMax;

        fStats.fAttackRatingLW = (fBaseAR + fFlatARLW)*(1.f + fBonusARLW);
        fStats.fAttackRatingRW = (fBaseAR + fFlatARRW)*(1.f + fBonusARRW);

        fStats.fMeeleSplashDamageLW = std::clamp(fStats.fMeeleSplashDamageLW, 0.f, 1.f);
        fStats.fMeeleSplashDamageRW = std::clamp(fStats.fMeeleSplashDamageRW, 0.f, 1.f);
    }
};

eSkillStats& eStats::leftSkill() {
    return fSkills[static_cast<int>(eSkillChoice::left)];
}

eSkillStats& eStats::rightSkill() {
    return fSkills[static_cast<int>(eSkillChoice::right)];
}

const eSkillStats& eStats::leftSkill() const {
    return fSkills[static_cast<int>(eSkillChoice::left)];
}

const eSkillStats& eStats::rightSkill() const {
    return fSkills[static_cast<int>(eSkillChoice::right)];
}

eSkillStats& eStats::skill(const eSkillChoice schoice) {
    return skill(static_cast<int>(schoice));
}

const eSkillStats& eStats::skill(const eSkillChoice schoice) const {
    return skill(static_cast<int>(schoice));
}

eSkillStats& eStats::skill(const int schoice) {
    return fSkills[schoice];
}

const eSkillStats& eStats::skill(const int schoice) const {
    return fSkills[schoice];
}

eWeaponType gWeaponType(const eItem& item) {
    if(item.fType == eItemType::weapon) {
        return static_cast<eWeaponType>(item.fSubType);
    } else if(item.fType == eItemType::shield) {
        return eWeaponType::shield;
    }
    return eWeaponType::none;
}

void eStats::calculate(const eAttributes& attr, const eEquipment& eq) {
    bool statsChanged = true;
    fLevel = attr.fLevel;
    fStrength = attr.fStrength;
    fDexterity = attr.fDexterity;
    fVitality = attr.fVitality;
    fEnergy = attr.fEnergy;

    const float healthFrac = fHealthF/fMaxHealth;
    const float manaFrac = fManaF/fMaxMana;

    const auto& leftW = (eq.fWeapons1 ?
                             eq.fWeapon1L :
                             eq.fWeapon2L);
    const auto& rightW = (eq.fWeapons1 ?
                              eq.fWeapon1R :
                              eq.fWeapon2R);

    const auto items = {
        eq.fBoots,
        eq.fGloves,
        eq.fHelmet,
        eq.fArmor,
        eq.fBelt,
        eq.fRingL,
        eq.fRingR,
        eq.fAmulet,
        leftW,
        rightW,
    };

    // defense
    float baseDef = 0.f;
    float ed = 0.f;

    // life
    float baseLife = 0.f;
    float bonusLife = 0.f;
    float lifeRegBonus = 0.f;

    // mana
    float baseMana = 0.f;
    float bonusMana = 0.f;
    float manaRegBonus = 0.f;

    fBlockChance = 0.f;
    fWalkRun = 0.f;
    fCastRate = 0.f;
    fAttackSpeedLW = 0.f;
    fAttackSpeedRW = 0.f;
    fFasterBlockRate = 0.f;
    fFasterHitRecovery = 0.f;

    fFireResistance = 0.f;
    fColdResistance = 0.f;
    fLightningResistance = 0.f;
    fPoisonResistance = 0.f;
    fPhysicalResistance = 0.f;

    fMaxFireResistance = 0.75f;
    fMaxColdResistance = 0.75f;
    fMaxLightningResistance = 0.75f;
    fMaxPoisonResistance = 0.75f;
    fMaxPhysicalResistance = 0.75f;

    fEffectiveSkillLevels = fBaseSkillLevels;

    const auto handleItemPassiveMod = [&](const eModifier& mod,
                                          const bool lw,
                                          const bool rw) {
        switch(mod.fType) {
        case eModifierType::defenseValue:
            baseDef += mod.fValue1;
            break;
        case eModifierType::defensePercent:
            ed += 0.01f*mod.fValue1;
            break;

        case eModifierType::blockChancePercent:
            fBlockChance += 0.01f*mod.fValue1;
            break;
        case eModifierType::walkRun:
            fWalkRun += 0.01f*mod.fValue1;
            break;
        case eModifierType::castRate:
            fCastRate += 0.01f*mod.fValue1;
            break;
        case eModifierType::attackSpeed: {
            if(lw) {
                fAttackSpeedLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fAttackSpeedRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::blockRecoverySpeed:
            fFasterBlockRate += 0.01f*mod.fValue1;
            break;
        case eModifierType::hitRecoverySpeed:
            fFasterHitRecovery += 0.01f*mod.fValue1;
            break;

        case eModifierType::lifeValue:
            baseLife += mod.fValue1;
            break;
        case eModifierType::lifePercent:
            bonusLife += 0.01f*mod.fValue1;
            break;

        case eModifierType::manaValue:
            baseMana += mod.fValue1;
            break;
        case eModifierType::manaPercent:
            bonusMana += 0.01f*mod.fValue1;
            break;

        case eModifierType::fireResistance:
            fFireResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::coldResistance:
            fColdResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::lightningResitance:
            fLightningResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::poisonResistance:
            fPoisonResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::physicalResistance:
            fPhysicalResistance += 0.01f*mod.fValue1;
            break;

        case eModifierType::maxFireResistance:
            fMaxFireResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::maxColdResistance:
            fMaxColdResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::maxLightningResitance:
            fMaxLightningResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::maxPoisonResistance:
            fMaxPoisonResistance += 0.01f*mod.fValue1;
            break;
        case eModifierType::maxPhysicalResistance:
            fMaxPhysicalResistance += 0.01f*mod.fValue1;
            break;

        case eModifierType::strength:
            fStrength += mod.fValue1;
            statsChanged = true;
            break;
        case eModifierType::dexterity:
            fDexterity += mod.fValue1;
            statsChanged = true;
            break;
        case eModifierType::vitality:
            fVitality += mod.fValue1;
            break;
        case eModifierType::energy:
            fEnergy += mod.fValue1;
            break;
        case eModifierType::allSkills: {
            const int inc = std::round(mod.fValue1);
            fEffectiveSkillLevels.incSkillLevels(inc);
        } break;
        case eModifierType::replenishLife:
            lifeRegBonus += mod.fValue1;
            break;
        case eModifierType::regenerateMana:
            manaRegBonus += mod.fValue1;
            break;
        case eModifierType::none:
        case eModifierType::count:
        case eModifierType::attackRatingValue:
        case eModifierType::attackRatingPercent:
        case eModifierType::damagePercent:
        case eModifierType::damageValue:
        case eModifierType::damageFire:
        case eModifierType::damageCold:
        case eModifierType::damageLightning:
        case eModifierType::damagePoison:
        case eModifierType::pierceChance:
        case eModifierType::lifeSteal:
        case eModifierType::manaSteal:
        case eModifierType::meeleSplashDamage:
        case eModifierType::knockback:
        case eModifierType::fireSkillDamage:
        case eModifierType::coldSkillDamage:
        case eModifierType::lightningSkillDamage:
        case eModifierType::poisonSkillDamage:
        case eModifierType::coldLength:
        case eModifierType::freezeLength:
            break;
        }
    };

    for(const auto& boost : fBoosts) {
        handleItemPassiveMod(boost, false, false);
    }

    for(const auto& it : fEffectiveSkillLevels) {
        const int skillId = it.first;
        if(skillId < 0) continue;
        const int skillLevelId = it.second;
        if(skillLevelId < 0) continue;
        const auto& skill = eSkills::sSkills.get(skillId);
        if(skill.fType != eSkillType::passive) continue;
        const auto& skillLevel = skill.skillLevel(skillLevelId);
        for(const auto& mod : skillLevel.fTotalModifiers) {
            handleItemPassiveMod(mod.second, false, false);
        }
    }

    std::vector<const eItem*> remItems;
    remItems.reserve(items.size());
    for(const auto& item : items) {
        remItems.emplace_back(&item);
    }
    while(!remItems.empty() && statsChanged) {
        statsChanged = false;
        for(int i = 0; i < remItems.size(); i++) {
            auto& item = *remItems[i];
            if(!itemReqsMet(item)) continue;
            eVectorHelpers::remove(remItems, &item);
            i--;
            switch(item.fType) {
            case eItemType::shield:
                fBlockChance += item.fBlockChance;
                [[fallthrough]];
            case eItemType::boots:
            case eItemType::gloves:
            case eItemType::helmet:
            case eItemType::armor:
            case eItemType::belt:
                baseDef += item.fDefense;
                [[fallthrough]];
            default: {
                const bool lw = &item != &rightW;
                const bool rw = &item != &leftW;
                for(const auto& mod : item.fModifiers) {
                    handleItemPassiveMod(mod, lw, rw);
                }
            } break;
            }
        }
    }

    for(int schoice = 0; schoice < fSkills.size(); schoice++) {
        calculateSkill(schoice, eq);
    };

    fWeaponMeeleRange = 0.f;
    int meeleRangeDiv = 0;
    fWeaponRangedRange = 10000.f;
    const auto handleWeapon = [&](const eItem& w, float& WSM) {
        const auto subtype = static_cast<eWeaponType>(w.fSubType);
        const bool r = gWeaponIsRanged(subtype);
        const auto& itemData = eItemsData::get(w.fDataId);
        WSM = itemData.fWSM;
        const float range = itemData.fRange;
        if(r) {
            fWeaponRangedRange = std::min(range, fWeaponRangedRange);
        }
        const bool m = gWeaponIsMeele(subtype);
        if(m) {
            meeleRangeDiv++;
            fWeaponMeeleRange += range;
        }
    };

    fWSMLW = 0.f;
    if(itemReqsMet(leftW)) {
        fWeaponTypeL = gWeaponType(leftW);
        if(leftW.fType == eItemType::weapon) {
            handleWeapon(leftW, fWSMLW);
        }
    } else {
        fWeaponTypeL = eWeaponType::none;
    }

    fWSMRW = 0.f;
    if(itemReqsMet(rightW)) {
        fWeaponTypeR = gWeaponType(rightW);
        if(rightW.fType == eItemType::weapon) {
            handleWeapon(rightW, fWSMRW);
        }
    } else {
        fWeaponTypeR = eWeaponType::none;
    }

    if(meeleRangeDiv > 0) {
        fWeaponMeeleRange /= meeleRangeDiv;
    }

    baseLife += 3.f*fVitality;
    baseMana += 1.5f*fEnergy;

    fDefense = baseDef*(1.f + ed) + fDexterity/4.f;
    fMaxHealth = baseLife*(1.f + bonusLife);
    fHealthF = healthFrac*fMaxHealth;
    fMaxMana = baseMana*(1.f + bonusMana);
    fManaF = manaFrac*fMaxMana;

    fMaxFireResistance = std::min(1.f, fMaxFireResistance);
    fMaxColdResistance = std::min(1.f, fMaxColdResistance);
    fMaxLightningResistance = std::min(1.f, fMaxLightningResistance);
    fMaxPoisonResistance = std::min(1.f, fMaxPoisonResistance);
    fMaxPhysicalResistance = std::min(1.f, fMaxPhysicalResistance);

    fFireResistance = std::min(fMaxFireResistance, fFireResistance);
    fColdResistance = std::min(fMaxColdResistance, fColdResistance);
    fLightningResistance = std::min(fMaxLightningResistance, fLightningResistance);
    fPoisonResistance = std::min(fMaxPoisonResistance, fPoisonResistance);
    fPhysicalResistance = std::min(fMaxPhysicalResistance, fPhysicalResistance);

    fManaRegeneration = fMaxMana*(100.f + manaRegBonus)/300000.f;
    fHealthRegeneration = lifeRegBonus/256.f;
}

void eStats::calculateSkill(const int schoice,
                            const eEquipment& eq) {
    auto& stats = fSkills[schoice];
    const int skillId = stats.fSkillId;
    stats = eSkillStats();
    stats.fSkillId = skillId;
    if(skillId < 0) return;
    const auto& skill = eSkills::sSkills.get(skillId);
    const int skillLevelId = fEffectiveSkillLevels.skillLevel(skillId);
    if(skillLevelId < 0) return;
    const auto& skillLevel = skill.skillLevel(skillLevelId);

    const auto handleSkillMod = [&](const eModifier& mod,
                                    const eModifierSource src,
                                    eSkillStatsHelper& helper,
                                    const bool lw, const bool rw) {
        auto& stats = helper.fStats;
        switch(mod.fType) {
        case eModifierType::attackRatingValue:
        case eModifierType::attackRatingPercent:
        case eModifierType::damagePercent:
        case eModifierType::damageValue:
        case eModifierType::damageFire:
        case eModifierType::damageCold:
        case eModifierType::damageLightning:
        case eModifierType::damagePoison:
        case eModifierType::pierceChance:
        case eModifierType::lifeSteal:
        case eModifierType::manaSteal:
        case eModifierType::meeleSplashDamage:
        case eModifierType::knockback:
        case eModifierType::attackSpeed:
        case eModifierType::fireSkillDamage:
        case eModifierType::coldSkillDamage:
        case eModifierType::lightningSkillDamage:
        case eModifierType::poisonSkillDamage:
        case eModifierType::coldLength:
        case eModifierType::freezeLength:
            helper.addMod(mod, src, lw, rw);
            break;
        case eModifierType::none:
        case eModifierType::count:

        case eModifierType::defenseValue:
        case eModifierType::defensePercent:

        case eModifierType::blockChancePercent:
        case eModifierType::walkRun:
        case eModifierType::castRate:
        case eModifierType::blockRecoverySpeed:
        case eModifierType::hitRecoverySpeed:

        case eModifierType::lifeValue:
        case eModifierType::lifePercent:

        case eModifierType::manaValue:
        case eModifierType::manaPercent:

        case eModifierType::fireResistance:
        case eModifierType::coldResistance:
        case eModifierType::lightningResitance:
        case eModifierType::poisonResistance:
        case eModifierType::physicalResistance:

        case eModifierType::maxFireResistance:
        case eModifierType::maxColdResistance:
        case eModifierType::maxLightningResitance:
        case eModifierType::maxPoisonResistance:
        case eModifierType::maxPhysicalResistance:

        case eModifierType::strength:
        case eModifierType::dexterity:
        case eModifierType::vitality:
        case eModifierType::energy:

        case eModifierType::allSkills:

        case eModifierType::replenishLife:
        case eModifierType::regenerateMana:
            break;
        }
    };

    const float minFistDmg = 1.f;
    const float maxFistDmg = 6.f;

    const float minFootDmg = 1.f;
    const float maxFootDmg = 6.f;

    eSkillStatsHelper helper{stats};

    const auto& leftW = (eq.fWeapons1 ?
                             eq.fWeapon1L :
                             eq.fWeapon2L);
    const auto& rightW = (eq.fWeapons1 ?
                              eq.fWeapon1R :
                              eq.fWeapon2R);

    const auto items = {
        eq.fBoots,
        eq.fGloves,
        eq.fHelmet,
        eq.fArmor,
        eq.fBelt,
        eq.fRingL,
        eq.fRingR,
        eq.fAmulet,
        leftW,
        rightW,
    };

    if(skill.fType == eSkillType::attack) {
        if(leftW.fType == eItemType::weapon) {
            float min;
            float max;
            gCalculateWeaponDmg(leftW, min, max);
            helper.fDmgMinLWBase.fPhysical += min;
            helper.fDmgMaxLWBase.fPhysical += max;
        } else if(leftW.fType == eItemType::none) {
            helper.fDmgMinLWBase.fPhysical += minFistDmg;
            helper.fDmgMaxLWBase.fPhysical += maxFistDmg;
        }
        if(rightW.fType == eItemType::weapon) {
            float min;
            float max;
            gCalculateWeaponDmg(rightW, min, max);
            helper.fDmgMinRWBase.fPhysical += min;
            helper.fDmgMaxRWBase.fPhysical += max;
        } else if(rightW.fType == eItemType::none) {
            if(leftW.fType == eItemType::none) {
                helper.fDmgMinRWBase.fPhysical += minFistDmg;
                helper.fDmgMaxRWBase.fPhysical += maxFistDmg;
            }
        }
    } else if(skill.fType == eSkillType::smite) {
        if(rightW.fType == eItemType::shield) {
            float min;
            float max;
            gCalculateWeaponDmg(rightW, min, max);
            helper.fDmgMinRWBase.fPhysical += min;
            helper.fDmgMaxRWBase.fPhysical += max;
        }
    } else if(skill.fType == eSkillType::kick) {
        const auto& boots = eq.fBoots;
        if(boots.fType == eItemType::boots) {
            float min;
            float max;
            gCalculateWeaponDmg(boots, min, max);
            helper.fDmgMinRWBase.fPhysical += min;
            helper.fDmgMaxRWBase.fPhysical += max;
        } else {
            helper.fDmgMinRWBase.fPhysical += minFootDmg;
            helper.fDmgMaxRWBase.fPhysical += maxFootDmg;
        }
    }
    if(skill.fType == eSkillType::missile ||
       skill.fType == eSkillType::shoot ||
       skill.fType == eSkillType::throw_ ||
       skill.fType == eSkillType::attack) {
        if(skill.fMissileId == -1) {
            const auto& itemDataL = eItemsData::get(leftW.fDataId);
            stats.fMissileIdLW = itemDataL.fMissileId;
            const auto& itemDataR = eItemsData::get(rightW.fDataId);
            stats.fMissileIdRW = itemDataR.fMissileId;
        } else {
            stats.fMissileIdLW = skill.fMissileId;
            stats.fMissileIdRW = skill.fMissileId;
        }
    }
    stats.fCount = skillLevel.fCount;
    stats.fManaCost += skillLevel.fManaCost;
    stats.fCooldown += skillLevel.fCooldown;

    if(skill.fType == eSkillType::attack ||
       skill.fType == eSkillType::shoot ||
       skill.fType == eSkillType::throw_) {
        stats.fMissileRangeTime = fWeaponRangedRange;
    } else {
        stats.fMissileRangeTime = skill.fRangeTime;
    }

    for(const auto& boost : fBoosts) {
        handleSkillMod(boost, eModifierSource::boost,
                       helper, true, true);
    }

    for(const auto& mod : skillLevel.fTotalModifiers) {
        handleSkillMod(mod.second, eModifierSource::skill,
                       helper, true, true);
    }
    for(const auto& item : items) {
        if(!itemReqsMet(item)) continue;
        for(const auto& mod : item.fModifiers) {
            if(item.fType == eItemType::weapon ||
                item.fType == eItemType::shield ||
                item.fType == eItemType::boots) {
                if(mod.fType == eModifierType::damagePercent ||
                    mod.fType == eModifierType::damageValue) {
                    continue;
                }
            }
            const bool lw = &item != &rightW;
            const bool rw = &item != &leftW;
            handleSkillMod(mod, eModifierSource::item,
                           helper, lw, rw);
        }
    }
    for(const auto& it : fEffectiveSkillLevels) {
        const int skillId = it.first;
        if(skillId < 0) continue;
        const int skillLevelId = it.second;
        if(skillLevelId < 0) continue;
        const auto& skill = eSkills::sSkills.get(skillId);
        if(skill.fType != eSkillType::passive) continue;
        const auto& skillLevel = skill.skillLevel(skillLevelId);
        for(const auto& mod : skillLevel.fTotalModifiers) {
            handleSkillMod(mod.second, eModifierSource::boost,
                           helper, true, true);
        }
    }

    for(const auto& s : skill.fSynergies) {
        const int sSkillId = s.fSkillId;
        const int sLevelId = effectiveSkillLevel(sSkillId);
        if(sLevelId < 0) continue;
        const int maxLevel = s.fBoostLevels.size() - 1;
        const int sMaxLevelId = std::min(sLevelId, maxLevel);
        for(int level = 0; level <= sMaxLevelId; level++) {
            const auto& boost = s.boostLevel(level);
            stats.fCount += boost.fCount;
            stats.fManaCost += boost.fManaCost;
            stats.fCooldown += boost.fCooldown;
            for(const auto& mod : boost.fTotalModifiers) {
                handleSkillMod(mod.second, eModifierSource::skill,
                               helper, true, true);
            }
        }
    }

    const float baseAR = (fDexterity - 7.f)*5.f + 20.f;
    const float attrMult = 0.01f*(fStrength + fDexterity);
    const auto skillType = helper.fSkillType;
    if(skillType == eSkillType::attack) {
        helper.fDmgMultMin.fPhysical += attrMult;
        helper.fDmgMultMax.fPhysical += attrMult;
    } else if(skillType == eSkillType::smite) {
        helper.fDmgMultMin.fPhysical += attrMult;
        helper.fDmgMultMax.fPhysical += attrMult;
    } else if(skillType == eSkillType::kick) {
        helper.fDmgMultMin.fPhysical += attrMult;
        helper.fDmgMultMax.fPhysical += attrMult;
    } else if(skillType == eSkillType::shoot) {
        helper.fDmgMultMin.fPhysical += attrMult;
        helper.fDmgMultMax.fPhysical += attrMult;
    } else if(skillType == eSkillType::throw_) {
        helper.fDmgMultMin.fPhysical += attrMult;
        helper.fDmgMultMax.fPhysical += attrMult;
    }

    helper.fBaseAR += baseAR;
    helper.apply();
}

bool eStats::canUseSkill(const eSkillChoice schoice) const {
    return canUseSkill(static_cast<int>(schoice));
}

bool eStats::rangedAttack(const eSkillChoice schoice) const {
    return rangedAttack(static_cast<int>(schoice));
}

float eStats::attackRange(const eSkillChoice schoice,
                          const float unit1Radius,
                          const float unit2Radius) const {
    return attackRange(static_cast<int>(schoice),
                       unit1Radius, unit2Radius);
}

bool eStats::canUseSkill(const int schoice) const {
    const auto& skillStats = fSkills[schoice];
    const int skillId = skillStats.fSkillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto skillType = skill.fType;
    const auto lw = fWeaponTypeL;
    const auto rw = fWeaponTypeR;
    switch(skillType) {
    case eSkillType::attack:
        return true;
    case eSkillType::smite:
        return lw == eWeaponType::shield ||
               rw == eWeaponType::shield;
    case eSkillType::kick:
        return true;
    case eSkillType::shoot:
        return lw == eWeaponType::ranged ||
               rw == eWeaponType::ranged;
    case eSkillType::throw_:
        return lw == eWeaponType::throwable ||
               rw == eWeaponType::throwable;
    case eSkillType::missile:
        return true;
    case eSkillType::wall:
        return true;
    case eSkillType::nova:
        return true;
    case eSkillType::summon:
        return true;
    case eSkillType::passive:
        return false;
    case eSkillType::aura:
        return false;
    case eSkillType::boostCurse:
        return true;
    }
    return false;
}

bool eStats::rangedAttack(const int schoice) const {
    const auto& skillStats = fSkills[schoice];
    const int skillId = skillStats.fSkillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto skillType = skill.fType;
    const auto lw = fWeaponTypeL;
    const auto rw = fWeaponTypeR;
    return skillType == eSkillType::missile ||
           skillType == eSkillType::nova ||
           skillType == eSkillType::wall ||
           skillType == eSkillType::summon ||
           skillType == eSkillType::shoot ||
           skillType == eSkillType::throw_ ||
           (skillId == 0 &&
            (lw == eWeaponType::ranged ||
             rw == eWeaponType::ranged));
}

float eStats::attackRange(const int schoice,
                          const float unit1Radius,
                          const float unit2Radius) const {
    const auto& skillStats = fSkills[schoice];
    const int skillId = skillStats.fSkillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const float meeleDist = fWeaponMeeleRange +
                            0.75f*(unit1Radius + unit2Radius);
    if(skill.fType == eSkillType::attack) {
        if(fWeaponTypeL == eWeaponType::none ||
           fWeaponTypeL == eWeaponType::meele ||
           fWeaponTypeL == eWeaponType::throwable ||
           fWeaponTypeR == eWeaponType::meele ||
           fWeaponTypeR == eWeaponType::throwable) {
            return meeleDist;
        } else {
            return fWeaponRangedRange;
        }
    } else if(skill.fType == eSkillType::smite ||
              skill.fType == eSkillType::kick) {
        return meeleDist;
    } else if(skill.fType == eSkillType::missile ||
              skill.fType == eSkillType::wall ||
              skill.fType == eSkillType::nova ||
              skill.fType == eSkillType::summon) {
        return skill.fCastRange;
    } else if(skill.fType == eSkillType::throw_ ||
              skill.fType == eSkillType::shoot) {
        return fWeaponRangedRange;
    }
    return meeleDist;
}

bool eStats::attackRangeSkill(const float minRange,
                              int& resultSchoice,
                              const float unit1Radius,
                              const float unit2Radius) const {
    std::vector<int> schoices;
    schoices.resize(fSkills.size());
    for(int i = 0; i < fSkills.size(); i++) {
        schoices[i] = i;
    }
    return attackRangeSkill(minRange, resultSchoice,
                            unit1Radius, unit2Radius);
}

bool eStats::attackRangeSkill(const float minRange,
                              int& resultSchoice,
                              const float unit1Radius,
                              const float unit2Radius,
                              std::vector<int>& schoices) const {
    eRand::randomShuffle(schoices);
    for(const int schoice : schoices) {
        const float r = attackRange(
            schoice, unit1Radius, unit2Radius);
        if(r >= minRange) {
            resultSchoice = schoice;
            return true;
        }
    }
    return false;
}

float eStats::maxRangeSkill(int& resultSchoice,
                            const float unit1Radius,
                            const float unit2Radius) const {
    float maxRange = 0.f;
    for(int schoice = 0; schoice < fSkills.size(); schoice++) {
        const float r = attackRange(
            schoice, unit1Radius, unit2Radius);
        if(r > maxRange) {
            maxRange = r;
            resultSchoice = schoice;
        }
    }
    return maxRange;
}

int eStats::baseSkillLevel(const int skillId) const {
    const auto it = fBaseSkillLevels.find(skillId);
    if(it == fBaseSkillLevels.end()) return -1;
    return it->second;
}

int eStats::effectiveSkillLevel(const int skillId) const {
    const auto it = fEffectiveSkillLevels.find(skillId);
    if(it == fEffectiveSkillLevels.end()) return -1;
    return it->second;
}

int eStats::incSkillLevel(const int skillId) {
    const int level = baseSkillLevel(skillId);
    if(fBaseSkillLevels.fRemainingPoints <= 0) return level;
    fBaseSkillLevels.fRemainingPoints--;
    fBaseSkillLevels[skillId] = level + 1;
    return level + 1;
}

float eStats::manaCost(const int schoice) const {
    const auto& skillStats = fSkills[schoice];
    return skillStats.fManaCost;
}

float eStats::cooldown(const int schoice) const {
    const auto& skillStats = fSkills[schoice];
    return skillStats.fCooldown;
}

bool eStats::itemReqsMet(const eItem& item) const {
    const auto& data = eItemsData::get(item.fDataId);
    const int level = std::max(data.fLevelReq, item.fRequiredLevel);
    const int str = data.fStrengthReq;
    const int dex = data.fDexterityReq;
    if(fLevel < level) return false;
    if(fStrength < str) return false;
    if(fDexterity < dex) return false;
    return true;
}

void eSkillLevels::read(ePacket& p) {
    p >> fRemainingPoints;
    uint16_t nSkills;
    p >> nSkills;
    for(int i = 0; i < nSkills; i++) {
        uint16_t skillId;
        p >> skillId;
        uint16_t level;
        p >> level;
        (*this)[skillId] = level;
    }
}

void eSkillLevels::write(ePacket& p) const {
    p << fRemainingPoints;
    const uint16_t nSkills = size();
    p << nSkills;
    for(const auto& skill : *this) {
        const uint16_t skillId = skill.first;
        p << skillId;
        const uint16_t level = skill.second;
        p << level;
    }
}

int eSkillLevels::skillLevel(const int skillId) const {
    const auto it = find(skillId);
    if(it == end()) return -1;
    return it->second;
}

void eSkillLevels::incSkillLevels(const int by) {
    for(auto& level : *this) {
        level.second += by;
    }
}
