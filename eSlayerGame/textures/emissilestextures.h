#ifndef EMISSILESTEXTURES_H
#define EMISSILESTEXTURES_H

#include "etexturecollection.h"

#include <eSlayerHelpers/estringidmapvector.h>

struct eMissileAnim {
    const std::shared_ptr<eTexture>&
    get(const int dir, const int frame);

    void load(SDL_Renderer* const r);

    int fNDirs = 0;
    int fNFrames = 0;
    std::vector<eTextureCollection> fDirs;
    std::string fPath;
};

class eMissileTextures {
    friend class eMissilesTextures;
public:
    const std::shared_ptr<eTexture>&
    get(const int animId, const int dir,
        const int frame);

    void load(SDL_Renderer* const r);

    int nFrames(const int animId) const;
    int nDirs(const int animId) const;

    float lighting() const { return mLighting; }
    void setLighting(const float l) { mLighting = l; }
private:
    float mLighting = 0.f;
    eStringIdMapVector<eMissileAnim> mAnims;
};

class eMissilesTextures {
public:
    static void loadData();
    static void loadTextures(SDL_Renderer* const r);

    static eStringIdMapVector<eMissileTextures>
    sMissiles;
private:
    static bool sLoaded;
};

#endif // EMISSILESTEXTURES_H
