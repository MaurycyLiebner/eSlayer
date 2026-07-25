#ifndef EMODSCOLLECTION_H
#define EMODSCOLLECTION_H

#include "eslayerhelpersexport.h"

#include "emodifier.h"

#include <map>

class ESLAYERHELPERS_API eModsCollection :
    public std::multimap<eModifierType, eModifier> {
public:
    void addBoost(const eModsCollection& other);
    void add(const eModifier& mod);
    void collapse();
    void collapseSkillLevel();

    int fCount = 0;
    int fConsecutive = 0;
    float fManaCost = 0.f;
    float fCooldown = 0.f;
    float fRadius = 0.f;
    float fRadiusRound = 0.f;

    void setRadius(const float r);
private:
    void applyMod(eModifier& mod,
                  const eModifierType multType) const;
};

struct ESLAYERHELPERS_API eModsCollectionLevel {
    eModsCollection fModifiers;
    eModsCollection fTotalModifiers;

    static eModsCollectionLevel parseLevel(
        const ordered_json& levelData,
        eModsCollection& totalMods);
    static void parseLevels(
        const ordered_json& levelsJson,
        std::vector<eModsCollectionLevel>& levels,
        const int maxLevel,
        const bool skipAll1,
        const int count = 0,
        const float cooldown = 0.f,
        const float manaCost = 0.f,
        const float radius = 0.f,
        const int consecutive = 0);
};

struct ESLAYERHELPERS_API eModsCollectionLevels :
    public std::vector<eModsCollectionLevel> {
    const eModsCollectionLevel& skillLevel(const int skillLevelId) const {
        return (*this)[std::clamp(skillLevelId, 0, int(size()) - 1)];
    }
};

#endif // EMODSCOLLECTION_H
