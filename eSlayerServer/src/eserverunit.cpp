#include "eserverunit.h"

#include "actions/ecomplexaction.h"
#include "actions/edieaction.h"
#include "actions/eexplodeaction.h"
#include "eserverarea.h"

#include <eSlayerHelpers/emovementhandlerbase.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eitemsdata.h>

std::atomic<uint32_t> eServerUnit::sNextCharId = 1;

eServerUnit::eServerUnit(const bool slayer,
                         const eCharData& data,
                         const int unitTypeId,
                         eServerArea& area,
                         ePathFinderMap& map) :
    mHandler(*this, map),
    mData(data),
    mArea(area),
    mSlayer(slayer),
    mUnitTypeId(unitTypeId) {}

bool eServerUnit::hitData(
    const eSkillStats& skill,
    const eWeaponChoice wchoice,
    eHitData& data) const {
    data.fAttackerId = fCharId;
    data.fAttackTeamId = fTeamId;
    data.fWChoice = wchoice;

    data.fFrom = fPos;
    data.fKnockback = knockback(skill, wchoice);

    data.fLifeSteal = lifeSteal(skill, wchoice);
    data.fManaSteal = manaSteal(skill, wchoice);

    data.fAlwaysHit = alwaysHit(skill, wchoice);
    data.fAttackRating = attackRating(skill, wchoice);
    data.fALvl = level();

    data.fSplashDmg = meeleSplashDamage(skill, wchoice);
    data.fDamage = attackDamage(skill, wchoice);

    data.fColdLength = coldLength(skill, wchoice);
    data.fFreezeLength = freezeLength(skill, wchoice);

    data.fOnAttack = onAttack(skill, wchoice);
    data.fOnStriking = onStriking(skill, wchoice);
    data.fOnKill = onKill(skill, wchoice);

    data.fBoosts = boosts(skill, wchoice);

    data.fManaBurn = manaBurn(skill, wchoice);

    const float spectral = spectralHit(skill, wchoice);
    if(spectral > 0.f) {
        float& physical = data.fDamage.fPhysical;
        const float conv = physical*spectral;
        physical = std::max(0.f, physical - conv);
        enum eType {
            fire, cold, lightning, poison, count
        };
        const int itype = eRand::rand(0, eType::count - 1);
        const eType type = static_cast<eType>(itype);
        switch(type) {
        case eType::fire:
            data.fDamage.fFire += conv;
            break;
        case eType::cold:
            data.fDamage.fCold += conv;
            data.fColdLength = std::max(data.fColdLength, 50.f);
            break;
        case eType::lightning:
            data.fDamage.fLightning += conv;
            break;
        case eType::poison:
        default:
            const float framesLength = 1.6f*25.f;
            const float bitRate = 256.f*conv/framesLength;
            data.fDamage.fPoisonPerFrame += bitRate/256.f;
            data.fDamage.fPoisonFrameLength += 1.6f*25.f;
            break;
        }
    }

    return true;
}

float eServerUnit::defense() const {
    if(isRunning()) {
        return 0.f;
    } else {
        return mStats.fDefense;
    }
}

float eServerUnit::blockChance() const {
    if(isRunning()) {
        return std::clamp(0.33f*mStats.fBlockChance, 0.f, 0.25f);
    } else {
        return std::clamp(mStats.fBlockChance, 0.f, 0.75f);
    }
}

bool eServerUnit::isRunning() const {
    return fAnim == mData.runAnimId();
}

eWeaponType eServerUnit::weaponType(const eWeaponChoice wchoice) const {
    return wchoice == eWeaponChoice::left ?
               mStats.fWeaponTypeL :
               mStats.fWeaponTypeR;
}

int eServerUnit::missileId(const int schoice,
                           const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return missileId(skill, wchoice);
}

int eServerUnit::missileId(const eSkillStats& stats,
                           const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fMissileIdLW;
    case eWeaponChoice::right:
        return stats.fMissileIdRW;
    }
    return -1;
}

float eServerUnit::missileRange(const int schoice,
                                const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return missileRange(skill, wchoice);
}

float eServerUnit::missileRange(const eSkillStats& stats,
                                const eWeaponChoice wchoice) {
    return stats.fMissileRange;
}

float eServerUnit::missileTime(const int schoice,
                               const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return missileTime(skill, wchoice);
}

float eServerUnit::missileTime(const eSkillStats& stats,
                               const eWeaponChoice wchoice) {
    return stats.fMissileTime;
}

float eServerUnit::radius(const int schoice,
                          const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return radius(skill, wchoice);
}

float eServerUnit::radius(const eSkillStats& stats,
                          const eWeaponChoice wchoice) {
    return stats.fRadius;
}

void eServerUnit::setEquipment(const eEquipment& eq,
                               const bool recalc) {
    mEquipment = eq;
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
}

void eServerUnit::setAttributes(const eAttributes& attrs,
                                const bool recalc) {
    mAttributes = attrs;
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
}

void eServerUnit::setSkillLevels(const eSkillLevels& skillLevels,
                                 const bool recalc) {
    mStats.fBaseSkillLevels = skillLevels;
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
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
    const auto itemType = item.fDataId;
    const auto& info = eItemsData::get(itemType);

    const float instantHealth = info.fPotionInstantHealth +
        info.fPotionInstantHealthFrac*mStats.fMaxHealth;
    const float instantMana = info.fPotionInstantMana +
        info.fPotionInstantManaFrac*mStats.fMaxMana;
    const float instantStamina = info.fPotionInstantStamina +
        info.fPotionInstantStaminaFrac*mStats.fMaxStamina;
    restoreHealth(instantHealth);
    restoreMana(instantMana);
    restoreStamina(instantStamina);
    const float len = info.fPotionFrameLength;
    if(len <= 0.f) return;
    auto& mit = mPotions[info.fSubtype];
    auto& it = mit[itemType];
    it.fFrameLength += len;
    const float totalHealth = info.fPotionTotalHealth +
        info.fPotionTotalHealthFrac*mStats.fMaxHealth;
    it.fHealthPerFrame = totalHealth/len;
    const float totalMana = info.fPotionTotalMana +
        info.fPotionTotalManaFrac*mStats.fMaxMana;
    it.fManaPerFrame = totalMana/len;
    const float totalStamina = info.fPotionTotalStamina +
        info.fPotionTotalStaminaFrac*mStats.fMaxStamina;
    it.fStaminaPerFrame = totalStamina/len;

    const auto& mods = info.fPotionMods;
    if(!mods.empty()) {
        addTimedBoost(mods, info.fPotionBoostType,
                      0, it.fFrameLength);
    }
}

float eServerUnit::itemsAttackSpeed(
    const eWeaponChoice wchoice) const {
    switch(wchoice) {
    case eWeaponChoice::left:
        return mStats.fAttackSpeedLW;
    case eWeaponChoice::right:
        return mStats.fAttackSpeedRW;
    }
    return 0.f;
}

float eServerUnit::skillsAttackSpeed(
    const int schoice) const {
    const auto& skill = mStats.skill(schoice);
    return skill.fAttackSpeedS;
}

float eServerUnit::weaponSpeedModifier(
    const eWeaponChoice wchoice) const {
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
    return knockback(skill, wchoice);
}

bool eServerUnit::knockback(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fKnockbackLW;
    case eWeaponChoice::right:
        return stats.fKnockbackRW;
    }
    return false;
}

bool eServerUnit::alwaysHit(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& stats = mStats.skill(schoice);
    return alwaysHit(stats, wchoice);
}

bool eServerUnit::alwaysHit(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    const int skillId = stats.fSkillId;
    const auto& skill = eSkills::sSkills.get(skillId);
    const auto skillType = skill.fType;
    return skillType == eSkillType::missile ||
           skillType == eSkillType::wall ||
           skillType == eSkillType::boostCurse ||
           skillType == eSkillType::nova;
}

float eServerUnit::attackRating(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return attackRating(skill, wchoice);
}

float eServerUnit::attackRating(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fAttackRatingLW;
    case eWeaponChoice::right:
        return stats.fAttackRatingRW;
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
    return lifeSteal(skill, wchoice);
}

float eServerUnit::lifeSteal(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fLifeStealLW;
    case eWeaponChoice::right:
        return stats.fLifeStealRW;
    }
    return 0.f;
}

float eServerUnit::manaSteal(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return manaSteal(skill, wchoice);
}

float eServerUnit::manaSteal(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fManaStealLW;
    case eWeaponChoice::right:
        return stats.fManaStealRW;
    }
    return 0.f;
}

float eServerUnit::coldLength(
    const int schoice, const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return coldLength(skill, wchoice);
}

float eServerUnit::coldLength(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fColdLengthLW;
    case eWeaponChoice::right:
        return stats.fColdLengthRW;
    }
    return 0.f;
}

float eServerUnit::freezeLength(
    const int schoice, const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return freezeLength(skill, wchoice);
}

float eServerUnit::freezeLength(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fFreezeLengthLW;
    case eWeaponChoice::right:
        return stats.fFreezeLengthRW;
    }
    return 0.f;
}

std::vector<eSkillStats> eServerUnit::onAttack(
    const int schoice, const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return onAttack(skill, wchoice);
}

std::vector<eSkillStats> eServerUnit::onAttack(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fOnAttackLW;
    case eWeaponChoice::right:
        return stats.fOnAttackRW;
    }
    return {};
}

std::vector<eSkillStats> eServerUnit::onStriking(
    const int schoice, const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return onStriking(skill, wchoice);
}

std::vector<eSkillStats> eServerUnit::onStriking(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fOnStrikingLW;
    case eWeaponChoice::right:
        return stats.fOnStrikingRW;
    }
    return {};
}

std::vector<eSkillStats> eServerUnit::onKill(
    const int schoice, const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return onKill(skill, wchoice);
}

std::vector<eSkillStats> eServerUnit::onKill(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fOnKillLW;
    case eWeaponChoice::right:
        return stats.fOnKillRW;
    }
    return {};
}

std::vector<eBoostCurse> eServerUnit::boosts(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) const {
    return stats.fBoostCurse;
}

float eServerUnit::meeleSplashDamage(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return meeleSplashDamage(skill, wchoice);
}

float eServerUnit::meeleSplashDamage(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fMeeleSplashDamageLW;
    case eWeaponChoice::right:
        return stats.fMeeleSplashDamageRW;
    }
    return 0.f;
}

float eServerUnit::manaBurn(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    return stats.fManaBurn;
}

float eServerUnit::spectralHit(
    const int schoice,
    const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return spectralHit(skill, wchoice);
}

float eServerUnit::spectralHit(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    float min = 0.f;
    float max = 0.f;
    switch(wchoice) {
    case eWeaponChoice::left:
        min = stats.fSpectralHitMinLW;
        max = stats.fSpectralHitMaxLW;
        break;
    case eWeaponChoice::right:
        min = stats.fSpectralHitMinRW;
        max = stats.fSpectralHitMaxRW;
        break;
    }
    min = std::min(min, max);
    return eRand::randF(min, max);
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
    return skillCount(skill, wchoice);
}

int eServerUnit::skillCount(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fCountLW;
    case eWeaponChoice::right:
        return stats.fCountRW;
    }
    return -1;
}

float eServerUnit::pierceChance(const int schoice,
                                const eWeaponChoice wchoice) const {
    const auto& skill = mStats.skill(schoice);
    return pierceChance(skill, wchoice);
}

float eServerUnit::pierceChance(const eSkillStats& stats,
                                const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return stats.fPierceLW;
    case eWeaponChoice::right:
        return stats.fPierceRW;
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
    setHealth(std::ceil(mStats.fHealthF));
    return totalDmg;
}

void eServerUnit::restoreHealth(const float by) {
    mStats.fHealthF = std::min(mStats.fMaxHealth, mStats.fHealthF + by);
    setHealth(std::ceil(mStats.fHealthF));
}

void eServerUnit::restoreMana(const float by) {
    mStats.fManaF = std::min(mStats.fMaxMana, mStats.fManaF + by);
}

void eServerUnit::restoreStamina(const float by) {
    mStats.fStaminaF = std::min(mStats.fMaxStamina, mStats.fStaminaF + by);
}

bool eServerUnit::consumeMana(const float mana) {
    if(mStats.fManaF < mana) return false;
    mStats.fManaF = std::max(0.f, mStats.fManaF - mana);
    return true;
}

eDamage eServerUnit::attackDamage(const int schoice,
                                  const eWeaponChoice wchoice) {
    const auto& skill = mStats.skill(schoice);
    return attackDamage(skill, wchoice);
}

eDamage eServerUnit::attackDamage(
    const eSkillStats& stats,
    const eWeaponChoice wchoice) {
    switch(wchoice) {
    case eWeaponChoice::left:
        return eDamage::sRandom(stats.fDamageMinLW, stats.fDamageMaxLW);
    case eWeaponChoice::right:
        return eDamage::sRandom(stats.fDamageMinRW, stats.fDamageMaxRW);
    }
    return eDamage();
}

void eServerUnit::increment(const float by) {
    {
        bool recalc = false;
        if(fHealth <= 0 && !mBoosts.empty()) {
            recalc = true;
            for(const auto& b : mBoosts) {
                removeBoost(b.fType, false);
                removeBoostDataTmp(b.fMissileId);
            }
            mBoosts.clear();
            applyBoostsTmp();
        } else {
            for(int i = 0; i < mBoosts.size(); i++) {
                auto& b = mBoosts[i];
                b.fRemTime -= by;
                if(b.fRemTime <= 0.f) {
                    removeBoost(b.fType, false);
                    removeBoostDataTmp(b.fMissileId);
                    applyBoostsTmp();
                    mBoosts.erase(mBoosts.begin() + i);
                    i--;
                    recalc = true;
                }
            }
        }
        if(recalc) {
            recalculateStats();
            recalculateAuras();
        }
    }

    if(fHealth > 0) {
        mDealsDamageCounter += by;
        if(mDealsDamageCounter >= mDealsDamagePeriod) {
            mDealsDamageCounter -= mDealsDamagePeriod;
            const auto& max = mStats.fDealsDamageMax;
            if(max.total() > 0.f) {
                eHitData data;
                const auto& min = mStats.fDealsDamageMin;
                data.fDamage = eDamage::sRandom(min, max);
                data.fAlwaysHit = true;
                data.fFrom = fPos;
                getHit(data);
            }
        }
    }

    float scaledBy = by;

    if(mFreezeLength > 0.f && fHealth > 0) {
        mFreezeLength = std::max(0.f, mFreezeLength - by);
        if(mSlayer) {
            scaledBy *= eUnitData::sColdSpeed;
            setCold(true);
        } else {
            scaledBy = 0.f;
            setFrozen(true);
        }
    } else {
        mFreezeLength = 0.f;
        setFrozen(false);
    }
    if(mColdLength > 0.f && fHealth > 0) {
        mColdLength = std::max(0.f, mColdLength - by);
        scaledBy *= eUnitData::sColdSpeed;
        setCold(true);
    } else {
        mColdLength = 0.f;
        setCold(false);
    }

    for(auto& it : mStats.fCooldowns) {
        it.second = std::max(0.f, it.second - by);
    }
    if(mAction) mAction->increment(scaledBy);
    if(fBlockingActionTime <= 0.f) {
        const auto oldPos = fPos;
        const bool r = mHandler.increment(scaledBy);
        if(r) {
            const auto newPos = mHandler.pos();
            const auto dir = ePointF::vector(newPos, oldPos);
            setAngle(dir.angle());
            setPosition(newPos);
        } else {
            mHandler.stopMoving();
        }
    }

    bool staminaPotion = false;
    bool poisoned = false;
    if(fHealth > 0) {
        float poisonDmg = 0.f;
        for(int i = 0; i < mPoison.size(); i++) {
            auto& p = mPoison[i];
            p.fFrameLength -= by;
            poisonDmg = std::max(poisonDmg, by*p.fPerFrame);
            if(p.fFrameLength <= 0.f) {
                mPoison.erase(mPoison.begin() + i);
                i--;
            }
        }
        poisonDmg *= 1.f - mStats.fPoisonResistance;
        poisoned = poisonDmg > 0.f;

        float healthReg = by*mStats.fHealthRegeneration;
        float manaReg = by*mStats.fManaRegeneration;
        float staminaReg = isRunning() ? -0.1f : 0.1f;
        if(!mPotions.empty()) {
            float potionHealth = 0.f;
            float potionMana = 0.f;
            float potionStamina = 0.f;
            int maxHealthPotionType = -1;
            int maxManaPotionType = -1;
            int maxStaminaPotionType = -1;
            for(auto& mit : mPotions) {
                auto& mp = mit.second;
                int maxLevel = -1;
                uint8_t maxItemLevelId = 0;
                for(const auto& it : mp) {
                    auto& p = it.second;
                    const auto itemId = it.first;
                    const auto& info = eItemsData::sItems.get(itemId);
                    if(maxLevel < info.fLevelReq) {
                        maxLevel = info.fLevelReq;
                        maxItemLevelId = itemId;
                    }
                }
                if(maxLevel > 0) {
                    auto& p = mp[maxItemLevelId];
                    potionHealth += p.fHealthPerFrame;
                    potionMana += p.fManaPerFrame;
                    potionStamina += p.fStaminaPerFrame;

                    p.fFrameLength -= by;
                    if(p.fFrameLength <= 0.f) {
                        mp.erase(maxItemLevelId);
                    }
                }
            }

            staminaPotion = potionStamina > 0.f;

            healthReg += by*potionHealth;
            manaReg += by*potionMana;
            staminaReg += by*potionStamina;
        }

        if(poisonDmg > 0.f) {
            mPoisonHitCounter += by;
            if(mPoisonHitCounter > 50.f) {
                mPoisonHitCounter = 0.f;
                const auto& onStruck = mStats.fOnStruck;
                const auto wchoice = eWeaponChoice::left;
                for(const auto& o : onStruck) {
                    mArea.castChance(*this, o, wchoice, fPos);
                }
            }
        }

        const float healthChange = healthReg - poisonDmg;
        mStats.fHealthF = std::clamp(mStats.fHealthF + healthChange,
                                     0.f, mStats.fMaxHealth);
        const float manaChange = manaReg;
        mStats.fManaF = std::clamp(mStats.fManaF + manaChange,
                                   0.f, mStats.fMaxMana);

        const float staminaChange = staminaReg;
        mStats.fStaminaF = std::clamp(mStats.fStaminaF + staminaChange,
                                      0.f, mStats.fMaxStamina);

        setHealth(std::ceil(mStats.fHealthF));
        if(fHealth <= 0) dieAndCast(fPos);
    } else {
        mPoison.clear();
    }

    setPoisoned(poisoned);
    setStaminaPotion(staminaPotion);
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
    if(mSlayer && mStats.manaCost(schoice) > mStats.fManaF) return false;
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
        mStats.fCooldowns[skillId] = cooldown*25.f;
    }
    if(mSlayer) {
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
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
}

void eServerUnit::setBoosts(
    const std::vector<eModifier>& mods,
    const bool recalc) {
    auto& b = mStats.fBoosts;
    for(const auto& m : mods) {
        b.emplace(eBoostCurseType::regular, m);
    }
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
}

void eServerUnit::addBoost(
    const std::vector<eModifier>& mods,
    const eBoostCurseType type,
    const bool recalc) {
    auto& b = mStats.fBoosts;
    switch(type) {
    case eBoostCurseType::regular:
        break;
    default: {
        const auto it = b.find(type);
        if(it != b.end()) {
            b.erase(type);
        }
    } break;
    }
    for(const auto& mod : mods) {
        b.emplace(type, mod);
    }
    mArea.boostsAurasChanged(fCharId);
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
}

void eServerUnit::removeBoost(
    const eBoostCurseType type,
    const bool recalc) {
    switch(type) {
    case eBoostCurseType::regular:
        return;
    default: {
        auto& b = mStats.fBoosts;
        b.erase(type);
    } break;
    }
    mArea.boostsAurasChanged(fCharId);
    if(recalc) {
        recalculateStats();
        recalculateAuras();
    }
}

void eServerUnit::addAura(
    const eAura& aura,
    const bool self,
    const bool recalc) {
    mAuraIds.emplace(aura.fId);
    const auto missileId = self ?
        aura.fSelfMissileId : aura.fMissileId;
    if(missileId > 0) {
        mAuras.emplace(missileId);
        addBoostDataTmp(missileId);
    }
    auto& a = mStats.fAuraBoosts;
    const auto type = aura.fType;
    const auto it = a.find(type);
    if(it != a.end()) {
        a.erase(type);
    }
    const auto& mods = aura.fMods;
    for(const auto& mod : mods) {
        a.emplace(type, mod);
    }
    if(recalc) recalculateStats();
}

void eServerUnit::removeAllAuras(
    const bool recalc) {
    for(const auto a : mAuras) {
        removeBoostDataTmp(a);
    }
    mAuras.clear();
    mAuraIds.clear();
    auto& a = mStats.fAuraBoosts;
    if(a.empty()) return;
    a.clear();
    if(recalc) recalculateStats();
}

bool eServerUnit::isAuraSource() const {
    return !mStats.fAuras.empty();
}

float eServerUnit::maxAuraRange() const {
    float result = 0.f;
    for(const auto& a : mStats.fAuras) {
        result = std::max(a.fRange, result);
    }
    return result;
}

bool eServerUnit::addAurasTo(eServerUnit& target) const {
    bool result = false;
    const bool self = &target == this;
    const float dist = ePointF::distance(fPos, target.fPos);
    for(const auto& a : mStats.fAuras) {
        switch(a.fTarget) {
        case eAuraTarget::allies: {
            const bool e = eTeams::areEnemies(fTeamId, target.fTeamId);
            if(e) continue;
        } break;
        case eAuraTarget::enemies: {
            const bool e = eTeams::areEnemies(fTeamId, target.fTeamId);
            if(!e) {
                if(self) {
                    const auto selfMissileId = a.fSelfMissileId;
                    if(selfMissileId > 0) {
                        target.mAuras.emplace(selfMissileId);
                        target.addBoostDataTmp(selfMissileId);
                    }
                }
                continue;
            }
        } break;
        }

        if(a.fRange < dist) continue;
        target.addAura(a, self, false);
        result = true;
    }
    return result;
}

void eServerUnit::addTimedBoost(
    const std::vector<eModifier>& mods,
    const eBoostCurseType type,
    const int missileId,
    const float time,
    const bool recalc) {
    if(type == eBoostCurseType::regular) return;
    addBoost(mods, type, recalc);
    for(int i = 0; i < mBoosts.size(); i++) {
        const auto& b = mBoosts[i];
        if(b.fType == type) {
            removeBoostDataTmp(b.fMissileId);
            mBoosts.erase(mBoosts.begin() + i);
            i--;
        }
    }
    mBoosts.emplace_back(type, missileId, time);
    addBoostDataTmp(missileId);
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
        setHealth(fMaxHealth);
        mStats.fHealthF = mStats.fMaxHealth;
        mStats.fManaF = mStats.fMaxMana;
        mStats.fStaminaF = mStats.fMaxStamina;
    }
}

void eServerUnit::dieAndCast(const ePointF& from) {
    const auto& stats = eServerUnit::stats();
    const auto& onDeath = stats.fOnDeath;
    const auto wchoice = eWeaponChoice::left;
    for(const auto& o : onDeath) {
        mArea.castChance(*this, o, wchoice, from);
    }
    die();
}

void eServerUnit::die(eExplodeType type) {
    if(mDead) return;
    if(!mSlayer && type == eExplodeType::none) {
        const bool f = frozen();
        if(f) type = eExplodeType::ice;
    }

    mDead = true;
    setHealth(0);
    mStats.fHealthF = 0.f;

    for(const auto fId : mFollowers) {
        const auto f = mArea.unit(fId);
        if(f) f->die();
    }
    mFollowers.clear();
    mPoison.clear();
    mPotions.clear();
    mArea.unitKilled(*this);
    if(type == eExplodeType::none) {
        const auto die = std::make_shared<eDieAction>(
            *this, mArea);
        setChildAction(die);
    } else {
        const auto explode = std::make_shared<eExplodeAction>(
            type, *this, mArea);
        setChildAction(explode);
    }
}

void eServerUnit::respawn() {
    setHealth(fMaxHealth);
    mStats.fHealthF = mStats.fMaxHealth;
    mStats.fManaF = mStats.fMaxMana;
    mStats.fStaminaF = mStats.fMaxStamina;
    mAction->setChild(nullptr);
    setBlockingActionTime(0.f);
    updateAll();
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
    setMaxHealth(std::ceil(mStats.fMaxHealth));
    setHealth(std::ceil(mStats.fHealthF));
}

void eServerUnit::recalculateAuras() {
    mStats.calculateAuras(mEquipment);
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

std::vector<uint32_t> eServerUnit::followers(const int unitInfoId) const {
    std::vector<uint32_t> result;
    for(const uint32_t charId : mFollowers) {
        const auto u = mArea.unit(charId);
        if(!u) continue;
        if(u->fUnitInfoId == unitInfoId) result.emplace_back(charId);
    }
    return result;
}

int eServerUnit::countFollowers(const int unitInfoId) const {
    int result = 0;
    for(const uint32_t charId : mFollowers) {
        const auto u = mArea.unit(charId);
        if(!u) continue;
        if(u->fUnitInfoId == unitInfoId) result++;
    }
    return result;
}

bool eServerUnit::moving() const {
    if(mSlayer) return mMoving;
    return mHandler.moving();
}

void eServerUnit::coldFor(const float frameLen) {
    mColdLength = std::max(mColdLength, frameLen);
    if(mColdLength > 0.f) setCold(true);
}

void eServerUnit::freezeFor(const float frameLen) {
    mFreezeLength = std::max(mFreezeLength, frameLen);
    if(mFreezeLength > 0.f) setCold(true);
}

uint16_t eServerUnit::requestUpdate(const uint32_t clientId) {
    const auto it = mUpdateMap.find(clientId);
    uint16_t result;
    if(it == mUpdateMap.end()) {
        result = std::numeric_limits<decltype(result)>::max();
    } else {
        result = it->second;
    }
    mUpdateMap[clientId] = 0;
    return result;
}

void eServerUnit::update(const eShift shift) {
    for(auto& it : mUpdateMap) {
        eUnitData::setUpdate(it.second, shift, true);
    }
}

void eServerUnit::updateAll() {
    for(auto& it : mUpdateMap) {
        it.second = std::numeric_limits<decltype(it.second)>::max();
    }
}

void eServerUnit::setPosition(const ePointF& pos) {
    if(eUnitData::setPosition(pos)) {
        update(eUnitData::eShift::position);
    }
}

void eServerUnit::setAnim(const uint8_t anim) {
    if(eUnitData::setAnim(anim)) {
        update(eUnitData::eShift::anim);
    }
}

void eServerUnit::setAnimId(const eAnimId& animId) {
    if(eUnitData::setAnimId(animId)) {
        update(eUnitData::eShift::animId);
    }
}

void eServerUnit::incAnimId(const int by) {
    if(eUnitData::incAnimId(by)) {
        update(eUnitData::eShift::animId);
    }
}

void eServerUnit::setAnimSpeed(const float animSpeed) {
    if(eUnitData::setAnimSpeed(animSpeed)) {
        update(eUnitData::eShift::animSpeed);
    }
}

void eServerUnit::setBlockingActionTime(const float time) {
    if(eUnitData::setBlockingActionTime(time)) {
        update(eUnitData::eShift::blockingActionTime);
    }
}

void eServerUnit::setAngle(const float angle) {
    if(eUnitData::setAngle(angle)) {
        update(eUnitData::eShift::angle);
    }
}

void eServerUnit::setHealth(const uint16_t health) {
    if(eUnitData::setHealth(health)) {
        update(eUnitData::eShift::health);
    }
}

void eServerUnit::setMaxHealth(const uint16_t maxHealth) {
    if(eUnitData::setMaxHealth(maxHealth)) {
        update(eUnitData::eShift::maxHealth);
    }
}

void eServerUnit::setState(const uint8_t state) {
    if(eUnitData::setState(state)) {
        update(eUnitData::eShift::state);
    }
}

void eServerUnit::setBoosts(const std::set<uint8_t>& boosts) {
    if(eUnitData::setBoosts(boosts)) {
        update(eUnitData::eShift::boosts);
    }
}

void eServerUnit::setTeamId(const eTeamId teamId) {
    if(eUnitData::setTeamId(teamId)) {
        update(eUnitData::eShift::teamId);
    }
}

void eServerUnit::setCold(const bool c) {
    if(eUnitData::setCold(c)) {
        update(eUnitData::eShift::state);
    }
}

void eServerUnit::setFrozen(const bool f) {
    if(eUnitData::setFrozen(f)) {
        update(eUnitData::eShift::state);
    }
}

void eServerUnit::setPoisoned(const bool p) {
    if(eUnitData::setPoisoned(p)) {
        update(eUnitData::eShift::state);
    }
}

void eServerUnit::setStaminaPotion(const bool p) {
    if(eUnitData::setStaminaPotion(p)) {
        update(eUnitData::eShift::state);
    }
}

void eServerUnit::applyBoostsTmp() {
    setBoosts(fBoostsTmp);
}

void eServerUnit::removeBoostDataTmp(const uint8_t id) {
    fBoostsTmp.erase(id);
}

void eServerUnit::addBoostDataTmp(const uint8_t id) {
    fBoostsTmp.emplace(id);
}
