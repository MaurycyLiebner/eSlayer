#include "euitextures.h"

#include "../efileloader.h"

bool eUITextures::sLoaded = false;
std::map<std::string, std::shared_ptr<eTexture>>
eUITextures::sSkillIcons;
std::shared_ptr<eTexture> eUITextures::sWalkIcon;
std::shared_ptr<eTexture> eUITextures::sRunIcon;

void eUITextures::sLoad(SDL_Renderer* const r) {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Textures";

    sWalkIcon = eFileLoader::readTexture(r, dir, "ui/walk.png");
    sRunIcon = eFileLoader::readTexture(r, dir, "ui/run.png");

    const auto path = "ui/skills/skills.json";
    const auto jdata = eFileLoader::parse(dir, path);
    const auto names = jdata.get<std::vector<std::string>>();

    for(const auto& name : names) {
        const auto tex = eFileLoader::readTexture(r, dir, "ui/skills/" + name + ".png");
        sSkillIcons[name] = tex;
    }
}
