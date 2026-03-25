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
        eSkill skill;
        const auto jdata = eFileLoaderBase::parse(dir, name + ".json");
        const std::string typeStr = jdata["type"];
        skill.fIcon = jdata["icon"];

        skill.fPath = jdata.value("path", "linear");
        skill.fMissile = jdata.value("missile", "none");
        skill.fBaseMissiles = jdata.value("missiles", 1);
        skill.fRadius = jdata.value("radius", 0.5f);
        skill.fSpeed = jdata.value("speed", 0.25f);
        skill.fBasePierceChance = jdata.value("pierce", 0.f);
        skill.fMaxAngle = jdata.value("maxAngle", 0.f);

        if(typeStr == "attack") {
            skill.fType = eSkillType::attack;
            skill.fCastRange = 0.f;
        } else if(typeStr == "missile") {
            skill.fType = eSkillType::missile;
            skill.fRangeTime = jdata.value("range", 8.f);
            skill.fCastRange = 8.f;
        } else if(typeStr == "wall") {
            skill.fType = eSkillType::wall;
            skill.fRangeTime = jdata.value("time", 100.f)*eRunSettings::sFPS;
            skill.fPath = "static";
            skill.fCastRange = 8.f;
        } else {
            eRuntimeThrow("Unrecognized skill type \"" + typeStr + "\" for " + name);
        }

        skill.fBaseCooldown = jdata.value("cooldown", 0.f);
        skill.fBaseDamage.fFire = jdata.value("fireDamage", 0.f);

        skill.fCastAnims = jdata.value("castAnimations", std::vector<std::string>());
        if(jdata.contains("levels")) {
            const auto& levels = jdata["levels"];
            int missiles = skill.fBaseMissiles;
            float pierce = skill.fBasePierceChance;
            float cooldown = skill.fBaseCooldown;
            eDamage damage = skill.fBaseDamage;
            for(auto& [name, levelData] : levels.items()) {
                eSkillLevel level;

                missiles = levelData.value("missiles", missiles);
                level.fMissiles = missiles;

                pierce = levelData.value("pierce", pierce);
                level.fPierceChance = pierce;

                cooldown = levelData.value("cooldown", cooldown);
                level.fPierceChance = cooldown;

                const float physicalIncrease = levelData.value("physicalDamageIncrease", 0.f);
                damage.fPhysical *= 1.f + physicalIncrease;
                const float fireIncrease = levelData.value("fireDamageIncrease", 0.f);
                damage.fFire *= 1.f + fireIncrease;
                const float coldIncrease = levelData.value("coldDamageIncrease", 0.f);
                damage.fCold *= 1.f + coldIncrease;
                const float lightningIncrease = levelData.value("lightningDamageIncrease", 0.f);
                damage.fLightning *= 1.f + lightningIncrease;
                level.fDamage = damage;

                skill.fLevels.emplace_back(level);
            }
        }
        sSkills.add(name, skill);
    }
}
