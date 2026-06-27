#include "eareanames.h"

#include "../efileloader.h"

eAreaNames eAreaNames::sInstance;

const std::string&
eAreaNames::name(const std::string& nameBase) {
    return sInstance.mNames[nameBase];
}

bool eAreaNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eAreaNames::reload() {
    const auto dir = "Maps";
    sInstance.mNames = eFileLoader::loadNames(dir, "names");
    return true;
}
