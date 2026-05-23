#ifndef EMISSILESTEXTURES_H
#define EMISSILESTEXTURES_H

#include "etexturecollection.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eResolution;

struct eMissileAnim {
    const std::shared_ptr<eTexture>&
    get(const int dir, const int frame);

    void load(const eResolution& res,
              SDL_Renderer* const r);

    int fNDirs = 0;
    int fNFrames = 0;
    std::vector<eTextureCollection> fDirs;
    std::string fPath;
};

enum class eMissileType {
    regular, explosion
};

class eMissileTextures {
    friend class eMissilesTextures;
public:
    const std::shared_ptr<eTexture>&
    get(const int animId, const int dir,
        const int frame);

    void load(const eResolution& res,
              SDL_Renderer* const r);

    int nFrames(const int animId) const;
    int nDirs(const int animId) const;

    int animId(const std::string& name) const;

    int appearAnimId() const { return mAppearAnimId; }
    int baseAnimId() const { return mBaseAnimId; }
    int hitAnimId() const { return mHitAnimId; }

    float lighting() const { return mLighting; }

    eMissileType type() const { return mType; }
private:
    eMissileType mType = eMissileType::regular;

    int mAppearAnimId = -1;
    int mBaseAnimId = -1;
    int mHitAnimId = -1;

    float mLighting = 0.f;
    eStringIdMapVector<eMissileAnim> mAnims;
};

class eMissilesTextures {
public:
    static void loadData();
    static void loadTextures(const eResolution& res,
                             SDL_Renderer* const r);

    static eStringIdMapVector<eMissileTextures>
    sMissiles;

    static int sFleshId;
    static int sIceId;
private:
    static bool sDataLoaded;
    static bool sTexsLoaded;
};

#endif // EMISSILESTEXTURES_H
