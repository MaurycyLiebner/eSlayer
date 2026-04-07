#include "eserverunit.h"

#include "ecomplexaction.h"
#include "edieaction.h"
#include "eserverarea.h"

#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/echardatainfo.h>

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const bool client,
                         const eCharData& data,
                         eServerArea& area) :
    mData(data), mArea(area), mClient(client) {}

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
                           const int schoice) const {
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
                                    const int schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fMissileRangeTime;
}

void eServerUnit::setEquipment(const eEquipment& eq,
                               const bool recalc) {
    mEquipment = eq;
    if(recalc) recalculateStats();
}

void eServerUnit::setAttributes(const eAttributes& attrs,
                                const bool recalc) {
    mAttributes = attrs;
    if(recalc) recalculateStats();
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

float eServerUnit::skillsAttackSpeed(const int schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fAttackSpeedS;
}

float eServerUnit::weaponSpeedModifier(const eWeaponChoice wchoice) const {
    switch(wchoice) {
    case eWeaponChoice::left: {
        return mStats.fWSMLW;
    } break;
    case eWeaponChoice::right: {
        return mStats.fWSMRW;
    } break;
    }
    return 0.f;
}

float eServerUnit::sHitChance(
    const eServerUnit& hit,
    const eServerUnit& by,
    const int schoice,
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

float eServerUnit::sLifeSteal(
    const eServerUnit& hit,
    const eServerUnit& by,
    const int schoice,
    const eWeaponChoice wchoice) {
    const auto& skill = by.mStats.skill(schoice);
    float ls;
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fLifeStealLW;
    case eWeaponChoice::right:
        return skill.fLifeStealRW;
    }
    return 0.f;
}

float eServerUnit::sManaSteal(
    const eServerUnit& hit,
    const eServerUnit& by,
    const int schoice,
    const eWeaponChoice wchoice) {
    const auto& skill = by.mStats.skill(schoice);
    float ls;
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fManaStealLW;
    case eWeaponChoice::right:
        return skill.fManaStealRW;
    }
    return 0.f;
}

int eServerUnit::skillCount(
    const int schoice,
    const eWeaponChoice wchoice) {
    const auto& skill = mStats.skill(schoice);
    return skill.fCount;
}

float eServerUnit::pierceChance(const int schoice,
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
    float totalDmg = 0.f;
    totalDmg += dmg.fPhysical;
    totalDmg += dmg.fFire*(1.f - mStats.fFireResistance);
    totalDmg += dmg.fCold*(1.f - mStats.fColdResistance);
    totalDmg += dmg.fLightning*(1.f - mStats.fLightningResistance);

    const float ppf = dmg.fPoisonPerFrame;
    const float pfl = dmg.fPoisonFrameLength;
    if(ppf > 0.f && pfl > 0.f) {
        mPoison.emplace_back(ePoisonDamage{ppf, pfl});
    }
    mStats.fHealthF = std::max(0.f, mStats.fHealthF - totalDmg);
    fHealth = std::ceil(mStats.fHealthF);
    return totalDmg;
}

void eServerUnit::restoreHealth(const float by) {
    mStats.fHealthF = std::min(mStats.fMaxHealth, mStats.fHealthF + by);
    fHealth = std::ceil(mStats.fHealthF);
}

void eServerUnit::restoreMana(const float by) {
    mStats.fManaF = std::min(mStats.fMaxMana, mStats.fManaF + by);
}

bool eServerUnit::consumeMana(const float mana) {
    if(mStats.fManaF < mana) return false;
    mStats.fManaF = std::max(0.f, mStats.fManaF - mana);
    return true;
}

eDamage eServerUnit::attackDamage(const int schoice,
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
    if(fBlockingActionTime <= 0.f) {
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

    if(fHealth > 0) {
        float poisonDmg = 0.f;
        for(int i = 0; i < mPoison.size(); i++) {
            auto& p = mPoison[i];
            p.fFrameLength -= by;
            poisonDmg = std::max(poisonDmg, p.fPerFrame);
            if(p.fFrameLength <= 0.f) {
                mPoison.erase(mPoison.begin() + i);
                i--;
            }
        }
        mStats.fHealthF = std::max(0.f, mStats.fHealthF - poisonDmg);
        fHealth = std::ceil(mStats.fHealthF);
        if(fHealth <= 0) {
            mArea.unitKilled(*this);
            const auto die = std::make_shared<eDieAction>(*this, mArea);
            setChildAction(die);
        }
    }
}

int eServerUnit::skillId(const eSkillChoice schoice) const {
    return skillId(static_cast<int>(schoice));
}

int eServerUnit::skillId(const int schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fSkillId;
}

int eServerUnit::skillLevel(const int skillId) const {
    const auto it = mStats.fSkillLevels.find(skillId);
    if(it == mStats.fSkillLevels.end()) return 0;
    return it->second;
}

bool eServerUnit::skillReady(const eSkillChoice schoice) const {
    return skillReady(static_cast<int>(schoice));
}

bool eServerUnit::skillReady(const int schoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int levelId = skillLevel(skillId);
    const auto& level = skill.fLevels[levelId];
    if(mClient && level.fManaCost > mStats.fManaF) return false;
    const auto it = mStats.fCooldowns.find(skillId);
    if(it == mStats.fCooldowns.end()) return true;
    return it->second <= 0.f;
}

void eServerUnit::useSkill(const eSkillChoice schoice) {
    useSkill(static_cast<int>(schoice));
}

void eServerUnit::useSkill(const int schoice) {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int levelId = skillLevel(skillId);
    const auto& level = skill.fLevels[levelId];
    const float cooldown = level.fCooldown;
    if(cooldown > 0.f) {
        mStats.fCooldowns[skillId] = cooldown*eRunSettings::sFPS;
    }
    if(mClient) {
        mStats.fManaF = std::max(0.f, mStats.fManaF - level.fManaCost);
    }
}

void eServerUnit::setSkillId(const eSkillChoice schoice,
                             const int skillId,
                             const bool recalc) {
    setSkillId(static_cast<int>(schoice), skillId, recalc);
}

void eServerUnit::setSkillId(const int schoice,
                             const int skillId,
                             const bool recalc) {
    auto& skill = mStats.skill(schoice);
    skill.fSkillId = skillId;
    if(recalc) recalculateStats();
}

void eServerUnit::setAction(const std::shared_ptr<eComplexAction>& a) {
    mAction = a;
}

void eServerUnit::setChildAction(const std::shared_ptr<eUnitAction>& a) {
    if(mAction) mAction->setChild(a);
}

std::vector<int> eServerUnit::castAnims(const int schoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    if(skillId == -1) return {};
    const auto& uskill = mData.getSkill(skillId);
    return uskill.fCastAnimIds;
}

bool eServerUnit::canUseSkill(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto weapon = wchoice == eWeaponChoice::left ?
        mStats.fWeaponTypeL : mStats.fWeaponTypeR;
    const auto otherWeapon = wchoice == eWeaponChoice::right ?
        mStats.fWeaponTypeL : mStats.fWeaponTypeR;
    const auto& skillStats = mStats.skill(schoice);
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
    case eSkillType::summon:
        return true;
    }
    return false;
}

void eServerUnit::killed(const eServerUnit& killed) {
    mAttributes.fExp += 25.f*std::pow(killed.mAttributes.fLevel, 1.5f);
    const auto nextLevel = mAttributes.nextLevelExp();
    if(nextLevel && mAttributes.fExp > nextLevel) {
        mAttributes.fExp = 0.f;
        mAttributes.fLevel++;
    }
}

void eServerUnit::respawn() {
    fHealth = fMaxHealth;
    mStats.fHealthF = mStats.fMaxHealth;
    mStats.fManaF = mStats.fMaxMana;
    mAction->setChild(nullptr);
}

eWeaponChoice eServerUnit::useWeapon(const int schoice) {
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

int eServerUnit::addSkill() {
    mStats.fSkills.emplace_back();
    return mStats.fSkills.size() - 1;
}

std::vector<int> eServerUnit::readySkills() const {
    std::vector<int> result;
    for(int i = 0; i < mStats.fSkills.size(); i++) {
        const bool r = skillReady(i);
        if(r) result.emplace_back(i);
    }
    return result;
}

std::vector<int> eServerUnit::followers(const int charDataId) const {
    std::vector<int> result;
    for(const int charId : mFollowers) {
        const auto u = mArea.unit(charId);
        if(!u) continue;
        if(u->fCharDataId == charDataId) result.emplace_back(charId);
    }
    return result;
}

int eServerUnit::countFollowers(const int charDataId) const {
    int result = 0;
    for(const int charId : mFollowers) {
        const auto u = mArea.unit(charId);
        if(!u) continue;
        if(u->fCharDataId == charDataId) result++;
    }
    return result;
}
