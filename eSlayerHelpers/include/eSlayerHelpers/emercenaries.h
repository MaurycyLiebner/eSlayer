#ifndef EMERCENARIES_H
#define EMERCENARIES_H

#include "estringidmapvector.h"
#include "eeqoptions.h"
#include "emodscollection.h"

#include <cstdint>

struct eMercenaryInfo {
    uint8_t fUnitType;

    eEqOptions fEq;

    uint16_t fBaseStrength = 20;
    uint16_t fBaseDexterity = 20;
    uint16_t fBaseVitality = 20;
    uint16_t fBaseEnergy = 15;

    float fStrengthPerLevel = 0.f;
    float fDexterityPerLevel = 0.f;
    float fVitalityPerLevel = 0.f;
    float fEnergyPerLevel = 0.f;

    float fLifePerLevel = 0.f;

    float fResistancePerLevel = 0.f;

    float fDamagePerLevel = 0.f;
    float fFireDamagePerLevel = 0.f;
    float fColdDamagePerLevel = 0.f;
    float fLightningDamagePerLevel = 0.f;

    float fDefensePerLevel = 0.f;

    eModsCollectionLevels fMods;
};

class ESLAYERHELPERS_API eMercenariesInfo {
public:
    static eStringIdMapVector<eMercenaryInfo> sMercs;

    static std::vector<eModifier> mods(
        const int mercType,
        const uint8_t level);

    static void load();
private:
    static bool sLoaded;
};

#endif // EMERCENARIES_H
