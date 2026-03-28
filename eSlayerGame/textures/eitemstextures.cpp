#include "eitemstextures.h"

#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/eitemsdata.h>

#include "../efileloader.h"

eItemsTextures eItemsTextures::sInstance;

eItemTexture& eItemsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eItemTexture& eItemsTextures::get(const int id) {
    return sInstance.getImpl(id);
}

void eItemsTextures::load() {
    return sInstance.loadImpl();
}

void eItemsTextures::loadImpl() {
    if(mLoaded) return;
    mLoaded = true;

    const auto dir = "Textures";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "ui/items/items.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            if(key == "shields") {
                for(auto& [name, data] : value.items()) {
                    eItemTexture itemTex;

                    const auto path = key + "/" + name;
                    itemTex.fItemDataId = eItemsData::id(path);
                    itemTex.fTexPath = path;

                    auto& itemData = eItemsData::get(itemTex.fItemDataId);
                    itemData.fWidth = data.value("width", 2);
                    itemData.fHeight = data.value("height", 2);

                    mTexs.add(path, itemTex);
                }
            } else if(key == "weapons") {
                for(auto& [type, items] : value.items()) {
                    for(auto& [name, data] : items.items()) {
                        eItemTexture itemTex;

                        const auto path = key + "/" + type + "/" + name;
                        itemTex.fItemDataId = eItemsData::id(path);
                        itemTex.fTexPath = path;

                        auto& itemData = eItemsData::get(itemTex.fItemDataId);
                        itemData.fWidth = data.value("width", 2);
                        itemData.fHeight = data.value("height", 2);

                        mTexs.add(path, itemTex);
                    }
                }
            } else {
                const std::vector<std::string> names = value;
                for(const auto& name : value) {
                    eItemTexture itemTex;
                    const auto path = key + "/" + name.get<std::string>();
                    itemTex.fItemDataId = eItemsData::id(path);
                    itemTex.fTexPath = path;

                    auto& itemData = eItemsData::get(itemTex.fItemDataId);
                    if(key == "amulets" || key == "rings") {
                        itemData.fWidth = 1;
                        itemData.fHeight = 1;
                    } else if(key == "armor") {
                        itemData.fWidth = 2;
                        itemData.fHeight = 3;
                    } else if(key == "belts") {
                        itemData.fWidth = 2;
                        itemData.fHeight = 1;
                    } else if(key == "boots") {
                        itemData.fWidth = 2;
                        itemData.fHeight = 2;
                    } else if(key == "gloves") {
                        itemData.fWidth = 2;
                        itemData.fHeight = 2;
                    } else {
                        eRuntimeThrow("Unrecognized item type " + key);
                    }

                    mTexs.add(path, itemTex);
                }
            }
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/ui/items/items.json");
    }
}

eItemTexture& eItemsTextures::getImpl(const std::string &name) {
    const int id = mTexs.id(name);
    return getImpl(id);
}

eItemTexture& eItemsTextures::getImpl(const int id) {
    if(id < 0 || id >= mTexs.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return mTexs.get(id);
}

void eItemTexture::request(SDL_Renderer* const r) {
    if(fTex) return;
    const auto dir = "Textures";
    const auto path = "ui/items/" + fTexPath + ".png";
    fTex = eFileLoader::readTexture(r, dir, path);
}
