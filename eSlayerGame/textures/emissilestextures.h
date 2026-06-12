#ifndef EMISSILESTEXTURES_H
#define EMISSILESTEXTURES_H

#include "etexturecollection.h"

#include <eSlayerHelpers/emissilesinfo.h>

class eResolution;

class eMissileAnimTextures {
public:
    const std::shared_ptr<eTexture>&
    get(const int dir, const int frame);

    void load(const eResolution& res,
              SDL_Renderer* const r,
              const eMissileAnim& info);
private:
    std::vector<eTextureCollection> mDirs;
};

class eMissileTextures {
public:
    const std::shared_ptr<eTexture>&
    get(const int animId, const int dir,
        const int frame);

    void load(const eResolution& res,
              SDL_Renderer* const r,
              const eMissileInfo& info);
private:
    eStringIdMapVector<eMissileAnimTextures> mAnims;
};

class eMissilesTextures {
public:
    static void load(const eResolution& res,
                     SDL_Renderer* const r);

    static eStringIdMapVector<eMissileTextures>
    sMissiles;
private:
    static bool sLoaded;
};

#endif // EMISSILESTEXTURES_H
