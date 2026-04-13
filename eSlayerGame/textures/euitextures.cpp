#include "euitextures.h"

#include "../efileloader.h"
#include "etexture.h"
#include "espriteloader.h"

#include <eSlayerHelpers/eskills.h>

bool eUITextures::sLoaded = false;
eStringIdMapVector<std::shared_ptr<eTexture>>
eUITextures::sSkillIcons;
std::shared_ptr<eTexture> eUITextures::sWalkIcon;
std::shared_ptr<eTexture> eUITextures::sRunIcon;

std::shared_ptr<eTexture> eUITextures::sLifeBar1;
std::shared_ptr<eTexture> eUITextures::sLifeBar2;

std::shared_ptr<eTexture> eUITextures::sStaminaBar1;
std::shared_ptr<eTexture> eUITextures::sStaminaBar2;

void eUITextures::sLoad(SDL_Renderer* const r,
                        const eResolution& res) {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Textures";

    {
        const auto suffix = res.textureSuffix();
        {
            const auto path = "ui/lifeBar/lifeBar" + suffix;
            eSpriteLoader loader(dir, path, r);
            sLifeBar1 = loader.load(0);
            sLifeBar2 = loader.load(1);
        }
        {
            const auto path = "ui/staminaBar/staminaBar" + suffix;
            eSpriteLoader loader(dir, path, r);
            sStaminaBar1 = loader.load(0);
            sStaminaBar2 = loader.load(1);
        }
    }

    sWalkIcon = eFileLoader::readTexture(r, dir, "ui/walk.png");
    sRunIcon = eFileLoader::readTexture(r, dir, "ui/run.png");

    const auto path = "ui/skills/skills.json";
    const auto jdata = eFileLoader::parse(dir, path);
    const auto names = jdata.get<std::vector<std::string>>();

    for(const auto& name : names) {
        const auto tex = eFileLoader::readTexture(r, dir, "ui/skills/" + name + ".png");
        sSkillIcons.add(name, tex->scaled(r, 100, 100));
    }

    for(const auto& it : eSkills::sSkills) {
        auto& skill = it.fValue;
        skill.fIconId = sSkillIcons.id(skill.fIcon);
    }
}
