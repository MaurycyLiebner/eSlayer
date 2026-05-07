#include "eserverunit.h"

#include "actions/ecomplexaction.h"
#include "actions/edieaction.h"
#include "eserverarea.h"

#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/echardatainfo.h>

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const bool client,
                         const eCharData& data,
                         eServerArea& area) :
    mHandler(fPos, fAngle),
    mData(data),
    mArea(area),
    mClient(client) {}

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
    case eWeaponChoice::left:
        return skill.fMissileIdLW;
    case eWeaponChoice::right:
        return skill.fMissileIdRW;
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

void eServerUnit::setSkillLevels(const eSkillLevels& skillLevels,
                                 const bool recalc) {
    mStats.fBaseSkillLevels = skillLevels;
    if(recalc) recalculateStats();
}

void eServerUnit::consumePotion(const uint32_t itemId) {
    const int x = mEquipment.beltX(itemId);
    eItem item;
    if(x != -1) {
        item = mEquipment.takePotion(x);
    } else {
        item = mEquipment.take(itemId);
    }
    if(item.fType != eItemType::potion) return;
    const auto potionType = static_cast<ePotionType>(item.fSubType);
    if(ePotionTypeHelpers::sameCategory(potionType, ePotionType::minorRejuvenation)) {
        float frac = 0.f;
        switch(potionType) {
        case ePotionType::minorRejuvenation:
            frac = 0.25f;
            break;
        case ePotionType::lightRejuvenation:
            frac = 0.5f;
            break;
        case ePotionType::rejuvenation:
            frac = 0.75f;
            break;
        case ePotionType::greaterRejuvenation:
            frac = 1.f;
            break;
        default:
            return;
        }
        restoreHealth(frac*mStats.fMaxHealth);
        restoreMana(frac*mStats.fMaxMana);
        return;
    }
    auto& it = mPotions[potionType];
    float total = 0.f;
    switch(potionType) {
    case ePotionType::minorHealing:
        total = 45.f;
        break;
    case ePotionType::lightHealing:
        total = 90.f;
        break;
    case ePotionType::healing:
        total = 150.f;
        break;
    case ePotionType::greaterHealing:
        total = 270.f;
        break;

    case ePotionType::minorMana:
        total = 30.f;
        break;
    case ePotionType::lightMana:
        total = 60.f;
        break;
    case ePotionType::mana:
        total = 120.f;
        break;
    case ePotionType::greaterMana:
        total = 225.f;
        break;
    default:
        return;
    }

    it.fFrameLength += 128.f;
    it.fPerFrame = total/128.f;
}

float eServerUnit::itemsAttackSpeed(const eWeaponChoice wchoice) const {
    switch(wchoice) {
    case eWeaponChoice::left:
        return mStats.fAttackSpeedLW;
    case eWeaponChoice::right:
        return mStats.fAttackSpeedRW;
    }
    return 0.f;
}

float eServerUnit::skillsAttackSpeed(const int schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fAttackSpeedS;
}

float eServerUnit::weaponSpeedModifier(const eWeaponChoice wchoice) const {
    switch(wchoice) {
    case eWeaponChoice::left:
        return mStats.fWSMLW;
    case eWeaponChoice::right:
        return mStats.fWSMRW;
    }
    return 0.f;
}

bool eServerUnit::knockback(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fKnockbackLW;
    case eWeaponChoice::right:
        return skill.fKnockbackRW;
    }
    return false;
}

bool eServerUnit::alwaysHit(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto skillType = skill.fType;
    return skillType == eSkillType::missile ||
           skillType == eSkillType::wall;
}

float eServerUnit::attackRating(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fAttackRatingLW;
    case eWeaponChoice::right:
        return skill.fAttackRatingRW;
    }
    return 0.f;
}

float eServerUnit::sHitChance(
    const eServerUnit& hit,
    const eServerUnit& by,
    const int schoice,
    const eWeaponChoice wchoice) {
    const float ar = by.attackRating(schoice, wchoice);
    return sHitChance(hit, by, ar);
}

float eServerUnit::sHitChance(const eServerUnit& hit,
                              const eServerUnit& by,
                              const float ar) {
    return sHitChance(hit, by.level(), ar);
}

float eServerUnit::sHitChance(const eServerUnit& hit,
                              const float alvl,
                              const float ar) {
    const float dlvl = hit.level();
    const float dr = hit.defense();
    return std::clamp(2.f*alvl/(alvl + dlvl)*ar/(ar + dr), 0.05f, 0.95f);

}

float eServerUnit::lifeSteal(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fLifeStealLW;
    case eWeaponChoice::right:
        return skill.fLifeStealRW;
    }
    return 0.f;
}

float eServerUnit::manaSteal(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fManaStealLW;
    case eWeaponChoice::right:
        return skill.fManaStealRW;
    }
    return 0.f;
}

float eServerUnit::meeleSplashDamage(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fMeeleSplashDamageLW;
    case eWeaponChoice::right:
        return skill.fMeeleSplashDamageRW;
    }
    return 0.f;
}

std::vector<eModifier>
eServerUnit::skillModifiers(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const int skillId = eServerUnit::skillId(schoice);
    const auto& skill = eSkills::sSkills.get(skillId);
    const int levelId = mStats.effectiveSkillLevel(skillId);
    if(levelId < 0) return {};
    const auto& level = skill.skillLevel(levelId);
    std::vector<eModifier> result;
    const auto& mods = level.fTotalModifiers;
    result.reserve(mods.size());
    for(const auto& mod : mods) {
        result.emplace_back(mod.second);
    }
    for(const auto& s : skill.fSynergies) {
        const int sSkillId = s.fSkillId;
        const int sLevelId = mStats.effectiveSkillLevel(sSkillId);
        if(levelId < 0) continue;
        const auto& sBoost = s.boostLevel(sLevelId);
        const auto& sMods = sBoost.fTotalModifiers;
        result.reserve(result.size() + sMods.size());
        for(const auto& mod : sMods) {
            result.emplace_back(mod.second);
        }
    }
    return result;
}

int eServerUnit::skillCount(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fCount;
}

float eServerUnit::pierceChance(const int schoice,
                                const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    switch(wchoice) {
    case eWeaponChoice::left:
        return skill.fPierceLW;
    case eWeaponChoice::right:
        return skill.fPierceRW;
    }
    return 0.f;
}

bool eServerUnit::getHit(const eHitData& data,
                         const bool splash) {
    if(!mAction) return false;
    return mAction->getHit(data, splash);
}

float eServerUnit::takeDamage(const eDamage& dmg) {
    float totalDmg = 0.f;
    totalDmg += dmg.fPhysical*(1.f - mStats.fPhysicalResistance);
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
        const auto oldPos = fPos;
        const bool r = mHandler.increment(by);
        if(r) {
            const auto newPos = mHandler.pos();
            const auto dir = ePointF::vector(newPos, oldPos);
            fAngle = dir.angle();
            fPos = newPos;
        } else {
            mHandler.stopMoving();
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
        poisonDmg *= 1.f - mStats.fPoisonResistance;

        float healthReg = mStats.fHealthRegeneration;
        float manaReg = mStats.fManaRegeneration;
        if(!mPotions.empty()) {
            for(const auto type : {ePotionType::greaterHealing,
                                   ePotionType::healing,
                                   ePotionType::lightHealing,
                                   ePotionType::minorHealing}) {
                const auto it = mPotions.find(type);
                if(it == mPotions.end()) continue;
                auto& p = it->second;
                if(p.fFrameLength > 0.f) {
                    p.fFrameLength -= by;
                    healthReg += p.fPerFrame;
                    if(p.fFrameLength <= 0.f) {
                        mPotions.erase(type);
                    }
                    break;
                }
            }
            for(const auto type : {ePotionType::greaterMana,
                                   ePotionType::mana,
                                   ePotionType::lightMana,
                                   ePotionType::minorMana}) {
                const auto it = mPotions.find(type);
                if(it == mPotions.end()) continue;
                auto& p = it->second;
                if(p.fFrameLength > 0.f) {
                    p.fFrameLength -= by;
                    manaReg += p.fPerFrame;
                    if(p.fFrameLength <= 0.f) {
                        mPotions.erase(type);
                    }
                    break;
                }
            }
        }
        const float healthChange = healthReg - poisonDmg;
        mStats.fHealthF = std::clamp(mStats.fHealthF + healthChange,
                                     0.f, mStats.fMaxHealth);
        const float manaChange = manaReg;
        mStats.fManaF = std::clamp(mStats.fManaF + manaChange,
                                   0.f, mStats.fMaxMana);
        fHealth = std::ceil(mStats.fHealthF);
        if(fHealth <= 0) {
            die();
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

bool eServerUnit::skillReady(const eSkillChoice schoice) const {
    return skillReady(static_cast<int>(schoice));
}

bool eServerUnit::skillReady(const int schoice) const {
    if(mClient && mStats.manaCost(schoice) > mStats.fManaF) return false;
    const int skillId = eServerUnit::skillId(schoice);
    const auto it = mStats.fCooldowns.find(skillId);
    if(it == mStats.fCooldowns.end()) return true;
    const float wait = it->second;
    return wait <= 0.f;
}

void eServerUnit::useSkill(const eSkillChoice schoice) {
    useSkill(static_cast<int>(schoice));
}

void eServerUnit::useSkill(const int schoice) {
    const int skillId = eServerUnit::skillId(schoice);
    const float cooldown = mStats.cooldown(schoice);
    if(cooldown > 0.f) {
        mStats.fCooldowns[skillId] = cooldown*eRunSettings::sFPS;
    }
    if(mClient) {
        const float manaCost = mStats.manaCost(schoice);
        mStats.fManaF = std::max(0.f, mStats.fManaF - manaCost);
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
    if(recalc) recalculateSkillStats(schoice);
}

void eServerUnit::setBoosts(
    const std::vector<eModifier>& mods,
    const bool recalc) {
    mStats.fBoosts = mods;
    if(recalc) recalculateStats();
}

void eServerUnit::addBoost(
    const eModifier& mod,
    const bool recalc) {
    mStats.fBoosts.emplace_back(mod);
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

void eServerUnit::killed(const eServerUnit& killed) {
    mAttributes.fExp += 25.f*std::pow(killed.mAttributes.fLevel, 1.5f);
    const auto nextLevel = mAttributes.nextLevelExp();
    if(nextLevel && mAttributes.fExp > nextLevel) {
        mAttributes.fExp = 0.f;
        mAttributes.fLevel++;
        fHealth = fMaxHealth;
        mStats.fHealthF = mStats.fMaxHealth;
        mStats.fManaF = mStats.fMaxMana;
    }
}

void eServerUnit::die() {
    fHealth = 0;
    mStats.fHealthF = 0.f;

    for(const auto fId : mFollowers) {
        const auto f = mArea.unit(fId);
        if(f) f->die();
    }
    mFollowers.clear();
    mPoison.clear();
    mPotions.clear();
    mArea.unitKilled(*this);
    const auto die = std::make_shared<eDieAction>(*this, mArea);
    setChildAction(die);
}

void eServerUnit::respawn() {
    fHealth = fMaxHealth;
    mStats.fHealthF = mStats.fMaxHealth;
    mStats.fManaF = mStats.fMaxMana;
    mAction->setChild(nullptr);
    fBlockingActionTime = 0.f;
}

eWeaponChoice eServerUnit::useWeapon(const int schoice) {
    const bool canUseL = mStats.canUseSkill(schoice, eWeaponChoice::left);
    const bool canUseR = mStats.canUseSkill(schoice, eWeaponChoice::right);
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

void eServerUnit::recalculateSkillStats(const int schoice) {
    mStats.calculateSkill(schoice, mEquipment);
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

bool eServerUnit::moving() const {
    if(mClient) return mMoving;
    return mHandler.moving();
}
