#include "eiteminstancetexture.h"

#include "eitemstextures.h"

#include <eSlayerHelpers/eitemaffixes.h>

eItemInstanceTexture::eItemInstanceTexture() {}

eItemInstanceTexture::eItemInstanceTexture(
    SDL_Renderer* const r,
    const eResolution& res,
    const eItem& item) {
    const auto p = item.fPrefix;
    const auto s = item.fSuffix;
    if(p || s) {
        const auto color = eItemInstanceTexture::color(item);
        mColor.set(color);
    }
    auto& itemTex = eItemsTextures::getByItemDataId(item.fDataId);
    itemTex.request(r, res);
    mTex = itemTex.fTex;

    for(const auto& j : item.fJewels) {
        mJewels.emplace_back(r, res, j);
    }

    mNSockets = item.fSockets;
}

eTextureColorHolder eItemInstanceTexture::request() const {
    return eTextureColorHolder(mColor, mTex);
}

eTextureColorHolder
eItemInstanceTexture::requestJewel(
    const uint8_t id) const {
    return mJewels[id].request();
}

void eItemInstanceTexture::reset() {
    mTex.reset();
}

ePointF eItemInstanceTexture::jewelPosition(
    const uint8_t id, const uint8_t nSockets) {
    if(nSockets <= 1) {
        return ePointF{0.5f, 0.5f};
    } else if(nSockets == 2) {
        return ePointF{0.5f, 0.33f*(1 + id)};
    } else if(nSockets == 3) {
        if(id == 0) return ePointF{28.0f/56.0f, 16.0f/56.0f};
        else if(id == 1) return ePointF{18.0f/56.0f, 36.0f/56.0f};
        else return ePointF{38.0f/56.0f, 36.0f/56.0f};
    } else if(nSockets == 4) {
        const uint8_t row = id/2;
        const uint8_t col = id - row*2;
        return ePointF{0.33f*(1 + row), 0.33f*(1 + col)};
    } else { // if(nSockets >= 5) {
        const uint8_t row = id/2;
        const uint8_t col = id - row*2;
        return ePointF{0.25f*(1 + row), 0.33f*(1 + col)};
    }
}

SDL_FColor eItemInstanceTexture::color(const eItemBase& item) {
    const auto p = item.fPrefix;
    const auto s = item.fSuffix;

    float r = 1.f;
    float g = 1.f;
    float b = 1.f;
    float a = 1.f;

    if(p) {
        const auto& prefix = eItemAffixes::sPrefixes.get(p);
        const auto& color = prefix.fColor;
        r = color.fR;
        g = color.fG;
        b = color.fB;
        a = color.fA;
    }

    if(s && r == 1.f && g == 1.f && b == 1.f && a == 1.f) {
        const auto& suffix = eItemAffixes::sSuffixes.get(s);
        const auto& color = suffix.fColor;
        r = color.fR;
        g = color.fG;
        b = color.fB;
        a = color.fA;
    }

    return SDL_FColor{r, g, b, a};
}
