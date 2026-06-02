#ifndef EELITEMODIFIERS_H
#define EELITEMODIFIERS_H

#include <eSlayerHelpers/eunitdata.h>

class eServerUnit;

class eEliteModifiers {
public:
    void initialize(const int nMods, const int level);

    void apply(eServerUnit& u, const bool boss);
private:
    eModifiersCollection mBossMods;
    eModifiersCollection mMinionMods;
    std::set<uint8_t> mMods;
};

#endif // EELITEMODIFIERS_H
