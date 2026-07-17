#include "eSlayerHelpers/eskilltrees.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eskills.h"

bool eSkillTrees::sLoaded = false;
eStringIdMapVector<eSkillTree>
eSkillTrees::sTrees;

void eSkillTrees::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Classes";

    std::vector<std::string> trees;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "skillTrees/skillTrees.json");
        trees = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/skillTrees/skillTrees.json");
    }

    sTrees.reserve(trees.size());

    for(const auto& name : trees) {
        try {
            eSkillTree tree;
            const auto jdata = eFileLoaderBase::parse(dir, "skillTrees/" + name + ".json");
            const auto& skills = jdata["skills"];
            for(auto& [skillName, skillData] : skills.items()) {
                const int skillId = eSkills::sSkills.id(skillName);
                tree.fSkills.emplace_back(skillId);
            }
            sTrees.add(name, tree);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}
