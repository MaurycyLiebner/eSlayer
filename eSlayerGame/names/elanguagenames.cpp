#include "elanguagenames.h"

#include "../efileloader.h"

eLanguageNames eLanguageNames::sInstance;

const std::string& eLanguageNames::name(const std::string& name) {
    return sInstance.mNames[name];
}

bool eLanguageNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eLanguageNames::reload() {
    const auto dir = "Languages";
    sInstance.mNames = eFileLoader::loadNames(dir, "names");
    return true;
}
