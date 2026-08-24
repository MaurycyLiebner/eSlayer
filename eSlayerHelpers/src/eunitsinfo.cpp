#include "eSlayerHelpers/eunitsinfo.h"

#include "eSlayerHelpers/echardatainfo.h"
#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/emissilesinfo.h"
#include "eSlayerHelpers/edifficulties.h"

bool eUnitsInfo::sLoaded = false;
eStringIdMapVector<eUnitInfo>
eUnitsInfo::sUnits;

void eUnitsInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Units";

    const auto parse = [](const ordered_json& jdata, eUnitInfo& u) {
        u.fClassName = jdata.value("class", u.fClassName);
        u.fRadius = jdata.value("radius", u.fRadius);
        u.fLevel = jdata.value("level", u.fLevel);
        u.fWalkSpeed = jdata.value("walkSpeed", u.fWalkSpeed);
        u.fRunSpeed = jdata.value("runSpeed", u.fRunSpeed);
        u.fLighting = jdata.value("lighting", u.fLighting);
        u.fDifficultyPenalties = jdata.value("difficultyPenalties", u.fDifficultyPenalties);

        if(jdata.contains("textures")) {
            const auto textures = jdata.value("textures", "");
            const auto id = eCharDataInfo::id(textures);
            if(id < 0) {
                eRuntimeThrow("Unrecognized unit textures \"" + textures + "\".")
            }
            u.fCharData = id;
        }

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

        const auto missileStr = jdata.value("missile", "none");
        const auto id = eMissilesInfo::sMissiles.id(missileStr);
        if(id < 0) {
            eRuntimeThrow("Unrecognized missile type \"" + missileStr + "\".");
        }
        u.fMissile = id;
        u.fMissileRange = jdata.value("missileRange", 0.f);

        u.fMeeleRange = jdata.value("meeleRange", 0.f);

        u.fColor = eColor{1.f, 1.f, 1.f, 1.f};
        if(jdata.contains("color")) {
            const auto& color = jdata["color"];
            u.fColor.fR = color.value("r", 1.f);
            u.fColor.fG = color.value("g", 1.f);
            u.fColor.fB = color.value("b", 1.f);
            u.fColor.fA = color.value("a", 1.f);
        }

        u.fFleeChance = jdata.value("fleeChance", u.fFleeChance);
        u.fFleeDistance = jdata.value("fleeDist", u.fFleeDistance);

        u.fTanChance = jdata.value("tanChance", u.fTanChance);
        u.fTanDistance = jdata.value("tanDist", u.fTanDistance);

        u.fStandChance = jdata.value("standChance", u.fStandChance);
        u.fStandLength = jdata.value("standLen", u.fStandLength);

        if(jdata.contains("NPCType")) {
            const auto npcTypeStr = jdata.value("NPCType", "none");
            if(npcTypeStr == "healer") {
                u.fNPCType = eNPCType::healer;
            } else if(npcTypeStr == "trader") {
                u.fNPCType = eNPCType::trader;
            } else if(npcTypeStr == "mercenary") {
                u.fNPCType = eNPCType::mercenary;
            } else if(npcTypeStr == "wounded") {
                u.fNPCType = eNPCType::wounded;
            } else if(npcTypeStr == "message") {
                u.fNPCType = eNPCType::message;
            } else if(npcTypeStr == "none") {
                u.fNPCType = eNPCType::none;
            } else {
                eRuntimeThrow("Unrecognized NPC type \"" + npcTypeStr + "\".");
            }
        }

        if(jdata.contains("itemTypes")) {
            const auto& types = jdata["itemTypes"];
            for(const auto& [diffStr, types] : types.items()) {
                const int diffId = eDifficulties::sDifficulties.id(diffStr);
                if(diffId < 0) {
                    eRuntimeThrow("Unrecognized difficulty \"" + diffStr + "\".");
                }
                auto& diffItems = u.fItemTypes[diffId];
                const auto itemTypes = std::vector<std::string>(types);
                for(const auto& str : itemTypes) {
                    const int id = eItemsData::id(str);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized item type \"" + str + "\".");
                    }
                    diffItems.emplace_back(id);
                }
            }
        }

        if(jdata.contains("potionTypes")) {
            const auto& types = jdata["potionTypes"];
            for(const auto& [diffStr, types] : types.items()) {
                const int diffId = eDifficulties::sDifficulties.id(diffStr);
                if(diffId < 0) {
                    eRuntimeThrow("Unrecognized difficulty \"" + diffStr +
                                  "\" in Units/objects.json");
                }
                auto& diffItems = u.fPotionTypes[diffId];
                const auto itemTypes = std::vector<std::string>(types);
                for(const auto& str : itemTypes) {
                    const int id = eItemsData::id(str);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized potion type \"" + str + "\".");
                    }
                    diffItems.emplace_back(id);
                }
            }
        }

        if(jdata.contains("mercTypes")) {
            const auto& types = jdata["mercTypes"];
            for(const auto& [diffStr, types] : types.items()) {
                const int diffId = eDifficulties::sDifficulties.id(diffStr);
                if(diffId < 0) {
                    eRuntimeThrow("Unrecognized difficulty \"" + diffStr + "\".");
                }
                auto& diffMercs = u.fMercTypeStrs[diffId];
                const auto mercTypes = std::vector<std::string>(types);
                for(const auto& str : mercTypes) {
                    diffMercs.emplace_back(str);
                }
            }
        }

        u.fResetSkillStats = jdata.value("resetSkillStats", u.fResetSkillStats);
    };

    std::vector<std::string> templates;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Templates/templates.json");
        templates = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Templates/templates.json");
    }

    std::map<std::string, eUnitInfo> templatesMap;
    for(const auto& name : templates) {
        try {
            const auto jdata = eFileLoaderBase::parse(dir, "Templates/" + name + ".json");
            eUnitInfo u;
            parse(jdata, u);
            templatesMap[name] = u;
        } catch(...) {
            eRuntimeThrow("Failed to parse \"" + dir + "/Templates/" + name + ".json\"");
        }
    }

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
            if(jdata.contains("template")) {
                const auto templateName = jdata.value("template", "");
                const auto it = templatesMap.find(templateName);
                if(it == templatesMap.end()) {
                    eRuntimeThrow("Unrecognized unit template \"" + templateName + "\"");
                }
                u = it->second;
            }
            parse(jdata, u);
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
        for(const auto& unitStr : skill.fUnitStrs) {
            const auto id = sUnits.id(unitStr);
            if(id < 0) {
                eRuntimeThrow("Unrecognized unit \"" + unitStr + "\".");
            }
            skill.fUnits.emplace_back(id);
        }
    }
}
