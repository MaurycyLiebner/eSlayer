#include "elanguage.h"

#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/efileloaderbase.h>

eLanguage eLanguage::sLanguage{"english", "en"};
std::vector<eLanguage> eLanguage::sLanguages;
bool eLanguage::sLoaded = false;

eLanguage::eLanguage(const std::string& name,
                     const std::string& suffix) :
    fName(name), fSuffix(suffix) {}

void eLanguage::setLanguage(const std::string& name) {
    for(const auto& l : sLanguages) {
        if(l.fName != name) continue;
        sLanguage = l;
        break;
    }
}

void eLanguage::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Languages";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "languages.json");
        const auto languages = jdata.get<std::vector<std::vector<std::string>>>();
        for(const auto& l : languages) {
            sLanguages.emplace_back(l[0], l[1]);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/languages.json");
    }
}
