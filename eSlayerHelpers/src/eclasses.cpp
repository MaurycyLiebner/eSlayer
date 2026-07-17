#include "eSlayerHelpers/eclasses.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eskilltrees.h"

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

    sClasses.reserve(classes.size());

    for(const auto& name : classes) {
        try {
            eClass class_;
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");
            const auto skillTrees = jdata.value("skillTrees", std::vector<std::string>());
            for(const auto& skillTree : skillTrees) {
                const int id = eSkillTrees::sTrees.id(skillTree);
                class_.fSkillTrees.emplace_back(id);
            }
            sClasses.add(name, class_);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}
