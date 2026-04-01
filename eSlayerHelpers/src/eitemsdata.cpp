#include "eSlayerHelpers/eitemsdata.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eexceptions.h"

// #include <fstream>

eItemsData eItemsData::sInstance;

eItemData& eItemsData::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eItemData& eItemsData::get(const int id) {
    return sInstance.getImpl(id);
}

int eItemsData::id(const std::string& name) {
    return sInstance.idImpl(name);
}

std::string eItemsData::name(const int id) {
    return sInstance.nameImpl(id);
}

void eItemsData::load() {
    return sInstance.loadImpl();
}

void eItemsData::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Items";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "items.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            if(key == "weapons") {
                for(auto& [type, types] : value.items()) {
                    for(const auto& name : types) {
                        loadImpl(name);
                    }
                }
            } else {
                for(const auto& name : value) {
                    loadImpl(name);
                }
            }
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/items.json");
    }
}

void eItemsData::loadImpl(const std::string& name) {
    const auto dir = "Items";

    // {
    //     const auto filePath = eFileLoaderBase::sFilePath(dir, name + ".json");
    //     std::ofstream output(filePath);
    //     output << "{" << std::endl << std::endl << "}";
    // }

    const auto jdata = eFileLoaderBase::parse(dir, name + ".json");

    eItemData itemData;
    itemData.fLevelReq = jdata.value("levelReq", 1);
    itemData.fStrengthReq = jdata.value("strReq", 0);
    itemData.fDexterityReq = jdata.value("dexReq", 0);
    mItems.add(name, itemData);
}

eItemData& eItemsData::getImpl(const std::string &name) {
    return getImpl(idImpl(name));
}

eItemData& eItemsData::getImpl(const int id) {
    if(id < 0 || id >= mItems.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return mItems.get(id);
}

int eItemsData::idImpl(const std::string& name) const {
    return mItems.id(name);
}

std::string eItemsData::nameImpl(const int id) const {
    return mItems.name(id);
}
