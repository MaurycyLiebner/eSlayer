#include "eSlayerHelpers/estats.h"

#include "eSlayerHelpers/eattributes.h"
#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eskills.h"

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

bool gWeaponIsRanged(const eWeaponSubtype subtype) {
    switch(subtype) {
    case eWeaponSubtype::sword:
    case eWeaponSubtype::longSword:
    case eWeaponSubtype::pike:
        return false;
    case eWeaponSubtype::bow:
    case eWeaponSubtype::throwingAxe:
        return true;
    }
    return false;
}

bool gWeaponIsMeele(const eWeaponSubtype subtype) {
    switch(subtype) {
    case eWeaponSubtype::sword:
    case eWeaponSubtype::longSword:
    case eWeaponSubtype::pike:
    case eWeaponSubtype::throwingAxe:
        return true;
    case eWeaponSubtype::bow:
        return false;
    }
    return false;
}

float gWeaponRangedRange(const eWeaponSubtype subtype) {
    switch(subtype) {
    case eWeaponSubtype::sword:
        return 0.f;
    case eWeaponSubtype::longSword:
        return 0.f;
    case eWeaponSubtype::pike:
        return 0.f;
    case eWeaponSubtype::bow:
        return 8.f;
    case eWeaponSubtype::throwingAxe:
        return 4.f;
    }
    return 0.f;
}

float gWeaponMeeleRange(const eWeaponSubtype subtype) {
    switch(subtype) {
    case eWeaponSubtype::sword:
        return 0.f;
    case eWeaponSubtype::longSword:
        return 0.2f;
    case eWeaponSubtype::pike:
        return 0.4f;
    case eWeaponSubtype::bow:
        return 0.f;
    case eWeaponSubtype::throwingAxe:
        return 0.f;
    }
    return 0.f;
}

void eStats::calculate(const eAttributes& attr,
                       const eEquipment& eq) {
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
        const auto subtype = static_cast<eWeaponSubtype>(w.fSubType);
        const bool r = gWeaponIsRanged(subtype);
        if(r) {
            const float range = gWeaponRangedRange(subtype);
            fWeaponRangedRange = std::min(range, fWeaponRangedRange);
        }
        const bool m = gWeaponIsMeele(subtype);
        if(m) {
            meeleRangeDiv++;
            const float range = gWeaponMeeleRange(subtype);
            fWeaponMeeleRange += range;
        }
    };
    if(leftW.fType == eItemType::weapon) {
        handleWeapon(leftW);
    }
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

    float strength = attr.fStrength;
    float dexterity = attr.fDexterity;
    float vitality = attr.fVitality;
    float energy = attr.fEnergy;

           // defense
    float baseDef = 0.f;
    float ed = 0.f;

           // attack rating
    float baseAR = (dexterity - 7.f)*5.f + 20.f;
    float flatAR = 0.f;
    float bonusAR = 0.f;

           // life
    float baseLife = 3.f*vitality;
    float bonusLife = 0.f;

           // mana
    float baseMana = 1.5f*energy;
    float bonusMana = 0.f;

    fBlockChance = 0.f;
    fWalkRun = 0.f;
    fCastRate = 0.f;
    fAttackSpeedLW = 0.f;
    fAttackSpeedRW = 0.f;
    fAttackSpeedLS = 0.f;
    fAttackSpeedRS = 0.f;
    fFasterBlockRate = 0.f;
    fFasterHitRecovery = 0.f;
    fMissilesL = 0;
    fMissilesR = 0;
    fPierceL = 0.f;
    fPierceR = 0.f;

    eDamage dmgBaseMinLWLS;
    eDamage dmgBaseMaxLWLS;
    eDamage dmgBaseMinLWRS;
    eDamage dmgBaseMaxLWRS;

    eDamage dmgBaseMinRWLS;
    eDamage dmgBaseMaxRWLS;
    eDamage dmgBaseMinRWRS;
    eDamage dmgBaseMaxRWRS;

    const float attrMult = 0.01f*(strength + dexterity);

    eDamage dmgMultMinLS{1.f, 1.f, 1.f, 1.f};
    eDamage dmgMultMaxLS{1.f, 1.f, 1.f, 1.f};
    eDamage dmgMultMinRS{1.f, 1.f, 1.f, 1.f};
    eDamage dmgMultMaxRS{1.f, 1.f, 1.f, 1.f};

    const auto handleMod = [&](const eModifier& mod,
                               const bool skill,
                               const bool ls, const bool rs) {
        switch(mod.fType) {
        case eModifierType::defenseValue:
            baseDef += mod.fValue1;
            break;
        case eModifierType::defensePercent:
            ed += mod.fValue1;
            break;

        case eModifierType::attackRatingValue:
            flatAR += mod.fValue1;
        case eModifierType::attackRatingPercent:
            bonusAR += mod.fValue1;
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
        case eModifierType::attackSpeed:
            if(skill) {
                if(rs) {
                    fAttackSpeedRS += mod.fValue1;
                }
                if(ls) {
                    fAttackSpeedLS += mod.fValue1;
                }
            } else {
                fAttackSpeedRW += mod.fValue1;
                fAttackSpeedLW += mod.fValue1;
            }
            break;
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

        case eModifierType::damagePercent: {
            if(ls) {
                dmgMultMinLS.fPhysical += mod.fValue1;
                dmgMultMaxLS.fPhysical += mod.fValue1;
            }
            if(rs) {
                dmgMultMinRS.fPhysical += mod.fValue1;
                dmgMultMaxRS.fPhysical += mod.fValue1;
            }
        } break;
        case eModifierType::damageValue: {
            if(ls) {
                dmgBaseMinLWLS.fPhysical += mod.fValue1;
                dmgBaseMinRWLS.fPhysical += mod.fValue1;
                dmgBaseMaxLWLS.fPhysical += mod.fValue1;
                dmgBaseMaxRWLS.fPhysical += mod.fValue1;
            }
            if(rs) {
                dmgBaseMinLWRS.fPhysical += mod.fValue1;
                dmgBaseMinRWRS.fPhysical += mod.fValue1;
                dmgBaseMaxLWRS.fPhysical += mod.fValue1;
                dmgBaseMaxRWRS.fPhysical += mod.fValue1;
            }
        } break;
        case eModifierType::damageFire: {
            if(ls) {
                dmgBaseMinLWLS.fFire += mod.fValue1;
                dmgBaseMinRWLS.fFire += mod.fValue1;
                dmgBaseMaxLWLS.fFire += mod.fValue1;
                dmgBaseMaxRWLS.fFire += mod.fValue1;
            }
            if(rs) {
                dmgBaseMinLWRS.fFire += mod.fValue1;
                dmgBaseMinRWRS.fFire += mod.fValue1;
                dmgBaseMaxLWRS.fFire += mod.fValue1;
                dmgBaseMaxRWRS.fFire += mod.fValue1;
            }
        } break;
        case eModifierType::damageCold: {
            if(ls) {
                dmgBaseMinLWLS.fCold += mod.fValue1;
                dmgBaseMinRWLS.fCold += mod.fValue1;
                dmgBaseMaxLWLS.fCold += mod.fValue1;
                dmgBaseMaxRWLS.fCold += mod.fValue1;
            }
            if(rs) {
                dmgBaseMinLWRS.fCold += mod.fValue1;
                dmgBaseMinRWRS.fCold += mod.fValue1;
                dmgBaseMaxLWRS.fCold += mod.fValue1;
                dmgBaseMaxRWRS.fCold += mod.fValue1;
            }
        } break;
        case eModifierType::damageLightning: {
            if(ls) {
                dmgBaseMinLWLS.fLightning += mod.fValue1;
                dmgBaseMinRWLS.fLightning += mod.fValue1;
                dmgBaseMaxLWLS.fLightning += mod.fValue1;
                dmgBaseMaxRWLS.fLightning += mod.fValue1;
            }
            if(rs) {
                dmgBaseMinLWRS.fLightning += mod.fValue1;
                dmgBaseMinRWRS.fLightning += mod.fValue1;
                dmgBaseMaxLWRS.fLightning += mod.fValue1;
                dmgBaseMaxRWRS.fLightning += mod.fValue1;
            }
        } break;
        case eModifierType::pierceChance: {
            if(ls) {
                fPierceL += mod.fValue1;
            }
            if(rs) {
                fPierceR += mod.fValue1;
            }
        } break;
        default:
            break;
        }
    };

    const float minFistDmg = 1.f;
    const float maxFistDmg = 6.f;

    const auto handleSkill = [&](const int skillId,
                                 eDamage& dmgMultMin,
                                 eDamage& dmgMultMax,
                                 eDamage& dmgBaseMinLW,
                                 eDamage& dmgBaseMaxLW,
                                 eDamage& dmgBaseMinRW,
                                 eDamage& dmgBaseMaxRW,
                                 int& missiles,
                                 const bool ls,
                                 const bool rs) {
        if(skillId == -1) return;
        const auto& skillL = eSkills::sSkills.get(skillId);
        const int skillLevelIdL = fSkillLevels[skillId];
        const auto& skillLevelL = skillL.fLevels[skillLevelIdL];
        if(skillL.fType == eSkillType::attack) {
            dmgMultMin.fPhysical += attrMult;
            dmgMultMax.fPhysical += attrMult;
            if(leftW.fType == eItemType::weapon) {
                float min;
                float max;
                gCalculateWeaponDmg(leftW, min, max);
                dmgBaseMinLW.fPhysical += min;
                dmgBaseMaxLW.fPhysical += max;
            } else if(leftW.fType == eItemType::none) {
                dmgBaseMinLW.fPhysical += minFistDmg;
                dmgBaseMaxLW.fPhysical += maxFistDmg;
            }
            if(rightW.fType == eItemType::weapon) {
                float min;
                float max;
                gCalculateWeaponDmg(rightW, min, max);
                dmgBaseMinRW.fPhysical += min;
                dmgBaseMaxRW.fPhysical += max;
            } else if(leftW.fType == eItemType::none) {
                dmgBaseMinRW.fPhysical += minFistDmg;
                dmgBaseMaxRW.fPhysical += maxFistDmg;
            }
        } else if(skillL.fType == eSkillType::smite) {
            dmgMultMin.fPhysical += attrMult;
            dmgMultMax.fPhysical += attrMult;
            if(rightW.fType == eItemType::shield) {
                float min;
                float max;
                gCalculateWeaponDmg(rightW, min, max);
                dmgBaseMinRW.fPhysical += min;
                dmgBaseMaxRW.fPhysical += max;
            }
        } else if(skillL.fType == eSkillType::kick) {
            dmgMultMin.fPhysical += attrMult;
            dmgMultMax.fPhysical += attrMult;
            const auto& boots = eq.fBoots;
            if(boots.fType == eItemType::boots) {
                float min;
                float max;
                gCalculateWeaponDmg(boots, min, max);
                dmgBaseMinRW.fPhysical += min;
                dmgBaseMaxRW.fPhysical += max;
            }
        }
        missiles = skillLevelL.fMissiles;

        for(const auto& mod : skillLevelL.fTotalModifiers) {
            handleMod(mod.second, true, ls, rs);
        }
    };

    handleSkill(fSkillL, dmgMultMinLS, dmgMultMaxLS,
                dmgBaseMinLWLS, dmgBaseMaxLWLS,
                dmgBaseMinRWLS, dmgBaseMaxRWLS,
                fMissilesL, true, false);
    handleSkill(fSkillR, dmgMultMinRS, dmgMultMaxRS,
                dmgBaseMinLWRS, dmgBaseMaxLWRS,
                dmgBaseMinRWRS, dmgBaseMaxRWRS,
                fMissilesR, false, true);

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
                if(mod.fType == eModifierType::attackSpeed) {
                    if(&item != &leftW) {
                        fAttackSpeedRW += mod.fValue1;
                    }
                    if(&item != &rightW) {
                        fAttackSpeedLW += mod.fValue1;
                    }
                    continue;
                }
                handleMod(mod, false, true, true);
            }
        } break;
        }
    }

    fDefense = baseDef*(1.f + ed) + dexterity/4.f;
    fAttackRatingL = fAttackRatingR = (baseAR + flatAR)*(1.f + bonusAR);
    fMaxHealth = baseLife*(1.f + bonusLife);
    fMaxMana = baseMana*(1.f + bonusMana);

    fDamageMinLWLS = dmgBaseMinLWLS*dmgMultMinLS;
    fDamageMaxLWLS = dmgBaseMaxLWLS*dmgMultMaxLS;
    fDamageMinRWLS = dmgBaseMinRWLS*dmgMultMinLS;
    fDamageMaxRWLS = dmgBaseMaxRWLS*dmgMultMaxLS;
    fDamageMinLWRS = dmgBaseMinLWRS*dmgMultMinRS;
    fDamageMaxLWRS = dmgBaseMaxLWRS*dmgMultMaxRS;
    fDamageMinRWRS = dmgBaseMinRWRS*dmgMultMinRS;
    fDamageMaxRWRS = dmgBaseMaxRWRS*dmgMultMaxRS;

}
