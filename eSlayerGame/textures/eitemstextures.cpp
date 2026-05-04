#include "eitemstextures.h"

#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/eexceptions.h>
#include <eSlayerHelpers/eitemsdata.h>

#include "../efileloader.h"
#include "../eresolution.h"

eItemsTextures eItemsTextures::sInstance;

eItemTexture& eItemsTextures::get(const std::string& name) {
    return sInstance.getImpl(name);
}

eItemTexture& eItemsTextures::get(const int id) {
    return sInstance.getImpl(id);
}

eItemTexture& eItemsTextures::getByItemDataId(
    const int itemDataId) {
    return sInstance.getByItemDataIdImpl(itemDataId);
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
                    const auto path = key + "/" + name;
                    const int w = data.value("width", 2);
                    const int h = data.value("height", 2);
                    loadImpl(name, path, w, h);
                }
            } else if(key == "weapons") {
                for(auto& [type, items] : value.items()) {
                    for(auto& [name, data] : items.items()) {
                        const auto path = key + "/" + type + "/" + name;
                        const int w = data.value("width", 2);
                        const int h = data.value("height", 3);
                        loadImpl(name, path, w, h);
                    }
                }
            } else {
                const std::vector<std::string> names = value;
                for(const auto& name : value) {
                    const auto path = key + "/" + name.get<std::string>();
                    int w;
                    int h;
                    if(key == "amulets" || key == "rings" || key == "potions") {
                        w = 1;
                        h = 1;
                    } else if(key == "armor") {
                        w = 2;
                        h = 3;
                    } else if(key == "belts") {
                        w = 2;
                        h = 1;
                    } else if(key == "boots" || key == "gloves") {
                        w = 2;
                        h = 2;
                    } else if(key == "bolts" || key == "arrows") {
                        w = 1;
                        h = 3;
                    } else {
                        eRuntimeThrow("Unrecognized item type " + key);
                    }
                    loadImpl(name, path, w, h);
                }
            }
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/ui/items/items.json");
    }
}

void eItemsTextures::loadImpl(const std::string& name,
                              const std::string& path,
                              const int width,
                              const int height) {
    eItemTexture itemTex;

    const int itemDataId = eItemsData::id(name);
    itemTex.fItemDataId = itemDataId;
    itemTex.fTexPath = path;

    auto& itemData = eItemsData::get(itemDataId);
    itemData.fWidth = width;
    itemData.fHeight = height;

    const int id = mTexs.add(name, itemTex);
    mItemDataIdToTexId[itemDataId] = id;
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

eItemTexture& eItemsTextures::getByItemDataIdImpl(
    const int itemDataId) {
    const int id = mItemDataIdToTexId[itemDataId];
    return getImpl(id);
}

void eItemTexture::request(SDL_Renderer* const r,
                           const eResolution& res) {
    if(fTex) return;
    const auto dir = "Textures";
    const auto suffix = res.textureSuffix();
    const auto path = "ui/items/" + fTexPath + suffix + ".png";
    fTex = eFileLoader::readTexture(r, dir, path);
    const int w = std::round(0.2f*fTex->width());
    const int h = std::round(0.2f*fTex->height());
    fTinyTex = fTex->scaled(r, w, h);
}
