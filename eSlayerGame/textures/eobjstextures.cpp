#include "eobjstextures.h"

#include <eSlayerHelpers/eobjstexturesdata.h>

eStringIdMapVector<eTileTextures>
eObjsTextures::sInstance;

int eObjsTextures::id(const std::string& name) {
    return sInstance.id(name);
}

eTileTextures& eObjsTextures::get(const std::string& name) {
    return get(id(name));
}

eTileTextures& eObjsTextures::get(const int id) {
    return sInstance.get(id);
}

void eObjsTextures::load() {
    eObjsTexturesData::forEach([](const eTileTexturesData::eIt& it) {
        eTileTextures texs;
        const auto& data = it.fValue;
        texs.initialize(data.fDirName, data.fName);
        sInstance.add(it.fName, texs);
    });
}
