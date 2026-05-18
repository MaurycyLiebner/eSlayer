#include "etexture.h"

#include <eSlayerHelpers/eexceptions.h>

eTexture::eTexture() {}

eTexture::~eTexture() {
    reset();
}

void eTexture::reset() {
    if(mTex) SDL_DestroyTexture(mTex);
    mTex = nullptr;
    mWidth = 0;
    mHeight = 0;
}

std::shared_ptr<eTexture>
eTexture::scaled(SDL_Renderer* const r,
                 const int width, const int height) {
    const auto result = std::make_shared<eTexture>();
    result->create(r, width, height);
    const auto holder = result->createTargetHolder(r);
    render(r, SDL_Rect{0, 0, mWidth, mHeight},
           SDL_Rect{0, 0, width, height});
    return result;
}

bool eTexture::create(SDL_Renderer* const r,
                      const int width, const int height,
                      const SDL_Color& col) {
    reset();
    mTex = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888,
                             SDL_TEXTUREACCESS_TARGET, width, height);
    if(!mTex) return false;
    fill(r, col);
    SDL_SetTextureBlendMode(mTex, SDL_BLENDMODE_BLEND);
    mWidth = width;
    mHeight = height;
    return true;
}

void eTexture::fill(SDL_Renderer* const r,
                    const SDL_Color& col) {
    const auto holder = createTargetHolder(r);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(r, nullptr);
}

eRenderTargetHolder eTexture::createTargetHolder(SDL_Renderer * const r) {
    return eRenderTargetHolder(r, mTex);
}

bool eTexture::save(SDL_Renderer* const r,
                    const std::string& path) {
    if(!mTex) return false;
    const auto holder = createTargetHolder(r);
    SDL_SetRenderTarget(r, mTex);
    const SDL_Rect rect{0, 0, mTex->w, mTex->h};
    const auto surface = SDL_RenderReadPixels(r, &rect);
    IMG_SavePNG(surface, path.c_str());
    SDL_DestroySurface(surface);
    return true;
}

bool eTexture::load(SDL_Renderer* const r,
                    const std::string& path,
                    const SDL_Color& colorKey) {
    reset();
    const auto surf = IMG_Load(path.c_str());
    if(!surf) {
        eExceptions::logError(
            "Unable to load image \"" + path + "\"!",
            SDL_GetError());
        return false;
    }
    return load(r, surf, colorKey);
}

bool eTexture::load(SDL_Renderer* const r,
                    SDL_Surface* const surf,
                    const SDL_Color& colorKey) {
    reset();
    if(colorKey.a) {
        SDL_Color col;
        const auto details = SDL_GetPixelFormatDetails(surf->format);
        const auto palette = SDL_GetSurfacePalette(surf);
        const Uint32 key = SDL_MapRGB(details, palette, colorKey.r, colorKey.g, colorKey.b);
        SDL_SetSurfaceColorKey(surf, true, key);
    }
    mTex = SDL_CreateTextureFromSurface(r, surf);
    mWidth = surf->w;
    mHeight = surf->h;
    SDL_DestroySurface(surf);
    if(!mTex) {
        eExceptions::logError(
            "Unable to create texture from surface!",
            SDL_GetError());
        return false;
    }

    return true;
}

void eTexture::render(SDL_Renderer* const r,
                      const SDL_Rect& srcRect,
                      const SDL_Rect& dstRect,
                      const bool flipped) const {
    SDL_FRect fSrcRect;
    SDL_RectToFRect(&srcRect, &fSrcRect);
    SDL_FRect fDstRect;
    SDL_RectToFRect(&dstRect, &fDstRect);
    render(r, fSrcRect, fDstRect, flipped);
}

void eTexture::render(SDL_Renderer * const r,
                      const SDL_FRect& srcRect,
                      const SDL_FRect& dstRect,
                      const bool flipped) const {
    if(mFlipTex) {
        mFlipTex->render(r, srcRect, dstRect, true);
    } else if(mAtlas) {
        mAtlas->render(r, srcRect, dstRect, flipped);
    } else if(mTex) {
        if(flipped) {
            SDL_RenderTextureRotated(r, mTex, &srcRect, &dstRect, 0, nullptr,
                                     SDL_FlipMode::SDL_FLIP_HORIZONTAL);
        } else {
            SDL_RenderTexture(r, mTex, &srcRect, &dstRect);
        }
    }
}

void eTexture::render(SDL_Renderer* const r,
                      const float x, const float y,
                      const bool flipped) const {
    const float sx = mFlipTex ? mFlipTex->x() : mX;
    const float sy = mFlipTex ? mFlipTex->y() : mY;
    const float width = mFlipTex ? mFlipTex->width() : mWidth;
    const float height = mFlipTex ? mFlipTex->height() : mHeight;
    const SDL_FRect srcRect{sx, sy, width, height};
    const SDL_FRect dstRect{x + mOffsetX, y + mOffsetY, width, height};
    render(r, srcRect, dstRect, flipped);
}

void eTexture::setOffset(const int x, const int y) {
    mOffsetX = x;
    mOffsetY = y;
}

bool eTexture::isNull() const {
    if(mFlipTex) return mFlipTex->isNull();
    else if(mAtlas) mAtlas->isNull();
    return mWidth <= 0 || mHeight <= 0;
}

void eTexture::setAlpha(const Uint8 alpha) {
    if(mFlipTex) mFlipTex->setAlpha(alpha);
    else if(mAtlas) mAtlas->setAlpha(alpha);
    else SDL_SetTextureAlphaMod(mTex, alpha);
}

void eTexture::clearAlphaMod() {
    setAlpha(255);
}

void eTexture::setColorMod(const Uint8 r, const Uint8 g, const Uint8 b) {
    if(mFlipTex) mFlipTex->setColorMod(r, g, b);
    else if(mAtlas) mAtlas->setColorMod(r, g, b);
    else SDL_SetTextureColorMod(mTex, r, g, b);
}

void eTexture::clearColorMod() {
    setColorMod(255, 255, 255);
}

void eTexture::setBlendMode(const SDL_BlendMode mode) {
    if(mFlipTex) mFlipTex->setBlendMode(mode);
    else if(mAtlas) mAtlas->setBlendMode(mode);
    else SDL_SetTextureBlendMode(mTex, mode);
}

void eTexture::setFlipTex(const std::shared_ptr<eTexture>& tex) {
    mFlipTex = tex;
    mX = mFlipTex->x();
    mY = mFlipTex->y();
    mWidth = mFlipTex->width();
    mHeight = mFlipTex->height();
}

void eTexture::setAtlas(const SDL_Rect& rect,
                        const std::shared_ptr<eTexture>& tex) {
    mAtlas = tex;
    mX = rect.x;
    mY = rect.y;
    mWidth = rect.w;
    mHeight = rect.h;
}

eSprite eTexture::sprite() const {
    eSprite result;
    if(mTex) {
        result.fTex = this;
        result.fTexCoordLeft = 0.f;
        result.fTexCoordTop = 0.f;
        result.fTexCoordRight = 1.f;
        result.fTexCoordBottom = 1.f;
    } else if(mAtlas) {
        result.fTex = &*mAtlas;
        const float aw = mAtlas->width();
        const float ah = mAtlas->height();
        result.fTexCoordLeft = mX/aw;
        result.fTexCoordTop = mY/ah;
        result.fTexCoordRight = (mX + mWidth)/aw;
        result.fTexCoordBottom = (mY + mHeight)/ah;
    }
    return result;
}
