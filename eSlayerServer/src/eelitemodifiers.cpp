#include "eelitemodifiers.h"

#include "eserverunit.h"

void eEliteModifiers::initialize(const int nMods, const float level) {
    std::vector<eUnitMod> options = {
        eUnitMod::coldEnchanted,
        eUnitMod::extraFast,
        eUnitMod::extraStrong,
        eUnitMod::fireEnchanted,
        eUnitMod::lightningEnchanted,
        eUnitMod::magicResistant,
        eUnitMod::stoneSkin
    };

    for(int i = 0; i < nMods; i++) {
        const int id = eRand::rand() % options.size();
        const auto m = options[id];
        options.erase(options.begin() + id);
        mMods.emplace(m);
        switch(m) {
        case eUnitMod::coldEnchanted: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damageCold;
            minionMod.fValue1 = 1.f * sqrt(level);
            minionMod.fValue2 = 2.f * sqrt(level);

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damageCold;
            bossMod.fValue1 = 2.f * sqrt(level);
            bossMod.fValue2 = 4.f * sqrt(level);
        } break;
        case eUnitMod::extraFast: {
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::walkRun;
                minionMod.fValue1 = 0.15f;
            }

            {
                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::walkRun;
                bossMod.fValue1 = 0.30f;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::attackSpeed;
                minionMod.fValue1 = 0.25f;
            }

            {
                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::attackSpeed;
                bossMod.fValue1 = 0.45f;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::castRate;
                minionMod.fValue1 = 0.25f;
            }

            {
                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::castRate;
                bossMod.fValue1 = 0.45f;
            }
        } break;
        case eUnitMod::extraStrong: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damagePercent;
            minionMod.fValue1 = 0.5f;
            minionMod.fValue2 = 1.f;

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damagePercent;
            bossMod.fValue1 = 1.f;
            bossMod.fValue2 = 2.f;
        } break;
        case eUnitMod::fireEnchanted: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damageFire;
            minionMod.fValue1 = 2.f * sqrt(level);
            minionMod.fValue2 = 4.f * sqrt(level);

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damageFire;
            bossMod.fValue1 = 4.f * sqrt(level);
            bossMod.fValue2 = 8.f * sqrt(level);
        } break;
        case eUnitMod::lightningEnchanted: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damageLightning;
            minionMod.fValue1 = 1.f;
            minionMod.fValue2 = 8.f * sqrt(level);

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damageLightning;
            bossMod.fValue1 = 1.f;
            bossMod.fValue2 = 16.f * sqrt(level);
        } break;
        case eUnitMod::magicResistant: {
            const auto mods1 = {
                eModifierType::fireResistance,
                eModifierType::coldResistance,
                eModifierType::lightningResitance,
                eModifierType::poisonResistance
            };

            for(const auto m : mods1) {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = m;
                minionMod.fValue1 = 0.25f;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = m;
                bossMod.fValue1 = 0.5f;
            }

            const auto mods2 = {
                eModifierType::maxFireResistance,
                eModifierType::maxColdResistance,
                eModifierType::maxLightningResitance,
                eModifierType::maxPoisonResistance
            };

            for(const auto m : mods2) {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = m;
                minionMod.fValue1 = 0.05f;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = m;
                bossMod.fValue1 = 0.1f;
            }
        } break;
        case eUnitMod::stoneSkin: {
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::lifePercent;
                minionMod.fValue1 = 1.f;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::lifePercent;
                bossMod.fValue1 = 2.f;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::defensePercent;
                minionMod.fValue1 = 1.f;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::defensePercent;
                bossMod.fValue1 = 2.f;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::physicalResistance;
                minionMod.fValue1 = 0.25f;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::physicalResistance;
                bossMod.fValue1 = 0.5f;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::maxPhysicalResistance;
                minionMod.fValue1 = 0.05f;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::maxPhysicalResistance;
                bossMod.fValue1 = 0.1f;
            }
        } break;
        default:
            continue;
        }
    }
}

void eEliteModifiers::apply(eServerUnit& u, const bool boss) {
    if(boss) {
        for(const auto m : mMods) {
            u.fMods.emplace_back(m);
        }
        for(const auto& m : mBossMods) {
            u.addBoost(m, false);
        }
    } else {
        u.fMods.emplace_back(eUnitMod::minion);
        for(const auto& m : mMinionMods) {
            u.addBoost(m, false);
        }
    }
}
