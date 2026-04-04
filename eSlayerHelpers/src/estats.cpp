#include "eSlayerHelpers/estats.h"

#include "eSlayerHelpers/eattributes.h"
#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eskills.h"
#include "eSlayerHelpers/erunsettings.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/erand.h"

void gCalculateWeaponDmg(const eItem& weapon,
                         float& min, float& max) {
    float baseMin = weapon.fValue1;
    float baseMax = weapon.fValue2;
    float percentIncMin = 0.f;
    float percentIncMax = 0.f;
    for(const auto& mod : weapon.fModifiers) {
        switch(mod.fType) {
        case eModifierType::damagePercent:
            percentIncMin += mod.fValue1;
            percentIncMax += mod.fValue1;
            break;
        case eModifierType::damageValue:
            baseMin += mod.fValue1;
            baseMax += mod.fValue1;
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
    item, skill
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

    eDamage fDmgMultMin{1.f, 1.f, 1.f, 1.f};
    eDamage fDmgMultMax{1.f, 1.f, 1.f, 1.f};

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
        case eSkillType::throw_: {
            return true;
        } break;
        case eSkillType::missile:
        case eSkillType::wall: {
            return src == eModifierSource::skill;
        } break;
        }
        return false;
    }

    void addMod(const eModifier& mod, const eModifierSource src,
                const bool lw, const bool rw) {
        const bool r = applyMod(mod.fType, src);
        if(!r) return;
        switch(mod.fType) {
        case eModifierType::damagePercent: {
            fDmgMultMin.fPhysical += mod.fValue1;
            fDmgMultMax.fPhysical += mod.fValue2;
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
            const float bitRate = (mod.fValue1*256.f)/(mod.fValue2*eRunSettings::sFPS);
            const float framesLength = mod.fValue2*eRunSettings::sFPS;
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
                fStats.fPierceLW += mod.fValue1;
            }
            if(rw) {
                fStats.fPierceRW += mod.fValue1;
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
                fBonusARLW += mod.fValue1;
            }
            if(rw) {
                fBonusARRW += mod.fValue1;
            }
        } break;
        default:
            break;
        }
    }

    void apply() const {
        fStats.fDamageMinLW = fDmgMinLWBase*fDmgMultMin;
        fStats.fDamageMaxLW = fDmgMaxLWBase*fDmgMultMax;
        fStats.fDamageMinRW = fDmgMinRWBase*fDmgMultMin;
        fStats.fDamageMaxRW = fDmgMaxRWBase*fDmgMultMax;

        fStats.fDamageMinLW.fPoisonPerFrame = fPoisonBitRateLW/256.f;
        fStats.fDamageMinLW.fPoisonFrameLength = fPoisonFrameLengthLW;
        fStats.fDamageMaxLW.fPoisonPerFrame = fPoisonBitRateLW/256.f;
        fStats.fDamageMaxLW.fPoisonFrameLength = fPoisonFrameLengthLW;

        fStats.fDamageMinRW.fPoisonPerFrame = fPoisonBitRateRW/256.f;
        fStats.fDamageMinRW.fPoisonFrameLength = fPoisonFrameLengthRW;
        fStats.fDamageMaxRW.fPoisonPerFrame = fPoisonBitRateRW/256.f;
        fStats.fDamageMaxRW.fPoisonFrameLength = fPoisonFrameLengthRW;

        fStats.fAttackRatingLW = (fBaseAR + fFlatARLW)*(1.f + fBonusARLW);
        fStats.fAttackRatingRW = (fBaseAR + fFlatARRW)*(1.f + fBonusARRW);
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
    const float healthFrac = fHealthF/fMaxHealth;
    const float manaFrac = fManaF/fMaxMana;

    const auto& leftW = (eq.fWeapons1 ?
                             eq.fWeapon1L :
                             eq.fWeapon2L);
    const auto& rightW = (eq.fWeapons1 ?
                              eq.fWeapon1R :
                              eq.fWeapon2R);

    fWeaponMeeleRange = 0.f;
    int meeleRangeDiv = 0;
    fWeaponRangedRange = 10000.f;
    const auto handleWeapon = [&](const eItem& w) {
        const auto subtype = static_cast<eWeaponType>(w.fSubType);
        const bool r = gWeaponIsRanged(subtype);
        const auto& itemData = eItemsData::get(w.fDataId);
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

    fWeaponTypeL = gWeaponType(leftW);
    if(leftW.fType == eItemType::weapon) {
        handleWeapon(leftW);
    }

    fWeaponTypeR = gWeaponType(rightW);
    if(rightW.fType == eItemType::weapon) {
        handleWeapon(rightW);
    }

    if(meeleRangeDiv > 0) {
        fWeaponMeeleRange /= meeleRangeDiv;
    }

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

    fStrength = attr.fStrength;
    fDexterity = attr.fDexterity;
    fVitality = attr.fVitality;
    fEnergy = attr.fEnergy;

    // defense
    float baseDef = 0.f;
    float ed = 0.f;

    // life
    float baseLife = 0.f;
    float bonusLife = 0.f;

    // mana
    float baseMana = 0.f;
    float bonusMana = 0.f;

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

    fMaxFireResistance = 0.75f;
    fMaxColdResistance = 0.75f;
    fMaxLightningResistance = 0.75f;
    fMaxPoisonResistance = 0.75f;

    std::vector<eSkillStatsHelper> skillHelpers;

    for(auto& s : fSkills) {
        s.fAttackSpeedS = 0.f;
        s.fAttackRatingLW = 0.f;
        s.fAttackRatingRW = 0.f;
        s.fMissiles = 0;
        s.fPierceLW = 0.f;
        s.fPierceRW = 0.f;

        skillHelpers.emplace_back(eSkillStatsHelper{s});
    }

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
            helper.addMod(mod, src, lw, rw);
            break;
        case eModifierType::none:

        case eModifierType::defenseValue:
        case eModifierType::defensePercent:

        case eModifierType::blockChancePercent:
        case eModifierType::walkRun:
        case eModifierType::castRate:
        case eModifierType::attackSpeed:
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

        case eModifierType::maxFireResistance:
        case eModifierType::maxColdResistance:
        case eModifierType::maxLightningResitance:
        case eModifierType::maxPoisonResistance:

        case eModifierType::strength:
        case eModifierType::dexterity:
        case eModifierType::vitality:
        case eModifierType::energy:
            break;
        }
    };

    const float minFistDmg = 1.f;
    const float maxFistDmg = 6.f;

    const float minFootDmg = 1.f;
    const float maxFootDmg = 6.f;

    for(auto& h : skillHelpers) {
        auto& stats = h.fStats;
        const int skillId = stats.fSkillId;
        const auto& skill = eSkills::sSkills.get(skillId);
        const int skillLevelId = fSkillLevels[skillId];
        const auto& skillLevel = skill.fLevels[skillLevelId];
        if(skill.fType == eSkillType::attack) {
            if(leftW.fType == eItemType::weapon) {
                float min;
                float max;
                gCalculateWeaponDmg(leftW, min, max);
                h.fDmgMinLWBase.fPhysical += min;
                h.fDmgMaxLWBase.fPhysical += max;
            } else if(leftW.fType == eItemType::none) {
                h.fDmgMinLWBase.fPhysical += minFistDmg;
                h.fDmgMaxLWBase.fPhysical += maxFistDmg;
            }
            if(rightW.fType == eItemType::weapon) {
                float min;
                float max;
                gCalculateWeaponDmg(rightW, min, max);
                h.fDmgMinRWBase.fPhysical += min;
                h.fDmgMinRWBase.fPhysical += max;
            } else if(leftW.fType == eItemType::none) {
                h.fDmgMinRWBase.fPhysical += minFistDmg;
                h.fDmgMaxRWBase.fPhysical += maxFistDmg;
            }
        } else if(skill.fType == eSkillType::smite) {
            if(rightW.fType == eItemType::shield) {
                float min;
                float max;
                gCalculateWeaponDmg(rightW, min, max);
                h.fDmgMinRWBase.fPhysical += min;
                h.fDmgMaxRWBase.fPhysical += max;
            }
        } else if(skill.fType == eSkillType::kick) {
            const auto& boots = eq.fBoots;
            if(boots.fType == eItemType::boots) {
                float min;
                float max;
                gCalculateWeaponDmg(boots, min, max);
                h.fDmgMinRWBase.fPhysical += min;
                h.fDmgMaxRWBase.fPhysical += max;
            } else {
                h.fDmgMinRWBase.fPhysical += minFootDmg;
                h.fDmgMaxRWBase.fPhysical += maxFootDmg;
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
        stats.fMissiles = skillLevel.fMissiles;
        if(skill.fType == eSkillType::attack ||
           skill.fType == eSkillType::shoot ||
           skill.fType == eSkillType::throw_) {
            stats.fMissileRangeTime = fWeaponRangedRange;
        } else {
            stats.fMissileRangeTime = skill.fRangeTime;
        }

        for(const auto& mod : skillLevel.fTotalModifiers) {
            handleSkillMod(mod.second, eModifierSource::skill,
                           h, true, true);
        }
        for(const auto& item : items) {
            switch(item.fType) {
            case eItemType::shield:
                fBlockChance += item.fValue4;
                [[fallthrough]];
            case eItemType::boots:
            case eItemType::gloves:
            case eItemType::helmet:
            case eItemType::armor:
            case eItemType::belt:
                baseDef += item.fValue3;
                [[fallthrough]];
            default: {
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
                                   h, lw, rw);
                }
            } break;
            }
        }
    };

    for(const auto& item : items) {
        switch(item.fType) {
        case eItemType::shield:
            fBlockChance += item.fValue4;
            [[fallthrough]];
        case eItemType::boots:
        case eItemType::gloves:
        case eItemType::helmet:
        case eItemType::armor:
        case eItemType::belt:
            baseDef += item.fValue3;
            [[fallthrough]];
        default: {
            for(const auto& mod : item.fModifiers) {
                switch(mod.fType) {
                case eModifierType::defenseValue:
                    baseDef += mod.fValue1;
                    break;
                case eModifierType::defensePercent:
                    ed += mod.fValue1;
                    break;

                case eModifierType::blockChancePercent:
                    fBlockChance += mod.fValue1;
                    break;
                case eModifierType::walkRun:
                    fWalkRun += mod.fValue1;
                    break;
                case eModifierType::castRate:
                    fCastRate += mod.fValue1;
                    break;
                case eModifierType::attackSpeed: {
                    if(&item != &rightW) {
                        fAttackSpeedLW += mod.fValue1;
                    }
                    if(&item != &leftW) {
                        fAttackSpeedRW += mod.fValue1;
                    }
                } break;
                case eModifierType::blockRecoverySpeed:
                    fFasterBlockRate += mod.fValue1;
                    break;
                case eModifierType::hitRecoverySpeed:
                    fFasterHitRecovery += mod.fValue1;
                    break;

                case eModifierType::lifeValue:
                    baseLife += mod.fValue1;
                    break;
                case eModifierType::lifePercent:
                    bonusLife += mod.fValue1;
                    break;

                case eModifierType::manaValue:
                    baseMana += mod.fValue1;
                    break;
                case eModifierType::manaPercent:
                    bonusMana += mod.fValue1;
                    break;

                case eModifierType::fireResistance:
                    fFireResistance += mod.fValue1;
                    break;
                case eModifierType::coldResistance:
                    fColdResistance += mod.fValue1;
                    break;
                case eModifierType::lightningResitance:
                    fLightningResistance += mod.fValue1;
                    break;
                case eModifierType::poisonResistance:
                    fPoisonResistance += mod.fValue1;
                    break;

                case eModifierType::maxFireResistance:
                    fMaxFireResistance += mod.fValue1;
                    break;
                case eModifierType::maxColdResistance:
                    fMaxColdResistance += mod.fValue1;
                    break;
                case eModifierType::maxLightningResitance:
                    fMaxLightningResistance += mod.fValue1;
                    break;
                case eModifierType::maxPoisonResistance:
                    fMaxPoisonResistance += mod.fValue1;
                    break;

                case eModifierType::strength:
                    fStrength += mod.fValue1;
                    break;
                case eModifierType::dexterity:
                    fDexterity += mod.fValue1;
                    break;
                case eModifierType::vitality:
                    fVitality += mod.fValue1;
                    break;
                case eModifierType::energy:
                    fEnergy += mod.fValue1;
                    break;
                case eModifierType::none:
                case eModifierType::attackRatingValue:
                case eModifierType::attackRatingPercent:
                case eModifierType::damagePercent:
                case eModifierType::damageValue:
                case eModifierType::damageFire:
                case eModifierType::damageCold:
                case eModifierType::damageLightning:
                case eModifierType::damagePoison:
                case eModifierType::pierceChance:
                    break;
                }
            }
        } break;
        }
    }

    baseLife += 3.f*fVitality;
    baseMana += 1.5f*fEnergy;

    fDefense = baseDef*(1.f + ed) + fDexterity/4.f;
    fMaxHealth = baseLife*(1.f + bonusLife);
    fHealthF = healthFrac*fMaxHealth;
    fMaxMana = baseMana*(1.f + bonusMana);
    fManaF = manaFrac*fMaxMana;

    fFireResistance = std::min(fMaxFireResistance, fFireResistance);
    fColdResistance = std::min(fMaxColdResistance, fColdResistance);
    fLightningResistance = std::min(fMaxLightningResistance, fLightningResistance);
    fPoisonResistance = std::min(fMaxPoisonResistance, fPoisonResistance);

    const float baseAR = (fDexterity - 7.f)*5.f + 20.f;
    const float attrMult = 0.01f*(fStrength + fDexterity);
    for(auto& h : skillHelpers) {
        const auto skillType = h.fSkillType;
        if(skillType == eSkillType::attack) {
            h.fDmgMultMin.fPhysical += attrMult;
            h.fDmgMultMax.fPhysical += attrMult;
        } else if(skillType == eSkillType::smite) {
            h.fDmgMultMin.fPhysical += attrMult;
            h.fDmgMultMax.fPhysical += attrMult;
        } else if(skillType == eSkillType::kick) {
            h.fDmgMultMin.fPhysical += attrMult;
            h.fDmgMultMax.fPhysical += attrMult;
        }

        h.fBaseAR += baseAR;
        h.apply();
    }
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
        break;
    case eSkillType::wall:
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
           skillType == eSkillType::wall ||
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
    }else if(skill.fType == eSkillType::missile ||
               skill.fType == eSkillType::wall) {
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
