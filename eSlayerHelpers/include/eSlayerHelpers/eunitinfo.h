#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "ecolor.h"
#include "emodifier.h"
#include "enpctype.h"

struct eUnitInfo {
    int fCharData;
    float fRadius;
    float fWalkSpeed;
    float fRunSpeed;
    float fLighting;
    int fLevel;
    eColor fColor;

    float fMeeleRange;

    int fMissile;
    float fMissileRange;

    float fFleeChance;
    float fFleeDistance;

    float fTanChance;
    float fTanDistance;

    float fStandChance;
    int fStandLength;

    bool fDifficultyPenalties;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
    std::vector<uint8_t> fItems;

    std::vector<uint32_t> fLevelExperience;
    uint8_t fMaxLevel ;
    uint8_t fSkillPointsPerLevel;
    uint8_t fStatPointsPerLevel;

    eNPCType fNPCType;
    bool fResetSkillStats;

    std::string fClassName;
    int fClassId;

    // for sellers
    std::map<int, std::vector<int>> fItemTypes;
    std::map<int, std::vector<int>> fPotionTypes;
    // for mercenaries
    std::map<int, std::vector<std::string>> fMercTypeStrs;
    std::map<int, std::vector<int>> fMercTypes;
};

#endif // EUNITINFO_H
