#ifndef ECHARTEXTURES_H
#define ECHARTEXTURES_H

#include "echarmodel.h"
#include "echarmodelloader.h"
#include "../ethreadpool.h"

#include <eSlayerHelpers/echardata.h>

class eResolution;

struct eCharTextureKey {
    int fAnimId;
    int fPartId;
    int fEqId;
    int fBaseId;

    bool operator<(const eCharTextureKey& other) const {
        return std::tie(fAnimId, fPartId, fEqId, fBaseId) <
               std::tie(other.fAnimId, other.fPartId, other.fEqId, other.fBaseId);
    }
};

class eCharTextures {
    struct eMaps {
        using eTexMap = std::map<eCharTextureKey, std::vector<std::shared_ptr<eTextureCollection>>>;
        eTexMap fTexMap;
        using eModelMap = std::map<std::vector<uint8_t>, std::shared_ptr<eCharModel>>;
        eModelMap fReadyModelMap;

        using eTexLoaderMap = std::map<eCharTextureKey, std::shared_ptr<eSpriteLoaderLoader>>;
        eTexLoaderMap fTexLoaderMap;
        using eModelLoaderMap = std::map<std::vector<uint8_t>, std::shared_ptr<eCharModelLoader>>;
        eModelLoaderMap fModelLoaderMap;

        void clear() {
            fTexMap.clear();
            fReadyModelMap.clear();
            fTexLoaderMap.clear();
            fModelLoaderMap.clear();
        }
    };
public:
    void setCharDataId(const int id);
    void load(const ordered_json& jdata);

    void clear(const bool forButton);

    void loadAll(const eResolution& res,
                 SDL_Renderer* const r);
    std::shared_ptr<eCharModel> requestModel(
        const eModelParts& modelParts,
        const eResolution& res,
        SDL_Renderer* const r,
        const eFinished& finished,
        const bool forButton = false) const;
    using eStringMap = std::map<std::string, std::string>;
    eModelParts mapToModelParts(const eStringMap& m) const;

    int animClamp(const int a) const;

    eCharData& charData() const;

    const std::vector<int>& partsOrder(const int dir) const;

    int baseSoundId() const { return mBaseSoundId; }
    int hitSoundId() const { return mHitSoundId; }
    int blockSoundId() const { return mBlockSoundId; }
    int evadeSoundId() const { return mEvadeSoundId; }
    int dieSoundId() const { return mDieSoundId; }
    int attackSoundId() const { return mAttackSoundId; }
    int missSoundId() const { return mMissSoundId; }

    static void handleLoaded();
    static void waitUntilAllLoaded();
private:
    static eThreadPool sTexturesThread;

    std::shared_ptr<eCharModel> requestModel(
        const eModelParts& modelParts,
        const eResolution& res,
        SDL_Renderer* const r,
        const eFinished& finished,
        eMaps& maps,
        const bool forButton = false) const;
    std::shared_ptr<eCharModel> generateModel(
        const eModelParts& modelParts,
        const eResolution& res,
        SDL_Renderer* const r,
        const eFinished& finished,
        eMaps& maps,
        const bool forButton) const;

    std::vector<std::vector<int>> mDirPartsOrder;

    int mCharDataId;
    SDL_Color mColorKey;

    int mBaseSoundId = -1;
    int mHitSoundId = -1;
    int mBlockSoundId = -1;
    int mEvadeSoundId = -1;
    int mDieSoundId = -1;
    int mAttackSoundId = -1;
    int mMissSoundId = -1;

    mutable eMaps mMaps;
    mutable eMaps mButtonMaps;
};

#endif // ECHARTEXTURES_H
