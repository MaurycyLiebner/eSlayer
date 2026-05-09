#ifndef ENOVASTEXTURES_H
#define ENOVASTEXTURES_H

#include "etexturecollection.h"

#include <eSlayerHelpers/estringidmapvector.h>

class eResolution;

class eNovaTextures {
    friend class eNovasTextures;
public:
    const std::shared_ptr<eTexture>&
    get(const int frame);

    void load(const eResolution& res,
              SDL_Renderer* const r);

    int nFrames() const;

    float lighting() const { return mLighting; }
    void setLighting(const float l) { mLighting = l; }
private:
    float mLighting = 0.f;
    eTextureCollection mTexs;
    std::string mPath;
};

class eNovasTextures {
public:
    static void loadData();
    static void loadTextures(const eResolution& res,
                             SDL_Renderer* const r);

    static eStringIdMapVector<eNovaTextures>
    sNovas;
private:
    static bool sDataLoaded;
    static bool sTexsLoaded;
};

#endif // ENOVASTEXTURES_H
