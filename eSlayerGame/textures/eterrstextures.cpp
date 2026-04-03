#include "eterrstextures.h"

#include <eSlayerHelpers/eterrstexturesdata.h>

eStringIdMapVector<eTileTextures>
eTerrsTextures::sInstance;

int eTerrsTextures::id(const std::string& name) {
    return sInstance.id(name);
}

eTileTextures& eTerrsTextures::get(const std::string& name) {
    return get(id(name));
}

eTileTextures& eTerrsTextures::get(const int id) {
    return sInstance.get(id);
}

void eTerrsTextures::load() {
    eTerrsTexturesData::forEach([](const eTileTexturesData::eIt& it) {
        eTileTextures texs;
        const auto& data = it.fValue;
        texs.initialize(data.fDirName, data.fName);
        sInstance.add(it.fName, texs);
    });
}
