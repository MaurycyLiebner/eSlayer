#include "eobjstextures.h"

#include "espriteloader.h"
#include "../efileloader.h"

#include <eSlayerHelpers/eobjectsinfo.h>

bool eObjsTextures::sLoaded = false;
eStringIdMapVector<eObjTextures>
eObjsTextures::sInstance;

void eObjTextures::load(
    const eResolution& res,
    SDL_Renderer* const r) {
    if(mLoaded) return;
    mLoaded = true;
    int texId = 0;
    const auto path = "objects/" + fName + "/" + fName;
    eSpriteLoader loader("Textures", path, res, r);
    for(auto& type : fTypes) {
        for(auto& anim : type) {
            for(int i = 0; i < anim.fFrames; i++) {
                loader.load(texId++, anim.fTexs);
            }
        }
    }
}

int eObjsTextures::id(const std::string& name) {
    return sInstance.id(name);
}

eObjTextures& eObjsTextures::get(const std::string& name) {
    return get(id(name));
}

eObjTextures& eObjsTextures::get(const int id) {
    return sInstance.get(id);
}

void eObjsTextures::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Textures";

    const auto path = "objects/objects.json";
    std::vector<std::string> names;
    try {
        const auto jdata = eFileLoader::parse(dir, path);
        names = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/" + path);
    }
    for(const auto& name : names) {
        const auto path = "objects/" + name + "/" + name + ".json";
        try {
            eObjTextures obj;
            obj.fName = name;
            const auto jdata = eFileLoader::parse(dir, path);
            const int types = jdata.value("types", 1);
            obj.fBlocksLight = jdata.value("blocksLight", true);
            obj.fShadow = jdata.value("shadow", true);
            obj.fFlat = jdata.value("flat", false);
            struct eAnimInfo {
                eAnimInfo(const std::string& name,
                          const int frames) :
                    fFrames(frames), fName(name) {}

                std::string fName;
                int fFrames;
            };
            std::vector<eAnimInfo> anims;
            if(jdata.contains("anims")) {
                const auto& animsJS = jdata["anims"];
                for(auto it = animsJS.begin(); it != animsJS.end(); ++it) {
                    const auto& name = it.key();
                    const auto& value = it.value();
                    if(value.contains("overwrite")) {
                        const auto oname = value["overwrite"];
                        obj.fAnimIds[name] = obj.fAnimIds[oname];
                    } else {
                        const int frames = value.value("frames", 1);
                        obj.fAnimIds[name] = anims.size();
                        anims.emplace_back(name, frames);
                    }
                }
            } else {
                const auto name = "normal";
                anims.emplace_back(name, 1);
                obj.fAnimIds[name] = 0;
            }
            for(int i = 0; i < types; i++) {
                auto& type = obj.fTypes.emplace_back();
                for(const auto& a : anims) {
                    type.emplace_back(a.fFrames);
                }
            }
            sInstance.add(name, obj);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + path);
        }
    }

    for(const auto& it : eObjectsInfo::sObjects) {
        auto& info = it.fValue;
        info.fTexId = sInstance.id(info.fTexStr);
    }
}
