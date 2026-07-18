#include "eSlayerHelpers/eclasses.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eskilltrees.h"
#include "eSlayerHelpers/eitemsdata.h"

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
                class_.fSkillTrees.emplace(id);
            }

            const auto iniItems = jdata.value("iniItems", std::vector<std::vector<std::string>>());
            for(const auto& option : iniItems) {
                std::vector<int> vec;
                for(const auto& item : option) {
                    const int id = eItemsData::id(item);
                    if(id < 0) {
                        eRuntimeThrow("Unrecognized item type \"" + item + "\".");
                    }
                    vec.emplace_back(id);
                }
                class_.fIniItems.emplace_back(vec);
            }

            sClasses.add(name, class_);
        } catch(const std::exception& e) {
            eExceptions::showDialog(e);
        }
    }
}
