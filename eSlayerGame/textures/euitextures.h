#ifndef EUITEXTURES_H
#define EUITEXTURES_H

#include "../eresolution.h"
#include "etexturecollection.h"

#include <eSlayerHelpers/estringidmapvector.h>

#include <memory>

class SDL_Renderer;
class eTexture;

class eUITextures {
public:
    static void sLoad(SDL_Renderer* const r,
                      const eResolution& res);

    static eStringIdMapVector<std::shared_ptr<eTexture>>
    sSkillIcons;
    static std::shared_ptr<eTexture>
    sWalkIcon;
    static std::shared_ptr<eTexture>
    sRunIcon;
    static std::shared_ptr<eTexture>
    sLifeBar1;
    static std::shared_ptr<eTexture>
    sLifeBar2;
    static std::shared_ptr<eTexture>
    sStaminaBar1;
    static std::shared_ptr<eTexture>
    sStaminaBar2;
    static std::shared_ptr<eTexture>
    sExpBar1;
    static std::shared_ptr<eTexture>
    sExpBar2;
    static std::shared_ptr<eTexture>
    sBottomBar;

    static std::shared_ptr<eTexture>
    sAmuletSlot;
    static std::shared_ptr<eTexture>
    sArmorSlot;
    static std::shared_ptr<eTexture>
    sBeltSlot;
    static std::shared_ptr<eTexture>
    sBootsSlot;
    static std::shared_ptr<eTexture>
    sGlovesSlot;
    static std::shared_ptr<eTexture>
    sHelmetSlot;
    static std::shared_ptr<eTexture>
    sRingSlot;
    static std::shared_ptr<eTexture>
    sWeaponSlot;
    static std::shared_ptr<eTexture>
    sEmptySlot;

    static eTextureCollection sBg;
private:
    static std::string sLoaded;
};


#endif // EUITEXTURES_H
