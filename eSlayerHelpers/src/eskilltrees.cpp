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
                eSkillTreeSkill skill;
                const int skillId = eSkills::sSkills.id(skillName);
                if(skillId < 0) {
                    eRuntimeThrow("Unrecognized skill \"" + skillName + "\".");
                }
                skill.fSkillId = skillId;
                skill.fLevelReq = skillData.value("levelReq", 1);
                skill.fColumn = skillData.value("column", 0);

                const auto preqs = skillData.value("prerequisites", std::vector<std::string>());
                for(const auto& skillName : preqs) {
                    const int skillId = eSkills::sSkills.id(skillName);
                    if(skillId < 0) {
                        eRuntimeThrow("Unrecognized skill \"" + skillName + "\".");
                    }
                    skill.fPrerequisites.emplace(skillId);
                }

                tree.fSkills.emplace_back(skill);
            }

            auto& maxReq = tree.fMaxLevelReq;
            if(jdata.contains("maxLevelReq")) {
                maxReq = jdata.value("maxLevelReq", 30);
            } else {
                maxReq = 1;
                for(const auto& s : tree.fSkills) {
                    maxReq = std::max(maxReq, s.fLevelReq);
                }
            }
            sTrees.add(name, tree);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}
