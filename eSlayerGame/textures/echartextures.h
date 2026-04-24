#ifndef ECHARTEXTURES_H
#define ECHARTEXTURES_H

#include "echarmodel.h"

#include <eSlayerHelpers/echardata.h>

class eResolution;

struct eCharTextureKey {
    int fAnimId;
    int fPartId;
    int fEqId;

    bool operator<(const eCharTextureKey& other) const {
        return std::tie(fAnimId, fPartId, fEqId) <
               std::tie(other.fAnimId, other.fPartId, other.fEqId);
    }
};

class eCharTextures {
public:
    void setCharDataId(const int id);
    void load(const ordered_json& jdata);

    void loadAll(const eResolution& res,
                 SDL_Renderer* const r);
    std::shared_ptr<eCharModel> requestModel(
        const eModelParts& modelParts,
        const eResolution& res,
        SDL_Renderer* const r) const;
    using eStringMap = std::map<std::string, std::string>;
    eModelParts mapToModelParts(const eStringMap& m) const;

    int animClamp(const int a) const;

    eCharData& charData() const;
private:
    std::shared_ptr<eCharModel> generateModel(
        const eModelParts& modelParts,
        const eResolution& res,
        SDL_Renderer* const r) const;

    int mCharDataId;
    SDL_Color mColorKey;
    using eTexMap = std::map<eCharTextureKey, std::vector<std::shared_ptr<eTextureCollection>>>;
    mutable eTexMap mTexMap;
    using eModelMap = std::map<std::vector<uint8_t>, std::shared_ptr<eCharModel>>;
    mutable eModelMap mModelMap;
};

#endif // ECHARTEXTURES_H
