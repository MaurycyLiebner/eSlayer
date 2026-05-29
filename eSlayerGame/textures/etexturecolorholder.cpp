#include "etexturecolorholder.h"

void eTextureColorSetting::set(const SDL_Color& col) {
    set(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f);
}

void eTextureColorSetting::set(const SDL_FColor& col) {
    set(col.r, col.g, col.b, col.a);
}

void eTextureColorSetting::set(
    const eColor& col) {
    set(col.fR, col.fG, col.fB, col.fA);
}

void eTextureColorSetting::set(
    const float r, const float g,
    const float b, const float a) {
    mMod = true;
    mR = r;
    mG = g;
    mB = b;
    mA = a;
}

eTextureColorHolder::eTextureColorHolder(
    const eTextureColorSetting& s,
    const std::shared_ptr<eTexture>& tex) :
    eTextureColorHolder(s.mMod, s.mR, s.mG, s.mB, s.mA, tex) {}

eTextureColorHolder::eTextureColorHolder(
    const bool mod,
    const float r, const float g,
    const float b, const float a,
    const std::shared_ptr<eTexture>& tex) :
    fMod(mod),
    fR(r), fG(g), fB(b), fA(a),
    fTex(tex) {
    if(fMod && fTex) {
        fTex->colorModF(mRTmp, mGTmp, mBTmp);
        mATmp = fTex->alphaModF();

        fTex->setColorModF(fR, fG, fB);
        fTex->setAlphaF(fA);
    }
}

eTextureColorHolder::~eTextureColorHolder() {
    if(!fMod || !fTex) return;
    fTex->setColorModF(mRTmp, mGTmp, mBTmp);
    fTex->setAlphaF(mATmp);
}
