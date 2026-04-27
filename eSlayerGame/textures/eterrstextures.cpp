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
    for(const auto& it : eTerrsTexturesData::sTexs) {
        const auto& data = it.fValue;
        eTileTextures texs;
        reinterpret_cast<eTileTextureData&>(texs) = data;
        sInstance.add(it.fName, texs);
    }
}
