#include "eSlayerHelpers/eboostcursetypes.h"

#include "eSlayerHelpers/efileloaderbase.h"

bool eBoostCurseTypes::sLoaded = false;
eStringIdMapVector<bool>
eBoostCurseTypes::sTypes;

void eBoostCurseTypes::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Skills";

    std::vector<std::string> types;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "boostCurseTypes.json");
        types = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/boostCurseTypes.json");
    }

    sTypes.add("permanent_", true);
    sTypes.add("merc_", true);
    sTypes.add("summon_", true);
    for(const auto& type : types) {
        sTypes.add(type, true);
    }
}
