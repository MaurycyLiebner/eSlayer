#include "eserverunit.h"

#include "actions/ecomplexaction.h"
#include "actions/edieaction.h"
#include "actions/eexplodeaction.h"
#include "eserverarea.h"

#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/erunsettings.h>
#include <eSlayerHelpers/echardatainfo.h>

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

    return true;
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
                removeBoostData(b.fMissileId);
            }
            mBoosts.clear();
        } else {
            for(int i = 0; i < mBoosts.size(); i++) {
                auto& b = mBoosts[i];
                b.fRemTime -= by;
                if(b.fRemTime <= 0.f) {
                    removeBoost(b.fType, false);
                    removeBoostData(b.fMissileId);
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
            fAngle = dir.angle();
            fPos = newPos;
        } else {
            mHandler.stopMoving();
        }
    }

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
        fHealth = std::ceil(mStats.fHealthF);
        if(fHealth <= 0) dieAndCast(fPos);
    } else {
        mPoison.clear();
    }

    setPoisoned(poisoned);
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
        addBoostData(missileId);
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
        removeBoostData(a);
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
                        target.addBoostData(selfMissileId);
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
            removeBoostData(b.fMissileId);
            mBoosts.erase(mBoosts.begin() + i);
            i--;
        }
    }
    mBoosts.emplace_back(type, missileId, time);
    addBoostData(missileId);
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
