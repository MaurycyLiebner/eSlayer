#ifndef EUITEXTURES_H
#define EUITEXTURES_H

#include "../eresolution.h"

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
private:
    static bool sLoaded;
};


#endif // EUITEXTURES_H
