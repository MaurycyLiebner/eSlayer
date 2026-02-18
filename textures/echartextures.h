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

    void load(json& jdata);

    void setName(const std::string& name) { mName = name; }
private:
    std::string mName;
    int mDirs;
    std::map<std::string, int> mAnimFrames;
    std::vector<std::map<std::string, std::vector<std::string>>> mGroups;
    std::map<std::string, std::map<int, std::shared_ptr<eTextureCollection>>> mTexMap;
};

#endif // ECHARTEXTURES_H
