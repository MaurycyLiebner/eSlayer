#ifndef ELIGHTINGTEXTURE_H
#define ELIGHTINGTEXTURE_H

#include "etexture.h"

class eLightingTexture : public eTexture {
public:
    using eTexture::eTexture;

    void initialize(SDL_Renderer* const r,
                    const int w, const int h,
                    const SDL_Color& color);
    void setClearColor(const SDL_Color& color);
    void clear(SDL_Renderer * const r);
    void renderLight(SDL_Renderer * const r,
                     const float x, const float y,
                     const float radius,
                     const SDL_Color& color);
private:
    SDL_Color mColor;
    std::shared_ptr<eTexture> mLightingTex;
};

#endif // ELIGHTINGTEXTURE_H
