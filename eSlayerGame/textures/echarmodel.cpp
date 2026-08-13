#include "echarmodel.h"

#include "../widgets/epainter.h"
#include "echartextures.h"

eCharModel::eCharModel(const eCharTextures& data) :
    mData(data) {}

std::shared_ptr<eTexture> eCharModel::get(
    const int anim, const int part,
    const int dir, const int frame) const {
    const auto& coll = mAnims[anim].fParts[part][dir];
    if(!coll) return nullptr;
    return coll->getTexture(frame);
}

int eCharModel::nFrames(const int anim) const {
    return mAnims[anim].fFrames;
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
        const auto& parts = mData.partsOrder(key.fDir);
        for(const int pp : parts) {
            const auto tex = get(key.fAnim, pp, key.fDir, key.fFrame);
            if(!tex) continue;
            sp.drawTexture(0, 0, tex);
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

    for(int pp = 0; pp < mNParts; pp++) {
        const auto tex = get(key.fAnim, pp, key.fDir, key.fFrame);
        if(!tex) continue;
        const int newX = std::min(texRect.x, tex->offsetX());
        texRect.w += texRect.x - newX;
        texRect.x = newX;
        texRect.w = std::max(texRect.w, -texRect.x + (tex->offsetX() + tex->width()));

        const int newY = std::min(texRect.y, tex->offsetY());
        texRect.h += texRect.y - newY;
        texRect.y = newY;
        texRect.h = std::max(texRect.h, -texRect.y + (tex->offsetY() + tex->height()));
    }

    return texRect;
}
