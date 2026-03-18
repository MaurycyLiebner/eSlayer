#include "echartextures.h"

#include "espriteloader.h"

eCharTextures::eCharTextures() {}

std::shared_ptr<eCharModel> eCharTextures::generateModel(
    const eModelParts& modelParts,
    SDL_Renderer * const r) {
    const auto dir = "Textures";
    const auto path = "chars/" + mName + "/";
    const auto result = std::make_shared<eCharModel>(*this);
    result->mNAnims = mAnims.size();
    result->mNGroups = mGroups.size();
    result->mNDirs = mDirs;
    for(const auto& parts : mGroups) {
        result->mNParts.push_back(parts.size());
    }
    for(const auto& anim : mAnims) {
        const auto animPath = path + anim.fName + "/";
        auto& ranim = result->mAnims.emplace_back();
        ranim.fFrames = anim.fValue.fFrames;
        ranim.fOffset = anim.fValue.fOffset;
        ranim.fClamp = anim.fValue.fClamp;
        for(const auto& parts : mGroups) {
            auto& rparts = ranim.fGroups.emplace_back();
            for(const int partId : parts) {
                const auto& partOptions = mParts.get(partId);
                const auto partName = mParts.name(partId);
                const int eqId = modelParts.fValues[partId];
                const auto eqName = partOptions.name(eqId);
                const auto partPath = animPath + partName + "_" + eqName;
                auto& rpart = rparts.emplace_back();
                rpart.reserve(mDirs);
                const auto it = mTexMap.find(partPath);
                if(it == mTexMap.end()) {
                    auto& partMap = mTexMap[partPath];
                    eSpriteLoader loader(dir, partPath, r, mColorKey);
                    for(int i = 0; i < mDirs; i++) {
                        const auto coll = std::make_shared<eTextureCollection>();
                        const int nFrames = anim.fValue.fFrames;
                        for(int f = 0; f < nFrames; f++) {
                            loader.load(i*nFrames + f, *coll);
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

void eCharTextures::load(ordered_json& jdata) {
    const auto colorKey = jdata.value("colorKey", std::vector<Uint8>{0, 0, 0, 0});
    if(colorKey.size() == 3) {
        mColorKey = SDL_Color{colorKey[0], colorKey[1], colorKey[2], 255};
    }
    return eCharData::load(jdata);
}
