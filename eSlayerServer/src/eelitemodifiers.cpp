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

    {
        auto& bossMod = mBossMods.emplace_back();
        bossMod.fType = eModifierType::lifePercent;
        bossMod.fValue1 = 100;
    }

    for(int i = 0; i < nMods; i++) {
        const int id = eRand::rand() % options.size();
        const auto m = options[id];
        options.erase(options.begin() + id);
        mMods.emplace(m);
        switch(m) {
        case eUnitMod::coldEnchanted: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damageCold;
            minionMod.fValue1 = 1 * level;
            minionMod.fValue2 = 2 * level;

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damageCold;
            bossMod.fValue1 = 2 * level;
            bossMod.fValue2 = 4 * level;
        } break;
        case eUnitMod::extraFast: {
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::walkRun;
                minionMod.fValue1 = 15;
            }

            {
                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::walkRun;
                bossMod.fValue1 = 30;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::attackSpeed;
                minionMod.fValue1 = 25;
            }

            {
                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::attackSpeed;
                bossMod.fValue1 = 45;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::castRate;
                minionMod.fValue1 = 25;
            }

            {
                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::castRate;
                bossMod.fValue1 = 45;
            }
        } break;
        case eUnitMod::extraStrong: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damagePercent;
            minionMod.fValue1 = 50;
            minionMod.fValue2 = 100;

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damagePercent;
            bossMod.fValue1 = 100;
            bossMod.fValue2 = 200;
        } break;
        case eUnitMod::fireEnchanted: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damageFire;
            minionMod.fValue1 = 2 * level;
            minionMod.fValue2 = 4 * level;

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damageFire;
            bossMod.fValue1 = 4 * level;
            bossMod.fValue2 = 8 * level;
        } break;
        case eUnitMod::lightningEnchanted: {
            auto& minionMod = mMinionMods.emplace_back();
            minionMod.fType = eModifierType::damageLightning;
            minionMod.fValue1 = 1;
            minionMod.fValue2 = 8 * level;

            auto& bossMod = mBossMods.emplace_back();
            bossMod.fType = eModifierType::damageLightning;
            bossMod.fValue1 = 1;
            bossMod.fValue2 = 16 * level;
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
                minionMod.fValue1 = 25;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = m;
                bossMod.fValue1 = 50;
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
                minionMod.fValue1 = 5;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = m;
                bossMod.fValue1 = 10;
            }
        } break;
        case eUnitMod::stoneSkin: {
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::defensePercent;
                minionMod.fValue1 = 100;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::defensePercent;
                bossMod.fValue1 = 200;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::physicalResistance;
                minionMod.fValue1 = 25;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::physicalResistance;
                bossMod.fValue1 = 50;
            }
            {
                auto& minionMod = mMinionMods.emplace_back();
                minionMod.fType = eModifierType::maxPhysicalResistance;
                minionMod.fValue1 = 5;

                auto& bossMod = mBossMods.emplace_back();
                bossMod.fType = eModifierType::maxPhysicalResistance;
                bossMod.fValue1 = 10;
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
