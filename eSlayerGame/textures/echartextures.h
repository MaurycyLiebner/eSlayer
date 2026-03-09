#ifndef ECHARTEXTURES_H
#define ECHARTEXTURES_H

#include "echarmodel.h"

#include <eSlayerHelpers/echardata.h>

class eCharTextures : public eCharData {
public:
    eCharTextures();

    void load(ordered_json& jdata) override;

    using eModelParts = std::map<std::string, std::string>;
    std::shared_ptr<eCharModel> generateModel(
        const eModelParts& modelParts,
        SDL_Renderer* const r);
private:
    SDL_Color mColorKey;

    std::map<std::string, std::map<int, std::shared_ptr<eTextureCollection>>> mTexMap;
};

#endif // ECHARTEXTURES_H
