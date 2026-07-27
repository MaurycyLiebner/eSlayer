#include "eelitemodifiers.h"

#include "eserverunit.h"

#include <eSlayerHelpers/eelitemodifiersinfo.h>

void eEliteModifiers::initialize(
    const std::set<uint8_t> mods, const int level) {
    const auto& e = eEliteModifiersInfo::sElite;
    for(const auto mod : mods) {
        mMods.emplace(mod);

        const auto& einfo = e.get(mod);

        const auto& bossL = einfo.fBoss.skillLevel(level);
        mBossMods.addBoost(bossL.fTotalModifiers);

        const auto& minionL = einfo.fMinions.skillLevel(level);
        mMinionMods.addBoost(minionL.fTotalModifiers);
    }
}

void eEliteModifiers::initialize(const int nMods, const int level) {
    std::vector<int> options;

    const auto& e = eEliteModifiersInfo::sElite;
    options.reserve(e.size() - 1);
    for(int i = 1; i < e.size(); i++) {
        options.emplace_back(i);
    }

    eRand::randomShuffle(options);

    std::set<uint8_t> mods;
    for(int i = 0; i < nMods; i++) {
        if(i >= options.size()) break;
        mods.emplace(options[i]);
    }

    initialize(mods, level);
}

void eEliteModifiers::setBoss(const bool boss) {
    mBoss = boss;
}

void eEliteModifiers::apply(eServerUnit& u) {
    if(mBoss) {
        for(const auto m : mMods) {
            u.fMods.emplace(m);
        }
        for(const auto& it : mBossMods) {
            const auto& mod = it.second;
            u.addBoost({mod}, eBoostCurseType::permanent, false);
        }
        mBoss = false;
    } else {
        u.fMods.emplace(0);
        for(const auto& it : mMinionMods) {
            const auto& mod = it.second;
            u.addBoost({mod}, eBoostCurseType::permanent, false);
        }
    }
}
