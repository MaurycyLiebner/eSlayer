#ifndef EITEMDATA_H
#define EITEMDATA_H

#include "eslayerhelpersexport.h"

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
    potion
};

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
