#ifndef EELITEMODIFIERS_H
#define EELITEMODIFIERS_H

#include <eSlayerHelpers/eunitdata.h>

class eServerUnit;

class eEliteModifiers {
public:
    void initialize(const int nMods, const float level);

    void apply(eServerUnit& u, const bool boss);
private:
    std::set<eUnitMod> mMods;
    std::vector<eModifier> mBossMods;
    std::vector<eModifier> mMinionMods;
};

#endif // EELITEMODIFIERS_H
