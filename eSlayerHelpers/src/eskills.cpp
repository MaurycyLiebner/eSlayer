#include "eSlayerHelpers/eskills.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/erunsettings.h"

bool eSkills::sLoaded = false;
eStringIdMapVector<eSkill> eSkills::sSkills;

void eSkills::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Skills";

    std::vector<std::string> skills;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "skills.json");
        skills = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/skills.json");
    }

    sSkills.reserve(skills.size());

    for(const auto& name : skills) {
        try {
            const int oldId = sSkills.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate skill '" + name + "' in " + dir + "/skills.json");
                continue;
            }
            std::map<eModifierType, eModifier> modifiers;

            eSkill skill;
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");
            const std::string typeStr = jdata["type"];
            skill.fIcon = jdata["icon"];

            skill.fPath = jdata.value("path", "linear");
            skill.fMissileStr = jdata.value("missile", "none");
            skill.fUnitStr = jdata.value("character", "none");
            skill.fMissileEnemyFindRange = jdata.value("enemyFindRange", 0.f);
            int count = jdata.value("count", 1);
            skill.fRadius = jdata.value("radius", 0.5f);
            skill.fSpeed = jdata.value("speed", 0.25f);
            skill.fMaxAngle = jdata.value("maxAngle", 0.f);
            skill.fAngleAdjust = jdata.value("angleAdjust", true);

            if(typeStr == "attack") {
                skill.fType = eSkillType::attack;
                skill.fCastRange = 0.f;
            } else if(typeStr == "smite") {
                skill.fType = eSkillType::smite;
                skill.fCastRange = 0.f;
            } else if(typeStr == "kick") {
                skill.fType = eSkillType::kick;
                skill.fCastRange = 0.f;
            } else if(typeStr == "missile") {
                skill.fType = eSkillType::missile;
                skill.fRangeTime = jdata.value("range", 8.f);
                skill.fCastRange = 8.f;
            } else if(typeStr == "shoot") {
                skill.fType = eSkillType::shoot;
            } else if(typeStr == "throw") {
                skill.fType = eSkillType::throw_;
            } else if(typeStr == "wall") {
                skill.fType = eSkillType::wall;
                skill.fRangeTime = jdata.value("time", 100.f)*eRunSettings::sFPS;
                skill.fPath = "static";
                skill.fCastRange = 8.f;
            } else if(typeStr == "summon") {
                skill.fType = eSkillType::summon;
                skill.fCastRange = 8.f;
            } else if(typeStr == "passive") {
                skill.fType = eSkillType::passive;
            } else {
                eRuntimeThrow("Unrecognized skill type \"" + typeStr + "\" for " + name);
            }

            float cooldown = jdata.value("cooldown", 0.f);
            float manaCost = jdata.value("manaCost", 0.f);

            skill.fCastAnims = jdata.value("castAnimations", std::vector<std::string>());
            if(jdata.contains("levels")) {
                std::map<eModifierType, eModifier> totalMods;
                const auto& levels = jdata["levels"];
                for(auto& [name, levelData] : levels.items()) {
                    eSkillLevel level;

                    count = levelData.value("count", count);
                    level.fCount = count;

                    cooldown = levelData.value("cooldown", cooldown);
                    level.fCooldown = cooldown;

                    manaCost = levelData.value("manaCost", manaCost);
                    level.fManaCost = manaCost;

                    for(auto it = levelData.begin(); it != levelData.end(); ++it) {
                        const auto& key = it.key();
                        if(key == "count" ||
                           key == "cooldown" ||
                           key == "manaCost") continue;
                        const auto& value = it.value();

                        eModifier mod;
                        mod.read(key, json(value));

                        auto& totalMod = totalMods[mod.fType];
                        totalMod.fType = mod.fType;
                        totalMod.fValue1 += mod.fValue1;
                        totalMod.fValue2 += mod.fValue2;
                    }

                    level.fTotalModifiers = totalMods;
                    skill.fLevels.emplace_back(level);
                }
            }
            if(jdata.contains("synergies")) {
                std::map<eModifierType, eModifier> totalMods;
                const auto& synergies = jdata["synergies"];
                skill.fSynergies.reserve(synergies.size());
                for(auto& [name, synergyData] : synergies.items()) {
                    auto& synergy = skill.fSynergies.emplace_back();
                    synergy.fSkillStr = name;
                    int count = 0;
                    float cooldown = 0.f;
                    float manaCost = 0.f;
                    for(auto it = synergyData.begin(); it != synergyData.end(); ++it) {
                        eSkillLevel level;

                        count += synergyData.value("count", 0);
                        level.fCount = count;

                        cooldown += synergyData.value("cooldown", 0.f);
                        level.fCooldown = cooldown;

                        manaCost += synergyData.value("manaCost", 0.f);
                        level.fManaCost = manaCost;

                        const auto& levelData = it.value();
                        for(auto it = levelData.begin(); it != levelData.end(); ++it) {
                            const auto& key = it.key();
                            if(key == "count" ||
                               key == "cooldown" ||
                               key == "manaCost") continue;
                            const auto& value = it.value();

                            eModifier mod;
                            mod.read(key, json(value));

                            auto& totalMod = totalMods[mod.fType];
                            totalMod.fType = mod.fType;
                            totalMod.fValue1 += mod.fValue1;
                            totalMod.fValue2 += mod.fValue2;
                        }

                        level.fTotalModifiers = totalMods;
                        synergy.fBoostLevels.emplace_back(level);
                    }
                }
            }
            sSkills.add(name, skill);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }

    for(const auto& it : sSkills) {
        auto& skill = it.fValue;
        auto& synergies = skill.fSynergies;
        for(auto& s : synergies) {
            const auto& name = s.fSkillStr;
            const int id = sSkills.id(name);
            s.fSkillId = id;
            if(id == -1) {
                eExceptions::showDialog("Unrecognized synergy \"" + name + "\".");
            }
        }
    }
}
