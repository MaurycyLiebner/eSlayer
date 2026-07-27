#include "eSlayerHelpers/estats.h"

#include "eSlayerHelpers/eattributes.h"
#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/erand.h"
#include "eSlayerHelpers/eskills.h"
#include "eSlayerHelpers/evectorhelpers.h"
#include "eSlayerHelpers/eweaponchoice.h"
#include "eSlayerHelpers/eweaponclass.h"
#include "eSlayerHelpers/edifficulties.h"
#include "eSlayerHelpers/eunitsinfo.h"
#include "eSlayerHelpers/eclasses.h"
#include "eSlayerHelpers/eskilltrees.h"

uint32_t eAura::sNextId = 1;

eAura::eAura() : fId(sNextId++) {}

void eStats::levelUp(const int uinfoId) {
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    addSkillPoints(uinfo.fSkillPointsPerLevel);
}

void eStats::addSkillPoints(const int count) {
    fBaseSkillLevels.fRemainingPoints += count;
}

bool eStats::canUseSkill(const int schoice, const eWeaponChoice wchoice) const {
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
    case eSkillType::dualAttack:
        return (weapon == eWeaponType::meele ||
                weapon == eWeaponType::throwable) &&
               (otherWeapon == eWeaponType::meele ||
                otherWeapon == eWeaponType::throwable);
    case eSkillType::smite:
        return weapon == eWeaponType::shield;
    case eSkillType::kick:
        return wchoice == eWeaponChoice::right;
    case eSkillType::shoot:
        return weapon == eWeaponType::ranged;
    case eSkillType::throw_:
        return weapon == eWeaponType::throwable;
    case eSkillType::missile:
    case eSkillType::wall:
    case eSkillType::nova:
    case eSkillType::summon:
    case eSkillType::area:
    case eSkillType::boostCurse:
        return true;
    case eSkillType::passive:
    case eSkillType::aura:
        return false;
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
    item, skill, boost, aura
};

struct eSkillStatsHelper {
    eSkillStatsHelper(const eStats& stats, const eEquipment& eq,
                      eSkillStats& skillStats) :
        fStats(stats), fEq(eq),
        fSkillStats(skillStats) {
        const int skillId = fSkillStats.fSkillId;
        if(skillId == -1) {
            fSkillType = eSkillType::attack;
        } else {
            const auto& skill = eSkills::sSkills.get(skillId);
            fSkillType = skill.fType;
        }
    }

    const eStats& fStats;
    const eEquipment& fEq;
    eSkillStats& fSkillStats;
    eSkillType fSkillType;

    float fManaBurn = 0.f;

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

    float fHealMin = 0.f;
    float fHealMax = 0.f;

    float fPoisonBitRateLW = 0.f;
    float fPoisonBitRateRW = 0.f;
    float fPoisonFrameLengthLW = 0.f;
    float fPoisonFrameLengthRW = 0.f;

    bool applyMod(const eModifierType type, const eModifierSource src) const {
        switch(fSkillType) {
        case eSkillType::attack:
        case eSkillType::dualAttack:
        case eSkillType::kick:
        case eSkillType::smite:
        case eSkillType::shoot:
        case eSkillType::throw_:
            return true;
        case eSkillType::missile:
        case eSkillType::wall:
        case eSkillType::nova:
        case eSkillType::area:
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
            const float framesLength = mod.fValue2*25.f;
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
                fSkillStats.fPierceLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fSkillStats.fPierceRW += 0.01f*mod.fValue1;
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
                fSkillStats.fLifeStealLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fSkillStats.fLifeStealRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::manaSteal: {
            if(lw) {
                fSkillStats.fManaStealLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fSkillStats.fManaStealRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::meeleSplashDamage: {
            if(lw) {
                fSkillStats.fMeeleSplashDamageLW += 0.01f*mod.fValue1;
            }
            if(rw) {
                fSkillStats.fMeeleSplashDamageRW += 0.01f*mod.fValue1;
            }
        } break;
        case eModifierType::knockback: {
            if(lw) {
                fSkillStats.fKnockbackLW = true;
            }
            if(rw) {
                fSkillStats.fKnockbackRW = true;
            }
        } break;
        case eModifierType::alwaysHits: {
            if(lw) {
                fSkillStats.fAlwaysHitLW = true;
            }
            if(rw) {
                fSkillStats.fAlwaysHitRW = true;
            }
        } break;
        case eModifierType::attackTargets: {
            fSkillStats.fAttackTargets += mod.fValue1;
        } break;
        case eModifierType::fleshExplode: {
            fSkillStats.fExplode = eExplodeType::flesh;
        } break;
        case eModifierType::iceExplode: {
            fSkillStats.fExplode = eExplodeType::ice;
        } break;
        case eModifierType::attackSpeed: {
            if(src == eModifierSource::skill) {
                fSkillStats.fAttackSpeedS += 0.01f*mod.fValue1;
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
            const float framesLength = mod.fValue1*25.f;
            if(lw) {
                fSkillStats.fColdLengthLW += framesLength;
            }
            if(rw) {
                fSkillStats.fColdLengthRW += framesLength;
            }
        } break;
        case eModifierType::freezeLength: {
            const float framesLength = mod.fValue1*25.f;
            if(lw) {
                fSkillStats.fFreezeLengthLW += framesLength;
            }
            if(rw) {
                fSkillStats.fFreezeLengthRW += framesLength;
            }
        } break;

        case eModifierType::onAttack: {
            const auto skill = fStats.statsFromMod(mod, fEq);
            if(lw) {
                fSkillStats.fOnAttackLW.emplace_back(skill);
            }
            if(rw) {
                fSkillStats.fOnAttackRW.emplace_back(skill);
            }
        } break;
        case eModifierType::onStriking: {
            const auto skill = fStats.statsFromMod(mod, fEq);
            if(lw) {
                fSkillStats.fOnStrikingLW.emplace_back(skill);
            }
            if(rw) {
                fSkillStats.fOnStrikingRW.emplace_back(skill);
            }
        } break;
        case eModifierType::onKill: {
            const auto skill = fStats.statsFromMod(mod, fEq);
            if(lw) {
                fSkillStats.fOnKillLW.emplace_back(skill);
            }
            if(rw) {
                fSkillStats.fOnKillRW.emplace_back(skill);
            }
        } break;
        case eModifierType::manaBurn: {
            fSkillStats.fManaBurn += 0.01f*mod.fValue1;
        } break;
        case eModifierType::multiShot: {
            if(fSkillStats.fMissileIdLW > 0) {
                fSkillStats.fCountLW += mod.fValue1;
            }
            if(fSkillStats.fMissileIdRW > 0) {
                fSkillStats.fCountRW += mod.fValue1;
            }
        } break;
        case eModifierType::spectralHit: {
            if(lw) {
                fSkillStats.fSpectralHitMinLW += 0.01f*mod.fValue1;
                fSkillStats.fSpectralHitMaxLW += 0.01f*mod.fValue2;
            }
            if(rw) {
                fSkillStats.fSpectralHitMinRW += 0.01f*mod.fValue1;
                fSkillStats.fSpectralHitMaxRW += 0.01f*mod.fValue2;
            }
        } break;
        case eModifierType::heal: {
            if(lw) {
                fSkillStats.fHealMinLW += mod.fValue1;
                fSkillStats.fHealMaxLW += mod.fValue2;
            }
            if(rw) {
                fSkillStats.fHealMinRW += mod.fValue1;
                fSkillStats.fHealMaxRW += mod.fValue2;
            }
        } break;
        case eModifierType::immobilize: {
            const float framesLength = mod.fValue1*25.f;
            if(lw) {
                fSkillStats.fImmobilizeLengthLW += framesLength;
            }
            if(rw) {
                fSkillStats.fImmobilizeLengthRW += framesLength;
            }
        } break;
        default:
            break;
        }
    }

    void clamp(eDamage& min, const eDamage& max) const {
        min.fPhysical = std::min(
            min.fPhysical, max.fPhysical);
        min.fFire = std::min(
            min.fFire, max.fFire);
        min.fCold = std::min(
            min.fCold, max.fCold);
        min.fLightning = std::min(
            min.fLightning, max.fLightning);
        min.fPoisonFrameLength = std::min(
            min.fPoisonFrameLength, max.fPoisonFrameLength);
        min.fPoisonPerFrame = std::min(
            min.fPoisonPerFrame, max.fPoisonPerFrame);
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

        fSkillStats.fDamageMinLW = fDmgMinLWBase*fDmgMultMin;
        fSkillStats.fDamageMaxLW = fDmgMaxLWBase*fDmgMultMax;
        fSkillStats.fDamageMinRW = fDmgMinRWBase*fDmgMultMin;
        fSkillStats.fDamageMaxRW = fDmgMaxRWBase*fDmgMultMax;

        clamp(fSkillStats.fDamageMinLW, fSkillStats.fDamageMaxLW);
        clamp(fSkillStats.fDamageMinRW, fSkillStats.fDamageMaxRW);

        fSkillStats.fAttackRatingLW = (fBaseAR + fFlatARLW)*(1.f + fBonusARLW);
        fSkillStats.fAttackRatingRW = (fBaseAR + fFlatARRW)*(1.f + fBonusARRW);

        fSkillStats.fMeeleSplashDamageLW = std::clamp(fSkillStats.fMeeleSplashDamageLW, 0.f, 1.f);
        fSkillStats.fMeeleSplashDamageRW = std::clamp(fSkillStats.fMeeleSplashDamageRW, 0.f, 1.f);
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
        const auto& class_ = eWeaponClasses::sClasses.get(item.fSubType);
        return class_.fType;
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
    const float staminaFrac = fStaminaF/fMaxStamina;

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

    float baseStamina = 0.f;
    float bonusStamina = 0.f;

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

    if(fDifficultyPenalties) {
        const int diffId = eDifficulties::sDifficulty;
        const auto& diff = eDifficulties::sDifficulties.get(diffId);
        const auto penalty = diff.fResistPenalty;
        fFireResistance += penalty;
        fColdResistance += penalty;
        fLightningResistance += penalty;
        fPoisonResistance += penalty;
    }

    fMaxFireResistance = 0.75f;
    fMaxColdResistance = 0.75f;
    fMaxLightningResistance = 0.75f;
    fMaxPoisonResistance = 0.75f;
    fMaxPhysicalResistance = 0.75f;

    fDealsDamageMin = eDamage();
    fDealsDamageMax = eDamage();

    fEffectiveSkillLevels = fBaseSkillLevels;

    int allSkillsInc = 0;
    std::map<int, int> classSkillsInc;

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
        case eModifierType::lightningResistance:
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
        case eModifierType::maxLightningResistance:
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
            allSkillsInc += inc;
        } break;
        case eModifierType::allClassSkills: {
            if(fClass == mod.fClassId) {
                const int inc = std::round(mod.fValue1);
                fEffectiveSkillLevels.incClassSkillLevels(fClass, inc);
            }
            classSkillsInc[fClass]++;
        } break;
        case eModifierType::classSkill: {
            if(fClass == mod.fClassId) {
                int inc = std::round(mod.fValue1);
                const bool ini = fEffectiveSkillLevels.count(mod.fSkillId) == 0;
                if(ini) {
                    inc += allSkillsInc;
                    inc += classSkillsInc[mod.fClassId];
                }
                fEffectiveSkillLevels.incSkillLevel(inc, mod.fSkillId);
            }
        } break;
        case eModifierType::skill: {
            int inc = std::round(mod.fValue1);
            const bool ini = fEffectiveSkillLevels.count(mod.fSkillId) == 0;
            if(ini) {
                inc += allSkillsInc;
                for(const auto& it : classSkillsInc) {
                    const auto classId = it.first;
                    const auto& class_ = eClasses::sClasses.get(classId);
                    const bool r = class_.isClassSkill(mod.fSkillId);
                    if(r) inc += it.second;
                }
            }
            fEffectiveSkillLevels.incSkillLevel(inc, mod.fSkillId);
        } break;
        case eModifierType::replenishLife:
            lifeRegBonus += mod.fValue1;
            break;
        case eModifierType::regenerateMana:
            manaRegBonus += mod.fValue1;
            break;
        case eModifierType::onStruck: {
            const auto skill = statsFromMod(mod, eq);
            fOnStruck.emplace_back(skill);
        } break;
        case eModifierType::onDeath: {
            const auto skill = statsFromMod(mod, eq);
            fOnDeath.emplace_back(skill);
        } break;

        case eModifierType::dealsFireDamage: {
            fDealsDamageMin.fFire += mod.fValue1;
            fDealsDamageMax.fFire += mod.fValue2;
        } break;
        case eModifierType::dealsColdDamage: {
            fDealsDamageMin.fCold += mod.fValue1;
            fDealsDamageMax.fCold += mod.fValue2;
        } break;
        case eModifierType::dealsColdLength: {
            fDealsColdLength = std::max(fDealsColdLength, 25.f*mod.fValue1);
        } break;
        case eModifierType::dealsFreezeLength: {
            fDealsFreezeLength = std::max(fDealsFreezeLength, 25.f*mod.fValue1);
        } break;
        case eModifierType::dealsLightningDamage: {
            fDealsDamageMin.fLightning += mod.fValue1;
            fDealsDamageMax.fLightning += mod.fValue2;
        } break;
        case eModifierType::dealsPoisonDamage: {
            const float framesLength = mod.fValue2*25.f;
            const float bitRate = 256.f*mod.fValue1/framesLength;
            fDealsDamageMin.fPoisonPerFrame += bitRate/256.f;
            fDealsDamageMax.fPoisonFrameLength += framesLength;
        } break;
        case eModifierType::dealsPhysicalDamage: {
            fDealsDamageMin.fPhysical += mod.fValue1;
            fDealsDamageMax.fPhysical += mod.fValue2;
        } break;

        case eModifierType::aura: {
        } break;

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

        case eModifierType::onAttack:
        case eModifierType::onStriking:
        case eModifierType::onKill:

        case eModifierType::fleshExplode:
        case eModifierType::iceExplode:

        case eModifierType::skillLevel:

        case eModifierType::manaBurn:
        case eModifierType::multiShot:
        case eModifierType::spectralHit:

        case eModifierType::curesPoison:
        case eModifierType::curesCold:

        case eModifierType::heal:
        case eModifierType::immobilize:
        case eModifierType::alwaysHits:

        case eModifierType::attackTargets:
            break;
        }
    };

    for(const auto& it : fBoosts) {
        const auto& boost = it.second;
        handleItemPassiveMod(boost, false, false);
    }

    for(const auto& it : fAuraBoosts) {
        const auto& aura = it.second;
        handleItemPassiveMod(aura, false, false);
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
        if(item.fType == eItemType::none) continue;
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

    fWeaponMeeleRange = 0.f;
    int meeleRangeDiv = 0;
    std::optional<float> rangedRange;
    const auto handleWeapon = [&](const eItem& w, float& WSM) {
        const auto subtype = gWeaponType(w);
        const bool r = gWeaponIsRanged(subtype);
        const auto& itemData = eItemsData::get(w.fDataId);
        WSM = itemData.fWSM;
        const float range = itemData.fRange;
        if(r) {
            if(rangedRange) {
                rangedRange = std::min(range, *rangedRange);
            } else {
                rangedRange = range;
            }
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

    if(rangedRange) {
        fWeaponRangedRange = *rangedRange;
    } else {
        fWeaponRangedRange = fDefaultRangedRange;
    }

    if(meeleRangeDiv > 1) {
        fWeaponMeeleRange /= meeleRangeDiv;
    }

    for(int schoice = 0; schoice < fSkills.size(); schoice++) {
        calculateSkill(schoice, eq);
    }

    const auto& class_ = eClasses::sClasses.get(fClass);

    baseLife += class_.fHealthPerVitality*fVitality;
    baseMana += class_.fManaPerEnergy*fEnergy;
    baseStamina += class_.fStaminaPerVitality*fVitality;

    fDefense = baseDef*(1.f + ed) + class_.fDefensePerDexterity*fDexterity;
    fMaxHealth = std::max(1.f, baseLife*(1.f + bonusLife));
    fHealthF = healthFrac*fMaxHealth;
    fMaxMana = baseMana*(1.f + bonusMana);
    fManaF = manaFrac*fMaxMana;

    fMaxStamina = baseStamina*(1.f + bonusStamina);
    fStaminaF = staminaFrac*fMaxStamina;

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

void eStats::calculateAuras(const eEquipment& eq) {
    fAuras.clear();

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

    const auto addAura = [&](const eSkill& skill,
                             const eModsCollection& mods) {
        auto& a = fAuras.emplace_back();
        a.fRange = mods.fRadius;
        a.fType = skill.fAuraType;
        a.fTarget = skill.fAuraTarget;
        a.fMissileId = skill.fAreaMissileId;
        a.fSelfMissileId = skill.fSelfAreaMissileId;
        a.fMods.reserve(mods.size());
        for(const auto& m : mods) {
            a.fMods.emplace_back(m.second);
        }
    };

    const auto handleAuraMod = [&](const eModifier& mod) {
        if(mod.fType != eModifierType::aura) return;
        const auto id = mod.fSkillId;
        const auto& skill = eSkills::sSkills.get(id);
        if(skill.fType == eSkillType::aura) {
            const auto level = mod.fValue2;
            if(level >= 0) {
                const auto& levelStats = skill.skillLevel(level);
                const auto& mods = levelStats.fTotalModifiers;
                addAura(skill, mods);
            }
        }
    };

    for(const auto& b : fBoosts) {
        const auto& mod = b.second;
        handleAuraMod(mod);
    }

    for(const auto& item : items) {
        const auto& mods = item.fModifiers;
        for(const auto& mod : mods) {
            handleAuraMod(mod);
        }
    }

    for(const auto& stats : fSkills) {
        const auto id = stats.fSkillId;
        const auto& skill = eSkills::sSkills.get(id);
        if(skill.fType != eSkillType::aura) continue;
        const auto level = fEffectiveSkillLevels.skillLevel(id);
        if(level < 0) continue;
        const auto& levelStats = skill.skillLevel(level);
        auto mods = levelStats.fTotalModifiers;

        for(const auto& s : skill.fSynergies) {
            const int sSkillId = s.fSkillId;
            const int sLevelId = effectiveSkillLevel(sSkillId);
            if(sLevelId < 0) continue;
            const auto& boost = s.boostLevel(sLevelId);
            mods.addBoost(boost.fTotalModifiers);
        }

        mods.collapse();

        addAura(skill, mods);
    }
}

void eStats::calculateSkill(
    const int schoice, const eEquipment& eq) {
    auto& stats = fSkills[schoice];
    calculateSkill(stats, eq, false);
}

void eStats::calculateSkill(eSkillStats& stats,
                            const eEquipment& eq,
                            const bool chanceSkill) const {
    const int skillIdTmp = stats.fSkillId;
    const float castChanceTmp = stats.fCastChance;
    const int skillLevelIdTmp = stats.fSkillLevelId;
    stats = eSkillStats();
    stats.fSkillId = skillIdTmp;
    stats.fCastChance = castChanceTmp;
    stats.fSkillLevelId = skillLevelIdTmp;
    if(skillIdTmp < 0) return;
    const auto& skill = eSkills::sSkills.get(skillIdTmp);
    const int skillLevelId = chanceSkill ? skillLevelIdTmp :
        fEffectiveSkillLevels.skillLevel(skillIdTmp);
    if(skillLevelId < 0) return;
    const auto& skillLevel = skill.skillLevel(skillLevelId);

    const auto handleSkillMod = [&](const eModifier& mod,
                                    const eModifierSource src,
                                    eSkillStatsHelper& helper,
                                    const bool lw, const bool rw) {
        auto& stats = helper.fSkillStats;
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

        case eModifierType::onAttack:
        case eModifierType::onStriking:
        case eModifierType::onKill:

        case eModifierType::fleshExplode:
        case eModifierType::iceExplode:

        case eModifierType::manaBurn:
        case eModifierType::multiShot:
        case eModifierType::spectralHit:

        case eModifierType::heal:
        case eModifierType::immobilize:
        case eModifierType::alwaysHits:

        case eModifierType::attackTargets:
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
        case eModifierType::lightningResistance:
        case eModifierType::poisonResistance:
        case eModifierType::physicalResistance:

        case eModifierType::maxFireResistance:
        case eModifierType::maxColdResistance:
        case eModifierType::maxLightningResistance:
        case eModifierType::maxPoisonResistance:
        case eModifierType::maxPhysicalResistance:

        case eModifierType::strength:
        case eModifierType::dexterity:
        case eModifierType::vitality:
        case eModifierType::energy:

        case eModifierType::allSkills:

        case eModifierType::replenishLife:
        case eModifierType::regenerateMana:

        case eModifierType::onStruck:
        case eModifierType::onDeath:

        case eModifierType::skillLevel:

        case eModifierType::aura:

        case eModifierType::dealsFireDamage:
        case eModifierType::dealsColdDamage:
        case eModifierType::dealsLightningDamage:
        case eModifierType::dealsPoisonDamage:
        case eModifierType::dealsPhysicalDamage:

        case eModifierType::curesPoison:
        case eModifierType::curesCold:

        case eModifierType::allClassSkills:
        case eModifierType::classSkill:
        case eModifierType::skill:

        case eModifierType::dealsColdLength:
        case eModifierType::dealsFreezeLength:
            break;
        }
    };

    const auto& class_ = eClasses::sClasses.get(fClass);

    const float minFistDmg = class_.fMinFistDamage;
    const float maxFistDmg = class_.fMaxFistDamage;

    const float minFootDmg = class_.fMinFootDamage;
    const float maxFootDmg = class_.fMaxFootDamage;

    eSkillStatsHelper helper{*this, eq, stats};

    const auto& leftW = (eq.fWeapons1 ?
                             eq.fWeapon1L :
                             eq.fWeapon2L);
    const auto& rightW = (eq.fWeapons1 ?
                              eq.fWeapon1R :
                              eq.fWeapon2R);

    const auto items = {
        &eq.fBoots,
        &eq.fGloves,
        &eq.fHelmet,
        &eq.fArmor,
        &eq.fBelt,
        &eq.fRingL,
        &eq.fRingR,
        &eq.fAmulet,
        &leftW,
        &rightW,
    };

    if(skill.fType == eSkillType::attack ||
       skill.fType == eSkillType::dualAttack ||
       skill.fType == eSkillType::shoot ||
       skill.fType == eSkillType::throw_) {
        if(leftW.fType == eItemType::weapon &&
           itemReqsMet(leftW)) {
            float min;
            float max;
            gCalculateWeaponDmg(leftW, min, max);
            helper.fDmgMinLWBase.fPhysical += min;
            helper.fDmgMaxLWBase.fPhysical += max;
        } else if(leftW.fType == eItemType::none) {
            helper.fDmgMinLWBase.fPhysical += minFistDmg;
            helper.fDmgMaxLWBase.fPhysical += maxFistDmg;
        }
        if(rightW.fType == eItemType::weapon &&
           itemReqsMet(rightW)) {
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
        if(rightW.fType == eItemType::shield &&
           itemReqsMet(rightW)) {
            float min;
            float max;
            gCalculateWeaponDmg(rightW, min, max);
            helper.fDmgMinRWBase.fPhysical += min;
            helper.fDmgMaxRWBase.fPhysical += max;
        }
    } else if(skill.fType == eSkillType::kick) {
        const auto& boots = eq.fBoots;
        if(boots.fType == eItemType::boots &&
           itemReqsMet(boots)) {
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


    auto skillMods = skillLevel.fTotalModifiers;

    if(!chanceSkill) {
        for(const auto& s : skill.fSynergies) {
            const int sSkillId = s.fSkillId;
            const int sLevelId = effectiveSkillLevel(sSkillId);
            if(sLevelId < 0) continue;
            const auto& boost = s.boostLevel(sLevelId);
            skillMods.addBoost(boost.fTotalModifiers);
        }
    }

    skillMods.collapse();

    if(skill.fType == eSkillType::missile ||
       skill.fType == eSkillType::shoot ||
       skill.fType == eSkillType::throw_ ||
       skill.fType == eSkillType::attack) {
        if(skill.fMissileId <= 0) {
            if(leftW.fType == eItemType::none) {
                stats.fMissileIdLW = fDefaultMissileId;
            } else {
                const auto& itemDataL = eItemsData::get(leftW.fDataId);
                stats.fMissileIdLW = itemDataL.fMissileId;
            }
            if(rightW.fType == eItemType::none) {
                stats.fMissileIdRW = fDefaultMissileId;
            } else {
                const auto& itemDataR = eItemsData::get(rightW.fDataId);
                stats.fMissileIdRW = itemDataR.fMissileId;
            }
        } else {
            stats.fMissileIdLW = skill.fMissileId;
            stats.fMissileIdRW = skill.fMissileId;
        }
    }

    if(stats.fMissileIdLW > 0 || stats.fMissileIdRW > 0) {
        stats.fAttackType = eAttackRangeType::ranged;
    } else {
        stats.fAttackType = eAttackRangeType::meele;
    }

    if(skill.fType == eSkillType::attack ||
       skill.fType == eSkillType::shoot ||
       skill.fType == eSkillType::throw_) {
       stats.fMissileRange = fWeaponRangedRange;
    } else {
        stats.fMissileRange = skill.fRange;
    }
    stats.fMissileTime = skill.fTime;

    stats.fCountLW = std::max(1, skillMods.fCount);
    stats.fCountRW = std::max(1, skillMods.fCount);

    stats.fManaCost = skillMods.fManaCost;
    stats.fRadius = skillMods.fRadius;
    stats.fCooldown = skillMods.fCooldown;
    stats.fConsecutive = skillMods.fConsecutive;

    for(const auto& it : fBoosts) {
        const auto& boost = it.second;
        handleSkillMod(boost, eModifierSource::boost,
                       helper, true, true);
    }

    for(const auto& it : fAuraBoosts) {
        const auto& aura = it.second;
        handleSkillMod(aura, eModifierSource::aura,
                       helper, true, true);
    }

    switch(skill.fType) {
    case eSkillType::boostCurse: {
        auto& bc = stats.fBoostCurse.emplace_back();
        bc.fType = skill.fBoostCurseType;
        bc.fMissileId = skill.fMissileId;
        bc.fTime = skill.fBoostCurseTime;
        for(const auto& mod : skillMods) {
            bc.fMods.emplace_back(mod.second);
        }
    } break;
    default: {
        for(const auto& mod : skillMods) {
            handleSkillMod(mod.second, eModifierSource::skill,
                           helper, true, true);
        }
    } break;
    }

    for(const auto item : items) {
        const auto& itemRef = *item;
        if(!itemReqsMet(itemRef)) continue;
        for(const auto& mod : itemRef.fModifiers) {
            if(itemRef.fType == eItemType::weapon ||
               itemRef.fType == eItemType::shield ||
               itemRef.fType == eItemType::boots) {
                if(mod.fType == eModifierType::damagePercent ||
                   mod.fType == eModifierType::damageValue) {
                    continue;
                }
            }
            const bool lw = item != &rightW;
            const bool rw = item != &leftW;
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

    const auto minARDex = class_.fMinARDexterity;
    const float ARPerDex = class_.fARPerDexterity;
    const float baseAR = std::max(0.f, fDexterity - minARDex)*ARPerDex + class_.fBaseAR;
    const auto skillType = helper.fSkillType;
    float attrMult = 0.f;
    if(skillType == eSkillType::attack ||
       skillType == eSkillType::dualAttack) {
        attrMult = class_.fStrengthAttackDamageMultiplier*fStrength +
                   class_.fDexterityAttackDamageMultiplier*fDexterity;
    } else if(skillType == eSkillType::smite) {
        attrMult = class_.fStrengthSmiteDamageMultiplier*fStrength +
                   class_.fDexteritySmiteDamageMultiplier*fDexterity;
    } else if(skillType == eSkillType::kick) {
        attrMult = class_.fStrengthKickDamageMultiplier*fStrength +
                   class_.fDexterityKickDamageMultiplier*fDexterity;
    } else if(skillType == eSkillType::shoot) {
        attrMult = class_.fStrengthShootDamageMultiplier*fStrength +
                   class_.fDexterityShootDamageMultiplier*fDexterity;
    } else if(skillType == eSkillType::throw_) {
        attrMult = class_.fStrengthThrowDamageMultiplier*fStrength +
                   class_.fDexterityThrowDamageMultiplier*fDexterity;
    }
    helper.fDmgMultMin.fPhysical += attrMult;
    helper.fDmgMultMax.fPhysical += attrMult;

    helper.fBaseAR += baseAR;
    helper.apply();

    if(fDifficultyPenalties) {
        const int diffId = eDifficulties::sDifficulty;
        const auto& diff = eDifficulties::sDifficulties.get(diffId);
        const float coldLenMult = std::clamp(1.f + diff.fColdLengthPenalty, 0.f, 1.f);
        helper.fSkillStats.fColdLengthLW *= coldLenMult;
        helper.fSkillStats.fColdLengthRW *= coldLenMult;
        const float freezeLenMult = std::clamp(1.f + diff.fFreezeLengthPenalty, 0.f, 1.f);
        helper.fSkillStats.fFreezeLengthLW *= freezeLenMult;
        helper.fSkillStats.fFreezeLengthRW *= freezeLenMult;
        const float leechMult = std::clamp(1.f + diff.fLeechPenalty, 0.f, 1.f);
        helper.fSkillStats.fLifeStealLW *= leechMult;
        helper.fSkillStats.fLifeStealRW *= leechMult;
        helper.fSkillStats.fManaStealLW *= leechMult;
        helper.fSkillStats.fManaStealRW *= leechMult;
    }
}

eSkillStats eStats::statsFromMod(
    const eModifier& mod, const eEquipment& eq) const {
    eSkillStats result;
    result.fSkillId = mod.fSkillId;
    result.fSkillLevelId = mod.fValue2;
    result.fCastChance = 0.01f * mod.fValue1;
    calculateSkill(result, eq, true);
    return result;
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
    return canUseSkillId(skillId);
}

bool eStats::canUseSkillId(const int skillId) const {
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto skillType = skill.fType;
    const auto lw = fWeaponTypeL;
    const auto rw = fWeaponTypeR;
    switch(skillType) {
    case eSkillType::attack:
        return true;
    case eSkillType::dualAttack:
        return (lw == eWeaponType::meele ||
                lw == eWeaponType::throwable) &&
               (rw == eWeaponType::meele ||
                rw == eWeaponType::throwable);
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
    case eSkillType::wall:
    case eSkillType::nova:
    case eSkillType::summon:
    case eSkillType::area:
    case eSkillType::boostCurse:
        return true;
    case eSkillType::passive:
    case eSkillType::aura:
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
           skillType == eSkillType::area ||
           skillType == eSkillType::boostCurse ||
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
    switch(skill.fType) {
    case eSkillType::attack: {
        const auto attackType = skillStats.fAttackType;
        if(attackType == eAttackRangeType::meele) {
            return meeleDist;
        } else {
            return fWeaponRangedRange;
        }
    } break;
    case eSkillType::dualAttack:
        return meeleDist;
    case eSkillType::smite:
    case eSkillType::kick:
        return meeleDist;
    case eSkillType::missile:
    case eSkillType::wall:
    case eSkillType::boostCurse:
    case eSkillType::area:
    case eSkillType::summon:
        return skill.fRange;
    case eSkillType::nova:
        return skillStats.fRadius;
    case eSkillType::throw_:
    case eSkillType::shoot:
        return fWeaponRangedRange;
    case eSkillType::passive:
    case eSkillType::aura:
        return 0.f;
    }
    return 0.f;
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
    uint8_t& rem = skillPoints();
    if(rem <= 0) return level;
    rem--;
    uint16_t& levelRef = fBaseSkillLevels[skillId];
    if(levelRef + 1 >= eSkills::sMaxSkillLevel) return level;
    levelRef = level + 1;
    return levelRef;
}

float eStats::manaCost(const int schoice) const {
    const auto& skillStats = fSkills[schoice];
    return skillStats.fManaCost;
}

float eStats::cooldown(const int schoice) const {
    const auto& skillStats = fSkills[schoice];
    return skillStats.fCooldown;
}

float eStats::cooldownBySkillId(const int skillId) const {
    const auto& skill = eSkills::sSkills.get(skillId);
    const int skillLevelId = fEffectiveSkillLevels.skillLevel(skillId);
    if(skillLevelId < 0) return 0.f;
    const auto& skillLevel = skill.skillLevel(skillLevelId);
    const auto& skillMods = skillLevel.fTotalModifiers;
    return skillMods.fCooldown;
}

void eStats::decCooldowns(const float by) {
    for(auto& it : fCooldowns) {
        it.second = std::max(0.f, it.second - by);
    }
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

bool eStats::validLevelsChange(const eSkillLevels& levels) const {
    const auto fromTotal = fBaseSkillLevels.totalPoints();
    const auto toTotal = levels.totalPoints();
    if(fromTotal != toTotal) return false;
    const auto& class_ = eClasses::sClasses.get(fClass);
    for(const auto& it : levels) {
        const auto skillId = it.first;
        if(skillId == 0) continue;
        const bool r = class_.isClassSkill(skillId);
        if(!r) return false;
        for(const auto skillTreeId : class_.fSkillTrees) {
            const auto& skillTree = eSkillTrees::sTrees.get(skillTreeId);
            for(const auto& skill : skillTree.fSkills) {
                if(skill.fSkillId != skillId) continue;
                if(skill.fLevelReq > fLevel) return false;
                for(const auto p : skill.fPrerequisites) {
                    if(levels.skillLevel(p) < 0) return false;
                }
            }
        }
    }
    return true;
}

uint8_t& eStats::skillPoints() {
    return fBaseSkillLevels.fRemainingPoints;
}

const uint8_t eStats::skillPoints() const {
    return fBaseSkillLevels.fRemainingPoints;
}
