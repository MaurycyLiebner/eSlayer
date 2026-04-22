#include "eareanames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eclasses.h>

eAreaNames eAreaNames::sInstance;

std::string eAreaNames::name(const std::string& nameBase) {
    return sInstance.mNames[nameBase];
}

bool eAreaNames::load() {
    const auto dir = "Maps";
    sInstance.mNames = eFileLoader::loadNames(dir, "names.txt");
    return true;
}
