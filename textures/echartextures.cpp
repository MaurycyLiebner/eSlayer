#include "echartextures.h"

#include "../egamedir.h"
#include "espriteloader.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;

#include <fstream>

eCharTextures::eCharTextures() {}

eCharModel eCharTextures::generateModel(const eModelParts& modelParts,
                                        SDL_Renderer * const r) {
    const auto dir = eGameDir::path("Textures/chars/char/");
    eCharModel result;
    result.mNAnims = mAnimFrames.size();
    result.mNGroups = mGroups.size();
    result.mNDirs = mDirs;
    for(const auto& parts : mGroups) {
        result.mNParts.push_back(parts.size());
    }
    for(const auto& anim : mAnimFrames) {
        const auto animDir = dir + anim.first + "/";
        auto& ranim = result.mAnims.emplace_back();
        ranim.fFrames = anim.second;
        for(const auto& parts : mGroups) {
            auto& rparts = ranim.fGroups.emplace_back();
            for(const auto& part : parts) {
                const auto modelPart = modelParts.at(part.first);
                const std::string partPath = animDir + part.first + "_" + modelPart;
                auto& rpart = rparts.emplace_back();
                const auto it = mTexMap.find(partPath);
                if(it == mTexMap.end()) {
                    auto& partMap = mTexMap[partPath];
                    eSpriteLoader loader(partPath, r, true);
                    for(int i = 0; i < mDirs; i++) {
                        const auto coll = std::make_shared<eTextureCollection>(r);
                        for(int f = 0; f < anim.second; f++) {
                            loader.load(i*anim.second + f, *coll);
                        }
                        partMap[i] = coll;
                        rpart.emplace_back(coll);
                    }
                } else {
                    auto& partMap = mTexMap[partPath];
                    for(int i = 0; i < mDirs; i++) {
                        rpart.emplace_back(partMap[i]);
                    }
                }
            }
        }
    }
    return result;
}

void eCharTextures::load(const std::string &path) {
    std::ifstream f(path);
    json data = json::parse(f);

    mDirs = data["directions"];
    const auto anims = data["animations"].items();
    for(auto& [name, animData] : anims) {
        const int frames = animData.value("frames", 0);
        mAnimFrames[name] = frames;
    }

    const auto groups = data["groups"];
    for(const auto& groupJson : groups) {
        auto& group = mGroups.emplace_back();
        const auto items = groupJson.items();
        for(auto& [key, valueArray] : items) {
            const auto values = valueArray.get<std::vector<std::string>>();
            group[key] = values;
        }
    }
}
