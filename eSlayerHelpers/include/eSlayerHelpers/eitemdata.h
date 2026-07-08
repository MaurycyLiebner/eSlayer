#ifndef EITEMDATA_H
#define EITEMDATA_H

#include "eslayerhelpersexport.h"

#include "emodifier.h"

#include <cstdint>
#include <string>
#include <vector>

enum class eItemType : uint8_t {
    none,
    boots,
    gloves,
    helmet,
    armor,
    belt,
    ring,
    amulet,
    weapon,
    shield,
    arrows,
    bolts,
    potion,
    jewel,
    gold,
    questItem
};

enum class eBoostCurseType : uint8_t;

namespace eItemTypeHelpers {
    ESLAYERHELPERS_API
    eItemType type(const std::string& name);
    ESLAYERHELPERS_API
    std::string name(const eItemType type);
}

struct eItemData {
    eItemType fType = eItemType::none;
    uint8_t fSubtype = 0;
    bool fTwoHanded = false;
    std::vector<eItemType> fSecondHand;
    float fRange = 0.f;
    // Weapon speed modifier
    float fWSM = 0.f;

    float fPotionFrameLength = 0.f;

    float fPotionInstantHealth = 0.f;
    float fPotionInstantMana = 0.f;
    float fPotionInstantStamina = 0.f;

    float fPotionInstantHealthFrac = 0.f;
    float fPotionInstantManaFrac = 0.f;
    float fPotionInstantStaminaFrac = 0.f;

    float fPotionTotalHealth = 0.f;
    float fPotionTotalMana = 0.f;
    float fPotionTotalStamina = 0.f;

    float fPotionTotalHealthFrac = 0.f;
    float fPotionTotalManaFrac = 0.f;
    float fPotionTotalStaminaFrac = 0.f;

    eBoostCurseType fPotionBoostType;
    std::vector<eModifier> fPotionMods;

    int fMinDamageMin = 0;
    int fMinDamageMax = 0;
    int fMaxDamageMin = 0;
    int fMaxDamageMax = 0;
    int fDefenseMin = 0;
    int fDefenseMax = 0;
    int fBlockChanceMin = 0;
    int fBlockChanceMax = 0;

    std::string fMissileStr;
    int fMissileId = -1;
    float fMissileSpeed = 0.f;
    float fMissileRadius = 0.f;

    uint8_t fWidth;
    uint8_t fHeight;

    uint8_t fLevelReq = 0;
    uint8_t fStrengthReq = 0;
    uint8_t fDexterityReq = 0;
};

#endif // EITEMDATA_H
