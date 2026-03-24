#ifndef ECHARTEXTURES_H
#define ECHARTEXTURES_H

#include "echarmodel.h"

#include <eSlayerHelpers/echardata.h>

class eCharTextures {
public:
    void setCharDataId(const int id);
    void load(ordered_json& jdata);

    std::shared_ptr<eCharModel> generateModel(
        const eModelParts& modelParts,
        SDL_Renderer* const r) const;

    eModelParts mapToModelParts(const std::map<std::string, std::string>& m) const;

    int animClamp(const int a) const;

    eCharData& charData() const;
private:
    int mCharDataId;
    SDL_Color mColorKey;
    mutable std::map<std::string, std::vector<std::shared_ptr<eTextureCollection>>> mTexMap;
};

#endif // ECHARTEXTURES_H
