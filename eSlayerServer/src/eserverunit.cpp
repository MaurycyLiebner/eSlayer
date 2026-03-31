#include "eserverunit.h"

#include "ecomplexaction.h"

#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/echardatainfo.h>

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const eCharData& data)
    : mData(data) {
    mStats.fSkills.emplace_back();
    mStats.fSkills.emplace_back();
}

float eServerUnit::defense() const {
    if(fAnim == mData.runAnimId()) {
        return 0.f;
    } else {
        return mStats.fDefense;
    }
}

float eServerUnit::blockChance() const {
    if(fAnim == mData.runAnimId()) {
        return std::clamp(0.33f*mStats.fBlockChance, 0.f, 0.25f);
    } else {
        return std::clamp(mStats.fBlockChance, 0.f, 0.75f);
    }
}

eWeaponType eServerUnit::weaponType(const eWeaponChoice wchoice) const {
    return wchoice == eWeaponChoice::left ?
               mStats.fWeaponTypeL :
               mStats.fWeaponTypeR;
}

int eServerUnit::missileId(const eWeaponChoice wchoice,
                           const eSkillChoice schoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left: {
        return skill.fMissileIdLW;
    } break;
    case eWeaponChoice::right: {
        return skill.fMissileIdRW;
    } break;
    }
    return -1;
}

float eServerUnit::missileRangeTime(const eWeaponChoice wchoice,
                                    const eSkillChoice schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fMissileRangeTime;
}

void eServerUnit::setEquipment(const eEquipment& eq) {
    mEquipment = eq;
    recalculateStats();
}

float eServerUnit::itemsAttackSpeed(const eWeaponChoice wchoice) const {
    switch(wchoice) {
    case eWeaponChoice::left: {
        return mStats.fAttackSpeedLW;
    } break;
    case eWeaponChoice::right: {
        return mStats.fAttackSpeedRW;
    } break;
    }
    return 0.f;
}

float eServerUnit::skillsAttackSpeed(const eSkillChoice schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fAttackSpeedS;
}

float eServerUnit::weaponSpeedModifier(const eWeaponChoice wchoice) const {
    return 0.f;
}

float eServerUnit::sHitChance(
    const eServerUnit& hit,
    const eServerUnit& by,
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    const float alvl = by.level();
    const float dlvl = hit.level();
    const auto& skill = by.mStats.skill(schoice);
    float ar;
    switch(wchoice) {
    case eWeaponChoice::left:
        ar = skill.fAttackRatingLW;
        break;
    case eWeaponChoice::right:
        ar = skill.fAttackRatingRW;
        break;
    }
    const float dr = hit.defense();
    return std::clamp(2.f*alvl/(alvl + dlvl)*ar/(ar + dr), 0.05f, 0.95f);
}

int eServerUnit::attackMissiles(
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    const auto& skill = mStats.skill(schoice);
    return skill.fMissiles;
}

float eServerUnit::pierceChance(
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fPierceLW;
    case eWeaponChoice::right:
        return skill.fPierceRW;
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
    mStats.fHealthF = std::max(0.f, mStats.fHealthF - totalDmg);
    fHealth = std::ceil(mStats.fHealthF);
    return totalDmg;
}

bool eServerUnit::consumeMana(const float mana) {
    if(mStats.fManaF < mana) return false;
    mStats.fManaF = std::max(0.f, mStats.fManaF - mana);
    return true;
}

eDamage eServerUnit::attackDamage(const eSkillChoice schoice,
                                  const eWeaponChoice wchoice) {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return eDamage::sRandom(skill.fDamageMinLW, skill.fDamageMaxLW);
    case eWeaponChoice::right:
        return eDamage::sRandom(skill.fDamageMinRW, skill.fDamageMaxRW);
    }
    return eDamage();
}

void eServerUnit::increment(const float by) {
    for(auto& it : mStats.fCooldowns) {
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
    const auto& skill = mStats.skill(schoice);
    return skill.fSkillId;
}

int eServerUnit::skillLevel(const int skillId) const {
    const auto it = mStats.fSkillLevels.find(skillId);
    if(it == mStats.fSkillLevels.end()) return 0;
    return it->second;
}

bool eServerUnit::skillReady(const eSkillChoice schoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int levelId = skillLevel(skillId);
    const auto& level = skill.fLevels[levelId];
    if(level.fManaCost > mStats.fManaF) return false;
    const auto it = mStats.fCooldowns.find(skillId);
    if(it == mStats.fCooldowns.end()) return true;
    return it->second <= 0.f;
}

void eServerUnit::useSkill(const eSkillChoice schoice) {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int levelId = skillLevel(skillId);
    const auto& level = skill.fLevels[levelId];
    mStats.fCooldowns[skillId] = level.fCooldown*eRunSettings::sFPS;
    mStats.fManaF = std::max(0.f, mStats.fManaF - level.fManaCost);
}

void eServerUnit::setSkillId(const eSkillChoice schoice,
                             const int skillId) {
    auto& skill = mStats.skill(schoice);
    skill.fSkillId = skillId;
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

bool eServerUnit::canUseSkill(
    const eSkillChoice schoice,
    const eWeaponChoice wchoice) const {
    const auto weapon = wchoice == eWeaponChoice::left ?
        mStats.fWeaponTypeL : mStats.fWeaponTypeR;
    const auto otherWeapon = wchoice == eWeaponChoice::right ?
        mStats.fWeaponTypeL : mStats.fWeaponTypeR;
    const auto& skillStats = mStats.skill(schoice);
    const int skillId = skillStats.fSkillId;
    if(skillId == -1) return false;
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
    }
    return false;
}

void eServerUnit::killed(const eServerUnit& killed) {
    mStats.fExperience += 25.f*std::pow(killed.mAttributes.fLevel, 1.5f);
    const float nextLevel = mAttributes.nextLevelExp();
    if(mStats.fExperience > nextLevel) {
        mStats.fExperience = 0.f;
        mAttributes.fLevel += 1.f;
    }
}

void eServerUnit::respawn() {
    fHealth = fMaxHealth;
    mStats.fHealthF = mStats.fMaxHealth;
    mStats.fManaF = mStats.fMaxMana;
    mAction->setChild(nullptr);
}

eWeaponChoice eServerUnit::useWeapon(const eSkillChoice schoice) {
    const bool canUseL = canUseSkill(schoice, eWeaponChoice::left);
    const bool canUseR = canUseSkill(schoice, eWeaponChoice::right);
    eWeaponChoice use;
    if(!canUseL) {
        use = eWeaponChoice::right;
    } else if(!canUseR) {
        use = eWeaponChoice::left;
    } else {
        use = mLastUsedWeapon == eWeaponChoice::right ?
                  eWeaponChoice::left : eWeaponChoice::right;
    }
    mLastUsedWeapon = use;
    return use;
}

void eServerUnit::recalculateStats() {
    mStats.calculate(mAttributes, mEquipment);
    fMaxHealth = std::ceil(mStats.fMaxHealth);
    fHealth = std::ceil(mStats.fHealthF);
}
