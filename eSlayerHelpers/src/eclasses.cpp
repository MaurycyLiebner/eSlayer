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
        sClasses.add(name, eClass());
    }

    for(const auto& it : sClasses) {
        const auto& name = it.fName;
        try {
            auto& class_ = it.fValue;
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");

            const auto skillTrees = jdata.value("skillTrees", std::vector<std::string>());
            for(const auto& skillTree : skillTrees) {
                const int id = eSkillTrees::sTrees.id(skillTree);
                class_.fSkillTrees.emplace(id);
            }

            if(jdata.contains("iniItems")) {
                for(const auto& option : jdata["iniItems"]) {
                    std::vector<eIniItem> vec;
                    for(const auto& item : option) {
                        const auto typeStr = item.value("type", "");
                        const int id = eItemsData::id(typeStr);
                        if(id < 0) {
                            eRuntimeThrow("Unrecognized item type \"" + typeStr + "\".");
                        }
                        eIniItem iniItem;
                        iniItem.fItemType = id;
                        if(item.contains("modifiers")) {
                            const auto& mods = item["modifiers"];
                            for(const auto& [name, modData] : mods.items()) {
                                auto& mod = iniItem.fMods.emplace_back();
                                mod.read(name, json(modData));
                            }
                        }
                        vec.emplace_back(iniItem);
                    }
                    class_.fIniItems.emplace_back(vec);
                }
            }
        } catch(...) {
            eRuntimeThrow("Failed to parse \"" + dir + "/" + name + ".json\"");
        }
    }
}
