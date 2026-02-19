#include "erendertargetholder.h"

eRenderTargetHolder::eRenderTargetHolder(
    SDL_Renderer* const r,
    SDL_Texture* const target) :
    mR(r) {
    mTmp = SDL_GetRenderTarget(r);
    SDL_SetRenderTarget(mR, target);
}

eRenderTargetHolder::~eRenderTargetHolder() {
    SDL_SetRenderTarget(mR, mTmp);
}
