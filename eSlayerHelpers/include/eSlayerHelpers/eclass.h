#ifndef ECLASS_H
#define ECLASS_H

#include <set>

#include "emodifier.h"

struct eIniItem {
    int fItemType;
    std::vector<eModifier> fMods;
};

struct eClass {
    std::set<int> fSkillTrees;
    std::vector<std::vector<eIniItem>> fIniItems;
    uint32_t fIniGold = 0;

    uint8_t fIniStrength = 0;
    uint8_t fIniDexterity = 0;
    uint8_t fIniVitality = 0;
    uint8_t fIniEnergy = 0;

    float fHealthPerVitality = 0;
    float fStaminaPerVitality = 0;
    float fManaPerEnergy = 0;

    float fDefensePerDexterity = 0.f;

    uint8_t fMinFistDamage = 0;
    uint8_t fMaxFistDamage = 0;
    uint8_t fMinFootDamage = 0;
    uint8_t fMaxFootDamage = 0;

    float fBaseAR = 0.f;
    uint8_t fMinARDexterity = 0;
    float fARPerDexterity = 0.f;

    float fStrengthAttackDamageMultiplier = 0.f;
    float fDexterityAttackDamageMultiplier = 0.f;

    float fStrengthSmiteDamageMultiplier = 0.f;
    float fDexteritySmiteDamageMultiplier = 0.f;

    float fStrengthKickDamageMultiplier = 0.f;
    float fDexterityKickDamageMultiplier = 0.f;

    float fStrengthShootDamageMultiplier = 0.f;
    float fDexterityShootDamageMultiplier = 0.f;

    float fStrengthThrowDamageMultiplier = 0.f;
    float fDexterityThrowDamageMultiplier = 0.f;

    bool isClassSkill(const int skillId) const;
};

#endif // ECLASS_H
