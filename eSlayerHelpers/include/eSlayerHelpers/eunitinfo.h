#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "ecolor.h"
#include "emodifier.h"
#include "enpctype.h"

struct eUnitInfo {
    int fCharData;
    float fRadius = 0.4f;
    float fWalkSpeed = 0.1f;
    float fRunSpeed = 0.1f;
    float fLighting = 0.f;
    int fLevel = 1;
    eColor fColor;

    float fMeeleRange = 0.f;

    int fMissile = 0;
    float fMissileRange = 0.f;

    float fFleeChance = 0.f;
    float fFleeDistance = 0.f;

    float fTanChance = 0.f;
    float fTanDistance = 0.f;

    float fStandChance = 0.f;
    int fStandLength = 1;

    bool fDifficultyPenalties = false;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
    std::vector<uint8_t> fItems;

    eNPCType fNPCType = eNPCType::none;
    bool fResetSkillStats = false;

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
