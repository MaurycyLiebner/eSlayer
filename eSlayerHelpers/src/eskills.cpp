#include "eSlayerHelpers/eskills.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/erunsettings.h"

bool eSkills::sLoaded = false;
eStringIdMapVector<eSkill> eSkills::sSkills;

void eSkills::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Skills";

    const auto jdata = eFileLoaderBase::parse(dir, "skills.json");
    const auto skills = jdata.get<std::vector<std::string>>();

    sSkills.reserve(skills.size());

    for(const auto& name : skills) {
        std::map<eModifierType, eModifier> modifiers;

        eSkill skill;
        const auto jdata = eFileLoaderBase::parse(dir, name + ".json");
        const std::string typeStr = jdata["type"];
        skill.fIcon = jdata["icon"];

        skill.fPath = jdata.value("path", "linear");
        skill.fMissile = jdata.value("missile", "none");
        skill.fMissileEnemyFindRange = jdata.value("enemyFindRange", 0.f);
        int nMissiles = jdata.value("missiles", 1);
        skill.fRadius = jdata.value("radius", 0.5f);
        skill.fSpeed = jdata.value("speed", 0.25f);
        skill.fMaxAngle = jdata.value("maxAngle", 0.f);

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

                nMissiles = levelData.value("missiles", nMissiles);
                level.fMissiles = nMissiles;

                cooldown = levelData.value("cooldown", cooldown);
                level.fCooldown = cooldown;

                manaCost = levelData.value("manaCost", manaCost);
                level.fManaCost = manaCost;

                for(auto it = levelData.begin(); it != levelData.end(); ++it) {
                    const auto& key = it.key();
                    const auto& value = it.value();
                    eModifier mod;
                    if(key == "fireDamage") {
                        mod.fType = eModifierType::damageFire;
                        mod.fValue1 = value[0];
                        mod.fValue2 = value[1];
                    } else if(key == "pierce") {
                        mod.fType = eModifierType::pierceChance;
                        mod.fValue1 = value;
                    } else {
                        continue;
                    }
                    auto& totalMod = totalMods[mod.fType];
                    totalMod.fType = mod.fType;
                    totalMod.fValue1 += mod.fValue1;
                    totalMod.fValue2 += mod.fValue2;
                }

                level.fTotalModifiers = totalMods;
                skill.fLevels.emplace_back(level);
            }
        }
        sSkills.add(name, skill);
    }
}
