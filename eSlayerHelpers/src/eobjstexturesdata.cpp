#include "eSlayerHelpers/eobjstexturesdata.h"

eObjsTexturesData eObjsTexturesData::sInstance;

eObjsTexturesData::eObjsTexturesData() :
    eTileTexturesData("objects") {}

int eObjsTexturesData::id(const std::string& name) {
    return sInstance.idImpl(name);
}

eTileTextureData& eObjsTexturesData::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eTileTextureData& eObjsTexturesData::get(const int id) {
    return sInstance.getImpl(id);
}

void eObjsTexturesData::load() {
    return sInstance.loadImpl();
}

void eObjsTexturesData::forEach(const eIter& iter) {
    sInstance.forEachImpl(iter);
}
