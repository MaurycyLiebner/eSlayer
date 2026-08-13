#include "echartextures.h"

#include "espriteloader.h"

#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eitempartsmap.h>

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
    result->mNParts = info.mNParts;
    result->mNDirs = info.mDirs;
    result->mAnims.reserve(info.mAnims.size());
    for(const auto& anim : info.mAnims) {
        const int nFrames = anim.fValue.fFrames;
        const auto animPath = path + anim.fName + "/";
        const int animId = result->mAnims.size();
        auto& ranim = result->mAnims.emplace_back();
        ranim.fFrames = anim.fValue.fFrames;
        ranim.fClamp = anim.fValue.fClamp;
        auto& rparts = ranim.fParts;
        rparts.reserve(info.mNParts);
        for(const auto& ipit : info.mParts) {
            const int partId = ipit.fId;
            const uint8_t eqId = modelParts.fValues[partId];
            auto& rpart = rparts.emplace_back();

            int basePartId = -1;
            uint8_t baseEqId = 255;
            const auto& partsBase = eItemPartsMap::sPartsBase;
            const auto& partName = ipit.fName;
            const auto pit = partsBase.find(partName);
            if(pit != partsBase.end()) {
                const auto& baseName = pit->second;
                basePartId = info.mParts.id(baseName);
                baseEqId = modelParts.fValues[basePartId];
            }

            const eCharTextureKey key{animId, partId, eqId, baseEqId};
            const auto it = mTexMap.find(key);
            if(it == mTexMap.end()) {
                auto& partMap = mTexMap[key];
                partMap.resize(info.mDirs, nullptr);
                if(eqId != 255) {
                    const auto& part = info.mParts.get(partId);

                    auto eqName = part.fEq.name(eqId);
                    if(basePartId >= 0) {
                        const auto& basePart = info.mParts.get(basePartId);
                        const auto baseEqName = basePart.fEq.name(baseEqId);
                        eqName += "_" + baseEqName;
                    }

                    const auto partPath = animPath + partName + "_" + eqName;
                    eSpriteLoader loader(dir, partPath, res, r, mColorKey);
                    for(int i = 0; i < info.mDirs; i++) {
                        const auto coll = std::make_shared<eTextureCollection>();
                        for(int f = 0; f < nFrames; f++) {
                            loader.load(i*nFrames + f, *coll);
                        }
                        partMap[i] = coll;
                    }
                }
                rpart = partMap;
            } else {
                rpart = it->second;
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

const std::vector<int>&
eCharTextures::partsOrder(const int dir) const {
    return mDirPartsOrder[dir];
}

void eCharTextures::setCharDataId(const int id) {
    mCharDataId = id;
}

struct ePartToOrder {
    int fId;
    ePartPlace fPlace;
};

void eCharTextures::load(const ordered_json& jdata) {
    const auto colorKey = jdata.value("colorKey", std::vector<Uint8>{0, 0, 0, 0});
    if(colorKey.size() == 3) {
        mColorKey = SDL_Color{colorKey[0], colorKey[1], colorKey[2], 255};
    }

    const auto& info = eCharDataInfo::get(mCharDataId);

    std::vector<ePartToOrder> parts;
    parts.reserve(info.mNParts);
    for(const auto& p : info.mParts) {
        auto& po = parts.emplace_back();
        po.fId = p.fId;
        po.fPlace = p.fValue.fPlace;
    }

    mDirPartsOrder.reserve(info.mDirs);
    for(int d = 0; d < info.mDirs; d++) {
        const int angle = (d*360 + info.mDirs/2)/info.mDirs;

        int fw;
        int bw;
        if(angle == 90 || angle == 270) {
            fw = 1;
            bw = 1;
        } else if(angle < 90 || angle > 270) {
            fw = 2;
            bw = 1;
        } else {
            fw = 1;
            bw = 2;
        }

        int rw;
        int lw;
        if(angle == 0) {
            rw = 3;
            lw = 3;
        } else if(angle == 180) {
            rw = 0;
            lw = 0;
        } else if(angle < 180) {
            rw = 0;
            lw = 3;
        } else {
            rw = 3;
            lw = 0;
        }

        const auto wGetter = [&](const ePartToOrder& p) {
            switch(p.fPlace) {
            case ePartPlace::regular:
                return 0;
            case ePartPlace::front:
                return fw;
            case ePartPlace::back:
                return bw;
            case ePartPlace::right:
                return rw;
            case ePartPlace::left:
                return lw;
            }
            return 0;
        };

        const auto sorter = [&](const ePartToOrder& p1,
                                const ePartToOrder& p2) {
            const auto w1 = wGetter(p1);
            const auto w2 = wGetter(p2);
            if(w1 == w2) return p1.fId < p2.fId;
            return w1 < w2;
        };
        std::sort(parts.begin(), parts.end(), sorter);

        auto& iparts = mDirPartsOrder.emplace_back();
        iparts.reserve(info.mNParts);
        for(const auto& p : parts) {
            iparts.emplace_back(p.fId);
        }
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
        for(const auto& ipit : info.mParts) {
            const int partId = ipit.fId;
            const auto& partOptions = info.mParts.get(partId);
            const auto partName = info.mParts.name(partId);

            int basePartId = -1;
            const auto& partsBase = eItemPartsMap::sPartsBase;
            const auto pit = partsBase.find(partName);
            if(pit != partsBase.end()) {
                basePartId = info.mParts.id(pit->second);
            }

            const int nBaseOptions = basePartId >= 0 ?
                info.mParts.get(basePartId).fEq.size() : 1;

            for(int eqId = 0; eqId < partOptions.fEq.size(); ++eqId) {
                for(int baseEqId = 0; baseEqId < nBaseOptions; baseEqId++) {
                    const auto baseEqIdV = basePartId >= 0 ? baseEqId : 255;
                    const eCharTextureKey key{anim.fId, partId, eqId, baseEqIdV};
                    const auto it = mTexMap.find(key);
                    if(it == mTexMap.end()) {
                        auto& partMap = mTexMap[key];
                        partMap.reserve(info.mDirs);

                        auto eqName = partOptions.fEq.name(eqId);
                        if(basePartId >= 0) {
                            const auto& basePartOptions = info.mParts.get(basePartId);
                            eqName += "_" + basePartOptions.fEq.name(baseEqId);
                        }

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
