#ifndef ECHARTEXTURES_H
#define ECHARTEXTURES_H

#include "echarmodel.h"

#include <map>
#include <string>

#include <nlohmann/json.hpp>

using namespace nlohmann;

class eCharTextures {
public:
    eCharTextures();

    using eModelParts = std::map<std::string, std::string>;
    eCharModel generateModel(const eModelParts& modelParts,
                             SDL_Renderer* const r);

    void load(ordered_json& jdata);

    void setName(const std::string& name) { mName = name; }
private:
    std::string mName;
    int mDirs;
    SDL_Color mColorKey;

    struct eAnimation {
        int fFrames;
        eOffset fOffset;
    };

    std::vector<std::pair<std::string, eAnimation>> mAnims;
    std::vector<std::map<std::string, std::vector<std::string>>> mGroups;
    std::map<std::string, std::map<int, std::shared_ptr<eTextureCollection>>> mTexMap;
};

#endif // ECHARTEXTURES_H
