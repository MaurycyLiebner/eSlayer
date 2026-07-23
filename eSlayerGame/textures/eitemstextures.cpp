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

void eItemsTextures::loadTextures(
    SDL_Renderer* const r,
    const eResolution& res) {
    return sInstance.loadTexturesImpl(r, res);
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
            if(key == "shield" || key == "questItem") {
                for(const auto& [name, data] : value.items()) {
                    const auto path = key + "/" + name;
                    const int w = data.value("width", 2);
                    const int h = data.value("height", 2);
                    loadImpl(name, path, w, h);
                }
            } else if(key == "weapon") {
                for(auto& [type, items] : value.items()) {
                    for(const auto& [name, data] : items.items()) {
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
                    if(key == "gold") {
                        w = 0;
                        h = 0;
                    } else if(key == "amulet" || key == "ring" ||
                              key == "potion" || key == "jewel") {
                        w = 1;
                        h = 1;
                    } else if(key == "armor") {
                        w = 2;
                        h = 3;
                    } else if(key == "belt") {
                        w = 2;
                        h = 1;
                    } else if(key == "boots" || key == "gloves" || key == "helmet") {
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

    for(const auto& it : eItemsData::sItems) {
        auto& value = it.fValue;
        const int id = it.fId;
        const auto& texStr = value.fTextureStr;
        const int texId = mTexs.id(texStr);
        if(texId < 0) {
            eRuntimeThrow("Missing item texture for \"" + it.fName + "\".");
        }
        mItemDataIdToTexId[id] = texId;
        const auto& tex = mTexs.get(texId);
        value.fWidth = tex.fWidth;
        value.fHeight = tex.fHeight;
    }
}

void eItemsTextures::loadImpl(const std::string& name,
                              const std::string& path,
                              const int width,
                              const int height) {
    eItemTexture itemTex;
    itemTex.fWidth = width;
    itemTex.fHeight = height;
    itemTex.fTexPath = path;
    mTexs.add(name, itemTex);
}

void eItemsTextures::loadTexturesImpl(
    SDL_Renderer* const r,
    const eResolution& res) {
    if(mTexturesLoaded) return;
    mTexturesLoaded = true;
    for(const auto& it : mTexs) {
        auto& item = it.fValue;
        item.request(r, res);
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

eItemTexture& eItemsTextures::getByItemDataIdImpl(
    const int itemDataId) {
    const int id = mItemDataIdToTexId[itemDataId];
    return getImpl(id);
}

void eItemTexture::request(SDL_Renderer* const r,
                           const eResolution& res) {
    const auto dir = "Textures";
    const auto suffix = res.textureSuffix();
    if(!fTex) {
        const auto path = "ui/items/" + fTexPath + suffix + ".png";
        const bool e = eFileLoaderBase::fileExists(dir, path);
        if(e) {
            fTex = eFileLoader::readTexture(r, dir, path);
        }
    }
    if(!fTinyTex) {
        const auto groundPath = "items/" + fTexPath + suffix + ".png";
        const bool e = eFileLoaderBase::fileExists(dir, groundPath);
        if(e) {
            fTinyTex = eFileLoader::readTexture(r, dir, groundPath);
        }
        if(!fTinyTex || fTinyTex->isNull() && fTex) {
            const int w = std::round(0.25f*fTex->width());
            const int h = std::round(0.25f*fTex->height());
            fTinyTex = fTex->scaled(r, w, h);
        }
    }
}
