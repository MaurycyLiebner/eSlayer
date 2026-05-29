#ifndef ETEXTURECOLORHOLDER_H
#define ETEXTURECOLORHOLDER_H

#include "etexture.h"

#include <eSlayerHelpers/ecolor.h>

class eTextureColorSetting {
    friend class eTextureColorHolder;
public:
    void set(const SDL_Color& col);
    void set(const SDL_FColor& col);
    void set(const eColor& col);
    void set(const float r, const float g,
             const float b, const float a = 1.f);
private:
    bool mMod = false;
    float mR;
    float mG;
    float mB;
    float mA;
};

class eTextureColorHolder {
public:
    eTextureColorHolder(const eTextureColorSetting& s,
                        const std::shared_ptr<eTexture>& tex);
    eTextureColorHolder(const bool mod,
                        const float r,
                        const float g,
                        const float b,
                        const float a,
                        const std::shared_ptr<eTexture>& tex);
    ~eTextureColorHolder();

    eTextureColorHolder(const eTextureColorHolder&) = delete;
    eTextureColorHolder& operator=(const eTextureColorHolder&) = delete;

    const bool fMod;

    const std::shared_ptr<eTexture> fTex;
    const float fR;
    const float fG;
    const float fB;
    const float fA;
private:
    float mRTmp;
    float mGTmp;
    float mBTmp;
    float mATmp;
};

#endif // ETEXTURECOLORHOLDER_H
