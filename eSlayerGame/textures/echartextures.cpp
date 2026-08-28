#include "echartextures.h"

#include "espriteloader.h"
#include "../eresolution.h"
#include "../audio/esounds.h"

#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eitempartsmap.h>

eThreadPool eCharTextures::sTexturesThread(1);

std::shared_ptr<eCharModel>
eCharTextures::requestModel(
    const eModelParts& modelParts,
    const eResolution& res,
    SDL_Renderer* const r,
    const eFinished& finished,
    const bool forButton) const {
    auto& maps = forButton ? mButtonMaps : mMaps;
    return requestModel(modelParts, res, r, finished, maps, forButton);
}

std::shared_ptr<eCharModel>
eCharTextures::requestModel(
    const eModelParts& modelParts,
    const eResolution& res,
    SDL_Renderer* const r,
    const eFinished& finished,
    eMaps& maps,
    const bool forButton) const {
    const auto it = maps.fReadyModelMap.find(modelParts.fValues);
    if(it == maps.fReadyModelMap.end()) {
        const auto lit = maps.fModelLoaderMap.find(modelParts.fValues);
        if(lit != maps.fModelLoaderMap.end()) {
            const auto& loader = lit->second;
            loader->addFinish(finished);
            return loader->model();
        }

        const auto model = generateModel(
            modelParts, res, r, finished, maps, forButton);
        return model;
    } else {
        if(finished) finished(it->second);
        return it->second;
    }
}

std::shared_ptr<eCharModel>
eCharTextures::generateModel(
    const eModelParts& modelParts,
    const eResolution& res,
    SDL_Renderer* const r,
    const eFinished& finished,
    eMaps& maps,
    const bool forButton) const {
    const auto result = std::make_shared<eCharModel>(*this);
    const auto modelLoader = std::make_shared<eCharModelLoader>(result);

    const auto& info = eCharDataInfo::get(mCharDataId);
    const auto dir = "Textures";
    const auto path = "units/" + info.mName + "/";
    result->mNAnims = info.mAnims.size();
    result->mNParts = info.mNParts;
    result->mNDirs = info.mDirs;
    result->mAnims.reserve(info.mAnims.size());
    bool loadAnim = true;
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
            auto& rpart = rparts.emplace_back();

            if(!loadAnim) continue;

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

            const int partId = ipit.fId;
            const uint8_t eqId = modelParts.fValues[partId];
            const eCharTextureKey key{animId, partId, eqId, baseEqId};
            const auto it = maps.fTexMap.find(key);
            if(it == maps.fTexMap.end()) {
                if(eqId != 255) {
                    const auto lit = maps.fTexLoaderMap.find(key);
                    if(lit != maps.fTexLoaderMap.end()) {
                        const auto& loader = lit->second;
                        loader->addFinish([&rpart, key, &maps]() {
                            const auto it = maps.fTexMap.find(key);
                            if(it != maps.fTexMap.end()) {
                                rpart = it->second;
                            }
                        });
                        modelLoader->addLoader(loader);
                    } else {
                        const auto loader = std::make_shared<eSpriteLoaderLoader>();
                        maps.fTexLoaderMap[key] = loader;

                        const auto& part = info.mParts.get(partId);

                        auto eqName = part.fEq.name(eqId);
                        if(basePartId >= 0) {
                            const auto& basePart = info.mParts.get(basePartId);
                            const auto baseEqName = basePart.fEq.name(baseEqId);
                            eqName += "_" + baseEqName;
                        }

                        const auto partPath = animPath + partName + "_" + eqName;

                        const int maxRows = forButton ? nFrames : 0;
                        const auto sloader = std::make_shared<eSpriteLoader>(
                            dir, partPath, res, r, mColorKey, maxRows);
                        loader->set(sloader);

                        loader->addFinish([this, result, key, &info,
                                           sloader, nFrames, &rpart,
                                           &maps, forButton]() {
                            auto& partMap = maps.fTexMap[key];
                            partMap.resize(info.mDirs, nullptr);

                            for(int i = 0; i < info.mDirs; i++) {
                                const auto coll = std::make_shared<eTextureCollection>();
                                for(int f = 0; f < nFrames; f++) {
                                    sloader->load(i*nFrames + f, *coll);
                                }
                                partMap[i] = coll;
                                if(forButton) break;
                            }

                            rpart = partMap;

                            maps.fTexLoaderMap.erase(key);
                        });

                        modelLoader->addLoader(loader);

                        const auto work = [loader]() {
                            loader->load();
                        };
                        const auto finish = [loader](const std::exception_ptr& e) {
                            if(e) {
                                try {
                                    std::rethrow_exception(e);
                                } catch(const std::exception& e) {
                                    eRuntimeThrow("Error loading image " + e.what());
                                }
                            }
                            loader->finish();
                        };
                        sTexturesThread.submit(work, finish);
                    }
                }
            } else {
                rpart = it->second;
            }
        }

        if(forButton) loadAnim = false;
    }

    const bool e = modelLoader->empty();
    if(e) {
        if(finished) finished(result);
    } else {
        if(finished) modelLoader->addFinish(finished);
        modelLoader->setFinishDelete([this, modelParts, &maps](
                const std::shared_ptr<eCharModel>& model) {
            maps.fReadyModelMap[modelParts.fValues] = model;
            maps.fModelLoaderMap.erase(modelParts.fValues);
        });
        maps.fModelLoaderMap[modelParts.fValues] = modelLoader;
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

void eCharTextures::handleLoaded() {
    sTexturesThread.update();
}

void eCharTextures::waitUntilAllLoaded() {
    sTexturesThread.wait();
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

    const auto parseSound = [&](const std::string& name) {
        const auto soundStr = jdata.value(name, "");
        if(soundStr.empty()) return -1;
        const int id = eSounds::sSounds.id(soundStr);
        if(id < 0) {
            eRuntimeThrow("Unrecognized sound \"" + soundStr + "\".");
        }
        return id;
    };

    mBaseSoundId = parseSound("baseSound");
    mHitSoundId = parseSound("hitSound");
    mBlockSoundId = parseSound("blockSound");
    mEvadeSoundId = parseSound("evadeSound");
    mDieSoundId = parseSound("dieSound");
    mAttackSoundId = parseSound("attackSound");
    mMissSoundId = parseSound("missSound");

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

void eCharTextures::clear(const bool forButton) {
    auto& maps = forButton ? mButtonMaps : mMaps;
    maps.clear();
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
                    const auto it = mMaps.fTexMap.find(key);
                    if(it == mMaps.fTexMap.end()) {
                        auto& partMap = mMaps.fTexMap[key];
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
