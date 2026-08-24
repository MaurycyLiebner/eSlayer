#include "eSlayerHelpers/eclasses.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eskilltrees.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/eunitsinfo.h"

bool eClasses::sLoaded = false;
eStringIdMapVector<eClass>
eClasses::sClasses;

void eClasses::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Classes";

    std::vector<std::string> classes;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "classes.json");
        classes = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/classes.json");
    }

    sClasses.reserve(classes.size() + 1);

    const std::string zeroClassName = "";
    sClasses.add(zeroClassName, eClass());

    for(const auto& name : classes) {
        sClasses.add(name, eClass());
    }

    for(const auto& it : sClasses) {
        const auto& name = it.fName;
        if(name == zeroClassName) continue;
        try {
            auto& class_ = it.fValue;
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");

            const bool isSlayer = jdata.value("slayerClass", false);
            class_.fSlayerClass = isSlayer;
            if(isSlayer) {
                const auto unitInfoStr = jdata.value("unit", "");
                const int id = eUnitsInfo::sUnits.id(unitInfoStr);
                if(id < 0) {
                    eRuntimeThrow("Unrecognized unit type \"" + unitInfoStr +
                                  "\" for slayer class.");
                }
                class_.fUnitInfoId = id;
            }

            const auto skillTrees = jdata.value("skillTrees", std::vector<std::string>());
            for(const auto& skillTree : skillTrees) {
                const int id = eSkillTrees::sTrees.id(skillTree);
                if(id < 0) {
                    eRuntimeThrow("Unrecognized skill tree \"" + skillTree + "\".");
                }
                class_.fSkillTrees.emplace(id);
            }

            class_.fMaxLevel = jdata.value("maxLevel", 99);
            if(jdata.contains("levels")) {
                const auto& jlevels = jdata["levels"];
                auto& exps = class_.fLevelExperience;
                exps.resize(class_.fMaxLevel - 1);
                for(int i = 1; i < class_.fMaxLevel; i++) {
                    const auto iStr = std::to_string(i);
                    exps[i - 1] = jlevels.value(iStr, i == 1 ? 0 : exps[i - 2]);
                }
            }

            class_.fIniStrength = jdata.value("iniStrength", 20);
            class_.fIniDexterity = jdata.value("iniDexterity", 20);
            class_.fIniVitality = jdata.value("iniVitality", 20);
            class_.fIniEnergy = jdata.value("iniEnergy", 15);

            class_.fSkillPointsPerLevel = jdata.value("skillPointsPerLevel", 1);
            class_.fStatPointsPerLevel = jdata.value("statPointsPerLevel", 5);

            class_.fHealthPerVitality = jdata.value("lifePerVitality", 3.f);
            class_.fStaminaPerVitality = jdata.value("staminaPerVitality", 3.f);
            class_.fManaPerEnergy = jdata.value("manaPerEnergy", 1.5f);

            class_.fDefensePerDexterity = jdata.value("defensePerDexterity", 0.025f);

            class_.fMinFistDamage = jdata.value("minFistDamage", 1);
            class_.fMaxFistDamage = jdata.value("maxFistDamage", 2);
            class_.fMinFootDamage = jdata.value("minFootDamage", 1);
            class_.fMaxFootDamage = jdata.value("maxFootDamage", 2);

            class_.fBaseAR = jdata.value("baseAR", 20.f);
            class_.fMinARDexterity = jdata.value("minARDexterity", 7);
            class_.fARPerDexterity = jdata.value("ARPerDexterity", 5.f);

            class_.fStrengthAttackDamageMultiplier = jdata.value("strengthAttackDamageMultiplier", 0.01f);
            class_.fDexterityAttackDamageMultiplier = jdata.value("dexterityAttackDamageMultiplier", 0.005f);

            class_.fStrengthSmiteDamageMultiplier = jdata.value("strengthSmiteDamageMultiplier", 0.01f);
            class_.fDexteritySmiteDamageMultiplier = jdata.value("dexteritySmiteDamageMultiplier", 0.005f);

            class_.fStrengthKickDamageMultiplier = jdata.value("strengthKickDamageMultiplier", 0.0075f);
            class_.fDexterityKickDamageMultiplier = jdata.value("dexterityKickDamageMultiplier", 0.0075f);

            class_.fStrengthShootDamageMultiplier = jdata.value("strengthShootDamageMultiplier", 0.005f);
            class_.fDexterityShootDamageMultiplier = jdata.value("dexterityShootDamageMultiplier", 0.01f);

            class_.fStrengthThrowDamageMultiplier = jdata.value("strengthThrowDamageMultiplier", 0.0075f);
            class_.fDexterityThrowDamageMultiplier = jdata.value("dexterityThrowDamageMultiplier", 0.0075f);

            if(jdata.contains("iniItems")) {
                for(const auto& option : jdata["iniItems"]) {
                    std::vector<eIniItem> vec;
                    for(const auto& item : option) {
                        const auto typeStr = item.value("type", "");
                        const int id = eItemsData::id(typeStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized item type \"" + typeStr + "\".");
                        }
                        eIniItem iniItem;
                        iniItem.fItemType = id;
                        if(item.contains("modifiers")) {
                            const auto& mods = item["modifiers"];
                            for(const auto& [name, modData] : mods.items()) {
                                auto& mod = iniItem.fMods.emplace_back();
                                mod.read(name, json(modData));
                            }
                        }
                        vec.emplace_back(iniItem);
                    }
                    class_.fIniItems.emplace_back(vec);
                }
            }

            class_.fIniGold = jdata.value("iniGold", 1000);
        } catch(...) {
            eRuntimeThrow("Failed to parse \"" + dir + "/" + name + ".json\"");
        }
    }

    for(const auto& it : eUnitsInfo::sUnits) {
        auto& info = it.fValue;
        info.fClassId = sClasses.id(info.fClassName);
    }
}
