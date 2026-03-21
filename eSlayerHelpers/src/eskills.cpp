#include "eSlayerHelpers/eskills.h"

#include "eSlayerHelpers/efileloaderbase.h"

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
        if(typeStr == "attack") {
            skill.fType = eSkillType::attack;
        } else if(typeStr == "missile") {
            skill.fType = eSkillType::missile;
            skill.fMissile = jdata["missile"];
            skill.fPath = jdata.value("path", "linear");
            skill.fRange = jdata.value("range", 5.f);
            skill.fRadius = jdata.value("radius", 0.5f);
            skill.fSpeed = jdata.value("speed", 0.25f);
            skill.fPierceChance = jdata.value("pierce", 0.f);
        } else {
            eRuntimeThrow("Unrecognized skill type \"" + typeStr + "\" for " + name);
        }
        const auto iconStr = jdata["icon"];
        skill.fIcon = iconStr;
        sSkills.add(name, skill);
    }
}
