#ifndef EATLAS_H
#define EATLAS_H

#include "etexture.h"

class eAtlas {
public:
    eAtlas();
    eAtlas(const SDL_Color& colorKey);
    ~eAtlas();

    void loadSurf(const std::string& dir,
                  const std::string& path);
    const std::shared_ptr<eTexture>&
    requestTex(SDL_Renderer * const r);
private:
    void generateTex(SDL_Renderer * const r);

    SDL_Color mColorKey;
    SDL_Surface* mSurf = nullptr;
    std::shared_ptr<eTexture> mTex;
};

#endif // EATLAS_H
