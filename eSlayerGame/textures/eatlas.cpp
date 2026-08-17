#include "eatlas.h"

#include "../efileloader.h"

eAtlas::eAtlas() :
    eAtlas(SDL_Color{0, 0, 0, 0}) {}

eAtlas::eAtlas(const SDL_Color& colorKey) :
    mColorKey(colorKey) {}

eAtlas::~eAtlas() {
    if(mSurf) SDL_DestroySurface(mSurf);
}

void eAtlas::loadSurf(const std::string& dir,
                      const std::string& path) {
    mSurf = eFileLoader::loadSurface(dir, path);
}

const std::shared_ptr<eTexture>&
eAtlas::requestTex(SDL_Renderer * const r) {
    if(!mTex) generateTex(r);
    return mTex;
}

void eAtlas::generateTex(SDL_Renderer * const r) {
    if(!mSurf) return;
    mTex = std::make_shared<eTexture>();
    mTex->load(r, mSurf, mColorKey);
    mSurf = nullptr;
}
