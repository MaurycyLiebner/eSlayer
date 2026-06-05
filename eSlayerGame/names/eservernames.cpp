#include "eservernames.h"

#include "../efileloader.h"

bool eServerNames::sLoaded = false;
std::map<std::string, std::string>
eServerNames::sMap;

std::string eServerNames::name(
    const std::string& key) {
    return sMap[key];
}

void eServerNames::load() {
    if(sLoaded) return;
    sLoaded = true;
    reload();
}

void eServerNames::reload() {
    const auto dir = "Servers";
    sMap = eFileLoader::loadNames(dir, "names");
}
