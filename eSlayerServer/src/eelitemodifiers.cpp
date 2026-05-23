#include "eelitemodifiers.h"

#include "eserverunit.h"

#include <eSlayerHelpers/eelitemodifiersinfo.h>

void eEliteModifiers::initialize(const int nMods, const int level) {
    std::vector<int> options;

    const auto& e = eEliteModifiersInfo::sElite;
    options.reserve(e.size() - 1);
    for(int i = 1; i < e.size(); i++) {
        options.emplace_back(i);
    }

    for(int i = 0; i < nMods; i++) {
        if(options.empty()) break;
        const int id = eRand::rand() % options.size();
        const int m = options[id];
        mMods.emplace(m);
        options.erase(options.begin() + id);

        const auto& einfo = e.get(m);

        const auto& bossL = einfo.fBoss.skillLevel(level);
        mBossMods = bossL.fTotalModifiers;

        const auto& minionL = einfo.fMinions.skillLevel(level);
        mMinionMods = minionL.fTotalModifiers;
    }
}

void eEliteModifiers::apply(eServerUnit& u, const bool boss) {
    if(boss) {
        for(const auto m : mMods) {
            u.fMods.emplace_back(m);
        }
        for(const auto& it : mBossMods) {
            const auto& mod = it.second;
            u.addBoost({mod}, eBoostCurseType::regular, false);
        }
    } else {
        u.fMods.emplace_back(0);
        for(const auto& it : mMinionMods) {
            const auto& mod = it.second;
            u.addBoost({mod}, eBoostCurseType::regular, false);
        }
    }
}
