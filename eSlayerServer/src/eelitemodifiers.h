#ifndef EELITEMODIFIERS_H
#define EELITEMODIFIERS_H

#include <eSlayerHelpers/eunitdata.h>

class eServerUnit;

class eEliteModifiers {
public:
    void initialize(const int nMods, const int level);

    void apply(eServerUnit& u);
private:
    bool fBoss = true;
    eModsCollection mBossMods;
    eModsCollection mMinionMods;
    std::set<uint8_t> mMods;
};

#endif // EELITEMODIFIERS_H
