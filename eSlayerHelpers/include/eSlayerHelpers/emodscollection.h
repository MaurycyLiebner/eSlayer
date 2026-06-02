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
    float fManaCost = 0.f;
    float fCooldown = 0.f;
    uint8_t fRadiusU = 0;
    float fRadius = 0.f;

    void setRadiusU(const uint8_t r);
private:
    void applyMod(eModifier& mod,
                  const eModifierType multType) const;
};

#endif // EMODSCOLLECTION_H
