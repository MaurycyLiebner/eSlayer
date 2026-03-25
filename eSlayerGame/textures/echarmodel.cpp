#include "echarmodel.h"

#include "../widgets/epainter.h"

eCharModel::eCharModel(const eCharTextures& data)
    : mData(data) {}

const eTextureSptr& eCharModel::get(
    const int anim, const int group,
    const int part, const int dir,
    const int frame) const {
    return mAnims[anim].fGroups[group][part][dir]->getTexture(frame);
}

int eCharModel::nParts(const int group) const {
    return mNParts[group];
}

int eCharModel::nFrames(const int anim) const {
    return mAnims[anim].fFrames;
}

const eOffset& eCharModel::animOffset(const int anim) const {
    return mAnims[anim].fOffset;
}

std::shared_ptr<eTexture>
eCharModel::requestTexture(
    SDL_Renderer* const r,
    const eTextureKey& key) {
    const auto it = mTexCache.find(key);
    if(it != mTexCache.end()) {
        return it->second;
    }

    const auto texRect = requestBoundingRect(key);

    const auto tex = std::make_shared<eTexture>();
    {
        tex->create(r, texRect.w, texRect.h);
        ePainter sp(r);
        sp.translate(-texRect.x, -texRect.y);
        const auto holder = tex->createTargetHolder(r);
        for(int g = 0; g < mNGroups; g++) {
            const int ppMax = nParts(g);
            for(int pp = 0; pp < ppMax; pp++) {
                const auto tex = get(key.fAnim, g, pp, key.fDir, key.fFrame);
                sp.drawTexture(0, 0, tex);
            }
        }
    }

    mTexCache[key] = tex;
    return tex;
}

SDL_Rect eCharModel::requestBoundingRect(const eTextureKey& key) {
    const auto it = mRectCache.find(key);
    if(it != mRectCache.end()) return it->second;
    const auto texRect = boundingRect(key);
    mRectCache[key] = texRect;
    return texRect;
}

SDL_Rect eCharModel::boundingRect(const eTextureKey& key) const {
    SDL_Rect texRect{0, 0, 0, 0};

    for(int g = 0; g < mNGroups; g++) {
        const int ppMax = nParts(g);
        for(int pp = 0; pp < ppMax; pp++) {
            const auto& tex = get(key.fAnim, g, pp, key.fDir, key.fFrame);

            const int newX = std::min(texRect.x, tex->offsetX());
            texRect.w += texRect.x - newX;
            texRect.x = newX;
            texRect.w = std::max(texRect.w, -texRect.x + (tex->offsetX() + tex->width()));

            const int newY = std::min(texRect.y, tex->offsetY());
            texRect.h += texRect.y - newY;
            texRect.y = newY;
            texRect.h = std::max(texRect.h, -texRect.y + (tex->offsetY() + tex->height()));
        }
    }

    return texRect;
}
