#include "emercenarynames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/emercenaries.h>

eStringIdMapVector<std::vector<std::string>>
eMercenaryNames::sNames;
std::map<int, std::string>
eMercenaryNames::sTypeNames;
bool eMercenaryNames::sLoaded = false;

bool eMercenaryNames::load() {
    if(sLoaded) return false;
    sLoaded = true;
    reload();
    return true;
}

bool eMercenaryNames::reload() {
    sNames.clear();
    const auto dir = "Units";

    std::vector<std::string> mercenaries;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Mercenaries/mercenaries.json");
        mercenaries = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Mercenaries/mercenaries.json");
    }

    for(const auto& m : mercenaries) {
        try {
            const auto strMap = eFileLoader::loadNames(dir, "Mercenaries/" + m);
            std::vector<std::string> toAdd;
            for(const auto& it : strMap) {
                toAdd.emplace_back(it.second);
            }
            sNames.add(m, toAdd);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/Mercenaries/" + m);
        }
    }

    const auto strMap = eFileLoader::loadNames(dir, "Mercenaries/names");
    for(const auto& it : strMap) {
        const auto id = eMercenariesInfo::sMercs.id(it.first);
        sTypeNames[id] = it.second;
    }
    return true;
}