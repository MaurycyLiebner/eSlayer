#include "eSlayerHelpers/eelitemodifiersinfo.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<eEliteInfo>
eEliteModifiersInfo::sElite;
bool eEliteModifiersInfo::sLoaded = false;

void eEliteModifiersInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Units";

    std::vector<std::string> elites;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Elite/modifiers.json");
        elites = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Elite/modifiers.json");
    }

    sElite.reserve(elites.size() + 1);

    sElite.add("minion", eEliteInfo());

    for(const auto& name : elites) {
        try {
            eEliteInfo elite;
            const int oldId = sElite.id(name);
            if(oldId != -1) {
                eExceptions::showDialog("Duplicate elite '" + name +
                                        "' in " + dir + "/Elite/modifiers.json");
                continue;
            }
            const auto jdata = eFileLoaderBase::parse(dir, "Elite/" + name + ".json");
            if(jdata.contains("minions")) {
                const auto& minions = jdata["minions"];
                eSkills::parseSkillLevels(minions, elite.fMinions);
            } else {
                const ordered_json empty = ordered_json::object();
                eSkills::parseSkillLevels(empty, elite.fMinions);
            }
            if(jdata.contains("boss")) {
                const auto& boss = jdata["boss"];
                eSkills::parseSkillLevels(boss, elite.fBoss);
            } else {
                const ordered_json empty = ordered_json::object();
                eSkills::parseSkillLevels(empty, elite.fBoss);
            }
            sElite.add(name, elite);
        } catch(...) {
            eRuntimeThrow("Failed to parse elite modifiers \"" + name + "\".");
        }
    }
}
