#ifndef EELITEMODIFIERS_H
#define EELITEMODIFIERS_H

#include <eSlayerHelpers/eunitdata.h>

class eServerUnit;

class eEliteModifiers {
public:
    void initialize(const std::set<uint8_t> mods, const int level);
    void initialize(const int nMods, const int level);

    void setBoss(const bool boss);
    bool boss() const { return mBoss; }

    void apply(eServerUnit& u);
private:
    bool mBoss = true;
    eModsCollection mBossMods;
    eModsCollection mMinionMods;
    std::set<uint8_t> mMods;
};

#endif // EELITEMODIFIERS_H
