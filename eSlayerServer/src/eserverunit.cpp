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
    const int skillId = eServerUnit::skillId(schoice);
    if(skillId == -1) return 0;
    const int levelId = skillLevel(skillId);
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto& level = skill.fLevels[std::clamp(levelId, 0, int(skill.fLevels.size()) - 1)];
    return level.fMissiles;
}

float eServerUnit::pierceChance(
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    const int skillId = eServerUnit::skillId(schoice);
    if(skillId == -1) return 0.f;
    const int levelId = skillLevel(skillId);
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto& level = skill.fLevels[std::clamp(levelId, 0, int(skill.fLevels.size()) - 1)];
    return level.fPierceChance;
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
    mCooldowns[skillId] = skill.fBaseCooldown*eRunSettings::sFPS;
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
        case eItemModifierType::damageMinPercent:
            percentIncMin += mod.fValue1;
            break;
        case eItemModifierType::damageMaxPercent:
            percentIncMax += mod.fValue1;
            break;
        case eItemModifierType::damagePercent:
            percentIncMin += mod.fValue1;
            percentIncMax += mod.fValue1;
            break;
        case eItemModifierType::damageMinValue:
            baseMin += mod.fValue1;
            break;
        case eItemModifierType::damageMaxValue:
            baseMax += mod.fValue1;
            break;
        case eItemModifierType::damageValue:
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

    eDamage dmgBaseMinLWLS;
    eDamage dmgBaseMaxLWLS;
    eDamage dmgBaseMinLWRS;
    eDamage dmgBaseMaxLWRS;

    eDamage dmgBaseMinRWLS;
    eDamage dmgBaseMaxRWLS;
    eDamage dmgBaseMinRWRS;
    eDamage dmgBaseMaxRWRS;

    const float attributeMult = 0.01f*(strength + dexterity);

    eDamage dmgMultMinLS{1.f + attributeMult, 1.f, 1.f, 1.f};
    eDamage dmgMultMaxLS{1.f + attributeMult, 1.f, 1.f, 1.f};
    eDamage dmgMultMinRS{1.f + attributeMult, 1.f, 1.f, 1.f};
    eDamage dmgMultMaxRS{1.f + attributeMult, 1.f, 1.f, 1.f};

    const auto& leftW = (mEquipment.fWeapons1 ?
                            mEquipment.fWeapon1L :
                            mEquipment.fWeapon2L);
    if(leftW.fType == eItemType::weapon) {
        float min;
        float max;
        gCalculateWeaponDmg(leftW, min, max);
        dmgBaseMinLWLS.fPhysical += min;
        dmgBaseMaxLWLS.fPhysical += max;
        dmgBaseMinLWRS.fPhysical += min;
        dmgBaseMaxLWRS.fPhysical += max;
    }
    const auto& rightW = (mEquipment.fWeapons1 ?
                             mEquipment.fWeapon1R :
                             mEquipment.fWeapon2R);
    if(rightW.fType == eItemType::weapon) {
        float min;
        float max;
        gCalculateWeaponDmg(rightW, min, max);
        dmgBaseMinRWLS.fPhysical += min;
        dmgBaseMaxRWLS.fPhysical += max;
        dmgBaseMinRWRS.fPhysical += min;
        dmgBaseMaxRWRS.fPhysical += max;
    }

    for(const auto& item : items) {
        switch(item.fType) {
        case eItemType::shield:
            mBlockChance += item.fValue2;
        case eItemType::boots:
        case eItemType::gloves:
        case eItemType::helmet:
        case eItemType::armor:
        case eItemType::belt:
            baseDef += item.fValue1;
        default: {
            for(const auto& mod : item.fModifiers) {
                switch(mod.fType) {
                case eItemModifierType::defenseValue:
                    baseDef += mod.fValue1;
                    break;
                case eItemModifierType::defensePercent:
                    ed += mod.fValue1;
                    break;

                case eItemModifierType::attackRatingValue:
                    flatAR += mod.fValue1;
                case eItemModifierType::attackRatingPercent:
                    bonusAR += mod.fValue1;
                    break;

                case eItemModifierType::blockChancePercent:
                    mBlockChance += mod.fValue1;
                    break;
                case eItemModifierType::walkRun:
                    mWalkRun += mod.fValue1;
                    break;
                case eItemModifierType::castRate:
                    mCastRate += mod.fValue1;
                    break;
                case eItemModifierType::attackSpeed:
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
                    break;
                case eItemModifierType::blockRecoverySpeed:
                    mFasterBlockRate += mod.fValue1;
                    break;
                case eItemModifierType::hitRecoverySpeed:
                    mFasterHitRecovery += mod.fValue1;
                    break;

                case eItemModifierType::lifeValue:
                    baseLife += mod.fValue1;
                    break;
                case eItemModifierType::lifePercent:
                    bonusLife += mod.fValue1;
                    break;

                case eItemModifierType::manaValue:
                    baseMana += mod.fValue1;
                    break;
                case eItemModifierType::manaPercent:
                    bonusMana += mod.fValue1;
                    break;

                case eItemModifierType::damageMinPercent: {
                    if(item.fType != eItemType::weapon) {
                        dmgMultMinLS.fPhysical += mod.fValue1;
                        dmgMultMinRS.fPhysical += mod.fValue1;
                    }
                } break;
                case eItemModifierType::damageMaxPercent: {
                    if(item.fType != eItemType::weapon) {
                        dmgMultMaxLS.fPhysical += mod.fValue1;
                        dmgMultMaxRS.fPhysical += mod.fValue1;
                    }
                } break;
                case eItemModifierType::damagePercent: {
                    if(item.fType != eItemType::weapon) {
                        dmgMultMinLS.fPhysical += mod.fValue1;
                        dmgMultMinRS.fPhysical += mod.fValue1;
                        dmgMultMaxLS.fPhysical += mod.fValue1;
                        dmgMultMaxRS.fPhysical += mod.fValue1;
                    }
                } break;
                case eItemModifierType::damageMinValue: {
                    if(item.fType != eItemType::weapon) {
                        dmgBaseMinLWLS.fPhysical += mod.fValue1;
                        dmgBaseMinRWLS.fPhysical += mod.fValue1;
                        dmgBaseMinLWRS.fPhysical += mod.fValue1;
                        dmgBaseMinRWRS.fPhysical += mod.fValue1;
                    }
                } break;
                case eItemModifierType::damageMaxValue: {
                    if(item.fType != eItemType::weapon) {
                        dmgBaseMaxLWLS.fPhysical += mod.fValue1;
                        dmgBaseMaxRWLS.fPhysical += mod.fValue1;
                        dmgBaseMaxLWRS.fPhysical += mod.fValue1;
                        dmgBaseMaxRWRS.fPhysical += mod.fValue1;
                    }
                } break;
                case eItemModifierType::damageValue: {
                    if(item.fType != eItemType::weapon) {
                        dmgBaseMinLWLS.fPhysical += mod.fValue1;
                        dmgBaseMinRWLS.fPhysical += mod.fValue1;
                        dmgBaseMinLWRS.fPhysical += mod.fValue1;
                        dmgBaseMinRWRS.fPhysical += mod.fValue1;

                        dmgBaseMaxLWLS.fPhysical += mod.fValue1;
                        dmgBaseMaxRWLS.fPhysical += mod.fValue1;
                        dmgBaseMaxLWRS.fPhysical += mod.fValue1;
                        dmgBaseMaxRWRS.fPhysical += mod.fValue1;
                    }
                } break;
                default:
                    break;
                }
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
