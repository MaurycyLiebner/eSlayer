#include "echartextures.h"

#include "espriteloader.h"

#include <eSlayerHelpers/echardatainfo.h>

std::shared_ptr<eCharModel>
eCharTextures::requestModel(
    const eModelParts& modelParts,
    const eResolution& res,
    SDL_Renderer* const r) const {
    const auto it = mModelMap.find(modelParts.fValues);
    if(it == mModelMap.end()) {
        const auto model = generateModel(modelParts, res, r);
        mModelMap[modelParts.fValues] = model;
        return model;
    } else {
        return it->second;
    }
}

std::shared_ptr<eCharModel>
eCharTextures::generateModel(
    const eModelParts& modelParts,
    const eResolution& res,
    SDL_Renderer* const r) const {
    const auto& info = eCharDataInfo::get(mCharDataId);
    const auto dir = "Textures";
    const auto path = "units/" + info.mName + "/";
    const auto result = std::make_shared<eCharModel>(*this);
    result->mNAnims = info.mAnims.size();
    result->mNGroups = info.mGroups.size();
    result->mNDirs = info.mDirs;
    for(const auto& parts : info.mGroups) {
        result->mNParts.push_back(parts.size());
    }
    result->mAnims.reserve(info.mAnims.size());
    for(const auto& anim : info.mAnims) {
        const int nFrames = anim.fValue.fFrames;
        const auto animPath = path + anim.fName + "/";
        const int animId = result->mAnims.size();
        auto& ranim = result->mAnims.emplace_back();
        ranim.fFrames = anim.fValue.fFrames;
        ranim.fOffset = anim.fValue.fOffset;
        ranim.fClamp = anim.fValue.fClamp;
        ranim.fGroups.reserve(info.mGroups.size());
        for(const auto& parts : info.mGroups) {
            auto& rparts = ranim.fGroups.emplace_back();
            rparts.reserve(parts.size());
            for(const int partId : parts) {
                const auto& partOptions = info.mParts.get(partId);
                const auto partName = info.mParts.name(partId);
                const int eqId = modelParts.fValues[partId];
                const auto eqName = partOptions.name(eqId);
                auto& rpart = rparts.emplace_back();
                const eCharTextureKey key{animId, partId, eqId};
                const auto it = mTexMap.find(key);
                if(it == mTexMap.end()) {
                    auto& partMap = mTexMap[key];
                    partMap.reserve(info.mDirs);
                    const auto partPath = animPath + partName + "_" + eqName;
                    eSpriteLoader loader(dir, partPath, res, r, mColorKey);
                    for(int i = 0; i < info.mDirs; i++) {
                        const auto coll = std::make_shared<eTextureCollection>();
                        for(int f = 0; f < nFrames; f++) {
                            loader.load(i*nFrames + f, *coll);
                        }
                        partMap.emplace_back(coll);
                    }
                    rpart = partMap;
                } else {
                    rpart = it->second;
                }
            }
        }
    }
    return result;
}

eModelParts eCharTextures::mapToModelParts(const eStringMap& m) const {
    const auto& info = eCharDataInfo::get(mCharDataId);
    return info.mapToModelParts(m);
}

int eCharTextures::animClamp(const int a) const {
    const auto& info = eCharDataInfo::get(mCharDataId);
    return info.animClamp(a);
}

eCharData& eCharTextures::charData() const {
    return eCharDataInfo::get(mCharDataId);;
}

void eCharTextures::setCharDataId(const int id) {
    mCharDataId = id;
}

void eCharTextures::load(const ordered_json& jdata) {
    const auto colorKey = jdata.value("colorKey", std::vector<Uint8>{0, 0, 0, 0});
    if(colorKey.size() == 3) {
        mColorKey = SDL_Color{colorKey[0], colorKey[1], colorKey[2], 255};
    }
}

void eCharTextures::loadAll(const eResolution& res,
                            SDL_Renderer* const r) {
    const auto& info = eCharDataInfo::get(mCharDataId);
    const auto dir = "Textures";
    const auto path = "units/" + info.mName + "/";
    for(const auto& anim : info.mAnims) {
        const int nFrames = anim.fValue.fFrames;
        const auto animPath = path + anim.fName + "/";
        for(const auto& parts : info.mGroups) {
            for(const int partId : parts) {
                const auto& partOptions = info.mParts.get(partId);
                const auto partName = info.mParts.name(partId);
                for(int eqId = 0; eqId < partOptions.size(); ++eqId) {
                    const auto eqName = partOptions.name(eqId);
                    const eCharTextureKey key{anim.fId, partId, eqId};
                    const auto it = mTexMap.find(key);
                    if(it == mTexMap.end()) {
                        auto& partMap = mTexMap[key];
                        partMap.reserve(info.mDirs);
                        const auto partPath = animPath + partName + "_" + eqName;
                        eSpriteLoader loader(dir, partPath, res, r, mColorKey);
                        for(int i = 0; i < info.mDirs; i++) {
                            const auto coll = std::make_shared<eTextureCollection>();
                            for(int f = 0; f < nFrames; f++) {
                                loader.load(i*nFrames + f, *coll);
                            }
                            partMap.emplace_back(coll);
                        }
                    }
                }
            }
        }
    }
}
