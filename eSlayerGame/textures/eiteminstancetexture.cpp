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
}

eTextureColorHolder eItemInstanceTexture::request() const {
    return eTextureColorHolder(mColor, mTex);
}

void eItemInstanceTexture::reset() {
    mTex.reset();
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
