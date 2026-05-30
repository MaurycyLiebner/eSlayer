#include "eSlayerHelpers/eauratypes.h"

#include "eSlayerHelpers/efileloaderbase.h"

bool eAuraTypes::sLoaded = false;
eStringIdMapVector<bool>
eAuraTypes::sTypes;

void eAuraTypes::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Skills";

    std::vector<std::string> types;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "auraTypes.json");
        types = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/auraTypes.json");
    }

    for(const auto& type : types) {
        sTypes.add(type, true);
    }
}
