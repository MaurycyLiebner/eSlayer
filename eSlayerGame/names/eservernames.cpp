#include "eservernames.h"

#include "../efileloader.h"

std::map<std::string, std::string>
eServerNames::sMap;

std::string eServerNames::name(
    const std::string& key) {
    return sMap[key];
}

void eServerNames::load() {
    const auto dir = "Servers";
    sMap = eFileLoader::loadNames(dir, "names.txt");
}
