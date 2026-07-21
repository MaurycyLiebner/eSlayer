#include "eSlayerHelpers/eunitsinfo.h"

#include "eSlayerHelpers/echardatainfo.h"
#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eitemsdata.h"

bool eUnitsInfo::sLoaded = false;
eStringIdMapVector<eUnitInfo>
eUnitsInfo::sUnits;

void eUnitsInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Units";

    std::vector<std::string> units;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "units.json");
        units = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/units.json");
    }

    sUnits.reserve(units.size());
    for(const auto& name : units) {
        try {
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");

            eUnitInfo u;
            u.fRadius = jdata.value("radius", 0.4f);
            u.fLevel = jdata.value("level", 1);
            u.fWalkSpeed = jdata.value("walkSpeed", 0.1f);
            u.fRunSpeed = jdata.value("runSpeed", u.fWalkSpeed);
            u.fLighting = jdata.value("lighting", 0.f);
            u.fDifficultyPenalties = jdata.value("difficultyPenalties", false);
            const auto textures = jdata.value("textures", "none");
            u.fCharData = eCharDataInfo::id(textures);
            if(jdata.contains("modifiers")) {
                const auto& mods = jdata["modifiers"];
                for(const auto& [name, modData] : mods.items()) {
                    auto& mod = u.fModifiers.emplace_back();
                    mod.read(name, json(modData));
                }
            }
            if(jdata.contains("skills")) {
                const auto& skills = jdata["skills"];
                for(const auto& [name, skillLevel] : skills.items()) {
                    const int skillId = eSkills::sSkills.id(name);
                    if(skillId < 0) {
                        eRuntimeThrow("Unrecognized skill type \"" + name + "\".");
                    }
                    u.fSkills[skillId] = skillLevel;
                }
            }
            if(jdata.contains("items")) {
                const auto& items = jdata.value("items", std::vector<std::string>());
                for(const auto& itemName : items) {
                    const auto id = eItemsData::id(itemName);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized item type \"" + itemName + "\".");
                    }
                    u.fItems.emplace_back(id);
                }
            }
            u.fColor = eColor{1.f, 1.f, 1.f, 1.f};
            if(jdata.contains("color")) {
                const auto& color = jdata["color"];
                u.fColor.fR = color.value("r", 1.f);
                u.fColor.fG = color.value("g", 1.f);
                u.fColor.fB = color.value("b", 1.f);
                u.fColor.fA = color.value("a", 1.f);
            }

            u.fFleeChance = jdata.value("fleeChance", 0.f);
            u.fFleeDistance = jdata.value("fleeDist", 0.f);

            u.fTanChance = jdata.value("tanChance", 0.f);
            u.fTanDistance = jdata.value("tanDist", 0.f);

            u.fSkillPointsPerLevel = jdata.value("skillPointsPerLevel", 1);
            u.fStatPointsPerLevel = jdata.value("statPointsPerLevel", 5);

            u.fMaxLevel = jdata.value("maxLevel", 99);
            if(jdata.contains("levels")) {
                const auto& jlevels = jdata["levels"];
                auto& exps = u.fLevelExperience;
                exps.resize(u.fMaxLevel - 1);
                for(int i = 1; i < u.fMaxLevel; i++) {
                    const auto iStr = std::to_string(i);
                    exps[i - 1] = jlevels.value(iStr, i == 1 ? 0 : exps[i - 2]);
                }
            }

            sUnits.add(name, u);
        } catch(...) {
            eRuntimeThrow("Failed to parse \"" + dir + "/" + name + ".json\"");
        }
    }

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fUnitId = sUnits.id(skill.fUnitStr);
        if(skill.fType == eSkillType::summon) {
            if(skill.fUnitId < 0) {
                eRuntimeThrow("Unrecognized summon character \"" +
                              skill.fUnitStr + "\".");
            }
        }
    }
}
