#include "etexture.h"

#include <algorithm>

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

bool eTexture::load(SDL_Renderer* const r,
                    const std::string& path,
                    const SDL_Color& colorKey) {
    reset();
    const auto surf = IMG_Load(path.c_str());
    if(!surf) {
        printf("Unable to load image %s! SDL_image Error: %s\n",
               path.c_str(), SDL_GetError());
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
        printf("Unable to create texture from surface!"
               "SDL Error: %s\n", SDL_GetError());
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
    } else if(mParentTex) {
        mParentTex->render(r, srcRect, dstRect, flipped);
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
    const SDL_FRect dstRect{x, y, width, height};
    render(r, srcRect, dstRect, flipped);
}

void eTexture::renderRelPortion(SDL_Renderer* const r,
                                const int dstX,
                                const int dstY,
                                const int srcX,
                                const int w,
                                const bool flipped) const {
    const int sx = mFlipTex ? mFlipTex->x() : mX;
    const int sy = mFlipTex ? mFlipTex->y() : mY;
    const int width = mFlipTex ? mFlipTex->width() : mWidth;
    const int height = mFlipTex ? mFlipTex->height() : mHeight;
    const int ww = std::min(w + srcX, width) - srcX;
    SDL_Rect srcRect{sx + srcX, sy, ww, height};
    SDL_Rect dstRect{dstX, dstY, ww, height};
    if(srcRect.x < sx) {
        const int dx = sx - srcRect.x;
        srcRect.x += dx;
        dstRect.x += dx;
        srcRect.w -= dx;
        dstRect.w -= dx;
    }
    if(srcRect.y < sy) {
        const int dy = sy - srcRect.y;
        srcRect.y += dy;
        dstRect.y += dy;
        srcRect.h -= dy;
        dstRect.h -= dy;
    }
    if(srcRect.x + srcRect.w > sx + width) {
        const int dw = sx + width - srcRect.x - srcRect.w;
        srcRect.w += dw;
        dstRect.w += dw;
    }
    if(srcRect.y + srcRect.h > sy + height) {
        const int dh = sy + height - srcRect.y - srcRect.h;
        srcRect.h += dh;
        dstRect.h += dh;
    }
    if(srcRect.w <= 0 || srcRect.h <= 0 ||
       dstRect.w <= 0 || dstRect.h <= 0) return;
    if(mFlipTex) {
        srcRect.x = mFlipTex->width() - srcRect.x - srcRect.w;
    }
    render(r, srcRect, dstRect, flipped);
}

void eTexture::setOffset(const int x, const int y) {
    mOffsetX = x;
    mOffsetY = y;
}

bool eTexture::isNull() const {
    if(mFlipTex) return mFlipTex->isNull();
    else if(mParentTex) mParentTex->isNull();
    return mWidth <= 0 || mHeight <= 0;
}

void eTexture::setAlpha(const Uint8 alpha) {
    if(mFlipTex) mFlipTex->setAlpha(alpha);
    else if(mParentTex) mParentTex->setAlpha(alpha);
    else SDL_SetTextureAlphaMod(mTex, alpha);
}

void eTexture::clearAlphaMod() {
    setAlpha(255);
}

void eTexture::setColorMod(const Uint8 r, const Uint8 g, const Uint8 b) {
    if(mFlipTex) mFlipTex->setColorMod(r, g, b);
    else if(mParentTex) mParentTex->setColorMod(r, g, b);
    else SDL_SetTextureColorMod(mTex, r, g, b);
}

void eTexture::clearColorMod() {
    setColorMod(255, 255, 255);
}

void eTexture::setBlendMode(const SDL_BlendMode mode) {
    if(mFlipTex) mFlipTex->setBlendMode(mode);
    else if(mParentTex) mParentTex->setBlendMode(mode);
    else SDL_SetTextureBlendMode(mTex, mode);
}

void eTexture::setFlipTex(const std::shared_ptr<eTexture>& tex) {
    mFlipTex = tex;
    mX = mFlipTex->x();
    mY = mFlipTex->y();
    mWidth = mFlipTex->width();
    mHeight = mFlipTex->height();
}

void eTexture::setParentTexture(const SDL_Rect& rect,
                                const std::shared_ptr<eTexture>& tex) {
    mParentTex = tex;
    mX = rect.x;
    mY = rect.y;
    mWidth = rect.w;
    mHeight = rect.h;
}
