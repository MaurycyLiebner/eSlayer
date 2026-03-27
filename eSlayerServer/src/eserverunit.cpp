#include "eserverunit.h"

#include "ecomplexaction.h"

#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/echardatainfo.h>

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const eCharData& data)
    : mData(data) {}

float eServerUnit::defense() const {
    if(fAnim == mData.runAnimId()) {
        return 0.f;
    } else {
        return mDefense;
    }
}

float eServerUnit::blockChance() const {
    if(fAnim == mData.runAnimId()) {
        return std::clamp(0.33f*mBlockChance, 0.f, 0.25f);
    } else {
        return std::clamp(mBlockChance, 0.f, 0.75f);
    }
}

float eServerUnit::sHitChance(
    const eServerUnit& hit,
    const eServerUnit& by,
    const eSkillChoice schoice) {
    const float alvl = by.level();
    const float dlvl = hit.level();
    const float ar = schoice == eSkillChoice::right ?
                         by.mAttackRatingR :
                         by.mAttackRatingL;
    const float dr = hit.defense();
    return std::clamp(2.f*alvl/(alvl + dlvl)*ar/(ar + dr), 0.05f, 0.95f);
}

int eServerUnit::attackMissiles(
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    switch(schoice) {
    case eSkillChoice::left:
        return mMissilesL;
    case eSkillChoice::right:
        return mMissilesR;
    }
    return 0;
}

float eServerUnit::pierceChance(
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    switch(schoice) {
    case eSkillChoice::left:
        return mPierceL;
    case eSkillChoice::right:
        return mPierceR;
    }
    return 0.f;
}

bool eServerUnit::getHit(const eHitData& data) {
    if(!mAction) return false;
    return mAction->getHit(data);
}

float eServerUnit::takeDamage(const eDamage& dmg) {
    const float totalDmg = dmg.fPhysical +
                           dmg.fCold +
                           dmg.fFire +
                           dmg.fLightning;
    mHealthF = std::max(0.f, mHealthF - totalDmg);
    fHealth = std::ceil(mHealthF);
    return totalDmg;
}

eDamage eServerUnit::attackDamage(const eSkillChoice schoice,
                                  const eWeaponChoice wchoice) {
    switch(schoice) {
    case eSkillChoice::left: {
        switch(wchoice) {
        case eWeaponChoice::left: {
            return eDamage::sRandom(mDamageMinLWLS, mDamageMaxLWLS);
        } break;
        case eWeaponChoice::right: {
            return eDamage::sRandom(mDamageMinRWLS, mDamageMaxRWLS);
        } break;
        }
    } break;
    case eSkillChoice::right: {
        switch(wchoice) {
        case eWeaponChoice::left: {
            return eDamage::sRandom(mDamageMinLWRS, mDamageMaxLWRS);
        } break;
        case eWeaponChoice::right: {
            return eDamage::sRandom(mDamageMinRWRS, mDamageMaxRWRS);
        } break;
        }
    } break;
    }
    return eDamage();
}

void eServerUnit::increment(const float by) {
    for(auto& it : mCooldowns) {
        it.second = std::max(0.f, it.second - by);
    }
    if(mAction) mAction->increment(by);
    const float tmp = fActionTime;
    fActionTime -= by;
    if(tmp > 0.f) return;
    const bool r = mHandler.increment(by);
    if(r) {
        const auto newPos = mHandler.pos();
        fVel = ePointF::vector(newPos, fPos);
        fAngle = fVel.angle();
        fPos = newPos;
    } else {
        mHandler.stopMoving();
        fVel = eVec2f{0.f, 0.f};
    }
}

int eServerUnit::skillId(const eSkillChoice schoice) const {
    int skillId;
    switch(schoice) {
    case eSkillChoice::left:
        skillId = mSkillL;
        break;
    case eSkillChoice::right:
        skillId = mSkillR;
        break;
    }
    return skillId;
}

int eServerUnit::skillLevel(const int skillId) const {
    const auto it = mSkillLevels.find(skillId);
    if(it == mSkillLevels.end()) return 0;
    return it->second;
}

bool eServerUnit::skillReady(const eSkillChoice schoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    const auto it = mCooldowns.find(skillId);
    if(it == mCooldowns.end()) return true;
    return it->second <= 0.f;
}

void eServerUnit::useSkill(const eSkillChoice schoice) {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int levelId = mSkillLevels[skillId];
    const auto& level = skill.fLevels[levelId];
    mCooldowns[skillId] = level.fCooldown*eRunSettings::sFPS;
}

void eServerUnit::setSkillId(const eSkillChoice schoice,
                             const int skillId) {
    switch(schoice) {
    case eSkillChoice::left:
        mSkillL = skillId;
        break;
    case eSkillChoice::right:
        mSkillR = skillId;
        break;
    }
    recalculateStats();
}

void eServerUnit::setAction(const std::shared_ptr<eComplexAction>& a) {
    mAction = a;
}

void eServerUnit::setChildAction(const std::shared_ptr<eUnitAction>& a) {
    if(mAction) mAction->setChild(a);
}

std::vector<int> eServerUnit::castAnims(const eSkillChoice schoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    if(skillId == -1) return {};
    const auto& uskill = mData.getSkill(skillId);
    return uskill.fCastAnimIds;
}

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

void eServerUnit::recalculateStats() {
    const auto items = {
        mEquipment.fBoots,
        mEquipment.fGloves,
        mEquipment.fHelmet,
        mEquipment.fArmor,
        mEquipment.fBelt,
        mEquipment.fRing,
        mEquipment.fAmulet,
        mEquipment.fWeapons1 ?
            mEquipment.fWeapon1L :
            mEquipment.fWeapon2L,
        mEquipment.fWeapons1 ?
            mEquipment.fWeapon1R :
            mEquipment.fWeapon2R,
    };

    float strength = 10.f;
    float dexterity = 10.f;
    float vitality = 10.f;
    float energy = 10.f;

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

    mBlockChance = 0.f;
    mWalkRun = 0.f;
    mCastRate = 0.f;
    mAttackSpeedLWLS = 0.f;
    mAttackSpeedLWRS = 0.f;
    mAttackSpeedRWLS = 0.f;
    mAttackSpeedRWRS = 0.f;
    mFasterBlockRate = 0.f;
    mFasterHitRecovery = 0.f;
    mMissilesL = 0;
    mMissilesR = 0;
    mPierceL = 0.f;
    mPierceR = 0.f;

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
            mBlockChance += mod.fValue1;
            break;
        case eModifierType::walkRun:
            mWalkRun += mod.fValue1;
            break;
        case eModifierType::castRate:
            mCastRate += mod.fValue1;
            break;
        case eModifierType::attackSpeed:
            if(rs) {
                mAttackSpeedRWRS += mod.fValue1;
                mAttackSpeedLWRS += mod.fValue1;
            }
            if(ls) {
                mAttackSpeedRWLS += mod.fValue1;
                mAttackSpeedLWLS += mod.fValue1;
            }
            break;
        case eModifierType::blockRecoverySpeed:
            mFasterBlockRate += mod.fValue1;
            break;
        case eModifierType::hitRecoverySpeed:
            mFasterHitRecovery += mod.fValue1;
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
                mPierceL += mod.fValue1;
            }
            if(rs) {
                mPierceR += mod.fValue1;
            }
        } break;
        default:
            break;
        }
    };

    if(mSkillL != -1) {
        const auto& skillL = eSkills::sSkills.get(mSkillL);
        const int skillLevelIdL = mSkillLevels[mSkillL];
        const auto& skillLevelL = skillL.fLevels[skillLevelIdL];
        if(skillL.fType == eSkillType::attack) {
            dmgMultMinLS.fPhysical += attrMult;
            dmgMultMaxLS.fPhysical += attrMult;
            const auto& leftW = (mEquipment.fWeapons1 ?
                                    mEquipment.fWeapon1L :
                                    mEquipment.fWeapon2L);
           if(leftW.fType == eItemType::weapon ||
              leftW.fType == eItemType::shield) {
                float min;
                float max;
                gCalculateWeaponDmg(leftW, min, max);
                dmgBaseMinLWLS.fPhysical += min;
                dmgBaseMaxLWLS.fPhysical += max;
                dmgBaseMinLWRS.fPhysical += min;
                dmgBaseMaxLWRS.fPhysical += max;
           }
        }
        mMissilesL = skillLevelL.fMissiles;

        for(const auto& mod : skillLevelL.fTotalModifiers) {
            handleMod(mod.second, true, false);
        }
    }

    if(mSkillR != -1) {
        const auto& skillR = eSkills::sSkills.get(mSkillR);
        const int skillLevelIdR = mSkillLevels[mSkillR];
        const auto& skillLevelR = skillR.fLevels[skillLevelIdR];
        if(skillR.fType == eSkillType::attack) {
            dmgMultMinRS.fPhysical += attrMult;
            dmgMultMaxRS.fPhysical += attrMult;
            const auto& rightW = (mEquipment.fWeapons1 ?
                                     mEquipment.fWeapon1R :
                                     mEquipment.fWeapon2R);
            if(rightW.fType == eItemType::weapon ||
               rightW.fType == eItemType::shield) {
                float min;
                float max;
                gCalculateWeaponDmg(rightW, min, max);
                dmgBaseMinRWLS.fPhysical += min;
                dmgBaseMaxRWLS.fPhysical += max;
                dmgBaseMinRWRS.fPhysical += min;
                dmgBaseMaxRWRS.fPhysical += max;
            }
        }
        mMissilesR = skillLevelR.fMissiles;

        for(const auto& mod : skillLevelR.fTotalModifiers) {
            handleMod(mod.second, false, true);
        }
    }

    for(const auto& item : items) {
        switch(item.fType) {
        case eItemType::shield:
            mBlockChance += item.fValue4;
        case eItemType::boots:
        case eItemType::gloves:
        case eItemType::helmet:
        case eItemType::armor:
        case eItemType::belt:
            baseDef += item.fValue3;
        default: {
            for(const auto& mod : item.fModifiers) {
                if(item.fType != eItemType::weapon ||
                   item.fType != eItemType::shield ||
                   item.fType != eItemType::boots) {
                    if(mod.fType == eModifierType::damagePercent ||
                       mod.fType == eModifierType::damageValue) {
                        continue;
                    }
                }
                if(mod.fType == eModifierType::attackSpeed) {
                    if(&item != (mEquipment.fWeapons1 ?
                                      &mEquipment.fWeapon1L :
                                      &mEquipment.fWeapon2L)) {
                        mAttackSpeedRWRS += mod.fValue1;
                        mAttackSpeedRWLS += mod.fValue1;
                    }
                    if(&item != (mEquipment.fWeapons1 ?
                                      &mEquipment.fWeapon1R :
                                      &mEquipment.fWeapon2R)) {
                        mAttackSpeedLWRS += mod.fValue1;
                        mAttackSpeedLWLS += mod.fValue1;
                    }
                    continue;
                }
                handleMod(mod, true, true);
            }
        } break;
        }
    }

    mDefense = baseDef*(1.f + ed) + dexterity/4.f;
    mAttackRatingL = mAttackRatingR = (baseAR + flatAR)*(1.f + bonusAR);
    mMaxHealth = baseLife*(1.f + bonusLife);
    mMaxMana = baseMana*(1.f + bonusMana);

    mDamageMinLWLS = dmgBaseMinLWLS*dmgMultMinLS;
    mDamageMaxLWLS = dmgBaseMaxLWLS*dmgMultMaxLS;
    mDamageMinRWLS = dmgBaseMinRWLS*dmgMultMinLS;
    mDamageMaxRWLS = dmgBaseMaxRWLS*dmgMultMaxLS;
    mDamageMinLWRS = dmgBaseMinLWRS*dmgMultMinRS;
    mDamageMaxLWRS = dmgBaseMaxLWRS*dmgMultMaxRS;
    mDamageMinRWRS = dmgBaseMinRWRS*dmgMultMinRS;
    mDamageMaxRWRS = dmgBaseMaxRWRS*dmgMultMaxRS;
}
