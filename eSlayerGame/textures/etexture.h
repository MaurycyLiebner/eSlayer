#ifndef ETEXTURE_H
#define ETEXTURE_H

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <memory>

#include "erendertargetholder.h"

class eTexture;

struct eSprite {
    SDL_Texture* fTex = nullptr;
    float fTexCoordLeft;
    float fTexCoordTop;
    float fTexCoordRight;
    float fTexCoordBottom;

    void mapCoords(float& x, float& y) const {
        x = fTexCoordLeft + (fTexCoordRight - fTexCoordLeft)*x;
        y = fTexCoordTop + (fTexCoordBottom - fTexCoordTop)*y;
    }

    void mapCoords(SDL_FPoint& p) const {
        mapCoords(p.x, p.y);
    }
};

class eTexture {
public:
    eTexture();
    ~eTexture();

    void reset();
    std::shared_ptr<eTexture> scaled(
        SDL_Renderer* const r,
        const int width,
        const int height);
    bool create(SDL_Renderer* const r,
                const int width, const int height,
                const SDL_Color& col = {0, 0, 0, 0});
    void fill(SDL_Renderer * const r,
              const SDL_Color& col);

    eRenderTargetHolder createTargetHolder(SDL_Renderer* const r);

    bool save(SDL_Renderer* const r,
              const std::string& path);
    bool load(SDL_Renderer* const r,
              const std::string& path,
              const SDL_Color& colorKey = {0, 0, 0, 0});
    bool load(SDL_Renderer* const r,
              SDL_Surface* const surf,
              const SDL_Color& colorKey = {0, 0, 0, 0});

    void render(SDL_Renderer* const r,
                const SDL_Rect &srcRect,
                const SDL_Rect &dstRect,
                const bool flipped = false) const;
    void render(SDL_Renderer* const r,
                const SDL_FRect &srcRect,
                const SDL_FRect &dstRect,
                const bool flipped = false) const;
    void render(SDL_Renderer* const r,
                const float x, const float y,
                const bool flipped = false) const;

    int x() const { return mX; }
    int y() const { return mY; }

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    void setOffset(const int x, const int y);

    int offsetX() const { return mOffsetX; }
    int offsetY() const { return mOffsetY; }

    bool isNull() const;

    void setAlpha(const Uint8 alpha);
    void setAlphaF(const float alpha);
    void clearAlphaMod();
    void setColorMod(const Uint8 r, const Uint8 g, const Uint8 b);
    void setColorModF(const float r, const float g, const float b);
    void clearColorMod();
    float alphaModF() const;
    void colorModF(float& r, float& g, float& b) const;

    void setBlendMode(const SDL_BlendMode mode);

    void setFlipTex(const std::shared_ptr<eTexture>& tex);
    void setAtlas(const SDL_Rect& rect,
                  const std::shared_ptr<eTexture>& tex);
    const std::shared_ptr<eTexture>& atlas() const { return mAtlas; }

    eSprite sprite() const;

    SDL_Texture* tex() const { return mTex; }
private:
    SDL_Texture* mTex = nullptr;
    std::shared_ptr<eTexture> mAtlas;
    std::shared_ptr<eTexture> mFlipTex;
    int mX = 0;
    int mY = 0;
    int mWidth = 0;
    int mHeight = 0;
    int mOffsetX = 0;
    int mOffsetY = 0;
};

#endif // ETEXTURE_H
