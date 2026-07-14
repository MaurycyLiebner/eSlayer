#include "eeffect.h"

void eEffects::initialize(
    SDL_Renderer* const r,
    const int w, const int h) {
    mWidth = w;
    mHeight = h;
    mTmp = std::make_shared<eTexture>();
    mTmp->create(r, w, h, {0, 0, 0, 255});
}

void eEffects::apply(SDL_Renderer* const r,
                     std::shared_ptr<eTexture>& to) {
    for(const auto& e : mEffects) {
        e->apply(r, to, mTmp);
    }
}

void eEffects::addEffect(const std::shared_ptr<eEffect>& e) {
    mEffects.emplace_back(e);
    e->initialize(mWidth, mHeight);
}

void eEffects::clearEffects() {
    mEffects.clear();
}
